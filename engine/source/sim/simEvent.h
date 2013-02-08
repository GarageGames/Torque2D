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

#ifndef _SIM_EVENT_H_
#define _SIM_EVENT_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

//---------------------------------------------------------------------------

class SimObject;
typedef U32 SimTime;

//---------------------------------------------------------------------------

/// Represents a queued event in the sim.
///
/// Sim provides an event queue for your convenience, which
/// can be used to schedule events. A few things which use
/// this event queue:
///
///     - The scene lighting system. In order to keep the game
///       responsive while scene lighting occurs, the lighting
///       process is divided into little chunks. In implementation
///       terms, there is a subclass of SimEvent called
///       SceneLightingProcessEvent. The process method of this
///       subclass calls into the lighting code, telling it to
///       perform the next chunk of lighting calculations.
///     - The schedule() console function uses a subclass of
///       SimEvent called SimConsoleEvent to keep track of
///       scheduled events.
class SimEvent
{
  public:
   SimEvent *nextEvent;     ///< Linked list details - pointer to next item in the list.
   SimTime startTime;       ///< When the event was posted.
   SimTime time;            ///< When the event is scheduled to occur.
   U32 sequenceCount;       ///< Unique ID. These are assigned sequentially based on order
                            ///  of addition to the list.
   SimObject *destObject;   ///< Object on which this event will be applied.

   SimEvent() { destObject = NULL; }
   virtual ~SimEvent() {}   ///< Destructor
                            ///
                            /// A dummy virtual destructor is required
                            /// so that subclasses can be deleted properly

   /// Function called when event occurs.
   ///
   /// This is where the meat of your event's implementation goes.
   ///
   /// See any of the subclasses for ideas of what goes in here.
   ///
   /// The event is deleted immediately after processing. If the
   /// object referenced in destObject is deleted, then the event
   /// is not called. The even will be executed unconditionally if
   /// the object referenced is NULL.
   ///
   /// @param   object  Object stored in destObject.
   virtual void process(SimObject *object)=0;
};

#endif // _SIM_EVENT_H_