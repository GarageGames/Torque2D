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

#ifndef _DYNAMIC_TEXTURE_H_
#define _DYNAMIC_TEXTURE_H_

#ifndef _TEXTURE_HANDLE_H_
#include "graphics/TextureHandle.h"
#endif

#ifndef _TEXTURE_HANDLE_H_
#include "graphics/textureHandle.h"
#endif

#ifndef _SIM_OBJECT_H_
#include "sim/simObject.h"
#endif

//-----------------------------------------------------------------------------

class DynamicTexture : SimObject
{
    typedef SimObject Parent;

private:
    StringTableEntry    mTextureKey;
    TextureHandle       mTextureHandle;
    GBitmap*            mpBitmap;

public:
    DynamicTexture();
    ~DynamicTexture();

    void setSize( const U32 texelWidth, const U32 texelHeight );
    TextureHandle getTextureHandle()
    {
    	return mTextureHandle;
    }
};

#endif // _DYNAMIC_TEXTURE_H_
