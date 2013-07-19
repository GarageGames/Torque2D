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

#ifndef _BITMAP_FONT_OBJECT_H_
#define _BITMAP_FONT_OBJECT_H_

#ifndef _STRINGBUFFER_H_
#include "string/stringBuffer.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/utility.h"
#endif

//-----------------------------------------------------------------------------

class ImageFont : public SceneObject
{
    typedef SceneObject          Parent;

public:
    enum TextAlignment
    {
        INVALID_ALIGN,

        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT
    };

private:
    struct TextCell
    {
        char CharValue;
        U32 FrameCell;

        TextCell(char charValue, U32 frameCell)
        {
            CharValue = charValue;
            FrameCell = frameCell;
        }
    };

private:
    AssetPtr<ImageAsset>    mImageAsset;
    StringBuffer            mText;
    F32                     mFontPadding;
    Vector2                 mFontSize;
    TextAlignment           mTextAlignment;

private:
    void calculateSpatials( void );

public:
    ImageFont();
    ~ImageFont();

    static void initPersistFields();

    bool onAdd();
    void onRemove();
    void copyTo(SimObject* object);

    virtual bool canPrepareRender( void ) const                             { return true; }
    virtual bool validRender( void ) const                                  { return mImageAsset.notNull() && mText.length() > 0; }
    virtual bool shouldRender( void ) const                                 { return true; }
    virtual void scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue );
    virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );

    bool setImage( const char* pImageAssetId );
    const char* getImage( void ) const                                      { return mImageAsset.getAssetId(); };
    void setText( const StringBuffer& text );
    inline StringBuffer& getText( void )                                    { return mText; }
    void setTextAlignment( const TextAlignment alignment );
    inline TextAlignment getTextAlignment( void ) const                     { return mTextAlignment; }
    void setFontSize( const Vector2& size );
    inline Vector2 getFontSize( void ) const                                { return mFontSize; }
    void setFontPadding( const F32 padding );
    inline F32 getFontPadding( void ) const                                 { return mFontPadding; }

    static TextAlignment getTextAlignmentEnum(const char* label);
    static const char* getTextAlignmentDescription(const TextAlignment alignment);

    // Declare Console Object.
    DECLARE_CONOBJECT(ImageFont);

protected:
    static bool setImage(void* obj, const char* data)                       { static_cast<ImageFont*>(obj)->setImage( data ); return false; }
    static const char* getImage(void* obj, const char* data)                { return static_cast<ImageFont*>(obj)->getImage(); }
    static bool writeImage( void* obj, StringTableEntry pFieldName )        { return static_cast<ImageFont*>(obj)->mImageAsset.notNull(); }
    static bool setText( void* obj, const char* data )                      { static_cast<ImageFont*>( obj )->setText( data ); return false; }
    static const char* getText( void* obj, const char* data )               { return static_cast<ImageFont*>( obj )->getText().getPtr8(); }
    static bool writeText( void* obj, StringTableEntry pFieldName )         { return static_cast<ImageFont*>(obj)->mText.length() != 0; }
    static bool setTextAlignment( void* obj, const char* data );
    static bool writeTextAlignment( void* obj, StringTableEntry pFieldName ){return static_cast<ImageFont*>(obj)->getTextAlignment() != ImageFont::ALIGN_CENTER; }
    static bool setFontSize( void* obj, const char* data )                  { static_cast<ImageFont*>( obj )->setFontSize( Utility::mGetStringElementVector(data) ); return false; }
	static bool writeFontSize( void* obj, StringTableEntry pFieldName )     { return static_cast<ImageFont*>(obj)->getFontSize().notEqual(Vector2::getOne()); }	
    static bool setFontPadding( void* obj, const char* data )               { static_cast<ImageFont*>( obj )->setFontPadding( dAtof(data) ); return false; }
    static bool writeFontPadding( void* obj, StringTableEntry pFieldName )  { return static_cast<ImageFont*>(obj)->getFontPadding() != 0; }
};

#endif // _BITMAP_FONT_OBJECT_H_
