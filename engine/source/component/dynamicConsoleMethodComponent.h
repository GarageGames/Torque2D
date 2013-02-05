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

#ifndef _DYNAMIC_CONSOLEMETHOD_COMPONENT_H_
#define _DYNAMIC_CONSOLEMETHOD_COMPONENT_H_

#include "component/simComponent.h"
#include "console/consoleInternal.h"

class DynamicConsoleMethodComponent : public SimComponent
{
   typedef SimComponent Parent;

protected:
   /// Internal callMethod : Actually does component notification and script method execution
   ///  @attention This method does some magic to the argc argv to make Con::execute act properly
   ///   as such it's internal and should not be exposed or used except by this class
   virtual const char* _callMethod( U32 argc, const char *argv[], bool callThis = true );

public:

   /// Call Method format string
   const char* callMethod( S32 argc, const char* methodName, ... );

   /// Call Method
   virtual const char* callMethodArgList( U32 argc, const char *argv[], bool callThis = true );

   // query for console method data
   virtual bool handlesConsoleMethod(const char * fname, S32 * routingId);

   ///
   virtual const char* callOnBehaviors( U32 argc, const char *argv[] );

   DECLARE_CONOBJECT(DynamicConsoleMethodComponent);
};

#endif