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

#ifndef _SCROLLER_H_
#include "Scroller.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _BITSTREAM_H_
#include "io/bitStream.h"
#endif

// Script bindings.
#include "Scroller_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(Scroller);

//------------------------------------------------------------------------------

Scroller::Scroller() :
    mRepeatX(1.0f),
    mRepeatY(1.0f),
    mScrollX(0.0f),
    mScrollY(0.0f),
    mTextureOffsetX(0.0f),
    mTextureOffsetY(0.0f)
{
   // Use a static body by default.
   mBodyDefinition.type = b2_staticBody;

   // Use fixed rotation by default.
   mBodyDefinition.fixedRotation = true;
}

//------------------------------------------------------------------------------

Scroller::~Scroller()
{
}

//------------------------------------------------------------------------------

void Scroller::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("repeatX", TypeF32, Offset(mRepeatX, Scroller), &setRepeatX, &defaultProtectedGetFn, &writeRepeatX, "");
    addProtectedField("repeatY", TypeF32, Offset(mRepeatY, Scroller), &setRepeatY, &defaultProtectedGetFn, &writeRepeatY, "");
    addField("scrollX", TypeF32, Offset(mScrollX, Scroller), &writeScrollX, "");
    addField("scrollY", TypeF32, Offset(mScrollY, Scroller), &writeScrollY, "");
    addField("scrollPositionX", TypeF32, Offset(mTextureOffsetX, Scroller), &writeScrollPositionX, "");
    addField("scrollPositionY", TypeF32, Offset(mTextureOffsetY, Scroller), &writeScrollPositionY, "");
}

//------------------------------------------------------------------------------

void Scroller::copyTo(SimObject* object)
{
   Parent::copyTo(object);

   AssertFatal(dynamic_cast<Scroller*>(object), "Scroller::copyTo() - Object is not the correct type.");
   Scroller* scroller = static_cast<Scroller*>(object);

   scroller->setRepeat(getRepeatX(), getRepeatY());
   scroller->setScroll(getScrollX(), getScrollY());
   scroller->setScrollPosition(getScrollPositionX(), getScrollPositionY());
}

//------------------------------------------------------------------------------

bool Scroller::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Reset Tick Scroll Positions.
    resetTickScrollPositions();

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void Scroller::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void Scroller::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Call Parent.
    Parent::integrateObject( totalTime, elapsedTime, pDebugStats );

    // Calculate texel shift per world-unit.
    const F32 scrollTexelX = mRepeatX / getSize().x;
    const F32 scrollTexelY = mRepeatY / getSize().y;

    // Calculate Scrolling Offsets.
    const F32 scrollOffsetX = scrollTexelX * mScrollX * elapsedTime;
    const F32 scrollOffsetY = scrollTexelY * mScrollY * elapsedTime;

    // Calculate new offset.
    mTextureOffsetX += scrollOffsetX;
    mTextureOffsetY += scrollOffsetY;

    // Update Tick Scroll Position.
    // NOTE:-   We *must* do the tick update here!
    updateTickScrollPosition();

    // Make sure the offsets used don't under/overflow.
    // NOTE-    We could simply use 'mFmod' on the offsets but unfortunately
    //          we need to ensure that we can do a modulo simultaneously on both
    //          the pre/post ticks values otherwise the pre/post interpolation
    //          won't worked correctly resulting in a nasty wrap 'hitch'.

    // Calculate Renormalized Offsets.
    const F32 renormalizedPreOffsetX = mFmod( mPreTickTextureOffset.x, 1.0f );
    const F32 renormalizedPreOffsetY = mFmod( mPreTickTextureOffset.y, 1.0f );
    const F32 renormalizedPostOffsetX = mFmod( mPostTickTextureOffset.x, 1.0f );
    const F32 renormalizedPostOffsetY = mFmod( mPostTickTextureOffset.y, 1.0f );

    // Scrolling X Positive?
    if ( mGreaterThanZero(scrollOffsetX) )
    {
        // Yes, so old/new normalised simultaneously?
        if ( mLessThan(renormalizedPreOffsetX, renormalizedPostOffsetX) )
        {
            // Yes, so normalised offset.
            mTextureOffsetX = renormalizedPostOffsetX;
            // Normalise Pre/Post Ticks.
            mPreTickTextureOffset.x = renormalizedPreOffsetX;
            mPostTickTextureOffset.x = renormalizedPostOffsetX;
        }        
    }
    else
    {
        // No, so old/new normalised simultaneously?
        if ( mGreaterThan(renormalizedPreOffsetX, renormalizedPostOffsetX) )
        {
            // Yes, so normalised offset.
            mTextureOffsetX = renormalizedPostOffsetX;
            // Normalise Pre/Post Ticks.
            mPreTickTextureOffset.x = renormalizedPreOffsetX;
            mPostTickTextureOffset.x = renormalizedPostOffsetX;
        }        
    }

    // Scrolling Y Positive?
    if ( mGreaterThanZero(scrollOffsetY) )
    {
        // Yes, so old/new normalised proportionally?
        if ( mLessThan(renormalizedPreOffsetY, renormalizedPostOffsetY) )
        {
            // Yes, so normalised offset.
            mTextureOffsetY = renormalizedPostOffsetY;
            // Normalise Pre/Post Ticks.
            mPreTickTextureOffset.y = renormalizedPreOffsetY;
            mPostTickTextureOffset.y = renormalizedPostOffsetY;
        }        
    }
    else
    {
        // No, so old/new normalised proportionally?
        if ( mGreaterThan(renormalizedPreOffsetY, renormalizedPostOffsetY) )
        {
            // Yes, so normalised offset.
            mTextureOffsetY = renormalizedPostOffsetY;
            // Normalise Pre/Post Ticks.
            mPreTickTextureOffset.y = renormalizedPreOffsetY;
            mPostTickTextureOffset.y = renormalizedPostOffsetY;
        }        
    }
}

