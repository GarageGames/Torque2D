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

ConsoleMethod(PointForceController, setPosition, void, 3, 4,    "(float x, float y) - Sets the position of the force center.\n"
                                                                "@param x The position along the horizontal axis.\n"
                                                                "@param y The position along the vertical axis.\n"
                                                                "@return No return value.")
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

ConsoleMethod(PointForceController, PointForceController, const char*, 2, 2,    "() Gets the position of the force center.\n"
                                                                                "@return (float x/float y) The x and y (horizontal and vertical) position of the force center.")
{
    // Get position.
    return object->getPosition().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, setRadius, void, 3, 3,       "(radius) - Sets the radius of the point force to use.\n"
                                                                "@param radius The radius of the point force to use.\n"
                                                                "@return No return value.")
{
    object->setRadius( dAtof(argv[2]) );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, getRadius, F32, 2, 2,        "() Gets the radius of the point force being used.\n"
                                                                "@return The radius of the point force being used.")
{
    return object->getRadius();
}

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, setForce, void, 3, 3,       "(force) - Sets the point force to use.\n"
                                                                "@param force The point force to use.\n"
                                                                "@return No return value.")
{
    object->setForce( dAtof(argv[2]) );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, getForce, F32, 2, 2,        "() Gets the point force being used.\n"
                                                                "@return The point force being used.")
{
    return object->getForce();
}

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, setLinearDrag, void, 3, 3,  "(linearDrag) - Sets the linear drag coefficient (0.0 to 1.0).\n"
                                                                "@param linearDrag The linear drag coefficient\n"
                                                                "@return No return value.")
{
    object->setLinearDrag( dAtof(argv[2]) );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, getLinearDrag, F32, 2, 2,   "() Gets the linear drag coefficient.\n"
                                                                "@return The linear drag coefficient.")
{
    return object->getLinearDrag();
}

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, setAngularDrag, void, 3, 3, "(angularDrag) - Sets the angular drag coefficient (0.0 to 1.0).\n"
                                                                "@param angularDrag The angular drag coefficient\n"
                                                                "@return No return value.")
{
    object->setAngularDrag( dAtof(argv[2]) );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, getAngularDrag, F32, 2, 2,  "() Gets the angular drag coefficient.\n"
                                                                "@return The angular drag coefficient.")
{
    return object->getAngularDrag();
}

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, setNonLinear, void, 3, 3,   "(nonLinear) - Sets whether to apply the force non-linearly (using the inverse square law) or linearly.\n"
                                                                "@param nonLinear whether to apply the force non-linearly (using the inverse square law) or linearly.\n"
                                                                "@return No return value.")
{
    object->setNonLinear( dAtob(argv[2]) );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, getNonLinear, bool, 2, 2,  "() Gets whether to apply the force non-linearly (using the inverse square law) or linearly.\n"
                                                                "@return Whether to apply the force non-linearly (using the inverse square law) or linearly.")
{
    return object->getNonLinear();
}

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, setTrackedObject, void, 3, 3,   "(sceneObject) - Sets a scene object from which the position will be tracked.\n"
                                                                    "@param sceneObject The scene object from which the position will be tracked.  An empty string will stop tracking.\n"
                                                                    "@return No return value.")
{
    // Find the scene object.
    SceneObject* pSceneObject = Sim::findObject<SceneObject>( argv[2] );

    object->setTrackedObject( pSceneObject );
} 

//-----------------------------------------------------------------------------

ConsoleMethod(PointForceController, getTrackedObject, const char*, 2, 2,    "() - Gets the scene object from which the position will be tracked.\n"
                                                                            "@return The scene object from which the position will be tracked or an empty string if nothing is being tracked.")
{
    // Fetch the scene object.
    SceneObject* pSceneObject = object->getTrackedObject();

    return pSceneObject == NULL ? NULL : pSceneObject->getIdString();
} 

