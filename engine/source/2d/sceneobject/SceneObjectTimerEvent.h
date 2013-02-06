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

#ifndef _SCENE_OBJECT_TIMER_EVENT_H_
#define _SCENE_OBJECT_TIMER_EVENT_H_

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

//-----------------------------------------------------------------------------

class SceneObjectTimerEvent : public SimEvent
{
public:
    SceneObjectTimerEvent( U32 timerPeriod ) : mTimerPeriod(timerPeriod) {}
    virtual  ~SceneObjectTimerEvent() {}

    virtual void process(SimObject *object)
    {
        /// Create new Timer Event.
        SceneObjectTimerEvent* pEvent = new SceneObjectTimerEvent( mTimerPeriod );

        /// Post Event.
        (dynamic_cast<SceneObject*>(object))->setPeriodicTimerID( Sim::postEvent( object, pEvent, Sim::getCurrentTime() + mTimerPeriod ) );

        // Script callback.
        /// This *must* be done here in-case the user turns off the timer which would be the one above!
        Con::executef( object, 1, "onTimer" );
    }

private:
    U32 mTimerPeriod;
};

#endif // _SCENE_OBJECT_TIMER_EVENT_H_