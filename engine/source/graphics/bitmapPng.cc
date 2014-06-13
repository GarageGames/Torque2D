//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "io/stream.h"
#include "io/fileStream.h"
#include "io/memstream.h"
#include "graphics/gPalette.h"
#include "graphics/gBitmap.h"
#include "memory/frameAllocator.h"


//-Mat used when checking for palleted textures
#include "console/console.h"
bool sgForcePalletedPNGsTo16Bit= false;


#define PNG_INTERNAL 1

// Ignore PNG time chunks
#define PNG_NO_READ_TIME
#define PNG_NO_WRITE_TIME

#include <time.h>
#include "png.h"
#include "zlib.h"

// Our chunk signatures...

static const U32 csgMaxRowPointers = (1 << GBitmap::c_maxMipLevels) - 1; ///< 2^11 = 2048, 12 mip levels (see c_maxMipLievels)
static png_bytep sRowPointers[csgMaxRowPointers];

//-------------------------------------- Instead of using the user_ptr,
//                                        we use a global pointer, we
//                                        need to ensure that only one thread
//                                        at once may be using the variable.
//                                       NOTE: Removed mutex for g_varAccess.
//                                        may have to re-thread safe this.
static Stream* sg_pStream = NULL;

//-------------------------------------- Replacement I/O for standard LIBPng
//                                        functions.  we don't wanna use
//                                        FILE*'s...
static void pngReadDataFn(png_structp  /*png_ptr*/,
                          png_bytep   data,
                          png_size_t  length)
{
   AssertFatal(sg_pStream != NULL, "No stream?");

   bool success;
   success = sg_pStream->read((U32)length, data);
    
   AssertFatal(success, "PNG read catastrophic error!");
}


//--------------------------------------
static void pngWriteDataFn(png_structp /*png_ptr*/,
                           png_bytep   data,
                           png_size_t  length)
{
   AssertFatal(sg_pStream != NULL, "No stream?");

   sg_pStream->write((U32)length, data);
}


//--------------------------------------
static void pngFlushDataFn(png_structp /*png_ptr*/)
{
   //
}

static png_voidp pngMallocFn(png_structp /*png_ptr*/, png_size_t size)
{
#ifndef _WIN64
   return FrameAllocator::alloc((U32)size);
#else
   return (png_voidp)dMalloc(size);
#endif
}

static void pngFreeFn(png_structp /*png_ptr*/, png_voidp mem)
{
#ifdef _WIN64
   dFree(mem);
#endif
}


//--------------------------------------
static void pngFatalErrorFn(png_structp     /*png_ptr*/,
                            png_const_charp pMessage)
{
   AssertISV(false, avar("Error reading PNG file:\n %s", pMessage));
}


//--------------------------------------
static void pngWarningFn(png_structp, png_const_charp pMessage)
{
   AssertWarn(false, avar("Warning reading PNG file:\n %s", pMessage));
}


//--------------------------------------
bool GBitmap::readPNG(Stream& io_rStream)
{
   static const U32 cs_headerBytesChecked = 8;

   U8 header[cs_headerBytesChecked];
   io_rStream.read(cs_headerBytesChecked, header);

   bool isPng = (png_check_sig(header, cs_headerBytesChecked)) != 0;
   if (isPng == false) 
   {
      AssertWarn(false, "GBitmap::readPNG: stream doesn't contain a PNG");
      return false;
   }

   U32 prevWaterMark = FrameAllocator::getWaterMark();

#if defined(PNG_USER_MEM_SUPPORTED)
   png_structp png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
                                                NULL,
                                                pngFatalErrorFn,
                                                pngWarningFn,
                                                NULL,
                                                pngMallocFn,
                                                pngFreeFn);
#else
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                NULL,
                                                pngFatalErrorFn,
                                                pngWarningFn);
