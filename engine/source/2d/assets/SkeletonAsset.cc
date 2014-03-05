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
        if (pAssetPtr == NULL )
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

SkeletonAsset::SkeletonAsset() :    mSkeletonFile(StringTable->EmptyString),
                                    mAtlasFile(StringTable->EmptyString),
                                    mAtlasDirty(true),
                                    mAtlas(NULL),
                                    mSkeletonData(NULL),
                                    mStateData(NULL)
{
}

//------------------------------------------------------------------------------

SkeletonAsset::~SkeletonAsset()
{
    spAnimationStateData_dispose(mStateData);
    spSkeletonData_dispose(mSkeletonData);
    spAtlas_dispose(mAtlas);
}

//------------------------------------------------------------------------------

void SkeletonAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Fields.
    addProtectedField("AtlasFile", TypeAssetLooseFilePath, Offset(mAtlasFile, SkeletonAsset), &setAtlasFile, &defaultProtectedGetFn, &writeAtlasFile, "The loose file pointing to the .atlas file used for skinning");
    addProtectedField("SkeletonFile", TypeAssetLooseFilePath, Offset(mSkeletonFile, SkeletonAsset), &setSkeletonFile, &defaultProtectedGetFn, &writeSkeletonFile, "The loose file produced by the editor, which is fed into this asset");
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

void SkeletonAsset::setSkeletonFile( const char* pSkeletonFile )
{
    // Sanity!
    AssertFatal( pSkeletonFile != NULL, "Cannot use a NULL skeleton file." );

    // Fetch skeleton file.
    pSkeletonFile = StringTable->insert( pSkeletonFile );

    // Ignore no change.
    if (pSkeletonFile == mSkeletonFile )
        return;

    // Update.
    mSkeletonFile = getOwned() ? expandAssetFilePath( pSkeletonFile ) : StringTable->insert( pSkeletonFile );

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void SkeletonAsset::setAtlasFile( const char* pAtlasFile )
{
    // Sanity!
    AssertFatal( pAtlasFile != NULL, "Cannot use a NULL atlas file." );

    // Fetch atlas file.
    pAtlasFile = StringTable->insert( pAtlasFile );

    // Ignore no change.
    if (pAtlasFile == mAtlasFile )
        return;

    // Update.
    mAtlasFile = getOwned() ? expandAssetFilePath( pAtlasFile ) : StringTable->insert( pAtlasFile );
    mAtlasDirty = true;

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
    pAsset->setAtlasFile( getAtlasFile() );
    pAsset->setSkeletonFile( getSkeletonFile() );
}

//------------------------------------------------------------------------------

void SkeletonAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    // Ensure the skeleton file is expanded.
    mSkeletonFile = expandAssetFilePath( mSkeletonFile );

    // Ensure the skeleton file is expanded.
    mAtlasFile = expandAssetFilePath( mAtlasFile );

    // Build the atlas data
    if (mAtlasDirty)
        buildAtlasData();

    // Build the skeleton data
    buildSkeletonData();
}

//------------------------------------------------------------------------------

void SkeletonAsset::onAssetRefresh( void )
{
    // Ignore if not yet added to the sim.
    if (!isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();

    // Reset any states or data
    if (mAtlasDirty)
        buildAtlasData();

    buildSkeletonData();
}

//-----------------------------------------------------------------------------

void SkeletonAsset::buildAtlasData( void )
{
    // If the atlas data was previously created, need to release it
    if (mAtlas)
        spAtlas_dispose(mAtlas);

    // If we are using a .atlas file
    if (mAtlasFile != StringTable->EmptyString)
        mAtlas = spAtlas_readAtlasFile(mAtlasFile);

    // Atlas load failure
    AssertFatal(mAtlas != NULL, "SkeletonAsset::buildAtlasData() - Atlas was not loaded.");

    spAtlasPage* currentPage = mAtlas->pages;

    while (currentPage != NULL)
    {
        // Allocate a new ImageAsset. If we have multiple atlases, we would loop this multiple times
        ImageAsset* pImageAsset = new ImageAsset();

        const char* imageFilePath = expandAssetFilePath(currentPage->name);
        
        // Point to the raw file (png or jpg)
        pImageAsset->setImageFile( imageFilePath);        

        // Enable Explicit Mode so we can use region coordinates
        pImageAsset->setExplicitMode( true );

        spAtlasRegion* currentRegion = mAtlas->regions;

        // Add it to the AssetDatabase, making it accessible everywhere
        mImageAsset = AssetDatabase.addPrivateAsset( pImageAsset );

        // Loop through the Atlas information to create cell regions
        while (currentRegion != NULL)
        {
            pImageAsset->addExplicitCell( currentRegion->x, currentRegion->y, currentRegion->width, currentRegion->height, currentRegion->name );

            currentRegion = currentRegion->next;
        }

        mImageAsset->forceCalculation();
        currentPage = currentPage->next;
    }

    mAtlasDirty = false;
}

//-----------------------------------------------------------------------------

void SkeletonAsset::buildSkeletonData( void )
{
    // Atlas load failure
    AssertFatal(mAtlas != NULL, "SkeletonAsset::buildSkeletonData() - Atlas was not loaded.");
    
    // Clear state data
    if (mStateData)
        spAnimationStateData_dispose(mStateData);

    // Clear skeleton data
    if (mSkeletonData)
        spSkeletonData_dispose(mSkeletonData);
    
    spSkeletonJson* json = spSkeletonJson_create(mAtlas);
    mSkeletonData = spSkeletonJson_readSkeletonDataFile(json, mSkeletonFile);

    if (!mSkeletonData)
    {
        spAtlas_dispose(mAtlas);
        mAtlas = 0;

        // Report json->error message
        AssertFatal(mSkeletonData != NULL, "SkeletonAsset::buildSkeletonData() - Skeleton data was not valid.");
    }

    spSkeletonJson_dispose(json);

    mStateData = spAnimationStateData_create(mSkeletonData);
}

//-----------------------------------------------------------------------------

bool SkeletonAsset::isAssetValid( void ) const
{
    return ((mAtlas != NULL) && (mSkeletonData != NULL) && (mStateData != NULL) && mImageAsset.notNull());
}

//-----------------------------------------------------------------------------

void SkeletonAsset::onTamlPreWrite( void )
{
    // Call parent.
    Parent::onTamlPreWrite();

    // Ensure the skeleton file is collapsed.
    mSkeletonFile = collapseAssetFilePath( mSkeletonFile );

    // Ensure the atlas file is collapsed.
    mAtlasFile = collapseAssetFilePath( mAtlasFile );
}

//-----------------------------------------------------------------------------

void SkeletonAsset::onTamlPostWrite( void )
{
    // Call parent.
    Parent::onTamlPostWrite();

    // Ensure the skeleton file is expanded.
    mSkeletonFile = expandAssetFilePath( mSkeletonFile );

    // Ensure the atlas file is expanded.
    mAtlasFile = expandAssetFilePath( mAtlasFile );
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
