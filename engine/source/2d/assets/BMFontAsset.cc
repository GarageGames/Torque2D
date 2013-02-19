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

#include "console/consoleTypes.h"
#include "2d/assets/ImageAsset.h"
#include "2d/assets/BMFontAsset.h"

//// Script bindings.
//#include "BMFontAsset_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

//------------------------------------------------------------------------------

ConsoleType( BMFontAssetPtr, TypeBMFontAssetPtr, sizeof(AssetPtr<BMFontAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeBMFontAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<BMFontAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeBMFontAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<BMFontAsset>* pAssetPtr = dynamic_cast<AssetPtr<BMFontAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeBMFontAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeBMFontAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(BMFontAsset);

//------------------------------------------------------------------------------

BMFontAsset::BMFontAsset()
{
}

//------------------------------------------------------------------------------

BMFontAsset::~BMFontAsset()
{
}

//------------------------------------------------------------------------------

void BMFontAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Fields.
    addProtectedField("FontFile", TypeAssetLooseFilePath, Offset(mFontFile, BMFontAsset), &setFontFile, &getFontFile, &defaultProtectedWriteFn, "");
}

//------------------------------------------------------------------------------

bool BMFontAsset::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void BMFontAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void BMFontAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();

    mFont = bmFont::create( mFontFile );
}

//------------------------------------------------------------------------------

bool BMFontAsset::isAssetValid( void ) const
{
    return (!mFont.isNull());
}

//------------------------------------------------------------------------------

void BMFontAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    mFont = bmFont::create( mFontFile );
}

void BMFontAsset::setFontFile( const char* pFontFile )
{
    // Sanity!
    AssertFatal( pFontFile != NULL, "Cannot use a NULL font file." );
    
    // Fetch image file.
    pFontFile = StringTable->insert( pFontFile );
    
    // Ignore no change,
    if ( pFontFile == mFontFile )
        return;
    
    // Update.
    mFontFile = getOwned() ? expandAssetFilePath( pFontFile ) : StringTable->insert( pFontFile );
    
    // Refresh the asset.
    refreshAsset();
}