#endif

   if (png_ptr == NULL) 
   {
      FrameAllocator::setWaterMark(prevWaterMark);
      return false;
   }

   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      png_destroy_read_struct(&png_ptr,
                              (png_infopp)NULL,
                              (png_infopp)NULL);
      FrameAllocator::setWaterMark(prevWaterMark);
      return false;
   }

   png_infop end_info = png_create_info_struct(png_ptr);
   if (end_info == NULL) {
      png_destroy_read_struct(&png_ptr,
                              &info_ptr,
                              (png_infopp)NULL);
      FrameAllocator::setWaterMark(prevWaterMark);
      return false;
   }

   sg_pStream = &io_rStream;
   png_set_read_fn(png_ptr, NULL, pngReadDataFn);

   // Read off the info on the image.
   png_set_sig_bytes(png_ptr, cs_headerBytesChecked);
   png_read_info(png_ptr, info_ptr);

   // OK, at this point, if we have reached it ok, then we can reset the
   //  image to accept the new data...
   //
   deleteImage();

   png_uint_32 width;
   png_uint_32 height;
   S32 bit_depth;
   S32 color_type;

   png_get_IHDR(png_ptr, info_ptr,
                &width, &height,             // obv.
                &bit_depth, &color_type,     // obv.
                NULL,                        // interlace
                NULL,                        // compression_type
                NULL);                       // filter_type

   // First, handle the color transformations.  We need this to read in the
   //  data as RGB or RGBA, _always_, with a maximal channel width of 8 bits.
   //
   bool transAlpha     = false;
   BitmapFormat format = RGB;

   // Strip off any 16 bit info
   //
   if (bit_depth == 16) {
      png_set_strip_16(png_ptr);
   }

   // Expand a transparency channel into a full alpha channel...
   //
   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
      png_set_expand(png_ptr);
      transAlpha = true;
   }

   if (color_type == PNG_COLOR_TYPE_PALETTE) 
   {
      png_set_expand(png_ptr);
      format = transAlpha ? RGBA : RGB;
   } 
   else if (color_type == PNG_COLOR_TYPE_GRAY) 
   {
      png_set_expand(png_ptr);
      //png_set_gray_to_rgb(png_ptr);
      format = Alpha; //transAlpha ? RGBA : RGB;
   }
   else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) 
   {
      png_set_expand(png_ptr);
      png_set_gray_to_rgb(png_ptr);
      format = RGBA;
   }
   else if (color_type == PNG_COLOR_TYPE_RGB) 
   {
      format = transAlpha ? RGBA : RGB;
      png_set_expand(png_ptr);
   }
   else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) 
   {
      png_set_expand(png_ptr);
      format = RGBA;
   }

   // Update the info pointer with the result of the transformations
   //  above...
   png_set_interlace_handling(png_ptr);
   png_read_update_info(png_ptr, info_ptr);

   png_uint_32 rowBytes = (png_uint_32)png_get_rowbytes(png_ptr, info_ptr);
   if (format == RGB) {
      AssertFatal(rowBytes == width * 3,
                  "Error, our rowbytes are incorrect for this transform... (3)");
   } 
   else if (format == RGBA) 
   {
      AssertFatal(rowBytes == width * 4,
                  "Error, our rowbytes are incorrect for this transform... (4)");
   }

   // actually allocate the bitmap space...
   allocateBitmap(width, height,
                  false,            // don't extrude miplevels...
                  format);          // use determined format...

   // Set up the row pointers...
   AssertISV(height <= csgMaxRowPointers, "Error, cannot load pngs taller than 2048 pixels!");
   png_bytep* rowPointers = sRowPointers;
   U8* pBase = (U8*)getBits();
   for (U32 i = 0; i < height; i++)
      rowPointers[i] = pBase + (i * rowBytes);

   // And actually read the image!
   png_read_image(png_ptr, rowPointers);

   // We're outta here, destroy the png structs, and release the lock
   //  as quickly as possible...
   //png_read_end(png_ptr, end_info);
   png_read_end(png_ptr, NULL);
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

   sg_pStream = NULL;

   // Ok, the image is read in, now we need to finish up the initialization,
   //  which means: setting up the detailing members, init'ing the palette
   //  key, etc...
   //
   // actually, all of that was handled by allocateBitmap, so we're outta here
   //
   FrameAllocator::setWaterMark(prevWaterMark);

    //
   //-Mat if all palleted images are to be converted, set mForce16bit
   if( color_type == PNG_COLOR_TYPE_PALETTE ) {
       sgForcePalletedPNGsTo16Bit = dAtob( Con::getVariable("$pref::iPhone::ForcePalletedPNGsTo16Bit") );
       if( sgForcePalletedPNGsTo16Bit ) {
           mForce16Bit = true;
       }
   }
   return true;
}


//--------------------------------------------------------------------------
bool GBitmap::_writePNG(Stream&   stream,
                        const U32 compressionLevel,
                        const U32 strategy,
                        const U32 filter) const
{
   // ONLY RGB bitmap writing supported at this time!
   AssertFatal(getFormat() == RGB || getFormat() == RGBA || getFormat() == Alpha, "GBitmap::writePNG: ONLY RGB bitmap writing supported at this time.");
   if (internalFormat != RGB && internalFormat != RGBA && internalFormat != Alpha)
      return (false);

   #define MAX_HEIGHT 4096

   if (height >= MAX_HEIGHT)
      return (false);

#if defined(PNG_USER_MEM_SUPPORTED)
   png_structp png_ptr = png_create_write_struct_2(PNG_LIBPNG_VER_STRING,
                                                 NULL,
                                                 pngFatalErrorFn,
                                                 pngWarningFn,
                                                 NULL,
                                                 pngMallocFn,
                                                 pngFreeFn);
#else
   png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL,
                                                 pngFatalErrorFn,
                                                 pngWarningFn);
