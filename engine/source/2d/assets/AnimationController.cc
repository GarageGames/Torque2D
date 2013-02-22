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

#include "graphics/gBitmap.h"
#include "console/consoleTypes.h"
#include "io/bitStream.h"
#include "2d/sceneobject/SceneObject.h"
#include "2d/assets/AnimationAsset.h"
#include "AnimationController.h"

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

AnimationController::AnimationController()
{
    // Register for animation asset refresh notifications.
    mAnimationAsset.registerRefreshNotify( this );

    // Reset the state.
    resetState();
}

//-----------------------------------------------------------------------------

AnimationController::~AnimationController()
{
}

//-----------------------------------------------------------------------------

void AnimationController::resetState( void )
{
    mCurrentFrameIndex = 0;
    mLastFrameIndex = 0;
    mMaxFrameIndex = 0;
    mCurrentTime = 0.0f;
    mPausedTime = 0.0f;
    mAnimationTimeScale = 1.0f;
    mTotalIntegrationTime = 0.0f;
    mFrameIntegrationTime = 0.0f;
    mAutoRestoreAnimation = false;
    mAnimationFinished = true;

    mAnimationAsset.clear();
    mLastAnimationAsset.clear();
}

//-----------------------------------------------------------------------------

void AnimationController::onAssetRefreshed( AssetPtrBase* pAssetPtrBase )
{
    // Don't perform any action if the animation is not already playing.
    if ( mAnimationFinished )
        return;

    // Attempt to restart the animation.
    playAnimation( mAnimationAsset, false );
}

//-----------------------------------------------------------------------------

const ImageAsset::FrameArea& AnimationController::getCurrentImageFrameArea( void ) const
{
    // Fetch current frame.
    const U32 currentFrame = getCurrentFrame();

    // Sanity!
    AssertFatal( mAnimationAsset.notNull(), "Animation controller requested image frame but no animation asset assigned." );

    // Fetch image asset.
    const AssetPtr<ImageAsset>& imageAsset = mAnimationAsset->getImage();

    // Sanity!
    AssertFatal( imageAsset.notNull(), "Animation controller requested image frame but no image asset assigned." );

    // Sanity!
    AssertFatal( currentFrame < imageAsset->getFrameCount(), "Animation controller requested image frame that is out of bounds." );

    return imageAsset->getImageFrameArea(currentFrame);
};

//-----------------------------------------------------------------------------

const U32 AnimationController::getCurrentFrame( void ) const
{
    // Sanity!
    AssertFatal( mAnimationAsset.notNull(), "Animation controller requested current image frame but no animation asset assigned." );

    // Fetch validated frames.
    const Vector<S32>& validatedFrames = mAnimationAsset->getValidatedAnimationFrames();

    // Sanity!
    AssertFatal( mCurrentFrameIndex < validatedFrames.size(), "Animation controller requested the current frame but it is out of bounds of the validated frames." );

    return validatedFrames[mCurrentFrameIndex];
};

//-----------------------------------------------------------------------------

bool AnimationController::isAnimationValid( void ) const
{
    // Not valid if no animation asset.
    if ( mAnimationAsset.isNull() )
        return false;

    // Fetch validated frames.
    const Vector<S32>& validatedFrames = mAnimationAsset->getValidatedAnimationFrames();

    // Not valid if current frame index is out of bounds of the validated frames.
    if ( mCurrentFrameIndex >= validatedFrames.size() )
        return false;

    // Fetch image asset.
    const AssetPtr<ImageAsset>& imageAsset = mAnimationAsset->getImage();

    // Not valid if no image asset.
    if ( imageAsset.isNull() )
        return false;

    // Fetch current frame.
    const U32 currentFrame = getCurrentFrame();

    // Not valid if current frame is out of bounds of the image asset.
    if ( currentFrame >= imageAsset->getFrameCount() )
        return false;

    // Valid.
    return true;
}

//-----------------------------------------------------------------------------

