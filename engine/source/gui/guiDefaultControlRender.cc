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
#include "gui/guiDefaultControlRender.h"
#include "gui/guiTypes.h"
#include "graphics/color.h"
#include "math/mRect.h"

static ColorI colorLightGray(192, 192, 192);
static ColorI colorGray(128, 128, 128);
static ColorI colorDarkGray(64, 64, 64);
static ColorI colorWhite(255,255,255);
static ColorI colorBlack(0,0,0);

void renderRaisedBox(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   dglDrawRectFill( bounds, profile->mFillColor);
   dglDrawLine(l, t, l, b - 1, profile->mBevelColorHL);
   dglDrawLine(l, t, r - 1, t, profile->mBevelColorHL);

   dglDrawLine(l, b, r, b, profile->mBevelColorLL);
   dglDrawLine(r, b - 1, r, t, profile->mBevelColorLL);

   dglDrawLine(l + 1, b - 1, r - 1, b - 1, profile->mBorderColor);
   dglDrawLine(r - 1, b - 2, r - 1, t + 1, profile->mBorderColor);
}

void renderSlightlyRaisedBox(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   dglDrawRectFill( bounds, profile->mFillColor);
   dglDrawLine(l, t, l, b, profile->mBevelColorHL);
   dglDrawLine(l, t, r, t, profile->mBevelColorHL);
   dglDrawLine(l + 1, b, r, b, profile->mBorderColor);
   dglDrawLine(r, t + 1, r, b - 1, profile->mBorderColor);
}

void renderLoweredBox(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   dglDrawRectFill( bounds, profile->mFillColor);

   dglDrawLine(l, b, r, b, profile->mBevelColorHL);
   dglDrawLine(r, b - 1, r, t, profile->mBevelColorHL);

   dglDrawLine(l, t, r - 1, t, profile->mBevelColorLL);
   dglDrawLine(l, t + 1, l, b - 1, profile->mBevelColorLL);

   dglDrawLine(l + 1, t + 1, r - 2, t + 1, profile->mBorderColor);
   dglDrawLine(l + 1, t + 2, l + 1, b - 2, profile->mBorderColor);
}

//void renderSlightlyLoweredBox(RectI &bounds, GuiControlProfile *profile)
//{
//   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
//   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;
//
//   dglDrawRectFill( bounds, profile->mFillColor);
//   dglDrawLine(l, b, r, b, profile->mBevelColorHL);
//   dglDrawLine(r, t, r, b - 1, profile->mBevelColorHL);
//   dglDrawLine(l, t, l, b - 1, profile->mBorderColor);
//   dglDrawLine(l + 1, t, r - 1, t, profile->mBorderColor);
//}

void renderSlightlyLoweredBox(RectI &bounds, GuiControlProfile *profile, bool active)
{
    S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
    S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

    if (active)
    {
        dglDrawRectFill( bounds, profile->mFillColor);
        dglDrawLine(l, b, r, b, profile->mBevelColorHL);
        dglDrawLine(r, t, r, b - 1, profile->mBevelColorHL);
        dglDrawLine(l, t, l, b - 1, profile->mBorderColor);
        dglDrawLine(l + 1, t, r - 1, t, profile->mBorderColor);
    }
    else
    {
        dglDrawRectFill( bounds, profile->mFillColorNA);
        dglDrawLine(l, b, r, b, profile->mBorderColorNA);
        dglDrawLine(r, t, r, b - 1, profile->mBorderColorNA);
        dglDrawLine(l, t, l, b - 1, profile->mBorderColorNA);
        dglDrawLine(l + 1, t, r - 1, t, profile->mBorderColorNA);
    }
}

