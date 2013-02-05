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
#include "sim/simBase.h"
#include "gui/guiCanvas.h"
#include "gui/containers/guiTabBookCtrl.h"
#include "platform/event.h"
#include "io/fileStream.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/editor/guiEditCtrl.h"
#include "gui/guiPopUpCtrl.h"
#include "gui/guiDefaultControlRender.h"


// So we can set tab alignment via gui editor
static EnumTable::Enums tabAlignEnums[] =
{
   { GuiTabBookCtrl::AlignTop,   "Top"    },
   { GuiTabBookCtrl::AlignLeft,  "Left"   },
   { GuiTabBookCtrl::AlignBottom,"Bottom" },
   { GuiTabBookCtrl::AlignRight,	"Right"  }
};
static EnumTable gTabAlignEnums(4,&tabAlignEnums[0]);


IMPLEMENT_CONOBJECT(GuiTabBookCtrl);

GuiTabBookCtrl::GuiTabBookCtrl()
{
   VECTOR_SET_ASSOCIATION(mPages);
   mTabHeight = 24;
   mLastTabHeight = mTabHeight;
   mTabPosition = GuiTabBookCtrl::AlignTop;
   mLastTabPosition = mTabPosition;
   mActivePage = NULL;
   mHoverTab = NULL;
   mHasTexture = false;
   mBitmapBounds = NULL;
   mBounds.extent.set( 400, 300 );
   mPageRect = RectI(0,0,0,0);
   mTabRect = RectI(0,0,0,0);

   mPages.reserve(12);
   mTabMargin = 7;
   mMinTabWidth = 64;
   mIsContainer = true;


}

void GuiTabBookCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("TabPosition",		TypeEnum,		Offset(mTabPosition,GuiTabBookCtrl), 1, &gTabAlignEnums );
   addField("TabHeight",		TypeS32,		Offset(mTabHeight,GuiTabBookCtrl) );
   addField("TabMargin",   TypeS32,    Offset(mTabMargin,GuiTabBookCtrl));
   addField("MinTabWidth", TypeS32,    Offset(mMinTabWidth,GuiTabBookCtrl));
}

// Empty for now, will implement for handling design time context menu for manipulating pages
ConsoleMethod( GuiTabBookCtrl, addPage, void, 2, 2, "() Empty")
{
   object->addNewPage();
}

//ConsoleMethod( GuiTabBookCtrl, removePage, void, 2, 2, "()")
//{
//}


bool GuiTabBookCtrl::onAdd()
{
   Parent::onAdd();

   return true;
}


void GuiTabBookCtrl::onRemove()
{
   Parent::onRemove();
}

void GuiTabBookCtrl::onChildRemoved( GuiControl* child )
{
   for (S32 i = 0; i < mPages.size(); i++ )
   {
      GuiTabPageCtrl* tab = mPages[i].Page;
      if( tab == child )
      {
         if( tab == mActivePage )
            mActivePage = NULL;
         mPages.erase( i );
         break;
      }
   }

   if( mPages.empty() )
      mActivePage = NULL;
   else if (mActivePage == NULL )
      mActivePage = static_cast<GuiTabPageCtrl*>(mPages[0].Page);

}

void GuiTabBookCtrl::onChildAdded( GuiControl *child )
{
   GuiTabPageCtrl *page = dynamic_cast<GuiTabPageCtrl*>(child);
   if( !page )
   {
      Con::warnf("GuiTabBookCtrl::onChildAdded - attempting to add NON GuiTabPageCtrl as child page");
      SimObject *simObj = reinterpret_cast<SimObject*>(child);
      removeObject( simObj );
      if( mActivePage )
      {
         mActivePage->addObject( simObj );
      }
      else
      {
         Con::warnf("GuiTabBookCtrl::onChildAdded - unable to find active page to reassign ownership of new child control to, placing on parent");
         GuiControl *rent = getParent();
         if( rent )
            rent->addObject( simObj );
      }
      return;
   }


   TabHeaderInfo newPage;

   newPage.Page      = page;
   newPage.TabRow    = -1;
   newPage.TabColumn = -1;

   mPages.push_back( newPage );

   // Calculate Page Information
   calculatePageTabs();

   child->resize( mPageRect.point, mPageRect.extent );


   // Select this Page
   selectPage( page );
}


