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

#ifndef _SPRITE_BATCH_H_
#define _SPRITE_BATCH_H_

#ifndef _SPRITE_BATCH_ITEM_H_
#include "2d/core/SpriteBatchItem.h"
#endif

#ifndef _SCENE_RENDER_OBJECT_H_
#include "2d/scene/SceneRenderObject.h"
#endif

//------------------------------------------------------------------------------  

class SpriteBatch
{
protected:
    /// Sprite batch tree.
    class SpriteBatchTree : public b2DynamicTree
    {
        friend class b2DynamicTree;

    public:
        // Render query.
        inline bool QueryCallback( S32 proxyId )
        {    
            // Fetch sprite batch item.    
            SpriteBatchItem* pSpriteBatchItem = static_cast<SpriteBatchItem*>( GetUserData( proxyId ) );

            // Ignore if not visible.
            if ( !pSpriteBatchItem->getVisible() )
                return true;

            // Use sprite batch item.
            mBatchQuery.push_back( pSpriteBatchItem );

            return true;
        }

        typedef Vector< SpriteBatchItem* > typeSpriteItemVector;
        typeSpriteItemVector mBatchQuery;
    };

public:
    static const S32                INVALID_SPRITE_PROXY = -1;  

protected:
    typedef HashMap< U32, SpriteBatchItem* > typeSpriteBatchHash;
    typedef HashMap< SpriteBatchItem::LogicalPosition, SpriteBatchItem* > typeSpritePositionHash;
    typedef HashMap< StringTableEntry, SpriteBatchItem* > typeSpriteNameHash;

    typeSpriteBatchHash             mSprites;
    typeSpritePositionHash          mSpritePositions;
    typeSpriteNameHash              mSpriteNames;
    SpriteBatchItem*                mSelectedSprite;
    SceneRenderQueue::RenderSort    mBatchSortMode;
    bool                            mBatchCulling;
    Vector2                         mDefaultSpriteStride;
    Vector2                         mDefaultSpriteSize;
    F32                             mDefaultSpriteAngle;

private:
    SpriteBatchTree*                mpSpriteBatchTree;
    U32                             mMasterBatchId;

    b2Transform                     mBatchTransform;
    bool                            mBatchTransformDirty;
    U32                             mBatchTransformId;

    Vector2                         mLocalExtents;
    bool                            mLocalExtentsDirty;

public:
    SpriteBatch();
    virtual ~SpriteBatch();

    void prepareRender( SceneRenderObject* pSceneRenderObject, const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue );
    void render( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );

    inline void setBatchTransformDirty( void ) { mBatchTransformDirty = true; mBatchTransformId++; }
    inline bool getBatchTransformDirty( void ) const { return mBatchTransformDirty; }
    inline U32 getBatchTransformId( void ) { return mBatchTransformId; }
    const b2Transform& getBatchTransform( void ) const { return mBatchTransform; }

    inline void setLocalExtentsDirty( void ) { mLocalExtentsDirty = true; }
    inline bool getLocalExtentsDirty( void ) const { return mLocalExtentsDirty; }
    inline const Vector2& getLocalExtents( void ) { if ( getLocalExtentsDirty() ) updateLocalExtents(); return mLocalExtents; }

    void createTreeProxy( const b2AABB& localAABB, SpriteBatchItem* spriteBatchItem );
    void destroyTreeProxy( SpriteBatchItem* spriteBatchItem );
    void moveTreeProxy( SpriteBatchItem* spriteBatchItem, const b2AABB& localAABB );         

    virtual void copyTo( SpriteBatch* pSpriteBatch ) const;

    inline U32 getSpriteCount( void ) { return (U32)mSprites.size(); }

    U32 addSprite( const SpriteBatchItem::LogicalPosition& logicalPosition );
    bool removeSprite( void );
    virtual void clearSprites( void );

    inline void setBatchSortMode( SceneRenderQueue::RenderSort sortMode ) { mBatchSortMode = sortMode; }
    inline SceneRenderQueue::RenderSort getBatchSortMode( void ) const { return mBatchSortMode; }

    void setBatchCulling( const bool batchCulling );
    inline bool getBatchCulling( void ) const { return mBatchCulling; }

