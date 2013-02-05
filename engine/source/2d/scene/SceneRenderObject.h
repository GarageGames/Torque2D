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

#ifndef _SCENE_RENDER_OBJECT_H_
#define _SCENE_RENDER_OBJECT_H_

//-----------------------------------------------------------------------------

#ifndef _SCENE_RENDER_QUEUE_H_
#include "2d/scene/SceneRenderQueue.h"
#endif

#ifndef _SCENE_RENDER_STATE_H_
#include "2d/scene/SceneRenderState.h"
#endif

#ifndef _BATCH_RENDER_H_
#include "2d/core/BatchRender.h"
#endif

//-----------------------------------------------------------------------------

class SceneRenderObject
{
public:
    SceneRenderObject() {}
    virtual ~SceneRenderObject() {}

    virtual bool isBatchRendered( void ) = 0;

    virtual bool getBatchIsolated( void ) = 0;

    virtual bool validRender( void ) const = 0;

    virtual bool shouldRender( void ) const = 0;

    virtual void scenePrepareRender(const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue ) = 0;

    virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer ) = 0;

    virtual void sceneRenderFallback( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer ) = 0;
};

#endif // _SCENE_RENDER_OBJECT_H_
