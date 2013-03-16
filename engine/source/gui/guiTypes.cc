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

#include "platform/platform.h"
#include "platform/types.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/codeBlock.h"
#include "graphics/gFont.h"
#include "graphics/dgl.h"
#include "gui/guiTypes.h"
#include "graphics/gBitmap.h"
#include "graphics/TextureManager.h"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
IMPLEMENT_CONOBJECT(GuiCursor);

GuiCursor::GuiCursor()
{
   mHotSpot.set(0,0);
   mRenderOffset.set(0.0f,0.0f);
   mExtent.set(1,1);
}

GuiCursor::~GuiCursor()
{
}

void GuiCursor::initPersistFields()
{
   Parent::initPersistFields();
   addField("hotSpot",     TypePoint2I,   Offset(mHotSpot, GuiCursor));
   addField("renderOffset",TypePoint2F,   Offset(mRenderOffset, GuiCursor));
   addField("bitmapName",  TypeFilename,  Offset(mBitmapName, GuiCursor));
}

bool GuiCursor::onAdd()
{
   if(!Parent::onAdd())
      return false;

   Sim::getGuiDataGroup()->addObject(this);

   return true;
}

void GuiCursor::onRemove()
{
   Parent::onRemove();
}

void GuiCursor::render(const Point2I &pos)
{
   if (!mTextureHandle && mBitmapName && mBitmapName[0])
   {
      mTextureHandle = TextureHandle(mBitmapName, TextureHandle::BitmapTexture);
      if(!mTextureHandle)
         return;
      mExtent.set(mTextureHandle.getWidth(), mTextureHandle.getHeight());
   }

   // Render the cursor centered according to dimensions of texture
   S32 texWidth = mTextureHandle.getWidth();
   S32 texHeight = mTextureHandle.getWidth();

   Point2I renderPos = pos;
   renderPos.x -= (S32)( texWidth  * mRenderOffset.x );
   renderPos.y -= (S32)( texHeight * mRenderOffset.y );
   
   dglClearBitmapModulation();
   dglDrawBitmap(mTextureHandle, renderPos);
}

//------------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiControlProfile);

static EnumTable::Enums alignEnums[] =
{
   { GuiControlProfile::LeftJustify,          "left"      },
   { GuiControlProfile::CenterJustify,        "center"    },
   { GuiControlProfile::RightJustify,         "right"     }
};
static EnumTable gAlignTable(3, &alignEnums[0]);

static EnumTable::Enums charsetEnums[]=
{
    { TGE_ANSI_CHARSET,         "ANSI" },
    { TGE_SYMBOL_CHARSET,       "SYMBOL" },
    { TGE_SHIFTJIS_CHARSET,     "SHIFTJIS" },
    { TGE_HANGEUL_CHARSET,      "HANGEUL" },
    { TGE_HANGUL_CHARSET,       "HANGUL" },
    { TGE_GB2312_CHARSET,       "GB2312" },
    { TGE_CHINESEBIG5_CHARSET,  "CHINESEBIG5" },
    { TGE_OEM_CHARSET,          "OEM" },
    { TGE_JOHAB_CHARSET,        "JOHAB" },
    { TGE_HEBREW_CHARSET,       "HEBREW" },
    { TGE_ARABIC_CHARSET,       "ARABIC" },
    { TGE_GREEK_CHARSET,        "GREEK" },
    { TGE_TURKISH_CHARSET,      "TURKISH" },
    { TGE_VIETNAMESE_CHARSET,   "VIETNAMESE" },
    { TGE_THAI_CHARSET,         "THAI" },
    { TGE_EASTEUROPE_CHARSET,   "EASTEUROPE" },
    { TGE_RUSSIAN_CHARSET,      "RUSSIAN" },
    { TGE_MAC_CHARSET,          "MAC" },
    { TGE_BALTIC_CHARSET,       "BALTIC" },
};

#define NUMCHARSETENUMS     (sizeof(charsetEnums) / sizeof(EnumTable::Enums))

