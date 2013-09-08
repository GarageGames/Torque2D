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

ConsoleMethodGroupBeginWithDocs(RenderProxy, SimObject)

/*! Gets whether the render-proxy is in static or dynamic (animated)mode.
    @return Returns whether the render-proxy is in static or dynamic (animated)mode.
*/
ConsoleMethodWithDocs(RenderProxy, isStaticFrameProvider, ConsoleBool, 2, 2, ())
{
    return object->isStaticFrameProvider();
}

//------------------------------------------------------------------------------

/*! Sets imageAssetId/Frame.
    @param imageAssetId The image asset Id to display
    @param frame The frame of the image to display
    @return Returns true on success.
*/
ConsoleMethodWithDocs(RenderProxy, setImage, ConsoleBool, 3, 4, (string imageAssetId, [int frame]))
{
    // Fetch image asset Id.
    const char* pImageAssetId = argv[2];

    // Calculate Frame.
    const U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;

    // Set Image.
    return static_cast<ImageFrameProvider*>(object)->setImage(pImageAssetId, frame );
}   

//------------------------------------------------------------------------------

/*! Gets current image asset Id.
    @return (string imageAssetId) The image being displayed
*/
ConsoleMethodWithDocs(RenderProxy, getImage, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "RenderProxy::getImage() - Method invalid, not in static mode." );
        return StringTable->EmptyString;
    }

    // Get Image.
    return static_cast<ImageFrameProvider*>(object)->getImage();
}   

//------------------------------------------------------------------------------

/*! Sets image frame.
    @param frame The frame to display
    @return Returns true on success.
*/
ConsoleMethodWithDocs(RenderProxy, setImageFrame, ConsoleBool, 3, 3, (int frame))
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "RenderProxy::setImageFrame() - Method invalid, not in static mode." );
        return false;
    }

    // Set Image Frame.
    return static_cast<ImageFrameProvider*>(object)->setImageFrame( dAtoi(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets current image Frame.
    @return (int frame) The frame currently being displayed
*/
ConsoleMethodWithDocs(RenderProxy, getImageFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "RenderProxy::getImageFrame() - Method invalid, not in static mode." );
        return -1;
    }

    // Get Image Frame.
    return object->getImageFrame();
}

//------------------------------------------------------------------------------

/*! Plays an animation.
    @param animationAssetId The animation asset Id to play
    @return Returns true on success.
*/
ConsoleMethodWithDocs(RenderProxy, playAnimation, ConsoleBool, 3, 3, (string animationAssetId))
{
    // Play Animation.
    return static_cast<ImageFrameProvider*>(object)->setAnimation( argv[2] );
}   

//-----------------------------------------------------------------------------

/*! Pause the current animation
    @param enable If true, pause the animation. If false, continue animating
*/
ConsoleMethodWithDocs(RenderProxy, pauseAnimation, ConsoleVoid, 3, 3, (bool enable))
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::pauseAnimation() - Method invalid, not in dynamic (animated) mode." );
        return;
    }

    static_cast<ImageFrameProvider*>(object)->pauseAnimation(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Stop the current animation
    @return No return value.
*/
ConsoleMethodWithDocs(RenderProxy, stopAnimation, ConsoleVoid, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::stopAnimation() - Method invalid, not in dynamic (animated) mode." );
        return;
    }

    object->stopAnimation();
}

//-----------------------------------------------------------------------------

/*! Sets the current animation frame.
    @param frame Which frame of the animation to display
    @return No return value.
*/
ConsoleMethodWithDocs(RenderProxy, setAnimationFrame, ConsoleVoid, 3, 3, (int frame))
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::setAnimationFrame() - Method invalid, not in dynamic (animated) mode." );
        return;
    }

    // Set Animation Frame
    object->setAnimationFrame( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets current animation frame.
    @return (int frame) The current animation frame
*/
ConsoleMethodWithDocs(RenderProxy, getAnimationFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::getAnimationFrame() - Method invalid, not in dynamic (animated) mode." );
        return -1;
    }

    // Get Animation Frame.
    return object->getCurrentAnimationFrame();
}

//-----------------------------------------------------------------------------

/*! Gets current animation asset Id.
    @return (string AnimationAssetId) The current animation asset Id.
*/
ConsoleMethodWithDocs(RenderProxy, getAnimation, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::getAnimation() - Method invalid, not in dynamic (animated) mode." );
        return StringTable->EmptyString;
    }


    // Get Current Animation.
    return object->getCurrentAnimationAssetId();
}

//-----------------------------------------------------------------------------

/*! Gets current animation time.
    @return (float time) The current animation time
*/
ConsoleMethodWithDocs(RenderProxy, getAnimationTime, ConsoleFloat, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::getAnimationTime() - Method invalid, not in dynamic (animated) mode." );
        return 0.0f;
    }


    // Get Animation Time.
    return object->getCurrentAnimationTime();
}

//-----------------------------------------------------------------------------

/*! Checks animation status.
    @return (bool finished) Whether or not the animation is finished
*/
ConsoleMethodWithDocs(RenderProxy, getIsAnimationFinished, ConsoleBool, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::getIsAnimationFinished() - Method invalid, not in dynamic (animated) mode." );
        return true;
    }

    // Return Animation Finished Status.
    return object->isAnimationFinished();
}

//-----------------------------------------------------------------------------

/*! Change the rate of animation.
    @param timeScale Value which will scale the frame animation speed. 1 by default.
*/
ConsoleMethodWithDocs(RenderProxy, setAnimationTimeScale, ConsoleVoid, 3, 3, (float timeScale))
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::setAnimationTimeScale() - Method invalid, not in dynamic (animated) mode." );
        return;
    }

    object->setAnimationTimeScale(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the animation time scale for this render-proxy.
    @return (float) Returns the animation time scale for this render-proxy.
*/
ConsoleMethodWithDocs(RenderProxy, getAnimationTimeScale, ConsoleFloat, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "RenderProxy::getSpeedFactor() - Method invalid, not in dynamic (animated) mode." );
        return 1.0f;
    }

    return object->getAnimationTimeScale();
}

ConsoleMethodGroupEndWithDocs(RenderProxy)
