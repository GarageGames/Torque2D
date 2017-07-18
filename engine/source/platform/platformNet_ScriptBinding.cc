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

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _PLATFORM_PLATFORMNET_H_
#include "platformNet.h"
#endif

/*! @addtogroup Network Network
	@ingroup TorqueScriptFunctions
	@{
*/

/*!
Determines if a specified address type can be reached.
@param protocol id
@return Returns true on success, false on fail
*/
ConsoleFunctionWithDocs(isAddressTypeAvailable, ConsoleBool, 2, 2, (int addressType))
{
	return Net::isAddressTypeAvailable((NetAddress::Type)dAtoi(argv[1]));
}
/*! 
    Set the network port for the game to use.
    @param The requested port as an integer
    @return Returns true on success, false on fail
*/
ConsoleFunctionWithDocs( setNetPort, ConsoleBool, 2, 3, (int port, bind))
{
	bool bind = true;
	if (argc > 2)
		bind = dAtob(argv[2]);
    return Net::openPort(dAtoi(argv[1]), bind);
}

//-----------------------------------------------------------------------------

/*! 
   @brief Closes the current network port
   @ingroup Networking
*/
ConsoleFunctionWithDocs( closeNetPort, ConsoleVoid, 1, 1, ())
{
   Net::closePort();
}


/*! @} */ // end group Network
