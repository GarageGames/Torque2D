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

#ifndef _PLATFORM_NETWORK_H_
#define _PLATFORM_NETWORK_H_

#ifndef _TORQUE_TYPES_H_
#include "platform/types.h"
#endif

//-----------------------------------------------------------------------------

struct NetAddress;

typedef S32 NetSocket;
const NetSocket InvalidSocket = -1;

//-----------------------------------------------------------------------------

struct Net
{
   enum Error
   {
      NoError,
      WrongProtocolType,
      InvalidPacketProtocol,
      WouldBlock,
      NotASocket,
      UnknownError
   };

   enum Protocol
   {
      UDPProtocol,
      IPXProtocol,
      TCPProtocol
   };

   static bool init();
   static void shutdown();
   static void process();

   // Unreliable network functions (UDP)
   static bool openPort(S32 connectPort);
   static void closePort();
   static Error sendto(const NetAddress *address, const U8 *buffer, S32 bufferSize);

   // Reliable network functions (TCP)
   static NetSocket openListenPort(U16 port);
   static NetSocket openConnectTo(const char *stringAddress); // does the DNS resolve etc.
   static void closeConnectTo(NetSocket socket);
   static Error sendtoSocket(NetSocket socket, const U8 *buffer, S32 bufferSize);

   static bool compareAddresses(const NetAddress *a1, const NetAddress *a2);
   static bool stringToAddress(const char *addressString, NetAddress *address);
   static void addressToString(const NetAddress *address, char addressString[256]);

   // Lower-level socket-based functions.
   static NetSocket openSocket();
   static Error closeSocket(NetSocket socket);
   static Error connect(NetSocket socket, const NetAddress *address);
   static Error listen(NetSocket socket, S32 maxConcurrentListens);
   static NetSocket accept(NetSocket acceptSocket, NetAddress *remoteAddress);
   static Error bind(NetSocket socket, U16    port);
   static Error setBufferSize(NetSocket socket, S32 bufferSize);
   static Error setBroadcast(NetSocket socket, bool broadcastEnable);
   static Error setBlocking(NetSocket socket, bool blockingIO);
   static Error send(NetSocket socket, const U8 *buffer, S32 bufferSize);
   static Error recv(NetSocket socket, U8 *buffer, S32 bufferSize, S32 *bytesRead);
};

#endif // _PLATFORM_NETWORK_H_
