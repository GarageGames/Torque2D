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


ConsoleMethod( RemoteDebugger1, getCodeFiles, const char*, 2, 2,    "() - Get the count of active code files.\n"
                                                                    "@return A count of the active count files." )
{
    // Fetch a return buffer.  This may be excessive but it avoids reallocation code.
    S32 bufferSize = 1024 * 65;
    char* pBuffer = Con::getReturnBuffer( bufferSize );

    // Get the code files.
    if ( !object->getCodeFiles( pBuffer, bufferSize ) )
    {
        // Warn.
        Con::warnf( "Fetching code files resulted in a buffer overflow." );
        return NULL;
    }

    return pBuffer;
}


//-----------------------------------------------------------------------------

ConsoleMethod( RemoteDebugger1, setNextStatementBreak, void, 3, 3,  "(bool enabled) - Set whether to break on next statement or not.\n"
                                                                    "@return No return value." )
{
    // Fetch enabled flag.
    const bool enabled = dAtob(argv[2]);

    object->setNextStatementBreak( enabled );
}

