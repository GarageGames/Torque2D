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

#ifndef _COMPOSITE_SPRITE_H_
#define _COMPOSITE_SPRITE_H_

#ifndef _SPRITE_BATCH_H_
#include "2d/core/SpriteBatch.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

//------------------------------------------------------------------------------  

class CompositeSprite : public SceneObject, public SpriteBatch
{
protected:
    typedef SceneObject Parent;

public:
    // Batch layout type.
    enum BatchLayoutType
    {
        INVALID_LAYOUT,

        NO_LAYOUT,
        RECTILINEAR_LAYOUT,
        ISOMETRIC_LAYOUT,
        CUSTOM_LAYOUT
    };

private:
    BatchLayoutType mBatchLayoutType;

public:
    CompositeSprite();
    virtual ~CompositeSprite();

    static void initPersistFields();

    virtual bool onAdd();
    virtual void onRemove();

    virtual void preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void interpolateObject( const F32 timeDelta );

    virtual inline void setSpatialDirty(void) { mSpatialDirty = true; }

    virtual bool canPrepareRender( void ) const { return true; }
    virtual bool shouldRender( void ) const { return true; }
    virtual void scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue );    
    virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );

    virtual void copyTo( SimObject* object );

    void setBatchLayout( const BatchLayoutType& batchLayoutType );
    BatchLayoutType getBatchLayout( void ) const { return mBatchLayoutType; }

    static BatchLayoutType getBatchLayoutTypeEnum( const char* label );
    static const char* getBatchLayoutTypeDescription( const BatchLayoutType batchLayoutType );

    /// Declare Console Object.
    DECLARE_CONOBJECT( CompositeSprite );

protected:
    virtual SpriteBatchItem* createSprite( const SpriteBatchItem::LogicalPosition& logicalPosition );
    virtual SpriteBatchItem* createSpriteRectilinearLayout( const SpriteBatchItem::LogicalPosition& logicalPosition );
    virtual SpriteBatchItem* createSpriteIsometricLayout( const SpriteBatchItem::LogicalPosition& logicalPosition );
    virtual SpriteBatchItem* createCustomLayout( const SpriteBatchItem::LogicalPosition& logicalPosition );

    virtual void onTamlCustomWrite( TamlCustomNodes& customNodes );
    virtual void onTamlCustomRead( const TamlCustomNodes& customNodes );

protected:
    static bool         writeDefaultSpriteStride( void* obj, StringTableEntry pFieldName )  { return !STATIC_VOID_CAST_TO(CompositeSprite, SpriteBatch, obj)->getDefaultSpriteStride().isEqual( Vector2::getOne() ); }
    static bool         writeDefaultSpriteSize( void* obj, StringTableEntry pFieldName )    { return !STATIC_VOID_CAST_TO(CompositeSprite, SpriteBatch, obj)->getDefaultSpriteSize().isEqual( Vector2::getOne() ); }
    static bool         setDefaultSpriteAngle(void* obj, const char* data)                  { STATIC_VOID_CAST_TO(CompositeSprite, SpriteBatch, obj)->setDefaultSpriteAngle(mDegToRad(dAtof(data))); return false; }
    static const char*  getDefaultSpriteAngle(void* obj, const char* data)                  { return Con::getFloatArg( mRadToDeg(STATIC_VOID_CAST_TO(CompositeSprite, SpriteBatch, obj)->getDefaultSpriteAngle()) ); }
    static bool         writeDefaultSpriteAngle( void* obj, StringTableEntry pFieldName )   { return mNotZero( STATIC_VOID_CAST_TO(CompositeSprite, SpriteBatch, obj)->getDefaultSpriteAngle() ); }
    static bool         writeBatchIsolated( void* obj, StringTableEntry pFieldName )        { return static_cast<CompositeSprite*>(obj)->getBatchIsolated(); }
    static bool         writeBatchSortMode( void* obj, StringTableEntry pFieldName )        { return static_cast<CompositeSprite*>(obj)->getBatchSortMode() != SceneRenderQueue::RENDER_SORT_OFF; }

    static bool         setBatchLayout(void* obj, const char* data)                         { static_cast<CompositeSprite*>(obj)->setBatchLayout( getBatchLayoutTypeEnum(data) ); return false; }
    static bool         writeBatchLayout( void* obj, StringTableEntry pFieldName )          { return static_cast<CompositeSprite*>(obj)->getBatchLayout() != CompositeSprite::NO_LAYOUT; }
    static bool         setBatchCulling(void* obj, const char* data)                        { STATIC_VOID_CAST_TO(CompositeSprite, SpriteBatch, obj)->setBatchCulling(dAtob(data)); return false; }
    static bool         writeBatchCulling( void* obj, StringTableEntry pFieldName )         { return !static_cast<CompositeSprite*>(obj)->getBatchCulling(); }
};

#endif // _COMPOSITE_SPRITE_H_
