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

#ifndef _CONSTANT_FORCE_CONTROLLER_H_
#include "2d/controllers/ConstantForceController.h"
#endif

// Script bindings.
#include "ConstantForceController_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(ConstantForceController);

//------------------------------------------------------------------------------

ConstantForceController::ConstantForceController()        
{
    // Reset the constant force.
    mForce.SetZero();
}

//------------------------------------------------------------------------------

ConstantForceController::~ConstantForceController()
{
}


//------------------------------------------------------------------------------

void ConstantForceController::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Force.
    addProtectedField("Force", TypeVector2, Offset( mForce, ConstantForceController), &defaultProtectedSetFn, &defaultProtectedGetFn, "The constant force to apply.");
}

//------------------------------------------------------------------------------

void ConstantForceController::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to controller.
    ConstantForceController* pController = static_cast<ConstantForceController*>(object);

    // Sanity!
    AssertFatal(pController != NULL, "ConstantForceController::copyTo() - Object is not the correct type.");

    // Copy state.
    pController->setForce( getForce() );
}

//------------------------------------------------------------------------------

void ConstantForceController::integrate( Scene* pScene, const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Process all the scene objects.
    for( SceneObjectSet::iterator itr = begin(); itr != end(); ++itr )
    {
        // Apply the force.
        (*itr)->applyForce( mForce, true );
    }
}
