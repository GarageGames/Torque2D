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
#include "gui/guiCanvas.h"
#include "gui/buttons/guiButtonBaseCtrl.h"


class GuiBorderButtonCtrl : public GuiButtonBaseCtrl
{
   typedef GuiButtonBaseCtrl Parent;

protected:
public:
   DECLARE_CONOBJECT(GuiBorderButtonCtrl);

   void onRender(Point2I offset, const RectI &updateRect);
};

IMPLEMENT_CONOBJECT(GuiBorderButtonCtrl);

void GuiBorderButtonCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   RectI bounds(offset, mBounds.extent);
   if(mActive && mMouseOver)
   {
      bounds.inset(2,2);
      dglDrawRect(bounds, mProfile->mFontColorHL);
      bounds.inset(-2,-2);
   }
   if(mActive && (mStateOn || mDepressed))
   {
      dglDrawRect(bounds, mProfile->mFontColorHL);
      bounds.inset(1,1);
      dglDrawRect(bounds, mProfile->mFontColorHL);
   }
   renderChildControls(offset, updateRect);
}

