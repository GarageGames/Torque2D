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
#include "2d/controllers/PointForceController.h"
#endif

// Script bindings.
#include "PointForceController_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(PointForceController);

//------------------------------------------------------------------------------

PointForceController::PointForceController()        
{
    // Reset he controller.
    mPosition.SetZero();
    mRadius = 1.0f;
    mForce = 0.0f;
    mNonLinear = true;
    mLinearDrag = 0.0f;
    mAngularDrag = 0.0f;
}

//------------------------------------------------------------------------------

PointForceController::~PointForceController()
{
}


//------------------------------------------------------------------------------

void PointForceController::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Force.
    addField( "Position", TypeVector2, Offset( mPosition, PointForceController), "The position of the attractor controller.");
    addField( "Radius", TypeF32, Offset( mRadius, PointForceController), "The radius of the attractor circle centered on the attractors position.");
    addField( "Force", TypeF32, Offset( mForce, PointForceController), "The force to apply to attact to the controller position.");
    addField( "NonLinear", TypeBool, Offset( mNonLinear, PointForceController), "Whether to apply the force non-linearly (using the inverse square law) or linearly.");
    addField( "LinearDrag", TypeF32, Offset(mLinearDrag, PointForceController), "The linear drag co-efficient for the fluid." );
    addField( "AngularDrag", TypeF32, Offset(mAngularDrag, PointForceController), "The angular drag co-efficient for the fluid." );
}

//------------------------------------------------------------------------------

void PointForceController::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to controller.
    PointForceController* pController = static_cast<PointForceController*>(object);

    // Sanity!
    AssertFatal(pController != NULL, "PointForceController::copyTo() - Object is not the correct type.");

    // Copy state.
    pController->setForce( getForce() );
}

//------------------------------------------------------------------------------

void PointForceController::setTrackedObject( SceneObject* pSceneObject )
{
    // Set tracked object.
    mTrackedObject = pSceneObject;
}

//------------------------------------------------------------------------------

void PointForceController::integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Finish if the attractor would have no effect.
    if ( mIsZero( mForce ) || mIsZero( mRadius ) )
        return;

    // Prepare query filter.
    WorldQuery* pWorldQuery = prepareQueryFilter( pScene );

    // Fetch the current position.
    const Vector2 currentPosition = getCurrentPosition();

    // Calculate the AABB of the attractor.
    b2AABB aabb;
    aabb.lowerBound.Set( currentPosition.x - mRadius, currentPosition.y - mRadius );
    aabb.upperBound.Set( currentPosition.x + mRadius, currentPosition.y + mRadius );

    // Query for candidate objects.
    pWorldQuery->anyQueryAABB( aabb ); 

    // Fetch results.
    typeWorldQueryResultVector& queryResults = pWorldQuery->getQueryResults();

    // Fetch result count.
    const U32 resultCount = (U32)queryResults.size();

    // Finish if nothing to process.
    if ( resultCount == 0 )
        return;

    // Calculate the radius squared.
    const F32 radiusSqr = mRadius * mRadius;

    // Calculate the force squared in-case we need it.
    const F32 forceSqr = mForce * mForce * (( mForce < 0.0f ) ? -1.0f : 1.0f);

    // Calculate drag coefficients (time-integrated).
    const F32 linearDrag = mClampF( mLinearDrag, 0.0f, 1.0f ) * elapsedTime;
    const F32 angularDrag = mClampF( mAngularDrag, 0.0f, 1.0f ) * elapsedTime;

    // Fetch the tracked object.
    const SceneObject* pTrackedObject = mTrackedObject;

    // Iterate the results.
    for ( U32 n = 0; n < resultCount; n++ )
    {
        // Fetch the scene object.
        SceneObject* pSceneObject = queryResults[n].mpSceneObject;

        // Ignore if it's the tracked object.
        if ( pSceneObject == pTrackedObject )
            continue;

        // Ignore if it's a static body.
        if ( pSceneObject->getBodyType() == b2_staticBody )
            continue;

        // Calculate the force distance to the controllers current position.
        Vector2 distanceForce = currentPosition - pSceneObject->getPosition();

        // Fetch distance squared.
        const F32 distanceSqr = distanceForce.LengthSquared();

        // Skip if the position is outside the radius or is centered on the controller.
        if ( distanceSqr > radiusSqr || distanceSqr < FLT_EPSILON )
            continue;

        // Non-Linear force?
        if ( mNonLinear )
        {
            // Yes, so use an approximation of the inverse-square law.
            distanceForce *= (1.0f / distanceSqr) * forceSqr;
        }
        else
        {
            // No, so normalize to the specified force (linear).
            distanceForce.Normalize( mForce );
        }

        // Apply the force.
        pSceneObject->applyForce( distanceForce, true );

        // Linear drag?
        if ( linearDrag > 0.0f )
        {
            // Yes, so fetch linear velocity.
            Vector2 linearVelocity = pSceneObject->getLinearVelocity();

            // Calculate linear velocity change.
            const Vector2 linearVelocityDelta = linearVelocity * linearDrag;

            // Set linear velocity.
            pSceneObject->setLinearVelocity( linearVelocity - linearVelocityDelta );
        }

        // Angular drag?
        if ( angularDrag > 0.0f )
        {
            // Yes, so fetch angular velocity.
            F32 angularVelocity = pSceneObject->getAngularVelocity();

            // Calculate angular velocity change.
            const F32 angularVelocityDelta = angularVelocity * angularDrag;

            // Set angular velocity.
            pSceneObject->setAngularVelocity( angularVelocity - angularVelocityDelta );
        }
    }
}

//------------------------------------------------------------------------------

void PointForceController::renderOverlay( Scene* pScene, const SceneRenderState* pSceneRenderState, BatchRender* pBatchRenderer )
{
    // Call parent.
    Parent::renderOverlay( pScene, pSceneRenderState, pBatchRenderer );

    // Draw force radius.
    pScene->mDebugDraw.DrawCircle( getCurrentPosition(), mRadius, ColorF(1.0f, 1.0f, 0.0f ) );
}