void renderBorder(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   switch(profile->mBorder)
   {
   case 1:
      dglDrawRect(bounds, profile->mBorderColor);
      break;
   case 2:
      dglDrawLine(l + 1, t + 1, l + 1, b - 2, profile->mBevelColorHL);
      dglDrawLine(l + 2, t + 1, r - 2, t + 1, profile->mBevelColorHL);
      dglDrawLine(r, t, r, b, profile->mBevelColorHL);
      dglDrawLine(l, b, r - 1, b, profile->mBevelColorHL);
      dglDrawLine(l, t, r - 1, t, profile->mBorderColorNA);
      dglDrawLine(l, t + 1, l, b - 1, profile->mBorderColorNA);
      dglDrawLine(l + 1, b - 1, r - 1, b - 1, profile->mBorderColorNA);
      dglDrawLine(r - 1, t + 1, r - 1, b - 2, profile->mBorderColorNA);
      break;
   case 3:
      dglDrawLine(l, b, r, b, profile->mBevelColorHL);
      dglDrawLine(r, t, r, b - 1, profile->mBevelColorHL);
      dglDrawLine(l + 1, b - 1, r - 1, b - 1, profile->mFillColor);
      dglDrawLine(r - 1, t + 1, r - 1, b - 2, profile->mFillColor);
      dglDrawLine(l, t, l, b - 1, profile->mBorderColorNA);
      dglDrawLine(l + 1, t, r - 1, t, profile->mBorderColorNA);
      dglDrawLine(l + 1, t + 1, l + 1, b - 2, profile->mBevelColorLL);
      dglDrawLine(l + 2, t + 1, r - 2, t + 1, profile->mBevelColorLL);
      break;
   case 4:
      dglDrawLine(l, t, l, b - 1, profile->mBevelColorHL);
      dglDrawLine(l + 1, t, r, t, profile->mBevelColorHL);
      dglDrawLine(l, b, r, b, profile->mBevelColorLL);
      dglDrawLine(r, t + 1, r, b - 1, profile->mBevelColorLL);
      dglDrawLine(l + 1, b - 1, r - 1, b - 1, profile->mBorderColor);
      dglDrawLine(r - 1, t + 1, r - 1, b - 2, profile->mBorderColor);
      break;
   case 5:
      renderFilledBorder( bounds, profile );
      break;
    
   case 6:// Draw boarder only on top and left
       dglDrawLine(l, t, l, b, profile->mBorderColor);
       dglDrawLine(l, t, r, t, profile->mBorderColor);
       break;
   case 7:// Draw boarder only on bottom and right
       dglDrawLine(r, t, r, b, profile->mBorderColor);
       dglDrawLine(l, b, r, b, profile->mBorderColor);
       break;
      // DAW:
   case -1:
      // Draw a simple sizable border with corners
      // Taken from the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin       
      if(profile->mBitmapArrayRects.size() >= 8)
      {
         dglClearBitmapModulation();

         RectI destRect;
         RectI stretchRect;
         RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

         // DAW: Indices into the bitmap array
         enum
         {
            BorderTopLeft = 0,
            BorderTop,
            BorderTopRight,
            BorderLeft,
            //Fill,
            BorderRight,
            BorderBottomLeft,
            BorderBottom,
            BorderBottomRight,
            NumBitmaps
         };

         // Draw all corners first.

         //top left border
         dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderTopLeft]);
         //top right border
         dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x,bounds.point.y),mBitmapBounds[BorderTopRight]);

         //bottom left border
         dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomLeft].extent.y),mBitmapBounds[BorderBottomLeft]);
         //bottom right border
         dglDrawBitmapSR(profile->mTextureHandle,Point2I(
            bounds.point.x + bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x,
            bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomRight].extent.y),
            mBitmapBounds[BorderBottomRight]);

         // End drawing corners

         // Begin drawing sides and top stretched borders

         //start with top line stretch
         destRect.point.x = bounds.point.x + mBitmapBounds[BorderTopRight].extent.x;
         destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x - mBitmapBounds[BorderTopLeft].extent.x;
         destRect.extent.y = mBitmapBounds[BorderTop].extent.y;
         destRect.point.y = bounds.point.y;
         //stretch it
         stretchRect = mBitmapBounds[BorderTop];
         stretchRect.inset(1,0);
         //draw it
         dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
         //bottom line stretch
         destRect.point.x = bounds.point.x + mBitmapBounds[BorderBottomRight].extent.x;
         destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x - mBitmapBounds[BorderBottomLeft].extent.x;
         destRect.extent.y = mBitmapBounds[BorderBottom].extent.y;
         destRect.point.y = bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottom].extent.y;
         //stretch it
         stretchRect = mBitmapBounds[BorderBottom];
         stretchRect.inset(1,0);
         //draw it
         dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
         //left line stretch
         destRect.point.x = bounds.point.x;
         destRect.extent.x = mBitmapBounds[BorderLeft].extent.x;
         destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopLeft].extent.y - mBitmapBounds[BorderBottomLeft].extent.y;
         destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopLeft].extent.y;
         //stretch it
         stretchRect = mBitmapBounds[BorderLeft];
         stretchRect.inset(0,1);
         //draw it
         dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
         //left line stretch
         destRect.point.x = bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x;
         destRect.extent.x = mBitmapBounds[BorderRight].extent.x;
         destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopRight].extent.y - mBitmapBounds[BorderBottomRight].extent.y;
         destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopRight].extent.y;
         //stretch it
         stretchRect = mBitmapBounds[BorderRight];
         stretchRect.inset(0,1);
         //draw it
         dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);

         // End drawing sides and top stretched borders
         break;
      }
   case -2:
      // Draw a simple sizable border with corners that is filled in
      renderSizableBitmapBordersFilled(bounds, 1, profile);
      break;
   case -3:
      // Draw a simple fixed height border with center fill horizontally.
      renderFixedBitmapBordersFilled( bounds, 1, profile );
      break;

   }
}

