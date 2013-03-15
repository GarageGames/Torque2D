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

/*
** Alive and Ticking
** (c) Copyright 2006 Burnt Wasp
**     All Rights Reserved.
**
** Filename:    scriptMsgListener.cc
** Author:      Tom Bampton
** Created:     19/8/2006
** Purpose:
**   Script Message Listener
**
*/

#include "console/consoleTypes.h"
#include "messaging/dispatcher.h"
#include "messaging/scriptMsgListener.h"

//////////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////////

ScriptMsgListener::ScriptMsgListener()
{
}

IMPLEMENT_CONOBJECT(ScriptMsgListener);

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

bool ScriptMsgListener::onMessageReceived(StringTableEntry queue, const char* event, const char* data)
{
   return dAtob(Con::executef(this, 4, "onMessageReceived", queue, event, data));
}

bool ScriptMsgListener::onMessageObjectReceived(StringTableEntry queue, Message *msg)
{
   return dAtob(Con::executef(this, 4, "onMessageObjectReceived", queue, Con::getIntArg(msg->getId())));
}

//////////////////////////////////////////////////////////////////////////

void ScriptMsgListener::onAddToQueue(StringTableEntry queue)
{
   Con::executef(this, 2, "onAddToQueue", queue);
   IMLParent::onAddToQueue(queue);
}

void ScriptMsgListener::onRemoveFromQueue(StringTableEntry queue)
{
   Con::executef(this, 2, "onRemoveFromQueue", queue);
   IMLParent::onRemoveFromQueue(queue);
}
