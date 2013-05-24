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

ConsoleMethodGroupBeginWithDocs(AmbientForceController, GroupedSceneController)

/*! Sets the ambient force to use.
    @param x The component of the ambient force along the horizontal (world) axis.
    @param y The component of the ambient force along the vertical (world) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(AmbientForceController, setForce, ConsoleVoid, 3, 4, (float x, float y))
{
    // The new force.
    b2Vec2 force;

    // Elements in the first argument.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ((elementCount == 2) && (argc == 3))
        force = Utility::mGetStringElementVector(argv[2]);

    // (x, y)
    else if ((elementCount == 1) && (argc == 4))
        force.Set(dAtof(argv[2]), dAtof(argv[3]));

    // Invalid
    else
    {
        Con::warnf("AmbientForceController::setForce() - Invalid number of parameters!");
        return;
    }

    object->setForce( force );
} 

//-----------------------------------------------------------------------------

/*! Gets the ambient force being used.
    @return The ambient force being used.
*/
ConsoleMethodWithDocs(AmbientForceController, getForce, ConsoleString, 2, 2, ())
{
    return object->getForce().scriptThis();
}

ConsoleMethodGroupEndWithDocs(AmbientForceController)
