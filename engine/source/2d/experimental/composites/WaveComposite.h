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
#define _WAVE_COMPOSITE_H_

#ifndef _COMPOSITE_SPRITE_H_
#include "2d/sceneobject/CompositeSprite.h"
#endif

//------------------------------------------------------------------------------  

class WaveComposite : public SceneObject, public SpriteBatch
{
protected:
    typedef SceneObject Parent;

private:
    AssetPtr<ImageAsset>        mImageAsset;
    U32                         mImageFrame;
    U32                         mSpriteCount;
    Vector2                     mSpriteSize;
    F32                         mAmplitude;
    F32                         mFrequency;

    typedef Vector<SpriteBatchItem*> typeWaveSpritesVector;
    typeWaveSpritesVector       mWaveSprites;
    F32                         mPreTickTime;
    F32                         mPostTickTime;

public:
    WaveComposite();
    virtual ~WaveComposite();

    static void initPersistFields();

    virtual void preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void interpolateObject( const F32 timeDelta );

    virtual void copyTo( SimObject* object );

    virtual bool canPrepareRender( void ) const { return true; }
    virtual bool validRender( void ) const { return mImageAsset.notNull(); }
    virtual bool shouldRender( void ) const { return true; }
    virtual void scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue );    
    virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );

    bool setImage( const char* pImageAssetId );
    inline StringTableEntry getImage( void ) const { return mImageAsset.getAssetId(); }
    bool setImageFrame( const U32 frame );
    inline U32 getImageFrame( void ) const { return mImageFrame; }
    void setSpriteCount( const U32 spriteCount );
    inline U32 getSpriteCount( void ) const { return mSpriteCount; }
    void setSpriteSize( const Vector2& spriteSize );
    inline const Vector2& getSpriteSize( void ) const { return mSpriteSize; };
    inline void setAmplitude( const F32 amplitude ) { mAmplitude = amplitude; }
    inline F32 getAmplitude( void ) const { return mAmplitude; }
    inline void setFrequency( const F32 frequency ) { mFrequency = frequency; }
    inline F32 getFrequency( void ) const { return mFrequency; }

    /// Declare Console Object.
    DECLARE_CONOBJECT( WaveComposite );

protected:
    void generateComposition( void );
    void updateComposition( const F32 time );

protected:
    static bool setImage(void* obj, const char* data) { static_cast<WaveComposite*>(obj)->setImage( data ); return false; }
    static const char* getImage(void* obj, const char* data) { return DYNAMIC_VOID_CAST_TO(WaveComposite, ImageFrameProvider, obj)->getImage(); }
    static bool writeImage( void* obj, StringTableEntry pFieldName ) { return static_cast<WaveComposite*>(obj)->mImageAsset.notNull(); }
    static bool setImageFrame(void* obj, const char* data) { static_cast<WaveComposite*>(obj)->setImageFrame( dAtoi(data) ); return false; }
    static bool writeImageFrame( void* obj, StringTableEntry pFieldName ) { return static_cast<WaveComposite*>(obj)->getImageFrame() > 0; }
    static bool setSpriteCount(void* obj, const char* data) { static_cast<WaveComposite*>(obj)->setSpriteCount( dAtoi(data) ); return false; }
    static bool setSpriteSize(void* obj, const char* data) { static_cast<WaveComposite*>(obj)->setSpriteSize( Vector2(data) ); return false; }
};

#endif // _WAVE_COMPOSITE_H_
