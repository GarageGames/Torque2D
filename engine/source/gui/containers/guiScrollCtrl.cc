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

#include "console/consoleTypes.h"
#include "console/console.h"
#include "graphics/gBitmap.h"
#include "graphics/TextureManager.h"
#include "io/resource/resourceManager.h"
#include "platform/event.h"
#include "graphics/dgl.h"
#include "gui/guiArrayCtrl.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/guiDefaultControlRender.h"

IMPLEMENT_CONOBJECT(GuiScrollCtrl);

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

GuiScrollCtrl::GuiScrollCtrl()
{
   mBounds.extent.set(200,200);
   mChildMargin.set(0,0);
   mBorderThickness = 1;
   mScrollBarThickness = 16;
   mScrollBarArrowBtnLength = 16;
   mScrollBarDragTolerance = 130;
   stateDepressed = false;
   curHitRegion = None;

   mWillFirstRespond = true;
   mUseConstantHeightThumb = false;
   mIsContainer = true;

   mForceVScrollBar = ScrollBarAlwaysOn;
   mForceHScrollBar = ScrollBarAlwaysOn;
}

static EnumTable::Enums scrollBarEnums[] =
{
   { GuiScrollCtrl::ScrollBarAlwaysOn,     "alwaysOn"     },
   { GuiScrollCtrl::ScrollBarAlwaysOff,    "alwaysOff"    },
   { GuiScrollCtrl::ScrollBarDynamic,      "dynamic"      },
};
static EnumTable gScrollBarTable(3, &scrollBarEnums[0]);

ConsoleMethod(GuiScrollCtrl, scrollToTop, void, 2, 2, "() Use the scrollToTop method to scroll the scroll control to the top of the child content area.\n"
                                                                "@return No return value.\n"
                                                                "@sa scrollToBottom")
{
   object->scrollTo( 0, 0 );
}

ConsoleMethod(GuiScrollCtrl, scrollToBottom, void, 2, 2, "() Use the scrollToBottom method to scroll the scroll control to the bottom of the child content area.\n"
                                                                "@return No return value.\n"
                                                                "@sa scrollToTop")
{
   object->scrollTo( 0, 0x7FFFFFFF );
}

ConsoleMethod(GuiScrollCtrl, setScrollPosition, void, 4, 4, "(x, y) - scrolls the scroll control to the specified position.")
{
   object->scrollTo(dAtoi(argv[2]), dAtoi(argv[3]));
}

ConsoleMethod(GuiScrollCtrl, getScrollPositionX, S32, 2, 2, "() - get the current x scroll position of the scroll control.")
{
   return object->getChildRelPos().x;
}

ConsoleMethod(GuiScrollCtrl, getScrollPositionY, S32, 2, 2, "() - get the current y scroll position of the scroll control.")
{
   return object->getChildRelPos().y;
}

ConsoleMethod(GuiScrollCtrl, computeSizes, void, 2, 2, "() - refresh all the contents in this scroll container.")
{
    return object->computeSizes();
}

ConsoleMethod(GuiScrollCtrl, getUseScrollEvents, bool, 2, 2, "() - get the current scroll callback state of the scroll control.")
{
   return object->getUseScrollEvents();
}

ConsoleMethod(GuiScrollCtrl, setUseScrollEvents, void, 3, 3, "() - set the scroll callback state of the scroll control.")
{
    object->setUseScrollEvents(dAtoi(argv[2]));
}

void GuiScrollCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("willFirstRespond",     TypeBool,    Offset(mWillFirstRespond, GuiScrollCtrl));
   addField("hScrollBar",           TypeEnum,    Offset(mForceHScrollBar, GuiScrollCtrl), 1, &gScrollBarTable);
   addField("vScrollBar",           TypeEnum,    Offset(mForceVScrollBar, GuiScrollCtrl), 1, &gScrollBarTable);
   addField("constantThumbHeight",  TypeBool,    Offset(mUseConstantHeightThumb, GuiScrollCtrl));
   addField("childMargin",          TypePoint2I, Offset(mChildMargin, GuiScrollCtrl));

}

void GuiScrollCtrl::resize(const Point2I &newPos, const Point2I &newExt)
{
   Parent::resize(newPos, newExt);
   computeSizes();
}

void GuiScrollCtrl::childResized(GuiControl *child)
{
   Parent::childResized(child);
   computeSizes();
}

