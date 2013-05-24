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

/*! @addtogroup FileSystem File System
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Gets the path to the system's temporary directory
    @return Returns the path to the temporary directory as a string
*/
ConsoleFunctionWithDocs(getTemporaryDirectory, ConsoleString, 1, 1, ())
{
   return Platform::getTemporaryDirectory();
}

/*! Generates a temporary filename for use.
    @return Returns the formatted temporary filename for use
*/
ConsoleFunctionWithDocs(getTemporaryFileName, ConsoleString, 1, 1, ())
{
   return Platform::getTemporaryFileName();
}


/*! 
    @return Returns a string to the directory storing the user's data
*/
ConsoleFunctionWithDocs(getUserDataDirectory, ConsoleString, 1, 1, ())
{
   return Platform::getUserDataDirectory();
}

/*! 
    @return Returns the path to the user's home directory.
*/
ConsoleFunctionWithDocs(getUserHomeDirectory, ConsoleString, 1, 1, ())
{
   return Platform::getUserHomeDirectory();
}

/*! @} */ // group FileSystem