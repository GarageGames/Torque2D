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
#include "sim/simBase.h"
#include "string/stringTable.h"
#include "console/console.h"
#include "io/fileStream.h"
#include "input/actionMap.h"
#include "io/resource/resourceManager.h"
#include "io/fileObject.h"
#include "console/consoleInternal.h"
#include "debug/profiler.h"
#include "console/consoleTypeValidators.h"
#include "memory/frameAllocator.h"

namespace Sim
{
   // Don't forget to InstantiateNamed* in simManager.cc - DMM
   ImplementNamedSet(ActiveActionMapSet)
   ImplementNamedSet(GhostAlwaysSet)
   ImplementNamedSet(BehaviorSet)
   ImplementNamedSet(AchievementSet);
   ImplementNamedGroup(ActionMapGroup)
   ImplementNamedGroup(ClientGroup)
   ImplementNamedGroup(GuiGroup)
   ImplementNamedGroup(GuiDataGroup)
   ImplementNamedGroup(TCPGroup)

   //groups created on the client
   ImplementNamedGroup(ClientConnectionGroup)
   ImplementNamedGroup(ChunkFileGroup)
}   

//-----------------------------------------------------------------------------

ConsoleFunction( getSimTime, S32, 1, 1, "() Use the getSimTime function to get the time, in ticks, that has elapsed since the engine started executing.\n"
                                                                "@return Returns the time in ticks since the engine was started.\n"
                                                                "@sa getRealTime")
{
   return Sim::getCurrentTime();
}

//-----------------------------------------------------------------------------

ConsoleFunctionGroupBegin ( SimFunctions, "Functions relating to Sim.");

ConsoleFunction(nameToID, S32, 2, 2, "( objectName ) Use the nameToID function to convert an object name into an object ID.\n"
                                                                "This function is a helper for those odd cases where a string will not covert properly, but generally this can be replaced with a statement like: (\"someName\")\n"
                                                                "@param objectName A string containing the name of an object.\n"
                                                                "@return Returns a positive non-zero value if the name corresponds to an object, or a -1 if it does not.")
{
   SimObject *obj = Sim::findObject(argv[1]);
   if(obj)
      return obj->getId();
   else
      return -1;
}

ConsoleFunction(isObject, bool, 2, 2, "( handle ) Use the isObject function to check if the name or ID specified in handle is a valid object.\n"
                                                                "@param handle A name or ID of a possible object.\n"
                                                                "@return Returns true if handle refers to a valid object, false otherwise")
{
   if (!dStrcmp(argv[1], "0") || !dStrcmp(argv[1], ""))
      return false;
   else
      return (Sim::findObject(argv[1]) != NULL);
}

ConsoleFunction(cancel,void,2,2,"( eventID ) Use the cancel function to cancel a previously scheduled event as specified by eventID.\n"
                                                                "@param eventID The numeric ID of a previously scheduled event.\n"
                                                                "@return No return value.\n"
                                                                "@sa getEventTimeLeft, getScheduleDuration, getTimeSinceStart, isEventPending, schedule, obj.schedule")
{
   Sim::cancelEvent(dAtoi(argv[1]));
}

ConsoleFunction(isEventPending, bool, 2, 2, "( eventID ) Use the isEventPending function to see if the event associated with eventID is still pending.\n"
                                                                "When an event passes, the eventID is removed from the event queue, becoming invalid, so there is no discnerable difference between a completed event and a bad event ID.\n"
                                                                "@param eventID The numeric ID of a previously scheduled event.\n"
                                                                "@return Returns true if this event is still outstanding and false if it has passed or eventID is invalid.\n"
                                                                "@sa cancel, getEventTimeLeft, getScheduleDuration, getTimeSinceStart, schedule, obj.schedule")
{
   return Sim::isEventPending(dAtoi(argv[1]));
}

ConsoleFunction(getEventTimeLeft, S32, 2, 2, "( eventID ) Use the getEventTimeLeft function to determine how much time remains until the event specified by eventID occurs.\n"
                                                                "@param eventID The numeric ID of a previously scheduled event.\n"
                                                                "@return Returns a non-zero integer value equal to the milliseconds until the event specified by eventID will occur. However, if eventID is invalid, or the event has passed, this function will return zero.\n"
                                                                "@sa cancel, getScheduleDuration, getTimeSinceStart, isEventPending, schedule, obj.schedule")
{
   return Sim::getEventTimeLeft(dAtoi(argv[1]));
}

ConsoleFunction(getScheduleDuration, S32, 2, 2, " ( eventID ) Use the getScheduleDuration function to determine how long the event associated with eventID was scheduled for.\n"
                                                                "@param eventID The numeric ID of a previously scheduled event.\n"
                                                                "@return Returns a non-zero integer value equal to the milliseconds used in the schedule call that created this event. However, if eventID is invalid, this function will return zero.\n"
                                                                "@sa cancel, getEventTimeLeft, getTimeSinceStart, isEventPending, schedule, obj.schedule")
{
   S32 ret = Sim::getScheduleDuration(dAtoi(argv[1]));
   return ret;
}

ConsoleFunction(getTimeSinceStart, S32, 2, 2, "( eventID ) Use the getTimeSinceStart function to determine how much time has passed since the event specified by eventID was scheduled.\n"
                                                                "@param eventID The numeric ID of a previously scheduled event.\n"
                                                                "@return Returns a non-zero integer value equal to the milliseconds that have passed since this event was scheduled. However, if eventID is invalid, or the event has passed, this function will return zero.\n"
                                                                "@sa cancel, getEventTimeLeft, getScheduleDuration, isEventPending, schedule, obj.schedule")
{
   S32 ret = Sim::getTimeSinceStart(dAtoi(argv[1]));
   return ret;
}

ConsoleFunction(schedule, S32, 4, 0, "( t , objID || 0 , functionName, arg0, ... , argN ) Use the schedule function to schedule functionName to be executed with optional arguments at time t (specified in milliseconds) in the future. This function may be associated with an object ID or not. If it is associated with an object ID and the object is deleted prior to this event occurring, the event is automatically canceled.\n"
                                                                "@param t The time to wait (in milliseconds) before executing functionName.\n"
                                                                "@param objID An optional ID to associate this event with.\n"
                                                                "@param functionName An unadorned (flat) function name.\n"
                                                                "@param arg0, .. , argN - Any number of optional arguments to be passed to functionName.\n"
                                                                "@return Returns a non-zero integer representing the event ID for the scheduled event.\n"
                                                                "@sa cancel, getEventTimeLeft, getScheduleDuration, getTimeSinceStart, isEventPending, obj.schedule")
{
   U32 timeDelta = U32(dAtof(argv[1]));
   SimObject *refObject = Sim::findObject(argv[2]);
   if(!refObject)
   {
      if(argv[2][0] != '0')
         return 0;

      refObject = Sim::getRootGroup();
   }
   SimConsoleEvent *evt = new SimConsoleEvent(argc - 3, argv + 3, false);

   S32 ret = Sim::postEvent(refObject, evt, Sim::getCurrentTime() + timeDelta);
// #ifdef DEBUG
//    Con::printf("ref %s schedule(%s) = %d", argv[2], argv[3], ret);
//    Con::executef(1, "backtrace");
// #endif
   return ret;
}

ConsoleFunctionGroupEnd( SimFunctions );