bool GuiScrollCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   mTextureHandle = mProfile->mTextureHandle;
   mTextureHandle.setFilter(GL_NEAREST);;

   bool result;
   result = mProfile->constructBitmapArray() >= BmpStates * BmpCount;

   //AssertFatal(result, "Failed to create the bitmap array");
   if (!result)
       Con::warnf("Failed to create the bitmap array for %s", mProfile->mBitmapName);

   mBitmapBounds = mProfile->mBitmapArrayRects.address();

   //init
   mBaseThumbSize = mBitmapBounds[BmpStates * BmpVThumbTopCap].extent.y +
                    mBitmapBounds[BmpStates * BmpVThumbBottomCap].extent.y;

   mScrollBarThickness      = mBitmapBounds[BmpStates * BmpVPage].extent.x;
   mScrollBarArrowBtnLength = mBitmapBounds[BmpStates * BmpUp].extent.y;
   computeSizes();
   return true;
}

void GuiScrollCtrl::onSleep()
{
   Parent::onSleep();
   mTextureHandle = NULL;
}

bool GuiScrollCtrl::calcChildExtents()
{
   // right now the scroll control really only deals well with a single
   // child control for its content
   if (!size())
      return false;

   GuiControl *ctrl = (GuiControl *) front();
   mChildExt = ctrl->mBounds.extent;
   mChildPos = ctrl->mBounds.point;
   return true;
}

RectI lastVisRect;

void GuiScrollCtrl::scrollRectVisible(RectI rect)
{
   // rect is passed in virtual client space
   if(rect.extent.x > mContentExt.x)
      rect.extent.x = mContentExt.x;
   if(rect.extent.y > mContentExt.y)
      rect.extent.y = mContentExt.y;

   // Determine the points bounding the requested rectangle
    Point2I rectUpperLeft  = rect.point;
    Point2I rectLowerRight = rect.point + rect.extent;

   lastVisRect = rect;

   // Determine the points bounding the actual visible area...
    Point2I visUpperLeft = mChildRelPos;
    Point2I visLowerRight = mChildRelPos + mContentExt;
    Point2I delta(0,0);

   // We basically try to make sure that first the top left of the given
   // rect is visible, and if it is, then that the bottom right is visible.

   // Make sure the rectangle is visible along the X axis...
    if(rectUpperLeft.x < visUpperLeft.x)
        delta.x = rectUpperLeft.x - visUpperLeft.x;
    else if(rectLowerRight.x > visLowerRight.x)
        delta.x = rectLowerRight.x - visLowerRight.x;

   // Make sure the rectangle is visible along the Y axis...
    if(rectUpperLeft.y < visUpperLeft.y)
        delta.y = rectUpperLeft.y - visUpperLeft.y;
    else if(rectLowerRight.y > visLowerRight.y)
        delta.y = rectLowerRight.y - visLowerRight.y;

   // If we had any changes, scroll, otherwise don't.
   if(delta.x || delta.y)
        scrollDelta(delta.x, delta.y);
}


void GuiScrollCtrl::addObject(SimObject *object)
{
   Parent::addObject(object);
   computeSizes();
}

GuiControl* GuiScrollCtrl::findHitControl(const Point2I &pt, S32 initialLayer)
{
   if(pt.x < mProfile->mBorderThickness || pt.y < mProfile->mBorderThickness)
      return this;
   if(pt.x >= mBounds.extent.x - mProfile->mBorderThickness - (mHasVScrollBar ? mScrollBarThickness : 0) ||
      pt.y >= mBounds.extent.y - mProfile->mBorderThickness - (mHasHScrollBar ? mScrollBarThickness : 0))
      return this;
   return Parent::findHitControl(pt, initialLayer);
}

