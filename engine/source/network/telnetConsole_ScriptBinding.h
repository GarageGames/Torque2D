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

/*! @defgroup TelnetConsole Telnet Console
	@ingroup TorqueScriptFunctions
	@{
*/

/*! 
    Initialize and open the telnet console.
    @param port    Port to listen on for console connections (0 will shut down listening).
    @param consolePass Password for read/write access to console.
    @param listenPass  Password for read access to console.
    @param remoteEcho  [optional] Enable echoing back to the client, off by default.
    @return No return value
*/
ConsoleFunctionWithDocs( telnetSetParameters, ConsoleVoid, 4, 5, (int port, string consolePass, string listenPass))
{
   if (TelConsole)
   {
      bool remoteEcho = false;
      if( argc == 5 )
         remoteEcho = dAtob( argv[4] );
       TelConsole->setTelnetParameters(dAtoi(argv[1]), argv[2], argv[3], remoteEcho);
   }
}

/*! @} */ // end group TelnetConsole
