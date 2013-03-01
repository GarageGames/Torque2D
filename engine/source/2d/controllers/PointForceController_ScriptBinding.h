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
