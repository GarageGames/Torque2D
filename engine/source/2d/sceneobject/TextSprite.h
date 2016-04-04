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

#ifndef _BITMAP_FONT_LINE_INFO_H_
#include "bitmapFont/BitmapFontLineInfo.h"
#endif

#ifndef _BITMAP_FONT_CHARACTER_INFO_H_
#include "bitmapFont/BitmapFontCharacterInfo.h"
#endif

using CharInfoMap = std::map<U32, BitmapFontCharacterInfo>;

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
       OVERFLOW_X_VISIBLE,
       OVERFLOW_X_HIDDEN,
       OVERFLOW_X_SHRINK,
    };

    enum OverflowModeY
    {
       INVALID_OVERFLOW_Y,

       OVERFLOW_Y_VISIBLE,
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

    std::vector<BitmapFontLineInfo> mLine;
    bool                    mFontSpatialsDirty;
    Vector2                 mCalculatedSize;
    CharInfoMap             mCharInfo;


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
    inline void setText(const StringBuffer& text)                           { mText.set(&text); mFontSpatialsDirty = true; mCharInfo.clear(); }
    inline StringBuffer& getText(void)                                      { return mText; }
    inline void setFontSize(const F32 size)                                 { mFontSize = size; mFontSpatialsDirty = true; }
    inline F32 getFontSize(void) const                                      { return mFontSize; }
    inline void setFontScaleX(const F32 scale)                              { mFontScaleX = scale; mFontSpatialsDirty = true; }
    inline F32 getFontScaleX(void) const                                    { return mFontScaleX; }
    inline void setFontScaleY(const F32 scale)                              { mFontScaleY = scale; mFontSpatialsDirty = true; }
    inline F32 getFontScaleY(void) const                                    { return mFontScaleY; }

    inline void setTextAlignment(const TextAlign alignment)                 { mTextAlign = alignment; mFontSpatialsDirty = true; }
    inline TextAlign getTextAlignment(void) const                           { return mTextAlign; }
    static TextAlign getTextAlignmentEnum(const char* label);
    static const char* getTextAlignmentDescription(const TextAlign alignment);

    inline void setTextVAlignment(const TextVAlign alignment)               { mTextVAlign = alignment; mFontSpatialsDirty = true; }
    inline TextVAlign getTextVAlignment(void) const                         { return mTextVAlign; }
    static TextVAlign getTextVAlignmentEnum(const char* label);
    static const char* getTextVAlignmentDescription(const TextVAlign alignment);

    inline void setOverflowModeX(const OverflowModeX mode)                  { mOverflowX = mode; mFontSpatialsDirty = true; }
    inline OverflowModeX getOverflowModeX(void) const                       { return mOverflowX; }
    static OverflowModeX getOverflowModeXEnum(const char* label);
    static const char* getOverflowModeXDescription(const OverflowModeX mode);

    inline void setOverflowModeY(const OverflowModeY mode)                  { mOverflowY = mode; mFontSpatialsDirty = true; }
    inline OverflowModeY getOverflowModeY(void) const                       { return mOverflowY; }
    static OverflowModeY getOverflowModeYEnum(const char* label);
    static const char* getOverflowModeYDescription(const OverflowModeY mode);

    inline void setAutoLineHeight(const bool isAuto)                        { mAutoLineHeight = isAuto; mFontSpatialsDirty = true; }
    inline bool getAutoLineHeight(void) const                               { return mAutoLineHeight; }
    inline void setCustomLineHeight(const F32 lineHeight)                   { mCustomLineHeight = lineHeight; mFontSpatialsDirty = true; }
    inline F32 getCustomLineHeight(void) const                              { return mCustomLineHeight; }

    inline void setKerning(const F32 kern)                                  { mKerning = kern; mFontSpatialsDirty = true; }
    inline F32 getKerning(void) const                                       { return mKerning; }

    void resetCharacterSettings(void)                                       { mCharInfo.clear(); }

    void setCharacterBlendColor(const U32 charNum, const ColorF color);
    ColorF getCharacterBlendColor(const U32 charNum);
    bool getCharacterHasBlendColor(const U32 charNum);
    void resetCharacterBlendColor(const U32 charNum);

    void setCharacterScale(const U32 charNum, const F32 scaleX, const F32 scaleY);
    Vector2 getCharacterScale(const U32 charNum);
    void resetCharacterScale(const U32 charNum);

    void setCharacterOffset(const U32 charNum, const F32 offsetX, const F32 offsetY);
    Vector2 getCharacterOffset(const U32 charNum);
    void resetCharacterOffset(const U32 charNum);

    // Declare Console Object.
    DECLARE_CONOBJECT(TextSprite);

protected:
    static bool setFont(void* obj, const char* data)                        { static_cast<TextSprite*>(obj)->setFont( data ); return false; }
    static const char* getFont(void* obj, const char* data)                 { return static_cast<TextSprite*>(obj)->getFont(); }
    static bool writeFont( void* obj, StringTableEntry pFieldName )         { return static_cast<TextSprite*>(obj)->mFontAsset.notNull(); }

    static bool setText( void* obj, const char* data )                      { static_cast<TextSprite*>( obj )->setText( data ); return false; }
    static const char* getText( void* obj, const char* data )               { return static_cast<TextSprite*>( obj )->getText().getPtr8(); }
    static bool writeText(void* obj, StringTableEntry pFieldName)         { return static_cast<TextSprite*>(obj)->mText.length() != 0; }

    static bool setFontSize(void* obj, const char* data)                    { static_cast<TextSprite*>(obj)->setFontSize(dAtof(data)); return false; }
    static bool writeFontSize(void* obj, StringTableEntry pFieldName)       { return static_cast<TextSprite*>(obj)->getFontSize() != 1.0f; }

    static bool setFontScaleX(void* obj, const char* data)                    { static_cast<TextSprite*>(obj)->setFontScaleX(dAtof(data)); return false; }
    static bool writeFontScaleX(void* obj, StringTableEntry pFieldName)       { return static_cast<TextSprite*>(obj)->getFontScaleX() != 1.0f; }

    static bool setFontScaleY(void* obj, const char* data)                    { static_cast<TextSprite*>(obj)->setFontScaleY(dAtof(data)); return false; }
    static bool writeFontScaleY(void* obj, StringTableEntry pFieldName)       { return static_cast<TextSprite*>(obj)->getFontScaleY() != 1.0f; }

    static bool setTextAlignment(void* obj, const char* data);
    static bool writeTextAlignment(void* obj, StringTableEntry pFieldName){ return static_cast<TextSprite*>(obj)->getTextAlignment() != TextSprite::ALIGN_LEFT; }

    static bool setTextVAlignment(void* obj, const char* data);
    static bool writeTextVAlignment(void* obj, StringTableEntry pFieldName){ return static_cast<TextSprite*>(obj)->getTextVAlignment() != TextSprite::VALIGN_TOP; }

    static bool setOverflowModeX(void* obj, const char* data);
    static bool writeOverflowModeX(void* obj, StringTableEntry pFieldName){ return static_cast<TextSprite*>(obj)->getOverflowModeX() != TextSprite::OVERFLOW_X_WRAP; }

    static bool setOverflowModeY(void* obj, const char* data);
    static bool writeOverflowModeY(void* obj, StringTableEntry pFieldName){ return static_cast<TextSprite*>(obj)->getOverflowModeY() != TextSprite::OVERFLOW_Y_HIDDEN; }

    static bool writeAutoLineHeight(void* obj, StringTableEntry pFieldName)       { return static_cast<TextSprite*>(obj)->getAutoLineHeight() != true; }

    static bool setCustomLineHeight(void* obj, const char* data)                    { static_cast<TextSprite*>(obj)->setCustomLineHeight(dAtof(data)); return false; }
    static bool writeCustomLineHeight(void* obj, StringTableEntry pFieldName)       { return static_cast<TextSprite*>(obj)->getCustomLineHeight() != 1.0f; }

    static bool setKerning(void* obj, const char* data)                    { static_cast<TextSprite*>(obj)->setKerning(dAtof(data)); return false; }
    static bool writeKerning(void* obj, StringTableEntry pFieldName)       { return static_cast<TextSprite*>(obj)->getKerning() != 0.0f; }

private:
   void RenderLetter(BatchRender* pBatchRenderer, Vector2& cursor, U32 charID, F32 ratio, U32 charNum);
   void ApplyAlignment(Vector2& cursor, U32 totalRows, U32 row, F32 length, U32 charCount, F32 ratio);
   F32 getCursorAdvance(U32 charID, S32 prevCharID, F32 ratio);
   F32 getCursorAdvance(const BitmapFontCharacter& bmChar, S32 prevCharID, F32 ratio);
   F32 GetLineHeight() { return (mAutoLineHeight) ? mFontSize : mCustomLineHeight; }
   void CalculateSpatials(F32 ratio);
};

#endif // _TEXT_SPRITE_H_
