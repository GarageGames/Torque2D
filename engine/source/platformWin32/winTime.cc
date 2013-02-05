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

#include "platformWin32/platformWin32.h"
#include "time.h"

void Platform::sleep(U32 ms)
{
   Sleep(ms);
}

//--------------------------------------
void Platform::getLocalTime(LocalTime &lt)
{
   struct tm *systime;
   time_t long_time;

   time( &long_time );                // Get time as long integer.
   systime = localtime( &long_time ); // Convert to local time.

   lt.sec      = systime->tm_sec;
   lt.min      = systime->tm_min;
   lt.hour     = systime->tm_hour;
   lt.month    = systime->tm_mon;
   lt.monthday = systime->tm_mday;
   lt.weekday  = systime->tm_wday;
   lt.year     = systime->tm_year;
   lt.yearday  = systime->tm_yday;
   lt.isdst    = systime->tm_isdst;
}

U32 Platform::getTime()
{
   time_t long_time;
   time( &long_time );
   return (U32)long_time;
}


U32 Platform::getRealMilliseconds()
{
   return GetTickCount();
}

U32 Platform::getVirtualMilliseconds()
{
   return winState.currentTime;
}

void Platform::advanceTime(U32 delta)
{
   winState.currentTime += delta;
}

