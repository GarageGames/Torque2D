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

#ifndef _BEHAVIORCOMPONENT_RAISEEVENT_H_
#define _BEHAVIORCOMPONENT_RAISEEVENT_H_

#ifndef _BEHAVIOR_COMPONENT_H_
#include "behaviorComponent.h"
#endif

//-----------------------------------------------------------------------------

class BehaviorComponentRaiseEvent : public SimEvent
{
public:
    BehaviorComponentRaiseEvent( BehaviorInstance* pOutputBehavior, StringTableEntry pOutputName )
    {
        // Sanity!
        AssertFatal( pOutputBehavior != NULL, "Output behavior cannot be NULL." );
        AssertFatal( pOutputBehavior->isProperlyAdded(), "Output behavior is not registered." );
        AssertFatal( pOutputName != NULL, "Output name cannot be NULL." );

        mpOutputBehavior = pOutputBehavior;
        mpOutputName = pOutputName;
    }
    virtual  ~BehaviorComponentRaiseEvent() {}

    virtual void process(SimObject *object)
    {
        // Fetch behavior component.
        BehaviorComponent* pBehaviorComponent = dynamic_cast<BehaviorComponent*>( object );

        // Sanity!
        AssertFatal( pBehaviorComponent, "BehaviorComponentRaiseEvent() - Could not process scheduled signal raise as the event was not raised on a behavior component." ); 

        // Is the output behavior still around?
        if ( !mpOutputBehavior )
        {
            // No, so warn.
            Con::warnf( "BehaviorComponentRaiseEvent() - Could not raise output '%s' on behavior as the behavior is not longer present.", mpOutputName );
            return;
        }

        // Raise output signal.
        pBehaviorComponent->raise( mpOutputBehavior, mpOutputName );
    }

private:
    SimObjectPtr<BehaviorInstance>  mpOutputBehavior;
    StringTableEntry                mpOutputName;
};

#endif // _BEHAVIORCOMPONENT_RAISEEVENT_H_