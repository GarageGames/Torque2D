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

#include "platformAndroid/platformAndroid.h"
#include "game/gameInterface.h"

static double startupTime = 0;
void android_StartupTime()
{
	struct timeval  tv;
	gettimeofday(&tv, NULL);
    startupTime = ((tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0);
}

#include <unistd.h>
//--------------------------------------
void Platform::getLocalTime(LocalTime &lt)
{
	struct tm systime;
   time_t long_time;

   /// Get time as long integer.
   time( &long_time );
   /// Convert to local time, thread safe.
   localtime_r( &long_time, &systime );
   
   /// Fill the return struct
   lt.sec      = systime.tm_sec;
   lt.min      = systime.tm_min;
   lt.hour     = systime.tm_hour;
   lt.month    = systime.tm_mon;
   lt.monthday = systime.tm_mday;
   lt.weekday  = systime.tm_wday;
   lt.year     = systime.tm_year;
   lt.yearday  = systime.tm_yday;
   lt.isdst    = systime.tm_isdst;
}   

/// Gets the time in seconds since the Epoch
U32 Platform::getTime()
{
   time_t epoch_time;
   time( &epoch_time );
   return epoch_time;
}   

static double absolute_to_seconds;
static double seconds_to_absolute;
static double absolute_to_millis;
static double millis_to_absolute;

/// Gets the time in milliseconds since some epoch. In this case, system start time.
/// Storing milisec in a U32 overflows every 49.71 days
U32 Platform::getRealMilliseconds()
{
   struct timeval  tv;
   gettimeofday(&tv, NULL);
   double ret = ((tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0) - startupTime;
   return (U32)ret;
}   

U32 Platform::getVirtualMilliseconds()
{
   return platState.currentTime;   
}   

void Platform::advanceTime(U32 delta)
{
   platState.currentTime += delta;
}   

/// Asks the operating system to put the process to sleep for at least ms milliseconds
void Platform::sleep(U32 ms)
{
   // note: this will overflow if you want to sleep for more than 49 days. just so ye know.
   
   //Luma:	Re-enable sleeping... why was it commented out? No sense in that!
   usleep( ms * 1000 );
}

#pragma mark ---- TimeManager ----
//--------------------------------------
static void _AndroidUpdateSleepTicks()
{
   if (platState.backgrounded)
      platState.sleepTicks = Platform::getBackgroundSleepTime();
   else
      platState.sleepTicks = sgTimeManagerProcessInterval;
}

//--------------------------------------
void TimeManager::process()
{
   _AndroidUpdateSleepTicks();
         
   U32 curTime = Platform::getRealMilliseconds(); // GTC returns Milliseconds, FYI.
   S32 elapsedTime = curTime - platState.lastTimeTick;

   if(elapsedTime <= platState.sleepTicks)
   {
      Platform::sleep(platState.sleepTicks - elapsedTime);
   }

   platState.lastTimeTick = Platform::getRealMilliseconds();

   TimeEvent event;
   event.elapsedTime = elapsedTime;
   Game->postEvent(event);
}
