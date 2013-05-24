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

ConsoleFunctionGroupBegin( Output, "Functions to output to the console." );

/*! @addtogroup ConsoleOutput Console Output
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the echo function to print messages to the console.
    @param text Any valid text string.
    @param ... Any additional valid text string(s).
    @return No return value.
    @sa error, warn
*/
ConsoleFunctionWithDocs(echo, ConsoleVoid, 2, 0, ( text, [...]* ))
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, argv[i]);

   Con::printf("%s", ret);
   ret[0] = 0;
}

/*! Prints a separator to the console.
*/
ConsoleFunctionWithDocs(echoSeparator, ConsoleVoid, 0, 0, ())
{
    Con::printSeparator();
}

/*! Use the warn function to print warning messages to the console. These messages usually yellow or orange.
    @param text Any valid text string.
    @param ... Any additional valid text string(s).
    @return No return value.
    @sa warn, error
*/
ConsoleFunctionWithDocs(warn, ConsoleVoid, 2, 0, ( text, [...]* ))
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, argv[i]);

   Con::warnf(ConsoleLogEntry::General, "%s", ret);
   ret[0] = 0;
}

/*! Use the error function to print error messages to the console. These messages usually print in red.
    @param text Any valid text string.
    @param ... Any additional valid text string(s).
    @return No return value.
    @sa echo, warn
*/
ConsoleFunctionWithDocs(error, ConsoleVoid, 2, 0, ( text, [...]* ))
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, argv[i]);

   Con::errorf(ConsoleLogEntry::General, "%s", ret);
   ret[0] = 0;
}

/*! Use the collapseEscape function to replace all escape sequences ('xx') with an expanded version ('xx').
    @param text A string, possibly containing escape sequences.
    @return Returns a copy of text with all escape sequences expanded.
    @sa collapseEscape
*/
ConsoleFunctionWithDocs(expandEscape, ConsoleString, 2, 2, ( text ))
{
   TORQUE_UNUSED( argc );
   char *ret = Con::getReturnBuffer(dStrlen(argv[1])*2 + 1);  // worst case situation
   expandEscape(ret, argv[1]);
   return ret;
}

/*! Use the collapseEscape function to replace all escape sequences ('xx') with a collapsed version ('xx').
    @param text A string, possibly containing escape sequences.
    @return Returns a copy of text with all escape sequences converted to an encoding.
    @sa expandEscape
*/
ConsoleFunctionWithDocs(collapseEscape, ConsoleString, 2, 2, ( text ))
{
   TORQUE_UNUSED( argc );
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1);  // worst case situation
   dStrcpy( ret, argv[1] );
   collapseEscape( ret );
   return ret;
}

/*! Use the setLogMode function to set the logging level based on bits that are set in the mode argument.
    This is a general debug method and should be used in all but release cases and perhaps even then.
    @param mode A bitmask enabling various types of logging. See 'Logging Modes' table below.
    @return No return value.
    @sa intputLog
*/
ConsoleFunctionWithDocs(setLogMode, ConsoleVoid, 2, 2, ( mode ))
{
   TORQUE_UNUSED( argc );
   Con::setLogMode(dAtoi(argv[1]));
}

/*! Use the setEchoFileLoads function to enable/disable echoing of file loads (to console).
    This does not completely disable message, but rather adds additional methods when echoing is set to true. File loads will always echo a compile statement if compiling is required, and an exec statement at all times
    @param enable A boolean value. If this value is true, extra information will be dumped to the console when files are loaded.
    @return No return value.
*/
ConsoleFunctionWithDocs(setEchoFileLoads, ConsoleVoid, 2, 2, ( enable ))
{
   TORQUE_UNUSED( argc );
   ResourceManager->setFileNameEcho(dAtob(argv[1]));
}


//----------------------------------------------------------------

/*! Use the quit function to stop the engine and quit to the command line.
    @return No return value
*/
ConsoleFunctionWithDocs(quit, ConsoleVoid, 1, 1, ())
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   Platform::postQuitMessage(0);
}

/*! quitWithErrorMessage(msg)
     - Quit, showing the provided error message. This is equivalent
     to an AssertISV.
    @param Error Message
    @return No return value
*/
ConsoleFunctionWithDocs(quitWithErrorMessage, ConsoleVoid, 2, 2, (msg string))
{
   AssertISV(false, argv[1]);
}

//----------------------------------------------------------------

/*! Open a URL in the user's favorite web browser.
*/
ConsoleFunctionWithDocs( gotoWebPage, ConsoleVoid, 2, 2, ( address ))
{
   TORQUE_UNUSED( argc );
   char* protocolSep = dStrstr(argv[1],"://");

   if( protocolSep != NULL )
   {
      Platform::openWebBrowser(argv[1]);
      return;
   }

   // if we don't see a protocol seperator, then we know that some bullethead
   // sent us a bad url. We'll first check to see if a file inside the sandbox
   // with that name exists, then we'll just glom "http://" onto the front of 
   // the bogus url, and hope for the best.
   
   char urlBuf[2048];
   if(Platform::isFile(argv[1]) || Platform::isDirectory(argv[1]))
   {
      dSprintf(urlBuf, sizeof(urlBuf), "file://%s",argv[1]);
   }
   else
      dSprintf(urlBuf, sizeof(urlBuf), "http://%s",argv[1]);
   
   Platform::openWebBrowser(urlBuf);
   return;
}

//----------------------------------------------------------------

/*! Use the cls function to clear the console output.
    @return No return value
*/
ConsoleFunctionWithDocs( cls, ConsoleVoid, 1, 1, ())
{
    Con::cls();
};

//----------------------------------------------------------------

ConsoleFunctionGroupEnd(Output)

/*! @} */ // group ConsoleOutput
