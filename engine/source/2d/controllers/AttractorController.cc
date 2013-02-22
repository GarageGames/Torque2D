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
#include "2d/controllers/AttractorController.h"
#endif

// Script bindings.
#include "AttractorController_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(AttractorController);

//------------------------------------------------------------------------------

AttractorController::AttractorController()        
{
    // Reset he controller.
    mPosition.SetZero();
    mRadius = 1.0f;
    mForce = 0.0f;
}

//------------------------------------------------------------------------------

AttractorController::~AttractorController()
{
}


//------------------------------------------------------------------------------

void AttractorController::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Force.
    addProtectedField("Position", TypeVector2, Offset( mPosition, AttractorController), &defaultProtectedSetFn, &defaultProtectedGetFn, "The position of the attractor controller.");
    addProtectedField("Radius", TypeVector2, Offset( mRadius, AttractorController), &defaultProtectedSetFn, &defaultProtectedGetFn, "The radius of the attractor circle centered on the attractors position.");
    addProtectedField("Force", TypeF32, Offset( mForce, AttractorController), &defaultProtectedSetFn, &defaultProtectedGetFn, "The force to apply to attact to the controller position.");
}

//------------------------------------------------------------------------------

void AttractorController::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to controller.
    AttractorController* pController = static_cast<AttractorController*>(object);

    // Sanity!
    AssertFatal(pController != NULL, "AttractorController::copyTo() - Object is not the correct type.");

    // Copy state.
    pController->setForce( getForce() );
}

//------------------------------------------------------------------------------

void AttractorController::integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Finish if the attractor would have no effect.
    if ( mIsZero( mForce ) || mIsZero( mRadius ) )
        return;

    // Fetch world query and clear results.
    WorldQuery* pWorldQuery = pScene->getWorldQuery( true );

    // Set filter.
    WorldQueryFilter queryFilter( 0xFFFFFFFF, 0xFFFFFFFF, true, false, true, true );
    pWorldQuery->setQueryFilter( queryFilter );

    // Calculate the AABB of the attractor.
    b2AABB aabb;
    aabb.lowerBound.Set( mPosition.x - mRadius, mPosition.y - mRadius );
    aabb.lowerBound.Set( mPosition.x + mRadius, mPosition.y + mRadius );

    // Query for candidate objects.
    pWorldQuery->anyQueryArea( aabb ); 

    // Fetch results.
    typeWorldQueryResultVector& queryResults = pWorldQuery->getQueryResults();

    // Add picked objects.
    for ( U32 n = 0; n < (U32)queryResults.size(); n++ )
    {
        // Fetch the scene object.
        SceneObject* pSceneObject = queryResults[n].mpSceneObject;

        // Ignore if it's a static body.
        if ( pSceneObject->getBodyType() == b2BodyType::b2_staticBody )
            continue;

        // Calculate the force direction to the controllers position.
        Vector2 forceDirection = mPosition - pSceneObject->getPosition();

        // Normalize to the specified force.
        forceDirection.Normalize( mForce );

        // Apply the force.
        pSceneObject->applyForce( forceDirection, true );
    }
}
