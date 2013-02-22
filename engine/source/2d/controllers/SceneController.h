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

#ifndef _SCENE_CONTROLLER_H_
#define _SCENE_CONTROLLER_H_

#ifndef _SCENE_OBJECT_SET_H_
#include "2d/sceneObject/sceneObjectSet.h"
#endif

#ifndef _SCENE_RENDER_STATE_H_
#include "2d/scene/sceneRenderState.h"
#endif

#ifndef _BATCH_RENDER_H_
#include "2d/core/batchRender.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneObject/sceneObject.h"
#endif

#ifndef _DEBUG_STATS_H_
#include "2d/scene/DebugStats.h"
#endif

//------------------------------------------------------------------------------

class Scene;

//------------------------------------------------------------------------------

class SceneController : public SceneObjectSet
{
    typedef SceneObjectSet Parent;

public:
    SceneController();
    virtual ~SceneController();

    static void initPersistFields();
    virtual void copyTo(SimObject* object);

    /// Integration.
    virtual void integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats ) {}

    // Scene render.
    virtual void renderOverlay( Scene* pScene, const SceneRenderState* pSceneRenderState, BatchRender* pBatchRenderer ) {}

    /// Declare Console Object.
    DECLARE_CONOBJECT( SceneController );
};

#endif // _SCENE_CONTROLLER_H_
