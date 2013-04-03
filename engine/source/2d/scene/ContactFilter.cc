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

#include "ContactFilter.h"
#include "2d/sceneobject/SceneObject.h"

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

bool ContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
    // Debug Profiling.
    PROFILE_SCOPE(ContactFilter_ShouldCollide);

    PhysicsProxy* pPhysicsProxyA = static_cast<PhysicsProxy*>(fixtureA->GetBody()->GetUserData());
    PhysicsProxy* pPhysicsProxyB = static_cast<PhysicsProxy*>(fixtureB->GetBody()->GetUserData());

    // If not scene objects then cannot collide.
    if ( pPhysicsProxyA->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ||
         pPhysicsProxyB->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
         return false;

    const SceneObject* pSceneObjectA = static_cast<SceneObject*>(pPhysicsProxyA);
    const SceneObject* pSceneObjectB = static_cast<SceneObject*>(pPhysicsProxyB);

    // No contact if either objects are suppressing collision.
    if ( pSceneObjectA->mCollisionSuppress || pSceneObjectB->mCollisionSuppress )
        return false;

    // Check collision rule A -> B.
    if ( (pSceneObjectA->mCollisionGroupMask & pSceneObjectB->mSceneGroupMask) != 0 &&
         (pSceneObjectA->mCollisionLayerMask & pSceneObjectB->mSceneLayerMask) != 0 )
         return true;

    // Check collision rule B -> A.
    if ( (pSceneObjectB->mCollisionGroupMask & pSceneObjectA->mSceneGroupMask) != 0 &&
         (pSceneObjectB->mCollisionLayerMask & pSceneObjectA->mSceneLayerMask) != 0 )
         return true;

    return false;
}
