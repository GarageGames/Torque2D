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
#define _BITMAP_FONT_H_

#ifndef _BITMAP_FONT_CHARACTER_H_
#include "bitmapFont/BitmapFontCharacter.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

using namespace std;
#include <map>
#include <string>
#include <vector>
using KerningMap = map<pair<U16, U16>, S16>;
using CharacterMap = map<U16, font::BitmapFontCharacter>;

namespace font
{
   class BitmapFont
   {
   private:
      U16 mWidth, mHeight;
      U16 mPages;

      CharacterMap mChar;       
      KerningMap mKerning;

   public:
      U16 mLineHeight;
      U16 mBaseline;
      U16 mSize;
      std::vector<StringTableEntry> mPageName;
      std::vector<TextureHandle> mTexture;

      BitmapFont();
      bool parseFont(Stream& io_rStream);
      TextureHandle LoadTexture(StringTableEntry fileName);
      const BitmapFontCharacter& getCharacter(const U16 charID) { return mChar[charID]; }
      inline const F32 getSizeRatio(const F32 size) { return size / mLineHeight; }
      inline const S16 getKerning(U16 first, U16 second) { return (S16)mKerning[make_pair(first, second)]; }

   private:
      inline void AddKerning(U16 first, U16 second, S16 amount) { mKerning[make_pair(first, second)] = amount; }
   };
}

#endif // _BITMAP_FONT_H_
