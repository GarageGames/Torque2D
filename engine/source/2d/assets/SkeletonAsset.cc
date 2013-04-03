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

#ifndef _SKELETON_ASSET_H_
#include "2d/assets/SkeletonAsset.h"
#endif

// Script bindings.
#include "SkeletonAsset_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SkeletonAsset);

//------------------------------------------------------------------------------

ConsoleType( skeletonAssetPtr, TypeSkeletonAssetPtr, sizeof(AssetPtr<SkeletonAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeSkeletonAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<SkeletonAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeSkeletonAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<SkeletonAsset>* pAssetPtr = dynamic_cast<AssetPtr<SkeletonAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeSkeletonAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeSkeletonAssetPtr) - Cannot set multiple args to a single asset." );
}


//------------------------------------------------------------------------------

SkeletonAsset::SkeletonAsset() :  mSourceFile(StringTable->EmptyString),
                               mImageTextureHandle(NULL)
{
    int x;
    x = 0;
}

//------------------------------------------------------------------------------

SkeletonAsset::~SkeletonAsset()
{
}

//------------------------------------------------------------------------------

void SkeletonAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Fields.
    addProtectedField("SourceFile", TypeAssetLooseFilePath, Offset(mSourceFile, SkeletonAsset), &setSourceFile, &getSourceFile, &defaultProtectedWriteFn, "");
}

//------------------------------------------------------------------------------

bool SkeletonAsset::onAdd()
{
    // Call Parent.
    if (!Parent::onAdd())
       return false;

    return true;
}

//------------------------------------------------------------------------------

void SkeletonAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void SkeletonAsset::setSourceFile( const char* pSourceFile )
{
    // Sanity!
    AssertFatal( pSourceFile != NULL, "Cannot use a NULL image file." );

    // Fetch image file.
    pSourceFile = StringTable->insert( pSourceFile );

    // Ignore no change,
    if ( pSourceFile == mSourceFile )
        return;

    // Update.
    mSourceFile = getOwned() ? expandAssetFilePath( pSourceFile ) : StringTable->insert( pSourceFile );

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void SkeletonAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    SkeletonAsset* pAsset = static_cast<SkeletonAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "SkeletonAsset::copyTo() - Object is not the correct type.");

    // Copy state.
    pAsset->setSourceFile( getSourceFile() );
}

//------------------------------------------------------------------------------

void SkeletonAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    // Ensure the image-file is expanded.
    mSourceFile = expandAssetFilePath( mSourceFile );

    // Initialize any states or data
}

//------------------------------------------------------------------------------

void SkeletonAsset::onAssetRefresh( void )
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();

    // Reset any states or data

}

//-----------------------------------------------------------------------------

void SkeletonAsset::onTamlPreWrite( void )
{
    // Call parent.
    Parent::onTamlPreWrite();

    // Ensure the source-file is collapsed.
    mSourceFile = collapseAssetFilePath( mSourceFile );
}

//-----------------------------------------------------------------------------

void SkeletonAsset::onTamlPostWrite( void )
{
    // Call parent.
    Parent::onTamlPostWrite();

    // Ensure the image-file is expanded.
    mSourceFile = expandAssetFilePath( mSourceFile );
}

//------------------------------------------------------------------------------

void SkeletonAsset::onTamlCustomWrite( TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(SkeletonAsset_OnTamlCustomWrite);

    // Call parent.
    Parent::onTamlCustomWrite( customNodes );
}

//-----------------------------------------------------------------------------

void SkeletonAsset::onTamlCustomRead( const TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(SkeletonAsset_OnTamlCustomRead);

    // Call parent.
    Parent::onTamlCustomRead( customNodes );
}
