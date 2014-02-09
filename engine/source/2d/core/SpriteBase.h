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

#ifndef _SPRITE_BASE_H_
#define _SPRITE_BASE_H_

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _IMAGE_FRAME_PROVIDER_H
#include "2d/core/ImageFrameProvider.h"
#endif

//------------------------------------------------------------------------------

class SpriteBase : public SceneObject, public ImageFrameProvider
{
    typedef SceneObject Parent;

public:
    SpriteBase();
    virtual ~SpriteBase();

    static void initPersistFields();

    virtual void integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );

    virtual bool validRender( void ) const;
    virtual bool shouldRender( void ) const { return true; }

    virtual void copyTo(SimObject* object);

    /// Declare Console Object.
    DECLARE_CONOBJECT( SpriteBase );

protected:
    virtual void onAnimationEnd( void );

protected:
    static bool setImage(void* obj, const char* data)                           { DYNAMIC_VOID_CAST_TO(SpriteBase, ImageFrameProvider, obj)->setImage(data); return false; };
    static const char* getImage(void* obj, const char* data)                    { return DYNAMIC_VOID_CAST_TO(SpriteBase, ImageFrameProvider, obj)->getImage(); }
    static bool writeImage( void* obj, StringTableEntry pFieldName )            { SpriteBase* pCastObject = static_cast<SpriteBase*>(obj); if ( !pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mImageAsset.notNull(); }
    static bool setImageFrame(void* obj, const char* data)                      { DYNAMIC_VOID_CAST_TO(SpriteBase, ImageFrameProvider, obj)->setImageFrame(dAtoi(data)); return false; };
    static bool setImageNameFrame(void* obj, const char* data)                  { DYNAMIC_VOID_CAST_TO(SpriteBase, ImageFrameProvider, obj)->setImageFrameByName(data); return false; };
    static bool writeImageFrame( void* obj, StringTableEntry pFieldName )       { SpriteBase* pCastObject = static_cast<SpriteBase*>(obj); if ( !pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mImageAsset.notNull(); }
    static bool writeImageNameFrame( void* obj, StringTableEntry pFieldName )   { SpriteBase* pCastObject = static_cast<SpriteBase*>(obj); if ( !pCastObject->isUsingNamedImageFrame() ) return false; return pCastObject->mImageAsset.notNull(); }
    static bool setAnimation(void* obj, const char* data)                       { DYNAMIC_VOID_CAST_TO(SpriteBase, ImageFrameProvider, obj)->setAnimation(data); return false; };
    static const char* getAnimation(void* obj, const char* data)                { return DYNAMIC_VOID_CAST_TO(SpriteBase, ImageFrameProvider, obj)->getAnimation(); }
    static bool writeAnimation( void* obj, StringTableEntry pFieldName )        { SpriteBase* pCastObject = static_cast<SpriteBase*>(obj); if ( pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mAnimationAsset.notNull(); }
};

#endif // _SPRITE_BASE_H_
