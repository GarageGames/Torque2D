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

#ifndef _SIM_CONSOLE_EVENT_H_
#define _SIM_CONSOLE_EVENT_H_

#ifndef _SIM_EVENT_H_
#include "sim/simEvent.h"
#endif

//-----------------------------------------------------------------------------

/// Implementation of schedule() function.
///
/// This allows you to set a console function to be
/// called at some point in the future.

class SimConsoleEvent : public SimEvent
{
protected:
   S32 mArgc;
   char **mArgv;
   bool mOnObject;
  public:

   /// Constructor
   ///
   /// Pass the arguments of a function call, optionally on an object.
   ///
   /// The object for the call to be executed on is specified by setting
   /// onObject and storing a reference to the object in destObject. If
   /// onObject is false, you don't need to store anything into destObject.
   ///
   /// The parameters here are passed unmodified to Con::execute() at the
   /// time of the event.
   ///
   /// @see Con::execute(S32 argc, const char *argv[])
   /// @see Con::execute(SimObject *object, S32 argc, const char *argv[])
   SimConsoleEvent(S32 argc, const char **argv, bool onObject);

   ~SimConsoleEvent();
   virtual void process(SimObject *object);
};

#endif // _SIM_CONSOLE_EVENT_H_