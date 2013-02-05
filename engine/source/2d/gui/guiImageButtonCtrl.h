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

#ifndef _GUIIMAGEBUTTON_H_
#define _GUIIMAGEBUTTON_H_

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif
#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

//-----------------------------------------------------------------------------

class GuiImageButtonCtrl : public GuiButtonCtrl
{
private:
   typedef GuiButtonCtrl Parent;

protected:
    StringTableEntry                mNormalAssetId;
    StringTableEntry                mHoverAssetId;
    StringTableEntry                mDownAssetId;
    StringTableEntry                mInactiveAssetId;

    AssetPtr<ImageAsset>  mImageNormalAsset;
    AssetPtr<ImageAsset>  mImageHoverAsset;
    AssetPtr<ImageAsset>  mImageDownAsset;
    AssetPtr<ImageAsset>  mImageInactiveAsset;

    void renderButton( ImageAsset* pImageAsset, const U32 frame, Point2I &offset, const RectI& updateRect);

protected:
    enum ButtonState
    {
        NORMAL,
        HOVER,
        DOWN,
        INACTIVE
    };

public:
   GuiImageButtonCtrl();
   bool onWake();
   void onSleep();
   void onRender(Point2I offset, const RectI &updateRect);

   static void initPersistFields();

   void setNormalImage( const char* pImageAssetId );
   inline StringTableEntry getNormalImage( void ) const { return mNormalAssetId; }
   void setHoverImage( const char* pImageAssetId );
   inline StringTableEntry getHoverImage( void ) const { return mHoverAssetId; }
   void setDownImage( const char* pImageAssetId );
   inline StringTableEntry getDownImage( void ) const { return mDownAssetId; }
   void setInactiveImage( const char* pImageAssetId );
   inline StringTableEntry getInactiveImage( void ) const { return mInactiveAssetId; }

   // Declare type.
   DECLARE_CONOBJECT(GuiImageButtonCtrl);

protected:
    static bool setNormalImage(void* obj, const char* data) { static_cast<GuiImageButtonCtrl*>(obj)->setNormalImage( data ); return false; }
    static const char* getNormalImage(void* obj, const char* data) { return static_cast<GuiImageButtonCtrl*>(obj)->getNormalImage(); }
    static bool setHoverImage(void* obj, const char* data) { static_cast<GuiImageButtonCtrl*>(obj)->setHoverImage( data ); return false; }
    static const char* getHoverImage(void* obj, const char* data) { return static_cast<GuiImageButtonCtrl*>(obj)->getHoverImage(); }
    static bool setDownImage(void* obj, const char* data) { static_cast<GuiImageButtonCtrl*>(obj)->setDownImage( data ); return false; }
    static const char* getDownImage(void* obj, const char* data) { return static_cast<GuiImageButtonCtrl*>(obj)->getDownImage(); }
    static bool setInactiveImage(void* obj, const char* data) { static_cast<GuiImageButtonCtrl*>(obj)->setInactiveImage( data ); return false; }
    static const char* getInactiveImage(void* obj, const char* data) { return static_cast<GuiImageButtonCtrl*>(obj)->getInactiveImage(); }
};

#endif //_GUIIMAGEBUTTON_H_
