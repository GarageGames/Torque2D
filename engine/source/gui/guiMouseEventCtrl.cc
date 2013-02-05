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

#include "gui/guiMouseEventCtrl.h"
#include "console/consoleTypes.h"

IMPLEMENT_CONOBJECT(GuiMouseEventCtrl);

GuiMouseEventCtrl::GuiMouseEventCtrl()
{
   mLockMouse = false;
}

//------------------------------------------------------------------------------
void GuiMouseEventCtrl::sendMouseEvent(const char * name, const GuiEvent & event)
{
   char buf[3][32];
   dSprintf(buf[0], 32, "%d", event.modifier);
   dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
   dSprintf(buf[2], 32, "%d", event.mouseClickCount);
   Con::executef(this, 4, name, buf[0], buf[1], buf[2]);
}

//------------------------------------------------------------------------------
void GuiMouseEventCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("lockMouse", TypeBool, Offset(mLockMouse, GuiMouseEventCtrl));

   Con::setIntVariable("$EventModifier::LSHIFT",      SI_LSHIFT);
   Con::setIntVariable("$EventModifier::RSHIFT",      SI_RSHIFT);
   Con::setIntVariable("$EventModifier::SHIFT",       SI_SHIFT);
   Con::setIntVariable("$EventModifier::LCTRL",       SI_LCTRL);
   Con::setIntVariable("$EventModifier::RCTRL",       SI_RCTRL);
   Con::setIntVariable("$EventModifier::CTRL",        SI_CTRL);
   Con::setIntVariable("$EventModifier::LALT",        SI_LALT);
   Con::setIntVariable("$EventModifier::RALT",        SI_RALT);
   Con::setIntVariable("$EventModifier::ALT",         SI_ALT);
}

//------------------------------------------------------------------------------
void GuiMouseEventCtrl::onMouseDown(const GuiEvent & event)
{
   if(mLockMouse)
      mouseLock();
   sendMouseEvent("onMouseDown", event);
}

void GuiMouseEventCtrl::onMouseUp(const GuiEvent & event)
{
   if(mLockMouse)
      mouseUnlock();
   sendMouseEvent("onMouseUp", event);
}

void GuiMouseEventCtrl::onMouseMove(const GuiEvent & event)
{
   sendMouseEvent("onMouseMove", event);
}

void GuiMouseEventCtrl::onMouseDragged(const GuiEvent & event)
{
   sendMouseEvent("onMouseDragged", event);
}

void GuiMouseEventCtrl::onMouseEnter(const GuiEvent & event)
{
   sendMouseEvent("onMouseEnter", event);
}

void GuiMouseEventCtrl::onMouseLeave(const GuiEvent & event)
{
   sendMouseEvent("onMouseLeave", event);
}

void GuiMouseEventCtrl::onRightMouseDown(const GuiEvent & event)
{
   if(mLockMouse)
      mouseLock();
   sendMouseEvent("onRightMouseDown", event);
}

void GuiMouseEventCtrl::onRightMouseUp(const GuiEvent & event)
{
   if(mLockMouse)
      mouseUnlock();
   sendMouseEvent("onRightMouseUp", event);
}

void GuiMouseEventCtrl::onRightMouseDragged(const GuiEvent & event)
{
   sendMouseEvent("onRightMouseDragged", event);
}
