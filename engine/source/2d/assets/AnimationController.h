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

#ifndef _ANIMATION_CONTROLLER_H_
#define _ANIMATION_CONTROLLER_H_

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#ifndef _ANIMATION_ASSET_H_
#include "2d/assets/AnimationAsset.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _FACTORY_CACHE_H_
#include "memory/factoryCache.h"
#endif

///-----------------------------------------------------------------------------

class AnimationController : public IFactoryObjectReset, private AssetPtrCallback
{
private:
    AssetPtr<AnimationAsset>                mAnimationAsset;
    AssetPtr<AnimationAsset>                mLastAnimationAsset;
    S32                                     mLastFrameIndex;
    S32                                     mCurrentFrameIndex;
    U32                                     mMaxFrameIndex;
    F32                                     mCurrentTime;
    F32                                     mPausedTime;
    F32                                     mCurrentModTime;
    F32                                     mAnimationTimeScale;
    F32                                     mTotalIntegrationTime;
    F32                                     mFrameIntegrationTime;
    bool                                    mAutoRestoreAnimation;
    bool                                    mAnimationFinished;

private:
    virtual void onAssetRefreshed( AssetPtrBase* pAssetPtrBase );

public:
    AnimationController();
    virtual ~AnimationController();

    TextureHandle&                                  getImageTexture( void )                 { return mAnimationAsset->getImage()->getImageTexture(); };
    const ImageAsset::FrameArea&                    getCurrentImageFrameArea( void ) const;
    const AnimationAsset*                           getCurrentAnimation( void ) const       { return mAnimationAsset.notNull() ? mAnimationAsset : NULL; };
    const StringTableEntry                          getCurrentAnimationAssetId( void ) const { return mAnimationAsset.getAssetId(); };
    const U32                                       getCurrentFrame( void ) const;
    const F32                                       getCurrentTime( void ) const            { return mCurrentTime; };
    bool                                            isAnimationValid( void ) const;
    bool                                            isAnimationFinished( void ) const       { return mAnimationFinished; };

    const AssetPtr<AnimationAsset>&                 getAnimationAsset( void ) const         { return mAnimationAsset; };

    void                                            setAnimationFrame( const U32 frameIndex );
    void                                            setAnimationTimeScale( const F32 scale ) { mAnimationTimeScale = scale; }
    inline F32                                      getAnimationTimeScale( void ) const     { return mAnimationTimeScale; }

    void                                            clearAssets( void )                     { mAnimationAsset.clear(); mLastAnimationAsset.clear(); }

    bool playAnimation( const AssetPtr<AnimationAsset>& animationAsset, const bool autoRestore );
    bool updateAnimation( const F32 elapsedTime );
    void stopAnimation( void );
    void resetTime( void );

    virtual void resetState( void );
};


#endif // _ANIMATION_CONTROLLER_H_
