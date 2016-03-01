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

#ifndef _BITMAP_FONT_CHARACTER_H_
#define _BITMAP_FONT_CHARACTER_H_

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

namespace font
{
   class BitmapFontCharacter
   {
   public:
      U16 mCharID;
      U16 mX, mY;
      U16 mWidth, mHeight;
      F32 mXOffset, mYOffset;
      F32 mXAdvance;
      U16 mPage;
      U16 mPageWidth, mPageHeight;
      Vector2 mOOBB[4];

      BitmapFontCharacter() : mX(0), mY(0), mWidth(0), mHeight(0), mXOffset(0), mYOffset(0), mXAdvance(0), mPage(0)
      {

      }

      void ProcessCharacter(U16 width, U16 height);
   };
}

#endif // _BITMAP_FONT_CHARACTER_H_
