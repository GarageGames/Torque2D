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

#include "gui/containers/guiStackCtrl.h"

IMPLEMENT_CONOBJECT(GuiStackControl);

static EnumTable::Enums stackTypeEnum[] =
{
   { GuiStackControl::stackingTypeVert, "Vertical"  },
   { GuiStackControl::stackingTypeHoriz,"Horizontal" },
   { GuiStackControl::stackingTypeDyn,"Dynamic" }
};
static EnumTable gStackTypeTable(3, &stackTypeEnum[0]);

static EnumTable::Enums stackHorizEnum[] =
{
   { GuiStackControl::horizStackLeft, "Left to Right"  },
   { GuiStackControl::horizStackRight,"Right to Left" }
};
static EnumTable gStackHorizSizingTable(2, &stackHorizEnum[0]);

static EnumTable::Enums stackVertEnum[] =
{
   { GuiStackControl::vertStackTop, "Top to Bottom"  },
   { GuiStackControl::vertStackBottom,"Bottom to Top" }
};
static EnumTable gStackVertSizingTable(2, &stackVertEnum[0]);



GuiStackControl::GuiStackControl()
{
   mMinExtent.set(24, 24);
   mResizing = false;
   mStackingType = stackingTypeVert;
   mStackVertSizing = vertStackTop;
   mStackHorizSizing = horizStackLeft;
   mPadding = 0;
   mIsContainer = true;
}

void GuiStackControl::initPersistFields()
{

   addGroup( "Stacking" );
   addField( "StackingType",       TypeEnum,         Offset(mStackingType, GuiStackControl), 1, &gStackTypeTable);
   addField( "HorizStacking",       TypeEnum,         Offset(mStackHorizSizing, GuiStackControl), 1, &gStackHorizSizingTable);
   addField( "VertStacking",        TypeEnum,         Offset(mStackVertSizing, GuiStackControl), 1, &gStackVertSizingTable);
   addField( "Padding", TypeS32, Offset(mPadding, GuiStackControl));
   endGroup( "Stacking" );

   Parent::initPersistFields();
}

ConsoleMethod( GuiStackControl, updateStack, void, 2, 2, "%stackCtrl.updateStack() - Restacks controls it owns")
{
   object->updatePanes();
}

bool GuiStackControl::onWake()
{
   if ( !Parent::onWake() )
      return false;

   updatePanes();

   return true;
}

void GuiStackControl::onSleep()
{
   Parent::onSleep();
}

void GuiStackControl::updatePanes()
{
   // Prevent recursion
   if(mResizing) 
      return;

   // Set Resizing.
   mResizing = true;

   Point2I extent = getExtent();

   // Do we need to stack horizontally?
   if( ( extent.x > extent.y && mStackingType == stackingTypeDyn ) || mStackingType == stackingTypeHoriz )
   {
      switch( mStackHorizSizing )
      {
      case horizStackLeft:
         stackFromLeft();
         break;
      case horizStackRight:
         stackFromRight();
         break;
      }
   }
   // Or, vertically?
   else if( ( extent.y > extent.x && mStackingType == stackingTypeDyn ) || mStackingType == stackingTypeVert)
   {
      switch( mStackVertSizing )
      {
      case vertStackTop:
         stackFromTop();
         break;
      case vertStackBottom:
         stackFromBottom();

         break;
      }
   }

   // Clear Sizing Flag.
   mResizing = false;
}

void GuiStackControl::freeze(bool _shouldfreeze)
{
   mResizing = _shouldfreeze;
}

