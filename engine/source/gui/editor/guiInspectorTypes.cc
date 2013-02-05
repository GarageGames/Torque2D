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
#include "gui/editor/guiInspectorTypes.h"

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeEnum 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeEnum);

GuiControl* GuiInspectorTypeEnum::constructEditControl()
{
   GuiControl* retCtrl = new GuiPopUpMenuCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiPopUpMenuCtrl *menu = dynamic_cast<GuiPopUpMenuCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "InspectorTypeEnumProfile" );

   menu->setField("text", getData());

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.%s = %d.getText();%d.inspect(%d);",mTarget->getId(), mField->pFieldname, menu->getId(), mParent->mParent->getId(), mTarget->getId() );
   menu->setField("Command", szBuffer );

   //now add the entries
   for(S32 i = 0; i < mField->table->size; i++)
      menu->addEntry(mField->table->table[i].label, mField->table->table[i].index);

   return retCtrl;
}

void GuiInspectorTypeEnum::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeEnum)->setInspectorFieldType("GuiInspectorTypeEnum");
}

void GuiInspectorTypeEnum::updateValue( StringTableEntry newValue )
{
   GuiPopUpMenuCtrl *ctrl = dynamic_cast<GuiPopUpMenuCtrl*>( mEdit );
   if( ctrl != NULL )
      ctrl->setText( newValue );
}

void GuiInspectorTypeEnum::setData( StringTableEntry data )
{
   if( mField == NULL || mTarget == NULL )
      return;

   mTarget->setDataField( mField->pFieldname, NULL, data );

   // Force our edit to update
   updateValue( data );
}

StringTableEntry  GuiInspectorTypeEnum::getData()
{
   if( mField == NULL || mTarget == NULL )
      return "";

   return mTarget->getDataField( mField->pFieldname, NULL );
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeCheckBox 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeCheckBox);

GuiControl* GuiInspectorTypeCheckBox::constructEditControl()
{
   GuiControl* retCtrl = new GuiCheckBoxCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiCheckBoxCtrl *check = dynamic_cast<GuiCheckBoxCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "InspectorTypeCheckboxProfile" );
   retCtrl->setField( "text", "" );

   check->mIndent = 4;

   retCtrl->setScriptValue( getData() );

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getValue());",getId(),check->getId() );
   check->setField("Command", szBuffer );

   return retCtrl;
}


void GuiInspectorTypeCheckBox::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeBool)->setInspectorFieldType("GuiInspectorTypeCheckBox");
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeGuiProfile 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeGuiProfile);

void GuiInspectorTypeGuiProfile::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeGuiProfile)->setInspectorFieldType("GuiInspectorTypeGuiProfile");
}

static S32 QSORT_CALLBACK stringCompare(const void *a,const void *b)
{
   StringTableEntry sa = *(StringTableEntry*)a;
   StringTableEntry sb = *(StringTableEntry*)b;
   return(dStricmp(sa, sb));
}

GuiControl* GuiInspectorTypeGuiProfile::constructEditControl()
{
   GuiControl* retCtrl = new GuiPopUpMenuCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiPopUpMenuCtrl *menu = dynamic_cast<GuiPopUpMenuCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "InspectorTypeEnumProfile" );

   menu->setField("text", getData());

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(),menu->getId() );
   menu->setField("Command", szBuffer );

   Vector<StringTableEntry> entries;

   SimGroup * grp = Sim::getGuiDataGroup();
   for(SimGroup::iterator i = grp->begin(); i != grp->end(); i++)
   {
      GuiControlProfile * profile = dynamic_cast<GuiControlProfile *>(*i);
      if(profile)
      {
         entries.push_back(profile->getName());
      }
   }

   // sort the entries
   dQsort(entries.address(), entries.size(), sizeof(StringTableEntry), stringCompare);
   for(U32 j = 0; j < (U32)entries.size(); j++)
      menu->addEntry(entries[j], 0);

   return retCtrl;
}


//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeFileName 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeFileName);

void GuiInspectorTypeFileName::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeFilename)->setInspectorFieldType("GuiInspectorTypeFileName");
}

