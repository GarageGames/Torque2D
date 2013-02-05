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

#include "sim/simConsoleThreadExecEvent.h"
#include "platform/platform.h"
#include "console/consoleInternal.h"

//-----------------------------------------------------------------------------

extern ExprEvalState gEvalState;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

SimConsoleThreadExecCallback::SimConsoleThreadExecCallback() : retVal(NULL)
{
   sem = Semaphore::createSemaphore(0);
}

SimConsoleThreadExecCallback::~SimConsoleThreadExecCallback()
{
   Semaphore::destroySemaphore(sem);
}

void SimConsoleThreadExecCallback::handleCallback(const char *ret)
{
   retVal = ret;
   Semaphore::releaseSemaphore(sem);
}

const char *SimConsoleThreadExecCallback::waitForResult()
{
   if(Semaphore::acquireSemaphore(sem, true))
   {
      return retVal;
   }

   return NULL;
}

//-----------------------------------------------------------------------------

SimConsoleThreadExecEvent::SimConsoleThreadExecEvent(S32 argc, const char **argv, bool onObject, SimConsoleThreadExecCallback *callback) : 
   SimConsoleEvent(argc, argv, onObject),
   cb(callback)
{
}

void SimConsoleThreadExecEvent::process(SimObject* object)
{
   const char *retVal;
   if(mOnObject)
      retVal = Con::execute(object, mArgc, const_cast<const char**>( mArgv ));
   else
      retVal = Con::execute(mArgc, const_cast<const char**>( mArgv ));

   if(cb)
      cb->handleCallback(retVal);
}
