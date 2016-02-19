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

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#include "console/consoleTypes.h"

#include "io/bitStream.h"

#include "string/stringBuffer.h"

#include "TextSprite.h"

// Script bindings.
#include "TextSprite_ScriptBinding.h"

//------------------------------------------------------------------------------

static EnumTable::Enums textAlignmentEnums[] = 
{
    { TextSprite::ALIGN_LEFT,      "Left" },
    { TextSprite::ALIGN_CENTER, "Center" },
    { TextSprite::ALIGN_RIGHT, "Right" },
    { TextSprite::ALIGN_JUSTIFY, "Justify" },
};

static EnumTable gTextAlignmentTable(4, &textAlignmentEnums[0]); 

//-----------------------------------------------------------------------------

TextSprite::TextAlign TextSprite::getTextAlignmentEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(textAlignmentEnums) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(textAlignmentEnums[i].label, label) == 0)
            return (TextAlign)textAlignmentEnums[i].index;
    }

    // Warn.
    Con::warnf("TextSprite::getTextAlignmentEnum() - Invalid text alignment of '%s'", label );

    return TextSprite::INVALID_ALIGN;
}

//-----------------------------------------------------------------------------

const char* TextSprite::getTextAlignmentDescription(const TextSprite::TextAlign alignment)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(textAlignmentEnums) / sizeof(EnumTable::Enums)); i++)
    {
        if( textAlignmentEnums[i].index == alignment )
            return textAlignmentEnums[i].label;
    }

    // Warn.
    Con::warnf( "TextSprite::getTextAlignmentDescription() - Invalid text alignment.");

    return StringTable->EmptyString;
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(TextSprite);

//-----------------------------------------------------------------------------

TextSprite::TextSprite() :
   mFontSize(1.0f),
   mFontScaleX(1.0f),
   mFontScaleY(1.0f),
   mTextAlign(TextSprite::ALIGN_LEFT),
   mTextVAlign(TextSprite::VALIGN_TOP),
   mOverflowX(TextSprite::OVERFLOW_X_WRAP),
   mOverflowY(TextSprite::OVERFLOW_Y_HIDDEN),
   mAutoLineHeight(true),
   mCustomLineHeight(1.0f),
   mKerning(0.0f)
{
}

//-----------------------------------------------------------------------------

TextSprite::~TextSprite()
{
}

//-----------------------------------------------------------------------------

void TextSprite::initPersistFields()
{    
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("font", TypeFontAssetPtr, Offset(mFontAsset, TextSprite), &setFont, &getFont, &writeFont, "");
    addProtectedField("text", TypeString, 0, setText, getText, &writeText, "The text to be displayed." );  
    //addProtectedField("textAlignment", TypeEnum, Offset(mTextAlignment, TextSprite), &setTextAlignment, &defaultProtectedGetFn, &writeTextAlignment, 1, &gTextAlignmentTable, "");
    addProtectedField("fontSize", TypeVector2, Offset(mFontSize, TextSprite), &setFontSize, &defaultProtectedGetFn,&writeFontSize, "" );
    //addProtectedField("fontPadding", TypeF32, Offset(mFontPadding, TextSprite), &setFontPadding, &defaultProtectedGetFn, &writeFontPadding, "" );
}

//-----------------------------------------------------------------------------

bool TextSprite::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;
    
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void TextSprite::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void TextSprite::copyTo(SimObject* object)
{
    // Fetch font object.
    TextSprite* pFontObject = dynamic_cast<TextSprite*>(object);

    // Sanity.
    AssertFatal(pFontObject != NULL, "TextSprite::copyTo() - Object is not the correct type.");

    // Call parent.
    Parent::copyTo(object);

    // Copy.
    pFontObject->setFont(getFont());
    pFontObject->setText(getText());
    pFontObject->setFontSize(getFontSize());
    pFontObject->setFontScaleX(getFontScaleX());
    pFontObject->setFontScaleY(getFontScaleY());
    pFontObject->setTextAlignment(getTextAlignment());
    pFontObject->setTextVAlignment(getTextVAlignment());
    pFontObject->setOverflowModeX(getOverflowModeX());
    pFontObject->setOverflowModeY(getOverflowModeY());
    pFontObject->setAutoLineHeight(getAutoLineHeight());
    pFontObject->setCustomLineHeight(getCustomLineHeight());
    pFontObject->setKerning(getKerning());
}

//------------------------------------------------------------------------------

void TextSprite::scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue )
{
    // Create a default render request.
    Scene::createDefaultRenderRequest( pSceneRenderQueue, this );
}

//------------------------------------------------------------------------------

void TextSprite::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Finish if no font asset.
    if ( mFontAsset.isNull() )
        return;

    // Fetch number of characters to render.
    const U32 renderCharacters = mText.length();

    // Ignore if no text to render.
    if( renderCharacters == 0 )
        return;

    // Get a size ratio
    const F32 ratio = mFontAsset->mBitmapFont.getSizeRatio(mFontSize);

    // Create a cursor
    Vector2 cursor(0.0f, mFontAsset->mBitmapFont.mBaseline * ratio);

    // Render all the characters.    
    for( U32 characterIndex = 0; characterIndex < renderCharacters; ++characterIndex )
    {
        // Fetch character.
        U32 character = mText.getChar( characterIndex );

        RenderLetter(pBatchRenderer, cursor, character, ratio);
    }
}

//-----------------------------------------------------------------------------


void TextSprite::RenderLetter(BatchRender* pBatchRenderer, Vector2& cursor, U32 charID, F32 ratio)
{
   const BitmapFontCharacter bmChar = mFontAsset->mBitmapFont.getCharacter(charID);
   cursor.x -= (bmChar.mXOffset * ratio);

   Vector2 left = (mRenderOOBB[1] - mRenderOOBB[0]);
   left.Normalize((F32)cursor.x + (bmChar.mXOffset * ratio));

   Vector2 right = (mRenderOOBB[1] - mRenderOOBB[0]);
   right.Normalize(bmChar.mWidth * ratio);

   Vector2 top = (mRenderOOBB[3] - mRenderOOBB[0]);
   top.Normalize((F32)cursor.y - (mFontAsset->mBitmapFont.mBaseline * ratio) + (bmChar.mYOffset * ratio));

   Vector2 bottom = (mRenderOOBB[3] - mRenderOOBB[0]);
   bottom.Normalize(bmChar.mHeight * ratio);

   Vector2 destOOBB[4];
   destOOBB[0] = (mRenderOOBB[3] + left - top - bottom);
   destOOBB[1] = (mRenderOOBB[3] + left + right - top - bottom);
   destOOBB[2] = (mRenderOOBB[3] + left + right - top);
   destOOBB[3] = (mRenderOOBB[3] + left - top);

   // Submit batched quad.
   pBatchRenderer->SubmitQuad(
      destOOBB[0],
      destOOBB[1],
      destOOBB[2],
      destOOBB[3],
      bmChar.mOOBB[0],
      bmChar.mOOBB[1],
      bmChar.mOOBB[2],
      bmChar.mOOBB[3],
      mFontAsset->getImageTexture(bmChar.mPage));

   cursor.x += (bmChar.mXAdvance * ratio);
}


//-----------------------------------------------------------------------------

bool TextSprite::setFont( const char* pFontAssetId )
{
    // Set asset.
    mFontAsset = pFontAssetId;

    // Finish if no font asset.
    if ( mFontAsset.isNull() )
        return false;
    
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool TextSprite::setTextAlignment( void* obj, const char* data )
{
    static_cast<TextSprite*>( obj )->setTextAlignment( getTextAlignmentEnum(data) ); return false;
}
