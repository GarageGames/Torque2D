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

#include "gui/guiScriptNotifyControl.h"
#include "console/consoleTypes.h"
//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiScriptNotifyCtrl);

GuiScriptNotifyCtrl::GuiScriptNotifyCtrl()
{
   mOnChildAdded = false;
   mOnChildRemoved = false;
   mOnResize = false;
   mOnChildResized = false;
   mOnParentResized = false;
}

GuiScriptNotifyCtrl::~GuiScriptNotifyCtrl()
{
}

void GuiScriptNotifyCtrl::initPersistFields()
{
   // Callbacks Group
   addGroup("Callbacks");
   addField("onChildAdded", TypeBool, Offset( mOnChildAdded, GuiScriptNotifyCtrl ) );
   addField("onChildRemoved", TypeBool, Offset( mOnChildRemoved, GuiScriptNotifyCtrl ) );
   addField("onChildResized", TypeBool, Offset( mOnChildResized, GuiScriptNotifyCtrl ) );
   addField("onParentResized", TypeBool, Offset( mOnParentResized, GuiScriptNotifyCtrl ) );
   addField("onResize", TypeBool, Offset( mOnResize, GuiScriptNotifyCtrl ) );
   addField("onLoseFirstResponder", TypeBool, Offset( mOnLoseFirstResponder, GuiScriptNotifyCtrl ) );
   addField("onGainFirstResponder", TypeBool, Offset( mOnGainFirstResponder, GuiScriptNotifyCtrl ) );
   endGroup("Callbacks");

   Parent::initPersistFields();
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
void GuiScriptNotifyCtrl::onChildAdded( GuiControl *child )
{
   Parent::onChildAdded( child );

   // Call Script.
   if( mOnChildAdded && isMethod( "onChildAdded" ) )
      Con::executef(this, 3, "onChildAdded", child->getIdString() );
}

void GuiScriptNotifyCtrl::onChildRemoved( GuiControl *child )
{
   Parent::onChildRemoved( child );

   // Call Script.
   if( mOnChildRemoved && isMethod( "onChildRemoved" ) )
      Con::executef(this, 3, "onChildRemoved", child->getIdString() ); 
}
//----------------------------------------------------------------

void GuiScriptNotifyCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   Parent::resize( newPosition, newExtent );

   // Call Script.
   if( mOnResize && isMethod( "onResize" ) )
      Con::executef(this, 2, "onResize" );

}

void GuiScriptNotifyCtrl::childResized(GuiScriptNotifyCtrl *child)
{
   Parent::childResized( child );

   // Call Script.
   if( mOnChildResized && isMethod( "onChildResized" ) )
      Con::executef(this, 3, "onChildResized", child->getIdString() );
}

void GuiScriptNotifyCtrl::parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent)
{
   Parent::parentResized( oldParentExtent, newParentExtent );

   // Call Script.
   if( mOnParentResized && isMethod( "onParentResized" ) )
      Con::executef(this, 2, "onParentResized" );

}
 
void GuiScriptNotifyCtrl::onLoseFirstResponder()
{
   Parent::onLoseFirstResponder();

   // Call Script.
   if( mOnLoseFirstResponder && isMethod( "onLoseFirstResponder" ) )
      Con::executef(this, 2, "onLoseFirstResponder" );

}

void GuiScriptNotifyCtrl::setFirstResponder( GuiControl* firstResponder )
{
   Parent::setFirstResponder( firstResponder );

   // Call Script.
   if( mOnGainFirstResponder && isFirstResponder() && isMethod( "onGainFirstResponder" ) )
      Con::executef(this, 2, "onGainFirstResponder" );

}

void GuiScriptNotifyCtrl::setFirstResponder()
{
   Parent::setFirstResponder();

   // Call Script.
   if( mOnGainFirstResponder && isFirstResponder() && isMethod( "onGainFirstResponder" ) )
      Con::executef(this, 2, "onGainFirstResponder" );
}

void GuiScriptNotifyCtrl::onMessage(GuiScriptNotifyCtrl *sender, S32 msg)
{
   Parent::onMessage( sender, msg );
}

void GuiScriptNotifyCtrl::onDialogPush()
{
   Parent::onDialogPush();
}

void GuiScriptNotifyCtrl::onDialogPop()
{
   Parent::onDialogPop();
}


//void GuiScriptNotifyCtrl::onMouseUp(const GuiEvent &event)
//{
//}
//
//void GuiScriptNotifyCtrl::onMouseDown(const GuiEvent &event)
//{
//}
//
//void GuiScriptNotifyCtrl::onMouseMove(const GuiEvent &event)
//{
//}
//
//void GuiScriptNotifyCtrl::onMouseDragged(const GuiEvent &event)
//{
//}
//
//void GuiScriptNotifyCtrl::onMouseEnter(const GuiEvent &)
//{
//}
//
//void GuiScriptNotifyCtrl::onMouseLeave(const GuiEvent &)
//{
//}
//
//bool GuiScriptNotifyCtrl::onMouseWheelUp( const GuiEvent &event )
//{
//}
//
//bool GuiScriptNotifyCtrl::onMouseWheelDown( const GuiEvent &event )
//{
//}
//
//void GuiScriptNotifyCtrl::onRightMouseDown(const GuiEvent &)
//{
//}
//
//void GuiScriptNotifyCtrl::onRightMouseUp(const GuiEvent &)
//{
//}
//
//void GuiScriptNotifyCtrl::onRightMouseDragged(const GuiEvent &)
//{
//}
//
//void GuiScriptNotifyCtrl::onMiddleMouseDown(const GuiEvent &)
//{
//}
//
//void GuiScriptNotifyCtrl::onMiddleMouseUp(const GuiEvent &)
//{
//}
//
//void GuiScriptNotifyCtrl::onMiddleMouseDragged(const GuiEvent &)
//{
//}
//void GuiScriptNotifyCtrl::onMouseDownEditor(const GuiEvent &event, Point2I offset)
//{
//}
//void GuiScriptNotifyCtrl::onRightMouseDownEditor(const GuiEvent &event, Point2I offset)
//{
//}

//bool GuiScriptNotifyCtrl::onKeyDown(const GuiEvent &event)
//{
//  if ( Parent::onKeyDown( event ) )
//     return true;
//}
//
//bool GuiScriptNotifyCtrl::onKeyRepeat(const GuiEvent &event)
//{
//   // default to just another key down.
//   return onKeyDown(event);
//}
//
//bool GuiScriptNotifyCtrl::onKeyUp(const GuiEvent &event)
//{
//  if ( Parent::onKeyUp( event ) )
//     return true;
//}
