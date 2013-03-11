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

#ifndef PickingSceneController
#include "2d/controllers/core/PickingSceneController.h"
#endif

// Script bindings.
#include "2d/controllers/core/PickingSceneController_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(PickingSceneController);

//------------------------------------------------------------------------------

PickingSceneController::PickingSceneController() :
        mControlGroupMask( MASK_ALL ),
        mControlLayerMask( MASK_ALL )
{
}

//------------------------------------------------------------------------------

PickingSceneController::~PickingSceneController()
{
}

//------------------------------------------------------------------------------

void PickingSceneController::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to controller.
    PickingSceneController* pController = static_cast<PickingSceneController*>(object);

    // Sanity!
    AssertFatal(pController != NULL, "PickingSceneController::copyTo() - Object is not the correct type.");

    // Set masks.
    pController->setControlGroupMask( getControlGroupMask() );
    pController->setControlLayerMask( getControlLayerMask() );
}

//------------------------------------------------------------------------------

WorldQuery* PickingSceneController::prepareQueryFilter( Scene* pScene, const bool clearQuery )
{
    // Fetch world query and clear results.
    WorldQuery* pWorldQuery = pScene->getWorldQuery( clearQuery );

    // Set filter.
    WorldQueryFilter queryFilter( mControlLayerMask, mControlGroupMask, true, false, true, true );
    pWorldQuery->setQueryFilter( queryFilter );

    return pWorldQuery;
}

