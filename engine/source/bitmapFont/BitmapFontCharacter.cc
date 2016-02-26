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
#include "bitmapFont/BitmapFontCharacter.h"
#endif

namespace font
{
   void BitmapFontCharacter::ProcessCharacter(U16 width, U16 height)
   {
      F32 top, right, bottom, left;

      bottom = (F32)mY / height;
      top = (F32)(mY + mHeight) / height;

      left = (F32)mX / width;
      right = (F32)(mX + mWidth) / width;

      mOOBB[0] = Vector2(left, top);
      mOOBB[1] = Vector2(right, top);
      mOOBB[2] = Vector2(right, bottom);
      mOOBB[3] = Vector2(left, bottom);
      mPageWidth = width;
      mPageHeight = height;
   }
}