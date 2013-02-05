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

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gui/guiCanvas.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/editor/guiSeparatorCtrl.h"

IMPLEMENT_CONOBJECT(GuiSeparatorCtrl);

static EnumTable::Enums separatorTypeEnum[] =
{
   { GuiSeparatorCtrl::separatorTypeVertical, "Vertical"  },
   { GuiSeparatorCtrl::separatorTypeHorizontal,"Horizontal" }
};
static EnumTable gSeparatorTypeTable(2, &separatorTypeEnum[0]);


//--------------------------------------------------------------------------
GuiSeparatorCtrl::GuiSeparatorCtrl() : GuiControl()
{
   mInvisible = false;
   mText = StringTable->EmptyString;
   mTextLeftMargin = 0;
   mMargin = 2;
   mBounds.extent.set( 12, 35 );
   mSeparatorType = GuiSeparatorCtrl::separatorTypeVertical;
}

//--------------------------------------------------------------------------
void GuiSeparatorCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("Caption",         TypeString, Offset(mText,           GuiSeparatorCtrl));
   addField("Type",            TypeEnum,   Offset(mSeparatorType,  GuiSeparatorCtrl), 1, &gSeparatorTypeTable);
   addField("BorderMargin",      TypeS32,    Offset(mMargin, GuiSeparatorCtrl));
   addField("Invisible",      TypeBool,    Offset(mInvisible, GuiSeparatorCtrl));
   addField("LeftMargin",      TypeS32,    Offset(mTextLeftMargin, GuiSeparatorCtrl));
}

//--------------------------------------------------------------------------
void GuiSeparatorCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   Parent::onRender( offset, updateRect );

   if( mInvisible )
      return;

   if(mText != StringTable->EmptyString && !( mSeparatorType == separatorTypeVertical ) )
   {
      // If text is present and we have a left margin, then draw some separator, then the
      // text, and then the rest of the separator
      S32 posx = offset.x + mMargin;
      S32 fontheight = mProfile->mFont->getHeight();
      S32 seppos = (fontheight - 2) / 2 + offset.y;
      if(mTextLeftMargin > 0)
      {
         RectI rect(Point2I(posx,seppos),Point2I(mTextLeftMargin,2));
         renderSlightlyLoweredBox(rect, mProfile);
         posx += mTextLeftMargin;
      }

      dglSetBitmapModulation( mActive ? mProfile->mFillColor : mProfile->mFillColorNA );
      posx += dglDrawText(mProfile->mFont, Point2I(posx,offset.y), mText, mProfile->mFontColors);
      //posx += mProfile->mFont->getStrWidth(mText);

      RectI rect(Point2I(posx,seppos),Point2I(mBounds.extent.x - posx + offset.x,2));
      renderSlightlyLoweredBox(rect, mProfile, mActive);

   } else
   {
      if( mSeparatorType == separatorTypeHorizontal )
      {
         S32 seppos = mBounds.extent.y / 2 + offset.y; 
         RectI rect(Point2I(offset.x + mMargin ,seppos),Point2I(mBounds.extent.x - (mMargin * 2),2));
         renderSlightlyLoweredBox(rect, mProfile);
      }
      else
      {
         S32 seppos = mBounds.extent.x / 2 + offset.x; 
         RectI rect(Point2I(seppos, offset.y + mMargin),Point2I(2, mBounds.extent.y - (mMargin * 2)));
         renderSlightlyLoweredBox(rect, mProfile);
      }
   }

   renderChildControls(offset, updateRect);
}