void GuiScrollCtrl::computeSizes()
{
   S32 thickness = (mProfile ? mProfile->mBorderThickness : 1);
   Point2I borderExtent(thickness, thickness);
   mContentPos = borderExtent + mChildMargin;
   mContentExt = mBounds.extent - (mChildMargin * 2)
                                - (borderExtent * 2);

   Point2I childLowerRight;

   mHBarEnabled = false;
   mVBarEnabled = false;
   mHasVScrollBar = (mForceVScrollBar == ScrollBarAlwaysOn);
   mHasHScrollBar = (mForceHScrollBar == ScrollBarAlwaysOn);

   setUpdate();

   if (calcChildExtents())
   {
      childLowerRight = mChildPos + mChildExt;

      if (mHasVScrollBar)
         mContentExt.x -= mScrollBarThickness;
      if (mHasHScrollBar)
         mContentExt.y -= mScrollBarThickness;
      if (mChildExt.x > mContentExt.x && (mForceHScrollBar == ScrollBarDynamic))
      {
         mHasHScrollBar = true;
         mContentExt.y -= mScrollBarThickness;
      }
      if (mChildExt.y > mContentExt.y && (mForceVScrollBar == ScrollBarDynamic))
      {
         mHasVScrollBar = true;
         mContentExt.x -= mScrollBarThickness;

         // If Extent X Changed, check Horiz Scrollbar.
         if (mChildExt.x > mContentExt.x && !mHasHScrollBar && (mForceHScrollBar == ScrollBarDynamic))
         {
            mHasHScrollBar = true;
            mContentExt.y -= mScrollBarThickness;
         }
      }
      Point2I contentLowerRight = mContentPos + mContentExt;

      // see if the child controls need to be repositioned (null space in control)
      Point2I delta(0,0);

      if (mChildPos.x > mContentPos.x)
         delta.x = mContentPos.x - mChildPos.x;
      else if (contentLowerRight.x > childLowerRight.x)
      {
         S32 diff = contentLowerRight.x - childLowerRight.x;
         delta.x = getMin(mContentPos.x - mChildPos.x, diff);
      }

      //reposition the children if the child extent > the scroll content extent
      if (mChildPos.y > mContentPos.y)
         delta.y = mContentPos.y - mChildPos.y;
      else if (contentLowerRight.y > childLowerRight.y)
      {
         S32 diff = contentLowerRight.y - childLowerRight.y;
         delta.y = getMin(mContentPos.y - mChildPos.y, diff);
      }

      // apply the deltas to the children...
      if (delta.x || delta.y)
      {
         SimGroup::iterator i;
         for(i = begin(); i != end();i++)
         {
            GuiControl *ctrl = (GuiControl *) (*i);
            ctrl->mBounds.point += delta;
         }
         mChildPos += delta;
         childLowerRight += delta;
      }
      // enable needed scroll bars
      if (mChildExt.x > mContentExt.x)
         mHBarEnabled = true;
      if (mChildExt.y > mContentExt.y)
         mVBarEnabled = true;
      mChildRelPos = mContentPos - mChildPos;
   }
   // build all the rectangles and such...
   calcScrollRects();
   calcThumbs();
}

void GuiScrollCtrl::calcScrollRects(void)
{
   S32 thickness = ( mProfile ? mProfile->mBorderThickness : 1 );
   if (mHasHScrollBar)
   {
      mLeftArrowRect.set(thickness,
                        mBounds.extent.y - thickness - mScrollBarThickness - 1,
                        mScrollBarArrowBtnLength,
                        mScrollBarThickness);

      mRightArrowRect.set(mBounds.extent.x - thickness - (mHasVScrollBar ? mScrollBarThickness : 0) - mScrollBarArrowBtnLength,
                        mBounds.extent.y - thickness - mScrollBarThickness - 1,
                        mScrollBarArrowBtnLength,
                        mScrollBarThickness);
      mHTrackRect.set(mLeftArrowRect.point.x + mLeftArrowRect.extent.x,
                        mLeftArrowRect.point.y,
                        mRightArrowRect.point.x - (mLeftArrowRect.point.x + mLeftArrowRect.extent.x),
                        mScrollBarThickness);
   }
   if (mHasVScrollBar)
   {
      mUpArrowRect.set(mBounds.extent.x - thickness - mScrollBarThickness,
                        thickness,
                        mScrollBarThickness,
                        mScrollBarArrowBtnLength);
      mDownArrowRect.set(mBounds.extent.x - thickness - mScrollBarThickness,
                        mBounds.extent.y - thickness - mScrollBarArrowBtnLength - (mHasHScrollBar ? ( mScrollBarThickness + 1 ) : 0),
                        mScrollBarThickness,
                        mScrollBarArrowBtnLength);
      mVTrackRect.set(mUpArrowRect.point.x,
                        mUpArrowRect.point.y + mUpArrowRect.extent.y,
                        mScrollBarThickness,
                        mDownArrowRect.point.y - (mUpArrowRect.point.y + mUpArrowRect.extent.y) );
   }
}

