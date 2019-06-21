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

#ifndef _GUIBUTTONCTRL_H_
#define _GUIBUTTONCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

class GuiButtonCtrl : public GuiControl
{
   typedef GuiControl Parent;
protected:
	bool mDepressed;
	bool mMouseOver;
	bool mHasTheme;
public:
	GuiButtonCtrl();
	bool onWake();

	DECLARE_CONOBJECT(GuiButtonCtrl);

	void acceleratorKeyPress(U32 index);
	void acceleratorKeyRelease(U32 index);

	void onTouchDown(const GuiEvent &);
	void onTouchUp(const GuiEvent &);
	void onRightMouseUp(const GuiEvent &);

	void onTouchEnter(const GuiEvent &);
	void onTouchLeave(const GuiEvent &);

	bool onKeyDown(const GuiEvent &event);
	bool onKeyUp(const GuiEvent &event);

	virtual void setScriptValue(const char *value);
	virtual const char *getScriptValue();

	virtual void onMessage(GuiControl *, S32 msg);
	virtual void onAction();
   
	virtual void onRender(Point2I offset, const RectI &updateRect);
};

#endif //_GUI_BUTTON_CTRL_H
