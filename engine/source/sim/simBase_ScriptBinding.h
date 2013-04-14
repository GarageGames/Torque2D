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

#ifndef _SIMBASE_H_
#include "simBase.h"
#endif

//-----------------------------------------------------------------------------

/*! Functions relating to Sim. */
ConsoleFunctionGroupBeginWithDocs( SimFunctions );

//-----------------------------------------------------------------------------

/*! Use the getSimTime function to get the time, in ticks, that has elapsed since the engine started executing.
    @return Returns the time in ticks since the engine was started.
    @sa getRealTime

	\par From Engine
	@copydoc Sim::getCurrentTime
*/
ConsoleFunctionWithDocs( getSimTime, ConsoleInt, 1, 1, () )
{
   return Sim::getCurrentTime();
}

//-----------------------------------------------------------------------------

/*! Use the nameToID function to convert an object name into an object ID.
    This function is a helper for those odd cases where a string will not covert properly, but generally this can be replaced with a statement like: (\"someName\")
    @param objectName A string containing the name of an object.
    @return Returns a positive non-zero value if the name corresponds to an object, or a -1 if it does not.

	\par From Engine
	@copydoc Sim::findObject
*/
ConsoleFunctionWithDocs( nameToID, ConsoleInt, 2, 2, ( string objectName ) )
{
   SimObject *obj = Sim::findObject(argv[1]);
   if(obj)
      return obj->getId();
   else
      return -1;
}

/*! Use the isObject function to check if the name or ID specified in handle is a valid object.
    @param handle A name or ID of a possible object.
    @return Returns true if handle refers to a valid object, false otherwise
*/
ConsoleFunctionWithDocs(isObject, ConsoleBool, 2, 2, ( handle ) )
{
   if (!dStrcmp(argv[1], "0") || !dStrcmp(argv[1], ""))
      return false;
   else
      return (Sim::findObject(argv[1]) != NULL);
}

/*! cancel a previously scheduled event
	@param eventID The numeric ID of a previously scheduled event.
	@return No return value.
	@sa getEventTimeLeft, getScheduleDuration, getTimeSinceStart, isEventPending, schedule, obj.schedule

	@par From Engine
	@copydoc Sim::cancelEvent
*/
ConsoleFunctionWithDocs(cancel,ConsoleVoid,2,2, ( eventID ) )
{
   Sim::cancelEvent(dAtoi(argv[1]));
}

/*!	Use the isEventPending function to see if the event associated with eventID is still pending.
	When an event passes, the eventID is removed from the event queue, becoming invalid, so there is no discnerable difference between a completed event and a bad event ID.
	@param eventID The numeric ID of a previously scheduled event.
	@return Returns true if this event is still outstanding and false if it has passed or eventID is invalid.
	@sa cancel, getEventTimeLeft, getScheduleDuration, getTimeSinceStart, schedule, obj.schedule

	@par From Engine
	@copydoc Sim::isEventPending
*/
ConsoleFunctionWithDocs(isEventPending, ConsoleBool, 2, 2, ( eventID ) )
{
   return Sim::isEventPending(dAtoi(argv[1]));
}

/*!
	Use the getEventTimeLeft function to determine how much time remains until the event specified by eventID occurs.
    @param eventID The numeric ID of a previously scheduled event.
    @return Returns a non-zero integer value equal to the milliseconds until the event specified by eventID will occur. However, if eventID is invalid, or the event has passed, this function will return zero.
    @sa cancel, getScheduleDuration, getTimeSinceStart, isEventPending, schedule, obj.schedule

	@par From Engine
	@copydoc Sim::getEventTimeLeft
*/
ConsoleFunctionWithDocs(getEventTimeLeft, ConsoleInt, 2, 2, ( eventID ) )
{
   return Sim::getEventTimeLeft(dAtoi(argv[1]));
}

/*!
	Use the getScheduleDuration function to determine how long the event associated with eventID was scheduled for.
	@param eventID The numeric ID of a previously scheduled event.
	@return Returns a non-zero integer value equal to the milliseconds used in the schedule call that created this event. However, if eventID is invalid, this function will return zero.
	@sa cancel, getEventTimeLeft, getTimeSinceStart, isEventPending, schedule, obj.schedule

	@par From Engine
	@copydoc Sim::getScheduleDuration
*/
ConsoleFunctionWithDocs(getScheduleDuration, ConsoleInt, 2, 2, ( eventID ) )
{
   S32 ret = Sim::getScheduleDuration(dAtoi(argv[1]));
   return ret;
}

/*!
	Use the getTimeSinceStart function to determine how much time has passed since the event specified by eventID was scheduled.
    @param eventID The numeric ID of a previously scheduled event.
    @return Returns a non-zero integer value equal to the milliseconds that have passed since this event was scheduled. However, if eventID is invalid, or the event has passed, this function will return zero.
    @sa cancel, getEventTimeLeft, getScheduleDuration, isEventPending, schedule, obj.schedule

	@par From Engine
	@copydoc Sim::getTimeSinceStart
*/
ConsoleFunctionWithDocs(getTimeSinceStart, ConsoleInt, 2, 2, ( eventID ) )
{
   S32 ret = Sim::getTimeSinceStart(dAtoi(argv[1]));
   return ret;
}

/*!
	Use the schedule function to schedule functionName to be executed with optional arguments at time t (specified in milliseconds) in the future. This function may be associated with an object ID or not. If it is associated with an object ID and the object is deleted prior to this event occurring, the event is automatically canceled.
    @param t The time to wait (in milliseconds) before executing functionName.
    @param objID An optional ID to associate this event with.
    @param functionName An unadorned (flat) function name.
    @param arg0, .. , argN - Any number of optional arguments to be passed to functionName.
    @return Returns a non-zero integer representing the event ID for the scheduled event.
    @sa cancel, getEventTimeLeft, getScheduleDuration, getTimeSinceStart, isEventPending, obj.schedule

	@par From Engine
	@copydoc Sim::postEvent
*/
ConsoleFunctionWithDocs(schedule, ConsoleInt, 4, 0, ( t , objID || 0 , functionName, arg0, ... , argN ) )
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

//-----------------------------------------------------------------------------

ConsoleFunctionGroupEndWithDocs( SimFunctions );
