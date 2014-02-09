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

#ifndef _SKELETON_H_
#define _SKELETON_H_

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

class Skeleton : public SceneObject, public SpriteBatch
{
protected:
    typedef SceneObject Parent;
    
private:
    typedef Vector<SpriteBatchItem*> typeSkeletonSpritesVector;
    typeSkeletonSpritesVector   mSkeletonSprites;

    AssetPtr<SkeletonAsset>     mSkeletonAsset;
    spSkeleton*          mSkeleton;
    spAnimationState*    mState;
    
    F32                         mPreTickTime;
    F32                         mPostTickTime;    
    F32                         mTimeScale;
    F32                         mLastFrameTime;
    F32                         mAnimationDuration;
    F32                         mTotalAnimationTime;

    bool                        mAnimationFinished;
    bool                        mAnimationCycle;
    Vector2                     mSkeletonScale;
    Vector2                     mSkeletonOffset;

    StringTableEntry            mCurrentAnimation;
    StringTableEntry            mCurrentSkin;

    
    
public:
    Skeleton();
    virtual ~Skeleton();
    
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
    
    inline bool setCurrentAnimation( const char* pAnimation ) { return setCurrentAnimation( pAnimation, mAnimationCycle ); }
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

    inline void setAnimationCycle( const bool isLooping ) { mAnimationCycle = isLooping; }
    inline bool getAnimationCycle( void ) const {return mAnimationCycle; };

    void onAnimationFinished();

    /// Declare Console Object.
    DECLARE_CONOBJECT( Skeleton );
    
protected:
    void generateComposition( void );
    void updateComposition( const F32 time );
    
protected:
    static bool setSkeletonAsset( void* obj, const char* data )                  { static_cast<Skeleton*>(obj)->setSkeletonAsset(data); return false; }
    static const char* getSkeletonAsset(void* obj, const char* data)             { return static_cast<Skeleton*>(obj)->getSkeletonAsset(); }
    static bool writeSkeletonAsset( void* obj, StringTableEntry pFieldName )     { return static_cast<Skeleton*>(obj)->mSkeletonAsset.notNull(); }

    static bool setCurrentAnimation( void* obj, const char* data )               { static_cast<Skeleton*>(obj)->setCurrentAnimation(data, static_cast<Skeleton*>(obj)->getAnimationCycle()); return false; }
    static const char* getCurrentAnimation(void* obj, const char* data)          { return static_cast<Skeleton*>(obj)->getCurrentAnimation(); }
    static bool writeCurrentAnimation( void*obj, StringTableEntry pAnimation )   { return static_cast<Skeleton*>(obj)->getCurrentAnimation() != StringTable->EmptyString; }

    static bool setCurrentSkin( void* obj, const char* data )                    { static_cast<Skeleton*>(obj)->setCurrentSkin(data); return false; }
    static const char* getCurrentSkin(void* obj, const char* data)               { return static_cast<Skeleton*>(obj)->getCurrentSkin(); }
    static bool writeCurrentSkin( void*obj, StringTableEntry pSkin )             { return static_cast<Skeleton*>(obj)->getCurrentSkin() != StringTable->EmptyString; }

    static bool setSkeletonScale(void* obj, const char* data)                    { static_cast<Skeleton*>(obj)->setSkeletonScale(Vector2(data)); return false; }
    static const char* getSkeletonScale(void* obj, const char* data)             { return static_cast<Skeleton*>(obj)->getSkeletonScale().scriptThis(); }
    static bool writeSkeletonScale( void* obj, StringTableEntry pFieldName )     { return static_cast<Skeleton*>(obj)->getSkeletonScale().notZero(); }

    static bool setSkeletonOffset(void* obj, const char* data)                   { static_cast<Skeleton*>(obj)->setSkeletonOffset(Vector2(data)); return false; }
    static const char* getSkeletonOffset(void* obj, const char* data)            { return static_cast<Skeleton*>(obj)->getSkeletonOffset().scriptThis(); }
    static bool writeSkeletonOffset( void* obj, StringTableEntry pFieldName )    { return static_cast<Skeleton*>(obj)->getSkeletonOffset().notZero(); }

    static bool setAnimationCycle( void* obj, const char* data )                 { static_cast<Skeleton*>(obj)->setAnimationCycle( dAtob(data) ); return false; }    
    static bool writeAnimationCycle( void* obj, StringTableEntry pFieldName )    { return static_cast<Skeleton*>(obj)->getAnimationCycle() == false; }
};

#endif // _SKELETON_H_
