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

#ifndef _SIM_CONSOLE_THREAD_EXEC_EVENT_H_
#define _SIM_CONSOLE_THREAD_EXEC_EVENT_H_

#ifndef _SIM_CONSOLE_EVENT_H_
#include "sim/simConsoleEvent.h"
#endif

//-----------------------------------------------------------------------------

/// Used by Con::threadSafeExecute()
struct SimConsoleThreadExecCallback
{
   void *sem;
   const char *retVal;

   SimConsoleThreadExecCallback();
   ~SimConsoleThreadExecCallback();

   void handleCallback(const char *ret);
   const char *waitForResult();
};

class SimConsoleThreadExecEvent : public SimConsoleEvent
{
   SimConsoleThreadExecCallback *cb;

public:
   SimConsoleThreadExecEvent(S32 argc, const char **argv, bool onObject, SimConsoleThreadExecCallback *callback);

   virtual void process(SimObject *object);
};

#endif // _SIM_CONSOLE_THREAD_EXEC_EVENT_H_