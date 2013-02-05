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

#include "platform/Tickable.h"

// The statics
U32 Tickable::smLastTick = 0;
U32 Tickable::smLastTime = 0;
U32 Tickable::smLastDelta = 0;

const U32 Tickable::smTickShift = 4;
const U32 Tickable::smTickMs = ( 1 << smTickShift );
const F32 Tickable::smTickSec = ( F32( Tickable::smTickMs ) / 1000.f );
const U32 Tickable::smTickMask = ( smTickMs - 1 );

//------------------------------------------------------------------------------

Tickable::Tickable() :
    mProcessTick( false )
{
    // We should always start with processing of ticks off!
}

//------------------------------------------------------------------------------

Tickable::~Tickable()
{
    setProcessTicks( false );
}

//------------------------------------------------------------------------------

Vector<Tickable *>& Tickable::getProcessList()
{
   // This helps to avoid the static initialization order fiasco
   static Vector<Tickable *> smProcessList; ///< List of tick controls
   return smProcessList;
}

//------------------------------------------------------------------------------

void Tickable::setProcessTicks( bool tick /* = true  */ )
{
    // Ignore no change.
    if ( tick == mProcessTick )
        return;

    // Update tick flag.
    mProcessTick = tick;

    // Are we processing ticks?
    if ( mProcessTick )
    {
        // Yes, so add to process list.
        getProcessList().push_back( this );
        return;
    }

    // No, so remove from process list.
    for( ProcessListIterator i = getProcessList().begin(); i != getProcessList().end(); i++ )
    {
        if( (*i) == this )
        {
            getProcessList().erase( i );
            return;
        }
    }
}

//------------------------------------------------------------------------------

bool Tickable::advanceTime( U32 timeDelta )
{
    U32 targetTime = smLastTime + timeDelta;
    U32 targetTick = ( targetTime + smTickMask ) & ~smTickMask;
    U32 tickCount = ( targetTick - smLastTick ) >> smTickShift;

    static Vector<Tickable*> safeProcessList;

    // Process ticks.
    if( tickCount )
    {
        // Fetch a copy of the process list.
        safeProcessList = getProcessList();

        for( ; smLastTick != targetTick; smLastTick += smTickMs )
        {
            for( ProcessListIterator i = safeProcessList.begin(); i != safeProcessList.end(); i++ )
            {           
                (*i)->processTick();
            }
        }
    }
    
    smLastDelta = ( smTickMs - ( targetTime & smTickMask ) ) & smTickMask;
    F32 dt = smLastDelta / F32( smTickMs );

    // Fetch a copy of the process list.
    safeProcessList = getProcessList();

    // Interpolate tick.
    for( ProcessListIterator i = safeProcessList.begin(); i != safeProcessList.end(); i++ )
    {
        (*i)->interpolateTick( dt );
    }

    // Fetch a copy of the process list.
    safeProcessList = getProcessList();

    dt = F32( timeDelta ) / 1000.f;	
    for( ProcessListIterator i = safeProcessList.begin(); i != safeProcessList.end(); i++ )
    {
        (*i)->advanceTime( dt );
    }

    smLastTime = targetTime;

    return tickCount != 0;
}
