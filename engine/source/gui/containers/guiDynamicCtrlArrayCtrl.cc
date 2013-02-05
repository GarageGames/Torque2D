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

#include "gui/containers/guiDynamicCtrlArrayCtrl.h"

ConsoleMethod(GuiDynamicCtrlArrayControl, refresh, void, 2, 2, "() Forces the child controls to recalculate")
{
   object->updateChildControls();
}

IMPLEMENT_CONOBJECT(GuiDynamicCtrlArrayControl);

GuiDynamicCtrlArrayControl::GuiDynamicCtrlArrayControl()
{
   mCols = 0;
   mColSize = 64;
   mRowSize = 64;
   mRowSpacing = 0;
   mColSpacing = 0;
   mIsContainer = true;

   mResizing = false;
}

GuiDynamicCtrlArrayControl::~GuiDynamicCtrlArrayControl()
{
}

void GuiDynamicCtrlArrayControl::initPersistFields()
{
  Parent::initPersistFields();

  addField("colCount",     TypeS32,       Offset(mCols,        GuiDynamicCtrlArrayControl));
  addField("colSize",      TypeS32,       Offset(mColSize,     GuiDynamicCtrlArrayControl));
  addField("rowSize",      TypeS32,       Offset(mRowSize,     GuiDynamicCtrlArrayControl));
  addField("rowSpacing",   TypeS32,       Offset(mRowSpacing,  GuiDynamicCtrlArrayControl));
  addField("colSpacing",   TypeS32,       Offset(mColSpacing,  GuiDynamicCtrlArrayControl));
}

void GuiDynamicCtrlArrayControl::inspectPostApply()
{
   resize(getPosition(), getExtent());
   Parent::inspectPostApply();
}

void GuiDynamicCtrlArrayControl::updateChildControls()
{
   // Prevent recursion
   if(mResizing) return;

   mResizing = true;

   // Store the sum of the heights of our controls.

   Point2I curPos = getPosition();

   // Calculate the number of columns
   mCols = ( getExtent().x / (mColSize));// + mColSpacing) ) - mColSpacing;
   if(mCols < 1)
      mCols = 1;

   // Place each child...
   S32 childcount = 0;
   S32 rows = 0;
   for(S32 i=0; i<size(); i++)
   {
      // Place control
      GuiControl * gc = dynamic_cast<GuiControl*>(operator [](i));

      if(gc && gc->isVisible()) // DAW: Added check if child is visible.  Invisible children don't take part
      {
         // Get the current column and row...
         S32 curCol = childcount % mCols;
         S32 curRow = childcount / mCols;

         rows = getMax( rows, curRow );

         // Reposition and resize
         Point2I newPos(mColSpacing + (curCol * (mColSize + mColSpacing)), mRowSpacing + (curRow * (mRowSize + mRowSpacing)));
         gc->resize(newPos, Point2I(mColSize, mRowSize));

         childcount++;
      }
   }

   rows++;

   // Conform our size to the sum of the child sizes.
   curPos.x = getExtent().x;
   curPos.y = mRowSpacing + (rows * (mRowSize + mRowSpacing));
   mBounds.extent.y = curPos.y;

   mResizing = false;
}

void GuiDynamicCtrlArrayControl::resize(const Point2I &newPosition, const Point2I &newExtent)
{

   // If we don't have any children, return.
   if(size() < 1)
   {
      Parent::resize(newPosition, newExtent);
      return;
   }

   // What we need to do is figure out how many children will fit on a row
   // given this control's new extent.  This gives us the number of columns.
   // Then the number of rows will expand or contract, essentially changing
   // the 'y' extent of this control.

   //call set update both before and after
   setUpdate();
   Point2I actualNewExtent = Point2I(getMax(mMinExtent.x, newExtent.x),
                                     getMax(mMinExtent.y, newExtent.y));

   mBounds.set(newPosition, actualNewExtent);

   GuiControl *parent = getParent();
   if (parent)
      parent->childResized(this);
   setUpdate();

   updateChildControls();
}

void GuiDynamicCtrlArrayControl::addObject(SimObject *obj)
{
   Parent::addObject(obj);

   updateChildControls();
}

void GuiDynamicCtrlArrayControl::childResized(GuiControl *child)
{
   Parent::childResized(child);

   updateChildControls();
}