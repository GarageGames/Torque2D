//-----------------------------------------------------------------------------
// COPYRIGHT HEADER HERE
//-----------------------------------------------------------------------------

#ifndef _SKELETON_OBJECT_H_
#define _SKELETON_OBJECT_H_

#ifndef _SPRITE_BATCH_H_
#include "2d/core/SpriteBatch.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _SKELETON_ASSET_H_
#include "2d/assets/SkeletonAsset.h"
#endif

//------------------------------------------------------------------------------

class SkeletonObject : public SceneObject, public SpriteBatch
{
protected:
    typedef SceneObject Parent;
    
private:
    typedef Vector<SpriteBatchItem*> typeSkeletonSpritesVector;
    typeSkeletonSpritesVector   mSkeletonSprites;

    AssetPtr<SkeletonAsset>     mSkeletonAsset;
    spine::Skeleton*            mSkeleton;
    spine::AnimationState*      mState;
    
    F32                         mPreTickTime;
    F32                         mPostTickTime;    
    F32                         mTimeScale;
    F32                         mLastFrameTime;
    F32                         mAnimationDuration;
    F32                         mTotalAnimationTime;

    bool                        mAnimationFinished;
    bool                        mIsLoopingAnimation;
    Vector2                     mSkeletonScale;
    Vector2                     mSkeletonOffset;

    StringTableEntry            mCurrentAnimation;
    StringTableEntry            mCurrentSkin;

    
    
public:
    SkeletonObject();
    virtual ~SkeletonObject();
    
    static void initPersistFields();
    
    virtual void preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void interpolateObject( const F32 timeDelta );
    
    virtual void copyTo( SimObject* object );
    
    virtual bool canPrepareRender( void ) const { return true; }
    virtual bool validRender( void ) const { return mSkeletonAsset.notNull(); }
    virtual bool shouldRender( void ) const { return true; }
    virtual void scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue );
    virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );
    
    bool setSkeletonAsset( const char* pSkeletonAssetId );
    inline StringTableEntry getSkeletonAsset( void ) const { return mSkeletonAsset.getAssetId(); }
    
    inline bool setCurrentAnimation( const char* pAnimation ) { return setCurrentAnimation( pAnimation, mIsLoopingAnimation ); }
    bool setCurrentAnimation( const char* pAnimation, const bool isLooping = false);
    inline StringTableEntry getCurrentAnimation( void ) const { return mCurrentAnimation; }

    bool setMix( const char* pFromAnimation, const char* pToAnimation, float time);
    
    bool setCurrentSkin( const char* pSkin );
    inline StringTableEntry getCurrentSkin( void ) const { return mCurrentSkin; }

    void setSkeletonScale( const Vector2& scale );
    inline void setSkeletonScale( const F32 x, const F32 y ){ setSkeletonScale( Vector2(x, y) ); }
    inline Vector2 getSkeletonScale( void ) const { return mSkeletonScale; }

    void setSkeletonOffset( const Vector2& scale );
    inline void setSkeletonOffset( const F32 x, const F32 y ){ setSkeletonOffset( Vector2(x, y) ); }
    inline Vector2 getSkeletonOffset( void ) const { return mSkeletonOffset; }

    inline F32 getAnimationDuration( void ) const { return mAnimationDuration; }
    inline bool isAnimationFinished( void ) const { return mAnimationFinished; };

    inline void setIsLoopingAnimation( const bool isLooping ) { mIsLoopingAnimation = isLooping; }
    inline bool getIsLoopingAnimation( void ) const {return mIsLoopingAnimation; };

    void onAnimationFinished();

    /// Declare Console Object.
    DECLARE_CONOBJECT( SkeletonObject );
    
protected:
    void generateComposition( void );
    void updateComposition( const F32 time );
    
protected:
    static bool setSkeletonAsset( void* obj, const char* data )                  { static_cast<SkeletonObject*>(obj)->setSkeletonAsset(data); return false; }
    static const char* getSkeletonAsset(void* obj, const char* data)             { return static_cast<SkeletonObject*>(obj)->getSkeletonAsset(); }
    static bool writeSkeletonAsset( void* obj, StringTableEntry pFieldName )     { return static_cast<SkeletonObject*>(obj)->mSkeletonAsset.notNull(); }

    static bool setCurrentAnimation( void* obj, const char* data )               { static_cast<SkeletonObject*>(obj)->setCurrentAnimation(data, static_cast<SkeletonObject*>(obj)->getIsLoopingAnimation()); return false; }
    static const char* getCurrentAnimation(void* obj, const char* data)          { return static_cast<SkeletonObject*>(obj)->getCurrentAnimation(); }
    static bool writeCurrentAnimation( void*obj, StringTableEntry pAnimation )   { return static_cast<SkeletonObject*>(obj)->getCurrentAnimation() != StringTable->EmptyString; }

    static bool setCurrentSkin( void* obj, const char* data )                    { static_cast<SkeletonObject*>(obj)->setCurrentSkin(data); return false; }
    static const char* getCurrentSkin(void* obj, const char* data)               { return static_cast<SkeletonObject*>(obj)->getCurrentSkin(); }
    static bool writeCurrentSkin( void*obj, StringTableEntry pSkin )             { return static_cast<SkeletonObject*>(obj)->getCurrentSkin() != StringTable->EmptyString; }

    static bool setSkeletonScale(void* obj, const char* data)                    { static_cast<SkeletonObject*>(obj)->setSkeletonScale(Vector2(data)); return false; }
    static const char* getSkeletonScale(void* obj, const char* data)             { return static_cast<SkeletonObject*>(obj)->getSkeletonScale().scriptThis(); }
    static bool writeSkeletonScale( void* obj, StringTableEntry pFieldName )     { return static_cast<SkeletonObject*>(obj)->getSkeletonScale().notZero(); }

    static bool setSkeletonOffset(void* obj, const char* data)                   { static_cast<SkeletonObject*>(obj)->setSkeletonOffset(Vector2(data)); return false; }
    static const char* getSkeletonOffset(void* obj, const char* data)            { return static_cast<SkeletonObject*>(obj)->getSkeletonOffset().scriptThis(); }
    static bool writeSkeletonOffset( void* obj, StringTableEntry pFieldName )    { return static_cast<SkeletonObject*>(obj)->getSkeletonOffset().notZero(); }

    static bool setIsLoopingAnimation( void* obj, const char* data )             { static_cast<SkeletonObject*>(obj)->setIsLoopingAnimation( dAtob(data) ); return false; }    
    static bool writeIsLoopingAnimation( void* obj, StringTableEntry pFieldName ){ return static_cast<SkeletonObject*>(obj)->getIsLoopingAnimation() == false; }
};

#endif // _SKELETON_OBJECT_H_