bool AnimationController::playAnimation( const AssetPtr<AnimationAsset>& animationAsset, const bool autoRestore )
{
    // Debug Profiling.
    PROFILE_SCOPE(AnimationController_PlayAnimation);

    // Stop animation.
    stopAnimation();

    // Finish if no animation asset.
    if ( animationAsset.isNull() )
        return true;

    // Fetch validated frames.
    const Vector<S32>& validatedFrames = animationAsset->getValidatedAnimationFrames();

    // Check we've got some frames.
    if ( validatedFrames.size() == 0 )
    {
        Con::warnf( "AnimationController::playAnimation() - Cannot play AnimationAsset '%s' - Animation has no validated frames!", mAnimationAsset.getAssetId() );
        return false;
    }

    // Set last animation asset.
    if ( autoRestore )
        mLastAnimationAsset = mAnimationAsset;
    else
        mLastAnimationAsset.clear();

    // Set animation asset.
    mAnimationAsset = animationAsset;

    // Set Maximum Frame Index.
    mMaxFrameIndex = validatedFrames.size()-1;

    // Calculate Total Integration Time.
    mTotalIntegrationTime = mAnimationAsset->getAnimationTime();

    // Calculate Frame Integration Time.
    mFrameIntegrationTime = mTotalIntegrationTime / validatedFrames.size();

    // No, so random Start?
    if ( mAnimationAsset->getRandomStart() )
    {
        // Yes, so calculate start time.
        mCurrentTime = CoreMath::mGetRandomF(0.0f, mTotalIntegrationTime*0.999f);
    }
    else
    {
        // No, so set first frame.
        mCurrentTime = 0.0f;
    }

    // Set Auto Restore Animation Flag.
    mAutoRestoreAnimation = autoRestore;

    // Reset animation finished flag.
    mAnimationFinished = false;

    // Do an initial animation update.
    updateAnimation(0.0f);

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void AnimationController::stopAnimation( void )
{
    // Flag as animation finished.
    mAnimationFinished = true;
}

//-----------------------------------------------------------------------------

bool AnimationController::updateAnimation( const F32 elapsedTime )
{
    // Debug Profiling.
    PROFILE_SCOPE(AnimationController_UpdateAnimation);

    // Finish if animation asset is not valid.
    if ( mAnimationAsset.isNull() || mAnimationAsset->getImage().isNull() )
        return false;

    // Finish if animation has finished.
    if ( mAnimationFinished )
        return false;

    // Fetch validated frames.
    const Vector<S32>& validatedFrames = mAnimationAsset->getValidatedAnimationFrames();

    // Finish if there are no validated frames.
    if ( validatedFrames.size() == 0 )
        return false;

    // Calculate scaled time.
    const F32 scaledTime = elapsedTime * mAnimationTimeScale;

    // Update Current Time.
    mCurrentTime += scaledTime;

    // Check if the animation has finished.
    if ( !mAnimationAsset->getAnimationCycle() && mGreaterThanOrEqual(mCurrentTime, mTotalIntegrationTime) )
    {
        // Animation has finished.
        mAnimationFinished = true;

        // Are we restoring the animation?
        if ( mAutoRestoreAnimation )
        {
            // Yes, so play last animation.
            playAnimation( mLastAnimationAsset, false );
        }
        else
        {
            // No, so fix Animation at end of frames.
            mCurrentTime = mTotalIntegrationTime - (mFrameIntegrationTime * 0.5f);
        }
    }

    // Update Current Mod Time.
    mCurrentModTime = mFmod( mCurrentTime, mTotalIntegrationTime );

    // Calculate Current Frame.
    mCurrentFrameIndex = (S32)(mCurrentModTime / mFrameIntegrationTime);

    // Fetch frame.
    S32 frame = validatedFrames[mCurrentFrameIndex];

    // Fetch image frame count.
    const S32 imageFrameCount = mAnimationAsset->getImage()->getFrameCount();

    // Clamp frames.
    if ( frame < 0 )
        frame = 0;
    else if (frame >= imageFrameCount )
        frame = imageFrameCount-1;

    // Calculate if frame has changed.
    bool frameChanged = (mCurrentFrameIndex != mLastFrameIndex);

    // Reset Last Frame.
    mLastFrameIndex = mCurrentFrameIndex;

    // Return Frame-Changed Flag.
    return frameChanged;
}

//-----------------------------------------------------------------------------

void AnimationController::resetTime( void )
{
    // Rest Time.
    mCurrentTime = 0.0f;
}

//-----------------------------------------------------------------------------

void AnimationController::setAnimationFrame( const U32 frameIndex )
{
    // Do we have a valid animation asset?
    if ( mAnimationAsset.isNull() )
    {
        // No, so warn.
        Con::warnf("AnimationController::setAnimationFrame() - Cannot set frame; animation is finished or is invalid!");
        return;
    }

    // Validate Frame Index?
    if ( (S32)frameIndex < 0 || frameIndex > mMaxFrameIndex )
    {
        // No, so warn.
        Con::warnf("AnimationController::setAnimationFrame() - Animation Frame-Index Invalid (frame#%d of %d in %s)", frameIndex, mMaxFrameIndex, mAnimationAsset.getAssetId() );
        // Finish here.
        return;
    }

    // Calculate current time.
    mCurrentTime = frameIndex*mFrameIntegrationTime;

    // Do an immediate animation update.
    updateAnimation(0.0f);
}