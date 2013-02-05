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

#ifndef _SPRITE_BATCH_ITEM_H_
#include "2d/core/SpriteBatchItem.h"
#endif

#ifndef _SPRITE_BATCH_H_
#include "2d/core/SpriteBatch.h"
#endif

#ifndef _SCENE_RENDER_REQUEST_H_
#include "2d/scene/SceneRenderRequest.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

//-----------------------------------------------------------------------------

static bool spriteBatchItemPropertiesInitialized = false;

static StringTableEntry spriteLogicalPositionName;
static StringTableEntry spriteVisibleName;
static StringTableEntry spriteLocalPositionName;
static StringTableEntry spriteLocalAngleName;
static StringTableEntry spriteDepthName;
static StringTableEntry spriteSizeName;
static StringTableEntry spriteFlipXName;
static StringTableEntry spriteFlipYName;
static StringTableEntry spriteSortPointName;
static StringTableEntry spriteRenderGroupName;
static StringTableEntry spriteBlendModeName;
static StringTableEntry spriteSrcBlendFactorName;
static StringTableEntry spriteDstBlendFactorName;
static StringTableEntry spriteBlendColorName;
static StringTableEntry spriteAlphaTestName;
static StringTableEntry spriteImageName;
static StringTableEntry spriteImageFrameName;
static StringTableEntry spriteAnimationName;
static StringTableEntry spriteDataObjectName;
static StringTableEntry spriteNameName;

//------------------------------------------------------------------------------

SpriteBatchItem::SpriteBatchItem() : mProxyId( SpriteBatch::INVALID_SPRITE_PROXY )
{
    // Are the sprite batch item properties initialized?
    if ( !spriteBatchItemPropertiesInitialized )
    {
        // No, so initialize...

        spriteNameName              = StringTable->insert("Name");
        spriteLogicalPositionName   = StringTable->insert("LogicalPosition");
        spriteVisibleName           = StringTable->insert("Visible");
        spriteLocalPositionName     = StringTable->insert("Position");
        spriteLocalAngleName        = StringTable->insert("Angle");
        spriteSizeName              = StringTable->insert("Size");
        spriteDepthName             = StringTable->insert("Depth");
        spriteFlipXName             = StringTable->insert("FlipX");
        spriteFlipYName             = StringTable->insert("FlipY");
        spriteSortPointName         = StringTable->insert("SortPoint");
        spriteRenderGroupName       = StringTable->insert("RenderGroup");
        spriteBlendModeName         = StringTable->insert("BlendMode");
        spriteSrcBlendFactorName    = StringTable->insert("SrcBlendFactor");
        spriteDstBlendFactorName    = StringTable->insert("DstBlendFactor");
        spriteBlendColorName        = StringTable->insert("BlendColor");
        spriteAlphaTestName         = StringTable->insert("AlphaTest");
        spriteImageName             = StringTable->insert("Image");
        spriteImageFrameName        = StringTable->insert("Frame");
        spriteAnimationName         = StringTable->insert("Animation");
        spriteDataObjectName        = StringTable->insert("DataObject");

        // Flag as initialized.
        spriteBatchItemPropertiesInitialized = true;
    }

    resetState();
}

//------------------------------------------------------------------------------

SpriteBatchItem::~SpriteBatchItem()
{    
    resetState();
}

//------------------------------------------------------------------------------

