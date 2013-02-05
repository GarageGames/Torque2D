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

#ifndef _REMOTE_DEBUGGER_BRIDGE_H_
#define _REMOTE_DEBUGGER_BRIDGE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

//-----------------------------------------------------------------------------

#define REMOTE_DEBUGGER_COMMAND_LINE_ARG            "-RemoteDebugger"
#define REMOTE_DEBUGGER_COMMAND_LINE_ARG_SEPARATOR  ","

//-----------------------------------------------------------------------------

class RemoteDebuggerBridge
{
public:
    enum ConnectionState
    {
        // Bridge is closed.
        Closed,

        // Bridge is open.
        Open,

        // Bridge is connection.
        Connected,

        // Bridge-session is logged off.
        LoggedOff,

        // Bridge-session is logged on.
        LoggedOn
    };

    // Constants.
    static const S32 IdleWaitPeriod = 100;

public:
    RemoteDebuggerBridge() {}
    virtual ~RemoteDebuggerBridge() {}

    /// Direct bridge control.
    static void processCommandLine( S32 argc, const char **argv );
    static bool open( const S32 debuggerVersion, const S32 port, const char* pPassword );
    static bool close( void );
    static ConnectionState getConnectionState( void );
    static StringTableEntry getConnectionPassword( void );
    
private:
    static void WaitForClientConnection( void );
    static void WaitForClientLogin( void );
};

#endif // _REMOTE_DEBUGGER_BRIDGE_H_