void GuiScrollCtrl::calcThumbs()
{
   if (mHBarEnabled)
   {
      U32 trackSize = mHTrackRect.len_x();

      if (mUseConstantHeightThumb)
         mHThumbSize = mBaseThumbSize;
      else
         mHThumbSize = getMax(mBaseThumbSize, S32((mContentExt.x * trackSize) / mChildExt.x));

      mHThumbPos = mHTrackRect.point.x + (mChildRelPos.x * (trackSize - mHThumbSize)) / (mChildExt.x - mContentExt.x);
   }
   if (mVBarEnabled)
   {
      U32 trackSize = mVTrackRect.len_y();

      if (mUseConstantHeightThumb)
         mVThumbSize = mBaseThumbSize;
      else
         mVThumbSize = getMax(mBaseThumbSize, S32((mContentExt.y * trackSize) / mChildExt.y));

      mVThumbPos = mVTrackRect.point.y + (mChildRelPos.y * (trackSize - mVThumbSize)) / (mChildExt.y - mContentExt.y);
   }
}


void GuiScrollCtrl::scrollDelta(S32 deltaX, S32 deltaY)
{
   scrollTo(mChildRelPos.x + deltaX, mChildRelPos.y + deltaY);
}

void GuiScrollCtrl::scrollTo(S32 x, S32 y)
{
   if(!size())
      return;

    // keep scroll start state
    Point2I startPoint = mChildPos;
    Point2I startPointRel = mChildRelPos;

   setUpdate();
   if (x > mChildExt.x - mContentExt.x)
      x = mChildExt.x - mContentExt.x;
   if (x < 0)
      x = 0;

   if (y > mChildExt.y - mContentExt.y)
      y = mChildExt.y - mContentExt.y;
   if (y < 0)
      y = 0;

   Point2I delta(x - mChildRelPos.x, y - mChildRelPos.y);
   mChildRelPos += delta;
   mChildPos -= delta;

   for(SimSet::iterator i = begin(); i != end();i++)
   {
      GuiControl *ctrl = (GuiControl *) (*i);
      ctrl->mBounds.point -= delta;
   }
   calcThumbs();

   // Execute callback
   if (mUseScrollEvents)
   {
	   char buf[4][32];
	   dSprintf(buf[0], 32, "%d %d", startPoint.x, startPoint.y);
	   dSprintf(buf[1], 32, "%d %d", startPointRel.x, startPointRel.y);
	   dSprintf(buf[2], 32, "%d %d", mChildPos.x, mChildPos.y);
	   dSprintf(buf[3], 32, "%d %d", mChildRelPos.x, mChildRelPos.y);
	   Con::executef(this, 5, "onScroll", buf[0], buf[1], buf[2], buf[3]);
   }
}

GuiScrollCtrl::Region GuiScrollCtrl::findHitRegion(const Point2I &pt)
{
   if (mVBarEnabled && mHasVScrollBar)
   {
      if (mUpArrowRect.pointInRect(pt))
         return UpArrow;
      else if (mDownArrowRect.pointInRect(pt))
         return DownArrow;
      else if (mVTrackRect.pointInRect(pt))
      {
         if (pt.y < mVThumbPos)
            return UpPage;
         else if (pt.y < mVThumbPos + mVThumbSize)
            return VertThumb;
         else
            return DownPage;
      }
   }
   if (mHBarEnabled && mHasHScrollBar)
   {
      if (mLeftArrowRect.pointInRect(pt))
         return LeftArrow;
      else if (mRightArrowRect.pointInRect(pt))
         return RightArrow;
      else if (mHTrackRect.pointInRect(pt))
      {
         if (pt.x < mHThumbPos)
            return LeftPage;
         else if (pt.x < mHThumbPos + mHThumbSize)
            return HorizThumb;
         else
            return RightPage;
      }
   }
   return None;
}

bool GuiScrollCtrl::wantsTabListMembership()
{
   return true;
}

bool GuiScrollCtrl::loseFirstResponder()
{
   setUpdate();
   return true;
}

bool GuiScrollCtrl::becomeFirstResponder()
{
   setUpdate();
   return mWillFirstRespond;
}