void SpriteBatchItem::resetState( void )
{
    // Call parent.
    Parent::resetState();

    // Do we have a proxy.
    if ( mProxyId != SpriteBatch::INVALID_SPRITE_PROXY )
    {
        // Sanity!
        AssertFatal( mSpriteBatch != NULL, "Cannot remove proxy with NULL sprite batch." );

        // Destroy proxy.
        mSpriteBatch->destroyTreeProxy( this );
    }

    mSpriteBatch = NULL;
    mBatchId = 0;
    mName = StringTable->EmptyString;
    mLogicalPosition.resetState();

    mVisible = true;

    mLocalPosition.SetZero();
    mDepth = 0.0f;
    mLocalAngle = 0.0f;
    setSize( Vector2( 1.0f, 1.0f ) );

    mFlipX = false;
    mFlipY = false;

    mSortPoint.SetZero();
    mRenderGroup = StringTable->EmptyString;

    mBlendMode = true;
    mSrcBlendFactor = GL_SRC_ALPHA;
    mDstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
    mBlendColor = ColorF(1.0f,1.0f,1.0f,1.0f);
    mAlphaTest = -1.0f;

    mDataObject = NULL;

    mLocalTransformDirty = true;
    mLocalAABB.lowerBound.Set( -0.5f, -0.5f );
    mLocalAABB.upperBound.Set( 0.5f, 0.5f );

    mRenderAABB.lowerBound.Set( -0.5f, -0.5f );
    mRenderAABB.upperBound.Set( 0.5f, 0.5f );
    mRenderPosition.setZero();
    mLastBatchTransformId = 0;

    // Require self ticking.
    mSelfTick = true;
}

//------------------------------------------------------------------------------

void SpriteBatchItem::setBatchParent( SpriteBatch* pSpriteBatch, const U32 batchId )
{
    // Sanity!
    AssertFatal( pSpriteBatch != NULL, "Cannot assign a NULL batch parent." );
    AssertFatal( mSpriteBatch == NULL, "Cannot assign batch parent as one is already assigned." );
    AssertFatal( batchId != 0, "Cannot assign a zero batch Id." );

    // Assign.
    mSpriteBatch = pSpriteBatch;
    mBatchId = batchId;

    // Create proxy.
    mSpriteBatch->createTreeProxy( mLocalAABB, this );
}

//------------------------------------------------------------------------------

void SpriteBatchItem::copyTo( SpriteBatchItem* pSpriteBatchItem ) const
{
    // Call parent.
    Parent::copyTo( pSpriteBatchItem );

    // Set sprite batch item.
    pSpriteBatchItem->setLogicalPosition( getLogicalPosition() );
    pSpriteBatchItem->setName( getName() );
    pSpriteBatchItem->setVisible( getVisible() );
    pSpriteBatchItem->setLocalPosition( getLocalPosition() );
    pSpriteBatchItem->setDepth( getDepth() );
    pSpriteBatchItem->setLocalAngle( getLocalAngle() );
    pSpriteBatchItem->setSize( getSize() );
    pSpriteBatchItem->setFlipX( getFlipX() );
    pSpriteBatchItem->setFlipY( getFlipY() );
    pSpriteBatchItem->setSortPoint( getSortPoint() );
    pSpriteBatchItem->setRenderGroup( getRenderGroup() );
    pSpriteBatchItem->setBlendMode( getBlendMode() );
    pSpriteBatchItem->setSrcBlendFactor( getSrcBlendFactor() );
    pSpriteBatchItem->setDstBlendFactor( getDstBlendFactor() );
    pSpriteBatchItem->setBlendColor( getBlendColor() );
    pSpriteBatchItem->setAlphaTest( getAlphaTest() );
}

//------------------------------------------------------------------------------

void SpriteBatchItem::prepareRender( SceneRenderRequest* pSceneRenderRequest, const U32 batchTransformId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchItem_PrepareRender);

    // Sanity!
    AssertFatal( pSceneRenderRequest != NULL, "Cannot prepare a sprite batch with a NULL scene render request." );

    // Update the world transform.
    updateWorldTransform( batchTransformId );

    pSceneRenderRequest->mWorldPosition = mRenderPosition;
    pSceneRenderRequest->mDepth = getDepth();
    pSceneRenderRequest->mSortPoint = getSortPoint();
    pSceneRenderRequest->mSerialId = getBatchId();
    pSceneRenderRequest->mRenderGroup = getRenderGroup();
    pSceneRenderRequest->mBlendMode = getBlendMode();
    pSceneRenderRequest->mSrcBlendFactor = getSrcBlendFactor();
    pSceneRenderRequest->mDstBlendFactor = getDstBlendFactor();
    pSceneRenderRequest->mBlendColor = getBlendColor();
    pSceneRenderRequest->mAlphaTest = getAlphaTest();
}

