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

#ifndef _NETWORK_PROCESS_LIST_H_
#include "network/networkProcessList.h"
#endif

//--------------------------------------------------------------------------

NetworkProcessList::NetworkProcessList( const bool isServer )
{
   mDirty = false;
   mCurrentTag = 0;
   mLastTick = 0;
   mLastTime = 0;
   mLastDelta = 0;
   mIsServer = isServer;
}

//--------------------------------------------------------------------------

bool NetworkProcessList::advanceTime( const SimTime& timeDelta )
{
    const U32 TickShift = 5;
    const U32 TickMs = 1 << TickShift;
    //const F32 TickSec = (F32)(TickMs) / 1000.0f;
    const U32 TickMask =   (TickMs - 1);
  

    SimTime targetTime = mLastTime + timeDelta;
    SimTime targetTick = targetTime & ~TickMask;

    bool ret = mLastTick != targetTick;

    mLastTime = targetTime;
    return ret;
}