bool GuiScrollCtrl::onKeyDown(const GuiEvent &event)
{
   if (mWillFirstRespond)
   {
      switch (event.keyCode)
      {
         case KEY_RIGHT:
            scrollByRegion(RightArrow);
            return true;

         case KEY_LEFT:
            scrollByRegion(LeftArrow);
            return true;

         case KEY_DOWN:
            scrollByRegion(DownArrow);
            return true;

         case KEY_UP:
            scrollByRegion(UpArrow);
            return true;

         case KEY_PAGE_UP:
            scrollByRegion(UpPage);
            return true;

         case KEY_PAGE_DOWN:
            scrollByRegion(DownPage);
            return true;
      }
   }
   return Parent::onKeyDown(event);
}

void GuiScrollCtrl::onMouseDown(const GuiEvent &event)
{
   mouseLock();

   setUpdate();

   Point2I curMousePos = globalToLocalCoord(event.mousePoint);
   curHitRegion = findHitRegion(curMousePos);
   stateDepressed = true;

   // Set a 0.5 second delay before we start scrolling
   mLastUpdated = Platform::getVirtualMilliseconds() + 500;

   scrollByRegion(curHitRegion);

   if (curHitRegion == VertThumb)
   {
      mChildRelPosAnchor = mChildRelPos;
      mThumbMouseDelta = curMousePos.y - mVThumbPos;
   }
   else if (curHitRegion == HorizThumb)
   {
      mChildRelPosAnchor = mChildRelPos;
      mThumbMouseDelta = curMousePos.x - mHThumbPos;
   }
}

void GuiScrollCtrl::onMouseUp(const GuiEvent &)
{
   mouseUnlock();

   setUpdate();

   curHitRegion = None;
   stateDepressed = false;
}

void GuiScrollCtrl::onMouseDragged(const GuiEvent &event)
{
   Point2I curMousePos = globalToLocalCoord(event.mousePoint);
   setUpdate();

   if ( (curHitRegion != VertThumb) && (curHitRegion != HorizThumb) )
   {
      Region hit = findHitRegion(curMousePos);
      if (hit != curHitRegion)
         stateDepressed = false;
      else
         stateDepressed = true;
      return;
   }

   // ok... if the mouse is 'near' the scroll bar, scroll with it
   // otherwise, snap back to the previous position.

   if (curHitRegion == VertThumb)
   {
      if (curMousePos.x >= mVTrackRect.point.x - mScrollBarDragTolerance &&
         curMousePos.x <= mVTrackRect.point.x + mVTrackRect.extent.x - 1 + mScrollBarDragTolerance &&
         curMousePos.y >= mVTrackRect.point.y - mScrollBarDragTolerance &&
         curMousePos.y <= mVTrackRect.point.y + mVTrackRect.extent.y - 1 + mScrollBarDragTolerance)
      {
         S32 newVThumbPos = curMousePos.y - mThumbMouseDelta;
         if(newVThumbPos != mVThumbPos)
         {
            S32 newVPos = (newVThumbPos - mVTrackRect.point.y) *
                          (mChildExt.y - mContentExt.y) /
                          (mVTrackRect.extent.y - mVThumbSize);

            scrollTo(mChildRelPosAnchor.x, newVPos);
         }
      }
      else
         scrollTo(mChildRelPosAnchor.x, mChildRelPosAnchor.y);
   }
   else if (curHitRegion == HorizThumb)
   {
      if (curMousePos.x >= mHTrackRect.point.x - mScrollBarDragTolerance &&
         curMousePos.x <= mHTrackRect.point.x + mHTrackRect.extent.x - 1 + mScrollBarDragTolerance &&
         curMousePos.y >= mHTrackRect.point.y - mScrollBarDragTolerance &&
         curMousePos.y <= mHTrackRect.point.y + mHTrackRect.extent.y - 1 + mScrollBarDragTolerance)
      {
         S32 newHThumbPos = curMousePos.x - mThumbMouseDelta;
         if(newHThumbPos != mHThumbPos)
         {
            S32 newHPos = (newHThumbPos - mHTrackRect.point.x) *
                          (mChildExt.x - mContentExt.x) /
                          (mHTrackRect.extent.x - mHThumbSize);

            scrollTo(newHPos, mChildRelPosAnchor.y);
         }
      }
      else
         scrollTo(mChildRelPosAnchor.x, mChildRelPosAnchor.y);
   }
}

