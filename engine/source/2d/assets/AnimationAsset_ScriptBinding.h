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

ConsoleMethod(AnimationAsset, setImage, void, 3, 3, "(assetId) Sets the image asset Id.\n"
                                                    "@return No return value.")
{
    object->setImage( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(AnimationAsset, getImage, const char*, 2, 2,  "() Gets the image asset Id.\n"
                                                            "@return The image asset Id.")
{
    return object->getImage().getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleMethod(AnimationAsset, setAnimationFrames, void, 3, 3,   "(animationFrames) Sets the image frames that compose the animation.\n"
                                                                "@param animationFrames A set of image frames that compose the animation.\n"
                                                                "@return No return value.")
{
    object->setAnimationFrames( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(AnimationAsset, getAnimationFrames, const char*, 2, 3,    "([bool validatedFrames]) Gets the frames that compose the animation or optionally only the ones validated against the image asset.\n"
                                                                        "@param validatedFrames - Whether to return only the validated frames or not.  Optional: Default is false.\n"
                                                                        "@return The image frames that compose the animation or optionally only the ones validated against the image asset.")
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

ConsoleMethod(AnimationAsset, getAnimationFrameCount, S32, 2, 3,    "([bool validatedFrames]) Gets the count of frame that compose the animation or optionally only the ones validated against the image asset.\n"
                                                                    "@param validatedFrames - Whether to return only the validated frames or not.  Optional: Default is false.\n"
                                                                    "@return The image frames that compose the animation or optionally only the ones validated against the image asset.")
{
    // Fetch validated frames flag.
    const bool validatedFrames = argc >= 3 ? dAtob( argv[2] ) : false;

    // Fetch specified frames.
    const Vector<S32>& frames = validatedFrames ? object->getValidatedAnimationFrames() : object->getSpecifiedAnimationFrames();

    return frames.size();
}

//-----------------------------------------------------------------------------

ConsoleMethod(AnimationAsset, setAnimationTime, void, 3, 3,      "(float animationTime) Sets the total time to cycle through all animation frames.\n"
                                                                        "@param animationTime The total time to cycle through all animation frames.\n"
                                                                        "@return No return value.")
{
    object->setAnimationTime( dAtof(argv[2] ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(AnimationAsset, getAnimationTime, F32, 2, 2,       "() Gets the total time to cycle through all animation frames.\n"
                                                                        "@return The total time to cycle through all animation frames.")
{
    return object->getAnimationTime();
}


//-----------------------------------------------------------------------------

ConsoleMethod(AnimationAsset, setAnimationCycle, void, 3, 3,     "(bool animationCycle) Sets whether the animation cycles or not.\n"
                                                                        "@param animationCycle Whether the animation cycles or not.\n"
                                                                        "@return No return value.")
{
    object->setAnimationCycle( dAtob(argv[2] ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(AnimationAsset, getAnimationCycle, bool, 2, 2,     "() Gets whether the animation cycles or not.\n"
                                                                        "@return Whether the animation cycles or not.")
{
    return object->getAnimationCycle();
}