static EnumTable gCharsetTable(NUMCHARSETENUMS, &charsetEnums[0]);

StringTableEntry GuiControlProfile::sFontCacheDirectory = "";

GuiControlProfile::GuiControlProfile(void) :
   mFontColor(mFontColors[BaseColor]),
   mFontColorHL(mFontColors[ColorHL]),
   mFontColorNA(mFontColors[ColorNA]),
   mFontColorSEL(mFontColors[ColorSEL])
{
    mRefCount = 0;
    mBitmapArrayRects.clear();
    mMouseOverSelected = false;
    
    mTabable       = false;
    mCanKeyFocus   = false;
    
    mOpaque        = false;
    
    mBorder        = 0;
    mBorderThickness = 1;
    
    // default font
    mFontType      = StringTable->EmptyString;
    mFontSize      = 12;
    mFontCharset   = TGE_ANSI_CHARSET;
    mFontColors[BaseColor].set(255,255,255,255);
    
    // default bitmap
    mBitmapName    = NULL;
    mTextOffset.set(0,0);
    
    //used by GuiTextCtrl
    mModal         = false;
    mAlignment     = LeftJustify;
    mAutoSizeWidth = false;
    mAutoSizeHeight= false;
    mReturnTab     = false;
    mNumbersOnly   = false;
    mProfileForChildren = NULL;

   GuiControlProfile *def = dynamic_cast<GuiControlProfile*>(Sim::findObject("GuiDefaultProfile"));
   if (def)
   {
      mTabable       = def->mTabable;
      mCanKeyFocus   = def->mCanKeyFocus;
      mMouseOverSelected = def->mMouseOverSelected;

      mOpaque        = def->mOpaque;
      mFillColor     = def->mFillColor;
      mFillColorHL   = def->mFillColorHL;
      mFillColorNA   = def->mFillColorNA;

      mBorder        = def->mBorder;
      mBorderThickness = def->mBorderThickness;
      mBorderColor   = def->mBorderColor;
      mBorderColorHL = def->mBorderColorHL;
      mBorderColorNA = def->mBorderColorNA;

      mBevelColorHL = def->mBevelColorHL;
      mBevelColorLL = def->mBevelColorLL;

      // default font
      mFontType      = def->mFontType;
      mFontSize      = def->mFontSize;
      mFontCharset   = def->mFontCharset;

      for(U32 i = 0; i < 10; i++)
         mFontColors[i] = def->mFontColors[i];

      // default bitmap
      mBitmapName    = def->mBitmapName;
      mTextOffset    = def->mTextOffset;

      //used by GuiTextCtrl
      mModal         = def->mModal;
      mAlignment     = def->mAlignment;
      mAutoSizeWidth = def->mAutoSizeWidth;
      mAutoSizeHeight= def->mAutoSizeHeight;
      mReturnTab     = def->mReturnTab;
      mNumbersOnly   = def->mNumbersOnly;
      mCursorColor   = def->mCursorColor;
      mProfileForChildren = def->mProfileForChildren;
   }
}

GuiControlProfile::~GuiControlProfile()
{
}


