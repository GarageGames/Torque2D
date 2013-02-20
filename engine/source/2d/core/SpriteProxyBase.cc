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

#ifndef _SPRITE_PROXY_BASE_H_
#include "2d/core/SpriteProxyBase.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _RENDER_PROXY_H_
#include "2d/core/RenderProxy.h"
#endif

#ifndef _STRINGBUFFER_H_
#include "string/stringBuffer.h"
#endif

//------------------------------------------------------------------------------

SpriteProxyBase::SpriteProxyBase() :
    mpAnimationController(NULL)
{
    resetState();
}

//------------------------------------------------------------------------------

SpriteProxyBase::~SpriteProxyBase()
{
    resetState();
}

//------------------------------------------------------------------------------

void SpriteProxyBase::resetState( void )
{
    clearAsset();
    mSelfTick = false;
    mAnimationPaused = false;
}

//------------------------------------------------------------------------------

bool SpriteProxyBase::update( const F32 elapsedTime )
{
    // Are we in static mode?
    if ( isStaticMode() )
    {
        // Yes, so turn-off tick processing.
        setProcessTicks( false );

        return false;
    }

    // Finish if no animation controller.
    if ( mpAnimationController == NULL )
        return false;

    // Finish if the animation has finished.
    if ( mpAnimationController->isAnimationFinished() )
        return false;

    // Finish if animation is paused.
    if ( mAnimationPaused )
        return true;

    // Update the animation.
    mpAnimationController->updateAnimation( Tickable::smTickSec );

    // Finish if the animation has NOT finished.
    if ( !mpAnimationController->isAnimationFinished() )
        return false;

    // Turn-off tick processing.
    setProcessTicks( false );

    // Perform callback.
    onAnimationEnd();

    // Flag animation as just finished.
    return true;
}

//------------------------------------------------------------------------------

void SpriteProxyBase::processTick( void )
{
    // Update using tick period.
    update( Tickable::smTickSec );
}

//------------------------------------------------------------------------------

bool SpriteProxyBase::validRender( void ) const
{
    // Are we in static mode?
    if ( isStaticMode() )
    {
        // Yes, so we must have an image asset and the frame must be in bounds.
        return mImageAsset.notNull() && ( getImageFrame() < mImageAsset->getFrameCount() );
    }

    // No, so we must have an animation controller and the animation must be valid.
    return mpAnimationController != NULL && mpAnimationController->isAnimationValid();
}

//------------------------------------------------------------------------------

void SpriteProxyBase::render(
    const bool flipX,
    const bool flipY,
    const Vector2& vertexPos0,
    const Vector2& vertexPos1,
    const Vector2& vertexPos2,
    const Vector2& vertexPos3,
    BatchRender* pBatchRenderer ) const
{
    // Finish if we can't render.
    if ( !validRender() )
        return;

    // Static mode?
    if ( isStaticMode() )
    {
        // Fetch current frame area.
        ImageAsset::FrameArea::TexelArea texelArea = mImageAsset->getImageFrameArea( mImageFrame ).mTexelArea;

        // Flip texture coordinates appropriately.
        texelArea.setFlip( flipX, flipY );
   
        // Fetch lower/upper texture coordinates.
        const Vector2& texLower = texelArea.mTexelLower;
        const Vector2& texUpper = texelArea.mTexelUpper;
    
        // Submit batched quad.
        pBatchRenderer->SubmitQuad(
            vertexPos0,
            vertexPos1,
            vertexPos2,
            vertexPos3,
            Vector2( texLower.x, texUpper.y ),
            Vector2( texUpper.x, texUpper.y ),
            Vector2( texUpper.x, texLower.y ),
            Vector2( texLower.x, texLower.y ),
            mImageAsset->getImageTexture() );

        return;
    }
    
    // Fetch current frame area.
    ImageAsset::FrameArea::TexelArea texelArea = mpAnimationController->getCurrentImageFrameArea().mTexelArea;

    // Flip texture coordinates appropriately.
    texelArea.setFlip( flipX, flipY );
   
    // Fetch lower/upper texture coordinates.
    const Vector2& texLower = texelArea.mTexelLower;
    const Vector2& texUpper = texelArea.mTexelUpper;
    
    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        vertexPos0,
        vertexPos1,
        vertexPos2,
        vertexPos3,
        Vector2( texLower.x, texUpper.y ),
        Vector2( texUpper.x, texUpper.y ),
        Vector2( texUpper.x, texLower.y ),
        Vector2( texLower.x, texLower.y ),
        mpAnimationController->getImageTexture() );
}

//-----------------------------------------------------------------------------

