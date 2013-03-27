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
#include "gui/buttons/guiButtonBaseCtrl.h"
#include "gui/language/lang.h"

IMPLEMENT_CONOBJECT(GuiButtonBaseCtrl);

GuiButtonBaseCtrl::GuiButtonBaseCtrl()
{
   mDepressed = false;
   mMouseOver = false;
   mActive = true;
   mButtonText = StringTable->insert("Button");
   mButtonTextID = StringTable->EmptyString;
   mStateOn = false;
    mRadioGroup = -1;
   mButtonType = ButtonTypePush;
   mUseMouseEvents = false;
}

bool GuiButtonBaseCtrl::onWake()
{
   if(!Parent::onWake())
      return false;

   // is we have a script variable, make sure we're in sync
   if ( mConsoleVariable[0] )
    mStateOn = Con::getBoolVariable( mConsoleVariable );
   if(mButtonTextID && *mButtonTextID != 0)
       setTextID(mButtonTextID);

   return true;
}

ConsoleMethod( GuiButtonBaseCtrl, performClick, void, 2, 2, "() - simulates a button click from script." )
{
   object->onAction();
}

ConsoleMethod( GuiButtonBaseCtrl, setText, void, 3, 3, "(string text) - Sets the text of the button to the string." )
{
   object->setText( argv[2] );
}

ConsoleMethod( GuiButtonBaseCtrl, setTextID, void, 3, 3, "(string id) - Sets the text of the button to the localized string." )
{
    object->setTextID(argv[2]);
}
ConsoleMethod( GuiButtonBaseCtrl, getText, const char *, 2, 2, "() - returns the text of the button.\n" 
              "@return The text member of the button as a char string")
{
   return object->getText( );
}
ConsoleMethod( GuiButtonBaseCtrl, setStateOn, void, 3, 3, "(bool isStateOn) - sets the state on member and updates siblings of the same group." )
{
   object->setStateOn(dAtob(argv[2]));
}

ConsoleMethod(GuiButtonBaseCtrl, getStateOn, bool, 2, 2, "(bool getStateOn) - gets whether the state of the button is currently 'on'" )
{
   return object->getStateOn();
}


static EnumTable::Enums buttonTypeEnums[] = 
{
   { GuiButtonBaseCtrl::ButtonTypePush, "PushButton" },
   { GuiButtonBaseCtrl::ButtonTypeCheck, "ToggleButton" },
   { GuiButtonBaseCtrl::ButtonTypeRadio, "RadioButton" },
};
      
static EnumTable gButtonTypeTable(3, &buttonTypeEnums[0]); 
      
      

void GuiButtonBaseCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("GuiButtonBaseCtrl");		
   addField("text", TypeCaseString, Offset(mButtonText, GuiButtonBaseCtrl));
   addField("textID", TypeString, Offset(mButtonTextID, GuiButtonBaseCtrl));
   addField("groupNum", TypeS32, Offset(mRadioGroup, GuiButtonBaseCtrl));
   addField("buttonType", TypeEnum, Offset(mButtonType, GuiButtonBaseCtrl), 1, &gButtonTypeTable);
   addField("useMouseEvents", TypeBool, Offset(mUseMouseEvents, GuiButtonBaseCtrl));
   endGroup("GuiButtonBaseCtrl");		
}

void GuiButtonBaseCtrl::setText(const char *text)
{
   mButtonText = StringTable->insert(text);
}

void GuiButtonBaseCtrl::setStateOn( bool bStateOn )
{
   if(!mActive)
      return;

   if(mButtonType == ButtonTypeCheck)
   {
      mStateOn = bStateOn;
   }
   else if(mButtonType == ButtonTypeRadio)
   {
      messageSiblings(mRadioGroup);
      mStateOn = bStateOn;
   }		
   setUpdate();
}

void GuiButtonBaseCtrl::setTextID(const char *id)
{
    S32 n = Con::getIntVariable(id, -1);
    if(n != -1)
    {
        mButtonTextID = StringTable->insert(id);
        setTextID(n);
    }
}
void GuiButtonBaseCtrl::setTextID(S32 id)
{
    const UTF8 *str = getGUIString(id);
    if(str)
        setText((const char*)str);
    //mButtonTextID = id;
}
const char *GuiButtonBaseCtrl::getText()
{
   return mButtonText;
}

//---------------------------------------------------------------------------
void GuiButtonBaseCtrl::acceleratorKeyPress(U32)
{
   if (! mActive)
      return;

   //set the bool
   mDepressed = true;

   if (mProfile->mTabable)
      setFirstResponder();
}

//---------------------------------------------------------------------------
void GuiButtonBaseCtrl::acceleratorKeyRelease(U32)
{
   if (! mActive)
      return;

   if (mDepressed)
   {
      //set the bool
      mDepressed = false;
      //perform the action
      onAction();
   }

   //update
   setUpdate();
}

