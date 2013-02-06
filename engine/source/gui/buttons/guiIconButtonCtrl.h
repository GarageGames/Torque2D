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

#ifndef _GUIICONBUTTON_H_
#define _GUIICONBUTTON_H_

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif
#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

///-------------------------------------
/// Icon Button Control
/// Draws the bitmap within a normal button control.
///
/// if the extent is set to (0,0) in the gui editor and apply hit, this control will
/// set it's extent to be exactly the size of the normal bitmap (if present)
///
class GuiIconButtonCtrl : public GuiButtonCtrl
{
private:
   typedef GuiButtonCtrl Parent;

protected:
   StringTableEntry  mBitmapName;
   TextureHandle     mTextureNormal;
   S32               mIconLocation;
   S32               mTextLocation;
   S32               mTextMargin;
   Point2I           mButtonMargin;
   bool              mFitBitmapToButton; // Make the bitmap fill the button extent

   // DAW: Optional bitmap to be displayed when the proper bitmap cannot be found
   StringTableEntry mErrorBitmapName;
   TextureHandle mErrorTextureHandle;

   void renderButton( Point2I &offset, const RectI& updateRect);

   enum 
   {
      stateNormal,
      stateMouseOver,
      statePressed,
      stateDisabled,
   };

   void renderBitmapArray(RectI &bounds, S32 state);

public:   
   enum {
      TextLocNone,
      TextLocBottom,
      TextLocRight,
      TextLocTop,
      TextLocLeft,
      TextLocCenter,
   };

   enum {
      IconLocLeft,
      IconLocRight,
      IconLocNone,
   };


   DECLARE_CONOBJECT(GuiIconButtonCtrl);
   GuiIconButtonCtrl();

   static void initPersistFields();

   //Parent methods
   bool onWake();
   void onSleep();
   void inspectPostApply();

   void setBitmap(const char *name);

   // DAW: Used to set the optional error bitmap
   void setErrorBitmap(const char *name);

   void onRender(Point2I offset, const RectI &updateRect);
};

#endif //_GUIICONBUTTON_H_
