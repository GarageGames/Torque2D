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

#ifndef _BMFONT_ASSET_H_
#define _BMFONT_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif
#include "platform/platformFont.h"
#include "graphics/bmFont.h"

//-----------------------------------------------------------------------------

DefineConsoleType( TypeBMFontAssetPtr )

//-----------------------------------------------------------------------------

class ImageAsset;

//-----------------------------------------------------------------------------

class BMFontAsset : public AssetBase
{
private:
    typedef AssetBase  Parent;

    StringTableEntry            mFontFile;

private:
    /// Imagery.
    Resource<bmFont>            mFont;

public:
    BMFontAsset();
    virtual ~BMFontAsset();

    static void initPersistFields();
    virtual bool onAdd();
    virtual void onRemove();

    void                    setFontFile( const char* pFontFile );
    inline StringTableEntry getFontFile( void ) const                      { return mFontFile; };

    // Asset validation.
    virtual bool    isAssetValid( void ) const;

    /// Declare Console Object.
    DECLARE_CONOBJECT(BMFontAsset);

protected:
    virtual void initializeAsset( void );
    virtual void onAssetRefresh( void );

protected:
    static void textureEventCallback( const U32 eventCode, void *userData );
    
    static bool setFontFile( void* obj, const char* data )                 { static_cast<BMFontAsset*>(obj)->setFontFile(data); return false; }
    static const char* getFontFile(void* obj, const char* data)            { return static_cast<BMFontAsset*>(obj)->getFontFile(); }
    static bool writeFontFile( void* obj, StringTableEntry pFieldName )    { return static_cast<BMFontAsset*>(obj)->getFontFile() != StringTable->EmptyString; }

};

#endif // _BMFONT_ASSET_H_