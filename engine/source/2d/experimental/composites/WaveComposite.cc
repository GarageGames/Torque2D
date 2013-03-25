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

#ifndef _WAVE_COMPOSITE_H_
#include "2d/experimental/composites/WaveComposite.h"
#endif

// Script bindings.
#include "2d/experimental/composites/WaveComposite_ScriptBinding.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(WaveComposite);

//------------------------------------------------------------------------------

WaveComposite::WaveComposite() :
    mPreTickTime( 0.0f ),
    mPostTickTime( 0.0f ),
    mImageFrame( 0 ),
    mSpriteCount( 50 ),
    mSpriteSize( 1.0f, 1.0f ),
    mAmplitude( 30.0f ),
    mFrequency( 10.0f )
{
    // Disable batch culling.
    // NOTE:    This stops the batch-query dynamic-tree from being generated.
    //          For smaller scale composites, this is more efficient and saves memory.
    //          Do not turn this off for larger scale composites like tile-maps.
    SpriteBatch::setBatchCulling( false );
}

//------------------------------------------------------------------------------

WaveComposite::~WaveComposite()
{
}

//------------------------------------------------------------------------------

void WaveComposite::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField( "Image", TypeImageAssetPtr, Offset(mImageAsset, WaveComposite), &setImage, &getImage, &writeImage, "The image asset Id used for the image." );
    addProtectedField( "Frame", TypeS32, Offset(mImageFrame, WaveComposite), &setImageFrame, &defaultProtectedGetFn, &writeImageFrame, "The image frame used for the image." );
    addProtectedField( "SpriteCount", TypeS32, Offset(mSpriteCount, WaveComposite), &setSpriteCount, &defaultProtectedGetFn, &defaultProtectedWriteFn, "The number of sprites to generate" );
    addProtectedField( "SpriteSize", TypeVector2, Offset(mSpriteSize, WaveComposite),&setSpriteSize, &defaultProtectedGetFn, &defaultProtectedWriteFn, "The size of each sprite." );
    addField( "Amplitude", TypeF32, Offset(mAmplitude, WaveComposite), "The amplitude of the sprite movement." );
    addField( "Frequency", TypeF32, Offset(mFrequency, WaveComposite), "The frequency of the sprite movement." );
}

//-----------------------------------------------------------------------------

void WaveComposite::preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
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

void WaveComposite::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
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

void WaveComposite::interpolateObject( const F32 timeDelta )
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

void WaveComposite::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object); 

    // Fetch object.
    WaveComposite* pComposite = dynamic_cast<WaveComposite*>(object);

    // Sanity!
    AssertFatal(pComposite != NULL, "WaveComposite::copyTo() - Object is not the correct type.");

    // Copy state.
    pComposite->setImage( getImage() );
    pComposite->setImageFrame( getImageFrame() );
    pComposite->setSpriteCount( getSpriteCount() );
    pComposite->setSpriteSize( getSpriteSize() );
    pComposite->setAmplitude( getAmplitude() );
}

//-----------------------------------------------------------------------------

void WaveComposite::scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue )
{
    // Prepare render.
    SpriteBatch::prepareRender( this, pSceneRenderState, pSceneRenderQueue );
}

//-----------------------------------------------------------------------------

void WaveComposite::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Render.
    SpriteBatch::render( pSceneRenderState, pSceneRenderRequest, pBatchRenderer );
}

//-----------------------------------------------------------------------------

bool WaveComposite::setImage( const char* pImageAssetId )
{
    // Sanity!
    AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

    // Fetch the asset Id.
    mImageAsset = pImageAssetId;

    // Reset image frame.
	mImageFrame = 0;

    // Generate composition.
    generateComposition();

	return true;
}

//-----------------------------------------------------------------------------

bool WaveComposite::setImageFrame( const U32 frame )
{
    // Check Existing Image.
    if ( mImageAsset.isNull() )
    {
        // Warn.
        Con::warnf("WaveComposite::setImageFrame() - Cannot set Frame without existing asset Id.");

        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= mImageAsset->getFrameCount() )
    {
        // Warn.
        Con::warnf( "WaveComposite::setImageFrame() - Invalid Frame #%d for asset Id '%s'.", frame, mImageAsset.getAssetId() );
        // Return Here.
        return false;
    }

    // Set Frame.
    mImageFrame = frame;

    // Generate composition.
    generateComposition();

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void WaveComposite::setSpriteCount( const U32 spriteCount )
{
    // Set sprite count.
    mSpriteCount = spriteCount;

    // Generate composition.
    generateComposition();
}

//-----------------------------------------------------------------------------

void WaveComposite::setSpriteSize( const Vector2& spriteSize )
{
    // Check Frame Validity.
    if ( spriteSize.x <= 0.0f && spriteSize.y <= 0.0f )
    {
        // Warn.
        Con::warnf( "WaveComposite::setSpriteSize() - Sprite sizes must be > 0." );

        return;
    }

    // Set sprite size.
    mSpriteSize = spriteSize;

    // Generate composition.
    generateComposition();
}

//-----------------------------------------------------------------------------

void WaveComposite::generateComposition( void )
{
    // Clear all existing sprites.
    clearSprites();
    mWaveSprites.clear();

    // Finish if image asset isn't available.
    if ( mImageAsset.isNull() )
        return;

    // Fetch asset Id.
    StringTableEntry assetId = mImageAsset.getAssetId();

    // Generate sprites.
    for( U32 n = 0; n < mSpriteCount; ++n )
    {
        // Create the sprite.
        SpriteBatchItem* pSprite = SpriteBatch::createSprite();

        // Configure the sprite.
        pSprite->setImage( assetId );
        pSprite->setImageFrame( mImageFrame );
        pSprite->setSize( mSpriteSize );

        // Store sprite reference.
        mWaveSprites.push_back( pSprite );
    }
}

//-----------------------------------------------------------------------------

void WaveComposite::updateComposition( const F32 time )
{
    // Scale time.
    const F32 scaledTime = time * 100.0f;

    // Calculate sprite start positin.
    Vector2 spritePosition( mSpriteSize.x * mSpriteCount * -0.5f, 0.0f );

    // Update sprite positions.
    for( typeWaveSpritesVector::iterator spriteItr = mWaveSprites.begin(); spriteItr < mWaveSprites.end(); ++spriteItr )
    {
        // Fetch sprite,
        SpriteBatchItem* pSprite = *spriteItr;

        // Set y position.
        spritePosition.y = mSin(mDegToRad(scaledTime + (spritePosition.x * mFrequency))) * mAmplitude;

        // Set sprite position.
        pSprite->setLocalPosition( spritePosition );

        // Update the position.
        spritePosition.x += mSpriteSize.x;
    }
}