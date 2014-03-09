//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
// Portions Copyright (c) 2014 James S Urquhart
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

#include "platformEmscripten/platformEmscripten.h"
#include "platform/platform.h"
#include "platform/event.h"
#include "platform/platformNetAsync.unix.h"

#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

// Header clean-up by William Taysom
#include <sys/ioctl.h>

// IPX fixes from William Taysom.
#define IPX_NODE_LEN 6

// for 10.2 compatability...
#ifndef socklen_t
#define socklen_t unsigned int
#endif

#include <stdlib.h>

#include "console/console.h"
#include "game/gameInterface.h"
#include "io/fileStream.h"
#include "collection/vector.h"

static Net::Error getLastError();
static S32 defaultPort = 28000;
static S32 netPort = 0;
static int ipxSocket = InvalidSocket;
static int udpSocket = InvalidSocket;

// local enum for socket states for polled sockets
enum SocketState
{
   InvalidState,
   Connected,
   ConnectionPending,
   Listening,
   NameLookupRequired
};

bool Net::init()
{
   return(true);
}

void Net::shutdown()
{
}

NetSocket Net::openListenPort(U16 port)
{
   Con::errorf("Sockets not implemented on Emscripten");
   return InvalidSocket;
}

NetSocket Net::openConnectTo(const char *addressString)
{
   Con::errorf("Sockets not implemented on Emscripten");
   return InvalidSocket;
}

void Net::closeConnectTo(NetSocket sock)
{
}

Net::Error Net::sendtoSocket(NetSocket socket, const U8 *buffer, int  bufferSize)
{
   return NoError;
}

bool Net::openPort(S32 port)
{
   return false;
}

void Net::closePort()
{
}

Net::Error Net::sendto(const NetAddress *address, const U8 *buffer, S32  bufferSize)
{
   return NoError;
}

void Net::process()
{
}

NetSocket Net::openSocket()
{
   return InvalidSocket;
}

Net::Error Net::closeSocket(NetSocket socket)
{
   return NoError;
}

Net::Error Net::connect(NetSocket socket, const NetAddress *address)
{
   return NoError;
}

Net::Error Net::listen(NetSocket socket, S32 backlog)
{
   return NoError;
}

NetSocket Net::accept(NetSocket acceptSocket, NetAddress *remoteAddress)
{
   return InvalidSocket;
}

Net::Error Net::bind(NetSocket socket, U16 port)
{
   return NoError;
}

Net::Error Net::setBufferSize(NetSocket socket, S32 bufferSize)
{
   return NoError;
}

Net::Error Net::setBroadcast(NetSocket socket, bool broadcast)
{
   return NoError;
}

Net::Error Net::setBlocking(NetSocket socket, bool blockingIO)
{
   return NoError;
}

Net::Error Net::send(NetSocket socket, const U8 *buffer, S32 bufferSize)
{
   return NoError;
}

Net::Error Net::recv(NetSocket socket, U8 *buffer, S32 bufferSize, S32  *bytesRead)
{
   return NoError;
}

bool Net::compareAddresses(const NetAddress *a1, const NetAddress *a2)
{
   if((a1->type != a2->type)  ||
      (*((U32 *)a1->netNum) != *((U32 *)a2->netNum)) ||
      (a1->port != a2->port))
      return false;

   if(a1->type == NetAddress::IPAddress)
      return true;
   for(S32 i = 0; i < 6; i++)
      if(a1->nodeNum[i] != a2->nodeNum[i])
         return false;
   return true;
}

bool Net::stringToAddress(const char *addressString, NetAddress  *address)
{
   return false;
}

void Net::addressToString(const NetAddress *address, char  addressString[256])
{
   addressString[0] = '\0';
}

Net::Error getLastError()
{
   return Net::UnknownError;
}