void GuiStackControl::stackFromBottom()
{
   // Store the sum of the heights of our controls.
   S32 totalHeight=0;

   Point2I curPos;
   // If we go from the bottom, things are basically the same...
   // except we have to assign positions in an arse backwards way
   // (literally :P)

   // Figure out how high everything is going to be...
   // Place each child...
   for( S32 i = 0; i < size(); i++ )
   {
      // Place control
      GuiControl * gc = dynamic_cast<GuiControl*>(operator [](i));

      if(gc && gc->isVisible() )
      {
         Point2I childExtent = gc->getExtent();
         totalHeight += childExtent.y;
      }
   }

   // Figure out where we're starting...
   curPos = getPosition();
   curPos.y += totalHeight;

   // Offset so the bottom control goes in the right place...
   GuiControl * gc = dynamic_cast<GuiControl*>(operator [](size()-1));
   if(gc)
      curPos.y -= gc->getExtent().y;


   // Now work up from there!
   for(S32 i=size()-1; i>=0; i--)
   {
      // Place control
      GuiControl * gc = dynamic_cast<GuiControl*>(operator [](i));

      if(gc)
      {
         // We must place the child...

         // Make it have our width but keep its height
         Point2I childExtent = gc->getExtent();

         // Update our state...
         curPos.y -= childExtent.y - mPadding;

         // And resize...
         gc->resize(curPos - getPosition(), Point2I(getExtent().x, childExtent.y));
      }
   }
}
void GuiStackControl::stackFromTop()
{
   // Store the sum of the heights of our controls.
   S32 totalHeight=0;

   Point2I curPos;
   // Position and resize everything...
   curPos = getPosition();

   // Place each child...
   for(S32 i=0; i<size(); i++)
   {
      // Place control
      GuiControl * gc = dynamic_cast<GuiControl*>(operator [](i));

      if(gc && gc->isVisible() )
      {
         // We must place the child...

         // Make it have our width but keep its height
         Point2I childExtent = gc->getExtent();

         gc->resize(curPos - getPosition(), Point2I(getExtent().x, childExtent.y));

         // Update our state...
         curPos.y    += childExtent.y + mPadding;
         totalHeight += childExtent.y + mPadding;
      }
   }
   // Conform our size to the sum of the child sizes.
   if( totalHeight > getExtent().y )
   {
      curPos.x = getExtent().x;
      curPos.y = totalHeight;
      resize(getPosition(), curPos);
   }
   else if( totalHeight < getExtent().y )
   {
      curPos.x = getExtent().x;
      curPos.y = getMax( totalHeight , mMinExtent.y );
      resize(getPosition(), curPos);
   }
}
void GuiStackControl::stackFromLeft()
{
   // Store the sum of the heights of our controls.
   S32 totalWidth=0;

   Point2I curPos;
   // Position and resize everything...
   curPos = getPosition();

   // Place each child...
   for(S32 i=0; i<size(); i++)
   {
      // Place control
      GuiControl * gc = dynamic_cast<GuiControl*>(operator [](i));

      if(gc && gc->isVisible() )
      {
         // We must place the child...

         // Make it have our width but keep its height
         Point2I childExtent = gc->getExtent();

         gc->resize(curPos - getPosition(), Point2I( childExtent.x,getExtent().y));

         // Update our state...
         curPos.x    += childExtent.x + mPadding;
         totalWidth += childExtent.x + mPadding;
      }
   }

   // Conform our size to the sum of the child sizes.
   if( totalWidth > getExtent().x )
   {
      curPos.x = totalWidth;
      curPos.y = getExtent().y;
      resize(getPosition(), curPos);
   }
}
void GuiStackControl::stackFromRight()
{
}

void GuiStackControl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   //call set update both before and after
   setUpdate();
   Point2I actualNewExtent = Point2I(  getMax(mMinExtent.x, newExtent.x),
                                       getMax(mMinExtent.y, newExtent.y));
   mBounds.set(newPosition, actualNewExtent);

   GuiControl *parent = getParent();
   if (parent)
      parent->childResized(this);
   setUpdate();

   updatePanes();
}

void GuiStackControl::addObject(SimObject *obj)
{
   Parent::addObject(obj);

   updatePanes();
}

void GuiStackControl::removeObject(SimObject *obj)
{
   Parent::removeObject(obj);

   updatePanes();
}

bool GuiStackControl::reOrder(SimObject* obj, SimObject* target)
{
   bool ret = Parent::reOrder(obj, target);
   if (ret)
      updatePanes();

   return ret;
}

void GuiStackControl::childResized(GuiControl *child)
{
   updatePanes();
}