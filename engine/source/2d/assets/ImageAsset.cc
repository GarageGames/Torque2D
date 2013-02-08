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

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _GBITMAP_H_
#include "graphics/gBitmap.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

// Script bindings.
#include "ImageAsset_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

//------------------------------------------------------------------------------

ConsoleType( imageAssetPtr, TypeImageAssetPtr, sizeof(AssetPtr<ImageAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeImageAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<ImageAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeImageAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<ImageAsset>* pAssetPtr = dynamic_cast<AssetPtr<ImageAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeImageAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeImageAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(ImageAsset);

//------------------------------------------------------------------------------

static bool explicitCellPropertiesInitialized = false;

static StringTableEntry cellCustomPropertyName;
static StringTableEntry cellAliasName;
static StringTableEntry cellOffsetName;
static StringTableEntry cellWidthName;
static StringTableEntry cellHeightName;

//------------------------------------------------------------------------------

static EnumTable::Enums textureFilterLookup[] =
                {
                { ImageAsset::FILTER_NEAREST,     "NEAREST"     },
                { ImageAsset::FILTER_BILINEAR,    "BILINEAR"    },
                };

EnumTable textureFilterTable(sizeof(textureFilterLookup) / sizeof(EnumTable::Enums), &textureFilterLookup[0]);

//------------------------------------------------------------------------------

ImageAsset::TextureFilterMode ImageAsset::getFilterModeEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(textureFilterLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(textureFilterLookup[i].label, label) == 0)
            return((ImageAsset::TextureFilterMode)textureFilterLookup[i].index);

    // Warn.
    Con::warnf("ImageAsset::getFilterModeEnum() - Invalid filter-mode '%s'", label );

    return ImageAsset::FILTER_INVALID;
}

//------------------------------------------------------------------------------

const char* ImageAsset::getFilterModeDescription( ImageAsset::TextureFilterMode filterMode )
{
    // Search for Mode.
    for(U32 i = 0; i < (sizeof(textureFilterLookup) / sizeof(EnumTable::Enums)); i++)
        if( textureFilterLookup[i].index == filterMode )
            return textureFilterLookup[i].label;

    // Warn.
    Con::warnf("ImageAsset::getFilterModeDescription() - Invalid filter-mode." );

    return StringTable->EmptyString;
}

//------------------------------------------------------------------------------

ImageAsset::ImageAsset() :  mImageFile(StringTable->EmptyString),
                            mForce16Bit(false),
                            mLocalFilterMode(FILTER_INVALID),
                            mExplicitMode(false),

                            mCellRowOrder(true),
                            mCellOffsetX(0),
                            mCellOffsetY(0),
                            mCellStrideX(0),
                            mCellStrideY(0),
                            mCellCountX(0),
                            mCellCountY(0),
                            mCellWidth(0),
                            mCellHeight(0),

                            mImageTextureHandle(NULL)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mFrames );
    VECTOR_SET_ASSOCIATION( mExplicitFrames );

    // Set the default filter mode.    
    setFilterMode( FILTER_BILINEAR );

    // Initialize explicit cell field names.
    if ( !explicitCellPropertiesInitialized )
    {
        cellCustomPropertyName      = StringTable->insert( "Cells" );
        cellAliasName               = StringTable->insert( "Cell" );
        cellOffsetName              = StringTable->insert( "Offset" );
        cellWidthName               = StringTable->insert( "Width" );
        cellHeightName              = StringTable->insert( "Height" );

        // Flag as initialized.
        explicitCellPropertiesInitialized = true;
    }
}

//------------------------------------------------------------------------------

ImageAsset::~ImageAsset()
{
}

//------------------------------------------------------------------------------

void ImageAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Fields.
    addProtectedField("ImageFile", TypeAssetLooseFilePath, Offset(mImageFile, ImageAsset), &setImageFile, &getImageFile, &defaultProtectedWriteFn, "");
    addProtectedField("Force16bit", TypeBool, Offset(mForce16Bit, ImageAsset), &setForce16Bit, &defaultProtectedGetFn, &writeForce16Bit, "");
    addProtectedField("FilterMode", TypeEnum, Offset(mLocalFilterMode, ImageAsset), &setFilterMode, &defaultProtectedGetFn, &writeFilterMode, 1, &textureFilterTable);   
    addProtectedField("ExplicitMode", TypeBool, Offset(mExplicitMode, ImageAsset), &setExplicitMode, &defaultProtectedGetFn, &defaultProtectedNotWriteFn, "");

    addProtectedField("CellRowOrder", TypeBool, Offset(mCellRowOrder, ImageAsset), &setCellRowOrder, &defaultProtectedGetFn, &writeCellRowOrder, "");
    addProtectedField("CellOffsetX", TypeS32, Offset(mCellOffsetX, ImageAsset), &setCellOffsetX, &defaultProtectedGetFn, &writeCellOffsetX, "");
    addProtectedField("CellOffsetY", TypeS32, Offset(mCellOffsetY, ImageAsset), &setCellOffsetY, &defaultProtectedGetFn, &writeCellOffsetY, "");
    addProtectedField("CellStrideX", TypeS32, Offset(mCellStrideX, ImageAsset), &setCellStrideX, &defaultProtectedGetFn, &writeCellStrideX, "");
    addProtectedField("CellStrideY", TypeS32, Offset(mCellStrideY, ImageAsset), &setCellStrideY, &defaultProtectedGetFn, &writeCellStrideY, "");
    addProtectedField("CellCountX", TypeS32, Offset(mCellCountX, ImageAsset), &setCellCountX, &defaultProtectedGetFn, &writeCellCountX, "");
    addProtectedField("CellCountY", TypeS32, Offset(mCellCountY, ImageAsset), &setCellCountY, &defaultProtectedGetFn, &writeCellCountY, "");
    addProtectedField("CellWidth", TypeS32, Offset(mCellWidth, ImageAsset), &setCellWidth, &defaultProtectedGetFn, &writeCellWidth, "");
    addProtectedField("CellHeight", TypeS32, Offset(mCellHeight, ImageAsset), &setCellHeight, &defaultProtectedGetFn, &writeCellHeight, "");
}

//------------------------------------------------------------------------------

bool ImageAsset::onAdd()
{
    // Call Parent.
    if (!Parent::onAdd())
       return false;

    return true;
}

//------------------------------------------------------------------------------

void ImageAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void ImageAsset::setImageFile( const char* pImageFile )
{
    // Sanity!
    AssertFatal( pImageFile != NULL, "Cannot use a NULL image file." );

    // Fetch image file.
    pImageFile = StringTable->insert( pImageFile );

    // Ignore no change,
    if ( pImageFile == mImageFile )
        return;

    // Update.
    mImageFile = getOwned() ? expandAssetFilePath( pImageFile ) : StringTable->insert( pImageFile );

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setForce16Bit( const bool force16Bit )
{
    // Ignore no change,
    if ( force16Bit == mForce16Bit )
        return;

    // Update.
    mForce16Bit = force16Bit;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setFilterMode( const ImageAsset::TextureFilterMode filterMode )
{
    // Ignore no change,
    if ( filterMode == mLocalFilterMode )
        return;

    // Invalid filter mode?
    if ( filterMode == FILTER_INVALID )
    {
        // Yes, so warn.
        Con::warnf( "Cannot set an invalid filter mode." );
        return;
    }

    // Update.
    mLocalFilterMode = filterMode;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setExplicitMode( const bool explicitMode )
{
    // Ignore no change,
    if ( explicitMode == mExplicitMode )
        return;

    // Update.
    mExplicitMode = explicitMode;

    // Refresh the asset.
    refreshAsset();
}    

//------------------------------------------------------------------------------

void ImageAsset::setCellRowOrder( const bool cellRowOrder )
{
    // Ignore no change.
    if ( cellRowOrder == mCellRowOrder )
        return;

    // Update.
    mCellRowOrder = cellRowOrder;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellOffsetX( const S32 cellOffsetX )
{
    // Ignore no change.
    if ( cellOffsetX == mCellOffsetX )
        return;

    // Valid?
    if ( cellOffsetX < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL offset X '%d'.", cellOffsetX );
        return;
    }

    // Update.
    mCellOffsetX = cellOffsetX;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellOffsetY( const S32 cellOffsetY )
{
    // Ignore no change.
    if ( cellOffsetY == mCellOffsetY )
        return;

    // Valid?
    if ( cellOffsetY < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL offset Y '%d'.", cellOffsetY );
        return;
    }

    // Update.
    mCellOffsetY = cellOffsetY;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellStrideX( const S32 cellStrideX )
{
    // Ignore no change.
    if ( cellStrideX == mCellStrideX )
        return;

    // Valid?
    if ( cellStrideX < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL stride X '%d'.", cellStrideX );
        return;
    }

    // Update.
    mCellStrideX = cellStrideX;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellStrideY( const S32 cellStrideY )
{
    // Ignore no change.
    if ( cellStrideY == mCellStrideY )
        return;

    // Valid?
    if ( cellStrideY < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL stride Y '%d'.", cellStrideY );
        return;
    }

    // Update.
    mCellStrideY = cellStrideY;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellCountX( const S32 cellCountX )
{
    // Ignore no change.
    if ( cellCountX == mCellCountX )
        return;

    // Valid?
    if ( cellCountX < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL count X '%d'.", cellCountX );
        return;
    }

    // Update.
    mCellCountX = cellCountX;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellCountY( const S32 cellCountY )
{
    // Ignore no change.
    if ( cellCountY == mCellCountY )
        return;

    // Valid?
    if ( cellCountY < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL count Y '%d'.", cellCountY );
        return;
    }

    // Update.
    mCellCountY = cellCountY;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellWidth( const S32 cellWidth )
{
    // Ignore no change.
    if ( cellWidth == mCellWidth )
        return;

    // Valid?
    if ( cellWidth < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL width '%d'.", cellWidth );
        return;
    }

    // Update.
    mCellWidth = cellWidth;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellHeight( const S32 cellheight )
{
    // Ignore no change.
    if ( cellheight == mCellHeight )
        return;

    // Valid?
    if ( cellheight < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL height '%d'.", cellheight );
        return;
    }

    // Update.
    mCellHeight = cellheight;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

bool ImageAsset::clearExplicitCells( void )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Clear explicit frames.
    mExplicitFrames.clear();

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::addExplicitCell( const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::addCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // The Cell Offset X needs to be within the image.
    if ( cellOffsetX < 0 || cellOffsetX >= imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::addCell() - Invalid Cell OffsetX of %d.", cellOffsetX );
        return false;
    }

    // The Cell Offset Y needs to be within the image.
    if ( cellOffsetY < 0 || cellOffsetY >= imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::addCell() - Invalid Cell OffsetY of %d.", cellOffsetY );
        return false;
    }

    // The Cell Width needs to be within the image.
    if ( cellWidth <= 0 || (cellOffsetX+cellWidth) > imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::addCell() - Invalid Cell Width of %d.", cellWidth );
        return false;
    }

    // The Cell Height needs to be within the image.
    if ( cellHeight <= 0 || (cellOffsetY+cellHeight) > imageHeight )
    {
        // Warn.
        Con::warnf("ImageAsset::addCell() - Invalid Cell Width of %d.", cellHeight );
        return false;
    }

    // Store frame.
    FrameArea::PixelArea pixelArea( cellOffsetX, cellOffsetY, cellWidth, cellHeight );
    mExplicitFrames.push_back( pixelArea );

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::insertExplicitCell( const S32 cellIndex, const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::insertCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // Fetch the explicit frame count.
    const S32 explicitFramelCount = mExplicitFrames.size();

    // The cell index needs to be in range.
    if ( cellIndex < 0 )
    {
        // Warn.
        Con::warnf("ImageAsset::insertCell() - Invalid Cell Index of %d.", cellIndex );
        return false;
    }

    // The Cell Offset X needs to be within the image.
    if ( cellOffsetX < 0 || cellOffsetX >= imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::insertCell() - Invalid Cell OffsetX of %d.", cellOffsetX );
        return false;
    }

    // The Cell Offset Y needs to be within the image.
    if ( cellOffsetY < 0 || cellOffsetY >= imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::insertCell() - Invalid Cell OffsetY of %d.", cellOffsetY );
        return false;
    }

    // The Cell Width needs to be within the image.
    if ( cellWidth <= 0 || (cellOffsetX+cellWidth) > imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::insertCell() - Invalid Cell Width of %d.", cellWidth );
        return false;
    }

    // The Cell Height needs to be within the image.
    if ( cellHeight <= 0 || (cellOffsetY+cellHeight) > imageHeight )
    {
        // Warn.
        Con::warnf("ImageAsset::insertCell() - Invalid Cell Width of %d.", cellHeight );
        return false;
    }

    // Configure frame.
    FrameArea::PixelArea pixelArea( cellOffsetX, cellOffsetY, cellWidth, cellHeight );

    // Insert frame appropriately.
    if ( cellIndex >= explicitFramelCount )
    {
        mExplicitFrames.push_back( pixelArea );
    }
    else
    {
        mExplicitFrames.insert( cellIndex );
        mExplicitFrames[cellIndex] = pixelArea;
    }

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::setExplicitCell( const S32 cellIndex, const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::setCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // Fetch the explicit frame count.
    const S32 explicitFrameCount = mExplicitFrames.size();

    // The cell index needs to be in range.
    if ( cellIndex < 0 || cellIndex >= explicitFrameCount )
    {
        // Warn.
        Con::warnf("ImageAsset::setCell() - Invalid Cell Index of %d.", cellIndex );
        return false;
    }

    // The Cell Offset X needs to be within the image.
    if ( cellOffsetX < 0 || cellOffsetX >= imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::setCell() - Invalid Cell OffsetX of %d.", cellOffsetX );
        return false;
    }

    // The Cell Offset Y needs to be within the image.
    if ( cellOffsetY < 0 || cellOffsetY >= imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::setCell() - Invalid Cell OffsetY of %d.", cellOffsetY );
        return false;
    }

    // The Cell Width needs to be within the image.
    if ( cellWidth <= 0 || (cellOffsetX+cellWidth) > imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::setCell() - Invalid Cell Width of %d.", cellWidth );
        return false;
    }

    // The Cell Height needs to be within the image.
    if ( cellHeight <= 0 || (cellOffsetY+cellHeight) > imageHeight )
    {
        // Warn.
        Con::warnf("ImageAsset::setCell() - Invalid Cell Width of %d.", cellHeight );
        return false;
    }

    // Configure frame.
    FrameArea::PixelArea pixelArea( cellOffsetX, cellOffsetY, cellWidth, cellHeight );

    // Set cell.
    mExplicitFrames[cellIndex] = pixelArea;

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::removeExplicitCell( const S32 cellIndex )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::removeCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Fetch the explicit frame count.
    const S32 explicitFrameCount = mExplicitFrames.size();

    // The cell index needs to be in range.
    if ( cellIndex < 0 || cellIndex >= explicitFrameCount )
    {
        // Warn.
        Con::warnf("ImageAsset::removeCell() - Invalid Cell Index of %d.", cellIndex );
        return false;
    }

    // Remove cell.
    mExplicitFrames.erase(cellIndex);

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

void ImageAsset::setTextureFilter( const TextureFilterMode filterMode )
{
    // Finish if no texture.
    if ( mImageTextureHandle.IsNull() )
        return;

    // Select Hardware Filter Mode.
    GLint glFilterMode;

    switch( filterMode )
    {
        // Nearest ("none").
        case FILTER_NEAREST:
        {
            glFilterMode = GL_NEAREST;

        } break;

        // Bilinear ("smooth").
        case FILTER_BILINEAR:
        {
            glFilterMode = GL_LINEAR;

        } break;

        // Huh?
        default:
            // Oh well...
            glFilterMode = GL_LINEAR;
    };

    // Set the texture objects filter mode.
    mImageTextureHandle.setFilter( glFilterMode );
}

//------------------------------------------------------------------------------

void ImageAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    // Ensure the image-file is expanded.
    mImageFile = expandAssetFilePath( mImageFile );

    // Calculate the image.
    calculateImage();
}

//------------------------------------------------------------------------------

void ImageAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();
    
    // Compile image.
    calculateImage();
}

//-----------------------------------------------------------------------------

void ImageAsset::onTamlPreWrite( void )
{
    // Call parent.
    Parent::onTamlPreWrite();

    // Ensure the image-file is collapsed.
    mImageFile = collapseAssetFilePath( mImageFile );
}

//-----------------------------------------------------------------------------

void ImageAsset::onTamlPostWrite( void )
{
    // Call parent.
    Parent::onTamlPostWrite();

    // Ensure the image-file is expanded.
    mImageFile = expandAssetFilePath( mImageFile );
}

//------------------------------------------------------------------------------

void ImageAsset::onTamlCustomWrite( TamlCustomProperties& customProperties )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_OnTamlCustomWrite);

    // Call parent.
    Parent::onTamlCustomWrite( customProperties );

    // Finish if not in explicit mode.
    if ( !mExplicitMode )
        return;

    // Add cell custom property.
    TamlCustomProperty* pCellProperty = customProperties.addProperty( cellCustomPropertyName );

    // Iterate explicit frames.
    for( typeExplicitFrameAreaVector::iterator frameItr = mExplicitFrames.begin(); frameItr != mExplicitFrames.end(); ++frameItr )
    {
        // Fetch pixel area.
        const FrameArea::PixelArea& pixelArea = *frameItr;

        // Add cell alias.
        TamlPropertyAlias* pCellAlias = pCellProperty->addAlias( cellAliasName );

        // Add cell properties.
        pCellAlias->addField( cellOffsetName, pixelArea.mPixelOffset );
        pCellAlias->addField( cellWidthName, pixelArea.mPixelWidth );
        pCellAlias->addField( cellHeightName, pixelArea.mPixelHeight );
    }
}

//-----------------------------------------------------------------------------

void ImageAsset::onTamlCustomRead( const TamlCustomProperties& customProperties )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_OnTamlCustomRead);

    // Call parent.
    Parent::onTamlCustomRead( customProperties );

    // Find cell custom property
    const TamlCustomProperty* pCellProperty = customProperties.findProperty( cellCustomPropertyName );

    // Finish if we don't have explicit cells.
    if ( pCellProperty == NULL )
        return;

    // Set explicit mode.
    mExplicitMode = true;

    // Iterate cells.
    for( TamlCustomProperty::const_iterator propertyAliasItr = pCellProperty->begin(); propertyAliasItr != pCellProperty->end(); ++propertyAliasItr )
    {
        // Fetch property alias.
        TamlPropertyAlias* pPropertyAlias = *propertyAliasItr;

        // Fetch alias name.
        StringTableEntry aliasName = pPropertyAlias->mAliasName;

        // Is this a valid alias?
        if ( aliasName != cellAliasName )
        {
            // No, so warn.
            Con::warnf( "ImageAsset::onTamlCustomRead() - Encountered an unknown custom alias name of '%s'.  Only '%s' is valid.", aliasName, cellAliasName );
            continue;
        }

        Point2I cellOffset(-1, -1);
        S32 cellWidth = 0;
        S32 cellHeight = 0;

        // Iterate property fields.
        for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
        {
            // Fetch property field.
            TamlPropertyField* pPropertyField = *propertyFieldItr;

            // Fetch property field name.
            StringTableEntry fieldName = pPropertyField->getFieldName();

            // Check common fields.
            if ( fieldName == cellOffsetName )
            {
                pPropertyField->getFieldValue( cellOffset );
            }
            else if ( fieldName == cellWidthName )
            {
                pPropertyField->getFieldValue( cellWidth );
            }
            else if ( fieldName == cellHeightName )
            {
                pPropertyField->getFieldValue( cellHeight );
            }
            else
            {
                // Unknown name so warn.
                Con::warnf( "ImageAsset::onTamlCustomRead() - Encountered an unknown custom field name of '%s'.", fieldName );
                continue;
            }
        }

        // Is cell offset valid?
        if ( cellOffset.x < 0 || cellOffset.y < 0 )
        {
            // No, so warn.
            Con::warnf( "ImageAsset::onTamlCustomRead() - Cell offset of '(%d,%d)' is invalid or was not set.", cellOffset.x, cellOffset.y );
            continue;
        }

        // Is cell width valid?
        if ( cellWidth <= 0 )
        {
            // No, so warn.
            Con::warnf( "ImageAsset::onTamlCustomRead() - Cell width of '%d' is invalid or was not set.", cellWidth );
            continue;
        }

        // Is cell height valid?
        if ( cellHeight <= 0 )
        {
            // No, so warn.
            Con::warnf( "ImageAsset::onTamlCustomRead() - Cell height of '%d' is invalid or was not set.", cellHeight );
            continue;
        }

        // Add explicit frame.
        FrameArea::PixelArea pixelArea( cellOffset.x, cellOffset.y, cellWidth, cellHeight );
        mExplicitFrames.push_back( pixelArea );
    }
}

//------------------------------------------------------------------------------

void ImageAsset::calculateImage( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_CalculateImage);

    // Clear frames.
    mFrames.clear();

    // Get image texture.
    mImageTextureHandle.set( mImageFile, TextureHandle::BitmapTexture, true, getForce16Bit() );

    // Is the texture valid?
    if ( mImageTextureHandle.IsNull() )
    {
        // No, so warn.
        Con::warnf( "Image '%s' could not load texture '%s'.", getAssetId(), mImageFile );
        return;
    }

    // Fetch global filter.
    const char* pGlobalFilter = Con::getVariable( "$pref::T2D::imageAssetGlobalFilterMode" );

    // Fetch global filter mode.
    TextureFilterMode filterMode;

    // Set the filter mode.
    if ( pGlobalFilter != NULL && dStrlen(pGlobalFilter) > 0 )
        filterMode = getFilterModeEnum( pGlobalFilter );
    else
        filterMode = FILTER_INVALID;
    
    // If global filter mode is invalid then use local filter mode.
    if ( filterMode == FILTER_INVALID )
        filterMode = mLocalFilterMode;

    // Set filter mode.
    if ( filterMode != FILTER_INVALID )
    {
        // Set filter mode if valid.
        setTextureFilter( filterMode );
    }
    else
    {
        // Set to nearest if invalid.
        setTextureFilter( FILTER_NEAREST );
    }

    // Calculate according to mode.
    if ( mExplicitMode )
    {
        calculateExplicitMode();
    }
    else
    {
        calculateImplicitMode();
    }
}

//------------------------------------------------------------------------------

void ImageAsset::calculateImplicitMode( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_CalculateImplicitMode);

    // Sanity!
    AssertFatal( !mExplicitMode, "Cannot calculate implicit cells when in explicit mode." );

    // Fetch the texture object.
    TextureObject* pTextureObject = ((TextureObject*)mImageTextureHandle);
 
    // Calculate texel scales.
    const F32 texelWidthScale = 1.0f / (F32)pTextureObject->getTextureWidth();
    const F32 texelHeightScale = 1.0f / (F32)pTextureObject->getTextureHeight();

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // Set full-frame as default.
    FrameArea frameArea( 0, 0, imageWidth, imageHeight, texelWidthScale, texelHeightScale );
    mFrames.push_back( frameArea );

    // Finish if no cell counts are specified.  This is how we default to full-frame mode.
    if ( mCellCountX < 1 || mCellCountY < 1 )
        return;

    // The cell width needs to be at maximum the image width!
    if ( mCellWidth < 1 || mCellWidth > imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::calculateImage() - Invalid Cell Width of %d.", mCellWidth );
        return;
    }

    // The cell height needs to be at maximum the image height!
    if ( mCellHeight < 1 || mCellHeight > imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::calculateImage() - Invalid Cell Height of %d.", mCellHeight );
        return;
    }

    // The Cell Offset X needs to be within the image.
    if ( mCellOffsetX < 0 || mCellOffsetX >= imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::calculateImage() - Invalid Cell OffsetX of %d.", mCellOffsetX );
        return;
    }

    // The Cell Offset Y needs to be within the image.
    if ( mCellOffsetY < 0 || mCellOffsetY >= imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::calculateImage() - Invalid Cell OffsetY of %d.", mCellOffsetY );
        return;
    }


    // Are we using Cell-StrideX?
    S32 cellStepX;
    if ( mCellStrideX != 0 )
    {
        // Yes, so set stepX to be StrideX.
        cellStepX = mCellStrideX;
    }
    else
    {
        // No, so set stepY to be Cell Width.
        cellStepX = mCellWidth;
    }

    // Are we using Cell-StrideY?
    S32 cellStepY;
    if ( mCellStrideY != 0 )
    {
        // Yes, so set stepY to be StrideY.
        cellStepY = mCellStrideY;
    }
    else
    {
        // No, so set stepY to be Cell Height.
        cellStepY = mCellHeight;
    }

    // Calculate Final Cell Position X.
    S32 cellFinalPositionX = mCellOffsetX + ((mCellCountX-((cellStepX<0)?1:0))*cellStepX);
    // Off Left?
    if ( cellFinalPositionX < 0 )
    {
        // Warn.
        Con::warnf("ImageAsset::calculateImage() - Invalid Cell OffsetX(%d)/Width(%d)/CountX(%d); off image left-hand-side.", mCellOffsetX, mCellWidth, mCellCountX );
        return;
    }
    // Off Right?
    else if ( cellFinalPositionX > imageWidth )
    {
        // Warn.
        Con::warnf("ImageAsset::calculateImage() - Invalid Cell OffsetX(%d)/Width(%d)/CountX(%d); off image right-hand-side.", mCellOffsetX, mCellWidth, mCellCountX );
        return;
    }

    // Calculate Final Cell Position Y.
    S32 cellFinalPositionY = mCellOffsetY + ((mCellCountY-((cellStepY<0)?1:0))*cellStepY);
    // Off Top?
    if ( cellFinalPositionY < 0 )
    {
        // Warn.
        Con::warnf("ImageAsset::calculateImage() - Invalid Cell OffsetY(%d)/Height(%d)/CountY(%d); off image top-side.", mCellOffsetY, mCellHeight, mCellCountY );
        return;
    }
    // Off Bottom?
    else if ( cellFinalPositionY > imageHeight )
    {
        // Warn.
        Con::warnf("ImageAsset::calculateImage() - Invalid Cell OffsetY(%d)/Height(%d)/CountY(%d); off image bottom-side.", mCellOffsetY, mCellHeight, mCellCountY );
        return;
    }

    // Clear default frame.
    mFrames.clear();

    // Cell Row Order?
    if ( mCellRowOrder )
    {
        // Yes, so RowRow Order.
        for ( S32 y = 0, cellPositionY = mCellOffsetY; y < mCellCountY; y++, cellPositionY+=cellStepY )
        {
            for ( S32 x = 0, cellPositionX = mCellOffsetX; x < mCellCountX; x++, cellPositionX+=cellStepX )
            {
                // Set frame area.
                frameArea.setArea( cellPositionX, cellPositionY, mCellWidth, mCellHeight, texelWidthScale, texelHeightScale );

                // Store fame.
                mFrames.push_back( frameArea );
            }
        }

        return;
    }

    // No, so Column Order.
    for ( S32 x = 0, cellPositionX = mCellOffsetX; x < mCellCountX; x++, cellPositionX+=cellStepX )
    {
        for ( S32 y = 0, cellPositionY = mCellOffsetY; y < mCellCountY; y++, cellPositionY+=cellStepY )
        {
            // Set frame area.
            frameArea.setArea( cellPositionX, cellPositionY, mCellWidth, mCellHeight, texelWidthScale, texelHeightScale );

            // Store fame.
            mFrames.push_back( frameArea );
        }
    }
}

//------------------------------------------------------------------------------

void ImageAsset::calculateExplicitMode( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_CalculateExplicitMode);

    // Sanity!
    AssertFatal( mExplicitMode, "Cannot calculate explicit cells when not in explicit mode." );

    // Fetch the texture object.
    TextureObject* pTextureObject = ((TextureObject*)mImageTextureHandle);
 
    // Calculate texel scales.
    const F32 texelWidthScale = 1.0f / (F32)pTextureObject->getTextureWidth();
    const F32 texelHeightScale = 1.0f / (F32)pTextureObject->getTextureHeight();

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // Clear default frame.
    mFrames.clear();

    // Are any explicit frames set.
    if ( mExplicitFrames.size() == 0 )
    {
        // No, so set full-frame as default.
        FrameArea frameArea( 0, 0, imageWidth, imageHeight, texelWidthScale, texelHeightScale );
        mFrames.push_back( frameArea );

        return;
    }

    // Iterate explicit frames.
    for( typeExplicitFrameAreaVector::iterator frameItr = mExplicitFrames.begin(); frameItr != mExplicitFrames.end(); ++frameItr )
    {
        // Fetch pixel area.
        const FrameArea::PixelArea& pixelArea = *frameItr;

        // Set frame area.
        FrameArea frameArea( pixelArea.mPixelOffset.x, pixelArea.mPixelOffset.y, pixelArea.mPixelWidth, pixelArea.mPixelHeight, texelWidthScale, texelHeightScale );

        // Store frame.
        mFrames.push_back( frameArea );
    }
}

//------------------------------------------------------------------------------

bool ImageAsset::setFilterMode( void* obj, const char* data )
{
    static_cast<ImageAsset*>(obj)->setFilterMode(getFilterModeEnum(data));
    return false;
}