//------------------------------------------------------------------------------

void Scroller::interpolateObject( const F32 timeDelta )
{
    // Base object interpolation.
    Parent::interpolateObject( timeDelta );

    // Calculate Render Tick Position.
    mRenderTickTextureOffset = (timeDelta * mPreTickTextureOffset) + ((1.0f-timeDelta) * mPostTickTextureOffset);
}

//------------------------------------------------------------------------------

void Scroller::resetTickScrollPositions( void )
{
    // Reset Scroll Positions.
    mRenderTickTextureOffset.Set( mTextureOffsetX, mTextureOffsetY );
    mPreTickTextureOffset = mPostTickTextureOffset = mRenderTickTextureOffset;
}

//------------------------------------------------------------------------------

void Scroller::updateTickScrollPosition( void )
{
    // Store Pre Tick Scroll Position.
    mPreTickTextureOffset = mPostTickTextureOffset;

    // Store Current Tick Scroll Position.
    mPostTickTextureOffset.Set( mTextureOffsetX, mTextureOffsetY );

    // Render Tick Position is at Pre-Tick Scroll Position.
    mRenderTickTextureOffset = mPreTickTextureOffset;
};

//------------------------------------------------------------------------------

void Scroller::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Finish if we can't render.
    if ( !ImageFrameProvider::validRender() )
        return;

    // Fetch texture and texture area.
    const ImageAsset::FrameArea::TexelArea& frameTexelArea = getProviderImageFrameArea().mTexelArea;
    TextureHandle& texture = getProviderTexture();

    // Calculate render offset.
    F32 renderOffsetX = mFmod( mRenderTickTextureOffset.x, 1.0f );
    F32 renderOffsetY = mFmod( mRenderTickTextureOffset.y, 1.0f );
    if ( renderOffsetX < 0.0f ) renderOffsetX += 1.0f;
    if ( renderOffsetY < 0.0f ) renderOffsetY += 1.0f;

    // Calculate if frame has split rendering or not.
    const bool isSplitRenderFrameX = mNotZero( renderOffsetX );
    const bool isSplitRenderFrameY = mNotZero( renderOffsetY );

    // Clamp Texture Offsets.
    const F32 textureOffsetX = frameTexelArea.mTexelWidth * renderOffsetX;
    const F32 textureOffsetY = frameTexelArea.mTexelHeight * renderOffsetY;

    // Fetch lower/upper texture coordinates.
    const Vector2& texLower = frameTexelArea.mTexelLower;
    const Vector2& texUpper = frameTexelArea.mTexelUpper;

    ScrollSplitRegion baseSplitRegion;

    // Calculate split texel regions.
    baseSplitRegion.mTexSplitLowerX1 = texLower.x + textureOffsetX;
    baseSplitRegion.mTexSplitLowerY1 = texUpper.y - textureOffsetY;
    baseSplitRegion.mTexSplitLowerX2 = texUpper.x;
    baseSplitRegion.mTexSplitLowerY2 = texLower.y;
    baseSplitRegion.mTexSplitUpperX1 = texLower.x;
    baseSplitRegion.mTexSplitUpperY1 = texUpper.y;
    baseSplitRegion.mTexSplitUpperX2 = baseSplitRegion.mTexSplitLowerX1;
    baseSplitRegion.mTexSplitUpperY2 = baseSplitRegion.mTexSplitLowerY1;

    // Fetch render area.
    const Vector2& renderOOBB0 = mRenderOOBB[0];
    const Vector2& renderOOBB1 = mRenderOOBB[1];
    const Vector2& renderOOBB3 = mRenderOOBB[3];

    // Calculate region dimensions.
    const F32 regionWidth = (renderOOBB1.x - renderOOBB0.x) / mRepeatX;
    const F32 regionHeight = (renderOOBB3.y - renderOOBB0.y) / mRepeatY;

    // Calculate split region dimensions.
    const F32 splitRegionWidth = regionWidth * (1.0f-renderOffsetX);
    const F32 splitRegionHeight = regionHeight * (1.0f-renderOffsetY);

    // Calculate split vertex regions.
    baseSplitRegion.mVertSplitLowerX1 = renderOOBB0.x;
    baseSplitRegion.mVertSplitLowerX2 = renderOOBB0.x + splitRegionWidth;
    baseSplitRegion.mVertSplitUpperX1 = baseSplitRegion.mVertSplitLowerX2;
    baseSplitRegion.mVertSplitUpperX2 = renderOOBB0.x + regionWidth;
    baseSplitRegion.mVertSplitLowerY1 = renderOOBB0.y;
    baseSplitRegion.mVertSplitLowerY2 = renderOOBB0.y + splitRegionHeight;
    baseSplitRegion.mVertSplitUpperY1 = baseSplitRegion.mVertSplitLowerY2;
    baseSplitRegion.mVertSplitUpperY2 = renderOOBB0.y + regionHeight;

    // Fetch the whole regions.
    const S32 wholeRegionX = (S32)mCeil( mRepeatX );
    const S32 wholeRegionY = (S32)mCeil( mRepeatY );

    // Flush any existing batches.
    pBatchRenderer->flush();

    // jamesu - no clip planes in webgl
