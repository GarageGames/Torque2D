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

#ifndef _SCENE_OBJECT_ROTATE_TO_EVENT_H_
#define _SCENE_OBJECT_ROTATE_TO_EVENT_H_

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

//-----------------------------------------------------------------------------

class SceneObjectRotateToEvent : public SimEvent
{
public:
    SceneObjectRotateToEvent( const F32 targetAngle, const bool autoStop, const bool warpToTarget ) :
        mAutoStop( autoStop ),
        mWarpToTarget( warpToTarget ),
        mTargetAngle( targetAngle ) {}
    virtual ~SceneObjectRotateToEvent() {};

    virtual void process(SimObject *object)
    {
        // Fetch scene object.
        SceneObject* pSceneObject = (dynamic_cast<SceneObject*>(object));
        if (pSceneObject == NULL )
            return;

        // Are we auto stopping?
        if ( mAutoStop )
        {
            // Yes, so angular velocity.
            pSceneObject->setAngularVelocity( 0.0f );
        }

        // Are we warping to target?
        if ( mWarpToTarget )
        {
            // Yes, so set angle to the target.
            pSceneObject->setAngle( mTargetAngle );
        }

        // Reset event Id.
        pSceneObject->mRotateToEventId = 0;

        // Script callback.
        Con::executef( object, 2, "onRotateToComplete", Con::getFloatArg((mRadToDeg(mTargetAngle))) );
    }

private:
    F32         mTargetAngle;
    bool        mAutoStop;
    bool        mWarpToTarget;
};

#endif // _SCENE_OBJECT_ROTATE_TO_EVENT_H_