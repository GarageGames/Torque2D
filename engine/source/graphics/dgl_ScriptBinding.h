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

/*! @defgroup ImageFileManipulation Image File Manipulation
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Capture a specific area of the screen
*/
ConsoleFunctionWithDocs(CaptureScreenArea, bool, 7, 7, (posX, posY, width, height, fileName, fileType))
{
    GLint positionX = dAtoi(argv[1]);
    GLint positionY = dAtoi(argv[2]);
    U32 width = dAtoi(argv[3]);
    U32 height = dAtoi(argv[4]);
    
    FileStream fStream;
    if(!fStream.open(argv[5], FileStream::Write))
    { 
        Con::printf("Failed to open file '%s'.", argv[5]);
        return false;
    }

    // Read gl pixels here
    glReadBuffer(GL_FRONT);
   
    Point2I extent;
    extent.x = width;
    extent.y = height;

    U8 * pixels = new U8[extent.x * extent.y * 4];
    glReadPixels(positionX, positionY, extent.x, extent.y, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    GBitmap * bitmap = new GBitmap;
    bitmap->allocateBitmap(U32(extent.x), U32(extent.y));
   
    // flip the rows
    for(U32 y = 0; y < (U32)extent.y; y++)
        dMemcpy(bitmap->getAddress(0, extent.y - y - 1), pixels + y * extent.x * 3, U32(extent.x * 3));

    if ( dStrcmp( argv[6], "JPEG" ) == 0 )
        bitmap->writeJPEG(fStream);
    else if( dStrcmp( argv[6], "PNG" ) == 0)
        bitmap->writePNG(fStream);
    else
        bitmap->writePNG(fStream);

    fStream.close();

    delete [] pixels;
    delete bitmap;

    return true;
}

/*! Use the png2jpg function to save a PNG file specified by pngFilename as a similarly named JPEG file with the optionally specified quality.
    @param pngFilename The path and file name of the PNG file to convert.
    @param quality An optional quality between 0 and 100. The default quality is 90.
    @return Returns -1 if the file could not be opened, 0 on other failures, and 1 if the conversion worked
*/
ConsoleFunctionWithDocs(png2jpg, ConsoleInt, 2, 3, ( pngFilename, [quality ]? ))
{
   extern U32 gJpegQuality;
   const char * rgbname = NULL;
   const char * alphaname = NULL;
   const char * bmpname = argv[1];
   if(argc == 3)
      gJpegQuality = dAtoi(argv[2]);
   else
      gJpegQuality = 90;

   Con::printf("Converting file: %s", argv[1]);

   if (!rgbname)
   {
      char * buf = new char[dStrlen(bmpname)+32];
      dStrcpy(buf,bmpname);
      char * pos = dStrstr((const char*)buf,".png");
      if (!pos)
         pos = buf + dStrlen(buf);
      dStrcpy(pos,".jpg");
      rgbname = buf;
   }
   if (!alphaname)
   {
      char * buf = new char[dStrlen(bmpname)+32];
      dStrcpy(buf,bmpname);
      char * pos = dStrstr((const char*)buf,".png");
      if (!pos)
         pos = buf + dStrlen(buf);
      dStrcpy(pos,".alpha.jpg");
      alphaname = buf;
   }
   GBitmap bmp;
   FileStream fs;
   if (fs.open(bmpname, FileStream::Read) == false) {
      Con::printf("Error: unable to open file: %s for reading\n", bmpname);
      return -1;
   }
   if (bmp.readPNG(fs) == false) {
      Con::printf("Error: unable to read %s as a .PNG\n", bmpname);
      return -1;
   }
   fs.close();

   if (bmp.getFormat() != GBitmap::RGB &&
       bmp.getFormat() != GBitmap::RGBA) {
      Con::printf("Error: %s is not a 24 or 32-bit .PNG\n", bmpname);
      return false;
   }

   GBitmap * outRGB = NULL;
   GBitmap * outAlpha = NULL;
   GBitmap workRGB, workAlpha;
   if (bmp.getFormat() == GBitmap::RGB)
      outRGB = &bmp;
   else
   {
      S32 w = bmp.getWidth();
      S32 h = bmp.getHeight();
      workRGB.allocateBitmap(w,h,false,GBitmap::RGB);
      workAlpha.allocateBitmap(w,h,false,GBitmap::Alpha);

      U8 * rgbBits = workRGB.getWritableBits();
      U8 * alphaBits = workAlpha.getWritableBits();
      U8 * bmpBits = bmp.getWritableBits();
      for (S32 i=0; i<w; i++)
      {
         for (S32 j=0; j<h; j++)
         {
            rgbBits[i*3 + j*3*w + 0] = bmpBits[i*4 + j*4*w + 0];
            rgbBits[i*3 + j*3*w + 1] = bmpBits[i*4 + j*4*w + 1];
            rgbBits[i*3 + j*3*w + 2] = bmpBits[i*4 + j*4*w + 2];
            alphaBits[i + j*w]       = bmpBits[i*4 + j*4*w + 3];
         }
      }
      Con::printf("texture: width=%i, height=%i\n",w,h);
      outRGB = &workRGB;
      outAlpha = &workAlpha;
   }

   if (outRGB)
   {
      FileStream fws;
      if (fws.open(rgbname, FileStream::Write) == false)
      {
         Con::printf("Error: unable to open file: %s for writing\n", rgbname);
         return -1;
      }

      if (dStrstr(rgbname,".png"))
      {
         if (outRGB->writePNG(fws) == false)
         {
            fws.close();
            Con::printf("Error: couldn't write RGB as a png\n");
            return -1;
         }
      }
      else if (outRGB->writeJPEG(fws) == false)
      {
         Con::printf("Error: couldn't write RGB as a jpg\n");
         return -1;
      }
      fws.close();
   }
   if (outAlpha)
   {
      gJpegQuality = 60;
      FileStream fws;
      if (fws.open(alphaname, FileStream::Write) == false)
      {
         Con::printf("Error: unable to open file: %s for writing\n", alphaname);
         return -1;
      }

      if (dStrstr(alphaname,".png"))
      {
         if (outAlpha->writePNG(fws) == false)
         {
            fws.close();
            Con::printf("Error: couldn't write alpha as a png\n");
            return -1;
         }
      }
      else if (outAlpha->writeJPEG(fws) == false)
      {
         Con::printf("Error: couldn't write alpha as a jpg\n");
         return -1;
      }
      fws.close();
   }

   return(0);
}

/*! @} */ // end group ImageFileManipulation
