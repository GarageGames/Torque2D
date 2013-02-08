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
#include "graphics/gPalette.h"

const U32 GPalette::csm_fileVersion       = 1;

GPalette::GPalette()
: m_paletteType(RGB)
{
   //
}

GPalette::~GPalette()
{
   //
}

//-------------------------------------- Supplimentary I/O
bool
GPalette::readMSPalette(const char* in_pFileName)
{
   AssertFatal(in_pFileName != NULL, "GPalette::readMSPalette: NULL FileName");

   FileStream frs;
   if (frs.open(in_pFileName, FileStream::Read) == false) {
      return false;
   } else {
      bool success = readMSPalette(frs);
      frs.close();

      return success;
   }
}

bool
GPalette::writeMSPalette(const char* in_pFileName) const
{
   AssertFatal(in_pFileName != NULL, "GPalette::writeMSPalette: NULL FileName");

   FileStream fws;
   if (fws.open(in_pFileName, FileStream::Write) == false) {
      return false;
   } else {
      bool success = writeMSPalette(fws);
      fws.close();

      return success;
   }
}

bool
GPalette::readMSPalette(Stream& io_rStream)
{
   AssertFatal(io_rStream.getStatus() != Stream::Closed,
               "GPalette::writeMSPalette: can't write to a closed stream!");

   U32 data;
   U32 size;

   io_rStream.read(&data);
   io_rStream.read(&size);

   if (data == makeFourCCTag('R', 'I', 'F', 'F')) {
      io_rStream.read(&data);
      io_rStream.read(&size);
   }

   if (data == makeFourCCTag('P', 'A', 'L', ' ')) {
      io_rStream.read(&data);    // get number of colors (ignored)
      io_rStream.read(&data);    // skip the version number.

      // Read the colors...
      io_rStream.read(256 * sizeof(ColorI), m_pColors);

      // With MS Pals, we assume that the type is RGB, clear out all the alpha
      //  members so the palette keys are consistent across multiple palettes
      //
      for (U32 i = 0; i < 256; i++)
         m_pColors[i].alpha = 0;

      m_paletteType = RGB;

      return (io_rStream.getStatus() == Stream::Ok);
   }

   AssertWarn(false, "GPalette::readMSPalette: not a MS Palette");
   return false;
}

bool
GPalette::writeMSPalette(Stream& io_rStream) const
{
   AssertFatal(io_rStream.getStatus() != Stream::Closed,
               "GPalette::writeMSPalette: can't write to a closed stream!");

   io_rStream.write(U32(makeFourCCTag('R', 'I', 'F', 'F')));
   io_rStream.write(U32((256 * sizeof(ColorI)) + 8 + 4 + 4));

   io_rStream.write(U32(makeFourCCTag('P', 'A', 'L', ' ')));
   io_rStream.write(U32(makeFourCCTag('d', 'a', 't', 'a')));

   io_rStream.write(U32(0x0404));   // Number of colors + 4

   io_rStream.write(U16(0x300));    // version
   io_rStream.write(U16(256));      // num colors...

   io_rStream.write(256 * sizeof(ColorI), m_pColors);

   return (io_rStream.getStatus() == Stream::Ok);
}

//------------------------------------------------------------------------------
//-------------------------------------- Persistent I/O
//
bool
GPalette::read(Stream& io_rStream)
{
   // Handle versioning
   U32 version;
   io_rStream.read(&version);
   AssertFatal(version == csm_fileVersion, "Palette::read: wrong file version...");

   U32 type;
   io_rStream.read(&type);
   m_paletteType = PaletteType(type);
   io_rStream.read(256 * sizeof(ColorI), m_pColors);

   return (io_rStream.getStatus() == Stream::Ok);
}

bool
GPalette::write(Stream& io_rStream) const
{
   // Handle versioning...
   io_rStream.write(csm_fileVersion);

   io_rStream.write(U32(m_paletteType));
   io_rStream.write(256 * sizeof(ColorI), m_pColors);

   return (io_rStream.getStatus() == Stream::Ok);
}
