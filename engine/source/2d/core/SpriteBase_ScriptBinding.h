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

ConsoleMethodGroupBeginWithDocs(SpriteBase, SceneObject)

/*! Gets whether the sprite is in static or dynamic (animated)mode.
    @return Returns whether the sprite is in static or dynamic (animated)mode.
*/
ConsoleMethodWithDocs(SpriteBase, isStaticFrameProvider, ConsoleBool, 2, 2, ())
{
    return object->isStaticFrameProvider();
}

//------------------------------------------------------------------------------

/*! Sets the sprite image and optionally frame.
    @param imageAssetId The image asset Id to display
    @param frame The numerical or named frame of the image to display
    @return Returns true on success.
*/
ConsoleMethodWithDocs(SpriteBase, setImage, ConsoleBool, 3, 4, (imageAssetId, [frame]))
{
    // Was a frame specified?
    if (argc >= 4)
    {
        // Was it a number or a string?
        if (!dIsalpha(*argv[3]))
        {
            // Fetch the numerical frame and set the image
            const U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;
            return static_cast<ImageFrameProvider*>(object)->setImage( argv[2], frame );
        }
        else
        {
            // Set the image and pass the named frame string
            return static_cast<ImageFrameProvider*>(object)->setImage( argv[2], argv[3] );
        }
    }
    else
    {
        // Frame was not specified, use default 0 and set the image
        const U32 frame = 0;
        return static_cast<ImageFrameProvider*>(object)->setImage( argv[2], frame );
    }
}

//------------------------------------------------------------------------------

/*! Gets current image asset Id.
    @return (string imageAssetId) The image being displayed
*/
ConsoleMethodWithDocs(SpriteBase, getImage, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "SpriteBase::getImage() - Method invalid, not in static mode." );
        return StringTable->EmptyString;
    }

    // Get image.
    return static_cast<ImageFrameProvider*>(object)->getImage();
}   

//------------------------------------------------------------------------------

/*! Sets the image frame.
    @param frame The frame to display
    @return Returns true on success.
*/
ConsoleMethodWithDocs(SpriteBase, setImageFrame, ConsoleBool, 3, 3, (frame))
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "SpriteBase::setImageFrame() - Method invalid, not in static mode." );
        return false;
    }

    // Set image Frame.
    return static_cast<ImageFrameProvider*>(object)->setImageFrame( dAtoi(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets the current image Frame.
    @return The current image frame.
*/
ConsoleMethodWithDocs(SpriteBase, getImageFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "SpriteBase::getImageFrame() - Method invalid, not in static mode." );
        return -1;
    }

    // Get image Frame.
    return static_cast<ImageFrameProvider*>(object)->getImageFrame();
}

//------------------------------------------------------------------------------

/*! Sets the image frame using a string.
    @param frame - The name of the frame.
    @return True on success.
*/
ConsoleMethodWithDocs(SpriteBase, setImageFrameName, ConsoleBool, 3, 3,  (frame))
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "SpriteBase::setImageFrameName() - Method invalid, not in static mode." );
        return false;
    }
    
    // Set image Frame.
    return static_cast<ImageFrameProvider*>(object)->setImageFrameByName( argv[2] );
}

//------------------------------------------------------------------------------

/*! Gets the current image frame name.
    @return The current image frame name.
*/
ConsoleMethodWithDocs(SpriteBase, getImageFrameName, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "SpriteBase::getImageFrameName() - Method invalid, not in static mode." );
        return NULL;
    }
    
    // Get image Frame.
    return static_cast<ImageFrameProvider*>(object)->getImageFrameByName();
}

//------------------------------------------------------------------------------

/*! Plays an animation.
    @param animationAssetId The animation asset Id to play.
    @return Returns true on success.
*/
ConsoleMethodWithDocs(SpriteBase, playAnimation, ConsoleBool, 3, 3, (string animationAssetId))
{
    // Play Animation.
    return static_cast<ImageFrameProvider*>(object)->setAnimation( argv[2] );
}   

//-----------------------------------------------------------------------------

/*! Pause the current animation
    @param enable If true, pause the animation. If false, continue animating
*/
ConsoleMethodWithDocs(SpriteBase, pauseAnimation, ConsoleVoid, 3, 3, (bool enable))
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::pauseAnimation() - Method invalid, not in dynamic (animated) mode." );
        return;
    }

    return static_cast<ImageFrameProvider*>(object)->pauseAnimation(dAtob(argv[2]));
}


//-----------------------------------------------------------------------------

/*! Stop the current animation
    @return No return value.
*/
ConsoleMethodWithDocs(SpriteBase, stopAnimation, ConsoleVoid, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::stopAnimation() - Method invalid, not in dynamic (animated) mode." );
        return;
    }

    object->stopAnimation();
}

//-----------------------------------------------------------------------------

/*! Sets the current animation frame.
    @param frame Which frame of the animation to display
    @return No return value.
*/
ConsoleMethodWithDocs(SpriteBase, setAnimationFrame, ConsoleVoid, 3, 3, (int frame))
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::setAnimationFrame() - Method invalid, not in dynamic (animated) mode." );
        return;
    }

    // Set Animation Frame
    object->setAnimationFrame( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets current animation frame.
    @return (int frame) The current animation frame
*/
ConsoleMethodWithDocs(SpriteBase, getAnimationFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::getAnimationFrame() - Method invalid, not in dynamic (animated) mode." );
        return -1;
    }

    // Get Animation Frame.
    return object->getCurrentAnimationFrame();
}

//-----------------------------------------------------------------------------

/*! Gets current animation asset Id.
    @return (string AnimationAssetId) The current animation asset Id.
*/
ConsoleMethodWithDocs(SpriteBase, getAnimation, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::getAnimation() - Method invalid, not in dynamic (animated) mode." );
        return StringTable->EmptyString;
    }


    // Get Current Animation.
    return object->getCurrentAnimationAssetId();
}

//-----------------------------------------------------------------------------

/*! Gets current animation time.
    @return (float time) The current animation time
*/
ConsoleMethodWithDocs(SpriteBase, getAnimationTime, ConsoleFloat, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::getAnimationTime() - Method invalid, not in dynamic (animated) mode." );
        return 0.0f;
    }


    // Get Animation Time.
    return object->getCurrentAnimationTime();
}

//-----------------------------------------------------------------------------

/*! Checks animation status.
    @return (bool finished) Whether or not the animation is finished
*/
ConsoleMethodWithDocs(SpriteBase, getIsAnimationFinished, ConsoleBool, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::getIsAnimationFinished() - Method invalid, not in dynamic (animated) mode." );
        return true;
    }

    // Return Animation Finished Status.
    return object->isAnimationFinished();
}

//-----------------------------------------------------------------------------

/*! Change the rate of animation.
    @param timeScale Value which will scale the frame animation speed. 1 by default.
*/
ConsoleMethodWithDocs(SpriteBase, setAnimationTimeScale, ConsoleVoid, 3, 3, (float timeScale))
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::setAnimationTimeScale() - Method invalid, not in dynamic (animated) mode." );
        return;
    }

    object->setAnimationTimeScale(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the animation time scale for this sprite.
    @return (float) Returns the animation time scale for this sprite.
*/
ConsoleMethodWithDocs(SpriteBase, getAnimationTimeScale, ConsoleFloat, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "SpriteBase::getSpeedFactor() - Method invalid, not in dynamic (animated) mode." );
        return 1.0f;
    }

    return object->getAnimationTimeScale();
}

ConsoleMethodGroupEndWithDocs(SpriteBase)
