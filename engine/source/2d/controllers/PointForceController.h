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

#ifndef _ATTRACTOR_CONTROLLER_H_
#define _ATTRACTOR_CONTROLLER_H_

#ifndef _PICKING_SCENE_CONTROLLER_H_
#include "2d/controllers/core/pickingSceneController.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/vector2.h"
#endif

//------------------------------------------------------------------------------

class PointForceController : public PickingSceneController
{
private:
    typedef PickingSceneController Parent;

    Vector2 mPosition;
    F32 mRadius;
    F32 mForce;

public:
    PointForceController();
    virtual ~PointForceController();

    static void initPersistFields();
    virtual void copyTo(SimObject* object);

    inline void setPosition( const Vector2& position ) { mPosition = position; }
    inline const Vector2& getPosition( void ) const { return mPosition; }
    inline void setRadius( const F32 radius ) { mRadius = getMax( radius, FLT_MIN ); }
    inline F32 getRadius( void ) const { return mRadius; }
    inline void setForce( const F32 force ) { mForce = force; }
    inline F32 getForce( void ) const { return mForce; }

    /// Integration.
    virtual void integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );

    // Scene render.
    virtual void renderOverlay( Scene* pScene, const SceneRenderState* pSceneRenderState, BatchRender* pBatchRenderer );

    /// Declare Console Object.
    DECLARE_CONOBJECT( PointForceController );
};

#endif // _ATTRACTOR_CONTROLLER_H_
