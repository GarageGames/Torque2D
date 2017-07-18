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

#ifndef _REMOTE_DEBUGGER_BASE_H_
#include "debug/remote/RemoteDebuggerBase.h"
#endif

#ifndef _REMOTE_DEBUGGER_BRIDGE_H_
#include "debug/remote/RemoteDebuggerBridge.h"
#endif

#ifndef _FRAMEALLOCATOR_H_
#include "memory/frameAllocator.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _CODEBLOCK_H_
#include "console/codeBlock.h"
#endif

// Script bindings.
#include "debug/remote/RemoteDebuggerBase_ScriptBinding.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(RemoteDebuggerBase);

//-----------------------------------------------------------------------------

RemoteDebuggerBase::RemoteDebuggerBase() :
    mClientSocket( NetSocket::INVALID ),
    mClientAuthenticated( false ),
    mReceiveCommandCursor( 0 )
{
    // Turn-on tick processing.
    setProcessTicks( true );
}

//-----------------------------------------------------------------------------

RemoteDebuggerBase::~RemoteDebuggerBase()
{
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBase::login( const char* pPassword )
{
    // Set client authentication.
    mClientAuthenticated = ( dStrcmp( RemoteDebuggerBridge::getConnectionPassword(), pPassword ) == 0 );

    // Was the client authenticated?
    if ( mClientAuthenticated )
    {
        // Yes, so perform the client log-in callback.
        onClientLogin();

        // Info.
        Con::printf( "Client authenticated on remote debugger." );
    }
    else
    {
        // No, so warn.
        Con::warnf( "Client failed authentication on remote debugger." );
    }

    return mClientAuthenticated;
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBase::addCodeBlock( CodeBlock* pCodeBlock )
{
    // Finish if client it not authenticated.
    if ( !isClientAuthenticated() )
        return false;

#if 1
        Con::printf( "+ AddCodeBlock: [%s] %s", pCodeBlock->name, pCodeBlock->fullPath );

        //Con::printSeparator();
        //for( U32 breakEntry = 0, breakIndex = 0; breakEntry < pCodeBlock->lineBreakPairCount; breakEntry++, breakIndex += 2 )
        //{
        //    Con::printf( "Line: %d, IP: %d", pCodeBlock->lineBreakPairs[breakIndex] >> 8, pCodeBlock->lineBreakPairs[breakIndex+1] );
        //}
#else
    for( CodeBlock* pCodeBlock = CodeBlock::getCodeBlockList(); pCodeBlock != NULL; pCodeBlock = pCodeBlock->nextFile )
    {
        Con::printf( "%s", pCodeBlock->fullPath );
        Con::printSeparator();
        for( U32 breakEntry = 0, breakIndex = 0; breakEntry < pCodeBlock->lineBreakPairCount; breakEntry++, breakIndex += 2 )
        {

            Con::printf( "Line: %d, IP: %d", pCodeBlock->lineBreakPairs[breakIndex] >> 8, pCodeBlock->lineBreakPairs[breakIndex+1] );
        }
        Con::printSeparator();
    }
#endif

    return true;
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBase::removeCodeBlock( CodeBlock* pCodeBlock )
{
    // Finish if client it not authenticated.
    if ( !isClientAuthenticated() )
        return false;

    Con::printf( "- RemoveCodeBlock: [%s] %s", pCodeBlock->name, pCodeBlock->fullPath );

    return true;
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBase::pushStackFrame( void )
{
    // Finish if client it not authenticated.
    if ( !isClientAuthenticated() )
        return false;

    return true;
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBase::popStackFrame( void )
{
    // Finish if client it not authenticated.
    if ( !isClientAuthenticated() )
        return false;

    return true;
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBase::executionStopped( CodeBlock *code, U32 lineNumber )
{
    // Finish if client it not authenticated.
    if ( !isClientAuthenticated() )
        return false;

    return true;
}

//-----------------------------------------------------------------------------

RemoteDebuggerBase* RemoteDebuggerBase::getRemoteDebugger( void )
{
    return Sim::findObject<RemoteDebuggerBase>( REMOTE_DEBUGGER_NAME );
}

//-----------------------------------------------------------------------------

void RemoteDebuggerBase::processTick( void )
{
    // Finish if the client socket is invalid.
    if ( mClientSocket == NetSocket::INVALID )
        return;
    
    // Calculate read point.
    char* pReadPoint = (mReceiveCommandBuffer + mReceiveCommandCursor);

    // Read from the socket.
    S32 readCount;
    Net::Error readStatus = Net::recv(mClientSocket, (U8*)pReadPoint, sizeof(mReceiveCommandBuffer)-mReceiveCommandCursor, &readCount);

    // Is the connection invalid?
    if ( readCount == 0 || (readStatus != Net::NoError && readStatus != Net::WouldBlock) )
    {
        // Yes, so terminate it.
        setProcessTicks(false);
        RemoteDebuggerBridge::close();
        return;
    }

    // Finish if the read would've blocked.
    if ( readStatus == Net::WouldBlock )
        return;

    // Process last read segment.
    for( S32 index = 0; index < readCount; ++index )
    {
        // Fetch character.
        const char character = pReadPoint[index];

        // Skip if this is not a command termination character.
        if ( character != '\r' && character != '\n' )
            continue;

        // Yes, so terminate command.
        pReadPoint[index] = 0;

        // Receive the command.
        receiveCommand( mReceiveCommandBuffer );

        // Search for any trailing characters after the command.
        for ( S32 trailIndex = index+1; trailIndex < readCount; ++trailIndex )
        {
            // Fetch trail character.
            const char trailCharacter = pReadPoint[trailIndex];

            // Skip if this is a command termination character.
            if ( trailCharacter == '\r' || trailCharacter == '\n' )
                continue;

            // Calculate remaining command characters.
            mReceiveCommandCursor = readCount-trailIndex;

            // Move the trailing characters to the start of the command buffer.
            dMemmove( mReceiveCommandBuffer, pReadPoint+trailIndex, mReceiveCommandCursor );

            // Finish!
            return;
        }

        // Reset receive command cursor.
        mReceiveCommandCursor = 0;

        // Finish!
        return;
    }

    // Move receive cursor.
    mReceiveCommandCursor += readCount;

    // Is the receive buffer full?
    if ( mReceiveCommandCursor >= sizeof(mReceiveCommandBuffer) )
    {
        // Yes, so warn.
        Con::warnf( "%s - Command receive buffer is full!  Resetting buffer.", getClassName() );
        mReceiveCommandCursor = 0;
    }
}

//-----------------------------------------------------------------------------

void RemoteDebuggerBase::receiveCommand( const char* pCommand )
{
    // Sanity!
    AssertFatal( pCommand != NULL, "Remote debugger command cannot be NULL." );

    // Is the client authenticated?
    if ( mClientAuthenticated )
    {
        // Yes, so finish if no command available.
        if ( dStrlen(pCommand) == 0 )
            return;

        // Evaluate the command.
        const char* pReturnValue = Con::evaluatef( pCommand );

        // Send the return value if it exists.
        sendCommand( pReturnValue );
        return;
    }

    // Attempt authentication with the received command.
    sendCommand( login( pCommand ) ? "1" : "0" );
}

//-----------------------------------------------------------------------------

bool RemoteDebuggerBase::sendCommand( const char* pCommand )
{
    // Is the client socket valid?
    if ( mClientSocket == NetSocket::INVALID )
    {
        // No, so warn.
        Con::warnf( "Cannot send command with invalid client socket." );
        return false;
    }

    // Fetch command length.
    const S32 commandLength = dStrlen(pCommand);

    // Calculate required send response size.
    // This size is the original command response plus termination null plus an extra for the newline command termination.
    const S32 requiredSendResponseBufferSize = commandLength+3;

    // Create response buffer.
    FrameTemp<char> sendResponseBuffer( requiredSendResponseBufferSize );

    // Append carriage-return to send command.
    dSprintf( sendResponseBuffer, requiredSendResponseBufferSize, "%s\n\r", pCommand );

    // Send the command.
    Net::send( mClientSocket, (const U8*)REMOTE_DEBUGGER_RESPONSE_START, dStrlen(REMOTE_DEBUGGER_RESPONSE_START) );
    Net::send( mClientSocket, (const U8*)~sendResponseBuffer, requiredSendResponseBufferSize-1 );
    Net::send( mClientSocket, (const U8*)REMOTE_DEBUGGER_RESPONSE_END, dStrlen(REMOTE_DEBUGGER_RESPONSE_END) );

    return true;
}

