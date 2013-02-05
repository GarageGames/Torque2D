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

#ifndef _GUISTACKCTRL_H_
#define _GUISTACKCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"

/// A stack of GUI controls.
///
/// This maintains a horizontal or vertical stack of GUI controls. If one is deleted, or
/// resized, then the stack is resized to fit. The order of the stack is
/// determined by the internal order of the children (ie, order of addition).
///
///
/// @todo Make this support horizontal right to left stacks.
class GuiStackControl : public GuiControl
{
protected:
   typedef GuiControl Parent;
   bool             mResizing;
   S32              mPadding;
   S32 mStackHorizSizing;      ///< Set from horizSizingOptions.
   S32 mStackVertSizing;       ///< Set from vertSizingOptions.
   S32 mStackingType;

public:
   GuiStackControl();

   enum stackingOptions
   {
      horizStackLeft = 0,///< Stack from left to right when horizontal
      horizStackRight,   ///< Stack from right to left when horizontal
      vertStackTop,      ///< Stack from top to bottom when vertical
      vertStackBottom,   ///< Stack from bottom to top when vertical
      stackingTypeVert,  ///< Always stack vertically
      stackingTypeHoriz, ///< Always stack horizontally
      stackingTypeDyn    ///< Dynamically switch based on width/height
   };


   void resize(const Point2I &newPosition, const Point2I &newExtent);
   void childResized(GuiControl *child);
   /// prevent resizing. useful when adding many items.
   void freeze(bool);

   bool onWake();
   void onSleep();

   void updatePanes();

   void stackFromLeft();
   void stackFromRight();
   void stackFromTop();
   void stackFromBottom();

   S32 getCount() { return size(); }; /// Returns the number of children in the stack

   void addObject(SimObject *obj);
   void removeObject(SimObject *obj);

   bool reOrder(SimObject* obj, SimObject* target = 0);

   static void initPersistFields();
   DECLARE_CONOBJECT(GuiStackControl);
};

#endif