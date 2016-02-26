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

#ifndef _FONT_ASSET_H_
#define _FONT_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#ifndef _BITMAP_FONT_H_
#include "bitmapFont/BitmapFont.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType(TypeFontAssetPtr)

//-----------------------------------------------------------------------------

using namespace font;

class FontAsset : public AssetBase
{
private:
    typedef AssetBase Parent;

public:
    StringTableEntry                mFontFile;
    AssetPtr<ImageAsset>            mImageAsset;
    BitmapFont                      mBitmapFont;

public:
    FontAsset();
    virtual ~FontAsset();

    /// Core.
    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();
    virtual void copyTo(SimObject* object);

    void                    setFontFile( const char* pFontFile );
    inline StringTableEntry getFontFile( void ) const                   { return mFontFile; }

    inline TextureHandle&   getImageTexture(U16 pageID)                         { return mBitmapFont.mTexture[pageID]; }

    /// Declare Console Object.
    DECLARE_CONOBJECT(FontAsset);

private:
    void buildFontData( void );

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );

    /// Taml callbacks.
    virtual void onTamlPreWrite( void );
    virtual void onTamlPostWrite( void );
    virtual void onTamlCustomWrite( TamlCustomNodes& customNodes );
    virtual void onTamlCustomRead( const TamlCustomNodes& customNodes );


protected:
    static bool setFontFile( void* obj, const char* data )              { static_cast<FontAsset*>(obj)->setFontFile(data); return false; }
    static bool writeFontFile( void* obj, StringTableEntry pFieldName ) { return static_cast<FontAsset*>(obj)->getFontFile() != StringTable->EmptyString; }
};

#endif // _FONT_ASSET_H_
