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

#ifndef _SIM_OBJECT_TIMER_EVENT_H_
#define _SIM_OBJECT_TIMER_EVENT_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

//-----------------------------------------------------------------------------

class SimObjectTimerEvent : public SimEvent
{
public:
    SimObjectTimerEvent( StringTableEntry callbackFunction, const U32 period, const U32 repeat = 0 ) :
        mCallbackFunction( callbackFunction ),
        mPeriod( period ),
        mRepeat( repeat )
    {}
    virtual ~SimObjectTimerEvent() {}

    virtual void process(SimObject *object)
    {
        // Do we have a repeat count?
        if ( mRepeat == 0 || mRepeat > 1 )
        {
            // Calculate the remaining repeats.
            const U32 repeat = mRepeat == 0 ? 0 : mRepeat-1;

            // Create new timer event.
            SimObjectTimerEvent* pEvent = new SimObjectTimerEvent( mCallbackFunction, mPeriod, repeat );

            // Post the event.
            object->setPeriodicTimerID( Sim::postEvent( object, pEvent, Sim::getCurrentTime() + mPeriod ) );
        }

        // Script callback.
        // This *must* be done here in-case the user turns off the timer which would be the one above!
        Con::executef( object, 1, mCallbackFunction );
    }

private:
    StringTableEntry mCallbackFunction;
    U32 mPeriod;
    U32 mRepeat;
};

#endif // _SIM_OBJECT_TIMER_EVENT_H_