bool GuiTabBookCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   mHasTexture = mProfile->constructBitmapArray();
   if( mHasTexture )
      mBitmapBounds = mProfile->mBitmapArrayRects.address();

   return true;
}

void GuiTabBookCtrl::onSleep()
{
   Parent::onSleep();
}


void GuiTabBookCtrl::addNewPage()
{
   char textbuf[1024];

   GuiTabPageCtrl * page = new GuiTabPageCtrl();

   page->setField("profile", "GuiTabPageProfile");

   dSprintf(textbuf, sizeof(textbuf), "TabBookPage%d_%d", getId(), page->getId());
   page->registerObject(textbuf);

   this->addObject( page );
}

void GuiTabBookCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   Parent::resize( newPosition, newExtent );

   calculatePageTabs();

   // Resize Children
   SimSet::iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      ctrl->resize( mPageRect.point, mPageRect.extent );
   }
}

void GuiTabBookCtrl::childResized(GuiControl *child)
{
   //Parent::childResized( child );

   child->resize( mPageRect.point, mPageRect.extent );
}

void GuiTabBookCtrl::onMouseDown(const GuiEvent &event)
{
    Point2I localMouse = globalToLocalCoord( event.mousePoint );
    if( mTabRect.pointInRect( localMouse ) )
    {
        GuiTabPageCtrl *tab = findHitTab( localMouse );
        if( tab != NULL && tab->isActive() )
            selectPage( tab );
    }
}

void GuiTabBookCtrl::onMouseMove(const GuiEvent &event)
{

   Point2I localMouse = globalToLocalCoord( event.mousePoint );
   if( mTabRect.pointInRect( localMouse ) )
   {

      GuiTabPageCtrl *tab = findHitTab( localMouse );
      if( tab != NULL && mHoverTab != tab )
         mHoverTab = tab;
      else if ( !tab )
         mHoverTab = NULL;
   }
   Parent::onMouseMove( event );
}

void GuiTabBookCtrl::onMouseLeave( const GuiEvent &event )
{
   mHoverTab = NULL;
}

bool GuiTabBookCtrl::onMouseDownEditor(const GuiEvent &event, Point2I offset)
{
   bool handled = false;
   Point2I localMouse = globalToLocalCoord( event.mousePoint );

   if( mTabRect.pointInRect( localMouse ) )
   {
      GuiTabPageCtrl *tab = findHitTab( localMouse );
      if( tab != NULL )
      {
         selectPage( tab );
         handled = true;
      }
   }

   // This shouldn't be called if it's not design time, but check just incase
   if ( GuiControl::smDesignTime )
   {
      // If we clicked in the editor and our addset is the tab book
      // ctrl, select the child ctrl so we can edit it's properties
      GuiEditCtrl* edit = GuiControl::smEditorHandle;
      if( edit  && ( edit->getAddSet() == this ) && mActivePage != NULL )
         edit->select( mActivePage );
   }

   // Return whether we handled this or not.
   return handled;

}

void GuiTabBookCtrl::onPreRender()
{
   // sometimes we need to resize because of a changed persistent field
   // that's what this does
   solveDirty();
}

void GuiTabBookCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   RectI tabRect = mTabRect;
   tabRect.point += offset;
   RectI pageRect = mPageRect;
   pageRect.point += offset;

   // We're so nice we'll store the old modulation before we clear it for our rendering! :)
   ColorI oldModulation;
   dglGetBitmapModulation( &oldModulation );

   // Wipe it out
   dglClearBitmapModulation();

   // Render background
   renderBackground( offset, updateRect );

   // Render our tabs
   renderTabs( offset );

   // Render Children
   renderChildControls( offset, updateRect );

   // Restore old modulation
   dglSetBitmapModulation( oldModulation );
}

void GuiTabBookCtrl::renderBackground( Point2I offset, const RectI& updateRect )
{
   RectI winRect;
   winRect.point = offset;
   winRect.extent = mBounds.extent;

   if( mHasTexture && mProfile->mBitmapArrayRects.size() >= NumBitmaps)
      renderSizableBitmapBordersFilledIndex( winRect, TabBackground, mProfile );
   else
      dglDrawRectFill(winRect, mProfile->mFillColor);

}


