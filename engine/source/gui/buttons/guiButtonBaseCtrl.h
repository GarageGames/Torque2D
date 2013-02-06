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

#ifndef _GUIBUTTONBASECTRL_H_
#define _GUIBUTTONBASECTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

// all the button functionality is moving into buttonBase
// thus, all subclasses will just be rendering classes,
// and radios and check boxes can be done using pushbuttons
// or bitmap buttons.

class GuiButtonBaseCtrl : public GuiControl
{
   typedef GuiControl Parent;

protected:
   StringTableEntry mButtonText;
   StringTableEntry mButtonTextID;
   bool mDepressed;
   bool mMouseOver;
   bool mStateOn;
   S32 mButtonType;
   S32 mRadioGroup;
   bool mUseMouseEvents;
public:
   enum {
      ButtonTypePush,
      ButtonTypeCheck,
      ButtonTypeRadio,
   };

   GuiButtonBaseCtrl();
   bool onWake();

   DECLARE_CONOBJECT(GuiButtonBaseCtrl);
   static void initPersistFields();

   void setText(const char *text);
   void setTextID(S32 id);
   void setTextID(const char *id);
   const char *getText();
   bool getStateOn(void)	{ return mStateOn; }
   void setStateOn( bool bStateOn );

   void acceleratorKeyPress(U32 index);
   void acceleratorKeyRelease(U32 index);

   void onMouseDown(const GuiEvent &);
   void onMouseUp(const GuiEvent &);
   void onRightMouseUp(const GuiEvent &);

   void onMouseEnter(const GuiEvent &);
   void onMouseLeave(const GuiEvent &);

   bool onKeyDown(const GuiEvent &event);
   bool onKeyUp(const GuiEvent &event);

   void setScriptValue(const char *value);
   const char *getScriptValue();

   void onMessage(GuiControl *,S32 msg);
   void onAction();

};

#endif
