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

SkeletonObject::SkeletonObject() : mPreTickTime( 0.0f ),
                                   mPostTickTime( 0.0f ),
                                   mTimeScale(1),
                                   mLastFrameTime(0),
                                   mTotalAnimationTime(0),
                                   mSkeleton(NULL),
                                   mState(NULL),
                                   mIsLoopingAnimation(false),
                                   mAnimationFinished(true),
                                   mAnimationDuration(0.0)
{
    mCurrentAnimation = StringTable->insert("");
    mSkeletonScale.SetZero();
    mSkeletonOffset.SetZero();
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
    
    addProtectedField("Asset", TypeSkeletonAssetPtr, Offset(mSkeletonAsset, SkeletonObject), &setSkeletonAsset, &getSkeletonAsset, &writeSkeletonAsset, "The skeleton asset ID used for the skeleton.");
    addProtectedField("Animation", TypeString, Offset(mCurrentAnimation, SkeletonObject), &setCurrentAnimation, &getCurrentAnimation, &writeCurrentAnimation, "The animation name to play.");
    addProtectedField("Skin", TypeString, Offset(mCurrentSkin, SkeletonObject), &setCurrentSkin, &getCurrentSkin, &writeCurrentSkin, "The skin to use.");
    addProtectedField("SkeletonScale", TypeVector2, NULL, &setSkeletonScale, &getSkeletonScale, &writeSkeletonScale, "Scaling of the skeleton's root bone");
    addProtectedField("SkeletonOffset", TypeVector2, NULL, &setSkeletonOffset, &getSkeletonOffset, &writeSkeletonOffset, "X/Y offset of the skeleton's root bone");
    addProtectedField("IsLoopingAnimation", TypeBool, Offset(mIsLoopingAnimation, SkeletonObject), &setIsLoopingAnimation, &defaultProtectedGetFn, &writeIsLoopingAnimation, "");
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

bool SkeletonObject::setCurrentAnimation( const char* pAnimation, const bool isLooping )
{
    // Make sure an asset was loaded.
    if (mSkeletonAsset.isNull())
        return false;

    // Set the animation.
    mCurrentAnimation = StringTable->insert(pAnimation);

    mIsLoopingAnimation = isLooping;
    
    // Generate composition.
    generateComposition();

    return true;
}

//-----------------------------------------------------------------------------

bool SkeletonObject::setMix( const char* pFromAnimation, const char* pToAnimation, float time)
{
    if (mSkeletonAsset.isNull())
    {
        Con::warnf("SkeletonObject::setMix() - Cannot mix. No asset assigned");
        return false;
    }
    
    // Check for valid animation state data
    AssertFatal( mSkeletonAsset->mStateData != NULL, "SkeletonObject::setMix() - Animation state data invalid" );
    
    // Check to see if the "from animation" is valid
    Animation* from = SkeletonData_findAnimation(mSkeleton->data, pFromAnimation);
    
    if (!from)
    {
        Con::warnf("SkeletonObject::setMix() - Animation %s does not exist.", pFromAnimation);
        return false;
    }
    
    // Check to see if the "to animation" is valid
	Animation* to = SkeletonData_findAnimation(mSkeleton->data, pToAnimation);
	
    if (!to)
    {
        Con::warnf("SkeletonObject::setMix() - Animation %s does not exist.", pToAnimation);
        return false;
    }
    
    // Check to see if a valid mix time was passsed
    if (time < 0.0f)
    {
        Con::warnf("SkeletonObject::setMix() - Invalid time set, %f", time);
        return false;
    }
    
    AnimationStateData_setMixByName(mSkeletonAsset->mStateData, pFromAnimation, pToAnimation, time);
    return true;
}

//-----------------------------------------------------------------------------

bool SkeletonObject::setCurrentSkin( const char* pSkin )
{
    if (mSkeletonAsset.isNull() || !mSkeleton)
    {
        Con::errorf("SkeletonObject::setCurrentSkin() - Skeleton Asset was null or skeleton was not built");
        return false;
    }

    S32 result = Skeleton_setSkinByName(mSkeleton, pSkin);
    if (result)
    {
        Skeleton_setSlotsToSetupPose(mSkeleton);
        return true;
    }
    else
    {
        Con::errorf("SkeletonObject::setCurrentSkin() - Skin %s not found", pSkin);
        return false;
    }
}

//-----------------------------------------------------------------------------

void SkeletonObject::setSkeletonScale(const Vector2& scale)
{
    mSkeletonScale = scale;

    if (!mSkeleton)
        return;

    if (mSkeletonScale.notZero())
    {
        Bone* rootBone = mSkeleton->root;
        rootBone->scaleX = mSkeletonScale.x;
        rootBone->scaleY = mSkeletonScale.y;
    }
}

//-----------------------------------------------------------------------------

void SkeletonObject::setSkeletonOffset(const Vector2& offset)
{
    mSkeletonOffset = offset;

     if (!mSkeleton)
        return;

    if (mSkeletonOffset.notZero())
    {
        Bone* rootBone = mSkeleton->root;
        rootBone->x = mSkeletonOffset.x;
        rootBone->y = mSkeletonOffset.y;
    }
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

    if (!mSkeleton)
        mSkeleton = Skeleton_create(mSkeletonAsset->mSkeletonData);

    if (!mState)
        mState = AnimationState_create(mSkeletonAsset->mStateData);

    if (mCurrentAnimation != StringTable->EmptyString)
    {
        AnimationState_setAnimationByName(mState, mCurrentAnimation, mIsLoopingAnimation);
        mAnimationDuration = mState->animation->duration;
        mAnimationFinished = false;
        mTotalAnimationTime = mLastFrameTime + mAnimationDuration;
    }
    
    if (mSkeletonScale.notZero())
    {
        Bone* rootBone = mSkeleton->root;
        rootBone->scaleX = mSkeletonScale.x;
        rootBone->scaleY = mSkeletonScale.y;
    }
    
    if (mSkeletonOffset.notZero())
    {
        Bone* rootBone = mSkeleton->root;
        rootBone->x = mSkeletonOffset.x;
        rootBone->y = mSkeletonOffset.y;
    }
}

//-----------------------------------------------------------------------------

void SkeletonObject::updateComposition( const F32 time )
{
    // Update position/orientation/state of visualization
    float delta = (time - mLastFrameTime) * mTimeScale;
    mLastFrameTime = time;

    Skeleton_update(mSkeleton, delta);
    
    if (!mAnimationFinished)
    {
        AnimationState_update(mState, delta);
        AnimationState_apply(mState, mSkeleton);
    }

    Skeleton_updateWorldTransform(mSkeleton);

    // Get the ImageAsset used by the sprites
    StringTableEntry assetId = (*mSkeletonAsset).mImageAsset.getAssetId();

    clearSprites();

    Vector2 vertices[4];

    float vertexPositions[8];
    for (int i = 0; i < mSkeleton->slotCount; ++i)
    {
        Slot* slot = mSkeleton->slots[i];
        Attachment* attachment = slot->attachment;
        
        if (!attachment || attachment->type != ATTACHMENT_REGION)
            continue;
        
        RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
		  RegionAttachment_computeVertices(regionAttachment, slot, vertexPositions);

        SpriteBatchItem* pSprite = SpriteBatch::createSprite();
		  
        pSprite->setSrcBlendFactor(GL_ONE);
        pSprite->setDstBlendFactor(GL_ONE_MINUS_SRC_ALPHA);

        float alpha = mSkeleton->a * slot->a;
        pSprite->setBlendColor(ColorF(
            mSkeleton->r * slot->r * alpha,
            mSkeleton->g * slot->g * alpha,
            mSkeleton->b * slot->b * alpha,
            alpha
        ));

        vertices[0].x = vertexPositions[VERTEX_X1];
        vertices[0].y = vertexPositions[VERTEX_Y1];
        vertices[1].x = vertexPositions[VERTEX_X4];
        vertices[1].y = vertexPositions[VERTEX_Y4];
        vertices[2].x = vertexPositions[VERTEX_X3];
        vertices[2].y = vertexPositions[VERTEX_Y3];
        vertices[3].x = vertexPositions[VERTEX_X2];
        vertices[3].y = vertexPositions[VERTEX_Y2];
        pSprite->setExplicitVertices(vertices);

        pSprite->setImage(assetId);
        pSprite->setImageFrameByName(attachment->name);
    }

    if (mLastFrameTime >= mTotalAnimationTime)
        mAnimationFinished = true;
    
    if (mAnimationFinished && !mIsLoopingAnimation)
    {
        mAnimationFinished = true;
        onAnimationFinished();
    }
    else
    {
        mAnimationFinished = false;
    }
}

void SkeletonObject::onAnimationFinished()
{
    // Do script callback.
    Con::executef( this, 2, "onAnimationFinished", mCurrentAnimation );
}