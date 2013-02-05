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

#include "graphics/DynamicTexture.h"
#include "graphics/TextureManager.h"

//-----------------------------------------------------------------------------

DynamicTexture::DynamicTexture() :
    mpBitmap( NULL )
{
    // Generate a texture key.
    mTextureKey = TextureManager::getUniqueTextureKey();
}

//-----------------------------------------------------------------------------

DynamicTexture::~DynamicTexture()
{
}

//-----------------------------------------------------------------------------

void DynamicTexture::setSize( const U32 texelWidth, const U32 texelHeight )
{
    // Finish if any existing bitmap is adequate.
    if ( mpBitmap != NULL && texelWidth == mpBitmap->getWidth() && texelHeight == mpBitmap->getHeight() )
        return;

    // Generate new bitmap.
    // NOTE: Any previous bitmap would be allocated to the texture handle therefore destroyed
    // when the texture handle is modified.
    mpBitmap = new GBitmap( texelWidth, texelHeight, false, GBitmap::RGBA );

    // Set texture against bitmap.
    mTextureHandle.set( mTextureKey, mpBitmap, TextureHandle::BitmapKeepTexture );
}