void GuiControlProfile::initPersistFields()
{
   Parent::initPersistFields();

   addField("tab",           TypeBool,       Offset(mTabable, GuiControlProfile));
   addField("canKeyFocus",   TypeBool,       Offset(mCanKeyFocus, GuiControlProfile));
   addField("mouseOverSelected", TypeBool,   Offset(mMouseOverSelected, GuiControlProfile));

   addField("modal",         TypeBool,       Offset(mModal, GuiControlProfile));
   addField("opaque",        TypeBool,       Offset(mOpaque, GuiControlProfile));
   addField("fillColor",     TypeColorI,     Offset(mFillColor, GuiControlProfile));
   addField("fillColorHL",   TypeColorI,     Offset(mFillColorHL, GuiControlProfile));
   addField("fillColorNA",   TypeColorI,     Offset(mFillColorNA, GuiControlProfile));
   addField("border",        TypeS32,        Offset(mBorder, GuiControlProfile));
   addField("borderThickness",TypeS32,       Offset(mBorderThickness, GuiControlProfile));
   addField("borderColor",   TypeColorI,     Offset(mBorderColor, GuiControlProfile));
   addField("borderColorHL", TypeColorI,     Offset(mBorderColorHL, GuiControlProfile));
   addField("borderColorNA", TypeColorI,     Offset(mBorderColorNA, GuiControlProfile));

   addField("bevelColorHL", TypeColorI,     Offset(mBevelColorHL, GuiControlProfile));
   addField("bevelColorLL", TypeColorI,     Offset(mBevelColorLL, GuiControlProfile));

   addField("fontType",      TypeString,     Offset(mFontType, GuiControlProfile));
   addField("fontSize",      TypeS32,        Offset(mFontSize, GuiControlProfile));
   addField("fontCharset",   TypeEnum,       Offset(mFontCharset, GuiControlProfile), 1, &gCharsetTable);
   addField("fontColors",    TypeColorI,     Offset(mFontColors, GuiControlProfile), 10);
   addField("fontColor",     TypeColorI,     Offset(mFontColors[BaseColor], GuiControlProfile));
   addField("fontColorHL",   TypeColorI,     Offset(mFontColors[ColorHL], GuiControlProfile));
   addField("fontColorNA",   TypeColorI,     Offset(mFontColors[ColorNA], GuiControlProfile));
   addField("fontColorSEL",  TypeColorI,     Offset(mFontColors[ColorSEL], GuiControlProfile));
   addField("fontColorLink", TypeColorI,     Offset(mFontColors[ColorUser0], GuiControlProfile));
   addField("fontColorLinkHL", TypeColorI,     Offset(mFontColors[ColorUser1], GuiControlProfile));

   addField("justify",       TypeEnum,       Offset(mAlignment, GuiControlProfile), 1, &gAlignTable);
   addField("textOffset",    TypePoint2I,    Offset(mTextOffset, GuiControlProfile));
   addField("autoSizeWidth", TypeBool,       Offset(mAutoSizeWidth, GuiControlProfile));
   addField("autoSizeHeight",TypeBool,       Offset(mAutoSizeHeight, GuiControlProfile));
   addField("returnTab",     TypeBool,       Offset(mReturnTab, GuiControlProfile));
   addField("numbersOnly",   TypeBool,       Offset(mNumbersOnly, GuiControlProfile));
   addField("cursorColor",   TypeColorI,     Offset(mCursorColor, GuiControlProfile));

   addField("bitmap",        TypeFilename,   Offset(mBitmapName, GuiControlProfile));

   addField("soundButtonDown", TypeAudioAssetPtr,  Offset(mSoundButtonDown, GuiControlProfile));
   addField("soundButtonOver", TypeAudioAssetPtr,  Offset(mSoundButtonOver, GuiControlProfile));
   addField("profileForChildren", TypeSimObjectPtr,  Offset(mProfileForChildren, GuiControlProfile));
}

bool GuiControlProfile::onAdd()
{
   if(!Parent::onAdd())
      return false;

   Sim::getGuiDataGroup()->addObject(this);

   return true;
}

