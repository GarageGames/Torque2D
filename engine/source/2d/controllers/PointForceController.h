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
#include "2d/controllers/core/PickingSceneController.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/vector2.h"
#endif

//------------------------------------------------------------------------------

class PointForceController : public PickingSceneController
{
private:
    typedef PickingSceneController Parent;

    /// Controller position.
    Vector2 mPosition;

    /// Controller radius.
    F32 mRadius;

    /// Controller force.
    F32 mForce;

    /// Whether to apply the force non-linearly (using the inverse square law) or linearly.
    bool mNonLinear;

    /// Linear drag co-efficient.
    F32 mLinearDrag;

    /// Linear drag co-efficient.
    F32 mAngularDrag;

    /// Tracked object.
    SimObjectPtr<SceneObject> mTrackedObject;

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
    inline void setNonLinear( const bool nonLinear ) { mNonLinear = nonLinear; }
    inline bool getNonLinear( void ) const { return mNonLinear; }
    inline void setLinearDrag( const F32 linearDrag ) { mLinearDrag = linearDrag; }
    inline F32 getLinearDrag( void ) const { return mLinearDrag; }
    inline void setAngularDrag( const F32 angularDrag ) { mAngularDrag = angularDrag; }
    inline F32 getAngularDrag( void ) const { return mAngularDrag; }
    void setTrackedObject( SceneObject* pSceneObject );
    inline SceneObject* getTrackedObject( void ) { return mTrackedObject; }
    inline Vector2 getCurrentPosition( void )
    {
        // Fetch the tracked object.
        SceneObject* pSceneObject = mTrackedObject;

        // Return the controller position if no tracked object else the tracked object position plus a tracked object local-space position.
        return pSceneObject == NULL ? mPosition : b2Mul( pSceneObject->getTransform(), mPosition);
    }

    /// Integration.
    virtual void integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );

    // Scene render.
    virtual void renderOverlay( Scene* pScene, const SceneRenderState* pSceneRenderState, BatchRender* pBatchRenderer );

    /// Declare Console Object.
    DECLARE_CONOBJECT( PointForceController );
};

#endif // _ATTRACTOR_CONTROLLER_H_
