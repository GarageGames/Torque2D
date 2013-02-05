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
#define _REMOTE_DEBUGGER_BASE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif

#ifndef _SIM_OBJECT_H_
#include "sim/simObject.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

//-----------------------------------------------------------------------------

#define REMOTE_DEBUGGER_MAX_COMMAND_SIZE    4096
#define REMOTE_DEBUGGER_NAME                "NucleusDebugger"
#define REMOTE_DEBUGGER_RESPONSE_START      REMOTE_DEBUGGER_NAME"_ResponseStart\n\r"
#define REMOTE_DEBUGGER_RESPONSE_END        REMOTE_DEBUGGER_NAME"_ResponseEnd\n\r"

//-----------------------------------------------------------------------------

class CodeBlock;

//-----------------------------------------------------------------------------

class RemoteDebuggerBase : public SimObject, public virtual Tickable
{
    friend class RemoteDebuggerBridge;

private:
    typedef SimObject Parent;
    NetSocket mClientSocket;
    bool mClientAuthenticated;
    char mReceiveCommandBuffer[REMOTE_DEBUGGER_MAX_COMMAND_SIZE];
    U32 mReceiveCommandCursor;

public:
    RemoteDebuggerBase();
    virtual ~RemoteDebuggerBase();

    bool login( const char* pPassword );
    inline bool isClientAuthenticated( void ) { return mClientAuthenticated; }

    /// Virtual functionality.
    virtual bool addCodeBlock( CodeBlock* pCodeBlock );
    virtual bool removeCodeBlock( CodeBlock* pCodeBlock );
    virtual bool pushStackFrame( void );
    virtual bool popStackFrame( void );
    virtual bool executionStopped( CodeBlock *code, U32 lineNumber );

    /// Fetch remote debugger.
    static RemoteDebuggerBase* getRemoteDebugger( void );

    DECLARE_CONOBJECT(RemoteDebuggerBase);

protected:
    virtual void processTick( void );
    virtual void interpolateTick( F32 delta ) {}
    virtual void advanceTime( F32 timeDelta ) {}

    virtual void onClientLogin( void ) {}

private:
    void receiveCommand( const char* pCommand );
    bool sendCommand( const char* pCommand );
};

#endif // _REMOTE_DEBUGGER_BASE_H_