#ifndef TORQUE_OS_EMSCRIPTEN

    // Set-up a set of clip-planes against the OOBB.
    GLdouble left[4] = {1, 0, 0, -renderOOBB0.x};
    GLdouble right[4] = {-1, 0, 0, renderOOBB1.x};
    GLdouble top[4] = {0, -1, 0, renderOOBB3.y};
    GLdouble bottom[4] = {0, 1, 0, -renderOOBB0.y};

    //This is to prevent some android devices from throwing opengl errors. For instance the tegra 3 only supports 1
    int maxClip = 4;
    glGetIntegerv(GL_MAX_CLIP_PLANES, &maxClip);

    glClipPlane(GL_CLIP_PLANE0, left);

    if (maxClip > 1)
    	glClipPlane(GL_CLIP_PLANE1, right);
    if (maxClip > 2)
    	glClipPlane(GL_CLIP_PLANE2, top);
    if (maxClip > 3)
    glClipPlane(GL_CLIP_PLANE3, bottom);

    glEnable(GL_CLIP_PLANE0);
    if (maxClip > 1)
    	glEnable(GL_CLIP_PLANE1);
    if (maxClip > 2)
    	glEnable(GL_CLIP_PLANE2);
    if (maxClip > 3)
    	glEnable(GL_CLIP_PLANE3);

#endif

    // Render repeat Y.
    for ( S32 repeatIndexY = 0; repeatIndexY < wholeRegionY; ++repeatIndexY )
    {
        // Set base split region.
        ScrollSplitRegion splitRegion = baseSplitRegion;

        // Move vertex if appropriate.
        if ( repeatIndexY > 0 )
            splitRegion.addVertexOffset( 0.0f, regionHeight * repeatIndexY );

        // Render repeat X.
        for ( S32 repeatIndexX = 0; repeatIndexX < wholeRegionX; ++repeatIndexX )
        {
            // Split in X only?
            if ( isSplitRenderFrameX && !isSplitRenderFrameY )
            {
                renderRegionSplitX( pBatchRenderer, texture, splitRegion );
            }
            // Split in Y only?
            else if ( !isSplitRenderFrameX && isSplitRenderFrameY )
            {
                renderRegionSplitY( pBatchRenderer, texture, splitRegion );
            }
            // Split in X and Y?
            else if ( isSplitRenderFrameX && isSplitRenderFrameY )
            {
                renderRegionSplitXY( pBatchRenderer, texture, splitRegion );
            }
            // Not split.
            else
            {
                renderRegionNoSplit( pBatchRenderer, texture, splitRegion );
            }

            // Offset vertexes.
            splitRegion.addVertexOffset( regionWidth, 0.0f );
        }
    }

    // Flush the scroller batches.
    pBatchRenderer->flush();

