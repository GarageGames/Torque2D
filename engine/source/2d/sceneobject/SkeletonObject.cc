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

#ifndef _SKELETON_OBJECT_H_
#include "2d/sceneobject/SkeletonObject.h"
#endif

// Script bindings.
#include "2d/sceneobject/SkeletonObject_ScriptBinding.h"

using namespace spine;

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SkeletonObject);

//------------------------------------------------------------------------------

SkeletonObject::SkeletonObject() :
mPreTickTime( 0.0f ),
mPostTickTime( 0.0f )
{
    
}

//------------------------------------------------------------------------------

SkeletonObject::~SkeletonObject()
{
}

//------------------------------------------------------------------------------

void SkeletonObject::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();
    
    addProtectedField( "Asset", TypeSkeletonAssetPtr, Offset(mSkeletonAsset, SkeletonObject), &setSkeletonAsset, &getSkeletonAsset, &writeSkeletonAsset, "The skeleton asset ID used for the skeleton." );
}

//-----------------------------------------------------------------------------

void SkeletonObject::preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Note tick times.
    mPreTickTime = mPostTickTime;
    mPostTickTime = totalTime;
    
    // Update composition at pre-tick time.
    updateComposition( mPreTickTime );
    
    // Are the spatials dirty?
    if ( getSpatialDirty() )
    {
        // Yes, so update the world transform.
        setBatchTransform( getRenderTransform() );
    }
    
    // Are the render extents dirty?
    if ( getLocalExtentsDirty() )
    {
        // Yes, so set size as local extents.
        setSize( getLocalExtents() );
    }
    
    // Call parent.
    Parent::preIntegrate( totalTime, elapsedTime, pDebugStats );
}

//-----------------------------------------------------------------------------

void SkeletonObject::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Call Parent.
    Parent::integrateObject( totalTime, elapsedTime, pDebugStats );
    
    // Finish if the spatials are NOT dirty.
    if ( !getSpatialDirty() )
        return;
    
    // Update the batch world transform.
    setBatchTransform( getRenderTransform() );
}

//-----------------------------------------------------------------------------

void SkeletonObject::interpolateObject( const F32 timeDelta )
{
    // Call parent.
    Parent::interpolateObject( timeDelta );
    
    // Update composition time (interpolated).
    updateComposition( (timeDelta * mPreTickTime) + ((1.0f-timeDelta) * mPostTickTime) );
    
    // Finish if the spatials are NOT dirty.
    if ( !getSpatialDirty() )
        return;
    
    // Update the batch world transform.
    setBatchTransform( getRenderTransform() );
}

//------------------------------------------------------------------------------

void SkeletonObject::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);
    
    // Fetch object.
    SkeletonObject* pComposite = dynamic_cast<SkeletonObject*>(object);
    
    // Sanity!
    AssertFatal(pComposite != NULL, "SkeletonObject::copyTo() - Object is not the correct type.");
    
    // Copy state.
    pComposite->setSkeletonAsset( getSkeletonAsset() );
	 // BOZO - Copy anything else?
}

//-----------------------------------------------------------------------------

void SkeletonObject::scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue )
{
    // BOZO - Why would I do something here and not in sceneRender?
    // Mich - Just how the system is basically set up. This is the last chance to
    // make any adjustments to how things will render, before the actual rendering
    // code is called. The most common work, like calculating AABB, sorting, and so
    // on is handled in SpriteBatch::prepareRender. This function exists for additional
    // massaging, which I don't think we'll be using for this task.

    // Prepare render.
    SpriteBatch::prepareRender( this, pSceneRenderState, pSceneRenderQueue );
}

//-----------------------------------------------------------------------------

void SkeletonObject::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Render.
    SpriteBatch::render( pSceneRenderState, pSceneRenderRequest, pBatchRenderer );

}

//-----------------------------------------------------------------------------

bool SkeletonObject::setSkeletonAsset( const char* pSkeletonAssetId )
{
    // Sanity!
    AssertFatal( pSkeletonAssetId != NULL, "Cannot use a NULL asset Id." );
    
    // Fetch the asset Id.
    mSkeletonAsset = pSkeletonAssetId;
    
    // Generate composition.
    generateComposition();
    
	return true;
}

//-----------------------------------------------------------------------------

void SkeletonObject::generateComposition( void )
{
    // Clear existing visualization
    clearSprites();
    mSkeletonSprites.clear();

    // Finish if skeleton asset isn't available.
    if ( mSkeletonAsset.isNull() )
        return;

    // Generate visualization.  

    /* // Get the ImageAsset used by the sprites
    StringTableEntry assetId = (*mSkeletonAsset).mImageAsset.getAssetId();

    if (*mSkeletonAsset).mImageAsset.isNull())
    {
        Con::warnf( "SkeletonObject::generateComposition() - Image asset was NULL, so nothing can be added to the composition.");
        return;
    }
    */

    // BOZO - Is this the right place to load stuff based on the SkeletonAsset?
    // Mich - Yup. It's here we will create Sprite objects, so we need to start
    // grabbing data from the mSkeletonAsset
	mSkeleton = _Torque2DSkeleton_create(mSkeletonAsset->mSkeletonData, this);
	mState = AnimationState_create(mSkeletonAsset->mStateData);

    /*
    for( U32 i = 0; i < mSkeleton->boneCount; ++i )
    {
        // Create the sprite.
        SpriteBatchItem* pSprite = SpriteBatch::createSprite();

        // Configure the sprite.
        pSprite->setImage(assetId);

        // Mich - Slot name?
        pSprite->setImageNameFrame();

        // scaleX and scaleY?
        pSprite->setSize();

        // Store the sprite reference.
        mSkeletonSprites.push_back(pSprite);
    }
    */
}

//-----------------------------------------------------------------------------
// time is the delta since the last update.
// Note that this is called twice, once from ::preIntegrate, then
// from ::interpolateObject. Check those functions out to see
// the different timing values that will be passed in.
void SkeletonObject::updateComposition( const F32 time )
{
    // Scale time.
    // Mich - This is a placeholder variable, just showing we can create
    // additional data based on the time that has elapsed. It can be anything we want it to be
    // This code was copied from WaveComposite, which needed the time to be scaled
    // to interpolate a sin wave. Right now we don't use it, so it can just be deleted
    //const F32 scaledTime = time * 100.0f; // BOZO - Why is time * 100?
    
    // Update position/orientation/state of visualization
    
    float deltaTime = 0.16f; // BOZO - Need time since last frame. What is "time"?
    Skeleton_update(mSkeleton, deltaTime);
    AnimationState_update(mState, deltaTime * mTimeScale);
    AnimationState_apply(mState, mSkeleton);
    Skeleton_updateWorldTransform(mSkeleton);

    // Update sprites
    for( typeSkeletonSpritesVector::iterator spriteItr = mSkeletonSprites.begin(); spriteItr < mSkeletonSprites.end(); ++spriteItr )
    {
        // Fetch sprite,
        SpriteBatchItem* pSprite = *spriteItr;

        // Update the local position
        // Update the scale
        // Update the rotation
        // Set the explicit vertices
        Vector2 vertices[4];
        pSprite->setExplicitVertices(vertices);
    }
}