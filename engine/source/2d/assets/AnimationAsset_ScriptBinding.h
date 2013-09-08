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

ConsoleMethodGroupBeginWithDocs(AnimationAsset, AssetBase)

/*! Sets the image asset Id.
    @return No return value.
*/
ConsoleMethodWithDocs(AnimationAsset, setImage, ConsoleVoid, 3, 3, (assetId))
{
    object->setImage( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the image asset Id.
    @return The image asset Id.
*/
ConsoleMethodWithDocs(AnimationAsset, getImage, ConsoleString, 2, 2, ())
{
    return object->getImage().getAssetId();
}

//-----------------------------------------------------------------------------

/*! Sets the image frames that compose the animation.
    @param animationFrames A set of image frames that compose the animation.
    @return No return value.
*/
ConsoleMethodWithDocs(AnimationAsset, setAnimationFrames, ConsoleVoid, 3, 3, (animationFrames))
{
    object->setAnimationFrames( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the frames that compose the animation or optionally only the ones validated against the image asset.
    @param validatedFrames - Whether to return only the validated frames or not.  Optional: Default is false.
    @return The image frames that compose the animation or optionally only the ones validated against the image asset.
*/
ConsoleMethodWithDocs(AnimationAsset, getAnimationFrames, ConsoleString, 2, 3, ([bool validatedFrames]))
{
    // Fetch a return buffer.
    S32 bufferSize = 4096;
    char* pBuffer = Con::getReturnBuffer( bufferSize );
    char* pReturnBuffer = pBuffer;    

    // Fetch validated frames flag.
    const bool validatedFrames = argc >= 3 ? dAtob( argv[2] ) : false;

    // Fetch specified frames.
    const Vector<S32>& frames = validatedFrames ? object->getValidatedAnimationFrames() : object->getSpecifiedAnimationFrames();

    // Fetch frame count.
    const U32 frameCount = (U32)frames.size();

    // Format frames.
    for ( U32 frameIndex = 0; frameIndex < frameCount; ++frameIndex )
    {
        const S32 offset = dSprintf( pBuffer, bufferSize, "%d ", frames[frameIndex] );
        pBuffer += offset;
        bufferSize -= offset;
    }

    // Return frames.
    return pReturnBuffer;
}

//-----------------------------------------------------------------------------

/*! Gets the count of frame that compose the animation or optionally only the ones validated against the image asset.
    @param validatedFrames - Whether to return only the validated frames or not.  Optional: Default is false.
    @return The image frames that compose the animation or optionally only the ones validated against the image asset.
*/
ConsoleMethodWithDocs(AnimationAsset, getAnimationFrameCount, ConsoleInt, 2, 3, ([bool validatedFrames]))
{
    // Fetch validated frames flag.
    const bool validatedFrames = argc >= 3 ? dAtob( argv[2] ) : false;

    // Fetch specified frames.
    const Vector<S32>& frames = validatedFrames ? object->getValidatedAnimationFrames() : object->getSpecifiedAnimationFrames();

    return frames.size();
}

//-----------------------------------------------------------------------------

/*! Sets the total time to cycle through all animation frames.
    @param animationTime The total time to cycle through all animation frames.
    @return No return value.
*/
ConsoleMethodWithDocs(AnimationAsset, setAnimationTime, ConsoleVoid, 3, 3, (float animationTime))
{
    object->setAnimationTime( dAtof(argv[2] ) );
}

//-----------------------------------------------------------------------------

/*! Gets the total time to cycle through all animation frames.
    @return The total time to cycle through all animation frames.
*/
ConsoleMethodWithDocs(AnimationAsset, getAnimationTime, ConsoleFloat, 2, 2, ())
{
    return object->getAnimationTime();
}


//-----------------------------------------------------------------------------

/*! Sets whether the animation cycles or not.
    @param animationCycle Whether the animation cycles or not.
    @return No return value.
*/
ConsoleMethodWithDocs(AnimationAsset, setAnimationCycle, ConsoleVoid, 3, 3, (bool animationCycle))
{
    object->setAnimationCycle( dAtob(argv[2] ) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the animation cycles or not.
    @return Whether the animation cycles or not.
*/
ConsoleMethodWithDocs(AnimationAsset, getAnimationCycle, ConsoleBool, 2, 2, ())
{
    return object->getAnimationCycle();
}

ConsoleMethodGroupEndWithDocs(AnimationAsset)
