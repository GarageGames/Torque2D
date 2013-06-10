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

ConsoleMethod(SkeletonObject, setAnimation, bool, 3, 4, "(animationName, [isLooping]) - Sets the animation for the object.\n"
                                                        "@param animationName String containing animation name\n"
                                                        "@param isLooping Optional bool to determine whether the animation should loop\n"
                                                        "@return Returns true on success.")
{
    // Determine looping
    bool shouldLoop = argc >= 4 ? dAtob(argv[3]) : false;
    
    return object->setCurrentAnimation(argv[2], shouldLoop);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, getAnimation, const char*, 2, 2, "() - Gets the name of the current animation.\n"
                                                               "@return String containing the animation name")
{
    return object->getCurrentAnimation();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, setSkin, void, 3, 3, "(skinName) - Sets the skin for the skeleton.\n")
{
    object->setCurrentSkin(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, getSkin, const char*, 2, 2, "() - Gets the name of the current skin.\n"
                                                               "@return String containing the skin name")
{
    return object->getCurrentSkin();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, setSkeletonScale, void, 3, 4, "(float scaleX, float scaleY) Sets scaling of the skeleton's root bone.\n"
                                                            "@param scaleX Base x coordinate scale.\n"
                                                            "@param scaleY Base y coordinate scale.\n"
                                                            "@return No return value.")
{
    F32 scaleX, scaleY;

    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("width height")
    if ((elementCount == 2) && (argc == 3))
    {
        scaleX = dAtof(Utility::mGetStringElement(argv[2], 0));
        scaleY = dAtof(Utility::mGetStringElement(argv[2], 1));
    }

    // (width, [height])
    else if (elementCount == 1)
    {
        scaleX = dAtof(argv[2]);

        if (argc > 3)
            scaleY = dAtof(argv[3]);
        else
            scaleY = scaleX;
    }

    // Invalid
    else
    {
        Con::warnf("SkeletonObject::setSkeletonScale() - Invalid number of parameters!");
        return;
    }
    
    // Set Size.
    object->setSkeletonScale(Vector2(scaleX, scaleY));
}  

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, getSkeletonScale, const char*, 2, 2, "() Gets the skeleton's root bone scale.\n"
                                                                   "@return (float x/y height) The x and y scale of the object's root bone.")
{
    return object->getSkeletonScale().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, setSkeletonOffset, void, 3, 4, "(float x, float y) Sets local offset of the skeleton's root bone.\n"
                                                            "@param x Base x coordinate.\n"
                                                            "@param y Base y coordinate.\n"
                                                            "@return No return value.")
{
    F32 x, y;

    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ((elementCount == 2) && (argc == 3))
    {
        x = dAtof(Utility::mGetStringElement(argv[2], 0));
        y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }

    // (x, [y])
    else if (elementCount == 1)
    {
        x = dAtof(argv[2]);

        if (argc > 3)
            y = dAtof(argv[3]);
        else
            y = x;
    }

    // Invalid
    else
    {
        Con::warnf("SkeletonObject::setSkeletonOffset() - Invalid number of parameters!");
        return;
    }
    
    // Set Size.
    object->setSkeletonOffset(Vector2(x, y));
}  

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, getSkeletonOffset, const char*, 2, 2, "() Gets the skeleton's root bone offset.\n"
                                                                   "@return (float x/y) The x and y offset of the object's root bone.")
{
    return object->getSkeletonOffset().scriptThis();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, getAnimationDuration, F32, 2, 2, "() Gets the duration of the current animation.\n"
                                                               "@return Duration of the animation in seconds.\n")
{
    return object->getAnimationDuration();
}

//-----------------------------------------------------------------------------

ConsoleMethod(SkeletonObject, setMix, bool, 5, 5, "(fromAnimation, toAnimatino, time) Mixes the current animation with another.\n"
                                                "@param animation The name of the animation to mix.\n"
                                                "@param time The time to start mixing.")
{
    Con::printf("Mixing %s with %s at %f", argv[2], argv[3], dAtof(argv[4]));
    
    return object->setMix(argv[2], argv[3], dAtof(argv[4]));
    
}