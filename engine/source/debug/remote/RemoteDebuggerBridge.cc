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
#include "debug/remote/RemoteDebuggerBridge.h"
#endif

#ifndef _REMOTE_DEBUGGER_BASE_H_
#include "debug/remote/RemoteDebuggerBase.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

#ifndef _EVENT_H_
#include "platform/event.h"
#endif

// Script bindings.
#include "debug/remote/RemoteDebuggerBridge_ScriptBinding.h"

//-----------------------------------------------------------------------------

static S32 DebuggerVersion = 0;
static S32 DebuggerPort = 0;
static StringTableEntry DebuggerPassword = NULL;
static NetSocket ServerSocket = NetSocket::INVALID;
static NetSocket ClientSocket = NetSocket::INVALID;
static RemoteDebuggerBridge::ConnectionState BridgeState = RemoteDebuggerBridge::Closed;

//-----------------------------------------------------------------------------

void RemoteDebuggerBridge::processCommandLine( S32 argc, const char **argv )
{
    // Find if the remote debugger is specified on the command-line.
    for( S32 argIndex = 0; argIndex < argc; ++argIndex )
    {
        // Fetch argument.
        const char* pArg = argv[argIndex];

        // Skip if this is this the remote debugger argument.
        if ( dStrnicmp( pArg, REMOTE_DEBUGGER_COMMAND_LINE_ARG, dStrlen( REMOTE_DEBUGGER_COMMAND_LINE_ARG ) ) != 0 )
            continue;

        // Fetch debugger argument count.
        const U32 debuggerArgumentCount = StringUnit::getUnitCount( pArg, REMOTE_DEBUGGER_COMMAND_LINE_ARG_SEPARATOR );
        
        // Are there enough arguments for opening the remote bridge?
        if ( debuggerArgumentCount != 3 && debuggerArgumentCount != 4 )
        {
            // No, so warn.
            Con::warnf( "Found the debugger command-line of '%s' however invalid arguments were specified.  Format is '%s%s<Version>%s<Port>[%s<Password>]'.",
                pArg,
                REMOTE_DEBUGGER_COMMAND_LINE_ARG,
                REMOTE_DEBUGGER_COMMAND_LINE_ARG_SEPARATOR,
                REMOTE_DEBUGGER_COMMAND_LINE_ARG_SEPARATOR,
                REMOTE_DEBUGGER_COMMAND_LINE_ARG_SEPARATOR );
            return;
        }

        // Fetch debugger version.
        const S32 debuggerVersion = dAtoi( StringUnit::getUnit( pArg, 1, REMOTE_DEBUGGER_COMMAND_LINE_ARG_SEPARATOR ) );

        // Fetch port.
        const S32 port = dAtoi( StringUnit::getUnit( pArg, 2, REMOTE_DEBUGGER_COMMAND_LINE_ARG_SEPARATOR ) );

        // Fetch password.
        const char* pPassword = debuggerArgumentCount == 4 ? StringUnit::getUnit( pArg, 3, REMOTE_DEBUGGER_COMMAND_LINE_ARG_SEPARATOR ) : "";

        // Open remote debugger with port and password.
        RemoteDebuggerBridge::open( debuggerVersion, port, pPassword );
    }
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBridge::open( const S32 debuggerVersion, const S32 port, const char* pPassword )
{
    // Sanity!
    AssertFatal( debuggerVersion > 0, "Debugger version must be >0 ." );
    AssertFatal( port > 0, "Debugger port must be >0." );
    AssertFatal( pPassword != NULL, "Debugger password cannot be NULL." );

    // Is the bridge closed?
    if ( BridgeState != RemoteDebuggerBridge::Closed )
    {
        // Yes, so warn.
        Con::warnf( "Cannot start remote debugger sessions as it is already started." );
        return false;
    }

    // Is the debugger version valid?
    if ( debuggerVersion < 1 )
    {
        // No, so warn.
        Con::warnf( "Invalid debugger version '%d'.", debuggerVersion );
        return false;
    }

    // Format debugger version.
    char debuggerClassBuffer[64];
    dSprintf( debuggerClassBuffer, sizeof(debuggerClassBuffer), "RemoteDebugger%d", debuggerVersion );

    // Find debugger.
    AbstractClassRep* pDebuggerRep = AbstractClassRep::findClassRep( debuggerClassBuffer );
    
    // Did we find the debugger?
    if ( pDebuggerRep == NULL )
    {
        // No, so warn.
        Con::warnf( "Failed to find debugger version '%d' (%s).", debuggerVersion, debuggerClassBuffer );
        return false;
    }

    // Create debugger.
    RemoteDebuggerBase* pRemoteDebugger = dynamic_cast<RemoteDebuggerBase*>( pDebuggerRep->create() );

    // Did we create the debugger?
    if ( pRemoteDebugger == NULL )
    {
        // No, so warn.
        Con::warnf( "Failed to create debugger version '%d' (%s).", debuggerVersion, debuggerClassBuffer );
        return false;
    }

    // Register the debugger.
    if ( !pRemoteDebugger->registerObject( REMOTE_DEBUGGER_NAME ) )
    {
        // Failed to register the debugger so warn.
        Con::warnf( "Failed to register debugger version '%d' (%s).", debuggerVersion, debuggerClassBuffer );

        // Delete the remove debugger.
        pRemoteDebugger->deleteObject();
        pRemoteDebugger = NULL;

        return false;
    }

    // Set debugger, its version, port and password.
    DebuggerVersion = debuggerVersion;
    DebuggerPort = port;
    DebuggerPassword = StringTable->insert( pPassword );

    // Set bridge state.
    BridgeState = Open;

    // Open the server socket.
    ServerSocket = Net::openSocket();

    // Did we get a valid server socket?
    if ( ServerSocket == NetSocket::INVALID )
    {
        // No, so warn.
        Con::warnf( "Could not open a remote debugger server socket. " );
        return false;
    }
	 NetAddress address;
	 Net::getIdealListenAddress(&address);
	 address.port = DebuggerPort;

    // Start the server listening.
    Net::bindAddress( address, ServerSocket);
    Net::listen( ServerSocket, 4 );
    Net::setBlocking( ServerSocket, false );

    // Wait for the client connection.
    WaitForClientConnection();

    // Set debugger client socket.
    // NOTE:    This signals the debugger that it can now start receiving commands from the specified socket.
    pRemoteDebugger->mClientSocket = ClientSocket;

    // Wait for the client log-in.
    WaitForClientLogin();

    return true;
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBridge::close( void )
{
    return false;
}

//-----------------------------------------------------------------------------

RemoteDebuggerBridge::ConnectionState RemoteDebuggerBridge::getConnectionState( void )
{
    return BridgeState;
}

//-----------------------------------------------------------------------------

StringTableEntry RemoteDebuggerBridge::getConnectionPassword( void )
{
    return DebuggerPassword;
}

//-----------------------------------------------------------------------------

void RemoteDebuggerBridge::WaitForClientConnection( void )
{
    // Sanity!
    AssertFatal( BridgeState == Open, "Invalid bridge state waiting for connection." );

    // Info.
    Con::warnf( "Debugger version #%d waiting for client to connect on port %d...", DebuggerVersion, DebuggerPort );

    // Wait for connection.
    while( BridgeState == Open )
    {
        // Wait a while.
        Platform::sleep( IdleWaitPeriod );

        NetAddress address;
        NetSocket socket = Net::accept( ServerSocket, &address );

        // Skip if we don't have a valid socket.
        if ( socket == NetSocket::INVALID)
            continue;

        // Info.
        Con::printf( "Client connected to remote debugger (port '%d') at %d (port %d).",
            DebuggerPort,
            socket.getHash(), 
            address.port );

        // Set client socket.
        ClientSocket = socket;

        // Set non-blocking socket.
        Net::setBlocking( ClientSocket, false );

        // Set bridge state.
        BridgeState = Connected;
    }
}

//-----------------------------------------------------------------------------

void RemoteDebuggerBridge::WaitForClientLogin( void )
{
    // Sanity!
    AssertFatal( BridgeState == Connected, "Invalid bridge state waiting for client log-in." );

    // Find remote debugger.
    RemoteDebuggerBase* pRemoteDebugger = Sim::findObject<RemoteDebuggerBase>( REMOTE_DEBUGGER_NAME );

    // Sanity!
    AssertFatal( pRemoteDebugger != NULL, "Could not find remote debugger waiting for client log-in." );

    // Info.
    Con::warnf( "Debugger version #%d waiting for client to authenticate on port %d...", DebuggerVersion, DebuggerPort );

    // Wait until the client has authenticated.
    while( !pRemoteDebugger->isClientAuthenticated() )
    {
        // Process the remote debugger explicitly.
        pRemoteDebugger->processTick();

        // Wait a while.
        Platform::sleep( IdleWaitPeriod );
    }
}
