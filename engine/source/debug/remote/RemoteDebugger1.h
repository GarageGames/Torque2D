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

#ifndef _REMOTE_DEBUGGER1_H_
#define _REMOTE_DEBUGGER1_H_

#ifndef _REMOTE_DEBUGGER_BASE_H_
#include "debug/remote/RemoteDebuggerBase.h"
#endif

//-----------------------------------------------------------------------------

class RemoteDebugger1 : public RemoteDebuggerBase
{
    typedef RemoteDebuggerBase Parent;

protected:
    // Code breakpoint.
    struct Breakpoint
    {
        StringTableEntry    mCodeFile;
        CodeBlock*          mpCodeBlock;
        U32                 mLineNumber;
        S32                 mHitCount;
        S32                 mCurrentHitCount;
        bool                mClearOnBreak;
        const char*         mpConditionalExpression;

        Breakpoint*         mNextBreakpoint;
    };

private:
    Breakpoint* mBreakPoints;

public:
    RemoteDebugger1();
    virtual ~RemoteDebugger1();

    /// Debugger commands.
    bool getCodeFiles( char* pBuffer, S32 bufferSize );
    const char* getValidBreakpoints( void );
    void addBreakpoint( const char* pCodeFile, const S32 lineNumber, const bool clear, const S32 hitCount, const char* pConditionalExpression );
    void setNextStatementBreak( const bool enabled );

    DECLARE_CONOBJECT(RemoteDebugger1);

protected:
    virtual void onClientLogin( void );
};

#endif // _REMOTE_DEBUGGER1_H_
