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
#include "platform/event.h"
#include "game/gameInterface.h"
#include "io/fileStream.h"
#include "console/console.h"
#include "platform/threads/mutex.h"

// Script binding.
#include "game/gameInterface_ScriptBinding.h"

GameInterface *Game = NULL;
void *gGameEventQueueMutex = NULL;
FileStream gJournalStream;

#ifdef TORQUE_DEBUG
static U32 sReentrantCount = 0;
#endif

//-----------------------------------------------------------------------------

struct ReadEvent : public Event
{
   U8 data[3072];
};

//-----------------------------------------------------------------------------

GameInterface::GameInterface()
{
   AssertFatal(Game == NULL, "ERROR: Multiple games declared.");
   Game = this;
   mJournalMode = JournalOff;
   mRunning = true;
   mRequiresRestart = false;
   if(!gGameEventQueueMutex)
      gGameEventQueueMutex = Mutex::createMutex();
   eventQueue = &eventQueue1;
}

//-----------------------------------------------------------------------------

void GameInterface::processEvent(Event *event)
{
   if(!mRunning)
      return;

   if(PlatformAssert::processingAssert()) // ignore any events if an assert dialog is up.
      return;

#ifdef TORQUE_DEBUG
   sReentrantCount++;
   AssertFatal(sReentrantCount == 1, "Error! ProcessEvent is NOT re-entrant.");
#endif

   switch(event->type)
   {
      case PacketReceiveEventType:
         processPacketReceiveEvent((PacketReceiveEvent *) event);
         break;
      case MouseMoveEventType:
         processMouseMoveEvent((MouseMoveEvent *) event);
         break;
      case InputEventType:
         processInputEvent((InputEvent *) event);
         break;
      case ScreenTouchEventType:
          processScreenTouchEvent((ScreenTouchEvent *) event);
          break;
      case QuitEventType:
         processQuitEvent();
         break;
      case TimeEventType:
         processTimeEvent((TimeEvent *) event);
         break;
      case ConsoleEventType:
         processConsoleEvent((ConsoleEvent *) event);
         break;
      case ConnectedAcceptEventType:
         processConnectedAcceptEvent( (ConnectedAcceptEvent *) event );
         break;
      case ConnectedReceiveEventType:
         processConnectedReceiveEvent( (ConnectedReceiveEvent *) event );
         break;
      case ConnectedNotifyEventType:
         processConnectedNotifyEvent( (ConnectedNotifyEvent *) event );
         break;
   }

#ifdef TORQUE_DEBUG
   sReentrantCount--;
#endif
}

//-----------------------------------------------------------------------------

void GameInterface::postEvent(Event &event)
{
#ifdef TORQUE_ALLOW_JOURNALING
   if(mJournalMode == JournalPlay && event.type != QuitEventType)
      return;
#endif //TORQUE_ALLOW_JOURNALING

   // Only one thread can post at a time.
   Mutex::lockMutex(gGameEventQueueMutex);

#ifdef TORQUE_ALLOW_JOURNALING
   if(mJournalMode == JournalSave)
   {
      gJournalStream.write(event.size, &event);
      gJournalStream.flush();
   }
#endif //TORQUE_ALLOW_JOURNALING 

   // Create a deep copy of event, and save a pointer to the copy in a vector.
   Event* copy = (Event*)dMalloc(event.size);
   dMemcpy(copy, &event, event.size);
   eventQueue->push_back(copy);
   
   Mutex::unlockMutex(gGameEventQueueMutex);   
}




void GameInterface::processEvents()
{
   // We want to lock the queue when processing as well - don't need
   // anyone putting new events in the middle of this.
   // We double-buffer the event queues so we'll block the other thread(s) for
   // a minimum amount of time.
   Mutex::lockMutex(gGameEventQueueMutex);
      // swap event queue pointers
      Vector<Event*> &fullEventQueue = *eventQueue;
      if(eventQueue == &eventQueue1)
         eventQueue = &eventQueue2;
      else
         eventQueue = &eventQueue1;
   Mutex::unlockMutex(gGameEventQueueMutex);

   // Walk the event queue in fifo order, processing the events, then clear the queue.
   for(int i=0; i < fullEventQueue.size(); i++)
   {
      Game->processEvent(fullEventQueue[i]);
      dFree(fullEventQueue[i]);
   }
   fullEventQueue.clear();

}

void GameInterface::journalProcess()
{
#ifdef TORQUE_ALLOW_JOURNALING
   if(mJournalMode == JournalPlay)
   {
      ReadEvent journalReadEvent;
// used to be:
//      if(gJournalStream.read(&journalReadEvent.type))
//        if(gJournalStream.read(&journalReadEvent.size))
// for proper non-endian stream handling, the read-ins should match the write-out by using bytestreams read:
      if(gJournalStream.read(sizeof(Event), &journalReadEvent))
      {
         if(gJournalStream.read(journalReadEvent.size - sizeof(Event), &journalReadEvent.data))
         {
            if(gJournalStream.getPosition() == gJournalStream.getStreamSize() && mJournalBreak)
               Platform::debugBreak();
            processEvent(&journalReadEvent);
            return;
         }
      }
      // JournalBreak is used for debugging, so halt all game
      // events if we get this far.
      if(mJournalBreak)
         mRunning = false;
      else
         mJournalMode = JournalOff;
   }
#endif //TORQUE_ALLOW_JOURNALING
}

void GameInterface::saveJournal(const char *fileName)
{
   mJournalMode = JournalSave;
   gJournalStream.open(fileName, FileStream::Write);
}

void GameInterface::playJournal(const char *fileName,bool journalBreak)
{
   mJournalMode = JournalPlay;
   mJournalBreak = journalBreak;
   gJournalStream.open(fileName, FileStream::Read);
}

FileStream *GameInterface::getJournalStream( void )
{
   return &gJournalStream;
}

void GameInterface::journalRead(U32 *val)
{
   gJournalStream.read(val);
}

void GameInterface::journalWrite(U32 val)
{
   gJournalStream.write(val);
}

void GameInterface::journalRead(U32 size, void *buffer)
{
   gJournalStream.read(size, buffer);
}

void GameInterface::journalWrite(U32 size, const void *buffer)
{
   gJournalStream.write(size, buffer);
}