void SpriteProxyBase::renderGui( GuiControl& owner, Point2I offset, const RectI &updateRect ) const
{
    // Are we in static mode?
    if ( isStaticMode() )
    {
        // Do we have a valid image to render?
        if ( mImageAsset.notNull() && mImageFrame < mImageAsset->getFrameCount() )
        {
            // Yes, so calculate source region.
            const ImageAsset::FrameArea& frameArea = mImageAsset->getImageFrameArea( mImageFrame );
            RectI sourceRegion( frameArea.mPixelArea.mPixelOffset, Point2I(frameArea.mPixelArea.mPixelWidth, frameArea.mPixelArea.mPixelHeight) );

            // Calculate destination region.
            RectI destinationRegion(offset, owner.mBounds.extent);

            // Render image.
			dglSetBitmapModulation( owner.mProfile->mFillColor );
            dglDrawBitmapStretchSR( mImageAsset->getImageTexture(), destinationRegion, sourceRegion );
            dglClearBitmapModulation();
        }
        else
        {
            // No, so render no-image render-proxy.
            renderNoImage( owner, offset, updateRect );
        }
    }
    else
    {
        // Do we have a valid animation to render?
        if ( mpAnimationController != NULL && mpAnimationController->getAnimationAsset().notNull() )
        {
            // Yes, so calculate source region.
            const ImageAsset::FrameArea& frameArea = mpAnimationController->getCurrentImageFrameArea();
            RectI sourceRegion( frameArea.mPixelArea.mPixelOffset, Point2I(frameArea.mPixelArea.mPixelWidth, frameArea.mPixelArea.mPixelHeight) );

            // Calculate destination region.
            RectI destinationRegion(offset, owner.mBounds.extent);

            // Render animation image.
			dglSetBitmapModulation( owner.mProfile->mFillColor );
            dglDrawBitmapStretchSR( mpAnimationController->getImageTexture(), destinationRegion, sourceRegion );
            dglClearBitmapModulation();

            // Update control.
            owner.setUpdate();
        }
        else
        {
            // No, so render no-image render-proxy.
            renderNoImage( owner, offset, updateRect );
        }
    }

    // Render child controls.
    owner.renderChildControls(offset, updateRect);
}


//------------------------------------------------------------------------------

void SpriteProxyBase::renderNoImage( GuiControl& owner, Point2I &offset, const RectI& updateRect ) const
{
    // Fetch the 'cannot render' proxy.
    RenderProxy* pNoImageRenderProxy = Sim::findObject<RenderProxy>( CANNOT_RENDER_PROXY_NAME );

    // Finish if no render proxy available or it can't render.
    if ( pNoImageRenderProxy == NULL || !pNoImageRenderProxy->validRender() )
        return;

    // Render using render-proxy.
    pNoImageRenderProxy->renderGui( owner, offset, updateRect );

    // Update control.
    owner.setUpdate();
}

//------------------------------------------------------------------------------

void SpriteProxyBase::copyTo(SpriteProxyBase* pSpriteProxyBase) const
{
    // Sanity!
    AssertFatal(pSpriteProxyBase != NULL, "SpriteProxyBase::copyTo - Copy object cannot be NULL.");

    // Set self ticking.
    pSpriteProxyBase->mSelfTick = mSelfTick;

    // Are we in static mode?
    if ( mStaticMode )
    {
        // Yes, so use the image/frame if we have an asset.
        if ( mImageAsset.notNull() )
            pSpriteProxyBase->setImage( getImage(), getImageFrame() );
    }
    else if ( mAnimationAsset.notNull() )
    {
        // No, so use current animation if we have an asset.
        if ( mAnimationAsset.notNull() )
            pSpriteProxyBase->setAnimation(getAnimation(), false );
    }
}

//------------------------------------------------------------------------------

void SpriteProxyBase::clearAsset( void )
{
    // Destroy animation controller if required.
    if ( mpAnimationController != NULL )
    {
        delete mpAnimationController;
        mpAnimationController = NULL;
    }

    mAnimationAsset = NULL;
    mImageAsset = NULL;
    mImageFrame = 0;
    mStaticMode = true;
    setProcessTicks( false );
}

//------------------------------------------------------------------------------

bool SpriteProxyBase::setImage( const char* pImageAssetId, const U32 frame )
{
    // Finish if invalid image asset.
    if ( pImageAssetId == NULL )
        return false;

    // Set asset.
    mImageAsset = pImageAssetId;

    // Set the image frame if the image asset was set.
    if ( mImageAsset.notNull() )
        setImageFrame( frame );

    // Destroy animation controller if required.
    if ( mpAnimationController != NULL )
    {
        delete mpAnimationController;
        mpAnimationController = NULL;
    }

    // Set Frame.
    mImageFrame = frame;

    // Set as static render.
    mStaticMode = true;

    // Turn-off tick processing.
    setProcessTicks( false );

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool SpriteProxyBase::setImageFrame( const U32 frame )
{
    // Check Existing Image.
    if ( mImageAsset.isNull() )
    {
        // Warn.
        Con::warnf("SpriteProxyBase::setImageFrame() - Cannot set Frame without existing asset Id.");

        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= mImageAsset->getFrameCount() )
    {
        // Warn.
        Con::warnf( "SpriteProxyBase::setImageFrame() - Invalid Frame #%d for asset Id '%s'.", frame, mImageAsset.getAssetId() );
        // Return Here.
        return false;
    }

    // Set Frame.
    mImageFrame = frame;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool SpriteProxyBase::setAnimation( const char* pAnimationAssetId, const bool autoRestore )
{
    // Set as dynamic render.
    mStaticMode = false;

    // Ensure animation is un-paused.
    mAnimationPaused = false;

    // Create animation controller if required.
    if ( mpAnimationController == NULL )
        mpAnimationController = new AnimationController();

    // Reset static asset.
    mImageAsset.clear();

    // Fetch animation asset.
    mAnimationAsset = StringTable->insert( pAnimationAssetId );

    // Finish if we didn't get an animation.
    if ( mAnimationAsset.isNull() )
        return false;

    // Play Animation.
    if ( !mpAnimationController->playAnimation( mAnimationAsset, autoRestore ) )
        return false;

    // Turn-on tick processing.
    setProcessTicks( true );

    // Return Okay.
    return true;
}
