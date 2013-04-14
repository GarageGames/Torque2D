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

ConsoleMethod( GuiSpriteCtrl, isStaticFrameProvider, bool, 2, 2, "() - Gets whether the control is in static or dynamic (animated)mode.\n"
                                                        "@return Returns whether the control is in static or dynamic (animated)mode.")
{
    return object->isStaticFrameProvider();
}

//-----------------------------------------------------------------------------

ConsoleMethod( GuiSpriteCtrl, setImage, void, 3, 3, "(imageAssetId) Sets the image asset Id to use as the image.\n"
                                                    "@param imageAssetId The image asset Id to use as the image.\n"
                                                    "@return No return value.")
{
   object->setImage( argv[2] );
}

//------------------------------------------------------------------------------

ConsoleMethod( GuiSpriteCtrl, getImage, const char*, 2, 2,  "() - Gets current image asset Id.\n"
                                                            "@return (string imageAssetId) The image being displayed.")
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

ConsoleMethod( GuiSpriteCtrl, setImageFrame, void, 3, 3,    "(int imageFrame) Sets the image frame to use as the image.\n"
                                                            "@param imageFrame The image frame to use as the image.\n"
                                                            "@return No return value.")
{
   object->setImageFrame( dAtoi(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod( GuiSpriteCtrl, getImageFrame, S32, 2, 2, "() - Gets current image Frame.\n"
                                                        "@return (int frame) The frame currently being displayed.")
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

ConsoleMethod( GuiSpriteCtrl, setAnimation, void, 3, 3, "(string animationAssetId) - Sets the animation asset Id to display.\n"
                                                        "@param animationAssetId The animation asset Id to play\n"
                                                        "@return No return value.")
{    
    // Set animation.
    object->setAnimation( argv[2] );
}

//------------------------------------------------------------------------------

ConsoleMethod( GuiSpriteCtrl, getAnimation, const char*, 2, 2,  "() - Gets the current animation asset Id.\n"
                                                                "@return (string ianimationAssetId) The animation being displayed.")
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