void GuiTabBookCtrl::renderTabs( const Point2I &offset )
{
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty() || mTabHeight <= 0 )
      return;

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      RectI tabBounds = mPages[i].TabRect;
      tabBounds.point += offset;
      GuiTabPageCtrl *tab = mPages[i].Page;
      if( tab != NULL )
         renderTab( tabBounds, tab );
   }
}

void GuiTabBookCtrl::renderTab( RectI tabRect, GuiTabPageCtrl *tab )
{
   StringTableEntry text = tab->getText();
   ColorI oldColor;

   dglGetBitmapModulation( &oldColor );

   // Is this a skinned control?
   if( mHasTexture && mProfile->mBitmapArrayRects.size() >= 9 )
   {
      S32 indexMultiplier = 1;
      switch( mTabPosition )
      {
      case AlignTop:
      case AlignBottom:
         
         if ( mActivePage == tab )
            indexMultiplier += TabSelected;
         else if( mHoverTab == tab )
            indexMultiplier += TabHover;
         else
            indexMultiplier += TabNormal;
         
         //dglDrawBitmapStretchSR(mProfile->mTextureHandle,tabRect,stretchRect, ( mTabPosition == AlignBottom ) ? GFlip_Y : 0 );
         break;
      case AlignLeft:
      case AlignRight:
         if ( mActivePage == tab )
            indexMultiplier += TabSelectedVertical;
         else if( mHoverTab == tab )
            indexMultiplier += TabHoverVertical;
         else
            indexMultiplier += TabNormalVertical;

         //dglDrawBitmapStretchSR(mProfile->mTextureHandle,tabRect,stretchRect, ( mTabPosition == AlignRight ) ? GFlip_X : 0 );
         break;
      } 

      renderFixedBitmapBordersFilled( tabRect, indexMultiplier, mProfile );
   }
   else
   {
      // If this isn't a skinned control or the bitmap is simply missing, handle it WELL
      if ( mActivePage == tab )
         dglDrawRectFill(tabRect, mProfile->mFillColor);
      else if( mHoverTab == tab )
         dglDrawRectFill(tabRect, mProfile->mFillColorHL);
      else
         dglDrawRectFill(tabRect, mProfile->mFillColorNA);

   }


   dglSetBitmapModulation(mProfile->mFontColor);

   switch( mTabPosition )
   {
   case AlignTop:
   case AlignBottom:
      renderJustifiedTextRot( tabRect.point, tabRect.extent, text, 0);
   break;
   case AlignLeft:
      renderJustifiedTextRot( tabRect.point, tabRect.extent, text, -90 );
      break;
   case AlignRight:
      renderJustifiedTextRot( tabRect.point, tabRect.extent, text, -90 );
      break;
   }

   dglSetBitmapModulation( oldColor);

}

void GuiTabBookCtrl::renderJustifiedTextRot(Point2I offset, Point2I extent, const char *text, F32 rot )
{
   GFont *font = mProfile->mFont;
   S32 textWidth = font->getStrWidth(text);
   Point2I start;

   offset += mProfile->mTextOffset;

   if( mTabPosition == AlignLeft || mTabPosition == AlignRight )
   {
      switch( mProfile->mAlignment )
      {
      case GuiControlProfile::RightJustify:
         start.set( 0, extent.y - textWidth);
         break;
      case GuiControlProfile::CenterJustify:
         start.set( 0, ( extent.y - textWidth) / 2);
         break;
      default:
         // GuiControlProfile::LeftJustify
         start.set( 0, 0 );
         break;
      }

      if( textWidth > extent.y )
         start.set( 0, 0 );
      start.x = ( ( extent.x - font->getHeight() ) / 2 ) + font->getHeight();

   }
   else
   {
      // align the horizontal
      switch( mProfile->mAlignment )
      {
      case GuiControlProfile::RightJustify:
         start.set( extent.x - textWidth, 0 );
         break;
      case GuiControlProfile::CenterJustify:
         start.set( ( extent.x - textWidth) / 2, 0 );
         break;
      default:
         // GuiControlProfile::LeftJustify
         start.set( 0, 0 );
         break;
      }

      if( textWidth > extent.x )
         start.set( 0, 0 );
      start.y = ( extent.y - font->getHeight() ) / 2;

   }

   dglDrawText( font, start + offset, text, mProfile->mFontColors,9,rot );
}