void GuiButtonBaseCtrl::onMouseDown(const GuiEvent &event)
{
   if (! mActive)
      return;

   if (mProfile->mCanKeyFocus)
      setFirstResponder();

   if (mProfile->mSoundButtonDown)
   {
      AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonDown);
      alxPlay(handle);
   }

   //lock the mouse
   mouseLock();
   mDepressed = true;

   //update
   setUpdate();
}

void GuiButtonBaseCtrl::onMouseEnter(const GuiEvent &event)
{
   setUpdate();

   if(mUseMouseEvents)
      Con::executef( this, 1, "onMouseEnter" );

   if(isMouseLocked())
   {
      mDepressed = true;
      mMouseOver = true;
   }
   else
   {
      if ( mActive && mProfile->mSoundButtonOver )
      {
         AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonOver);
         alxPlay(handle);
      }
      mMouseOver = true;
   }
}

//Luma: Some fixes from the forums, Dave Calabrese
//http://www.garagegames.com/community/forums/viewthread/93467/1#comment-669559
void GuiButtonBaseCtrl::onMouseLeave(const GuiEvent &)  
{  
    if(isMouseLocked())  
        mDepressed = false;  
      
    mouseUnlock();  
    setUpdate();  
  
   if(mUseMouseEvents)  
      Con::executef( this, 1, "onMouseLeave" );  
  
   mMouseOver = false;  
}  

void GuiButtonBaseCtrl::onMouseUp(const GuiEvent &event)
{
   if (! mActive)
      return;

   mouseUnlock();

   setUpdate();

   //if we released the mouse within this control, perform the action
   if (mDepressed)
      onAction();

   // Execute callback
   if (mUseMouseEvents)
   {
       char buf[3][32];
       dSprintf(buf[0], 32, "%d", event.modifier);
       dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
       dSprintf(buf[2], 32, "%d", event.mouseClickCount);
       Con::executef(this, 4, "onMouseUp", buf[0], buf[1], buf[2]);
   }

   mDepressed = false;
}

void GuiButtonBaseCtrl::onRightMouseUp(const GuiEvent &event)
{
   Con::executef( this, 2, "onRightClick" );

   Parent::onRightMouseUp( event );
}

//--------------------------------------------------------------------------
bool GuiButtonBaseCtrl::onKeyDown(const GuiEvent &event)
{
   //if the control is a dead end, kill the event
   if (!mActive)
      return true;

   //see if the key down is a return or space or not
   if ((event.keyCode == KEY_RETURN || event.keyCode == KEY_SPACE)
       && event.modifier == 0)
   {
       if ( mProfile->mSoundButtonDown )
       {
          AUDIOHANDLE handle = alxCreateSource( mProfile->mSoundButtonDown );
          alxPlay( handle );
       }
      return true;
   }
   //otherwise, pass the event to it's parent
   return Parent::onKeyDown(event);
}

//--------------------------------------------------------------------------
bool GuiButtonBaseCtrl::onKeyUp(const GuiEvent &event)
{
   //if the control is a dead end, kill the event
   if (!mActive)
      return true;

   //see if the key down is a return or space or not
   if (mDepressed &&
      (event.keyCode == KEY_RETURN || event.keyCode == KEY_SPACE) &&
      event.modifier == 0)
   {
      onAction();
      return true;
   }

   //otherwise, pass the event to it's parent
   return Parent::onKeyUp(event);
}

//---------------------------------------------------------------------------
void GuiButtonBaseCtrl::setScriptValue(const char *value)
{
    mStateOn = dAtob(value);

    // Update the console variable:
    if ( mConsoleVariable[0] )
        Con::setBoolVariable( mConsoleVariable, mStateOn );

   setUpdate();
}

//---------------------------------------------------------------------------
const char *GuiButtonBaseCtrl::getScriptValue()
{
    return mStateOn ? "1" : "0";
}

//---------------------------------------------------------------------------
void GuiButtonBaseCtrl::onAction()
{
    if(!mActive)
        return;

    if(mButtonType == ButtonTypeCheck)
    {
        mStateOn = mStateOn ? false : true;

        // Update the console variable:
        if ( mConsoleVariable[0] )
            Con::setBoolVariable( mConsoleVariable, mStateOn );
        // Execute the console command (if any). Unnecessary. Parent does this already.
        //if( mConsoleCommand[0] )
        //   Con::evaluate( mConsoleCommand, false );

    }
    else if(mButtonType == ButtonTypeRadio)
    {
        mStateOn = true;
        messageSiblings(mRadioGroup);
    }		
    setUpdate();


    // Provide and onClick script callback.
    if( isMethod("onClick") )
       Con::executef( this, 2, "onClick" );

    Parent::onAction();
}

//---------------------------------------------------------------------------
void GuiButtonBaseCtrl::onMessage( GuiControl *sender, S32 msg )
{
    Parent::onMessage(sender, msg);
    if( mRadioGroup == msg && mButtonType == ButtonTypeRadio )
    {
        setUpdate();
        mStateOn = ( sender == this );
    }
}
