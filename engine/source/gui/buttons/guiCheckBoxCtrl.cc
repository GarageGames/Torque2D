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
#include "gui/guiCanvas.h"
#include "gui/buttons/guiCheckBoxCtrl.h"
#include "console/consoleTypes.h"
#include "gui/guiDefaultControlRender.h"

#include "guiCheckBoxCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiCheckBoxCtrl);

//---------------------------------------------------------------------------
GuiCheckBoxCtrl::GuiCheckBoxCtrl()
{
   mBounds.extent.set(140, 30);
   mStateOn = false;
   mBoxOffset.set(0,7);
   mBoxExtent.set(16, 16);
   mTextOffset.set(22, 7);
   mTextExtent.set(mBounds.extent.x - mTextOffset.x, mBounds.extent.y - mTextOffset.y);
}

//---------------------------------------------------------------------------

void GuiCheckBoxCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("stateOn", TypeBool, Offset(mStateOn, GuiCheckBoxCtrl));
   addField("boxOffset", TypePoint2I, Offset(mBoxOffset, GuiCheckBoxCtrl));
   addField("boxExtent", TypePoint2I, Offset(mBoxExtent, GuiCheckBoxCtrl));
   addField("textOffset", TypePoint2I, Offset(mTextOffset, GuiCheckBoxCtrl));
   addField("textExtent", TypePoint2I, Offset(mTextExtent, GuiCheckBoxCtrl));
}

bool GuiCheckBoxCtrl::onWake()
{
   if(!Parent::onWake())
      return false;

   return true;
}

void GuiCheckBoxCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	GuiControlState currentState = GuiControlState::NormalState;
	if (!mActive)
	{
		currentState = GuiControlState::DisabledState;
	}
	else if (mDepressed || mStateOn)
	{
		currentState = GuiControlState::SelectedState;
	}
	else if (mMouseOver)
	{
		currentState = GuiControlState::HighlightState;
	}

	RectI boundsRect(offset, mBounds.extent);
	RectI boxRect(offset + mBoxOffset, mBoxExtent);

	if(mProfile->mBitmapName != NULL && mProfile->constructBitmapArray() >= 6)
	{
		//Use the bitmap to create the checkbox
		S32 index = 1;
		if (mStateOn || mDepressed)
		{
			index = 2;
		}
		if (mMouseOver)
		{
			index += 2;
		}
		else if (!mActive)
		{
			index += 4;
		}

		RectI dest = RectI(offset + mBoxOffset, mBoxExtent);
		dglClearBitmapModulation();
		dglDrawBitmapStretchSR(mProfile->mTextureHandle, dest, mProfile->mBitmapArrayRects[index-1]);
	}
	else
	{
		//Draw the checkbox
		renderBorderedRect(boxRect, mProfile, currentState);
	}
   
    if(mText[0] != '\0')
    {
		ColorI fontColor = mProfile->getFontColor(currentState);
		dglSetBitmapModulation( fontColor );

		Point2I textArea = mTextExtent;
		if ((textArea.x + mTextOffset.x) > mBounds.extent.x)
		{
			textArea.x = mBounds.extent.x;
		}
		if ((textArea.y + mTextOffset.y) > mBounds.extent.y)
		{
			textArea.y = mBounds.extent.y;
		}

		renderJustifiedText(Point2I(offset.x + mTextOffset.x, offset.y + mTextOffset.y), textArea, mText);
    }

   //render the children
   renderChildControls(offset, updateRect);
}

void GuiCheckBoxCtrl::onAction()
{
	if (!mActive)
		return;

	mStateOn = !mStateOn;

	Parent::onAction();
}

void GuiCheckBoxCtrl::setStateOn(bool bStateOn)
{
	if (!mActive)
		return;

	mStateOn = bStateOn;
	
	setUpdate();
}

void GuiCheckBoxCtrl::setScriptValue(const char *value)
{
	mStateOn = dAtob(value);

	setUpdate();
}

const char *GuiCheckBoxCtrl::getScriptValue()
{
	return mStateOn ? "1" : "0";
}

void GuiCheckBoxCtrl::onMessage(GuiControl *sender, S32 msg)
{
	Parent::onMessage(sender, msg);
}