// This is nothing but a clever hack to allow the tab page children
// to cast this to a GuiControl* so that the file doesn't need to have circular
// includes.  generic method overriding for the win!
void GuiTabBookCtrl::setUpdate()
{
   Parent::setUpdate();

   setUpdateRegion(Point2I(0,0), mBounds.extent);

   calculatePageTabs();
}

void GuiTabBookCtrl::solveDirty()
{
   bool dirty = false;
   if( mTabPosition != mLastTabPosition )
   {
      mLastTabPosition = mTabPosition;
      dirty = true;
   }

   if( mTabHeight != mLastTabHeight )
   {
      mLastTabHeight = mTabHeight;
      dirty = true;
   }

   if( mTabWidth != mLastTabWidth )
   {
      mLastTabWidth = mTabWidth;
      dirty = true;
   }

   if( dirty )
   {
      calculatePageTabs();
      resize( mBounds.point, mBounds.extent );
   }

}

S32 GuiTabBookCtrl::calculatePageTabWidth( GuiTabPageCtrl *page )
{
   if( !page )
      return mTabWidth;

   StringTableEntry text = page->getText();

   if( !text || dStrlen(text) == 0 || mProfile->mFont == '\0' )
      return mTabWidth;

   GFont *font = mProfile->mFont;

   return font->getStrNWidth( text, dStrlen(text) );

}

void GuiTabBookCtrl::calculatePageTabs()
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty() || mTabHeight <= 0 )
      return;

   S32 currRow    = 0;
   S32 currColumn = 0;
   S32 currX      = 0;
   S32 currY      = 0;
   S32 maxWidth   = 0;

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      // Fetch Tab Width
      S32 tabWidth = calculatePageTabWidth( mPages[i].Page ) + ( mTabMargin * 2 );
      tabWidth = getMax( tabWidth, mMinTabWidth );
      TabHeaderInfo &info = mPages[i];
      switch( mTabPosition )
      {
      case AlignTop:
      case AlignBottom:
         // If we're going to go outside our bounds
         // with this tab move it down a row
         if( currX + tabWidth > mBounds.extent.x )
         {
            // Calculate and Advance State.
            maxWidth = getMax( tabWidth, maxWidth );
            balanceRow( currRow, currX );
            info.TabRow = ++currRow;
            // Reset Necessaries
            info.TabColumn = currColumn = maxWidth = currX = 0;
         }
         else
         {
            info.TabRow = currRow;
            info.TabColumn = currColumn++;
         }

         // Calculate Tabs Bounding Rect
         info.TabRect.point.x  = currX;
         info.TabRect.extent.x = tabWidth;
         info.TabRect.extent.y = mTabHeight;

         // Adjust Y Point based on alignment
         if( mTabPosition == AlignTop )
            info.TabRect.point.y  = ( info.TabRow * mTabHeight );
         else 
            info.TabRect.point.y  = mBounds.extent.y - ( ( 1 + info.TabRow ) * mTabHeight );

         currX += tabWidth;
         break;
      case AlignLeft:
      case AlignRight:
         // If we're going to go outside our bounds
         // with this tab move it down a row
         if( currY + tabWidth > mBounds.extent.y )
         {

            // Balance Tab Column.
            balanceColumn( currColumn, currY );

            // Calculate and Advance State.
            info.TabColumn = ++currColumn;
            info.TabRow = currRow = currY = 0;
         }
         else
         {
            info.TabColumn = currColumn;
            info.TabRow = currRow++;
         }

         // Calculate Tabs Bounding Rect
         info.TabRect.point.y  = currY;
         info.TabRect.extent.y = tabWidth;
         info.TabRect.extent.x = mTabHeight;

         // Adjust Y Point based on alignment
         if( mTabPosition == AlignLeft )
            info.TabRect.point.x  = ( info.TabColumn * mTabHeight );
         else 
            info.TabRect.point.x  = mBounds.extent.x - ( (1 + info.TabColumn) * mTabHeight );

         currY += tabWidth;
         break;
      };
   }

   currRow++;
   currColumn++;

   Point2I localPoint = mBounds.extent;

   // Calculate 
   switch( mTabPosition )
   {
   case AlignTop:

      localPoint.y -= mBounds.point.y;

      mTabRect.point.x = 0;
      mTabRect.extent.x = localPoint.x;
      mTabRect.point.y = 0;
      mTabRect.extent.y = currRow * mTabHeight;

      mPageRect.point.x = 0;
      mPageRect.point.y = mTabRect.extent.y;
      mPageRect.extent.x = mTabRect.extent.x;
      mPageRect.extent.y = mBounds.extent.y - mTabRect.extent.y;

      break;
   case AlignBottom:
      mTabRect.point.x = 0;
      mTabRect.extent.x = localPoint.x;
      mTabRect.extent.y = currRow * mTabHeight;
      mTabRect.point.y = mBounds.extent.y - mTabRect.extent.y;

      mPageRect.point.x = 0;
      mPageRect.point.y = 0;
      mPageRect.extent.x = mTabRect.extent.x;
      mPageRect.extent.y = localPoint.y - mTabRect.extent.y;

      break;
   case AlignLeft:
      

      mTabRect.point.y = 0;
      mTabRect.extent.y = mBounds.extent.y;
      mTabRect.point.x = 0;
      mTabRect.extent.x = currColumn * mTabHeight;

      mPageRect.point.y = 0;
      mPageRect.point.x = mTabRect.extent.x;
      mPageRect.extent.y = localPoint.y;
      mPageRect.extent.x = localPoint.x - mTabRect.extent.x;

      break;
   case AlignRight:


      mTabRect.extent.x = currColumn * mTabHeight;
      mTabRect.point.y = 0;
      mTabRect.extent.y = localPoint.y;
      mTabRect.point.x = localPoint.x - mTabRect.extent.x;
      
      mPageRect.point.y = 0;
      mPageRect.point.x = 0;
      mPageRect.extent.y = localPoint.y;
      mPageRect.extent.x = localPoint.x - mTabRect.extent.x;

      break;
   };


}

