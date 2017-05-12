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

//-----------------------------------------------------------------------------
// Text Alignment
//-----------------------------------------------------------------------------

bool TextSprite::setTextAlignment(void* obj, const char* data)
{
   static_cast<TextSprite*>(obj)->setTextAlignment(getTextAlignmentEnum(data)); return false;
}

//------------------------------------------------------------------------------

static EnumTable::Enums textAlignmentEnums[] =
{
   { TextSprite::ALIGN_LEFT, "Left" },
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
      if (dStricmp(textAlignmentEnums[i].label, label) == 0)
         return (TextAlign)textAlignmentEnums[i].index;
   }

   // Warn.
   Con::warnf("TextSprite::getTextAlignmentEnum() - Invalid text alignment of '%s'", label);

   return TextSprite::INVALID_ALIGN;
}

//-----------------------------------------------------------------------------

const char* TextSprite::getTextAlignmentDescription(const TextSprite::TextAlign alignment)
{
   // Search for Mnemonic.
   for (U32 i = 0; i < (sizeof(textAlignmentEnums) / sizeof(EnumTable::Enums)); i++)
   {
      if (textAlignmentEnums[i].index == alignment)
         return textAlignmentEnums[i].label;
   }

   // Warn.
   Con::warnf("TextSprite::getTextAlignmentDescription() - Invalid text alignment.");

   return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------
// Verticle Text Alignment
//-----------------------------------------------------------------------------

bool TextSprite::setTextVAlignment(void* obj, const char* data)
{
   static_cast<TextSprite*>(obj)->setTextVAlignment(getTextVAlignmentEnum(data)); return false;
}

//------------------------------------------------------------------------------

static EnumTable::Enums textVAlignmentEnums[] =
{
   { TextSprite::VALIGN_TOP, "Top" },
   { TextSprite::VALIGN_MIDDLE, "Middle" },
   { TextSprite::VALIGN_BOTTOM, "Bottom" },
};

static EnumTable gTextVAlignmentTable(3, &textVAlignmentEnums[0]);

//-----------------------------------------------------------------------------

TextSprite::TextVAlign TextSprite::getTextVAlignmentEnum(const char* label)
{
   // Search for Mnemonic.
   for (U32 i = 0; i < (sizeof(textVAlignmentEnums) / sizeof(EnumTable::Enums)); i++)
   {
      if (dStricmp(textVAlignmentEnums[i].label, label) == 0)
         return (TextVAlign)textVAlignmentEnums[i].index;
   }

   // Warn.
   Con::warnf("TextSprite::getTextVAlignmentEnum() - Invalid vertical text alignment of '%s'", label);

   return TextSprite::INVALID_VALIGN;
}

//-----------------------------------------------------------------------------

const char* TextSprite::getTextVAlignmentDescription(const TextSprite::TextVAlign alignment)
{
   // Search for Mnemonic.
   for (U32 i = 0; i < (sizeof(textVAlignmentEnums) / sizeof(EnumTable::Enums)); i++)
   {
      if (textVAlignmentEnums[i].index == alignment)
         return textVAlignmentEnums[i].label;
   }

   // Warn.
   Con::warnf("TextSprite::getTextVAlignmentDescription() - Invalid vertical text alignment.");

   return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------
// Overflow Mode X
//-----------------------------------------------------------------------------

bool TextSprite::setOverflowModeX(void* obj, const char* data)
{
   static_cast<TextSprite*>(obj)->setOverflowModeX(getOverflowModeXEnum(data)); return false;
}

//------------------------------------------------------------------------------

static EnumTable::Enums overflowModeXEnums[] =
{
   { TextSprite::OVERFLOW_X_WRAP, "Wrap" },
   { TextSprite::OVERFLOW_X_VISIBLE, "Visible" },
   { TextSprite::OVERFLOW_X_HIDDEN, "Hidden" },
   { TextSprite::OVERFLOW_X_SHRINK, "Shrink" },
};

static EnumTable gOverflowModeXTable(4, &overflowModeXEnums[0]);

//-----------------------------------------------------------------------------

TextSprite::OverflowModeX TextSprite::getOverflowModeXEnum(const char* label)
{
   // Search for Mnemonic.
   for (U32 i = 0; i < (sizeof(overflowModeXEnums) / sizeof(EnumTable::Enums)); i++)
   {
      if (dStricmp(overflowModeXEnums[i].label, label) == 0)
         return (OverflowModeX)overflowModeXEnums[i].index;
   }

   // Warn.
   Con::warnf("TextSprite::getOverflowModeXEnum() - Invalid overflow mode X of '%s'", label);

   return TextSprite::INVALID_OVERFLOW_X;
}

//-----------------------------------------------------------------------------

const char* TextSprite::getOverflowModeXDescription(const TextSprite::OverflowModeX modeX)
{
   // Search for Mnemonic.
   for (U32 i = 0; i < (sizeof(overflowModeXEnums) / sizeof(EnumTable::Enums)); i++)
   {
      if (overflowModeXEnums[i].index == modeX)
         return overflowModeXEnums[i].label;
   }

   // Warn.
   Con::warnf("TextSprite::getOverflowModeXDescription() - Invalid overflow mode X.");

   return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------
// Overflow Mode Y
//-----------------------------------------------------------------------------

bool TextSprite::setOverflowModeY(void* obj, const char* data)
{
   static_cast<TextSprite*>(obj)->setOverflowModeY(getOverflowModeYEnum(data)); return false;
}

//------------------------------------------------------------------------------

static EnumTable::Enums overflowModeYEnums[] =
{
   { TextSprite::OVERFLOW_Y_VISIBLE, "Visible" },
   { TextSprite::OVERFLOW_Y_HIDDEN, "Hidden" },
   { TextSprite::OVERFLOW_Y_SHRINK, "Shrink" },
};

static EnumTable gOverflowModeYTable(3, &overflowModeYEnums[0]);

//-----------------------------------------------------------------------------

TextSprite::OverflowModeY TextSprite::getOverflowModeYEnum(const char* label)
{
   // Search for Mnemonic.
   for (U32 i = 0; i < (sizeof(overflowModeYEnums) / sizeof(EnumTable::Enums)); i++)
   {
      if (dStricmp(overflowModeYEnums[i].label, label) == 0)
         return (OverflowModeY)overflowModeYEnums[i].index;
   }

   // Warn.
   Con::warnf("TextSprite::getOverflowModeYEnum() - Invalid overflow mode Y of '%s'", label);

   return TextSprite::INVALID_OVERFLOW_Y;
}

//-----------------------------------------------------------------------------

const char* TextSprite::getOverflowModeYDescription(const TextSprite::OverflowModeY modeY)
{
   // Search for Mnemonic.
   for (U32 i = 0; i < (sizeof(overflowModeYEnums) / sizeof(EnumTable::Enums)); i++)
   {
      if (overflowModeYEnums[i].index == modeY)
         return overflowModeYEnums[i].label;
   }

   // Warn.
   Con::warnf("TextSprite::getOverflowModeYDescription() - Invalid overflow mode Y.");

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
   mKerning(0.0f),
   mFontSpatialsDirty(true),
   mCalculatedSize(1.0f, 1.0f)
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
    addProtectedField("text", TypeString, 0, setText, getText, &writeText, "The text to be displayed.");
    addProtectedField("fontSize", TypeVector2, Offset(mFontSize, TextSprite), &setFontSize, &defaultProtectedGetFn, &writeFontSize, "");
    addProtectedField("fontScaleX", TypeF32, Offset(mFontScaleX, TextSprite), &setFontScaleX, &defaultProtectedGetFn, &writeFontScaleX, "");
    addProtectedField("fontScaleY", TypeF32, Offset(mFontScaleY, TextSprite), &setFontScaleY, &defaultProtectedGetFn, &writeFontScaleY, "");
    addProtectedField("textAlignment", TypeEnum, Offset(mTextAlign, TextSprite), &setTextAlignment, &defaultProtectedGetFn, &writeTextAlignment, 1, &gTextAlignmentTable, "");
    addProtectedField("textVAlignment", TypeEnum, Offset(mTextVAlign, TextSprite), &setTextVAlignment, &defaultProtectedGetFn, &writeTextVAlignment, 1, &gTextVAlignmentTable, "");
    addProtectedField("overflowModeX", TypeEnum, Offset(mOverflowX, TextSprite), &setOverflowModeX, &defaultProtectedGetFn, &writeOverflowModeX, 1, &gOverflowModeXTable, "");
    addProtectedField("overflowModeY", TypeEnum, Offset(mOverflowY, TextSprite), &setOverflowModeY, &defaultProtectedGetFn, &writeOverflowModeY, 1, &gOverflowModeYTable, "");
    addField("autoLineHeight", TypeBool, Offset(mAutoLineHeight, TextSprite), &writeAutoLineHeight, "");
    addProtectedField("customLineHeight", TypeF32, Offset(mCustomLineHeight, TextSprite), &setCustomLineHeight, &defaultProtectedGetFn, &writeCustomLineHeight, "");
    addProtectedField("kerning", TypeF32, Offset(mKerning, TextSprite), &setKerning, &defaultProtectedGetFn, &writeKerning, "");
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
    pFontObject->mCharInfo = mCharInfo;
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

    //Make sure none of our settings are invalid
    if (mTextAlign == INVALID_ALIGN)
    {
       mTextAlign = ALIGN_LEFT;
    }

    if (mTextVAlign == INVALID_VALIGN)
    {
       mTextVAlign = VALIGN_TOP;
    }

    if (mOverflowX == INVALID_OVERFLOW_X)
    {
       mOverflowX = OVERFLOW_X_WRAP;
    }

    if (mOverflowY == INVALID_OVERFLOW_Y)
    {
       mOverflowY = OVERFLOW_Y_HIDDEN;
    }

    // Get a size ratio
    const F32 ratio = mFontAsset->mBitmapFont.getSizeRatio(mFontSize);

    //get the line height
    const F32 lineHeight = GetLineHeight();

    //prep for justify
    if (mTextAlign == ALIGN_JUSTIFY)
    {
       mKerning = 0;
    }

    //determine the rows
    if (mFontSpatialsDirty || mSize != mCalculatedSize)
    {
       CalculateSpatials(ratio);
    }

    // If we're shrinking the set the scale
    bool shrinkX = false;
    bool shrinkY = false;
    F32 origScaleX, origScaleY;
    F32 origLengthX;
    if (mOverflowX == OVERFLOW_X_SHRINK && mLine.front().mLength > mSize.x)
    {
       shrinkX = true;
       origScaleX = mFontScaleX;
       mFontScaleX = mSize.x / mLine.front().mLength;
       origLengthX = mLine.front().mLength;
       mLine.front().mLength = mSize.x;
    }
    if (mOverflowY == OVERFLOW_Y_SHRINK && (mLine.size() * lineHeight * mFontScaleY) > mSize.y)
    {
       shrinkY = true;
       origScaleY = mFontScaleY;
       mFontScaleY = mSize.y / (mLine.size() * lineHeight);
    }

    // Create a cursor
    Vector2 cursor(0.0f, 0.0f);

    ApplyAlignment(cursor, mLine.size(), 0, mLine.front().mLength, mLine.front().mEnd - mLine.front().mStart + 1, ratio);

    // Render all the characters.  
    U32 row = 0;
    S32 prevCharID = -1;
    for (U32 characterIndex = mLine.front().mStart; characterIndex < renderCharacters; ++characterIndex)
    {
        // Fetch character.
        U32 charID = mText.getChar( characterIndex );

        if (characterIndex >= mLine[row].mStart)
        {
           RenderLetter(pBatchRenderer, cursor, charID, ratio, characterIndex);
        }
        
        if ((row + 1) < mLine.size() && mLine[row + 1].mStart == (characterIndex + 1))
        {
           row++;
           cursor.x = 0;
           prevCharID = -1;
           ApplyAlignment(cursor, mLine.size(), row, mLine[row].mLength, mLine[row].mEnd - mLine[row].mStart + 1, ratio);
        }
        else
        {
           cursor.x += getCursorAdvance(charID, prevCharID, ratio);
           prevCharID = charID;
        }
    }

    //clean up for justify
    if (mTextAlign == ALIGN_JUSTIFY)
    {
       mKerning = 0;
    }

    //clean up scale
    if (mOverflowX == OVERFLOW_X_SHRINK && shrinkX)
    {
       mFontScaleX = origScaleX;
       mLine.front().mLength = origLengthX;
    }
    if (mOverflowY == OVERFLOW_Y_SHRINK && shrinkY)
    {
       mFontScaleY = origScaleY;
    }
}

//-----------------------------------------------------------------------------


void TextSprite::RenderLetter(BatchRender* pBatchRenderer, Vector2& cursor, U32 charID, F32 ratio, U32 charNum)
{
   const BitmapFontCharacter& bmChar = mFontAsset->mBitmapFont.getCharacter(charID);

   Vector2 charScale = getCharacterScale(charNum);
   Vector2 charOffset = getCharacterOffset(charNum);
   ColorF charColor = getCharacterBlendColor(charNum);

   F32 fontScaleX = mFontScaleX * charScale.x;
   F32 fontScaleY = mFontScaleY * charScale.y;

   F32 cursorX = cursor.x + charOffset.x - (bmChar.mWidth * ratio * mFontScaleX * ((charScale.x - 1) / 2));
   F32 cursorY = cursor.y - charOffset.y;

   //inset, for hiding part of a letter
   F32 insetLeft, insetRight, insetTop, insetBottom;
   insetLeft = 0;
   insetRight = 0;
   insetTop = 0;
   insetBottom = 0;

   // Cropping time!
   if (mOverflowX == OVERFLOW_X_HIDDEN) 
   {
      if ((cursorX + (bmChar.mWidth * ratio * fontScaleX) <= 0) || (cursorX >= mSize.x))
         return;

      if (cursorX < 0 && (cursorX + (bmChar.mWidth * ratio * fontScaleX) > 0))
      {
         insetLeft = -(cursorX / (bmChar.mWidth * ratio * fontScaleX));
      }

      if (cursorX < mSize.x && (cursorX + (bmChar.mWidth * ratio * fontScaleX) > mSize.x))
      {
         insetRight = (((cursorX + (bmChar.mWidth * ratio * fontScaleX)) - mSize.x) / (bmChar.mWidth * ratio * fontScaleX));
      }

      if (insetLeft + insetRight > 1)
         return;
   }
   if (mOverflowY == OVERFLOW_Y_HIDDEN)
   {
      F32 tempTop, tempBottom;
      tempTop = cursorY - (mFontAsset->mBitmapFont.mBaseline * ratio * fontScaleY) + (bmChar.mYOffset * ratio * fontScaleY);
      tempBottom = tempTop + (bmChar.mHeight * ratio * fontScaleY);

      if ((tempBottom <= 0) || (tempTop >= mSize.y))
         return;

      if (tempTop < 0 && tempBottom > 0)
      {
         insetTop = -(tempTop / (bmChar.mHeight * ratio * fontScaleY));
      }

      if (tempTop < mSize.y && tempBottom > mSize.y)
      {
         insetBottom = (tempBottom - mSize.y) / (bmChar.mHeight * ratio * fontScaleY);
      }

      if (insetTop + insetBottom > 1)
         return;
   }

   //create the source rect
   Vector2 sourceOOBB[4];
   sourceOOBB[0].Set(bmChar.mOOBB[0].x + ((insetLeft * (F32)bmChar.mWidth) / (F32)bmChar.mPageWidth), bmChar.mOOBB[0].y - ((insetBottom * (F32)bmChar.mHeight) / (F32)bmChar.mPageHeight));
   sourceOOBB[1].Set(bmChar.mOOBB[1].x - ((insetRight * (F32)bmChar.mWidth) / (F32)bmChar.mPageWidth), bmChar.mOOBB[1].y - ((insetBottom * (F32)bmChar.mHeight) / (F32)bmChar.mPageHeight));
   sourceOOBB[2].Set(bmChar.mOOBB[2].x - ((insetRight * (F32)bmChar.mWidth) / (F32)bmChar.mPageWidth), bmChar.mOOBB[2].y + ((insetTop * (F32)bmChar.mHeight) / (F32)bmChar.mPageHeight));
   sourceOOBB[3].Set(bmChar.mOOBB[3].x + ((insetLeft * (F32)bmChar.mWidth) / (F32)bmChar.mPageWidth), bmChar.mOOBB[3].y + ((insetTop * (F32)bmChar.mHeight) / (F32)bmChar.mPageHeight));

   //create the destination rect
   Vector2 destLeft = (mRenderOOBB[1] - mRenderOOBB[0]);
   destLeft.Normalize((F32)cursorX + (insetLeft * bmChar.mWidth * ratio * fontScaleX));

   Vector2 destWidth = (mRenderOOBB[1] - mRenderOOBB[0]);
   destWidth.Normalize(((bmChar.mWidth * ratio) - (insetLeft * (bmChar.mWidth * ratio)) - (insetRight * (bmChar.mWidth * ratio))) * fontScaleX);

   Vector2 destTop = -(mRenderOOBB[3] - mRenderOOBB[0]);
   destTop.Normalize((F32)cursorY - (((mFontAsset->mBitmapFont.mBaseline * ratio) - (bmChar.mYOffset * ratio) - (insetTop * bmChar.mHeight * ratio)) * fontScaleY));

   Vector2 destHeight = -(mRenderOOBB[3] - mRenderOOBB[0]);
   destHeight.Normalize(((bmChar.mHeight * ratio) - (insetBottom * bmChar.mHeight * ratio) - (insetTop * bmChar.mHeight * ratio)) * fontScaleY);

   Vector2 destOOBB[4];
   destOOBB[0] = (mRenderOOBB[3] + destLeft + destTop + destHeight);
   destOOBB[1] = (mRenderOOBB[3] + destLeft + destWidth + destTop + destHeight);
   destOOBB[2] = (mRenderOOBB[3] + destLeft + destWidth + destTop);
   destOOBB[3] = (mRenderOOBB[3] + destLeft + destTop);

   if (charColor != mBlendColor)
   {
      // Submit batched quad.
      pBatchRenderer->SubmitQuad(
         destOOBB[0],
         destOOBB[1],
         destOOBB[2],
         destOOBB[3],
         sourceOOBB[0],
         sourceOOBB[1],
         sourceOOBB[2],
         sourceOOBB[3],
         mFontAsset->getImageTexture(bmChar.mPage),
         charColor);
   }
   else
   {
      // Submit batched quad.
      pBatchRenderer->SubmitQuad(
         destOOBB[0],
         destOOBB[1],
         destOOBB[2],
         destOOBB[3],
         sourceOOBB[0],
         sourceOOBB[1],
         sourceOOBB[2],
         sourceOOBB[3],
         mFontAsset->getImageTexture(bmChar.mPage));
   }
}

//-----------------------------------------------------------------------------

void TextSprite::CalculateSpatials(F32 ratio)
{
   F32 length = 0;
   S32 start = -1;
   S32 wordEnd = 0;
   F32 wordEndLength = 0;
   S32 prevCharID = -1;
   mLine.clear();
   const U32 renderCharacters = mText.length();

   for (U32 i = 0; i < renderCharacters; i++)
   {
      U32 charID = mText.getChar(i);
      const BitmapFontCharacter& bmChar = mFontAsset->mBitmapFont.getCharacter(charID);

      if (start == -1 && charID != 32)
      {
         start = i;
         length = 0;
         mLine.push_back(BitmapFontLineInfo(start));
      }

      if (i == start)
      {
         length -= (bmChar.mXOffset * ratio * mFontScaleX);
      }

      length += getCursorAdvance(bmChar, prevCharID, ratio);

      if (mOverflowX == OVERFLOW_X_WRAP)
      {
         if (prevCharID != 32 && prevCharID != -1 && charID == 32)
         {
            wordEnd = i - 1;
            wordEndLength = length - getCursorAdvance(bmChar, prevCharID, ratio);
         }

         if (length > mSize.x && wordEnd > start && start != -1)
         {
            mLine.back().mEnd = wordEnd;
            U32 endCharID = mText.getChar(wordEnd);
            const BitmapFontCharacter& bmCharEnd = mFontAsset->mBitmapFont.getCharacter(endCharID);
            i = wordEnd;

            start = -1;
            length = 0;

            S32 prevEndCharID = -1;
            if (wordEnd != 0)
            {
               prevEndCharID = mText.getChar(wordEnd - 1);
            }

            wordEndLength += -getCursorAdvance(bmCharEnd, prevEndCharID, ratio) + (bmCharEnd.mWidth * ratio * mFontScaleX) + (bmCharEnd.mXOffset * ratio * mFontScaleX);
            mLine.back().mLength = wordEndLength;
         }
      }

      prevCharID = charID;

      if ((i + 1) == renderCharacters)
      {
         if (charID != 32)//this is the last character and if it's not a space then it's the end of a word.
         {
            wordEndLength = length;
            wordEnd = i;
         }
         mLine.back().mEnd = wordEnd;
         U32 endCharID = mText.getChar(wordEnd);
         const BitmapFontCharacter& bmCharEnd = mFontAsset->mBitmapFont.getCharacter(endCharID);

         S32 prevEndCharID = -1;
         if (wordEnd != 0)
         {
            prevEndCharID = mText.getChar(wordEnd - 1);
         }

         wordEndLength += -getCursorAdvance(bmCharEnd, prevEndCharID, ratio) + (bmCharEnd.mWidth * ratio * mFontScaleX) + (bmCharEnd.mXOffset * ratio * mFontScaleX);
         mLine.back().mLength = wordEndLength;
      }
   }

   mFontSpatialsDirty = false;
   mCalculatedSize = mSize;
}

//-----------------------------------------------------------------------------

void TextSprite::ApplyAlignment(Vector2& cursor, U32 totalRows, U32 row, F32 length, U32 charCount, F32 ratio)
{
   //Horizontal Align
   if (mTextAlign == ALIGN_CENTER)
   {
      cursor.x += ((mSize.x - length) / 2.0f);
   }
   else if (mTextAlign == ALIGN_RIGHT)
   {
      cursor.x += (mSize.x - length);
   }
   else if (mTextAlign == ALIGN_JUSTIFY)
   {
      U32 gapCount = charCount - 1;
      mKerning = (-(mSize.x - length) / gapCount) / mFontScaleX;
   }

   //Vertical Align
   const F32 lineHeight = GetLineHeight();
   if (mTextVAlign == VALIGN_TOP)
   {
      cursor.y = ((mFontAsset->mBitmapFont.mBaseline * ratio) + (lineHeight * row)) * mFontScaleY;
   }
   else if (mTextVAlign == VALIGN_MIDDLE)
   {
      cursor.y = (mSize.y / 2.0f) - ((((lineHeight / 2.0f) * totalRows) - (lineHeight * row) - (mFontAsset->mBitmapFont.mBaseline * ratio)) * mFontScaleY);
   }
   else if (mTextVAlign == VALIGN_BOTTOM)
   {
      cursor.y = mSize.y - (((lineHeight * totalRows) - (lineHeight * row) - (mFontAsset->mBitmapFont.mBaseline * ratio)) * mFontScaleY);
   }
}


//-----------------------------------------------------------------------------

F32 TextSprite::getCursorAdvance(U32 charID, S32 prevCharID, F32 ratio)
{
   const BitmapFontCharacter& bmChar = mFontAsset->mBitmapFont.getCharacter(charID);
   return getCursorAdvance(bmChar, prevCharID, ratio);
}

F32 TextSprite::getCursorAdvance(const BitmapFontCharacter& bmChar, S32 prevCharID, F32 ratio)
{
   if (prevCharID != -1)
   {
      S16 kerning = mFontAsset->mBitmapFont.getKerning(prevCharID, bmChar.mCharID);
      return (((bmChar.mXAdvance - kerning) * ratio) - mKerning) * mFontScaleX;
   }
   else
   {
      return ((bmChar.mXAdvance * ratio) - mKerning) * mFontScaleX;
   }
}


//-----------------------------------------------------------------------------

bool TextSprite::setFont( const char* pFontAssetId )
{
    // Set asset.
    mFontAsset = pFontAssetId;

    // Finish if no font asset.
    if ( mFontAsset.isNull() )
        return false;

    mFontSpatialsDirty = true;
    
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void TextSprite::setCharacterBlendColor(const U32 charNum, const ColorF color)
{
   if (mCharInfo.find(charNum) != mCharInfo.end())
   {
      mCharInfo[charNum].mColor = color;
      mCharInfo[charNum].mUseColor = true;
   }
   else
   {
      mCharInfo[charNum] = BitmapFontCharacterInfo(color);
   }
}

//-----------------------------------------------------------------------------

ColorF TextSprite::getCharacterBlendColor(const U32 charNum)
{
   if (mCharInfo.find(charNum) != mCharInfo.end())
   {
      return mCharInfo[charNum].mColor;
   }
   else
   {
      return mBlendColor;
   }
}

//-----------------------------------------------------------------------------

bool TextSprite::getCharacterHasBlendColor(const U32 charNum)
{
   return (mCharInfo.find(charNum) != mCharInfo.end() && mCharInfo[charNum].mUseColor);
}

//-----------------------------------------------------------------------------

void TextSprite::resetCharacterBlendColor(const U32 charNum)
{
   if (mCharInfo.find(charNum) != mCharInfo.end())
   {
      mCharInfo[charNum].mUseColor = false;

      if (mCharInfo[charNum].isDefault())
      {
         mCharInfo.erase(charNum);
      }
   }
}

//-----------------------------------------------------------------------------

void TextSprite::setCharacterScale(const U32 charNum, const F32 scaleX, const F32 scaleY)
{
   if (mCharInfo.find(charNum) == mCharInfo.end())
   {
      mCharInfo[charNum] = BitmapFontCharacterInfo();
   }
   mCharInfo[charNum].mScaleX = scaleX;
   mCharInfo[charNum].mScaleY = scaleY;

   if (mCharInfo[charNum].isDefault())
   {
      mCharInfo.erase(charNum);
   }
}

//-----------------------------------------------------------------------------

Vector2 TextSprite::getCharacterScale(const U32 charNum)
{
   if (mCharInfo.find(charNum) != mCharInfo.end())
   {
      return Vector2(mCharInfo[charNum].mScaleX, mCharInfo[charNum].mScaleY);
   }
   else
   {
      return Vector2(1.0f, 1.0f);
   }
}

//-----------------------------------------------------------------------------

void TextSprite::resetCharacterScale(const U32 charNum)
{
   if (mCharInfo.find(charNum) != mCharInfo.end())
   {
      mCharInfo[charNum].mScaleX = 1.0f;
      mCharInfo[charNum].mScaleY = 1.0f;

      if (mCharInfo[charNum].isDefault())
      {
         mCharInfo.erase(charNum);
      }
   }
}

//-----------------------------------------------------------------------------

void TextSprite::setCharacterOffset(const U32 charNum, const F32 offsetX, const F32 offsetY)
{
   if (mCharInfo.find(charNum) == mCharInfo.end())
   {
      mCharInfo[charNum] = BitmapFontCharacterInfo();
   }
   mCharInfo[charNum].mOffsetX = offsetX;
   mCharInfo[charNum].mOffsetY = offsetY;

   if (mCharInfo[charNum].isDefault())
   {
      mCharInfo.erase(charNum);
   }
}

//-----------------------------------------------------------------------------

Vector2 TextSprite::getCharacterOffset(const U32 charNum)
{
   if (mCharInfo.find(charNum) != mCharInfo.end())
   {
      return Vector2(mCharInfo[charNum].mOffsetX, mCharInfo[charNum].mOffsetY);
   }
   else
   {
      return Vector2(0.0f, 0.0f);
   }
}

//-----------------------------------------------------------------------------

void TextSprite::resetCharacterOffset(const U32 charNum)
{
   if (mCharInfo.find(charNum) != mCharInfo.end())
   {
      mCharInfo[charNum].mOffsetX = 0.0f;
      mCharInfo[charNum].mOffsetY = 0.0f;

      if (mCharInfo[charNum].isDefault())
      {
         mCharInfo.erase(charNum);
      }
   }
}