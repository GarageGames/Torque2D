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

#include "gui/guiBitmapCtrl.h"

class GuiFadeinBitmapCtrl : public GuiBitmapCtrl
{
   typedef GuiBitmapCtrl Parent;
public:
   U32 wakeTime;
   bool done;
   U32 fadeinTime;
   U32 waitTime;
   U32 fadeoutTime;

   GuiFadeinBitmapCtrl()
   {
      wakeTime    = 0;
      fadeinTime  = 1000;
      waitTime    = 2000;
      fadeoutTime = 1000;
      done        = false;
   }
   void onPreRender()
   {
      Parent::onPreRender();
      setUpdate();
   }
   void onMouseDown(const GuiEvent &)
   {
      Con::executef(this, 1, "click");
   }
   bool onKeyDown(const GuiEvent &)
   {
      Con::executef(this, 1, "click");
      return true;
   }
   DECLARE_CONOBJECT(GuiFadeinBitmapCtrl);
   bool onWake()
   {
      if(!Parent::onWake())
         return false;
      wakeTime = Platform::getRealMilliseconds();
      done = false;
      return true;
   }
   void onRender(Point2I offset, const RectI &updateRect)
   {
      Parent::onRender(offset, updateRect);
      U32 elapsed = Platform::getRealMilliseconds() - wakeTime;

      U32 alpha;
      if (elapsed < fadeinTime)
      {
         // fade-in
         alpha = (U32)(255 - (255 * (F32(elapsed) / F32(fadeinTime))));
      }
      else if (elapsed < (fadeinTime+waitTime))
      {
         // wait
         alpha = 0;
      }
      else if (elapsed < (fadeinTime+waitTime+fadeoutTime))
      {
         // fade out
         elapsed -= (fadeinTime+waitTime);
         alpha = (U32)(255 * F32(elapsed) / F32(fadeoutTime));
      }
      else
      {
         // done state
         alpha = fadeoutTime ? 255 : 0;
         done = true;
      }
      ColorI color(0,0,0,alpha);
      dglDrawRectFill(offset, mBounds.extent + offset, color);
   }
   static void initPersistFields()
   {
      Parent::initPersistFields();
      addField("fadeinTime", TypeS32, Offset(fadeinTime, GuiFadeinBitmapCtrl));
      addField("waitTime", TypeS32, Offset(waitTime, GuiFadeinBitmapCtrl));
      addField("fadeoutTime", TypeS32, Offset(fadeoutTime, GuiFadeinBitmapCtrl));
      addField("done", TypeBool, Offset(done, GuiFadeinBitmapCtrl));
   }
};

IMPLEMENT_CONOBJECT(GuiFadeinBitmapCtrl);

