bool GuiScrollCtrl::onMouseWheelUp(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

   Point2I previousPos = mChildPos;
   scrollByRegion((event.modifier & SI_CTRL) ? UpPage : UpArrow);

   // Tell the kids that the mouse moved (relatively):
   iterator itr;
   for ( itr = begin(); itr != end(); itr++ )
   {
      GuiControl* grandKid = static_cast<GuiControl*>( *itr );
      grandKid->onMouseMove( event );
   }

   // If no scrolling happened (already at the top), pass it on to the parent.
   GuiControl* parent = getParent();
   if (parent && (previousPos == mChildPos))
      return parent->onMouseWheelUp(event);

   return true;
}

bool GuiScrollCtrl::onMouseWheelDown(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

   Point2I previousPos = mChildPos;
   scrollByRegion((event.modifier & SI_CTRL) ? DownPage : DownArrow);

   // Tell the kids that the mouse moved (relatively):
   iterator itr;
   for ( itr = begin(); itr != end(); itr++ )
   {
      GuiControl* grandKid = static_cast<GuiControl *>( *itr );
      grandKid->onMouseMove( event );
   }

   // If no scrolling happened (already at the bottom), pass it on to the parent.
   GuiControl* parent = getParent();
   if (parent && (previousPos == mChildPos))
      return parent->onMouseWheelDown(event);

   return true;
}

void GuiScrollCtrl::onPreRender()
{
   Parent::onPreRender();

   // Short circuit if not depressed to save cycles
   if( stateDepressed != true )
      return;

   //default to one second, though it shouldn't be necessary
   U32 timeThreshold = 1000;

   // We don't want to scroll by pages at an interval the same as when we're scrolling
   // using the arrow buttons, so adjust accordingly.
   switch( curHitRegion )
   {
   case UpPage:
   case DownPage:
   case LeftPage:
   case RightPage:
      timeThreshold = 200;
      break;
   case UpArrow:
   case DownArrow:
   case LeftArrow:
   case RightArrow:
      timeThreshold = 20;
      break;
   default:
      // Neither a button or a page, don't scroll (shouldn't get here)
      return;
      break;
   };

   S32 timeElapsed = Platform::getVirtualMilliseconds() - mLastUpdated;

   if ( ( timeElapsed > 0 ) && ( timeElapsed > (S32)timeThreshold ) )
   {

      mLastUpdated = Platform::getVirtualMilliseconds();
      scrollByRegion(curHitRegion);
   }

}

void GuiScrollCtrl::scrollByRegion(Region reg)
{
   setUpdate();
   if(!size())
      return;
   GuiControl *content = (GuiControl *) front();
   U32 rowHeight, columnWidth;
   U32 pageHeight, pageWidth;

   content->getScrollLineSizes(&rowHeight, &columnWidth);

   if(rowHeight >= (U32)mContentExt.y)
      pageHeight = 1;
   else
      pageHeight = mContentExt.y - rowHeight;

   if(columnWidth >= (U32)mContentExt.x)
      pageWidth = 1;
   else
      pageWidth = mContentExt.x - columnWidth;

   if (mVBarEnabled)
   {
      switch(reg)
      {
         case UpPage:
            scrollDelta(0, -(S32)pageHeight);
            break;
         case DownPage:
            scrollDelta(0, pageHeight);
            break;
         case UpArrow:
            scrollDelta(0, -(S32)rowHeight);
            break;
         case DownArrow:
            scrollDelta(0, rowHeight);
            break;
         case LeftArrow:
         case RightArrow:
         case LeftPage:
         case RightPage:
         case VertThumb:
         case HorizThumb:
         case None:
          {
              //Con::errorf("Unhandled case in GuiScrollCtrl::scrollByRegion");
          }
      }
   }

   if (mHBarEnabled)
   {
      switch(reg)
      {
         case LeftPage:
            scrollDelta(-(S32)pageWidth, 0);
            break;
         case RightPage:
            scrollDelta(pageWidth, 0);
            break;
         case LeftArrow:
            scrollDelta(-(S32)columnWidth, 0);
            break;
         case RightArrow:
            scrollDelta(columnWidth, 0);
            break;
         case UpArrow:
         case DownArrow:
         case UpPage:
         case DownPage:
         case VertThumb:
         case HorizThumb:
         case None:
          {
              //Con::errorf("Unhandled case in GuiScrollCtrl::scrollByRegion");
              break;
          }
      }
   }
}


void GuiScrollCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   RectI r(offset.x, offset.y, mBounds.extent.x, mBounds.extent.y);

   if (mProfile->mOpaque)
      dglDrawRectFill(r, mProfile->mFillColor);

    if (mProfile->mBorder)
        renderFilledBorder(r, mProfile);

   // draw scroll bars
   if (mHasVScrollBar)
      drawVScrollBar(offset);

   if (mHasHScrollBar)
      drawHScrollBar(offset);

   //draw the scroll corner
   if (mHasVScrollBar && mHasHScrollBar)
      drawScrollCorner(offset);

   // draw content controls
   // create a rect to intersect with the updateRect
   RectI contentRect(mContentPos.x + offset.x, mContentPos.y + offset.y, mContentExt.x, mContentExt.y);
   if(contentRect.intersect(updateRect))
      renderChildControls(offset, contentRect);

   // Finally draw the last vis rect (debug aid, BJG)
   //RectI renderRect = lastVisRect;
   //renderRect.point += mContentPos + offset;

   //dglSetClipRect(r);
   //dglDrawRect(renderRect, ColorI(0, 255, 0));
}

void GuiScrollCtrl::drawBorder( const Point2I &offset, bool /*isFirstResponder*/ )
{
}

void GuiScrollCtrl::drawVScrollBar(const Point2I &offset)
{
   Point2I pos = offset + mUpArrowRect.point;
   S32 bitmap = (mVBarEnabled ? ((curHitRegion == UpArrow && stateDepressed) ?
         BmpStates * BmpUp + BmpHilite : BmpStates * BmpUp) : BmpStates * BmpUp + BmpDisabled);

   dglClearBitmapModulation();
   dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[bitmap]);

   pos.y += mScrollBarArrowBtnLength;
   S32 end;
   if (mVBarEnabled)
      end = mVThumbPos + offset.y;
   else
      end = mDownArrowRect.point.y + offset.y;

   bitmap = (mVBarEnabled ? ((curHitRegion == DownPage && stateDepressed) ?
         BmpStates * BmpVPage + BmpHilite : BmpStates * BmpVPage) : BmpStates * BmpVPage + BmpDisabled);

   if (end > pos.y)
   {
      dglClearBitmapModulation();
      dglDrawBitmapStretchSR(mTextureHandle, RectI(pos, Point2I(mBitmapBounds[bitmap].extent.x, end - pos.y)), mBitmapBounds[bitmap]);
   }

   pos.y = end;
   if (mVBarEnabled)
   {
      bool thumbSelected = (curHitRegion == VertThumb && stateDepressed);
      S32 ttop = (thumbSelected ? BmpStates * BmpVThumbTopCap + BmpHilite : BmpStates * BmpVThumbTopCap);
      S32 tmid = (thumbSelected ? BmpStates * BmpVThumb + BmpHilite : BmpStates * BmpVThumb);
      S32 tbot = (thumbSelected ? BmpStates * BmpVThumbBottomCap + BmpHilite : BmpStates * BmpVThumbBottomCap);

      // draw the thumb
      dglClearBitmapModulation();
      dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[ttop]);
      pos.y += mBitmapBounds[ttop].extent.y;
      end = mVThumbPos + mVThumbSize - mBitmapBounds[tbot].extent.y + offset.y;

      if (end > pos.y)
      {
         dglClearBitmapModulation();
         dglDrawBitmapStretchSR(mTextureHandle, RectI(pos, Point2I(mBitmapBounds[tmid].extent.x, end - pos.y)), mBitmapBounds[tmid]);
      }

      pos.y = end;
      dglClearBitmapModulation();
      dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[tbot]);
      pos.y += mBitmapBounds[tbot].extent.y;
      end = mVTrackRect.point.y + mVTrackRect.extent.y - 1 + offset.y;

      bitmap = (curHitRegion == DownPage && stateDepressed) ? BmpStates * BmpVPage + BmpHilite : BmpStates * BmpVPage;
      if (end > pos.y)
      {
         dglClearBitmapModulation();
         dglDrawBitmapStretchSR(mTextureHandle, RectI(pos, Point2I(mBitmapBounds[bitmap].extent.x, end - pos.y)), mBitmapBounds[bitmap]);
      }

      pos.y = end;
   }

   bitmap = (mVBarEnabled ? ((curHitRegion == DownArrow && stateDepressed ) ?
         BmpStates * BmpDown + BmpHilite : BmpStates * BmpDown) : BmpStates * BmpDown + BmpDisabled);

   dglClearBitmapModulation();
   dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[bitmap]);
}

