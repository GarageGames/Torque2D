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

#ifndef _GROUPED_SCENE_CONTROLLER_H_
#include "2d/controllers/core/GroupedSceneController.h"
#endif

// Script bindings.
#include "2d/controllers/core/GroupedSceneController_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GroupedSceneController);

//------------------------------------------------------------------------------

void GroupedSceneController::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to controller.
    GroupedSceneController* pController = static_cast<GroupedSceneController*>(object);

    // Sanity!
    AssertFatal(pController != NULL, "GroupedSceneController::copyTo() - Object is not the correct type.");

    // Add objects to the controller.
    for( SceneObjectSet::iterator objectItr = begin(); objectItr != end(); ++objectItr )
        pController->addObject( *objectItr );
}

