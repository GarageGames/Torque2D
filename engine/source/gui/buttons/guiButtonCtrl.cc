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

#include "console/console.h"
#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#include "gui/guiCanvas.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "gui/guiDefaultControlRender.h"

IMPLEMENT_CONOBJECT(GuiButtonCtrl);

GuiButtonCtrl::GuiButtonCtrl()
{
   mBounds.extent.set(140, 30);
   mButtonText = StringTable->EmptyString;
}

bool GuiButtonCtrl::onWake()
{
   if( !Parent::onWake() )
      return false;

   // Button Theme?
   if( mProfile->constructBitmapArray() >= 36 )
      mHasTheme = true;
   else
      mHasTheme = false;

   return true;

}
//--------------------------------------------------------------------------

void GuiButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	GuiControlState currentState = GuiControlState::normal;
	if (!mActive)
	{
		currentState = GuiControlState::disabled;
	}
	else if (mDepressed || mStateOn)
	{
		currentState = GuiControlState::selected;
	}
	else if(mMouseOver)
	{
		currentState = GuiControlState::highlight;
	}

	RectI boundsRect(offset, mBounds.extent);

	if( !mHasTheme )
	{
		renderBorderedRect(boundsRect, mProfile, currentState);
	}
   else if( mHasTheme )
   {
      S32 indexMultiplier = 1;
      if ( currentState == selected) 
         indexMultiplier = 3;
      else if ( currentState == highlight )
         indexMultiplier = 2;
      else if ( currentState == disabled )
         indexMultiplier = 4;

      renderSizableBitmapBordersFilled( boundsRect, indexMultiplier, mProfile );
   }

	//Get the border profiles
	GuiBorderProfile *leftProfile = (mProfile->mBorderLeft) ? mProfile->mBorderLeft : mProfile->mBorderDefault;
	GuiBorderProfile *rightProfile = (mProfile->mBorderRight) ? mProfile->mBorderRight : mProfile->mBorderDefault;
	GuiBorderProfile *topProfile = (mProfile->mBorderTop) ? mProfile->mBorderTop : mProfile->mBorderDefault;
	GuiBorderProfile *bottomProfile = (mProfile->mBorderBottom) ? mProfile->mBorderBottom : mProfile->mBorderDefault;

	S32 leftSize = (leftProfile) ? leftProfile->getBorder(currentState) : 0;
	S32 rightSize = (rightProfile) ? rightProfile->getBorder(currentState) : 0;
	S32 topSize = (topProfile) ? topProfile->getBorder(currentState) : 0;
	S32 bottomSize = (bottomProfile) ? bottomProfile->getBorder(currentState) : 0;

	//Get the inner rect
	RectI innerRect = RectI(offset.x + leftSize, offset.y + topSize, (mBounds.extent.x - leftSize) - rightSize, (mBounds.extent.y - topSize) - bottomSize);

	ColorI fontColor = mProfile->getFontColor(currentState);

   dglSetBitmapModulation( fontColor );
   renderJustifiedText(innerRect.point, innerRect.extent, mButtonText);

   //render the children
   renderChildControls( offset, updateRect);
}

