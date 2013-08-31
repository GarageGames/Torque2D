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

#include "ImageFont.h"

// Script bindings.
#include "ImageFont_ScriptBinding.h"

//------------------------------------------------------------------------------

static EnumTable::Enums textAlignmentEnums[] = 
{
    { ImageFont::ALIGN_LEFT,      "Left" },
    { ImageFont::ALIGN_CENTER,    "Center" },
    { ImageFont::ALIGN_RIGHT,     "Right" },
};

static EnumTable gTextAlignmentTable(3, &textAlignmentEnums[0]); 

//-----------------------------------------------------------------------------

ImageFont::TextAlignment ImageFont::getTextAlignmentEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(textAlignmentEnums) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(textAlignmentEnums[i].label, label) == 0)
            return (TextAlignment)textAlignmentEnums[i].index;
    }

    // Warn.
    Con::warnf("ImageFont::getTextAlignmentEnum() - Invalid text alignment of '%s'", label );

    return ImageFont::INVALID_ALIGN;
}

//-----------------------------------------------------------------------------

const char* ImageFont::getTextAlignmentDescription(const ImageFont::TextAlignment alignment)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(textAlignmentEnums) / sizeof(EnumTable::Enums)); i++)
    {
        if( textAlignmentEnums[i].index == alignment )
            return textAlignmentEnums[i].label;
    }

    // Warn.
    Con::warnf( "ImageFont::getTextAlignmentDescription() - Invalid text alignment.");

    return StringTable->EmptyString;
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(ImageFont);

//-----------------------------------------------------------------------------

ImageFont::ImageFont() :
    mTextAlignment( ImageFont::ALIGN_CENTER ),
    mFontSize( 1.0f, 1.0f ),
    mFontPadding( 0 )
{
   // Use a static body by default.
   mBodyDefinition.type = b2_staticBody;

    // Set as auto-sizing.
    mAutoSizing = true;
}

//-----------------------------------------------------------------------------

ImageFont::~ImageFont()
{
}

//-----------------------------------------------------------------------------

void ImageFont::initPersistFields()
{    
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("image", TypeImageAssetPtr, Offset(mImageAsset, ImageFont), &setImage, &getImage, &writeImage, "");
    addProtectedField("text", TypeString, 0, setText, getText, &writeText, "The text to be displayed." );  
    addProtectedField("textAlignment", TypeEnum, Offset(mTextAlignment, ImageFont), &setTextAlignment, &defaultProtectedGetFn, &writeTextAlignment, 1, &gTextAlignmentTable, "");
    addProtectedField("fontSize", TypeVector2, Offset(mFontSize, ImageFont), &setFontSize, &defaultProtectedGetFn,&writeFontSize, "" );
    addProtectedField("fontPadding", TypeF32, Offset(mFontPadding, ImageFont), &setFontPadding, &defaultProtectedGetFn, &writeFontPadding, "" );
}

//-----------------------------------------------------------------------------

bool ImageFont::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;
    
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void ImageFont::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void ImageFont::copyTo(SimObject* object)
{
    // Fetch font object.
    ImageFont* pFontObject = dynamic_cast<ImageFont*>(object);

    // Sanity.
    AssertFatal(pFontObject != NULL, "ImageFont::copyTo() - Object is not the correct type.");

    // Call parent.
    Parent::copyTo(object);

    // Copy.
    pFontObject->setImage( getImage() );
    pFontObject->setText( getText() );
    pFontObject->setTextAlignment( getTextAlignment() );
    pFontObject->setFontSize( getFontSize() );
    pFontObject->setFontPadding( getFontPadding() );
}

//------------------------------------------------------------------------------

void ImageFont::scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue )
{
    // Create a default render request.
    Scene::createDefaultRenderRequest( pSceneRenderQueue, this );
}

//------------------------------------------------------------------------------

