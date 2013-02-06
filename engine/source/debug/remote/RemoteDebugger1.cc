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
#include "debug/remote/RemoteDebugger1.h"
#endif

#ifndef _COMPILER_H_
#include "console/compiler.h"
#endif

// Script bindings.
#include "debug/remote/RemoteDebugger1_ScriptBinding.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(RemoteDebugger1);

//-----------------------------------------------------------------------------

RemoteDebugger1::RemoteDebugger1() :
    mBreakPoints( NULL )
{    
}

//-----------------------------------------------------------------------------

RemoteDebugger1::~RemoteDebugger1()
{
}

//-----------------------------------------------------------------------------

bool RemoteDebugger1::getCodeFiles( char* pBuffer, S32 bufferSize )
{
    // Iterate all code-blocks.
    for( CodeBlock* pCodeBlock = CodeBlock::getCodeBlockList(); pCodeBlock != NULL; pCodeBlock = pCodeBlock->nextFile )
    {
        // Finish if out of buffer.
        if ( bufferSize <= 0 )
            return false;

        // Format code file.
        S32 offset = dSprintf( pBuffer, bufferSize, pCodeBlock->nextFile == NULL ? "%s" : "%s,", pCodeBlock->fullPath );
        pBuffer += offset;
        bufferSize -= offset;           
    }

    // Finish if out of buffer.
    if ( bufferSize < 2 )
        return false;

    // Terminate return buffer.
    dSprintf( pBuffer, bufferSize, "\n" );

    return true;
}

//-----------------------------------------------------------------------------

const char* RemoteDebugger1::getValidBreakpoints( void )
{
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

    return NULL;
}

//-----------------------------------------------------------------------------

void RemoteDebugger1::setNextStatementBreak( const bool enabled )
{
   if ( enabled )
   {
      // Apply breaks on all the code blocks.
      for(CodeBlock *walk = CodeBlock::getCodeBlockList(); walk; walk = walk->nextFile)
         walk->setAllBreaks();

      //mBreakOnNextStatement = true;
   } 
   else if ( !enabled )
   {
      // Clear all the breaks on the code-blocks 
      // then go reapply the breakpoints.
      for(CodeBlock *walk = CodeBlock::getCodeBlockList(); walk; walk = walk->nextFile)
         walk->clearAllBreaks();

      //mBreakOnNextStatement = false;
   }
}

//-----------------------------------------------------------------------------

void RemoteDebugger1::onClientLogin( void )
{
    // Call parent.
    Parent::onClientLogin();
}

