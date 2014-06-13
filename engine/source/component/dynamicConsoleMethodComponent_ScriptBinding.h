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

ConsoleMethodGroupBeginWithDocs(DynamicConsoleMethodComponent, SimComponent)

/*! Calls script defined method on first behavior that defines it.
   @param methodName The method's name as a string
   @param argi Any arguments to pass to the method
   @return Any returned value from the behavior that handles the method, or the string 'ERR_CALL_NOT_HANDLED' if 
   no behavior can handle the method.

*/
ConsoleMethodWithDocs( DynamicConsoleMethodComponent, callOnBehaviors, ConsoleString, 3, 64 , (methodName, argi))
{
   // Put this in the correct form of: [method, caller, args...]
   // We do this by just copying the third argument (the method we want to call)
   // over the second (originally the caller ID).
   argv[1] = argv[2];
   return object->callOnBehaviors( argc - 1, argv + 1 );
}

ConsoleMethodGroupEndWithDocs(DynamicConsoleMethodComponent)
