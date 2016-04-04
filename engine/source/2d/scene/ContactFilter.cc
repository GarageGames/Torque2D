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

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

bool ContactFilter::ShouldCollide(b2Fixture* pFixtureA, b2Fixture* pFixtureB)
{
    // Debug Profiling.
    PROFILE_SCOPE(ContactFilter_ShouldCollide);

    PhysicsProxy* pPhysicsProxyA = static_cast<PhysicsProxy*>(pFixtureA->GetBody()->GetUserData());
    PhysicsProxy* pPhysicsProxyB = static_cast<PhysicsProxy*>(pFixtureB->GetBody()->GetUserData());

    // If not scene objects then cannot collide.
    if ( pPhysicsProxyA->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ||
         pPhysicsProxyB->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
         return false;

    SceneObject* pSceneObjectA = static_cast<SceneObject*>(pPhysicsProxyA);
    SceneObject* pSceneObjectB = static_cast<SceneObject*>(pPhysicsProxyB);

    // No contact if either objects are suppressing collision.
    if ( pSceneObjectA->mCollisionSuppress || pSceneObjectB->mCollisionSuppress )
        return false;

    // Check collision rules for one way shapes.
    if ( pSceneObjectA->mCollisionOneWay || pSceneObjectB->mCollisionOneWay )
    {
        // Filter out one way collisions.
        bool result = FilterOneWay(pSceneObjectA, pSceneObjectB, pFixtureA, pFixtureB);

        if (result)
            return false;
    }

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

//-----------------------------------------------------------------------------

bool ContactFilter::FilterOneWay(SceneObject* pSceneObjectA, SceneObject* pSceneObjectB, b2Fixture* pFixtureA, b2Fixture* pFixtureB)
{
    // One way collisions only apply to edge or chain shapes.
    if ((pFixtureA->GetType() == b2Shape::e_chain || pFixtureA->GetType() == b2Shape::e_edge) ||
        (pFixtureB->GetType() == b2Shape::e_chain || pFixtureB->GetType() == b2Shape::e_edge))
    {
        // Convenience renaming.
        SceneObject* pPlatformObject = NULL;
        SceneObject* pMovingObject = NULL;
        b2Fixture* pFixturePlatform = NULL;
        b2Fixture* pFixtureObject = NULL;

        if (pSceneObjectA->mCollisionOneWay)
        {
            pPlatformObject = pSceneObjectA;
            pMovingObject = pSceneObjectB;
            pFixturePlatform = pFixtureA;
            pFixtureObject = pFixtureB;
        }
        else if (pSceneObjectB->mCollisionOneWay)
        {
            pPlatformObject = pSceneObjectB;
            pMovingObject = pSceneObjectA;
            pFixturePlatform = pFixtureB;
            pFixtureObject = pFixtureA;
        }

        // Attempting to "cheat" by just getting a bounding box for the shape and using that center.
        b2Vec2 shapeCentroid;
        b2AABB* box = new b2AABB();

        if (pFixturePlatform->GetType() == b2Shape::e_chain)
        {
            const b2ChainShape* shape = pPlatformObject->getCollisionChainShape(0);
            shape->ComputeAABB(box, pPlatformObject->getTransform(), 0);
            shapeCentroid = box->GetCenter();
        }
        else
        {
            const b2EdgeShape* shape = pPlatformObject->getCollisionEdgeShape(0);
            shape->ComputeAABB(box, pPlatformObject->getTransform(), 0);
            shapeCentroid = box->GetCenter();
        }

        // We no longer need the bounding box, so delete it.
        delete(box);

        // Get normalized vector from the position of the platform shape to platform object.
        b2Vec2 nPosition = pPlatformObject->getPosition();
        nPosition = nPosition - shapeCentroid;
        nPosition.Normalize();

        // Get normalized velocity vector of the moving object.
        b2Vec2 nVelocity = pMovingObject->getLinearVelocity();
        nVelocity.Normalize();

        // Calculate the dot product.
        F32 product = b2Dot(nPosition, nVelocity);

        // If the result is less than zero, we have a pass through condition so flag as true.
        if (product < 0.0f)
            return true;
    }

    // The moving object should collide with platform, so flag as false.
    return false;
}
