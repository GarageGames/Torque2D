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
#ifndef _GUIAUTOSCROLLCTRL_H_
#define _GUIAUTOSCROLLCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _GUITICKCTRL_H_
#include "gui/guiTickCtrl.h"
#endif

class GuiAutoScrollCtrl : public GuiTickCtrl
{
private:
   typedef GuiTickCtrl Parent;
   bool mScrolling;
   F32 mCurrentTime;
   F32 mStartDelay;
   F32 mResetDelay;
   S32 mChildBorder;
   F32 mScrollSpeed;
   bool mTickCallback;

   F32 mControlPositionY;

   void resetChild(GuiControl* control);

public:
   // Constructor/Destructor/Conobject Declaration
   GuiAutoScrollCtrl();
   ~GuiAutoScrollCtrl();
   DECLARE_CONOBJECT(GuiAutoScrollCtrl);

   // Persistence
   static void initPersistFields();

   // Control Events
   virtual void onChildAdded(GuiControl* control);
   virtual void onChildRemoved(GuiControl* control);
   virtual void resize(const Point2I& newPosition, const Point2I& newExtent);
   virtual void childResized(GuiControl *child);

   virtual void processTick();
   virtual void advanceTime(F32 timeDelta);
};
#endif