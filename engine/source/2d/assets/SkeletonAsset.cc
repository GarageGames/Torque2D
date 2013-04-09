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

using namespace spine;

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

SkeletonAsset::SkeletonAsset() :  mSkeletonFile(StringTable->EmptyString),
                               mImageTextureHandle(NULL)
{
}

//------------------------------------------------------------------------------

SkeletonAsset::~SkeletonAsset()
{
	AnimationStateData_dispose(mStateData);
	SkeletonData_dispose(mSkeletonData);
	Atlas_dispose(mAtlas);
}

//------------------------------------------------------------------------------

void SkeletonAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Fields.
    
    addProtectedField("SkeletonFile", TypeAssetLooseFilePath, Offset(mSkeletonFile, SkeletonAsset), &setSkeletonFile, &getSkeletonFile, &defaultProtectedWriteFn, "The loose file produced by the editor, which is fed into this asset");
    addProtectedField("Scale", TypeF32, Offset(mScale, SkeletonAsset), &defaultProtectedSetFn, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
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
    AssertFatal( pSkeletonFile != NULL, "Cannot use a NULL image file." );

    // Fetch image file.
    pSkeletonFile = StringTable->insert( pSkeletonFile );

    // Ignore no change,
    if ( pSkeletonFile == mSkeletonFile )
        return;

    // Update.
    mSkeletonFile = getOwned() ? expandAssetFilePath( pSkeletonFile ) : StringTable->insert( pSkeletonFile );

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void SkeletonAsset::setScale( F32 fScale)
{
    mScale = fScale;

    // Scale has been set, what should happen after this?
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
    pAsset->setSkeletonFile( getSkeletonFile() );
	 // BOZO - Need to copy data loaded from files?
}

//------------------------------------------------------------------------------

void SkeletonAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    // Ensure the image-file is expanded.
    mSkeletonFile = expandAssetFilePath( mSkeletonFile );

    // Initialize any states or data
    // Mich - At this point, this is where we should probably
    // create an ImageAsset, build it based on the data from the .atlas.
    // We can then add it to our AssetDatabase as a private asset
    // This will later be used by the SkeletonObject. That object will
    // create sprites and feed them this ImageAsset
    // If there are multiple .atlas files (skins, right?), we would loop
    // through them all, load them, then convert them to ImageAsset
    mAtlas = Atlas_readAtlasFile("spineboy.atlas"); // BOZO - Use reference to ImageAsset?

    /*
    // Allocate a new ImageAsset. If we have multiple atlases, we would loop this multiple times
    ImageAsset* pImageAsset = new ImageAsset();

    // Optionally set the name
    // pImageAsset->setAssetName("Some_Name");

    // Point to the raw file (png or jpg)
    pImageAsset->setImageFile( imageFilePath);

    // Enable Explicit Mode so we can use region coordinates
    pImageAsset->setExplicitMode( true );
    
    // Loop through the Atlas information to create cell regions
    for (each_each_region_in_atlas)
    {
        // Starting left position of the region
        xOffset = ?;

        // Starting top position of the region
        yOffset = ?;

        // Width of region
        regionWidth = ?;

        // Height of region
        regionHeight = ?;

        // Name of region
        regionName = ?;

        pImageAsset->addExplicitCell( xOffset, yOffset, regionWidth, regionHeight, regionName );
    }

    // Add it to the AssetDatabase, making it accessible everywhere
    mImageAsset = AssetDatabase.addPrivateAsset( pImageAsset );
    */

    // Atlas load failure
    AssertFatal(mAtlas != NULL, "SkeletonAsset::initializeAsset() - Atlas was not loaded.");


    SkeletonJson* json = SkeletonJson_create(mAtlas);
    json->scale = mScale;
    mSkeletonData = SkeletonJson_readSkeletonDataFile(json, mSkeletonFile);

    if (!mSkeletonData)
    {
        Atlas_dispose(mAtlas);
		mAtlas = 0;

        // Report json->error message
        AssertFatal(mSkeletonData != NULL, "SkeletonAsset::initializeAsset() - Skeleton data was not valid.");
    }

	SkeletonJson_dispose(json);

    mStateData = AnimationStateData_create(mSkeletonData);
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

    // Ensure the skeleton file is collapsed.
    mSkeletonFile = collapseAssetFilePath( mSkeletonFile );
}

//-----------------------------------------------------------------------------

void SkeletonAsset::onTamlPostWrite( void )
{
    // Call parent.
    Parent::onTamlPostWrite();

    // Ensure the skeleton file is expanded.
    mSkeletonFile = expandAssetFilePath( mSkeletonFile );
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
