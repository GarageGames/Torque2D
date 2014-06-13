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


ConsoleFunctionGroupBegin(ConsoleDoc, "Console self-documentation functions. These output psuedo C++ suitable for feeeding through Doxygen or another auto documentation tool.");

/*! @addtogroup ConsoleOutput Console Output
	@ingroup TorqueScriptFunctions
	@{
*/

/*! dumps all declared console classes to the console.
    This will dump all classes and methods that were registered from within the engine, AND from the console via scripts.
    @param dumpScript Specifies whether or not classes defined in script should be dumped.
    @param dumpEngine Specifies whether or not classes defined in the engine should be dumped.
*/
ConsoleFunctionWithDocs(dumpConsoleClasses, ConsoleVoid, 1, 3, (bool dumpScript = true, bool dumpEngine = true))
{
   bool dumpScript = true;
   if( argc > 1 )
      dumpScript = dAtob( argv[1] );
   
   bool dumpEngine = true;
   if( argc > 2 )
      dumpEngine = dAtob( argv[2] );

   Namespace::dumpClasses( dumpScript, dumpEngine );
}

/*! Dumps all declared console functions to the console.
    This will dump all funtions that were registered from within the engine, AND from the console via scripts.
    @param dumpScript Specifies whether or not functions defined in script should be dumped.
    @param dumpEngine Specifies whether or not functions defined in the engine should be dumped.
    @sa dumpConsoleMethods
*/
ConsoleFunctionWithDocs(dumpConsoleFunctions, ConsoleVoid, 1, 3, (bool dumpScript = true, bool dumpEngine = true))
{
   bool dumpScript = true;
   if( argc > 1 )
      dumpScript = dAtob( argv[1] );
   
   bool dumpEngine = true;
   if( argc > 2 )
      dumpEngine = dAtob( argv[2] );

   Namespace::dumpFunctions( dumpScript, dumpEngine );
}

ConsoleFunctionGroupEnd(ConsoleDoc);

/*! @} */ // group ConsoleOutput
