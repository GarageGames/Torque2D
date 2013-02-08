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

#ifndef _GUIFILTERCTRL_H_
#define _GUIFILTERCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif



//--------------------------------------
// helper class
class Filter: public Vector<F32>
{
public:
   Filter() : Vector<F32>(__FILE__, __LINE__) { }

   void set(S32 argc, const char *argv[]);
   F32  getValue(F32 t) const;
};


//--------------------------------------
class GuiFilterCtrl : public GuiControl
{
  private:
   typedef GuiControl Parent;

   S32 mControlPointRequest;
   S32 mCurKnot;
   Filter mFilter;

  public:
   //creation methods
   DECLARE_CONOBJECT(GuiFilterCtrl);
   GuiFilterCtrl();
   static void initPersistFields();

   //Parental methods
   bool onWake();

   void onMouseDown(const GuiEvent &event);
   void onMouseDragged(const GuiEvent &event);
   void onMouseUp(const GuiEvent &);

   F32  getValue(S32 n);
   const Filter* get() { return &mFilter; }
   void set(const Filter &f);
   S32  getNumControlPoints() {return mFilter.size(); }
   void identity();

   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect );
};


inline F32 GuiFilterCtrl::getValue(S32 n)
{
   getMin(getMax(n,0), (S32)mFilter.size()-1);
   return mFilter[n];
}


inline void GuiFilterCtrl::set(const Filter &f)
{
   mControlPointRequest = f.size();
   mFilter = f;
}

#endif
