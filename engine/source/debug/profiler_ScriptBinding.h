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

#ifdef TORQUE_ENABLE_PROFILER

extern Profiler *gProfiler;

ConsoleFunctionGroupBegin( Profiler, "Profiler functionality.");

/*! @defgroup ProfilerFunctions Profiler
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Enables (or disables) a marker for the profiler
    @param markerName The name of the marker to (un)set
    @param enable Boolean value. Set if true, unset if false
    @return No Return Value
*/
ConsoleFunctionWithDocs(profilerMarkerEnable, ConsoleVoid, 3, 3, (string markerName, bool enable))
{
   if(gProfiler)
      gProfiler->enableMarker(argv[1], dAtob(argv[2]));
}

/*! Use the profileEnable function to enable (or disable) engine profiling.
    @param enable A boolean value. If set to true and the engine was compiled with DEBUG specified, engine profiling is enabled, otherwise it is disabled.
    @return No return value.
*/
ConsoleFunctionWithDocs(profilerEnable, ConsoleVoid, 2, 2, ( enable ))
{
   if(gProfiler)
      gProfiler->enable(dAtob(argv[1]));
}

/*! Use the profilerDump function to dump engine profile statistics to the console.
    @return No return value
*/
ConsoleFunctionWithDocs(profilerDump, ConsoleVoid, 1, 1, ())
{
   if(gProfiler)
      gProfiler->dumpToConsole();
}

/*! Dump profiling stats to a file.
*/
ConsoleFunctionWithDocs(profilerDumpToFile, ConsoleVoid, 2, 2, (string filename))
{
   if(gProfiler)
      gProfiler->dumpToFile(argv[1]);
}

/*! Resets the profiler, clearing all of its data.
*/
ConsoleFunctionWithDocs(profilerReset, ConsoleVoid, 1, 1, ())
{
   if(gProfiler)
      gProfiler->reset();
}

ConsoleFunctionGroupEnd( Profiler );

/*! @} */ // group ProfilerFunctions

#endif
