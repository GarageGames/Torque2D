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

#ifndef _SCENE_CONTROLLER_H_
#include "2d/controllers/SceneController.h"
#endif

#ifndef _SCENE_OBJECT_SET_H_
#include "2d/sceneObject/sceneObjectSet.h"
#endif

#ifndef _SCENE_H_
#include "2d/scene/scene.h"
#endif

// Script bindings.
#include "SceneController_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SceneController);

//------------------------------------------------------------------------------

SceneController::SceneController()
{
}

//------------------------------------------------------------------------------

SceneController::~SceneController()
{
}


//------------------------------------------------------------------------------

void SceneController::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

}

//------------------------------------------------------------------------------

void SceneController::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to controller.
    SceneController* pController = static_cast<SceneController*>(object);

    // Sanity!
    AssertFatal(pController != NULL, "SceneController::copyTo() - Object is not the correct type.");
}