void GuiScrollCtrl::drawHScrollBar(const Point2I &offset)
{
   S32 bitmap;

   //draw the left arrow
   bitmap = (mHBarEnabled ? ((curHitRegion == LeftArrow && stateDepressed) ?
            BmpStates * BmpLeft + BmpHilite : BmpStates * BmpLeft) : BmpStates * BmpLeft + BmpDisabled);

   Point2I pos = offset;
   pos += mLeftArrowRect.point;

   dglClearBitmapModulation();
   dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[bitmap]);

   pos.x += mLeftArrowRect.extent.x;

   //draw the left page
   S32 end;
   if (mHBarEnabled)
      end = mHThumbPos + offset.x;
   else
      end = mRightArrowRect.point.x + offset.x;

   bitmap = (mHBarEnabled ? ((curHitRegion == LeftPage && stateDepressed) ?
            BmpStates * BmpHPage + BmpHilite : BmpStates * BmpHPage) : BmpStates * BmpHPage + BmpDisabled);

   if (end > pos.x)
   {
      dglClearBitmapModulation();
      dglDrawBitmapStretchSR(mTextureHandle, RectI(pos, Point2I(end - pos.x, mBitmapBounds[bitmap].extent.y)), mBitmapBounds[bitmap]);
   }
   pos.x = end;

   //draw the thumb and the rightPage
   if (mHBarEnabled)
   {
      bool thumbSelected = (curHitRegion == HorizThumb && stateDepressed);
      S32 ttop = (thumbSelected ? BmpStates * BmpHThumbLeftCap + BmpHilite : BmpStates * BmpHThumbLeftCap );
      S32 tmid = (thumbSelected ? BmpStates * BmpHThumb + BmpHilite : BmpStates * BmpHThumb);
      S32 tbot = (thumbSelected ? BmpStates * BmpHThumbRightCap + BmpHilite : BmpStates * BmpHThumbRightCap);

      // draw the thumb
      dglClearBitmapModulation();
      dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[ttop]);
      pos.x += mBitmapBounds[ttop].extent.x;
      end = mHThumbPos + mHThumbSize - mBitmapBounds[tbot].extent.x + offset.x;
      if (end > pos.x)
      {
         dglClearBitmapModulation();
         dglDrawBitmapStretchSR(mTextureHandle, RectI(pos, Point2I(end - pos.x, mBitmapBounds[tmid].extent.y)), mBitmapBounds[tmid]);
      }

      pos.x = end;
      dglClearBitmapModulation();
      dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[tbot]);
      pos.x += mBitmapBounds[tbot].extent.x;
      end = mHTrackRect.point.x + mHTrackRect.extent.x - 1 + offset.x;

      bitmap = ((curHitRegion == RightPage && stateDepressed) ? BmpStates * BmpHPage + BmpHilite : BmpStates * BmpHPage);

      if (end > pos.x)
      {
         dglClearBitmapModulation();
         dglDrawBitmapStretchSR(mTextureHandle, RectI(pos, Point2I(end - pos.x, mBitmapBounds[bitmap].extent.y)), mBitmapBounds[bitmap]);
      }

      pos.x = end;
   }
   bitmap = (mHBarEnabled ? ((curHitRegion == RightArrow && stateDepressed) ?
            BmpStates * BmpRight + BmpHilite : BmpStates * BmpRight) : BmpStates * BmpRight + BmpDisabled);

   dglClearBitmapModulation();
   dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[bitmap]);
}

void GuiScrollCtrl::drawScrollCorner(const Point2I &offset)
{
   Point2I pos = offset;
   pos.x += mRightArrowRect.point.x + mRightArrowRect.extent.x;
   pos.y += mRightArrowRect.point.y;
   dglClearBitmapModulation();
   dglDrawBitmapSR(mTextureHandle, pos, mBitmapBounds[BmpStates * BmpResize]);
}

void GuiScrollCtrl::autoScroll(Region reg)
{
   scrollByRegion(reg);
}
