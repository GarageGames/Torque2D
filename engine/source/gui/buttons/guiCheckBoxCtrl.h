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

#ifndef _GUICHECKBOXCTRL_H_
#define _GUICHECKBOXCTRL_H_

#ifndef _GUIBUTTONBASECTRL_H_
#include "gui/buttons/guiButtonBaseCtrl.h"
#endif

class GuiCheckBoxCtrl : public GuiButtonBaseCtrl
{
   typedef GuiButtonBaseCtrl Parent;

   bool mUseInactiveState;

protected:
public:
   S32 mIndent;
   DECLARE_CONOBJECT(GuiCheckBoxCtrl);
   GuiCheckBoxCtrl();

   void setStateOn(S32 state);
   virtual const char* getScriptValue();

   virtual void onMouseDown(const GuiEvent& event);
   virtual void onMouseUp(const GuiEvent& event);
   virtual void onAction();
   void onRender(Point2I offset, const RectI &updateRect);
   bool onWake();

   static void initPersistFields();
};

#endif //_GUI_CHECKBOX_CTRL_H
