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
#ifndef _GUI_INSPECTOR_H_
#define _GUI_INSPECTOR_H_

#ifndef _CONSOLE_BASE_TYPE_H_
#include "console/consoleBaseType.h"
#endif

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _GUICANVAS_H_
#include "gui/guiCanvas.h"
#endif

#ifndef _GUISTACKCTRL_H_
#include "gui/containers/guiStackCtrl.h"
#endif

#ifndef _H_GUIDEFAULTCONTROLRENDER_
#include "gui/guiDefaultControlRender.h"
#endif

#ifndef _GUITICKCTRL_H_
#include "gui/guiTickCtrl.h"
#endif

#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif

#ifndef _GUITEXTEDITCTRL_H_
#include "gui/guiTextEditCtrl.h"
#endif

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif

#ifndef _GUIPOPUPCTRL_H_
#include "gui/guiPopUpCtrl.h"
#endif

#include "gui/containers/guiRolloutCtrl.h"


// Forward Declare GuiInspectorGroup
class GuiInspectorGroup;
// Forward Declare GuiInspectorField
class GuiInspectorField;
// Forward Declare GuiInspectorDatablockField
class GuiInspectorDatablockField;

class GuiInspector : public GuiStackControl
{
private:
   typedef GuiStackControl Parent;
public:
   // Members
   Vector<GuiInspectorGroup*>    mGroups;
   SimObjectPtr<SimObject>       mTarget;

   GuiInspector();
   ~GuiInspector();
   DECLARE_CONOBJECT(GuiInspector);

   virtual void parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent);
   void inspectObject( SimObject *object );
   inline SimObject *getInspectObject() { return mTarget.isNull() ? NULL : mTarget; };
   void setName( const char* newName );
   void clearGroups();
   bool onAdd();
   bool findExistentGroup( StringTableEntry groupName );
};

class GuiInspectorField : public GuiControl
{
private:
   typedef GuiControl Parent;
public:
   // Static Caption Width (in percentage) for all inspector fields
   static S32                 smCaptionWidth;

   // Members
   StringTableEntry           mCaption;
   GuiInspectorGroup*         mParent;
   SimObjectPtr<SimObject>    mTarget;
   AbstractClassRep::Field*   mField;
   StringTableEntry           mFieldArrayIndex;

   // Constructed Field Edit Control
   GuiControl*                mEdit;

   GuiInspectorField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, AbstractClassRep::Field* field );
   GuiInspectorField();
   ~GuiInspectorField();
   DECLARE_CONOBJECT(GuiInspectorField);

   virtual void setTarget( SimObjectPtr<SimObject> target ) { mTarget = target; };
   virtual void setParent( GuiInspectorGroup* parent ) { mParent = parent; };
   virtual void setInspectorField( AbstractClassRep::Field *field, const char*arrayIndex = NULL );

protected:
   void registerEditControl( GuiControl *ctrl );
public:
   virtual GuiControl* constructEditControl();
   virtual void        updateValue( const char* newValue );
   virtual StringTableEntry getFieldName();
   virtual void              setData( const char* data );
   virtual const char*  getData();

   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);
   virtual bool onAdd();
   virtual void onRender(Point2I offset, const RectI &updateRect);
};

class GuiInspectorGroup : public GuiRolloutCtrl
{
private:
   typedef GuiRolloutCtrl Parent;
public:
   // Members
   SimObjectPtr<SimObject>             mTarget;
   SimObjectPtr<GuiInspector>          mParent;
   Vector<GuiInspectorField*>          mChildren;
   GuiStackControl*                    mStack;

   // Constructor/Destructor/Conobject Declaration
   GuiInspectorGroup();
   GuiInspectorGroup( SimObjectPtr<SimObject> target, StringTableEntry groupName, SimObjectPtr<GuiInspector> parent );
   ~GuiInspectorGroup();
   DECLARE_CONOBJECT(GuiInspectorGroup);

   virtual GuiInspectorField* constructField( S32 fieldType );
   virtual GuiInspectorField* findField( StringTableEntry fieldName );

   // Publicly Accessible Information about this group
   StringTableEntry getGroupName() { return mCaption; };
   SimObjectPtr<SimObject> getGroupTarget() { return mTarget; };
   SimObjectPtr<GuiInspector> getContentCtrl() { return mParent; };

   bool onAdd();
   virtual bool inspectGroup();

   virtual void animateToContents();
protected:
   // overridable method that creates our inner controls.
   virtual bool createContent();


};

class GuiInspectorDynamicField : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;
   SimObjectPtr<GuiControl>     mRenameCtrl;
public:
   SimFieldDictionary::Entry*   mDynField;

   GuiInspectorDynamicField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, SimFieldDictionary::Entry* field );
   GuiInspectorDynamicField() {};
   ~GuiInspectorDynamicField() {};
   DECLARE_CONOBJECT(GuiInspectorDynamicField);

   virtual void setData( const char* data );
   virtual const char* getData();

   virtual StringTableEntry getFieldName() { return ( mDynField != NULL ) ? mDynField->slotName : StringTable->EmptyString; };

   // Override onAdd so we can construct our custom field name edit control
   virtual bool onAdd();
   // Rename a dynamic field
   void renameField( StringTableEntry newFieldName );
   // Create an edit control to overlay the field name (for renaming dynamic fields)
   GuiControl* constructRenameControl();
   // Override parentResized so we can resize our renaming control
   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);
};

class GuiInspectorDynamicGroup : public GuiInspectorGroup
{
private:
   typedef GuiInspectorGroup Parent;
   GuiControl* mAddCtrl;


public:
   DECLARE_CONOBJECT(GuiInspectorDynamicGroup);
   GuiInspectorDynamicGroup() { /*mNeedScroll=false;*/ };
   GuiInspectorDynamicGroup( SimObjectPtr<SimObject> target, StringTableEntry groupName, SimObjectPtr<GuiInspector> parent ) : GuiInspectorGroup( target, groupName, parent) { /*mNeedScroll=false;*/};
   
   //////////////////////////////////////////////////////////////////////////
   // inspectGroup is overridden in GuiInspectorDynamicGroup to inspect an 
   // objects FieldDictionary (dynamic fields) instead of regular persistent
   // fields.
   bool inspectGroup();

   // For scriptable dynamic field additions
   void addDynamicField();

   // Clear our fields (delete them)
   void clearFields();

   // Find an already existent field by name in the dictionary
   virtual SimFieldDictionary::Entry* findDynamicFieldInDictionary( StringTableEntry fieldName );
protected:
   // create our inner controls when we add
   virtual bool createContent();

};


//////////////////////////////////////////////////////////////////////////
// GuiInspectorDatablockField - custom field type for datablock enumeration
//////////////////////////////////////////////////////////////////////////
class GuiInspectorDatablockField : public GuiInspectorField
{
private:
   typedef GuiInspectorField Parent;

   AbstractClassRep *mDesiredClass;
public:
   DECLARE_CONOBJECT(GuiInspectorDatablockField);
   GuiInspectorDatablockField( StringTableEntry className );
   GuiInspectorDatablockField() { mDesiredClass = NULL; };

   void setClassName( StringTableEntry className );

   //////////////////////////////////////////////////////////////////////////
   // Override able methods for custom edit fields (Both are REQUIRED)
   //////////////////////////////////////////////////////////////////////////
   virtual GuiControl* constructEditControl();

};

#endif