#ifndef TORQUE_OS_EMSCRIPTEN
    // Disable the OOBB clip-planes.
    glDisable(GL_CLIP_PLANE0);
    if (maxClip > 1)
    	glDisable(GL_CLIP_PLANE1);
    if (maxClip > 2)
    	glDisable(GL_CLIP_PLANE2);
    if (maxClip > 3)
    	glDisable(GL_CLIP_PLANE3);

#endif
}

//------------------------------------------------------------------------------

void Scroller::renderRegionSplitX( BatchRender* pBatchRenderer, TextureHandle& texture, const ScrollSplitRegion& splitRegion )
{
    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitLowerX2, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitLowerX2, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitLowerY2 ),
        texture );

    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitUpperX1, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mVertSplitUpperX1, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitUpperX1, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitUpperX2, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitUpperX2, splitRegion.mTexSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitUpperX1, splitRegion.mTexSplitLowerY2 ),
        texture );
}

//------------------------------------------------------------------------------

void Scroller::renderRegionSplitY( BatchRender* pBatchRenderer, TextureHandle& texture, const ScrollSplitRegion& splitRegion )
{
    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitLowerY2 ),
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitLowerY2 ),
        texture );

    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitUpperY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitUpperY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitUpperY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitUpperY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitUpperY2 ),
        texture );

}

//------------------------------------------------------------------------------

void Scroller::renderRegionSplitXY( BatchRender* pBatchRenderer, TextureHandle& texture, const ScrollSplitRegion& splitRegion )
{
    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitLowerX2, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitLowerX2, splitRegion.mVertSplitLowerY2 ),
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitLowerY2 ),
        texture );

    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitUpperX1, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitLowerY2 ),
        Vector2( splitRegion.mVertSplitUpperX1, splitRegion.mVertSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitUpperX1, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitUpperX2, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitUpperX2, splitRegion.mTexSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitUpperX1, splitRegion.mTexSplitLowerY2 ),
        texture );

    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitUpperY1 ),
        Vector2( splitRegion.mVertSplitLowerX2, splitRegion.mVertSplitUpperY1 ),
        Vector2( splitRegion.mVertSplitLowerX2, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitUpperY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitUpperY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitUpperY2 ),
        texture );

    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitUpperX1, splitRegion.mVertSplitUpperY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitUpperY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mVertSplitUpperX1, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitUpperX1, splitRegion.mTexSplitUpperY1 ),
        Vector2( splitRegion.mTexSplitUpperX2, splitRegion.mTexSplitUpperY1 ),
        Vector2( splitRegion.mTexSplitUpperX2, splitRegion.mTexSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitUpperX1, splitRegion.mTexSplitUpperY2 ),
        texture );
}

//------------------------------------------------------------------------------

void Scroller::renderRegionNoSplit( BatchRender* pBatchRenderer, TextureHandle& texture, const ScrollSplitRegion& splitRegion )
{
    // Submit batched quad.
    pBatchRenderer->SubmitQuad(
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitLowerY1 ),
        Vector2( splitRegion.mVertSplitUpperX2, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mVertSplitLowerX1, splitRegion.mVertSplitUpperY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitLowerY1 ),
        Vector2( splitRegion.mTexSplitLowerX2, splitRegion.mTexSplitLowerY2 ),
        Vector2( splitRegion.mTexSplitLowerX1, splitRegion.mTexSplitLowerY2 ),
        texture );
}

//------------------------------------------------------------------------------

void Scroller::setRepeat( const F32 repeatX, const F32 repeatY )
{
    // Warn.
    if ( repeatX <= 0.0f || repeatY <= 0.0f )
    {
        Con::warnf("Scroller::setRepeat() - Repeats must be greater than zero!");
        return;
    }

    // Set Repeat X/Y.
    mRepeatX = repeatX;
    mRepeatY = repeatY;
}

//------------------------------------------------------------------------------

void Scroller::setScroll( F32 scrollX, F32 scrollY )
{
    // Set Scroll X/Y.
    mScrollX = scrollX;
    mScrollY = scrollY;

    // Reset Tick Scroll Positions.
    resetTickScrollPositions();
}

//------------------------------------------------------------------------------

void Scroller::setScrollPosition( F32 scrollX, F32 scrollY )
{
    // Yes, so calculate texel shift per world-unit.
    const F32 scrollTexelX = mRepeatX / getSize().x;
    const F32 scrollTexelY = mRepeatY / getSize().y;

    // Calculate new offset and clamp.
    mTextureOffsetX = mFmod( scrollTexelX * scrollX, 1.0f );
    mTextureOffsetY = mFmod( scrollTexelY * scrollY, 1.0f );

    // Reset Tick Scroll Positions.
    resetTickScrollPositions();
}
