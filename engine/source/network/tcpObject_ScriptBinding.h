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

ConsoleMethodGroupBeginWithDocs(TCPObject, SimObject)

/*! Use the send method to send any number of parameters, as strings, one at a time to the agent at the other end of the connection.
    @param ... Any number of arguments, as strings. Each string is sent separately. i.e. The arguments are not concatenated.
    @return No return value
*/
ConsoleMethodWithDocs( TCPObject, send, ConsoleVoid, 3, 0, ( ... ))
{
   for(S32 i = 2; i < argc; i++)
      object->send((const U8 *) argv[i], dStrlen(argv[i]));
}

/*! Use the listen method to allow this TCPObject to accept connections on the specified port.
    @param port A value between 1000 and 65536.
    @return No return value
*/
ConsoleMethodWithDocs( TCPObject, listen, ConsoleVoid, 3, 3, ( port ))
{
   object->listen(U32(dAtoi(argv[2])));
}

/*! Use the connect method to request a connection to a remote agent at the address addr.
    @param addr A string containing an address of the form: ?A.B.C.D:Port?, where A .. B are standard IP numbers between 0 and 255 and Port can be between 1000 and 65536.
    @return No return value.
    @sa disconnect
*/
ConsoleMethodWithDocs( TCPObject, connect, ConsoleVoid, 3, 3, ( addr ))
{
   object->connect(argv[2]);
}


//Luma:	Used to force networking to be opened before connecting... written specifically to handle GPRS/EDGE/3G situation on iPhone, but can be expanded to other platforms too
/*! 
    Connect to the given address, making sure that the connection is open first.
*/
ConsoleMethodWithDocs( TCPObject, openAndConnect, ConsoleVoid, 3, 3, (string addr))
{
   object->openAndConnect(argv[2]);
}

/*! Use the disconnect method to close a previously opened connection without destroying the requesting TCPOpbject.
    This will close any open connection, but not destroy this object. Thus, the object can be used to open a new connection.
    @return No return value.
    @sa connect
*/
ConsoleMethodWithDocs( TCPObject, disconnect, ConsoleVoid, 2, 2, ())
{
   object->disconnect();
}


//Luma:	Encode data before sending via TCP so that only valid URL characters are sent
/*! Performs URLEncoding on a single string.
*/
ConsoleMethodWithDocs(TCPObject, URLEncodeString, ConsoleString, 3, 3, (string data))
{
    U8	*pEncodedString;
    U32	iNewBufferLen;


    pEncodedString = object->URLEncodeData((U8 *)argv[2], dStrlen(argv[2]) + 1, &iNewBufferLen);

    //copy string to return buffer
    char	*pcReturnBuffer = Con::getReturnBuffer(iNewBufferLen);
    dMemcpy(pcReturnBuffer, pEncodedString, iNewBufferLen);

    //free encoded data pointer
    dFree((void *)pEncodedString);
   
    return pcReturnBuffer;
}

ConsoleMethodGroupEndWithDocs(TCPObject)
