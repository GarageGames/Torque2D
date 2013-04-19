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

#include "spine/extension.h"

// Script bindings.
#include "2d/sceneobject/SkeletonObject_ScriptBinding.h"

namespace spine {

void _AtlasPage_createTexture (AtlasPage* self, const char* path) {
}
void _AtlasPage_disposeTexture (AtlasPage* self) {
}

char* _Util_readFile (const char* path, int* length) {
    return _readFile(path, length);
}

}

using namespace spine;

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SkeletonObject);

//------------------------------------------------------------------------------

SkeletonObject::SkeletonObject() :
mPreTickTime( 0.0f ),
mPostTickTime( 0.0f ),
mTimeScale(1),
mLastFrameTime(0)
{
    
}

//------------------------------------------------------------------------------

SkeletonObject::~SkeletonObject()
{
    if (mSkeleton) {
        Skeleton_dispose(mSkeleton);
        mSkeleton = NULL;
    }
    if (mState) {
        AnimationState_dispose(mState);
        mState = NULL;
    }
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
    if ((*mSkeletonAsset).mImageAsset.isNull())
    {
        Con::warnf( "SkeletonObject::generateComposition() - Image asset was NULL, so nothing can be added to the composition.");
        return;
    }

    mSkeleton = Skeleton_create(mSkeletonAsset->mSkeletonData);
    mState = AnimationState_create(mSkeletonAsset->mStateData);
	 AnimationState_setAnimationByName(mState, "walk", true); // BOZO - For testing, not sure how to set the animation from a script.
}

//-----------------------------------------------------------------------------

void SkeletonObject::updateComposition( const F32 time )
{
    // Update position/orientation/state of visualization
    
    float delta = (time - mLastFrameTime) * mTimeScale;
	 mLastFrameTime = time;

    Skeleton_update(mSkeleton, delta);
    AnimationState_update(mState, delta);
    AnimationState_apply(mState, mSkeleton);
    Skeleton_updateWorldTransform(mSkeleton);

     // Get the ImageAsset used by the sprites
    StringTableEntry assetId = (*mSkeletonAsset).mImageAsset.getAssetId();

	 clearSprites();

	 Vector2 vertices[4];
    for (int i = 0; i < mSkeleton->slotCount; ++i) {
        Slot* slot = mSkeleton->slots[i];
        Attachment* attachment = slot->attachment;
        if (!attachment || attachment->type != ATTACHMENT_REGION) continue;
        RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
        RegionAttachment_updateVertices(regionAttachment, slot);

		  SpriteBatchItem* pSprite = SpriteBatch::createSprite();
		  
		  pSprite->setSrcBlendFactor(GL_ONE);
		  pSprite->setDstBlendFactor(GL_ONE_MINUS_SRC_ALPHA);

		  pSprite->setBlendColor(ColorF(
			  mSkeleton->r * slot->r,
			  mSkeleton->g * slot->g,
			  mSkeleton->b * slot->b,
			  mSkeleton->a * slot->a
		  ));

		  vertices[0].x = regionAttachment->vertices[VERTEX_X1];
		  vertices[0].y = regionAttachment->vertices[VERTEX_Y1];
		  vertices[1].x = regionAttachment->vertices[VERTEX_X4];
		  vertices[1].y = regionAttachment->vertices[VERTEX_Y4];
		  vertices[2].x = regionAttachment->vertices[VERTEX_X3];
		  vertices[2].y = regionAttachment->vertices[VERTEX_Y3];
		  vertices[3].x = regionAttachment->vertices[VERTEX_X2];
		  vertices[3].y = regionAttachment->vertices[VERTEX_Y2];
        pSprite->setExplicitVertices(vertices);

		  pSprite->setImage(assetId);
        pSprite->setImageFrameByName(attachment->name);
    }
}