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

#ifndef _GUISPRITECTRL_H_
#include "2d/gui/guiSpriteCtrl.h"
#endif

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#include "guiSpriteCtrl_ScriptBindings.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiSpriteCtrl);

//-----------------------------------------------------------------------------

GuiSpriteCtrl::GuiSpriteCtrl( void ) :
    mImageAssetId( StringTable->EmptyString ),
    mAnimationAssetId( StringTable->EmptyString )
{
	// Set to self ticking.
	mSelfTick = true;
}

//-----------------------------------------------------------------------------

GuiSpriteCtrl::~GuiSpriteCtrl()
{
}

//-----------------------------------------------------------------------------

void GuiSpriteCtrl::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField( "Image", TypeAssetId, Offset(mImageAssetId, GuiSpriteCtrl), &setImage, &getImage, &writeImage, "The image asset Id used for the image." );
    addProtectedField( "Frame", TypeS32, Offset(mImageFrame, GuiSpriteCtrl), &setImageFrame, &defaultProtectedGetFn, &writeImageFrame, "The image frame used for the image." );
    addProtectedField( "Animation", TypeAssetId, Offset(mAnimationAssetId, GuiSpriteCtrl), &setAnimation, &getAnimation, &writeAnimation, "The animation to use.");
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::onWake()
{
    // Call parent.
    if (!Parent::onWake())
        return false;

    // Are we in static mode?
    if ( mImageAssetId != StringTable->EmptyString )
    {
        // Set image asset.
		ImageFrameProvider::setImage( mImageAssetId );
    }
    else if ( mAnimationAssetId != StringTable->EmptyString )
    {
        // Play animation asset.
		ImageFrameProvider::setAnimation( mAnimationAssetId );
    }

    return true;
}

//-----------------------------------------------------------------------------

void GuiSpriteCtrl::onSleep()
{
    // Clear assets.
	ImageFrameProvider::clearAssets();

    // Call parent.
    Parent::onSleep();
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setImage( const char* pImageAssetId )
{
    // Sanity!
    AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

    // Reset animation.
    mAnimationAssetId = StringTable->EmptyString;

    // Fetch the asset Id.
    mImageAssetId = StringTable->insert(pImageAssetId);

    // Reset image frame.
	mImageFrame = 0;

	// Finish if not awake.
    if ( !isAwake() )
		return true;

	// Call parent.
	if ( !ImageFrameProvider::setImage( pImageAssetId, mImageFrame ) )
		return false;

    // Update control.
    setUpdate();

	return true;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setImageFrame( const U32 frame )
{
	// Call parent.
	if ( !ImageFrameProvider::setImageFrame( frame ) )
		return false;

    // Update control.
    setUpdate();

	return true;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setAnimation( const char* pAnimationAssetId )
{
    // Sanity!
    AssertFatal( pAnimationAssetId != NULL, "Cannot use a NULL asset Id." );

    // Fetch the asset Id.
    mAnimationAssetId = StringTable->insert(pAnimationAssetId);

    // Reset the image asset Id.
    mImageAssetId = StringTable->EmptyString;

    // Finish if not awake.
    if ( !isAwake() )
		return true;

    // Play animation asset if it's valid.
    if ( mAnimationAssetId != StringTable->EmptyString )
		ImageFrameProvider::setAnimation( mAnimationAssetId );

	return true;
}

//-----------------------------------------------------------------------------

void GuiSpriteCtrl::onRender( Point2I offset, const RectI &updateRect)
{
	// Call parent.
	ImageFrameProvider::renderGui( *this, offset, updateRect );
}

//------------------------------------------------------------------------------

void GuiSpriteCtrl::onAnimationEnd( void )
{
    // Clear assets.
	ImageFrameProvider::clearAssets();
}
