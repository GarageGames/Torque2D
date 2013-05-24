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

ConsoleMethodGroupBeginWithDocs(GuiSpriteCtrl, GuiControl)

/*! Gets whether the control is in static or dynamic (animated)mode.
    @return Returns whether the control is in static or dynamic (animated)mode.
*/
ConsoleMethodWithDocs( GuiSpriteCtrl, isStaticFrameProvider, ConsoleBool, 2, 2, ())
{
    return object->isStaticFrameProvider();
}

//-----------------------------------------------------------------------------

/*! Sets the image asset Id to use as the image.
    @param imageAssetId The image asset Id to use as the image.
    @return No return value.
*/
ConsoleMethodWithDocs( GuiSpriteCtrl, setImage, ConsoleVoid, 3, 3, (imageAssetId))
{
   object->setImage( argv[2] );
}

//------------------------------------------------------------------------------

/*! Gets current image asset Id.
    @return (string imageAssetId) The image being displayed.
*/
ConsoleMethodWithDocs( GuiSpriteCtrl, getImage, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "GuiSpriteCtrl::getImage() - Method invalid, not in static mode." );
        return StringTable->EmptyString;
    }

    // Get image.
    return DYNAMIC_VOID_CAST_TO(GuiSpriteCtrl, ImageFrameProvider, object)->getImage();
}

//-----------------------------------------------------------------------------

/*! Sets the image frame to use as the image.
    @param imageFrame The image frame to use as the image.
    @return No return value.
*/
ConsoleMethodWithDocs( GuiSpriteCtrl, setImageFrame, ConsoleVoid, 3, 3, (int imageFrame))
{
   object->setImageFrame( dAtoi(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets current image Frame.
    @return (int frame) The frame currently being displayed.
*/
ConsoleMethodWithDocs( GuiSpriteCtrl, getImageFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "GuiSpriteCtrl::getFrame() - Method invalid, not in static mode." );
        return -1;
    }

    // Get image frame.
    return object->getImageFrame();
}

//------------------------------------------------------------------------------

/*! Sets the animation asset Id to display.
    @param animationAssetId The animation asset Id to play
    @return No return value.
*/
ConsoleMethodWithDocs( GuiSpriteCtrl, setAnimation, ConsoleVoid, 3, 3, (string animationAssetId))
{
    // Set animation.
    object->setAnimation( argv[2] );
}

//------------------------------------------------------------------------------

/*! Gets the current animation asset Id.
    @return (string ianimationAssetId) The animation being displayed.
*/
ConsoleMethodWithDocs( GuiSpriteCtrl, getAnimation, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf( "GuiSpriteCtrl::getAnimation() - Method invalid, in static mode." );
        return StringTable->EmptyString;
    }

    // Get animation.
    return DYNAMIC_VOID_CAST_TO(GuiSpriteCtrl, ImageFrameProvider, object)->getAnimation();
}

ConsoleMethodGroupEndWithDocs(GuiSpriteCtrl)
