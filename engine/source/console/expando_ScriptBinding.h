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

/*! @defgroup PathExpandoFunctions Path Expando
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Expands an expando or relative path into a full path.
*/
ConsoleFunctionWithDocs(expandPath, ConsoleString, 2, 2, (string path))
{
    char* ret = Con::getReturnBuffer( 1024 );
    Con::expandPath(ret, 1024, argv[1]);
    return ret;
}

//-----------------------------------------------------------------------------

/*! Collapses a path into either an expando path or a relative path.
*/
ConsoleFunctionWithDocs(collapsePath, ConsoleString, 2, 2, (string path))
{
    char* ret = Con::getReturnBuffer( 1024 );
    Con::collapsePath(ret, 1024, argv[1]);
    return ret;
}

//-----------------------------------------------------------------------------

/*! Adds the expando to the path.  If it already exists then it is replaced.
*/
ConsoleFunctionWithDocs(addPathExpando, ConsoleVoid, 3, 3, (string expando, string path))
{
    Con::addPathExpando(argv[1], argv[2]);
}

//-----------------------------------------------------------------------------

/*! Removes the specified path expando.
*/
ConsoleFunctionWithDocs(removePathExpando, ConsoleVoid, 2, 2, (string expando))
{
    Con::removePathExpando(argv[1]);
}

//-----------------------------------------------------------------------------

/*! Checks whether the specified path expando is current set or not.
*/
ConsoleFunctionWithDocs(isPathExpando, ConsoleBool, 2, 2, (string expando))
{
    return Con::isPathExpando(argv[1]);
}

//-----------------------------------------------------------------------------

/*! Gets the expando path count.
*/
ConsoleFunctionWithDocs(getPathExpandoCount, ConsoleInt, 1, 1, ())
{
    return Con::getPathExpandoCount();
}

//-----------------------------------------------------------------------------

/*! Gets the path expando key (the expando name) at the specified index.
*/
ConsoleFunctionWithDocs(getPathExpandoKey, ConsoleString, 2, 2, (int expandoIndex))
{
    // Fetch expando index.
    const S32 expandoIndex = dAtoi(argv[1]);

    // Is the expando index in range?
    if ( expandoIndex < 0 || expandoIndex >= (S32) Con::getPathExpandoCount() )
    {
        // No, so warn.
        Con::warnf("getPathExpandoKey() - Expando index of '%d' is out of bounds.  Current expando count is '%d'.",
            expandoIndex,
            Con::getPathExpandoCount() );
        return StringTable->EmptyString;
    }

    // Fetch path expando key.
    return Con::getPathExpandoKey( expandoIndex );
}

//-----------------------------------------------------------------------------

/*! Gets the path expando value (the expando path) at the specified index.
*/
ConsoleFunctionWithDocs(getPathExpandoValue, ConsoleString, 2, 2, (int expandoIndex))
{
    // Fetch expando index.
    const S32 expandoIndex = dAtoi(argv[1]);

    // Is the expando index in range?
    if ( expandoIndex < 0 || expandoIndex >= (S32) Con::getPathExpandoCount() )
    {
        // No, so warn.
        Con::warnf("getPathExpandoValue() - Expando index of '%d' is out of bounds.  Current expando count is '%d'.",
            expandoIndex,
            Con::getPathExpandoCount() );
        return StringTable->EmptyString;
    }

    // Fetch path expando value.
    return Con::getPathExpandoValue( expandoIndex );
}

/*! @} */ // group PathExpandoFunctions
