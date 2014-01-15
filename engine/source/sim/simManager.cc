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
#include "platform/threads/mutex.h"
#include "sim/simBase.h"
#include "string/stringTable.h"
#include "console/console.h"
#include "io/fileStream.h"
#include "io/resource/resourceManager.h"
#include "io/fileObject.h"
#include "console/consoleInternal.h"
#include "memory/safeDelete.h"

//---------------------------------------------------------------------------

namespace Sim
{
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// event queue variables:

SimTime gCurrentTime;
SimTime gTargetTime;

void *gEventQueueMutex;
SimEvent *gEventQueue;
U32 gEventSequence;

//---------------------------------------------------------------------------
// event queue init/shutdown

void initEventQueue()
{
   gCurrentTime = 0;
   gTargetTime = 0;
   gEventSequence = 1;
   gEventQueue = NULL;
   gEventQueueMutex = Mutex::createMutex();
}

void shutdownEventQueue()
{
   // Delete all pending events
   Mutex::lockMutex(gEventQueueMutex);
   SimEvent *walk = gEventQueue;
   while(walk)
   {
      SimEvent *temp = walk->nextEvent;
      delete walk;
      walk = temp;
   }
   Mutex::unlockMutex(gEventQueueMutex);
   Mutex::destroyMutex(gEventQueueMutex);
}

//---------------------------------------------------------------------------
// event post

U32 postEvent(SimObject *destObject, SimEvent* event,U32 time)
{
    AssertFatal(time == -1 || time >= getCurrentTime(),
        "Sim::postEvent: Cannot go back in time. (flux capacitor unavailable -- BJG)");
   AssertFatal(destObject, "Destination object for event doesn't exist.");

   Mutex::lockMutex(gEventQueueMutex);

   if( time == -1 )
      time = gCurrentTime;

   event->time = time;
   event->startTime = gCurrentTime;
   event->destObject = destObject;

   if(!destObject)
   {
      delete event;

      Mutex::unlockMutex(gEventQueueMutex);

      return InvalidEventId;
   }
   event->sequenceCount = gEventSequence++;
   SimEvent **walk = &gEventQueue;
   SimEvent *current;
   
   while((current = *walk) != NULL && (current->time < event->time))
      walk = &(current->nextEvent);
   
   // [tom, 6/24/2005] This ensures that SimEvents are dispatched in the same order that they are posted.
   // This is needed to ensure Con::threadSafeExecute() executes script code in the correct order.
   while((current = *walk) != NULL && (current->time == event->time))
      walk = &(current->nextEvent);
   
   event->nextEvent = current;
   *walk = event;

   U32 seqCount = event->sequenceCount;

   Mutex::unlockMutex(gEventQueueMutex);

   return seqCount;
}

//---------------------------------------------------------------------------
// event cancellation

/*! cancel a previously scheduled event.
	@param eventSequence The numeric ID of a previously scheduled event.
	@return No return value.
	@sa getEventTimeLeft, getScheduleDuration, getTimeSinceStart, isEventPending, schedule, obj.schedule
*/
void cancelEvent(U32 eventSequence)
{
   Mutex::lockMutex(gEventQueueMutex);

   SimEvent **walk = &gEventQueue;
   SimEvent *current;
   
   while((current = *walk) != NULL)
   {
      if(current->sequenceCount == eventSequence)
      {
         *walk = current->nextEvent;
         delete current;
         Mutex::unlockMutex(gEventQueueMutex);
         return;
      }
      else
         walk = &(current->nextEvent);
   }

   Mutex::unlockMutex(gEventQueueMutex);
}

void cancelPendingEvents(SimObject *obj)
{
   Mutex::lockMutex(gEventQueueMutex);

   SimEvent **walk = &gEventQueue;
   SimEvent *current;
   
   while((current = *walk) != NULL)
   {
      if(current->destObject == obj)
      {
         *walk = current->nextEvent;
         delete current;
      }
      else
         walk = &(current->nextEvent);
   }
   Mutex::unlockMutex(gEventQueueMutex);
}

//---------------------------------------------------------------------------
// event pending test

/*!	see if the event associated with eventID is still pending.

	When an event passes, the eventID is removed from the event queue, becoming invalid, so there is no discernable difference between a completed event and a bad event ID.
	@param eventID The numeric ID of a previously scheduled event.
	@return true if this event is still outstanding and false if it has passed or eventID is invalid.

	@sa cancel, getEventTimeLeft, getScheduleDuration, getTimeSinceStart, schedule, obj.schedule
*/
bool isEventPending(U32 eventSequence)
{
   Mutex::lockMutex(gEventQueueMutex);

   for(SimEvent *walk = gEventQueue; walk; walk = walk->nextEvent)
      if(walk->sequenceCount == eventSequence)
      {
         Mutex::unlockMutex(gEventQueueMutex);
         return true;
      }
   Mutex::unlockMutex(gEventQueueMutex);
   return false;
}

/*!
	determines how much time remains until the event specified by eventID occurs.

    @param eventID The numeric ID of a previously scheduled event.
    @return a non-zero integer value equal to the milliseconds until the event specified by eventID will occur. However, if eventID is invalid, or the event has passed, this function will return zero.
    @sa cancel, getScheduleDuration, getTimeSinceStart, isEventPending, schedule, SimObject::schedule
*/
U32 getEventTimeLeft(U32 eventSequence)
{
   Mutex::lockMutex(gEventQueueMutex);

   for(SimEvent *walk = gEventQueue; walk; walk = walk->nextEvent)
      if(walk->sequenceCount == eventSequence)
      {
         SimTime t = walk->time - getCurrentTime();
         Mutex::unlockMutex(gEventQueueMutex);
         return t;
      }

   Mutex::unlockMutex(gEventQueueMutex);

   return 0;   
}

/*!
	Determines how long the event associated with eventID was scheduled for.

	@param eventID The numeric ID of a previously scheduled event.
	@return a non-zero integer value equal to the milliseconds used in the schedule call that created this event. However, if eventID is invalid, this function will return zero.
	@sa cancel, getEventTimeLeft, getTimeSinceStart, isEventPending, schedule, SimObject::schedule
*/
U32 getScheduleDuration(U32 eventSequence)
{
   for(SimEvent *walk = gEventQueue; walk; walk = walk->nextEvent)
      if(walk->sequenceCount == eventSequence)
         return (walk->time-walk->startTime);
   return 0;
}

/*!
	Determines how much time has passed since the event specified by eventID was scheduled.

    @param eventID The numeric ID of a previously scheduled event.
    @return a non-zero integer value equal to the milliseconds that have passed since this event was scheduled. However, if eventID is invalid, or the event has passed, this function will return zero.
    @sa cancel, getEventTimeLeft, getScheduleDuration, isEventPending, schedule, SimObject::schedule
*/
U32 getTimeSinceStart(U32 eventSequence)
{
   for(SimEvent *walk = gEventQueue; walk; walk = walk->nextEvent)
      if(walk->sequenceCount == eventSequence)
         return (getCurrentTime()-walk->startTime);
   return 0;
}

//---------------------------------------------------------------------------
// event timing
    
void advanceToTime(SimTime targetTime)
{
   AssertFatal(targetTime >= getCurrentTime(), "EventQueue::process: cannot advance to time in the past.");

   Mutex::lockMutex(gEventQueueMutex);
   gTargetTime = targetTime;
   while(gEventQueue && gEventQueue->time <= targetTime)
   {
      SimEvent *event = gEventQueue;
      gEventQueue = gEventQueue->nextEvent;
      AssertFatal(event->time >= gCurrentTime,
            "SimEventQueue::pop: Cannot go back in time (flux capacitor not installed - BJG).");
      gCurrentTime = event->time;
      SimObject *obj = event->destObject;

      if(!obj->isDeleted())
         event->process(obj);
      delete event;
   }
    gCurrentTime = targetTime;
   Mutex::unlockMutex(gEventQueueMutex);
}

void advanceTime(SimTime delta)
{
   advanceToTime(getCurrentTime() + delta);
}

/*! get the time, in ticks, that has elapsed since the engine started executing.

    @return the time in ticks since the engine was started.
    @sa getRealTime
*/
U32 getCurrentTime()
{
   if(gEventQueueMutex)
      Mutex::lockMutex(gEventQueueMutex);
   
   SimTime t = gCurrentTime;
   
   if(gEventQueueMutex)
      Mutex::unlockMutex(gEventQueueMutex);

   return t;
}

U32 getTargetTime()
{
   return gTargetTime;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

SimGroup *gRootGroup = NULL;
SimManagerNameDictionary *gNameDictionary;
SimIdDictionary *gIdDictionary;
U32 gNextObjectId;

void initRoot()
{
   gIdDictionary = new SimIdDictionary;
   gNameDictionary = new SimManagerNameDictionary;

   gRootGroup = new SimGroup();
   gRootGroup->setId(RootGroupId);
   gRootGroup->assignName("RootGroup");
   gRootGroup->registerObject();

   gNextObjectId = DynamicObjectIdFirst;
}

void shutdownRoot()
{
   gRootGroup->deleteObject();

   SAFE_DELETE(gNameDictionary);
   SAFE_DELETE(gIdDictionary);
}

//---------------------------------------------------------------------------

SimObject* findObject(const char* name)
{
   // Play nice with bad code - JDD
   if( !name )
      return NULL;

   SimObject *obj;
   char c = *name;
   if(c == '/')
      return gRootGroup->findObject(name + 1 );
   if(c >= '0' && c <= '9')
   {
      // it's an id group
      const char* temp = name + 1;
      for(;;)
      {
         c = *temp++;
         if(!c)
            return findObject(dAtoi(name));
         else if(c == '/')
         {
            obj = findObject(dAtoi(name));
            if(!obj)
               return NULL;
            return obj->findObject(temp);
         }
      }
   }
   S32 len;

   //-Mat ensure > 0, instead of just != 0 (prevent running through bogus memory on non-NULL-terminated strings)
   for(len = 0; name[len] > 0 && name[len] != '/'; len++)
      ;
   StringTableEntry stName = StringTable->lookupn(name, len);
   if(!stName)
      return NULL;
   obj = gNameDictionary->find(stName);
   if(!name[len])
      return obj;
   if(!obj)
      return NULL;
   return obj->findObject(name + len + 1);
}

SimObject* findObject(SimObjectId id)
{
    return gIdDictionary->find(id);
}

SimGroup *getRootGroup()
{
   return gRootGroup;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#define InstantiateNamedSet(set) g##set = new SimSet; g##set->registerObject(#set); gRootGroup->addObject(g##set); SIMSET_SET_ASSOCIATION((*g##set))
#define InstantiateNamedGroup(set) g##set = new SimGroup; g##set->registerObject(#set); gRootGroup->addObject(g##set); SIMSET_SET_ASSOCIATION((*g##set))

SimDataBlockGroup *gDataBlockGroup;
SimDataBlockGroup *getDataBlockGroup()
{
   return gDataBlockGroup;
}


void init()
{
   initEventQueue();
   initRoot();

   InstantiateNamedSet(ActiveActionMapSet);
   InstantiateNamedSet(GhostAlwaysSet);
   InstantiateNamedGroup(ActionMapGroup);
   InstantiateNamedGroup(ClientGroup);
   InstantiateNamedGroup(GuiGroup);
   InstantiateNamedGroup(GuiDataGroup);
   InstantiateNamedGroup(TCPGroup);
   InstantiateNamedGroup(ClientConnectionGroup);
   InstantiateNamedGroup(ChunkFileGroup);

   InstantiateNamedSet(BehaviorSet);
   InstantiateNamedSet(AchievementSet);

   gDataBlockGroup = new SimDataBlockGroup();
   gDataBlockGroup->registerObject("DataBlockGroup");
   gRootGroup->addObject(gDataBlockGroup);
}

void shutdown()
{
   shutdownRoot();
   shutdownEventQueue();
}

} // Sim Namespace.

SimDataBlockGroup::SimDataBlockGroup()
{
   mLastModifiedKey = 0;
}

S32 QSORT_CALLBACK SimDataBlockGroup::compareModifiedKey(const void* a,const void* b)
{
    return (reinterpret_cast<const SimDataBlock* >(a))->getModifiedKey() -
        (reinterpret_cast<const SimDataBlock*>(b))->getModifiedKey();
}


void SimDataBlockGroup::sort()
{
   if(mLastModifiedKey != SimDataBlock::getNextModifiedKey())
   {
      mLastModifiedKey = SimDataBlock::getNextModifiedKey();
        dQsort(objectList.address(),objectList.size(),sizeof(SimObject *),compareModifiedKey);
   }
}
