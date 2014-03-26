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

ConsoleMethodGroupBeginWithDocs(SkeletonObject, SceneObject)

/*! Sets the skeleton asset Id to use.
    @param skeletonAssetId The skeleton asset Id to use.
    @return No return value.
*/
ConsoleMethodWithDocs(SkeletonObject, setSkeletonAsset, ConsoleVoid, 3, 3, (skeletonAssetId?))
{
    object->setSkeletonAsset( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the skeleton asset Id.
    @return The skeleton asset Id.
*/
ConsoleMethodWithDocs(SkeletonObject, getSkeletonAsset, ConsoleString, 2, 2, ())
{
    return object->getSkeletonAsset();
}

//-----------------------------------------------------------------------------

/*! Sets the animation for the object.
    @param animationName String containing animation name.
    @param cycle Optional bool to determine whether the animation should loop.
    @return Returns true on success."
*/
ConsoleMethodWithDocs(SkeletonObject, setAnimationName, ConsoleBool, 3, 4, (animationName, [cycle]))
{
    // Determine looping
    bool shouldLoop = argc >= 4 ? dAtob(argv[3]) : false;
    
    return object->setAnimationName(argv[2], shouldLoop);
}

//-----------------------------------------------------------------------------

/*! Gets the name of the current animation.
    @return String containing the animation name.
*/
ConsoleMethodWithDocs(SkeletonObject, getAnimationName, ConsoleString, 2, 2, ())
{
    return object->getAnimationName();
}

//-----------------------------------------------------------------------------

/*! Sets the skin for the skeleton.
    @return No return value.
*/
ConsoleMethodWithDocs(SkeletonObject, setSkin, ConsoleVoid, 3, 3, (skinName))
{
    object->setCurrentSkin(argv[2]);
}

//-----------------------------------------------------------------------------

/*! Gets the name of the current skin.
    @return String containing the skin name.
*/
ConsoleMethodWithDocs(SkeletonObject, getSkin, ConsoleString, 2, 2, ())
{
    return object->getCurrentSkin();
}

//-----------------------------------------------------------------------------

/*! Sets scaling of the skeleton's root bone.
    @param scaleX Base x coordinate scale.
    @param scaleY Base y coordinate scale.
    @return No return value.
*/
ConsoleMethodWithDocs(SkeletonObject, setRootBoneScale, ConsoleVoid, 3, 4, (float scaleX, float scaleY))
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
        Con::warnf("SkeletonObject::setRootBoneScale() - Invalid number of parameters!");
        return;
    }
    
    // Set Scale.
    object->setRootBoneScale(Vector2(scaleX, scaleY));
}

//-----------------------------------------------------------------------------

/*! Gets the skeleton's root bone scale.
    @return (float x/y height) The x and y scale of the object's root bone.
*/
ConsoleMethodWithDocs(SkeletonObject, getRootBoneScale, ConsoleString, 2, 2, ())
{
    return object->getRootBoneScale().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets local offset of the skeleton's root bone.
    @param x Base x coordinate.
    @param y Base y coordinate.
    @return No return value.
*/
ConsoleMethodWithDocs(SkeletonObject, setRootBoneOffset, ConsoleVoid, 3, 4, (float x, float y))
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
        Con::warnf("SkeletonObject::setRootBoneOffset() - Invalid number of parameters!");
        return;
    }
    
    // Set Size.
    object->setRootBoneOffset(Vector2(x, y));
}

//-----------------------------------------------------------------------------

/*! Gets the skeleton's root bone offset.
    @return (float x/y) The x and y offset of the object's root bone.
*/
ConsoleMethodWithDocs(SkeletonObject, getRootBoneOffset, ConsoleString, 2, 2, ())
{
    return object->getRootBoneOffset().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets whether the animation cycles or not.
    @param cycle Bool to determine whether the animation should loop.
    @return No return value.
*/
ConsoleMethodWithDocs(SkeletonObject, setAnimationCycle, ConsoleVoid, 3, 3, (bool cycle))
{
    object->setAnimationCycle( dAtob(argv[2] ) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the animation cycles or not.
    @return Whether the animation cycles or not.
*/
ConsoleMethodWithDocs(SkeletonObject, getAnimationCycle, ConsoleBool, 2, 2, ())
{
    return object->getAnimationCycle();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite texture flipping for each axis.
    @param flipX Whether or not to flip the texture along the x (horizontal) axis.
    @param flipY Whether or not to flip the texture along the y (vertical) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(SkeletonObject, setFlip, ConsoleVoid, 4, 4, (bool flipX, bool flipY))
{
    // Set Flip.
    object->setFlip( dAtob(argv[2]), dAtob(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Gets the flip for each axis.
    @return (bool flipX/bool flipY) Whether or not the texture is flipped along the x and y axis.
*/
ConsoleMethodWithDocs(SkeletonObject, getFlip, ConsoleString, 2, 2, ())
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getFlipX(), object->getFlipY());
    
    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets whether or not the texture is flipped horizontally.
    @param flipX Whether or not to flip the texture along the x (horizontal) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(SkeletonObject, setFlipX, ConsoleVoid, 3, 3, (bool flipX))
{
    // Set Flip.
    object->setFlipX( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Sets whether or not the texture is flipped vertically.
    @param flipY Whether or not to flip the texture along the y (vertical) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(SkeletonObject, setFlipY, ConsoleVoid, 3, 3, (bool flipY))
{
    // Set Flip.
    object->setFlipY( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether or not the texture is flipped horizontally.
    @return (bool flipX) Whether or not the texture is flipped along the x axis.
*/
ConsoleMethodWithDocs(SkeletonObject, getFlipX, ConsoleBool, 2, 2, ())
{
    return object->getFlipX();
}

//-----------------------------------------------------------------------------

/*! Gets whether or not the texture is flipped vertically.
    @return (bool flipY) Whether or not the texture is flipped along the y axis.
*/
ConsoleMethodWithDocs(SkeletonObject, getFlipY, ConsoleBool, 2, 2, ())
{
    return object->getFlipY();
}

//-----------------------------------------------------------------------------

/*! Gets the duration of the current animation.
    @return Duration of the animation in seconds.
*/
ConsoleMethodWithDocs(SkeletonObject, getAnimationDuration, ConsoleFloat, 2, 2, ())
{
    return object->getAnimationDuration();
}

//-----------------------------------------------------------------------------

/*! Mixes the current animation with another.
    @param animation The name of the animation to mix.
    @param time The time to start mixing.
*/
ConsoleMethodWithDocs(SkeletonObject, setMix, ConsoleBool, 5, 5, (fromAnimation, toAnimation, time))
{
    Con::printf("Mixing %s with %s at %f", argv[2], argv[3], dAtof(argv[4]));
    
    return object->setMix(argv[2], argv[3], dAtof(argv[4]));
}

ConsoleMethodGroupEndWithDocs(SkeletonObject)