void ImageFont::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Finish if no image asset.
    if ( mImageAsset.isNull() )
        return;

    // Fetch number of characters to render.
    const U32 renderCharacters = mText.length();

    // Ignore if no text to render.
    if( renderCharacters == 0 )
        return;

    // Fetch render OOBB.
    const Vector2& renderOOBB0 = mRenderOOBB[0];
    const Vector2& renderOOBB1 = mRenderOOBB[1];
    const Vector2& renderOOBB3 = mRenderOOBB[3];

    Vector2 characterOOBB0;
    Vector2 characterOOBB1;
    Vector2 characterOOBB2;
    Vector2 characterOOBB3;

    // Calculate the starting render position based upon text alignment.
    switch( mTextAlignment )
    {
        case ALIGN_LEFT:
            {
                // Size is twice the padded text width as we're aligning to the left from the position expanding rightwards.
                characterOOBB0.Set( (renderOOBB0.x + renderOOBB1.x)*0.5f, renderOOBB0.y );
            }
            break;

        case ALIGN_RIGHT:
            {
                // Size is twice the padded text width as we're aligning to the right from the position expanding leftwards.
                characterOOBB0 = renderOOBB0;
            }
            break;

        default:
            {
                // Warn.
                Con::warnf("ImageFont() - Unknown text alignment!");
            }
        case ALIGN_CENTER:
            {
                // Size is the total padded text size as we're simply centered on the position.
                characterOOBB0 = renderOOBB0;
            }
            break;
    }

    // Calculate character width stride.
    Vector2 characterWidthStride = (renderOOBB1 - renderOOBB0);
    characterWidthStride.Normalize( mFontSize.x + mFontPadding );

    // Calculate character height stride.
    Vector2 characterHeightStride = (renderOOBB3 - renderOOBB0);
    characterHeightStride.Normalize( mFontSize.y );

    // Complete character OOBB.
    characterOOBB1 = characterOOBB0 + characterWidthStride;
    characterOOBB2 = characterOOBB1 + characterHeightStride;
    characterOOBB3 = characterOOBB2 - characterWidthStride;

    // Render all the characters.    
    for( U32 characterIndex = 0; characterIndex < renderCharacters; ++characterIndex )
    {
        // Fetch character.
        U32 character = mText.getChar( characterIndex );

        // Set character to "space" if it's out of bounds.
        if ( character < 32 || character > 128 )
            character = 32;

        // Calculate character frame index.
        const U32 characterFrameIndex = character - 32;

        // Fetch current frame area.
        const ImageAsset::FrameArea::TexelArea& texelFrameArea = mImageAsset->getImageFrameArea( characterFrameIndex ).mTexelArea;

        // Fetch lower/upper texture coordinates.
        const Vector2& texLower = texelFrameArea.mTexelLower;
        const Vector2& texUpper = texelFrameArea.mTexelUpper;

        // Submit batched quad.
        pBatchRenderer->SubmitQuad(
            characterOOBB0,
            characterOOBB1,
            characterOOBB2,
            characterOOBB3,
            Vector2( texLower.x, texUpper.y ),
            Vector2( texUpper.x, texUpper.y ),
            Vector2( texUpper.x, texLower.y ),
            Vector2( texLower.x, texLower.y ),
            mImageAsset->getImageTexture() );

        // Translate character OOBB.
        characterOOBB0 += characterWidthStride;
        characterOOBB1 += characterWidthStride;
        characterOOBB2 += characterWidthStride;
        characterOOBB3 += characterWidthStride;
    }
}


//-----------------------------------------------------------------------------

bool ImageFont::setImage( const char* pImageAssetId )
{
    // Set asset.
    mImageAsset = pImageAssetId;

    // Finish if no image asset.
    if ( mImageAsset.isNull() )
        return false;

    // We need a minimum of 96 frames here.
    if ( mImageAsset->getFrameCount() < 96 )
    {
        // Warn.
        Con::warnf("ImageFont::setImage() - The image needs to have at least 96 frames to be used as a font! (%s)", mImageAsset.getAssetId() );
        mImageAsset.clear();
        return false;
    }
    
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void ImageFont::setText( const StringBuffer& text )
{
    // Set text.
    mText.set( &text );
    calculateSpatials();   
}

//-----------------------------------------------------------------------------

void ImageFont::setTextAlignment( const TextAlignment alignment )
{
    mTextAlignment = alignment;
    calculateSpatials();
}

//-----------------------------------------------------------------------------

void ImageFont::setFontSize( const Vector2& size )
{
    mFontSize = size;
    mFontSize.clampZero();
    calculateSpatials();
}

//-----------------------------------------------------------------------------

void ImageFont::setFontPadding( const F32 padding )
{
    mFontPadding = padding;
    calculateSpatials();
}

//-----------------------------------------------------------------------------

void ImageFont::calculateSpatials( void )
{
    // Fetch number of characters to render.
    const U32 renderCharacters = mText.length();

    // Set size as a single character if no text.
    if ( renderCharacters == 0 )
    {
        setSize( mFontSize );
        return;
    }

    // Calculate total font padding.
    const F32 totalFontPadding = (renderCharacters * mFontPadding) - mFontPadding;

    // Calculate total character size.
    const Vector2 totalFontSize( renderCharacters * mFontSize.x, mFontSize.y );

    // Calculate total padded text size.
    const Vector2 totalPaddedTextSize( totalFontSize.x + totalFontPadding, totalFontSize.y );

    // Calculate size (AABB) including alignment relative to position.
    // NOTE:    For left/right alignment we have to double the size width as clipping is based upon size and
    //          we're aligning to the position here.  We cannot align to the size AABB itself as that changes
    //          as the text length changes therefore it is not a stable point from which to align from.
    switch( mTextAlignment )
    {
        case ALIGN_LEFT:
            {
                // Size is twice the padded text width as we're aligning to the left from the position expanding rightwards.
                setSize( totalPaddedTextSize.x * 2.0f, totalPaddedTextSize.y );
            }
            break;

        case ALIGN_RIGHT:
            {
                // Size is twice the padded text width as we're aligning to the right from the position expanding leftwards.
                setSize( totalPaddedTextSize.x * 2.0f, totalPaddedTextSize.y );
            }
            break;

        case ALIGN_CENTER:
            {
                // Size is the total padded text size as we're simply centered on the position.
                setSize( totalPaddedTextSize );
            }
            break;

        default:
            {
                // Warn.
                Con::warnf("ImageFont() - Unknown text alignment!");
            }
    }
}

//-----------------------------------------------------------------------------

bool ImageFont::setTextAlignment( void* obj, const char* data )
{
    static_cast<ImageFont*>( obj )->setTextAlignment( getTextAlignmentEnum(data) ); return false;
}
