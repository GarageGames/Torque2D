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


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// Advanced hardware functionality.

// now owns the global stuff for FSAA, rather than sceneGraph.

// define the globals first.  don't bracket them, as it doesn't hurt to have
// them defined always...

S32 gFSAASamples = 1; ///< 1==no FSAA.

ConsoleFunctionGroupBegin( MacFSAA, "Mac-specific FSAA control functions.");

/*! @defgroup MacFSAA Mac Anti-Aliasing
	@ingroup TorqueScriptFunctions
	@{
*/

#if defined(TORQUE_OS_OSX)
// new FSAA simple-method handling


//------------------------------------------------------------------------------
/*!
*/
ConsoleFunctionWithDocs( setFSAA, ConsoleVoid, 2, 2, (int))
{
   gFSAASamples = dAtoi(argv[1]);
   if (gFSAASamples<1)
      gFSAASamples = 1;
   else if (gFSAASamples>gGLState.maxFSAASamples)
      gFSAASamples = gGLState.maxFSAASamples;
   dglSetFSAASamples(gFSAASamples);
}

//------------------------------------------------------------------------------
/*! 
*/
ConsoleFunctionWithDocs( increaseFSAA, ConsoleVoid, 1, 1, ())
{
   if (gFSAASamples<gGLState.maxFSAASamples)
   {
      gFSAASamples<<=1;
      dglSetFSAASamples(gFSAASamples);
   }
}

//------------------------------------------------------------------------------
/*! 
*/
ConsoleFunctionWithDocs( decreaseFSAA, ConsoleVoid, 1, 1, ())
{
   if (gFSAASamples>1)
   {
      gFSAASamples>>=1;
      dglSetFSAASamples(gFSAASamples);
   }
}

ConsoleFunctionGroupEnd( MacFSAA );

#endif

/*! @} */ // end group MacFSAA
