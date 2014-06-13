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

#ifndef _BUOYANCY_CONTROLLER_H_
#define _BUOYANCY_CONTROLLER_H_

#ifndef _PICKING_SCENE_CONTROLLER_H_
#include "2d/controllers/core/PickingSceneController.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/vector2.h"
#endif

//------------------------------------------------------------------------------

class BuoyancyController : public PickingSceneController
{
private:
    typedef PickingSceneController Parent;

    /// The fluid area.
    b2AABB mFluidArea;

    /// The fluid density.
    F32 mFluidDensity;

    /// Fluid flow velocity for drag calculations.
    Vector2 mFlowVelocity;

    /// Linear drag co-efficient.
    F32 mLinearDrag;

    /// Linear drag co-efficient.
    F32 mAngularDrag;

    /// Gravity to use inside the fluid.
    Vector2 mFluidGravity;

    /// Whether to use the collision shape densities or assume a uniform density.
    bool mUseShapeDensity;

    /// The outer fluid surface normal.
    Vector2 mSurfaceNormal;

protected:
    F32 ComputeCircleSubmergedArea( const b2Transform& bodyTransform, const b2CircleShape* pShape, Vector2& center );
    F32 ComputePolygonSubmergedArea( const b2Transform& bodyTransform, const b2PolygonShape* pShape, Vector2& center );

public:
    BuoyancyController();
    virtual ~BuoyancyController();

    static void initPersistFields();
    virtual void copyTo(SimObject* object);

    /// Integration.
    virtual void integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );

    // Scene render.
    virtual void renderOverlay( Scene* pScene, const SceneRenderState* pSceneRenderState, BatchRender* pBatchRenderer );

    /// Declare Console Object.
    DECLARE_CONOBJECT( BuoyancyController );
};

#endif // _BUOYANCY_CONTROLLER_H_
