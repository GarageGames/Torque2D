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

IMPLEMENT_CONOBJECT(GuiCheckBoxCtrl);

//---------------------------------------------------------------------------
GuiCheckBoxCtrl::GuiCheckBoxCtrl()
{
   mBounds.extent.set(140, 30);
    mStateOn = false;
   mIndent = 0;
   mButtonType = ButtonTypeCheck;
   mUseInactiveState = false;
}

//---------------------------------------------------------------------------

void GuiCheckBoxCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("useInactiveState", TypeBool, Offset(mUseInactiveState, GuiCheckBoxCtrl));
}

bool GuiCheckBoxCtrl::onWake()
{
   if(!Parent::onWake())
      return false;

   // make sure there is a bitmap array for this control type
   // if it is declared as such in the control
   mProfile->constructBitmapArray();

   return true;
}

void GuiCheckBoxCtrl::onMouseDown(const GuiEvent& event)
{
   if (!mUseInactiveState)
      return Parent::onMouseDown(event);

   if (mProfile->mCanKeyFocus)
      setFirstResponder();

   if (mProfile->mSoundButtonDown)
   {
      AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonDown);
      alxPlay(handle);
   }

   //lock the mouse
   mouseLock();
   mDepressed = true;

   //update
   setUpdate();
}

void GuiCheckBoxCtrl::onMouseUp(const GuiEvent& event)
{
   if (!mUseInactiveState)
      return Parent::onMouseUp(event);

   mouseUnlock();

   setUpdate();

   //if we released the mouse within this control, perform the action
   if (mDepressed)
      onAction();

   mDepressed = false;
}

void GuiCheckBoxCtrl::onAction()
{
   if (!mUseInactiveState)
      return Parent::onAction();

   if(mButtonType == ButtonTypeCheck)
   {
      if (!mActive)
      {
         mActive = true;
         mStateOn = true;
      }
      else if (mStateOn)
         mStateOn = false;
      else if (!mStateOn)
         mActive = false;

      // Update the console variable:
      if ( mConsoleVariable[0] )
         Con::setBoolVariable( mConsoleVariable, mStateOn );
      // Execute the console command (if any)
      if( mConsoleCommand[0] )
         Con::evaluate( mConsoleCommand, false );
    }
   setUpdate();

   // Provide and onClick script callback.
   if( isMethod("onClick") )
      Con::executef( this, 2, "onClick" );
}

//---------------------------------------------------------------------------
void GuiCheckBoxCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   ColorI backColor = mActive ? mProfile->mFillColor : mProfile->mFillColorNA;
   ColorI fontColor = mMouseOver ? mProfile->mFontColorHL : mProfile->mFontColor;
    ColorI insideBorderColor = isFirstResponder() ? mProfile->mBorderColorHL : mProfile->mBorderColor;

   // just draw the check box and the text:
   S32 xOffset = 0;
    dglClearBitmapModulation();
   if(mProfile->mBitmapArrayRects.size() >= 4)
   {
       // if size >= 4 then the inactive state images should be present
      S32 index = mStateOn;
      if(!mActive)
         index = 4; // inactive state images are indexes 4 and 5.
      else if(mDepressed)
         index += 2;
      xOffset = mProfile->mBitmapArrayRects[0].extent.x + 2 + mIndent;
      S32 y = (mBounds.extent.y - mProfile->mBitmapArrayRects[0].extent.y) / 2;
      dglDrawBitmapSR(mProfile->mTextureHandle, offset + Point2I(mIndent, y), mProfile->mBitmapArrayRects[index]);
   }
   
    if(mButtonText[0] != '\0')
    {
       dglSetBitmapModulation( fontColor );
      renderJustifiedText(Point2I(offset.x + xOffset, offset.y),
                          Point2I(mBounds.extent.x - mBounds.extent.y, mBounds.extent.y),
                          mButtonText);
    }
   //render the children
   renderChildControls(offset, updateRect);
}

ConsoleMethod(GuiCheckBoxCtrl, setStateOn, void, 3, 3, "(state) Sets the control as active and updates siblings of the same group."
              "@param state This argument may be a boolean value or an integer."
              "state < 0: Parent::setStateOn(false), obj::setActive(false)\n"
              "state == 0 (or false): Parent::setStateOn(false), obj::setActive(true)\n"
              "state > 0 (or true): Parent::setStateOn(true), obj::setActive(true)")
{
   if (dStricmp(argv[2], "true") == 0)
      object->setStateOn(1);
   else if (dStricmp(argv[2], "false") == 0)
      object->setStateOn(0);
   else
      object->setStateOn(dAtoi(argv[2]));
}

void GuiCheckBoxCtrl::setStateOn(S32 state)
{
   if (mUseInactiveState)
   {
      if (state < 0)
      {
         setActive(false);
         Parent::setStateOn(false);
      }
      else if (state == 0)
      {
         setActive(true);
         Parent::setStateOn(false);
      }
      else if (state > 0)
      {
         setActive(true);
         Parent::setStateOn(true);
      }
   }
   else
      Parent::setStateOn((bool)state);
}

const char* GuiCheckBoxCtrl::getScriptValue()
{
   if (mUseInactiveState)
   {
      if (isActive())
         if (mStateOn)
            return "1";
         else
            return "0";
      else
         return "-1";
   }
   else
      return Parent::getScriptValue();
}

//---------------------------------------------------------------------------
// EOF //


