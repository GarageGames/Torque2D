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
** Filename:    dispatcher.cc
** Author:      Tom Bampton
** Created:     19/8/2006
** Purpose:
**   Message Dispatcher
**
*/

#include "messaging/dispatcher.h"
#include "platform/threads/mutex.h"
#include "collection/simpleHashTable.h"
#include "memory/safeDelete.h"

#include "dispatcher_ScriptBinding.h"

namespace Dispatcher
{

//////////////////////////////////////////////////////////////////////////
// IMessageListener Methods
//////////////////////////////////////////////////////////////////////////

IMessageListener::~IMessageListener()
{
   for(S32 i = 0;i < mQueues.size();i++)
   {
      unregisterMessageListener(mQueues[i], this);
   }
}

void IMessageListener::onAddToQueue(StringTableEntry queue)
{
   // [tom, 8/20/2006] The dispatcher won't let us get added twice, so no need
   // to worry about it here.

   mQueues.push_back(queue);
}

void IMessageListener::onRemoveFromQueue(StringTableEntry queue)
{
   for(S32 i = 0;i < mQueues.size();i++)
   {
      if(mQueues[i] == queue)
      {
         mQueues.erase(i);
         return;
      }
   }
}

//////////////////////////////////////////////////////////////////////////
// Global State
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// @brief Internal class used by the dispatcher
//////////////////////////////////////////////////////////////////////////
static struct _DispatchData
{
   void *mMutex;
   SimpleHashTable<MessageQueue> mQueues;

   _DispatchData()
   {
      mMutex = Mutex::createMutex();
   }

   ~_DispatchData()
   {
      if(Mutex::lockMutex( mMutex ) )
      {
         mQueues.clearTables();

         Mutex::unlockMutex( mMutex );
      }

      Mutex::destroyMutex( mMutex );
      //SAFE_DELETE(mMutex);
      mMutex = NULL;
   }
} gDispatchData;

//////////////////////////////////////////////////////////////////////////
// Queue Registration
//////////////////////////////////////////////////////////////////////////

bool isQueueRegistered(const char *name)
{
   MutexHandle mh;
   if(mh.lock(gDispatchData.mMutex, true))
   {
      return gDispatchData.mQueues.retrieve(name) != NULL;
   }

   return false;
}

void registerMessageQueue(const char *name)
{
   if(isQueueRegistered(name))
      return;

   if(Mutex::lockMutex( gDispatchData.mMutex, true ))
   {
      MessageQueue *queue = new MessageQueue;
      queue->mQueueName = StringTable->insert(name);
      gDispatchData.mQueues.insert(queue, name);

      Mutex::unlockMutex( gDispatchData.mMutex );
   }
}

void unregisterMessageQueue(const char *name)
{
   MutexHandle mh;
   if(mh.lock(gDispatchData.mMutex, true))
   {
      MessageQueue *queue = gDispatchData.mQueues.remove(name);
      if(queue == NULL)
         return;

      // Tell the listeners about it
      for(S32 i = 0;i < queue->mListeners.size();i++)
      {
         queue->mListeners[i]->onRemoveFromQueue(name);
      }

      delete queue;
   }
}

//////////////////////////////////////////////////////////////////////////
// Message Listener Registration
//////////////////////////////////////////////////////////////////////////

bool registerMessageListener(const char *queue, IMessageListener *listener)
{
   if(! isQueueRegistered(queue))
      registerMessageQueue(queue);

   MutexHandle mh;

   if(! mh.lock(gDispatchData.mMutex, true))
      return false;

   MessageQueue *q = gDispatchData.mQueues.retrieve(queue);
   if(q == NULL)
   {
      Con::errorf("Dispatcher::registerMessageListener - Queue '%s' not found?! It should have been added automatically!", queue);
      return false;
   }

   for(VectorPtr<IMessageListener *>::iterator i = q->mListeners.begin();i != q->mListeners.end();i++)
   {
      if(*i == listener)
         return false;
   }

   q->mListeners.push_front(listener);
   listener->onAddToQueue(StringTable->insert(queue));
   return true;
}

void unregisterMessageListener(const char *queue, IMessageListener *listener)
{
   if(! isQueueRegistered(queue))
      return;

   MutexHandle mh;

   if(! mh.lock(gDispatchData.mMutex, true))
      return;

   MessageQueue *q = gDispatchData.mQueues.retrieve(queue);
   if(q == NULL)
      return;

   for(VectorPtr<IMessageListener *>::iterator i = q->mListeners.begin();i != q->mListeners.end();i++)
   {
      if(*i == listener)
      {
         listener->onRemoveFromQueue(StringTable->insert(queue));
         q->mListeners.erase(i);
         return;
      }
   }
}

//////////////////////////////////////////////////////////////////////////
// Dispatcher
//////////////////////////////////////////////////////////////////////////

bool dispatchMessage(const char *queue, const char *msg, const char *data)
{
   MutexHandle mh;

   if(! mh.lock(gDispatchData.mMutex, true))
      return true;

   MessageQueue *q = gDispatchData.mQueues.retrieve(queue);
   if(q == NULL)
   {
      Con::errorf("Dispatcher::dispatchMessage - Attempting to dispatch to unknown queue '%s'", queue);
      return true;
   }

   return q->dispatchMessage(msg, data);
}


bool dispatchMessageObject(const char *queue, Message *msg)
{
   MutexHandle mh;

   if(msg == NULL)
      return true;

   msg->addReference();

   if(! mh.lock(gDispatchData.mMutex, true))
   {
      msg->freeReference();
      return true;
   }

   MessageQueue *q = gDispatchData.mQueues.retrieve(queue);
   if(q == NULL)
   {
      Con::errorf("Dispatcher::dispatchMessage - Attempting to dispatch to unknown queue '%s'", queue);
      msg->freeReference();
      return true;
   }

   // [tom, 8/19/2006] Make sure that the message is registered with the sim, since
   // when it's ref count is zero it'll be deleted with deleteObject()
   if(! msg->isProperlyAdded())
   {
      SimObjectId id = Message::getNextMessageID();
      if(id != 0xffffffff)
         msg->registerObject(id);
      else
      {
         Con::errorf("dispatchMessageObject: Message was not registered and no more object IDs are available for messages");

         msg->freeReference();
         return false;
      }
   }

   bool bResult = q->dispatchMessageObject(msg);
   msg->freeReference();

   return bResult;
}

//////////////////////////////////////////////////////////////////////////
// Internal Functions
//////////////////////////////////////////////////////////////////////////

MessageQueue * getMessageQueue(const char *name)
{
   return gDispatchData.mQueues.retrieve(name);
}

extern bool lockDispatcherMutex()
{
   return Mutex::lockMutex(gDispatchData.mMutex);
}

extern void unlockDispatcherMutex()
{
   Mutex::unlockMutex(gDispatchData.mMutex);
}

} // end namespace Dispatcher
