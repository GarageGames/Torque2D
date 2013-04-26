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

ConsoleMethod(SkeletonObject, setAnimation, void, 3, 3, "(animationName) - Sets the animation for the object.\n")
{
    object->setCurrentAnimation(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, setSkin, void, 3, 3, "(skinName) - Sets the skin for the skeleton.\n")
{
    object->setCurrentSkin(argv[2]);
}

// BOZO - What goes here?
// This file is reserved for creating functions that will expose
// For example, how the CompositeSprite::clearSprites function is
// exposed to script:
/*
ConsoleMethod(CompositeSprite, clearSprites, void, 2, 2,    "() - Removes all sprites.\n"
                                                            "@return No return value." )
{
    return object->clearSprites();
}
*/

// The ConsoleMethod has a strict format
/*
ConsoleMethod(Class, function, returnType, minimumParameters, maxParameters, "Usage String")
{
   // Access parameters via argv[n]. They are always strings, so you will have to
   // convert acordingly (dAtob to conver to bool, dAtof to conver to float, etc)
   // argv will always have contents for index 0 and index 1

   // The class string
   argv[0];

   // The function string
   argv[1];

   // Optional passed parameters
   argv[2-n];

   // Access the instance using "object"
   object->function();
}
*/

// For our purposes, we might want to allow a user to change the skin of their object
// So it might be something like this:
/*
ConsoleMethod(SkeletonObject, setSkin, void, 3, 3,    "(skinName) - Changes the skin of the object.\n"
                                                            "@return No return value." )
{
    object->setSkin(argv[2]);
}
*/