#endif

   if (png_ptr == NULL)
      return (false);

   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
      return false;
   }

   sg_pStream = &stream;
   png_set_write_fn(png_ptr, NULL, pngWriteDataFn, pngFlushDataFn);

   // Set the compression level, image filters, and compression strategy...
   png_set_compression_strategy( png_ptr, strategy );
   png_set_compression_window_bits(png_ptr, 15);
   png_set_compression_level(png_ptr, compressionLevel);
   png_set_filter(png_ptr, 0, filter);

   // Set the image information here.  Width and height are up to 2^31,
   // bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
   // the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
   // PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
   // or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
   // PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
   // currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED

   if (getFormat() == RGB) {
      png_set_IHDR(png_ptr, info_ptr,
                   width, height,               // the width & height
                   8, PNG_COLOR_TYPE_RGB,       // bit_depth, color_type,
                   PNG_INTERLACE_NONE,          // no interlace
                   PNG_COMPRESSION_TYPE_BASE,   // compression type
                   PNG_FILTER_TYPE_BASE);       // filter type
   }
   else if (getFormat() == RGBA) {
      png_set_IHDR(png_ptr, info_ptr,
                   width, height,               // the width & height
                   8, PNG_COLOR_TYPE_RGB_ALPHA, // bit_depth, color_type,
                   PNG_INTERLACE_NONE,          // no interlace
                   PNG_COMPRESSION_TYPE_BASE,   // compression type
                   PNG_FILTER_TYPE_BASE);       // filter type
   }
   else if (getFormat() == Alpha) {
      png_set_IHDR(png_ptr, info_ptr,
                   width, height,               // the width & height
                   8, PNG_COLOR_TYPE_GRAY,      // bit_depth, color_type,
                   PNG_INTERLACE_NONE,          // no interlace
                   PNG_COMPRESSION_TYPE_BASE,   // compression type
                   PNG_FILTER_TYPE_BASE);       // filter type
   }

   png_write_info(png_ptr, info_ptr);
   png_bytep row_pointers[MAX_HEIGHT];
   for (U32 i=0; i<height; i++)
      row_pointers[i] = const_cast<png_bytep>(getAddress(0, i));

   png_write_image(png_ptr, row_pointers);

   // Write S3TC data if present...
   // Write FXT1 data if present...

   png_write_end(png_ptr, info_ptr);
   png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

   return true;
}


//--------------------------------------------------------------------------
bool GBitmap::writePNG(Stream& stream, const bool compressHard) const
{
   U32 waterMark = FrameAllocator::getWaterMark();

   if (compressHard == false) {
      bool retVal = _writePNG(stream, 6, 0, PNG_ALL_FILTERS);
      FrameAllocator::setWaterMark(waterMark);
      return retVal;
   } else {
      U8* buffer = new U8[1 << 22]; // 4 Megs.  Should be enough...
      MemStream* pMemStream = new MemStream(1 << 22, buffer, false, true);

      // We have to try the potentially useful compression methods here.

      const U32 zStrategies[] = { Z_DEFAULT_STRATEGY,
                                  Z_FILTERED };
      const U32 pngFilters[]  = { PNG_FILTER_NONE,
                                  PNG_FILTER_SUB,
                                  PNG_FILTER_UP,
                                  PNG_FILTER_AVG,
                                  PNG_FILTER_PAETH,
                                  PNG_ALL_FILTERS };

      U32 minSize      = 0xFFFFFFFF;
      U32 bestStrategy = 0xFFFFFFFF;
      U32 bestFilter   = 0xFFFFFFFF;
      U32 bestCLevel   = 0xFFFFFFFF;

      for (U32 cl = 0; cl <=9; cl++) 
      {
         for (U32 zs = 0; zs < 2; zs++) 
         {
            for (U32 pf = 0; pf < 6; pf++) 
            {
               pMemStream->setPosition(0);

               if (_writePNG(*pMemStream, cl, zStrategies[zs], pngFilters[pf]) == false)
                  AssertFatal(false, "PNG output failed!");

               if (pMemStream->getPosition() < minSize) 
               {
                  minSize = pMemStream->getPosition();
                  bestStrategy = zs;
                  bestFilter   = pf;
                  bestCLevel   = cl;
               }
            }
         }
      }
      AssertFatal(minSize != 0xFFFFFFFF, "Error, no best found?");

      delete pMemStream;
      delete [] buffer;


      bool retVal = _writePNG(stream,
                              bestCLevel,
                              zStrategies[bestStrategy],
                              pngFilters[bestFilter]);
      FrameAllocator::setWaterMark(waterMark);
      return retVal;
   }
}

//--------------------------------------------------------------------------
bool GBitmap::writePNGUncompressed(Stream& stream) const
{
   U32 waterMark = FrameAllocator::getWaterMark();

   bool retVal = _writePNG(stream, 0, 0, PNG_FILTER_NONE);

   FrameAllocator::setWaterMark(waterMark);
   return retVal;
}
