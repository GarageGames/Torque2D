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

#ifndef _GUIBITMAPCTRL_H_
#define _GUIBITMAPCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif
#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif


/// Renders a bitmap.
class GuiBitmapCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

protected:
   static bool setBitmapName( void *obj, const char *data );
   static const char *getBitmapName( void *obj, const char *data );

   StringTableEntry mBitmapName;
   TextureHandle mTextureHandle;
   Point2I startPoint;
   bool mWrap;

   //Luma:	ability to specify source rect for image UVs
   bool		mUseSourceRect;
   RectI	mSourceRect;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiBitmapCtrl);
   GuiBitmapCtrl();
   static void initPersistFields();

   //Parental methods
   bool onWake();
   void onSleep();
   void inspectPostApply();

   void setBitmap(const char *name,bool resize = false);
   void setBitmap(const TextureHandle &handle,bool resize = false);

   S32 getWidth() const       { return(mTextureHandle.getWidth()); }
   S32 getHeight() const      { return(mTextureHandle.getHeight()); }

   //Luma:	ability to specify source rect for image UVs
   void setSourceRect(U32 x, U32 y, U32 width, U32 height);
   void setUseSourceRect(bool bUse);

   void onRender(Point2I offset, const RectI &updateRect);
   void setValue(S32 x, S32 y);
};

#endif