    inline void setDefaultSpriteStride( const Vector2& defaultStride ) { mDefaultSpriteStride = defaultStride; }
    inline Vector2 getDefaultSpriteStride( void ) const { return mDefaultSpriteStride; }

    inline void setDefaultSpriteSize( const Vector2& defaultSize ) { mDefaultSpriteSize = defaultSize; }
    inline Vector2 getDefaultSpriteSize( void ) const { return mDefaultSpriteSize; }

    inline void setDefaultSpriteAngle( const F32 defaultAngle ) { mDefaultSpriteAngle = defaultAngle; }
    inline F32 getDefaultSpriteAngle( void ) const { return mDefaultSpriteAngle; }

    bool selectSprite( const SpriteBatchItem::LogicalPosition& logicalPosition );
    bool selectSpriteId( const U32 batchId );
    bool selectSpriteName( const char* pName );
    inline void deselectSprite( void ) { mSelectedSprite = NULL; }
    bool isSpriteSelected( void ) const { return mSelectedSprite != NULL; }

    void setSpriteImage( const char* pAssetId, const U32 imageFrame = 0 );
    StringTableEntry getSpriteImage( void ) const;
    void setSpriteImageFrame( const U32 imageFrame );
    U32 getSpriteImageFrame( void ) const;
    void setSpriteAnimation( const char* pAssetId, const bool autoRestore = false );
    StringTableEntry getSpriteAnimation( void ) const;
    void clearSpriteAsset( void );

    void setSpriteVisible( const bool visible );
    bool getSpriteVisible( void ) const;

    void setSpriteLocalPosition( const Vector2& localPosition );
    Vector2 getSpriteLocalPosition( void );

    void setSpriteAngle( const F32 localAngle );
    F32 getSpriteAngle( void ) const;

    void setSpriteDepth( const F32 depth );
    F32 getSpriteDepth( void ) const;

    void setSpriteSize( const Vector2& size );
    Vector2 getSpriteSize( void ) const;

    void setSpriteFlipX( const bool flipX );
    bool getSpriteFlipX( void ) const;

    void setSpriteFlipY( const bool flipY );
    bool getSpriteFlipY( void ) const;

    void setSpriteSortPoint( const Vector2& sortPoint );
    Vector2 getSpriteSortPoint( void ) const;

    void setSpriteRenderGroup( const char* pRenderGroup );
    StringTableEntry getSpriteRenderGroup( void ) const;

    void setSpriteBlendMode( const bool blendMode );
    bool getSpriteBlendMode( void ) const;
    void setSpriteSrcBlendFactor( GLenum srcBlendFactor );
    GLenum getSpriteSrcBlendFactor( void ) const;
    void setSpriteDstBlendFactor( GLenum dstBlendFactor );
    GLenum getSpriteDstBlendFactor( void ) const;
    void setSpriteBlendColor( const ColorF& blendColor );
    const ColorF& getSpriteBlendColor( void ) const;
    void setSpriteBlendAlpha( const F32 alpha );
    F32  getSpriteBlendAlpha( void ) const;
    
    void setSpriteAlphaTest( const F32 alphaTestMode );
    F32 getSpriteAlphaTest( void ) const;

    void setSpriteDataObject( SimObject* pDataObject );
    SimObject* getSpriteDataObject( void ) const;

    void setSpriteName( const char* pName );
    StringTableEntry getSpriteName( void ) const;

protected:
    SpriteBatchItem* createSprite( void );
    SpriteBatchItem* findSpritePosition( const SpriteBatchItem::LogicalPosition& logicalPosition );
    SpriteBatchItem* findSpriteId( const U32 batchId );
    SpriteBatchItem* findSpriteName( const char* pName );

    virtual SpriteBatchItem* createSprite( const SpriteBatchItem::LogicalPosition& logicalPosition );

    void setBatchTransform( const b2Transform& batchTransform );
    void updateLocalExtents( void );

    void createSpriteBatchTree( void );
    void destroySpriteBatchTree( void );

    void onTamlCustomWrite( TamlCustomProperty* pSpritesProperty );
    void onTamlCustomRead( const TamlCustomProperty* pSpritesProperty );

private:
    bool destroySprite( const U32 batchId );
    bool checkSpriteSelected( void ) const;

    b2AABB calculateLocalAABB( const b2AABB& renderAABB );
};

#endif // _SPRITE_BATCH_H_
