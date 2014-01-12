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

#ifndef _PICKING_SCENE_CONTROLLER_H_
#define _PICKING_SCENE_CONTROLLER_H_

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _SCENE_H_
#include "2d/scene/Scene.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _SCENE_CONTROLLER_H_
#include "2d/controllers/core/SceneController.h"
#endif

//------------------------------------------------------------------------------

class PickingSceneController : public SimObject, public SceneController
{
    typedef SimObject Parent;

private:
    U32 mControlGroupMask;
    U32 mControlLayerMask;

public:
    PickingSceneController();
    virtual ~PickingSceneController();

    virtual void copyTo(SimObject* object);

    inline void setControlGroupMask( const U32 groupMask ) { mControlGroupMask = groupMask; }
    inline U32 getControlGroupMask( void ) const { return mControlGroupMask; }
    inline void setControlLayerMask( const U32 layerMask ) { mControlLayerMask = layerMask; }
    inline U32 getControlLayerMask( void ) const { return mControlLayerMask; }

    /// Integration.
    virtual void integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats ) {}

    // Scene render.
    virtual void renderOverlay( Scene* pScene, const SceneRenderState* pSceneRenderState, BatchRender* pBatchRenderer ) {}

    /// Declare Console Object.
    DECLARE_CONOBJECT( PickingSceneController );

protected:
    WorldQuery* prepareQueryFilter( Scene* pScene, const bool clearQuery = true );
};

#endif // _PICKING_SCENE_CONTROLLER_H_
