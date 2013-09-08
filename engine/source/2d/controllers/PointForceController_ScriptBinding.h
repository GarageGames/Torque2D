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

ConsoleMethodGroupBeginWithDocs(PointForceController, PickingSceneController)

/*! Sets the position of the force center.
    @param x The position along the horizontal axis.
    @param y The position along the vertical axis.
    @return No return value.
*/
ConsoleMethodWithDocs(PointForceController, setPosition, ConsoleVoid, 3, 4, (float x, float y))
{
    // The new position.
    b2Vec2 position;

    // Elements in the first argument.
    U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ((elementCount == 2) && (argc == 3))
        position = Utility::mGetStringElementVector(argv[2]);

    // (x, y)
    else if ((elementCount == 1) && (argc == 4))
        position.Set(dAtof(argv[2]), dAtof(argv[3]));

    // Invalid
    else
    {
        Con::warnf("PointForceController::setPosition() - Invalid number of parameters!");
        return;
    }

    // Set Position.
    object->setPosition(position);
}

//-----------------------------------------------------------------------------

/*! Gets the position of the force center.
    @return (float x/float y) The x and y (horizontal and vertical) position of the force center.
*/
ConsoleMethodWithDocs(PointForceController, getPosition, ConsoleString, 2, 2, ())
{
    // Get position.
    return object->getPosition().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the radius of the point force to use.
    @param radius The radius of the point force to use.
    @return No return value.
*/
ConsoleMethodWithDocs(PointForceController, setRadius, ConsoleVoid, 3, 3, (radius))
{
    object->setRadius( dAtof(argv[2]) );
} 

//-----------------------------------------------------------------------------

/*! Gets the radius of the point force being used.
    @return The radius of the point force being used.
*/
ConsoleMethodWithDocs(PointForceController, getRadius, ConsoleFloat, 2, 2, ())
{
    return object->getRadius();
}

//-----------------------------------------------------------------------------

/*! Sets the point force to use.
    @param force The point force to use.
    @return No return value.
*/
ConsoleMethodWithDocs(PointForceController, setForce, ConsoleVoid, 3, 3, (force))
{
    object->setForce( dAtof(argv[2]) );
} 

//-----------------------------------------------------------------------------

/*! Gets the point force being used.
    @return The point force being used.
*/
ConsoleMethodWithDocs(PointForceController, getForce, ConsoleFloat, 2, 2, ())
{
    return object->getForce();
}

//-----------------------------------------------------------------------------

/*! Sets the linear drag coefficient (0.0 to 1.0).
    @param linearDrag The linear drag coefficient
    @return No return value.
*/
ConsoleMethodWithDocs(PointForceController, setLinearDrag, ConsoleVoid, 3, 3, (linearDrag))
{
    object->setLinearDrag( dAtof(argv[2]) );
} 

//-----------------------------------------------------------------------------

/*! Gets the linear drag coefficient.
    @return The linear drag coefficient.
*/
ConsoleMethodWithDocs(PointForceController, getLinearDrag, ConsoleFloat, 2, 2, ())
{
    return object->getLinearDrag();
}

//-----------------------------------------------------------------------------

/*! Sets the angular drag coefficient (0.0 to 1.0).
    @param angularDrag The angular drag coefficient
    @return No return value.
*/
ConsoleMethodWithDocs(PointForceController, setAngularDrag, ConsoleVoid, 3, 3, (angularDrag))
{
    object->setAngularDrag( dAtof(argv[2]) );
} 

//-----------------------------------------------------------------------------

/*! Gets the angular drag coefficient.
    @return The angular drag coefficient.
*/
ConsoleMethodWithDocs(PointForceController, getAngularDrag, ConsoleFloat, 2, 2, ())
{
    return object->getAngularDrag();
}

//-----------------------------------------------------------------------------

/*! Sets whether to apply the force non-linearly (using the inverse square law) or linearly.
    @param nonLinear whether to apply the force non-linearly (using the inverse square law) or linearly.
    @return No return value.
*/
ConsoleMethodWithDocs(PointForceController, setNonLinear, ConsoleVoid, 3, 3, (nonLinear))
{
    object->setNonLinear( dAtob(argv[2]) );
} 

//-----------------------------------------------------------------------------

/*! Gets whether to apply the force non-linearly (using the inverse square law) or linearly.
    @return Whether to apply the force non-linearly (using the inverse square law) or linearly.
*/
ConsoleMethodWithDocs(PointForceController, getNonLinear, ConsoleBool, 2, 2, ())
{
    return object->getNonLinear();
}

//-----------------------------------------------------------------------------

/*! Sets a scene object from which the position will be tracked.
    @param sceneObject The scene object from which the position will be tracked.  An empty string will stop tracking.
    @return No return value.
*/
ConsoleMethodWithDocs(PointForceController, setTrackedObject, ConsoleVoid, 3, 3, (sceneObject))
{
    // Find the scene object.
    SceneObject* pSceneObject = Sim::findObject<SceneObject>( argv[2] );

    object->setTrackedObject( pSceneObject );
} 

//-----------------------------------------------------------------------------

/*! Gets the scene object from which the position will be tracked.
    @return The scene object from which the position will be tracked or an empty string if nothing is being tracked.
*/
ConsoleMethodWithDocs(PointForceController, getTrackedObject, ConsoleString, 2, 2, ())
{
    // Fetch the scene object.
    SceneObject* pSceneObject = object->getTrackedObject();

    return pSceneObject == NULL ? NULL : pSceneObject->getIdString();
} 

ConsoleMethodGroupEndWithDocs(PointForceController)