//------------------------------------------------------------------------------

void SpriteBatchItem::render( BatchRender* pBatchRenderer, const SceneRenderRequest* pSceneRenderRequest, const U32 batchTransformId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchItem_Render);

    // Update the world transform.
    updateWorldTransform( batchTransformId );

    // Set the blend mode.
    pBatchRenderer->setBlendMode( pSceneRenderRequest );

    // Set the alpha test mode.
    pBatchRenderer->setAlphaTestMode( pSceneRenderRequest );

    // Render.
    Parent::render( mFlipX, mFlipY,
                    mRenderOOBB[0],
                    mRenderOOBB[1],
                    mRenderOOBB[2],
                    mRenderOOBB[3],
                    pBatchRenderer );
}

//------------------------------------------------------------------------------

void SpriteBatchItem::updateLocalTransform( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchItem_UpdateLocalTransform);

    // Sanity!
    AssertFatal( mSpriteBatch != NULL, "SpriteBatchItem::updateLocalTransform() - Cannot update local transform with a NULL sprite batch." );

    // Finish if local transform is not dirty.
    if ( !mLocalTransformDirty )
        return;

    // Set local transform.
    b2Transform localTransform;
    localTransform.p = mLocalPosition;
    localTransform.q.Set( mLocalAngle );

    // Calculate half size.
    const F32 halfWidth = mSize.x * 0.5f;
    const F32 halfHeight = mSize.y * 0.5f;

    // Set local size vertices.
    mLocalOOBB[0].Set( -halfWidth, -halfHeight );
    mLocalOOBB[1].Set( +halfWidth, -halfHeight );
    mLocalOOBB[2].Set( +halfWidth, +halfHeight );
    mLocalOOBB[3].Set( -halfWidth, +halfHeight );

    // Calculate local OOBB.
    CoreMath::mCalculateOOBB( mLocalOOBB, localTransform, mLocalOOBB );

    // Calculate local AABB.
    CoreMath::mOOBBtoAABB( mLocalOOBB, mLocalAABB );

    // Move tree proxy.
    mSpriteBatch->moveTreeProxy( this, mLocalAABB );

    // Flag local transform as NOT dirty.
    mLocalTransformDirty = false;
}

//------------------------------------------------------------------------------

void SpriteBatchItem::updateWorldTransform( const U32 batchTransformId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchItem_UpdateWorldTransform);

    // Sanity!
    AssertFatal( mSpriteBatch != NULL, "SpriteBatchItem::updateWorldTransform() - Cannot update transform with a NULL sprite batch." );

    // Update the local transform if needed.
    if ( mLocalTransformDirty )
        updateLocalTransform();

    // Finish if the batch transform is up-to-date.
    if ( batchTransformId == mLastBatchTransformId )
        return;

    // Fetch world transform.
    const b2Transform& worldTransform = mSpriteBatch->getBatchTransform();

    // Calculate world OOBB.
    CoreMath::mCalculateOOBB( mLocalOOBB, worldTransform, mRenderOOBB );

    // Calculate render AABB.
    CoreMath::mOOBBtoAABB( mRenderOOBB, mRenderAABB );

    // Calculate the render position.
    mRenderPosition = mRenderAABB.GetCenter();

    // Note the last batch transform Id.
    mLastBatchTransformId = batchTransformId;
}

//------------------------------------------------------------------------------

