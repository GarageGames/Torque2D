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
    AssetPtr<SkeletonAsset>     mSkeletonAsset;
    F32                         mPreTickTime;
    F32                         mPostTickTime;
	 spine::Skeleton*            mSkeleton;
	 spine::AnimationState*      mState;
	 float                       mTimeScale;
    
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
    
    /// Declare Console Object.
    DECLARE_CONOBJECT( SkeletonObject );
    
protected:
    void generateComposition( void );
    void updateComposition( const F32 time );
    
protected:
    static bool setSkeletonAsset( void* obj, const char* data )                 { static_cast<SkeletonObject*>(obj)->setSkeletonAsset(data); return false; }
    static const char* getSkeletonAsset(void* obj, const char* data)            { return static_cast<SkeletonObject*>(obj)->getSkeletonAsset(); }
    static bool writeSkeletonAsset( void* obj, StringTableEntry pFieldName ) { return static_cast<SkeletonObject*>(obj)->mSkeletonAsset.notNull(); }
};

#endif // _SKELETON_OBJECT_H_
