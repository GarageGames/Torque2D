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

/*! Open the remote debugger.
    @param debuggerVersion The debugger version required.
    @param port The port the remote debugger should be listening for a debugging session on.
    @param password The optional password the remote debugger should use for a debugging session authentication.
    @return Whether the remote debugger was opened or not.
*/
ConsoleFunctionWithDocs( OpenRemoteDebugger, ConsoleBool, 4, 4, ( int debuggerVersion, int port, string password ))
{
    // Fetch debugger version.
    const S32 debuggerVersion = dAtoi(argv[1]);

    // Fetch port.
    const S32 port = dAtoi(argv[2]);

    // Fetch password.
    const char* pPassword = argv[3];

    // Open remote debugger with port and password.
    return RemoteDebuggerBridge::open( debuggerVersion, port, pPassword );
}