S32 GuiControlProfile::constructBitmapArray()
{
   if(mBitmapArrayRects.size())
      return mBitmapArrayRects.size();

   GBitmap *bmp = mTextureHandle.getBitmap();

   // Make sure the texture exists.
   if( !bmp )
      return 0;
  
   //get the separator color
   ColorI sepColor;
   if ( !bmp || !bmp->getColor( 0, 0, sepColor ) )
    {
      Con::errorf("Failed to create bitmap array from %s for profile %s - couldn't ascertain seperator color!", mBitmapName, getName());
      AssertFatal( false, avar("Failed to create bitmap array from %s for profile %s - couldn't ascertain seperator color!", mBitmapName, getName()));
      return 0;
    }

   //now loop through all the scroll pieces, and find the bounding rectangle for each piece in each state
   S32 curY = 0;

   // ascertain the height of this row...
   ColorI color;
   mBitmapArrayRects.clear();
   while(curY < (S32)bmp->getHeight())
   {
      // skip any sep colors
      bmp->getColor( 0, curY, color);
      if(color == sepColor)
      {
         curY++;
         continue;
      }
      // ok, process left to right, grabbing bitmaps as we go...
      S32 curX = 0;
      while(curX < (S32)bmp->getWidth())
      {
         bmp->getColor(curX, curY, color);
         if(color == sepColor)
         {
            curX++;
            continue;
         }
         S32 startX = curX;
         while(curX < (S32)bmp->getWidth())
         {
            bmp->getColor(curX, curY, color);
            if(color == sepColor)
               break;
            curX++;
         }
         S32 stepY = curY;
         while(stepY < (S32)bmp->getHeight())
         {
            bmp->getColor(startX, stepY, color);
            if(color == sepColor)
               break;
            stepY++;
         }
         mBitmapArrayRects.push_back(RectI(startX, curY, curX - startX, stepY - curY));
      }
      // ok, now skip to the next separation color on column 0
      while(curY < (S32)bmp->getHeight())
      {
         bmp->getColor(0, curY, color);
         if(color == sepColor)
            break;
         curY++;
      }
   }
   return mBitmapArrayRects.size();
}

void GuiControlProfile::incRefCount()
{
   if(!mRefCount++)
   {
      sFontCacheDirectory = Con::getVariable("$GUI::fontCacheDirectory");

      //verify the font
      mFont = GFont::create(mFontType, mFontSize, sFontCacheDirectory);
      if (mFont.isNull())
         Con::errorf("Failed to load/create profile font (%s/%d)", mFontType, mFontSize);
       
      if ( mBitmapName != NULL && mBitmapName != StringTable->EmptyString )
      {
          mTextureHandle = TextureHandle(mBitmapName, TextureHandle::BitmapKeepTexture);
          if (!(bool)mTextureHandle)
             Con::errorf("Failed to load profile bitmap (%s)",mBitmapName);

          // If we've got a special border, make sure it's usable.
          if( mBorder == -1 || mBorder == -2 )
             constructBitmapArray();
      }
   }
}

void GuiControlProfile::decRefCount()
{
   AssertFatal(mRefCount, "GuiControlProfile::decRefCount: zero ref count");
   if(!mRefCount)
      return;

   if(!--mRefCount)
   {
      mFont = NULL;
      mTextureHandle = NULL;
   }
}

ConsoleType( GuiProfile, TypeGuiProfile, sizeof(GuiControlProfile*), "" )

ConsoleSetType( TypeGuiProfile )
{
   GuiControlProfile *profile = NULL;
   if(argc == 1)
      Sim::findObject(argv[0], profile);

   AssertWarn(profile != NULL, avar("GuiControlProfile: requested gui profile (%s) does not exist.", argv[0]));
   if(!profile)
      profile = dynamic_cast<GuiControlProfile*>(Sim::findObject("GuiDefaultProfile"));

   AssertFatal(profile != NULL, avar("GuiControlProfile: unable to find specified profile (%s) and GuiDefaultProfile does not exist!", argv[0]));

   GuiControlProfile **obj = (GuiControlProfile **)dptr;
   if((*obj) == profile)
      return;

   if(*obj)
      (*obj)->decRefCount();

   *obj = profile;
   (*obj)->incRefCount();
}

ConsoleGetType( TypeGuiProfile )
{
   static char returnBuffer[256];

   GuiControlProfile **obj = (GuiControlProfile**)dptr;
   dSprintf(returnBuffer, sizeof(returnBuffer), "%s", *obj ? (*obj)->getName() ? (*obj)->getName() : (*obj)->getIdString() : "");
   return returnBuffer;
}