void GuiTabBookCtrl::balanceColumn( S32 column , S32 totalTabWidth )
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty() || mTabHeight <= 0 )
      return;

   Vector<TabHeaderInfo*> rowTemp;
   rowTemp.clear();

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      TabHeaderInfo &info = mPages[i];

      if(info.TabColumn == column )
         rowTemp.push_back( &mPages[i] );
   }

   if( rowTemp.empty() )
      return;

   // Balance the tabs across the remaining space
   S32 spaceToDivide = mBounds.extent.y - totalTabWidth;
   S32 pointDelta    = 0;
   for( S32 i = 0; i < rowTemp.size(); i++ )
   {
      TabHeaderInfo &info = *rowTemp[i];
      S32 extraSpace = (S32)( spaceToDivide / rowTemp.size() );
      info.TabRect.extent.y += extraSpace;
      info.TabRect.point.y += pointDelta;
      pointDelta += extraSpace;
   }

}
void GuiTabBookCtrl::balanceRow( S32 row, S32 totalTabWidth )
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty() || mTabHeight <= 0 )
      return;

   Vector<TabHeaderInfo*> rowTemp;
   rowTemp.clear();

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      TabHeaderInfo &info = mPages[i];

      if(info.TabRow == row )
         rowTemp.push_back( &mPages[i] );
   }

   if( rowTemp.empty() )
      return;

   // Balance the tabs across the remaining space
   S32 spaceToDivide = mBounds.extent.x - totalTabWidth;
   S32 pointDelta    = 0;
   for( S32 i = 0; i < rowTemp.size(); i++ )
   {
      TabHeaderInfo &info = *rowTemp[i];
      S32 extraSpace = (S32)spaceToDivide / ( rowTemp.size() );
      info.TabRect.extent.x += extraSpace;
      info.TabRect.point.x += pointDelta;
      pointDelta += extraSpace;
   }
}


GuiTabPageCtrl *GuiTabBookCtrl::findHitTab( const GuiEvent &event )
{
   return findHitTab( event.mousePoint );
}