void renderFilledBorder( RectI &bounds, GuiControlProfile *profile )
{
   renderFilledBorder( bounds, profile->mBorderColor, profile->mFillColor );
}

void renderFilledBorder( RectI &bounds, ColorI &borderColor, ColorI &fillColor )
{
   RectI fillBounds = bounds;
   fillBounds.inset( 1, 1 );
   dglDrawRect( bounds, borderColor ); 
   dglDrawRectFill( fillBounds, fillColor );
}

// DAW: Render out the sizable bitmap borders based on a multiplier into the bitmap array
// Based on the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin
void renderSizableBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 9;
   S32 BorderTopLeft =     NumBitmaps * baseMultiplier - NumBitmaps;
   S32 BorderTop =         1 + BorderTopLeft;
   S32 BorderTopRight =    2 + BorderTopLeft;
   S32 BorderLeft =        3 + BorderTopLeft;
   S32 Fill =              4 + BorderTopLeft;
   S32 BorderRight =       5 + BorderTopLeft;
   S32 BorderBottomLeft =  6 + BorderTopLeft;
   S32 BorderBottom =      7 + BorderTopLeft;
   S32 BorderBottomRight = 8 + BorderTopLeft;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (NumBitmaps * baseMultiplier))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //top left border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderTopLeft]);
      //top right border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x,bounds.point.y),mBitmapBounds[BorderTopRight]);

      //bottom left border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomLeft].extent.y),mBitmapBounds[BorderBottomLeft]);
      //bottom right border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(
         bounds.point.x + bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x,
         bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomRight].extent.y),
         mBitmapBounds[BorderBottomRight]);

      // End drawing corners

      // Begin drawing sides and top stretched borders

      //start with top line stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderTopRight].extent.x;
      destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x - mBitmapBounds[BorderTopLeft].extent.x;
      destRect.extent.y = mBitmapBounds[BorderTop].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[BorderTop];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
      //bottom line stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderBottomRight].extent.x;
      destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x - mBitmapBounds[BorderBottomLeft].extent.x;
      destRect.extent.y = mBitmapBounds[BorderBottom].extent.y;
      destRect.point.y = bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottom].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[BorderBottom];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
      //left line stretch
      destRect.point.x = bounds.point.x;
      destRect.extent.x = mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopLeft].extent.y - mBitmapBounds[BorderBottomLeft].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopLeft].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[BorderLeft];
      stretchRect.inset(0,1);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
      //left line stretch
      destRect.point.x = bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.x = mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopRight].extent.y - mBitmapBounds[BorderBottomRight].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopRight].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[BorderRight];
      stretchRect.inset(0,1);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTop].extent.y - mBitmapBounds[BorderBottom].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[BorderTop].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,1);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);

      // End drawing sides and top stretched borders
   }
}


