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
#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#include "gui/guiCanvas.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/buttons/guiToolboxButtonCtrl.h"

IMPLEMENT_CONOBJECT(GuiToolboxButtonCtrl);

//-------------------------------------
GuiToolboxButtonCtrl::GuiToolboxButtonCtrl()
{
   mNormalBitmapName = StringTable->EmptyString;
   mLoweredBitmapName = StringTable->insert("sceneeditor/client/images/buttondown");
   mHoverBitmapName = StringTable->insert("sceneeditor/client/images/buttonup");
   mMinExtent.set( 16, 16 );
   mBounds.extent.set(48, 48);
   mButtonType = ButtonTypeRadio;
   mTipHoverTime = 100;
   
}


//-------------------------------------
void GuiToolboxButtonCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("normalBitmap", TypeFilename, Offset(mNormalBitmapName, GuiToolboxButtonCtrl));
   addField("loweredBitmap", TypeFilename, Offset(mLoweredBitmapName, GuiToolboxButtonCtrl));
   addField("hoverBitmap", TypeFilename, Offset(mHoverBitmapName, GuiToolboxButtonCtrl));
}


//-------------------------------------
bool GuiToolboxButtonCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   setActive( true );
   
   setNormalBitmap( mNormalBitmapName );
   setLoweredBitmap( mLoweredBitmapName );
   setHoverBitmap( mHoverBitmapName );

   return true;
}


//-------------------------------------
void GuiToolboxButtonCtrl::onSleep()
{
   mTextureNormal = NULL;
   mTextureLowered = NULL;
   mTextureHover = NULL;
   Parent::onSleep();
}


//-------------------------------------

ConsoleMethod( GuiToolboxButtonCtrl, setNormalBitmap, void, 3, 3, "( filepath name ) Sets the bitmap that shows when the button is active"
                          "@param name The path of the desired bitmap file\n"
              "@return No Return Value.")
{
   object->setNormalBitmap(argv[2]);
}

ConsoleMethod( GuiToolboxButtonCtrl, setLoweredBitmap, void, 3, 3, "( filepath name ) Sets the bitmap that shows when the button is disabled"
                          "@param name The path of the desired bitmap file\n"
              "@return No Return Value.")
{
   object->setLoweredBitmap(argv[2]);
}

ConsoleMethod( GuiToolboxButtonCtrl, setHoverBitmap, void, 3, 3, "( filepath name ) Sets the bitmap that shows when the button is disabled"
              "@param name The path of the desired bitmap file\n"
              "@return No Return Value.")
{
   object->setHoverBitmap(argv[2]);
}

//-------------------------------------
void GuiToolboxButtonCtrl::inspectPostApply()
{
   // if the extent is set to (0,0) in the gui editor and appy hit, this control will
   // set it's extent to be exactly the size of the normal bitmap (if present)
   Parent::inspectPostApply();

   if ((mBounds.extent.x == 0) && (mBounds.extent.y == 0) && mTextureNormal)
   {
      TextureObject *texture = (TextureObject *) mTextureNormal;
      mBounds.extent.x = texture->getBitmapWidth();
      mBounds.extent.y = texture->getBitmapHeight();
   }
}


//-------------------------------------
void GuiToolboxButtonCtrl::setNormalBitmap( StringTableEntry bitmapName )
{
   mNormalBitmapName = StringTable->insert( bitmapName );
   
   if(!isAwake())
      return;

   if ( *mNormalBitmapName )
      mTextureNormal = TextureHandle( mNormalBitmapName, TextureHandle::BitmapTexture, true );
   else
      mTextureNormal = NULL;
   
   setUpdate();
}   

void GuiToolboxButtonCtrl::setLoweredBitmap( StringTableEntry bitmapName )
{
   mLoweredBitmapName = StringTable->insert( bitmapName );
   
   if(!isAwake())
      return;

   if ( *mLoweredBitmapName )
      mTextureLowered = TextureHandle( mLoweredBitmapName, TextureHandle::BitmapTexture, true );
   else
      mTextureLowered = NULL;
   
   setUpdate();
}   

void GuiToolboxButtonCtrl::setHoverBitmap( StringTableEntry bitmapName )
{
   mHoverBitmapName = StringTable->insert( bitmapName );

   if(!isAwake())
      return;

   if ( *mHoverBitmapName )
      mTextureHover = TextureHandle( mHoverBitmapName, TextureHandle::BitmapTexture, true );
   else
      mTextureHover = NULL;

   setUpdate();
}   



//-------------------------------------
void GuiToolboxButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
   // Only render the state rect (hover/down) if we're active
   if (mActive)
   {
      RectI r(offset, mBounds.extent);
      if ( mDepressed  || mStateOn )
         renderStateRect( mTextureLowered , r );
      else if (mMouseOver) 
         renderStateRect( mTextureHover , r );    
   }

   // Now render the image
   if( mTextureNormal )
   {
      renderButton( mTextureNormal, offset, updateRect );
      return;
   }

   Point2I textPos = offset;
   if( mDepressed )
      textPos += Point2I(1,1);

   // Make sure we take the profile's textOffset into account.
   textPos += mProfile->mTextOffset;

   dglSetBitmapModulation( mProfile->mFontColor );
   renderJustifiedText(textPos, mBounds.extent, mButtonText);

}

void GuiToolboxButtonCtrl::renderStateRect( TextureHandle &texture, const RectI& rect )
{
   if (texture)
   {
      dglClearBitmapModulation();
      dglDrawBitmapStretch( texture, rect );
   }
}

//------------------------------------------------------------------------------

void GuiToolboxButtonCtrl::renderButton(TextureHandle &texture, Point2I &offset, const RectI& updateRect)
{
   if (texture)
   {
      Point2I finalOffset = offset;

      finalOffset.x += ( ( mBounds.extent.x / 2 ) - ( texture.getWidth() / 2 ) );
      finalOffset.y += ( ( mBounds.extent.y / 2 ) - ( texture.getHeight() / 2 ) );

      dglClearBitmapModulation();
      dglDrawBitmap(texture, finalOffset);
      renderChildControls( offset, updateRect);
   }
}