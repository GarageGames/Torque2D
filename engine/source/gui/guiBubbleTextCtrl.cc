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

#include "gui/guiBubbleTextCtrl.h"
#include "gui/guiCanvas.h"

IMPLEMENT_CONOBJECT(GuiBubbleTextCtrl);

//------------------------------------------------------------------------------
void GuiBubbleTextCtrl::popBubble()
{
	// Release the mouse:
	mInAction = false;
	mouseUnlock();

	// Pop the dialog
	getRoot()->popDialogControl(mDlg);

	// Kill the popup
	mDlg->removeObject(mPopup);
	mPopup->removeObject(mMLText);
	mMLText->deleteObject();
	mPopup->deleteObject();
	mDlg->deleteObject();
}

//------------------------------------------------------------------------------
void GuiBubbleTextCtrl::onMouseDown(const GuiEvent &event)
{
	if (mInAction)
	{
		popBubble();

		return;
	}

	mDlg = new GuiControl();
   AssertFatal(mDlg, "Failed to create the GuiControl for the BubbleTextCtrl");
   mDlg->setField("profile","GuiModelessDialogProfile");
   mDlg->setField("horizSizing", "width");
	mDlg->setField("vertSizing", "height");
	//MIN_RESOLUTION defined in platformWin32/platformGL.h
	mDlg->setField("extent", MIN_RESOLUTION_XY_STRING);

   mPopup = new GuiControl();
   AssertFatal(mPopup, "Failed to create the GuiControl for the BubbleTextCtrl");
   mPopup->setField("profile","GuiBubblePopupProfile");

   mMLText = new GuiMLTextCtrl();
   AssertFatal(mMLText, "Failed to create the GuiMLTextCtrl for the BubbleTextCtrl");
	mMLText->setField("profile","GuiBubbleTextProfile");
	mMLText->setField("position", "2 2");
	mMLText->setField("extent", "296 51");	
	
	mMLText->setText((char*)mText,dStrlen(mText));

	mMLText->registerObject();
	mPopup->registerObject();
	mDlg->registerObject();

	mPopup->addObject(mMLText);
	mDlg->addObject(mPopup);

	mPopup->resize(event.mousePoint,Point2I(300,55));

	getRoot()->pushDialogControl(mDlg,0);
	mouseLock();

	mInAction = true;
}
