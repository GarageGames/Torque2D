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

#ifndef _BITMAP_FONT_H_
#include "bitmapFont/BitmapFont.h"
#endif

#ifndef _STRING_UNIT_H_
#include "string/stringUnit.h"
#endif

#include <string>

namespace font
{
   BitmapFont::BitmapFont()
   {

   }

   bool BitmapFont::parseFont(Stream& io_rStream)
   {
      U32 numBytes = io_rStream.getStreamSize() - io_rStream.getPosition();
      while ((io_rStream.getStatus() != Stream::EOS) && numBytes > 0)
      {
         char Read[256];
         char Token[256];
         char *buffer = Con::getReturnBuffer(256);
         io_rStream.readLine((U8 *)buffer, 256);

         char temp[256];
         U32 tokenCount = StringUnit::getUnitCount(buffer, "\"");

         if (tokenCount > 1)
         {
            dSprintf(Token, 256, "%s", StringUnit::getUnit(buffer, 1, "\""));
            dSprintf(temp, 256, "tok1");
            dSprintf(buffer, 256, "%s", (char*)StringUnit::setUnit(buffer, 1, temp, "\""));
         }

         U32 wordCount = StringUnit::getUnitCount(buffer, " \t\n");

         dSprintf(Read, 256, "%s", StringUnit::getUnit(buffer, 0, " \t\n"));
         if (dStrcmp(Read, "info") == 0)
         {
            U32 currentWordCount = 1;
            while (currentWordCount < wordCount)
            {
               dSprintf(Read, 256, StringUnit::getUnit(buffer, currentWordCount, " \t\n"));
               char temp[256];
               char Key[256];
               char Value[256];

               dSprintf(temp, 256, "%s", Read);
               dSprintf(Key, 256, "%s", StringUnit::getUnit(temp, 0, "="));
               dSprintf(Value, 256, "%s", StringUnit::getUnit(temp, 1, "="));

               if (dStrcmp(Value, "\"tok1\"") == 0) {
                  dSprintf(Value, 256, "%s", Token);
               }

               if (dStrcmp(Key, "size") == 0)
                  mSize = U16(dAtoi(Value));
               currentWordCount++;
            }
         }
         if (dStrcmp(Read, "common") == 0)
         {
            //this holds common data
            U32 currentWordCount = 1;
            while (currentWordCount < wordCount)
            {
               dSprintf(Read, 256, "%s", StringUnit::getUnit(buffer, currentWordCount, " \t\n"));
               char temp[256];
               char Key[256];
               char Value[256];

               dSprintf(temp, 256, "%s", Read);
               dSprintf(Key, 256, "%s", StringUnit::getUnit(temp, 0, "="));
               dSprintf(Value, 256, "%s", StringUnit::getUnit(temp, 1, "="));

               if (dStrcmp(Value, "\"tok1\"") == 0) {
                  dSprintf(Value, 256, "%s", Token);
               }

               if (dStrcmp(Key, "lineHeight") == 0)
                  mLineHeight = U16(dAtoi(Value));
               else if (dStrcmp(Key, "base") == 0)
                  mBaseline = U16(dAtoi(Value));
               else if (dStrcmp(Key, "scaleW") == 0)
                  mWidth = U16(dAtoi(Value));
               else if (dStrcmp(Key, "scaleH") == 0)
                  mHeight = U16(dAtoi(Value));
               else if (dStrcmp(Key, "pages") == 0)
                  mPages = U16(dAtoi(Value));
               currentWordCount++;
            }
         }
         else if (dStrcmp(Read, "page") == 0)
         {
            //this is data for a page
            U32 currentWordCount = 1;
            while (currentWordCount < wordCount)
            {
               dSprintf(Read, 256, "%s", StringUnit::getUnit(buffer, currentWordCount, " \t\n"));
               char temp[256];
               char Key[256];
               char Value[256];

               dSprintf(temp, 256, "%s", Read);
               dSprintf(Key, 256, "%s", StringUnit::getUnit(temp, 0, "="));
               dSprintf(Value, 256, "%s", StringUnit::getUnit(temp, 1, "="));

               if (dStrcmp(Value, "\"tok1\"") == 0) {
                  dSprintf(Value, 256, "%s", Token);
               }

               //assign the correct value
               if (dStrcmp(Key, "file") == 0)
               {
                  mPageName.push_back(StringTable->insert(Value));
               }

               currentWordCount++;
            }
         }
         else if (dStrcmp(Read, "char") == 0 && dStrcmp(Read, "chars") != 0)
         {
            //this is data for a character
            BitmapFontCharacter ci;
            U16 CharID = 0;
            U32 currentWordCount = 1;
            while (currentWordCount < wordCount)
            {
               dSprintf(Read, 256, "%s", StringUnit::getUnit(buffer, currentWordCount, " \t\n"));
               char temp[256];
               char Key[256];
               char Value[256];


               dSprintf(temp, 256, "%s", Read);
               dSprintf(Key, 256, "%s", StringUnit::getUnit(temp, 0, "="));
               dSprintf(Value, 256, "%s", StringUnit::getUnit(temp, 1, "="));

               if (dStrcmp(Value, "\"tok1\"") == 0) {
                  dSprintf(Value, 256, "%s", Token);
               }

               //assign the correct value
               if (dStrcmp(Key, "id") == 0)
                  CharID = U32(dAtoi(Value));
               else if (dStrcmp(Key, "x") == 0)
                  ci.mX = U32(dAtoi(Value));
               else if (dStrcmp(Key, "y") == 0)
                  ci.mY = U32(dAtoi(Value));
               else if (dStrcmp(Key, "width") == 0)
                  ci.mWidth = U32(dAtoi(Value));
               else if (dStrcmp(Key, "height") == 0)
                  ci.mHeight = U32(dAtoi(Value));
               else if (dStrcmp(Key, "xoffset") == 0)
                  ci.mXOffset = F32(dAtoi(Value));
               else if (dStrcmp(Key, "yoffset") == 0)
                  ci.mYOffset = F32(dAtoi(Value));
               else if (dStrcmp(Key, "xadvance") == 0)
                  ci.mXAdvance = F32(dAtoi(Value));
               else if (dStrcmp(Key, "page") == 0)
                  ci.mPage = S32(dAtoi(Value));
               currentWordCount++;
            }
            ci.mCharID = CharID;
            ci.ProcessCharacter(mWidth, mHeight);
            mChar[CharID] = ci;
         }
         else if (dStrcmp(Read, "kerning") == 0 && dStrcmp(Read, "kernings") != 0)
         {
            //this is data for a kerning pair
            U16 first = 0;
            U16 second = 0;
            S16 amount = 0;
            U32 currentWordCount = 1;
            while (currentWordCount < wordCount)
            {
               dSprintf(Read, 256, "%s", StringUnit::getUnit(buffer, currentWordCount, " \t\n"));
               char temp[256];
               char Key[256];
               char Value[256];


               dSprintf(temp, 256, "%s", Read);
               dSprintf(Key, 256, "%s", StringUnit::getUnit(temp, 0, "="));
               dSprintf(Value, 256, "%s", StringUnit::getUnit(temp, 1, "="));

               if (dStrcmp(Value, "\"tok1\"") == 0) {
                  dSprintf(Value, 256, "%s", Token);
               }

               //assign the correct value
               if (dStrcmp(Key, "first") == 0)
                  first = U16(dAtoi(Value));
               else if (dStrcmp(Key, "second") == 0)
                  second = U16(dAtoi(Value));
               else if (dStrcmp(Key, "amount") == 0)
                  amount = S16(dAtoi(Value));
               
               currentWordCount++;
            }
            AddKerning(first, second, amount);
         }
      }

      return (io_rStream.getStatus() == Stream::EOS);
   }

   TextureHandle BitmapFont::LoadTexture(StringTableEntry fileName)
   {
      // Debug Profiling.
      PROFILE_SCOPE(FontAsset_LoadTexture);

      // Get image texture.
      TextureHandle mImageTextureHandle;
      mImageTextureHandle.set(fileName, TextureHandle::BitmapTexture, true, false);

      // Is the texture valid?
      if (mImageTextureHandle.IsNull())
      {
         // No, so warn.
         Con::warnf("FontAsset could not load texture '%s'.", fileName);
         return mImageTextureHandle;
      }

      //Set the filter mode. For now we'll just support bilinear. In the future we should give the user an option.
      mImageTextureHandle.setFilter(GL_LINEAR);

      return mImageTextureHandle;
   }
}