void SpriteBatchItem::onTamlCustomWrite( TamlPropertyAlias* pSpriteAlias )
{
    // Write name.
    if ( getName() != StringTable->EmptyString )
        pSpriteAlias->addField( spriteNameName, getName() );

    // Write asset.
    if ( isStaticMode() )
    {
        // Fetch image asset Id.
        StringTableEntry assetId = getImage();

        // Do we have an image?
        if ( assetId != StringTable->EmptyString )
        {
            // Yes, so write image asset Id.
            pSpriteAlias->addField( spriteImageName, assetId );

            // Write image frame.
            pSpriteAlias->addField( spriteImageFrameName, getImageFrame() );
        }
    }
    else
    {
        // Fetch animation asset Id.
        StringTableEntry assetId = getAnimation();

        // Do we have an animation?
        if ( assetId != StringTable->EmptyString )
        {
            // Yes, so write animation asset Id.
            pSpriteAlias->addField( spriteAnimationName, assetId );

        }
    }

    // Write visible.
    if ( !mVisible )
        pSpriteAlias->addField( spriteVisibleName, mVisible );

    // Write local position.
    pSpriteAlias->addField( spriteLocalPositionName, mLocalPosition );

    // Write local angle.
    if ( mNotZero(mLocalAngle) )
        pSpriteAlias->addField( spriteLocalAngleName, mRadToDeg(mLocalAngle) );

    // Write size.
    pSpriteAlias->addField( spriteSizeName, mSize );

    // Write depth.
    if ( mNotZero(mDepth) )
        pSpriteAlias->addField( spriteDepthName, mDepth );

    // Write flipX
    if ( mFlipX )
        pSpriteAlias->addField( spriteFlipXName, mFlipX );

    // Write flipY
    if ( mFlipY )
        pSpriteAlias->addField( spriteFlipYName, mFlipY );

    // Write sort point.
    if ( mSortPoint.notZero() )
        pSpriteAlias->addField( spriteSortPointName, mSortPoint );

    // Write render group.
    if ( mRenderGroup != StringTable->EmptyString )
        pSpriteAlias->addField( spriteRenderGroupName, mRenderGroup );

    // Write blend mode.
    if ( !mBlendMode )
        pSpriteAlias->addField( spriteBlendModeName, mBlendMode );

    // Write source blend factor.
    if ( mBlendMode && mSrcBlendFactor != GL_SRC_ALPHA )
        pSpriteAlias->addField( spriteBlendModeName, SceneObject::getSrcBlendFactorDescription(mSrcBlendFactor) );
        
    // Write destination blend factor.
    if ( mBlendMode && mDstBlendFactor != GL_ONE_MINUS_SRC_ALPHA )
        pSpriteAlias->addField( spriteDstBlendFactorName, SceneObject::getDstBlendFactorDescription(mDstBlendFactor) );

    // Write blend color.
    if ( mBlendMode && mBlendColor != ColorF(1.0f, 1.0f, 1.0f, 1.0f) )
        pSpriteAlias->addField( spriteBlendColorName, mBlendColor );

    // Write alpha test.
    if ( mBlendMode && mAlphaTest >= 0.0f )
        pSpriteAlias->addField( spriteAlphaTestName, mAlphaTest );

    // Write logical position.
    if ( getLogicalPosition().isValid() )
        pSpriteAlias->addField( spriteLogicalPositionName, getLogicalPosition().getString() );

    // Write data object.
    if ( getDataObject() != NULL )
        pSpriteAlias->addField( spriteDataObjectName, getDataObject() );
}

//------------------------------------------------------------------------------

