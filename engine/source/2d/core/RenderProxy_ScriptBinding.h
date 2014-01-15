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

ConsoleMethod(RenderProxy, isStaticFrameProvider, bool, 2, 2,    "() - Gets whether the render-proxy is in static or dynamic (animated)mode.\n"
                                                        "@return Returns whether the render-proxy is in static or dynamic (animated)mode.")
{
    return object->isStaticFrameProvider();
}

//------------------------------------------------------------------------------

ConsoleMethod(RenderProxy, setImage, bool, 3, 4,    "(string imageAssetId, [int frame]) - Sets imageAssetId/Frame.\n"
                                                    "@param imageAssetId The image asset Id to display\n"
                                                    "@param frame The frame of the image to display\n"
                                                    "@return Returns true on success.")
{
    // Fetch image asset Id.
    const char* pImageAssetId = argv[2];

    // Calculate Frame.
    const U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;

    // Set Image.
    return static_cast<ImageFrameProvider*>(object)->setImage(pImageAssetId, frame );
}   

//------------------------------------------------------------------------------

ConsoleMethod(RenderProxy, getImage, const char*, 2, 2, "() - Gets current image asset Id.\n"
                                                        "@return (string imageAssetId) The image being displayed")
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

ConsoleMethod(RenderProxy, setImageFrame, bool, 3, 3,   "(int frame) - Sets image frame.\n"
                                                        "@param frame The frame to display\n"
                                                        "@return Returns true on success.")
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

ConsoleMethod(RenderProxy, getImageFrame, S32, 2, 2,    "() - Gets current image Frame.\n"
                                                        "@return (int frame) The frame currently being displayed")
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

ConsoleMethod(RenderProxy, playAnimation, bool, 3, 3,   "(string animationAssetId) - Plays an animation.\n"
                                                        "@param animationAssetId The animation asset Id to play\n"
                                                        "@return Returns true on success.")
{    
    // Play Animation.
    return static_cast<ImageFrameProvider*>(object)->setAnimation( argv[2] );
}   

//-----------------------------------------------------------------------------

ConsoleMethod(RenderProxy, pauseAnimation, void, 3, 3,  "(bool enable) - Pause the current animation\n"
                                                        "@param enable If true, pause the animation. If false, continue animating\n")
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

ConsoleMethod(RenderProxy, stopAnimation, void, 2, 2,   "() - Stop the current animation\n"
                                                        "@return No return value.")
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

ConsoleMethod(RenderProxy, setAnimationFrame, void, 3, 3, "(int frame) - Sets the current animation frame.\n"
                                                                "@param frame Which frame of the animation to display\n"
                                                                "@return No return value.")
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

ConsoleMethod(RenderProxy, getAnimationFrame, S32, 2, 2,    "() - Gets current animation frame.\n"
                                                            "@return (int frame) The current animation frame")
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

ConsoleMethod(RenderProxy, getAnimation, const char*, 2, 2, "() - Gets current animation asset Id.\n"
                                                            "@return (string AnimationAssetId) The current animation asset Id.")
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

ConsoleMethod(RenderProxy, getAnimationTime, F32, 2, 2, "() - Gets current animation time.\n"
                                                        "@return (float time) The current animation time")
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

ConsoleMethod(RenderProxy, getIsAnimationFinished, bool, 2, 2,  "() - Checks animation status.\n"
                                                                "@return (bool finished) Whether or not the animation is finished")
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

ConsoleMethod(RenderProxy, setAnimationTimeScale, void, 3, 3,   "(float timeScale) - Change the rate of animation.\n"
                                                                "@param timeScale Value which will scale the frame animation speed. 1 by default.\n")
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

ConsoleMethod(RenderProxy, getAnimationTimeScale, F32, 2, 2,    "() - Get the animation time scale for this render-proxy.\n"
                                                                "@return (float) Returns the animation time scale for this render-proxy.\n")
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

