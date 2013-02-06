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

#ifndef _GUISLIDERCTRL_H_
#define _GUISLIDERCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

class GuiSliderCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

protected:
   Point2F mRange;
   U32  mTicks;
   F32  mValue;
   RectI   mThumb;
   Point2I mThumbSize;
   void updateThumb( F32 value, bool snap = true, bool onWake = false );
   S32 mShiftPoint;
   S32 mShiftExtent;
   bool mDisplayValue;
   bool mDepressed;
   bool mMouseOver;
   bool mHasTexture;

   enum
   {
	   SliderLineLeft = 0,
	   SliderLineCenter,
	   SliderLineRight,
	   SliderButtonNormal,
	   SliderButtonHighlight,
	   NumBitmaps
   };
   	RectI *mBitmapBounds;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiSliderCtrl);
   GuiSliderCtrl();
   static void initPersistFields();

   //Parental methods
   bool onWake();

   void onMouseDown(const GuiEvent &event);
   void onMouseDragged(const GuiEvent &event);
   void onMouseUp(const GuiEvent &);
   void onMouseLeave(const GuiEvent &);
   void onMouseEnter(const GuiEvent &);
   F32 getValue() { return mValue; }
   void setScriptValue(const char *val);

   void onRender(Point2I offset, const RectI &updateRect);
};

#endif
