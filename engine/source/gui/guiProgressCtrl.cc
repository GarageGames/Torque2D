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
#include "console/consoleTypes.h"
#include "graphics/dgl.h"

#include "gui/guiProgressCtrl.h"

IMPLEMENT_CONOBJECT(GuiProgressCtrl);

GuiProgressCtrl::GuiProgressCtrl()
{
   mProgress = 0.0f;
}

const char* GuiProgressCtrl::getScriptValue()
{
   char * ret = Con::getReturnBuffer(64);
   dSprintf(ret, 64, "%g", mProgress);
   return ret;
}

void GuiProgressCtrl::setScriptValue(const char *value)
{
   //set the value
   if (! value)
      mProgress = 0.0f;
   else
      mProgress = dAtof(value);

   //validate the value
   mProgress = mClampF(mProgress, 0.f, 1.f);
   setUpdate();
}

void GuiProgressCtrl::onPreRender()
{
   const char * var = getVariable();
   if(var)
   {
      F32 value = mClampF(dAtof(var), 0.f, 1.f);
      if(value != mProgress)
      {
         mProgress = value;
         setUpdate();
      }
   }
}

void GuiProgressCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   RectI ctrlRect(offset, mBounds.extent);

   //draw the progress
   S32 width = (S32)((F32)mBounds.extent.x * mProgress);
   if (width > 0)
   {
      RectI progressRect = ctrlRect;
      progressRect.extent.x = width;
      dglDrawRectFill(progressRect, mProfile->mFillColor);
   }

   //now draw the border
   if (mProfile->mBorder)
      dglDrawRect(ctrlRect, mProfile->mBorderColor);

   Parent::onRender( offset, updateRect );

   //render the children
   renderChildControls(offset, updateRect);
}