GuiControl* GuiInspectorTypeFileName::constructEditControl()
{
   GuiControl* retCtrl = new GuiTextEditCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiInspectorTextEditProfile" );

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(),retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );

   mBrowseButton = new GuiButtonCtrl();

   if( mBrowseButton != NULL )
   {
      RectI browseRect( Point2I( ( mBounds.point.x + mBounds.extent.x) - 26, mBounds.point.y + 2), Point2I(20, mBounds.extent.y - 4) );
      char szBuffer[512];
      dSprintf( szBuffer, 512, "getLoadFilename(\"*.*\", \"%d.apply\", \"%s\");",getId(), getData());
      mBrowseButton->setField( "Command", szBuffer );
      mBrowseButton->setField( "text", "..." );
      mBrowseButton->setField( "Profile", "GuiInspectorTypeFileNameProfile" );
      mBrowseButton->registerObject();
      addObject( mBrowseButton );

      // Position
      mBrowseButton->resize( browseRect.point, browseRect.extent );
   }

   return retCtrl;
}

void GuiInspectorTypeFileName::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   Parent::resize( newPosition, newExtent );

   if( mEdit != NULL )
   {
      // Calculate Caption Rect
      RectI captionRect( mBounds.point , Point2I( (S32)mFloor( mBounds.extent.x * (F32)( (F32)GuiInspectorField::smCaptionWidth / 100.0f ) ) - 2, (S32)mBounds.extent.y ) );

      // Calculate Edit Field Rect
      RectI editFieldRect( Point2I( captionRect.extent.x + 1, 0 ) , Point2I( mBounds.extent.x - ( captionRect.extent.x + 25 ) , mBounds.extent.y ) );

      mEdit->resize( editFieldRect.point, editFieldRect.extent );

      if( mBrowseButton != NULL )
      {
         RectI browseRect( Point2I( ( mBounds.point.x + mBounds.extent.x) - 26, 2), Point2I(20, mBounds.extent.y - 4) );
         mBrowseButton->resize( browseRect.point, browseRect.extent );
      }
   }
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeColor (Base for ColorI/ColorF) 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeColor);

GuiControl* GuiInspectorTypeColor::constructEditControl()
{
   GuiControl* retCtrl = new GuiTextEditCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiInspectorTextEditProfile" );

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(), retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );

   mBrowseButton = new GuiButtonCtrl();

   if( mBrowseButton != NULL )
   {
      RectI browseRect( Point2I( ( mBounds.point.x + mBounds.extent.x) - 26, mBounds.point.y + 2), Point2I(20, mBounds.extent.y - 4) );
      char szBuffer[512];
      dSprintf( szBuffer, 512, "%s(\"%s\", \"%d.apply\");", mColorFunction, getData(), getId());
      mBrowseButton->setField( "Command", szBuffer );
      mBrowseButton->setField( "text", "..." );
      mBrowseButton->setField( "Profile", "GuiInspectorTypeFileNameProfile" );
      mBrowseButton->registerObject();
      addObject( mBrowseButton );

      // Position
      mBrowseButton->resize( browseRect.point, browseRect.extent );
   }

   return retCtrl;
}

void GuiInspectorTypeColor::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   Parent::resize( newPosition, newExtent );

   if( mEdit != NULL )
   {
      // Calculate Caption Rect
      RectI captionRect( mBounds.point , Point2I( (S32)mFloor( mBounds.extent.x * (F32)( (F32)GuiInspectorField::smCaptionWidth / 100.0f ) ) - 2, (S32)mBounds.extent.y ) );

      // Calculate Edit Field Rect
      RectI editFieldRect( Point2I( captionRect.extent.x + 1, 0 ) , Point2I( mBounds.extent.x - ( captionRect.extent.x + 25 ) , mBounds.extent.y ) );

      mEdit->resize( editFieldRect.point, editFieldRect.extent );

      if( mBrowseButton != NULL )
      {
         RectI browseRect( Point2I( ( mBounds.point.x + mBounds.extent.x) - 26, 2), Point2I(20, mBounds.extent.y - 4) );
         mBrowseButton->resize( browseRect.point, browseRect.extent );
      }
   }
}


//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeColorI
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeColorI);

void GuiInspectorTypeColorI::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeColorI)->setInspectorFieldType("GuiInspectorTypeColorI");
}

GuiInspectorTypeColorI::GuiInspectorTypeColorI()
{
   mColorFunction = StringTable->insert("getColorI");
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeColorF
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeColorF);

void GuiInspectorTypeColorF::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeColorF)->setInspectorFieldType("GuiInspectorTypeColorF");
}

GuiInspectorTypeColorF::GuiInspectorTypeColorF()
{
   mColorFunction = StringTable->insert("getColorF");
}