void SpriteBatchItem::onTamlCustomRead( const TamlPropertyAlias* pSpriteAlias )
{
    // Sanity!
    AssertFatal( mSpriteBatch != NULL, "SpriteBatchItem::onTamlCustomRead() - Cannot read sprite batch item with sprite batch." );

    // Iterate property fields.
    for ( TamlPropertyAlias::const_iterator propertyFieldItr = pSpriteAlias->begin(); propertyFieldItr != pSpriteAlias->end(); ++propertyFieldItr )
    {
        // Fetch sprite field.
        TamlPropertyField* pSpriteField = *propertyFieldItr;

        // Fetch sprite field name.
        StringTableEntry fieldName = pSpriteField->getFieldName();

        // Reset image frame.
        S32 imageFrame = -1;

        if ( fieldName == spriteNameName )
        {
            setName( pSpriteField->getFieldValue() );
        }
        else if ( fieldName == spriteImageName )
        {
            setImage( pSpriteField->getFieldValue() );

            // Set image frame if it's available.
            if ( imageFrame != -1 )
                setImageFrame( imageFrame );

        }
        else if ( fieldName == spriteImageFrameName )
        {
            pSpriteField->getFieldValue( imageFrame );

            // Set image frame if image is available.
            if ( getImage() != StringTable->EmptyString )
                setImageFrame( imageFrame );
        }
        else if ( fieldName == spriteAnimationName )
        {
            setAnimation( pSpriteField->getFieldValue() );
        }
        else if ( fieldName == spriteVisibleName )
        {
            bool visible;
            pSpriteField->getFieldValue( visible );
            setVisible( visible );
        }
        else if ( fieldName == spriteLocalPositionName )
        {
            Vector2 localPosition;
            pSpriteField->getFieldValue( localPosition );
            setLocalPosition( localPosition );
        }
        else if ( fieldName == spriteLocalAngleName )
        {
            F32 localAngle;
            pSpriteField->getFieldValue( localAngle );
            setLocalAngle( mDegToRad( localAngle ) );
        }
        else if ( fieldName == spriteSizeName )
        {
            Vector2 size;
            pSpriteField->getFieldValue( size );
            setSize( size );
        }
        else if ( fieldName == spriteDepthName )
        {
            F32 depth;
            pSpriteField->getFieldValue( depth );
            setDepth( depth );
        }
        else if ( fieldName == spriteFlipXName )
        {
            bool flipX;
            pSpriteField->getFieldValue( flipX );
            setFlipX( flipX );
        }
        else if ( fieldName == spriteFlipYName )
        {
            bool flipY;
            pSpriteField->getFieldValue( flipY );
            setFlipY( flipY );
        }
        else if ( fieldName == spriteSortPointName )
        {
            Vector2 sortPoint;
            pSpriteField->getFieldValue( sortPoint );
            setSortPoint( sortPoint );
        }
        else if ( fieldName == spriteRenderGroupName )
        {
            setRenderGroup( pSpriteField->getFieldValue() );
        }
        else if ( fieldName == spriteBlendModeName )
        {
            bool blendMode;
            pSpriteField->getFieldValue( blendMode );
            setBlendMode( blendMode );
        }
        else if ( fieldName == spriteSrcBlendFactorName )
        {
            setSrcBlendFactor( (GLenum)SceneObject::getSrcBlendFactorEnum( pSpriteField->getFieldValue() ) );
        }
        else if ( fieldName == spriteDstBlendFactorName )
        {
            setDstBlendFactor( (GLenum)SceneObject::getDstBlendFactorEnum( pSpriteField->getFieldValue() ) );
        }
        else if ( fieldName == spriteBlendColorName )
        {
            ColorF blendColor;
            pSpriteField->getFieldValue( blendColor );
            setBlendColor( blendColor );
        }
        else if ( fieldName == spriteAlphaTestName )
        {
            F32 alphaTest;
            pSpriteField->getFieldValue( alphaTest );
            setAlphaTest( alphaTest );
        }
        // Logical position.
        else if ( fieldName == spriteLogicalPositionName )
        {
            // Fetch logical position.
            const char* pLogicalPositionArgs = pSpriteField->getFieldValue();

            // Is there any data?
            if ( dStrlen( pLogicalPositionArgs ) == 0 )
            {
                // No, so warn.
                Con::warnf( "SpriteBatchItem::onTamlCustomRead() - Encountered an empty sprite key.  This sprite will no longer be addressable by logical position." );
                continue;
            }

            // Set logical position.
            setLogicalPosition( LogicalPosition( pLogicalPositionArgs ) );
        }
        else if ( fieldName == spriteDataObjectName )
        {            
            setDataObject( pSpriteField->getFieldObject() );
        }
    }
}
