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

ConsoleFunctionGroupBegin(NetInterface, "Global control functions for the netInterfaces.");

/*! @addtogroup Network Network
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the allowConnections to enable (or disable) remote connections to the local game server.
    @param enable A boolean value enabling, or disabling connections to the local server.
    @return No return value
*/
ConsoleFunctionWithDocs(allowConnections, ConsoleVoid,2,2, ( enable ))
{
   GNet->setAllowsConnections(dAtob(argv[1]));
}

ConsoleFunctionGroupEnd(NetInterface);

/*! @} */ // end group Network
