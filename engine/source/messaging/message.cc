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
** Filename:    messageQueue.cc
** Author:      Tom Bampton
** Created:     19/8/2006
** Purpose:
**   Message
**
*/

#include "platform/platform.h"
#include "console/consoleTypes.h"
#include "messaging/message.h"
#include "memory/safeDelete.h"
#include "io/bitStream.h"

#include "message_ScriptBinding.h"

//////////////////////////////////////////////////////////////////////////

namespace Sim
{
extern SimIdDictionary *gIdDictionary;
}

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

Message::Message()
{
   mRefCount = 0;
}


IMPLEMENT_CONOBJECT(Message);


//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

SimObjectId Message::getNextMessageID()
{
   for(S32 i = MessageObjectIdFirst;i < MessageObjectIdLast;i++)
   {
      if(Sim::gIdDictionary->find(i) == NULL)
         return i;
   }

   // Oh shit ...
   return 0xffffffff;
}

//////////////////////////////////////////////////////////////////////////

const char *Message::getType()
{
   if(mClassName && mClassName[0] != 0)
      return mClassName;

   return getClassName();
}

//////////////////////////////////////////////////////////////////////////

void Message::addReference()
{
   mRefCount++;
}

void Message::freeReference()
{
   AssertFatal(mRefCount >= 0, "Negative refcount ? Someone's not cleaning up properly!");

   mRefCount--;
   if(mRefCount <= 0)
   {
      // [tom, 8/26/2006] When messages are dispatched, the calling code assumes
      // that dispatchMessage() will free the message unless a reference has been
      // added.
      //
      // It is possible if dispatchMessage() fails for the message to not get added
      // to the sim, and thus we need to delete this; in that case.
      if( isProperlyAdded() )
         deleteObject();
      else
         delete this;
      return;
   }
}
