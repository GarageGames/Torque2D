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

/*! @defgroup TelnetDebuggerFunctions Telnet Debugger
	@ingroup TorqueScriptFunctions
	@{
*/

/*! 
    Open a debug server port on the specified port, requiring the specified password, 
    and optionally waiting for the debug client to connect.
    @param port The IP port to set the password on.
    @param password The password for this port. Set this to a NULL string to clear the password for the port.
    @return No return value
*/
ConsoleFunctionWithDocs( dbgSetParameters, ConsoleVoid, 3, 4, (int port, string password, bool waitForClient))
{
   if (TelDebugger)
       TelDebugger->setDebugParameters(dAtoi(argv[1]), argv[2], argc > 3 ? dAtob(argv[3]) : false );
}

/*! 
    @return Returns true if a script debugging client is connected else return false.
*/
ConsoleFunctionWithDocs( dbgIsConnected, ConsoleBool, 1, 1, ())
{
   return TelDebugger && TelDebugger->isConnected();
}

/*! 
    Forcibly disconnects any attached script debugging client.
    @return No Return Value
*/
ConsoleFunctionWithDocs( dbgDisconnect, ConsoleVoid, 1, 1, ())
{
   if (TelDebugger)
       TelDebugger->disconnect();
}

/*! @} */ // group TelnetDebuggerFunctions
