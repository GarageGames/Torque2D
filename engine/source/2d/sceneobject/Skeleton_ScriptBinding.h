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

ConsoleMethodGroupBeginWithDocs(Skeleton, SceneObject)

/*! Sets the skeleton asset Id to use.
    @param skeletonAssetId The skeleton asset Id to use.
    @return No return value.
*/
ConsoleMethodWithDocs(Skeleton, setSkeletonAsset, ConsoleVoid, 3, 3, (skeletonAssetId?))
{
    object->setSkeletonAsset( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the skeleton asset Id.
    @return The skeleton asset Id.
*/
ConsoleMethodWithDocs(Skeleton, getSkeletonAsset, ConsoleString, 2, 2, ())
{
    return object->getSkeletonAsset();
}

//-----------------------------------------------------------------------------

/*! Sets the animation for the object.
    @param animationName String containing animation name.
    @param cycle Optional bool to determine whether the animation should loop.
    @return Returns true on success."
*/
ConsoleMethodWithDocs(Skeleton, setAnimation, ConsoleBool, 3, 4, (animationName, [cycle]))
{
    // Determine looping
    bool shouldLoop = argc >= 4 ? dAtob(argv[3]) : false;
    
    return object->setCurrentAnimation(argv[2], shouldLoop);
}

//-----------------------------------------------------------------------------

/*! Gets the name of the current animation.
    @return String containing the animation name.
*/
ConsoleMethodWithDocs(Skeleton, getAnimation, ConsoleString, 2, 2, ())
{
    return object->getCurrentAnimation();
}

//-----------------------------------------------------------------------------

/*! Sets the skin for the skeleton.
    @return No return value.
*/
ConsoleMethodWithDocs(Skeleton, setSkin, ConsoleVoid, 3, 3, (skinName))
{
    object->setCurrentSkin(argv[2]);
}

//-----------------------------------------------------------------------------

/*! Gets the name of the current skin.
    @return String containing the skin name.
*/
ConsoleMethodWithDocs(Skeleton, getSkin, ConsoleString, 2, 2, ())
{
    return object->getCurrentSkin();
}

//-----------------------------------------------------------------------------

/*! Sets scaling of the skeleton's root bone.
    @param scaleX Base x coordinate scale.
    @param scaleY Base y coordinate scale.
    @return No return value.
*/
ConsoleMethodWithDocs(Skeleton, setRootBoneScale, ConsoleVoid, 3, 4, (float scaleX, float scaleY))
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
        Con::warnf("Skeleton::setRootBoneScale() - Invalid number of parameters!");
        return;
    }
    
    // Set Size.
    object->setSkeletonScale(Vector2(scaleX, scaleY));
}  

//-----------------------------------------------------------------------------

/*! Gets the skeleton's root bone scale.
    @return (float x/y height) The x and y scale of the object's root bone.
*/
ConsoleMethodWithDocs(Skeleton, getRootBoneScale, ConsoleString, 2, 2, ())
{
    return object->getSkeletonScale().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets local offset of the skeleton's root bone.
    @param x Base x coordinate.
    @param y Base y coordinate.
    @return No return value.
*/
ConsoleMethodWithDocs(Skeleton, setRootBoneOffset, ConsoleVoid, 3, 4, (float x, float y))
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
        Con::warnf("Skeleton::setRootBoneOffset() - Invalid number of parameters!");
        return;
    }
    
    // Set Size.
    object->setSkeletonOffset(Vector2(x, y));
}  

//-----------------------------------------------------------------------------

/*! Gets the skeleton's root bone offset.
    @return (float x/y) The x and y offset of the object's root bone.
*/
ConsoleMethodWithDocs(Skeleton, getRootBoneOffset, ConsoleString, 2, 2, ())
{
    return object->getSkeletonOffset().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets the duration of the current animation.
    @return Duration of the animation in seconds.
*/
ConsoleMethodWithDocs(Skeleton, getAnimationDuration, ConsoleFloat, 2, 2, ())
{
    return object->getAnimationDuration();
}

//-----------------------------------------------------------------------------

/*! Mixes the current animation with another.
    @param animation The name of the animation to mix.
    @param time The time to start mixing.
*/
ConsoleMethodWithDocs(Skeleton, setMix, ConsoleBool, 5, 5, (fromAnimation, toAnimation, time))
{
    Con::printf("Mixing %s with %s at %f", argv[2], argv[3], dAtof(argv[4]));
    
    return object->setMix(argv[2], argv[3], dAtof(argv[4]));
}

ConsoleMethodGroupEndWithDocs(Skeleton)