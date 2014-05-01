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
    spSkeleton*                 mSkeleton;
    spAnimationState*           mState;
    
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
    
    bool                        mFlipX;
    bool                        mFlipY;
    
    
    
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
    
    /// Render flipping.
    void setFlip( const bool flipX, const bool flipY )  { mFlipX = flipX; mFlipY = flipY; }
    void setFlipX( const bool flipX )                   { setFlip( flipX, mFlipY ); }
    void setFlipY( const bool flipY )                   { setFlip( mFlipX, flipY ); }
    inline bool getFlipX( void ) const                  { return mFlipX; }
    inline bool getFlipY( void ) const                  { return mFlipY; }
    
    bool setSkeletonAsset( const char* pSkeletonAssetId );
    inline StringTableEntry getSkeletonAsset( void ) const { return mSkeletonAsset.getAssetId(); }
    
    inline bool setAnimationName( const char* pAnimation ) { return setAnimationName( pAnimation, mAnimationCycle ); }
    bool setAnimationName( const char* pAnimation, const bool isLooping = false);
    inline StringTableEntry getAnimationName( void ) const { return mCurrentAnimation; }
    
    bool setMix( const char* pFromAnimation, const char* pToAnimation, float time);
    
    bool setCurrentSkin( const char* pSkin );
    inline StringTableEntry getCurrentSkin( void ) const { return mCurrentSkin; }
    
    void setRootBoneScale( const Vector2& scale );
    inline void setRootBoneScale( const F32 x, const F32 y ){ setRootBoneScale( Vector2(x, y) ); }
    inline Vector2 getRootBoneScale( void ) const { return mSkeletonScale; }
    
    void setRootBoneOffset( const Vector2& scale );
    inline void setRootBoneOffset( const F32 x, const F32 y ){ setRootBoneOffset( Vector2(x, y) ); }
    inline Vector2 getRootBoneOffset( void ) const { return mSkeletonOffset; }
    
    inline F32 getAnimationDuration( void ) const { return mAnimationDuration; }
    inline bool isAnimationFinished( void ) const { return mAnimationFinished; };
    
    inline void setAnimationCycle( const bool isLooping ) { mAnimationCycle = isLooping; }
    inline bool getAnimationCycle( void ) const {return mAnimationCycle; };
    
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
    
    static bool setAnimationName( void* obj, const char* data )                  { static_cast<SkeletonObject*>(obj)->setAnimationName(data, static_cast<SkeletonObject*>(obj)->getAnimationCycle()); return false; }
    static const char* getAnimationName(void* obj, const char* data)             { return static_cast<SkeletonObject*>(obj)->getAnimationName(); }
    static bool writeAnimationName( void*obj, StringTableEntry pAnimation )      { return static_cast<SkeletonObject*>(obj)->getAnimationName() != StringTable->EmptyString; }
    
    static bool setCurrentSkin( void* obj, const char* data )                    { static_cast<SkeletonObject*>(obj)->setCurrentSkin(data); return false; }
    static const char* getCurrentSkin(void* obj, const char* data)               { return static_cast<SkeletonObject*>(obj)->getCurrentSkin(); }
    static bool writeCurrentSkin( void*obj, StringTableEntry pSkin )             { return static_cast<SkeletonObject*>(obj)->getCurrentSkin() != StringTable->EmptyString; }
    
    static bool setRootBoneScale(void* obj, const char* data)                    { static_cast<SkeletonObject*>(obj)->setRootBoneScale(Vector2(data)); return false; }
    static const char* getRootBoneScale(void* obj, const char* data)             { return static_cast<SkeletonObject*>(obj)->getRootBoneScale().scriptThis(); }
    static bool writeRootBoneScale( void* obj, StringTableEntry pFieldName )     { return static_cast<SkeletonObject*>(obj)->getRootBoneScale().notZero(); }
    
    static bool setRootBoneOffset(void* obj, const char* data)                   { static_cast<SkeletonObject*>(obj)->setRootBoneOffset(Vector2(data)); return false; }
    static const char* getRootBoneOffset(void* obj, const char* data)            { return static_cast<SkeletonObject*>(obj)->getRootBoneOffset().scriptThis(); }
    static bool writeRootBoneOffset( void* obj, StringTableEntry pFieldName )    { return static_cast<SkeletonObject*>(obj)->getRootBoneOffset().notZero(); }
    
    static bool setAnimationCycle( void* obj, const char* data )                 { static_cast<SkeletonObject*>(obj)->setAnimationCycle( dAtob(data) ); return false; }
    static bool writeAnimationCycle( void* obj, StringTableEntry pFieldName )    { return static_cast<SkeletonObject*>(obj)->getAnimationCycle() == false; }
    
    static bool writeFlipX( void* obj, StringTableEntry pFieldName )             { return static_cast<SkeletonObject*>(obj)->getFlipX() == true; }
    static bool writeFlipY( void* obj, StringTableEntry pFieldName )             { return static_cast<SkeletonObject*>(obj)->getFlipY() == true; }
};

#endif // _SKELETON_OBJECT_H_