GuiTabPageCtrl *GuiTabBookCtrl::findHitTab( Point2I hitPoint )
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty() || mTabHeight <= 0 )
      return NULL;

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      if( mPages[i].TabRect.pointInRect( hitPoint ) )
         return mPages[i].Page;
   }
   return NULL;
}

void GuiTabBookCtrl::selectPage( S32 index )
{
   if( mPages.size() < index )
      return;

   // Select the page
   selectPage( mPages[ index ].Page );
}


void GuiTabBookCtrl::selectPage( GuiTabPageCtrl *page )
{
   Vector<TabHeaderInfo>::iterator i = mPages.begin();
   for( ; i != mPages.end() ; i++ )
   {
      GuiTabPageCtrl *tab = reinterpret_cast<GuiTabPageCtrl*>((*i).Page);
      if( page == tab )
      {
         mActivePage = tab;
         tab->setVisible( true );

         // Notify User
         char *retBuffer = Con::getReturnBuffer( 512 );
         dStrcpy( retBuffer, tab->getText() );
         Con::executef( this, 2, "onTabSelected",  retBuffer );

      }
      else
         tab->setVisible( false );
   }
}

void GuiTabBookCtrl::selectPage( const char* pageName )
{
   Vector<TabHeaderInfo>::iterator i = mPages.begin();
   for( ; i != mPages.end() ; i++ )
   {
      GuiTabPageCtrl *tab = reinterpret_cast<GuiTabPageCtrl*>((*i).Page);
      if( dStricmp( pageName, tab->getText() ) == 0 )
      {
         mActivePage = tab;
         tab->setVisible( true );

         // Notify User
         char *retBuffer = Con::getReturnBuffer( 512 );
         dStrcpy( retBuffer, tab->getText() );
         Con::executef( this, 2, "onTabSelected",  retBuffer );

      }
      else
         tab->setVisible( false );
   }
}


bool GuiTabBookCtrl::onKeyDown(const GuiEvent &event)
{
   // Tab      = Next Page
   // Ctrl-Tab = Previous Page
   if( 0 && event.keyCode == KEY_TAB )
   {
      if( event.modifier & SI_CTRL )
         selectPrevPage();
      else 
         selectNextPage();

      return true;
   }

   return Parent::onKeyDown( event );
}

void GuiTabBookCtrl::selectNextPage()
{
   if( mPages.empty() )
      return;

   if( mActivePage == NULL )
      mActivePage = mPages[0].Page;

   S32 nI = 0;
   for( ; nI < mPages.size(); nI++ )
   {
      GuiTabPageCtrl *tab = mPages[ nI ].Page;
      if( tab == mActivePage )
      {
         if( nI == ( mPages.size() - 1 ) )
            selectPage( 0 );
         else if ( nI + 1 <= ( mPages.size() - 1 ) ) 
            selectPage( nI + 1 );
         else
            selectPage( 0 );

         // Notify User
         if( isMethod( "onTabSelected" ) )
         {
            char *retBuffer = Con::getReturnBuffer( 512 );
            dStrcpy( retBuffer, tab->getText() );
            Con::executef( this, 2, "onTabSelected",  retBuffer );
         }

         return;
      }
   }
}

void GuiTabBookCtrl::selectPrevPage()
{
   if( mPages.empty() )
      return;

   if( mActivePage == NULL )
      mActivePage = mPages[0].Page;

   S32 nI = 0;
   for( ; nI < mPages.size(); nI++ )
   {
      GuiTabPageCtrl *tab = mPages[ nI ].Page;
      if( tab == mActivePage )
      {
         if( nI == 0 )
            selectPage( mPages.size() - 1 );
         else
            selectPage( nI - 1 );

         // Notify User
         if( isMethod( "onTabSelected" ) )
         {
            char *retBuffer = Con::getReturnBuffer( 512 );
            dStrcpy( retBuffer, tab->getText() );
            Con::executef( this, 2, "onTabSelected",  retBuffer );
         }

         return;
      }
   }

}

ConsoleMethod( GuiTabBookCtrl, selectPage, void, 3, 3, "(int pageIndex)")
{
   S32 pageIndex = dAtoi(argv[2]);

   object->selectPage(pageIndex);
}

ConsoleMethod( GuiTabBookCtrl, selectPageName, void, 3, 3, "(pageName)")
{
   object->selectPage(argv[2]);
}