// DAW: Render out the sizable bitmap borders based on a multiplier into the bitmap array
// Based on the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin
void renderSizableBitmapBordersFilledIndex(RectI &bounds, S32 startIndex, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 9;
   S32 BorderTopLeft =     startIndex;
   S32 BorderTop =         1 + BorderTopLeft;
   S32 BorderTopRight =    2 + BorderTopLeft;
   S32 BorderLeft =        3 + BorderTopLeft;
   S32 Fill =              4 + BorderTopLeft;
   S32 BorderRight =       5 + BorderTopLeft;
   S32 BorderBottomLeft =  6 + BorderTopLeft;
   S32 BorderBottom =      7 + BorderTopLeft;
   S32 BorderBottomRight = 8 + BorderTopLeft;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (startIndex + NumBitmaps))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //top left border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderTopLeft]);
      //top right border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x,bounds.point.y),mBitmapBounds[BorderTopRight]);

      //bottom left border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomLeft].extent.y),mBitmapBounds[BorderBottomLeft]);
      //bottom right border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(
         bounds.point.x + bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x,
         bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomRight].extent.y),
         mBitmapBounds[BorderBottomRight]);

      // End drawing corners

      // Begin drawing sides and top stretched borders

      //start with top line stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderTopLeft].extent.x;
      destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x - mBitmapBounds[BorderTopLeft].extent.x;
      destRect.extent.y = mBitmapBounds[BorderTop].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[BorderTop];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
      //bottom line stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderBottomLeft].extent.x;
      destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x - mBitmapBounds[BorderBottomLeft].extent.x;
      destRect.extent.y = mBitmapBounds[BorderBottom].extent.y;
      destRect.point.y = bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottom].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[BorderBottom];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
      //left line stretch
      destRect.point.x = bounds.point.x;
      destRect.extent.x = mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopLeft].extent.y - mBitmapBounds[BorderBottomLeft].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopLeft].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[BorderLeft];
      stretchRect.inset(0,1);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
      //left line stretch
      destRect.point.x = bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.x = mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopRight].extent.y - mBitmapBounds[BorderBottomRight].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopRight].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[BorderRight];
      stretchRect.inset(0,1);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTop].extent.y - mBitmapBounds[BorderBottom].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[BorderTop].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,1);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);

      // End drawing sides and top stretched borders
   }
}



// DAW: Render out the fixed bitmap borders based on a multiplier into the bitmap array
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 3;
   S32 BorderLeft =     NumBitmaps * baseMultiplier - NumBitmaps;
   S32 Fill =              1 + BorderLeft;
   S32 BorderRight =       2 + BorderLeft;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (NumBitmaps * baseMultiplier))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //left border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderLeft]);
      //right border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x,bounds.point.y),mBitmapBounds[BorderRight]);

      // End drawing corners

      // Begin drawing fill

      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = mBitmapBounds[Fill].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);

      // End drawing fill
   }
}

// DAW: Render out the fixed bitmap borders based on a multiplier into the bitmap array
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersFilledIndex(RectI &bounds, S32 startIndex, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 3;
   S32 BorderLeft =     startIndex;
   S32 Fill =              1 + startIndex;
   S32 BorderRight =       2 + startIndex;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (startIndex + NumBitmaps))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //left border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderLeft]);
      //right border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x,bounds.point.y),mBitmapBounds[BorderRight]);

      // End drawing corners

      // Begin drawing fill

      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = mBitmapBounds[Fill].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);

      // End drawing fill
   }
}

// DAW: Render out the fixed bitmap borders based on a multiplier into the bitmap array
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersStretchYFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 3;
   S32 BorderLeft =     NumBitmaps * baseMultiplier - NumBitmaps;
   S32 Fill =              1 + BorderLeft;
   S32 BorderRight =       2 + BorderLeft;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (NumBitmaps * baseMultiplier))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //left border
      dglDrawBitmapStretchSR(profile->mTextureHandle, RectI( bounds.point.x, bounds.point.y, mBitmapBounds[BorderLeft].extent.x, bounds.extent.y ), mBitmapBounds[BorderLeft] );
      //right border
      dglDrawBitmapStretchSR(profile->mTextureHandle, RectI(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x, bounds.point.y, mBitmapBounds[BorderRight].extent.x, bounds.extent.y ), mBitmapBounds[BorderRight] );

      // End drawing corners

      // Begin drawing fill

      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = bounds.extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle, destRect, stretchRect);

      // End drawing fill
   }
}

