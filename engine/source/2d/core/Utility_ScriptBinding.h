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

/*! @defgroup Utility Utility
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Returns the common elements in two sets.
	@param set1 string of space-separated values
	@param set2 string of space-separated values
	@return string of space-separated values
*/
ConsoleFunctionWithDocs( t2dGetCommonElements, ConsoleString, 3, 3, (set1, set2))
{
    if (argc != 3)
    {
        Con::warnf("t2dGetCommonElements - Invalid number of parameters!");
        return NULL;
    }

    // Grab the element count of the first set.
    const U32 elementCount1 = Utility::mGetStringElementCount(argv[1]);

    // Make sure we get at least one number.
    if (elementCount1 < 1)
    {
        return NULL;
    }

    // Grab the element count of the second set.
    const U32 elementCount2 = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount2 < 1)
    {
        return NULL;
    }

    char* buffer = Con::getReturnBuffer(dStrlen(argv[1]) + 1);
    buffer[0] = '\0';
    bool first = true;
    
    // Individual elements assumed to be 1024 or less in length
    char element[1024];

    // Check for common elements
    for (U32 i = 0; i < elementCount1; i++)
    {
        dStrcpy(element,  Utility::mGetStringElement(argv[1], i, true));
        
        for (U32 j = 0; j < elementCount2; j++)
        {
            if (!dStrcmp(element, Utility::mGetStringElement(argv[2], j, true)))
            {
                if (!first)
                    dStrcat(buffer, " ");
                else
                    first = false;

                dStrcat(buffer, element);
            }
        }
    }

    return buffer;
}

/*! @} */ // end group Utility
