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

#ifndef _GUIGRAPHCTRL_H_
#define _GUIGRAPHCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif
#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif


class GuiGraphCtrl : public GuiControl
{
private:
	typedef GuiControl Parent;

public:
   enum Constants {
      MaxPlots = 6,
      MaxDataPoints = 200
   };

   enum GraphType {
      Point,
      Polyline,
      Filled,
      Bar
   };

   struct PlotInfo
   {
	  const char	*mAutoPlot;
	  U32			mAutoPlotDelay;
	  SimTime		mAutoPlotLastDisplay;
	  ColorF		mGraphColor;
	  Vector<F32>	mGraphData;
	  F32			mGraphMax;
	  GraphType		mGraphType;
   };

   PlotInfo		mPlots[MaxPlots];

	//creation methods
   DECLARE_CONOBJECT(GuiGraphCtrl);
   GuiGraphCtrl();

   //Parental methods
   bool onWake();

   void onRender(Point2I offset, const RectI &updateRect);

   // Graph interface
   void addDatum(S32 plotID, F32 v);
   F32  getDatum(S32 plotID, S32 samples);
   void addAutoPlot(S32 plotID, const char *variable, S32 update);
   void removeAutoPlot(S32 plotID);
   void setGraphType(S32 plotID, const char *graphType);
};

#endif
