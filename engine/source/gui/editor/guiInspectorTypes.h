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
#ifndef _GUI_INSPECTOR_TYPES_H_
#define _GUI_INSPECTOR_TYPES_H_

#ifndef _GUI_INSPECTOR_H_
#include "gui/editor/guiInspector.h"
#endif

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _H_GUIDEFAULTCONTROLRENDER_
#include "gui/guiDefaultControlRender.h"
#endif

#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif

#ifndef _GUITEXTEDITCTRL_H_
#include "gui/guiTextEditCtrl.h"
#endif

#ifndef _GUIPOPUPCTRL_H_
#include "gui/guiPopUpCtrl.h"
#endif

#ifndef _GUICHECKBOXCTRL_H_
#include "gui/buttons/guiCheckBoxCtrl.h"
#endif


//////////////////////////////////////////////////////////////////////////
// TypeEnum GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeEnum : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeEnum);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl();
   virtual void               setData( StringTableEntry data );
   virtual StringTableEntry   getData();
   virtual void               updateValue( StringTableEntry newValue );
};


//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeCheckBox Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeCheckBox : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeCheckBox);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields (Both are REQUIRED)
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl* constructEditControl();
};

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeGuiProfile Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeGuiProfile : public GuiInspectorTypeEnum
{
private:
   typedef GuiInspectorTypeEnum Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeGuiProfile);
   static void consoleInit();

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields (Both are REQUIRED)
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl* constructEditControl();
};


//////////////////////////////////////////////////////////////////////////
// TypeFileName GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeFileName : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeFileName);
   static void consoleInit();

   SimObjectPtr<GuiButtonCtrl>   mBrowseButton;

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl();
   virtual void               resize(const Point2I &newPosition, const Point2I &newExtent);
};


//////////////////////////////////////////////////////////////////////////
// TypeColor GuiInspectorField Class (Base for ColorI/ColorF)
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeColor : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
public:
   DECLARE_CONOBJECT(GuiInspectorTypeColor);

   StringTableEntry  mColorFunction;
   SimObjectPtr<GuiButtonCtrl>   mBrowseButton;

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl*        constructEditControl();
   virtual void               resize(const Point2I &newPosition, const Point2I &newExtent);
};

//////////////////////////////////////////////////////////////////////////
// TypeColorI GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeColorI : public GuiInspectorTypeColor
{
private:
   typedef GuiInspectorTypeColor Parent;
public:
   GuiInspectorTypeColorI();

   DECLARE_CONOBJECT(GuiInspectorTypeColorI);
   static void consoleInit();
};

//////////////////////////////////////////////////////////////////////////
// TypeColorF GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
class GuiInspectorTypeColorF : public GuiInspectorTypeColor
{
private:
   typedef GuiInspectorTypeColor Parent;
public:
   GuiInspectorTypeColorF();

   DECLARE_CONOBJECT(GuiInspectorTypeColorF);
   static void consoleInit();
};

#endif