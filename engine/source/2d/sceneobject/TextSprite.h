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

#ifndef _TEXT_SPRITE_H_
#define _TEXT_SPRITE_H_

#ifndef _STRINGBUFFER_H_
#include "string/stringBuffer.h"
#endif

#ifndef _FONT_ASSET_H_
#include "2d/assets/FontAsset.h"
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

class TextSprite : public SceneObject
{
    typedef SceneObject          Parent;

public:
    enum TextAlign
    {
        INVALID_ALIGN,

        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT,
        ALIGN_JUSTIFY,
    };

    enum TextVAlign
    {
       INVALID_VALIGN,

       VALIGN_TOP,
       VALIGN_MIDDLE,
       VALIGN_BOTTOM,
    };

    enum OverflowModeX
    {
       INVALID_OVERFLOW_X,

       OVERFLOW_X_WRAP,
       OVERFLOW_X_EXPAND,
       OVERFLOW_X_HIDDEN,
       OVERFLOW_X_SHRINK,
    };

    enum OverflowModeY
    {
       INVALID_OVERFLOW_Y,

       OVERFLOW_Y_EXPAND,
       OVERFLOW_Y_HIDDEN,
       OVERFLOW_Y_SHRINK,
    };

private:
    AssetPtr<FontAsset>     mFontAsset;
    StringBuffer            mText;
    F32                     mFontSize;
    F32                     mFontScaleX;
    F32                     mFontScaleY;
    TextAlign               mTextAlign;
    TextVAlign              mTextVAlign;
    OverflowModeX           mOverflowX;
    OverflowModeY           mOverflowY;
    bool                    mAutoLineHeight;
    F32                     mCustomLineHeight;
    F32                     mKerning;

public:
    TextSprite();
    ~TextSprite();

    static void initPersistFields();

    bool onAdd();
    void onRemove();
    void copyTo(SimObject* object);

    virtual bool canPrepareRender( void ) const                             { return true; }
    virtual bool validRender( void ) const                                  { return mFontAsset.notNull() && mText.length() > 0; }
    virtual bool shouldRender( void ) const                                 { return true; }
    virtual void scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue );
    virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );

    bool setFont( const char* pFontAssetId );
    const char* getFont(void) const                                         { return mFontAsset.getAssetId(); }
    inline void setText(const StringBuffer& text)                           { mText.set(&text); }
    inline StringBuffer& getText(void)                                      { return mText; }
    inline void setFontSize(const F32 size)                                 { mFontSize = size; }
    inline F32 getFontSize(void) const                                      { return mFontSize; }
    inline void setFontScaleX(const F32 scale)                              { mFontScaleX = scale; }
    inline F32 getFontScaleX(void) const                                    { return mFontScaleX; }
    inline void setFontScaleY(const F32 scale)                              { mFontScaleY = scale; }
    inline F32 getFontScaleY(void) const                                    { return mFontScaleY; }

    inline void setTextAlignment(const TextAlign alignment)                 { mTextAlign = alignment; }
    inline TextAlign getTextAlignment(void) const                           { return mTextAlign; }
    static TextAlign getTextAlignmentEnum(const char* label);
    static const char* getTextAlignmentDescription(const TextAlign alignment);

    inline void setTextVAlignment(const TextVAlign alignment)               { mTextVAlign = alignment; }
    inline TextVAlign getTextVAlignment(void) const                         { return mTextVAlign; }
    static TextVAlign getTextVAlignmentEnum(const char* label);
    static const char* getTextVAlignmentDescription(const TextVAlign alignment);

    inline void setOverflowModeX(const OverflowModeX mode)                  { mOverflowX = mode; }
    inline OverflowModeX getOverflowModeX(void) const                       { return mOverflowX; }
    static OverflowModeX getOverflowModeXEnum(const char* label);
    static const char* getOverflowModeXDescription(const OverflowModeX mode);

    inline void setOverflowModeY(const OverflowModeY mode)                  { mOverflowY = mode; }
    inline OverflowModeY getOverflowModeY(void) const                       { return mOverflowY; }
    static OverflowModeY getOverflowModeYEnum(const char* label);
    static const char* getOverflowModeYDescription(const OverflowModeY mode);

    inline void setAutoLineHeight(const bool isAuto)                        { mAutoLineHeight = isAuto; }
    inline bool getAutoLineHeight(void) const                               { return mAutoLineHeight; }
    inline void setCustomLineHeight(const F32 lineHeight)                   { mCustomLineHeight = lineHeight; }
    inline F32 getCustomLineHeight(void) const                              { return mCustomLineHeight; }

    inline void setKerning(const F32 kern)                                  { mKerning = kern; }
    inline F32 getKerning(void) const                                       { return mKerning; }

    // Declare Console Object.
    DECLARE_CONOBJECT(TextSprite);

protected:
    static bool setFont(void* obj, const char* data)                        { static_cast<TextSprite*>(obj)->setFont( data ); return false; }
    static const char* getFont(void* obj, const char* data)                 { return static_cast<TextSprite*>(obj)->getFont(); }
    static bool writeFont( void* obj, StringTableEntry pFieldName )         { return static_cast<TextSprite*>(obj)->mFontAsset.notNull(); }

    static bool setText( void* obj, const char* data )                      { static_cast<TextSprite*>( obj )->setText( data ); return false; }
    static const char* getText( void* obj, const char* data )               { return static_cast<TextSprite*>( obj )->getText().getPtr8(); }
    static bool writeText( void* obj, StringTableEntry pFieldName )         { return static_cast<TextSprite*>(obj)->mText.length() != 0; }

    static bool setFontSize(void* obj, const char* data)                    { static_cast<TextSprite*>(obj)->setFontSize( dAtof(data) ); return false; }
    static bool writeFontSize(void* obj, StringTableEntry pFieldName)       { return static_cast<TextSprite*>(obj)->getFontSize() != 1.0f; }

    static bool setTextAlignment( void* obj, const char* data );
    static bool writeTextAlignment( void* obj, StringTableEntry pFieldName ){return static_cast<TextSprite*>(obj)->getTextAlignment() != TextSprite::ALIGN_CENTER; }

private:
   void RenderLetter(BatchRender* pBatchRenderer, Vector2& cursor, U32 charID, F32 ratio);
};

#endif // _TEXT_SPRITE_H_
