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


#include "platformWin32/platformWin32.h"
#include "platform/platform.h"
#include "platform/event.h"
#include <winsock.h>

#if !defined(USE_IPX) || defined(TORQUE_COMPILER_MINGW)
#  define NO_IPX_SUPPORT
#endif

#if !defined(NO_IPX_SUPPORT)
#  include <wsipx.h>
#else
   typedef void* SOCKADDR_IPX;
#endif

#include "console/console.h"
#include "game/gameInterface.h"
#include "io/fileStream.h"

struct NameLookup
{
   U8 hostEntStruct[MAXGETHOSTSTRUCT];
   HANDLE lookupHandle;
   SOCKET socket;
   U16 port;
   NameLookup *nextLookup;
};

static NameLookup *lookupList = NULL;

static Net::Error getLastError();
static S32 defaultPort = 28000;
static S32 netPort = 0;
static SOCKET ipxSocket = INVALID_SOCKET;
static SOCKET udpSocket = INVALID_SOCKET;

enum WinNetConstants {
   MaxConnections = 1024,  ///< Maximum allowed number of connections.
};

HWND winsockWindow = NULL;

static LRESULT PASCAL WinsockProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   U32 error;
   U32 bufLen;
   U32 event;
   SOCKET socket;
   Net::Error err;
   S32 bytesRead;

   static ConnectedNotifyEvent notifyEvent;
   static ConnectedReceiveEvent receiveEvent;
   static ConnectedAcceptEvent acceptEvent;

   switch(message)
   {
      case WM_USER:
         error = WSAGETSELECTERROR(lParam);
         event = WSAGETSELECTEVENT(lParam);
         socket = wParam;
         switch(event)
         {
            case FD_READ:
               err = Net::recv(socket, receiveEvent.data, MaxPacketDataSize, &bytesRead);
               if(err == Net::NoError && bytesRead != 0)
               {
                  receiveEvent.tag = socket;
                  receiveEvent.size = ConnectedReceiveEventHeaderSize + bytesRead;
                  Game->postEvent(receiveEvent);
               }
               break;
            case FD_CONNECT:
               notifyEvent.tag = socket;
               if(error)
                  notifyEvent.state = ConnectedNotifyEvent::ConnectFailed;
               else
                  notifyEvent.state = ConnectedNotifyEvent::Connected;
               Game->postEvent(notifyEvent);
               break;
            case FD_CLOSE:
               // see first if there is anything to read:
               for(;;)
               {
                  err = Net::recv(socket, receiveEvent.data, MaxPacketDataSize, &bytesRead);
                  if(err != Net::NoError || bytesRead == 0)
                     break;

                  receiveEvent.tag = socket;
                  receiveEvent.size = ConnectedReceiveEventHeaderSize + bytesRead;
                  Game->postEvent(receiveEvent);
               }
               notifyEvent.tag = socket;
               notifyEvent.state = ConnectedNotifyEvent::Disconnected;
               Game->postEvent(notifyEvent);
               break;
            case FD_ACCEPT:
               acceptEvent.portTag = socket;
               acceptEvent.connectionTag = Net::accept(socket, &acceptEvent.address);
               if(acceptEvent.connectionTag != InvalidSocket)
               {
                  Net::setBlocking(acceptEvent.connectionTag, false);
                  WSAAsyncSelect(acceptEvent.connectionTag, winsockWindow, WM_USER, FD_READ | FD_CONNECT | FD_CLOSE);
                  Game->postEvent(acceptEvent);
               }
               break;
         }
         break;
      case WM_USER + 1:
         error = WSAGETASYNCERROR(lParam);
         bufLen = WSAGETASYNCBUFLEN(lParam);
         HANDLE handle;
         handle = HANDLE(wParam);
         NameLookup **walk;
         for(walk = &lookupList; *walk; walk = &((*walk)->nextLookup))
         {
            if((*walk)->lookupHandle == handle)
            {
               NameLookup *temp = *walk;
               struct hostent *hp = (struct hostent *) temp->hostEntStruct;

               if(error)
               {
                  notifyEvent.state = ConnectedNotifyEvent::DNSFailed;
                  notifyEvent.tag = temp->socket;
                  ::closesocket(temp->socket);
               }
               else
               {
                  SOCKADDR_IN ipAddr;
                  memcpy(&ipAddr.sin_addr.s_addr, hp->h_addr, sizeof(IN_ADDR));
                  ipAddr.sin_port = temp->port;
                  ipAddr.sin_family = AF_INET;

                  notifyEvent.tag = temp->socket;

                  WSAAsyncSelect(temp->socket, winsockWindow, WM_USER, FD_READ | FD_CONNECT | FD_CLOSE);
                  bool wserr = ::connect(temp->socket, (PSOCKADDR) &ipAddr, sizeof(ipAddr)); // always errors out
                  if (wserr && WSAGetLastError() == WSAEWOULDBLOCK)
                     notifyEvent.state = ConnectedNotifyEvent::DNSResolved;
                  else {
                     Con::printf("Connect error: %d", WSAGetLastError());
                     ::closesocket(temp->socket);
                     notifyEvent.state = ConnectedNotifyEvent::ConnectFailed;
                  }
               }
               Game->postEvent(notifyEvent);

               *walk = temp->nextLookup;
               delete temp;
               break;
            }
         }
         break;
      default:
         return DefWindowProc( hWnd, message, wParam, lParam );
   }
   return 0;
}


static void InitNetWindow()
{
   WNDCLASS wc;
   dMemset(&wc, 0, sizeof(wc));

   wc.style         = 0;
   wc.lpfnWndProc   = WinsockProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = winState.appInstance;
   wc.hIcon         = 0;
   wc.hCursor       = 0;
   wc.hbrBackground = 0;
   wc.lpszMenuName  = 0;
   wc.lpszClassName = dT("WinSockClass");
   RegisterClass( &wc );
   winsockWindow = CreateWindowEx(
      0,
      dT("WinSockClass"),
      dT(""),
      0,
      0, 0, 0, 0,
      NULL, NULL,
      winState.appInstance,
      NULL);
}

bool Net::init()
{
   WSADATA stWSAData;
   InitNetWindow();
   return !WSAStartup(0x0101, &stWSAData);
}

void Net::shutdown()
{
   while(lookupList)
   {
      NameLookup *temp = lookupList;
      lookupList = temp->nextLookup;
      WSACancelAsyncRequest ( temp->lookupHandle );
      delete temp;
   }
   DestroyWindow(winsockWindow);
   closePort();
   WSACleanup();
}

static void netToIPSocketAddress(const NetAddress *address, SOCKADDR_IN *sockAddr)
{
   dMemset(sockAddr, 0, sizeof(SOCKADDR_IN));
   sockAddr->sin_family = AF_INET;
   sockAddr->sin_port = htons(address->port);
   sockAddr->sin_addr.s_net = address->netNum[0];
   sockAddr->sin_addr.s_host = address->netNum[1];
   sockAddr->sin_addr.s_lh = address->netNum[2];
   sockAddr->sin_addr.s_impno = address->netNum[3];
}

static void IPSocketToNetAddress(const SOCKADDR_IN *sockAddr, NetAddress *address)
{
   address->type = NetAddress::IPAddress;
   address->port = htons(sockAddr->sin_port);
   address->netNum[0] = sockAddr->sin_addr.s_net;
   address->netNum[1] = sockAddr->sin_addr.s_host;
   address->netNum[2] = sockAddr->sin_addr.s_lh;
   address->netNum[3] = sockAddr->sin_addr.s_impno;
}

static void netToIPXSocketAddress(const NetAddress *address, SOCKADDR_IPX *sockAddr)
{
#if !defined(NO_IPX_SUPPORT)
   dMemset(sockAddr, 0, sizeof(SOCKADDR_IPX));
   sockAddr->sa_family = AF_INET;
   sockAddr->sa_socket = htons(address->port);
   sockAddr->sa_netnum[0] = address->netNum[0];
   sockAddr->sa_netnum[1] = address->netNum[1];
   sockAddr->sa_netnum[2] = address->netNum[2];
   sockAddr->sa_netnum[3] = address->netNum[3];
   sockAddr->sa_nodenum[0] = address->nodeNum[0];
   sockAddr->sa_nodenum[1] = address->nodeNum[1];
   sockAddr->sa_nodenum[2] = address->nodeNum[2];
   sockAddr->sa_nodenum[3] = address->nodeNum[3];
   sockAddr->sa_nodenum[4] = address->nodeNum[4];
   sockAddr->sa_nodenum[5] = address->nodeNum[5];
#endif
}

static void IPXSocketToNetAddress(const SOCKADDR_IPX *sockAddr, NetAddress *address)
{
#if !defined(NO_IPX_SUPPORT)
   address->type = NetAddress::IPXAddress;
   address->port = htons(sockAddr->sa_socket);
   address->netNum[0]  = sockAddr->sa_netnum[0] ;
   address->netNum[1]  = sockAddr->sa_netnum[1] ;
   address->netNum[2]  = sockAddr->sa_netnum[2] ;
   address->netNum[3]  = sockAddr->sa_netnum[3] ;
   address->nodeNum[0] = sockAddr->sa_nodenum[0];
   address->nodeNum[1] = sockAddr->sa_nodenum[1];
   address->nodeNum[2] = sockAddr->sa_nodenum[2];
   address->nodeNum[3] = sockAddr->sa_nodenum[3];
   address->nodeNum[4] = sockAddr->sa_nodenum[4];
   address->nodeNum[5] = sockAddr->sa_nodenum[5];
#endif
}

NetSocket Net::openListenPort(U16 port)
{
#ifdef TORQUE_ALLOW_JOURNALING
   if(Game->isJournalReading())
   {
      U32 ret;
      Game->journalRead(&ret);
      return NetSocket(ret);
   }
#endif //TORQUE_ALLOW_JOURNALING

   NetSocket sock = openSocket();
   bind(sock, port);
   listen(sock, 4);
   setBlocking(sock, false);
   if(WSAAsyncSelect ( sock, winsockWindow, WM_USER, FD_ACCEPT ))
      Con::printf("Connect error: %d", WSAGetLastError());

#ifdef TORQUE_ALLOW_JOURNALING
   if(Game->isJournalWriting())
      Game->journalWrite(U32(sock));
#endif //TORQUE_ALLOW_JOURNALING

   return sock;
}

NetSocket Net::openConnectTo(const char *addressString)
{
   if(!dStrnicmp(addressString, "ipx:", 4))
      return InvalidSocket;
   if(!dStrnicmp(addressString, "ip:", 3))
      addressString += 3;  // eat off the ip:
   char remoteAddr[256];
   dStrcpy(remoteAddr, addressString);

   char *portString = dStrchr(remoteAddr, ':');

   U16 port;
   if(portString)
   {
      *portString++ = 0;
      port = htons(dAtoi(portString));
   }
   else
      port = htons(defaultPort);

   if(!dStricmp(remoteAddr, "broadcast"))
      return InvalidSocket;

#ifdef TORQUE_ALLOW_JOURNALING
   if(Game->isJournalReading())
   {
      U32 ret;
      Game->journalRead(&ret);
      return NetSocket(ret);
   }
#endif //TORQUE_ALLOW_JOURNALING

   NetSocket sock = openSocket();
   setBlocking(sock, false);

   SOCKADDR_IN ipAddr;

   ipAddr.sin_addr.s_addr = inet_addr(remoteAddr);

   if(ipAddr.sin_addr.s_addr != INADDR_NONE)
   {
      ipAddr.sin_port = port;
      ipAddr.sin_family = AF_INET;
      WSAAsyncSelect(sock, winsockWindow, WM_USER, FD_READ | FD_CONNECT | FD_CLOSE);
      if(::connect(sock, (PSOCKADDR) &ipAddr, sizeof(ipAddr) ) )
      {
         if(WSAGetLastError() != WSAEWOULDBLOCK)
         {
            Con::printf("Connect error: %d", WSAGetLastError());
            ::closesocket(sock);
            sock = InvalidSocket;
         }
      }
   }
   else
   {
      NameLookup *lookup = new NameLookup;
      lookup->socket = sock;
      lookup->port = port;
      lookup->lookupHandle = WSAAsyncGetHostByName (winsockWindow, WM_USER + 1, remoteAddr, (char *) lookup->hostEntStruct, MAXGETHOSTSTRUCT );
      if(!lookup->lookupHandle)
      {
         delete lookup;
         ::closesocket(sock);
         sock = InvalidSocket;
      }
      else
      {
         lookup->nextLookup = lookupList;
         lookupList = lookup;
      }
   }

#ifdef TORQUE_ALLOW_JOURNALING
   if(Game->isJournalWriting())
      Game->journalWrite(U32(sock));
#endif //TORQUE_ALLOW_JOURNALING

   return sock;
}

void Net::closeConnectTo(NetSocket sock)
{
#ifdef TORQUE_ALLOW_JOURNALING
   if(Game->isJournalReading())
      return;
#endif //TORQUE_ALLOW_JOURNALING

   for(NameLookup **walk = &lookupList; *walk; walk = &((*walk)->nextLookup) )
   {
      NameLookup *lookup = *walk;
      if(lookup->socket == sock)
      {
         WSACancelAsyncRequest ( lookup->lookupHandle );
         closesocket(lookup->socket);
         *walk = lookup->nextLookup;
         delete lookup;
         return;
      }
   }
   closesocket(sock);
}

Net::Error Net::sendtoSocket(NetSocket socket, const U8 *buffer, S32 bufferSize)
{
#ifdef TORQUE_ALLOW_JOURNALING
   if(Game->isJournalReading())
   {
      U32 e;
      Game->journalRead(&e);

      return (Net::Error) e;
   }
#endif //TORQUE_ALLOW_JOURNALING

   Net::Error e = send(socket, buffer, bufferSize);

#ifdef TORQUE_ALLOW_JOURNALING
   if(Game->isJournalWriting())
      Game->journalWrite(U32(e));
#endif //TORQUE_ALLOW_JOURNALING
   return e;
}

bool Net::openPort(S32 port)
{
   if(udpSocket != INVALID_SOCKET)
      closesocket(udpSocket);
   if(ipxSocket != INVALID_SOCKET)
      closesocket(ipxSocket);

   udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

#if !defined(NO_IPX_SUPPORT)
   ipxSocket = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
#else
   ipxSocket = INVALID_SOCKET;
#endif

   if(udpSocket != INVALID_SOCKET)
   {
      Net::Error error;
      error = bind(udpSocket, port);
      if(error == NoError)
         error = setBufferSize(udpSocket, 32768);
      if(error == NoError)
         error = setBroadcast(udpSocket, true);
      if(error == NoError)
         error = setBlocking(udpSocket, false);
      if(error == NoError)
         Con::printf("UDP initialized on port %d", port);
      else
      {
         closesocket(udpSocket);
         udpSocket = INVALID_SOCKET;
         Con::printf("Unable to initialize UDP - error %d", error);
      }
   }

#if !defined(NO_IPX_SUPPORT)
   if(ipxSocket != INVALID_SOCKET)
   {
      Net::Error error = NoError;
      SOCKADDR_IPX ipxAddress;
   	memset((char *)&ipxAddress, 0, sizeof(ipxAddress));
	   ipxAddress.sa_family = AF_IPX;
	   ipxAddress.sa_socket = htons(port);
      S32 err = ::bind(ipxSocket, (PSOCKADDR) &ipxAddress, sizeof(ipxAddress));
      if(err)
         error = getLastError();
      if(error == NoError)
         error = setBufferSize(ipxSocket, 32768);
      if(error == NoError)
         error = setBroadcast(ipxSocket, true);
      if(error == NoError)
         error = setBlocking(ipxSocket, false);
      if(error == NoError)
         Con::printf("IPX initialized on port %d", port);
      else
      {
         closesocket(ipxSocket);
         ipxSocket = INVALID_SOCKET;
         Con::printf("Unable to initialize IPX - error %d", error);
      }
   }
#endif
   netPort = port;
   return ipxSocket != INVALID_SOCKET || udpSocket != INVALID_SOCKET;
}

void Net::closePort()
{
   if(ipxSocket != INVALID_SOCKET)
      closesocket(ipxSocket);
   if(udpSocket != INVALID_SOCKET)
      closesocket(udpSocket);
}

Net::Error Net::sendto(const NetAddress *address, const U8 *buffer, S32 bufferSize)
{
#ifdef TORQUE_ALLOW_JOURNALING
   if(Game->isJournalReading())
      return NoError;
#endif //TORQUE_ALLOW_JOURNALING

   if(address->type == NetAddress::IPXAddress)
   {
      SOCKADDR_IPX ipxAddr;
      netToIPXSocketAddress(address, &ipxAddr);
      if(::sendto(ipxSocket, (const char*)buffer, bufferSize, 0,
            (PSOCKADDR) &ipxAddr, sizeof(SOCKADDR_IPX)) == SOCKET_ERROR)
         return getLastError();
      else
         return NoError;
   }
   else
   {
      SOCKADDR_IN ipAddr;
      netToIPSocketAddress(address, &ipAddr);
      if(::sendto(udpSocket, (const char*)buffer, bufferSize, 0,
            (PSOCKADDR) &ipAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
         return getLastError();
      else
         return NoError;
   }
}

void Net::process()
{
   SOCKADDR sa;

   PacketReceiveEvent receiveEvent;
   for(;;)
   {
      S32 addrLen = sizeof(sa);
      S32 bytesRead = SOCKET_ERROR;
      if(udpSocket != INVALID_SOCKET)
         bytesRead = recvfrom(udpSocket, (char *) receiveEvent.data, MaxPacketDataSize, 0, &sa, &addrLen);
      if(bytesRead == SOCKET_ERROR && ipxSocket != INVALID_SOCKET)
      {
         addrLen = sizeof(sa);
         bytesRead = recvfrom(ipxSocket, (char *) receiveEvent.data, MaxPacketDataSize, 0, &sa, &addrLen);
      }

      if(bytesRead == SOCKET_ERROR)
         break;

      if(sa.sa_family == AF_INET)
         IPSocketToNetAddress((SOCKADDR_IN *) &sa, &receiveEvent.sourceAddress);
      else if(sa.sa_family == AF_IPX)
         IPXSocketToNetAddress((SOCKADDR_IPX *) &sa, &receiveEvent.sourceAddress);
      else
         continue;

      NetAddress &na = receiveEvent.sourceAddress;
      if(na.type == NetAddress::IPAddress &&
            na.netNum[0] == 127 &&
            na.netNum[1] == 0 &&
            na.netNum[2] == 0 &&
            na.netNum[3] == 1 &&
            na.port == netPort)
         continue;
      if(bytesRead <= 0)
         continue;
      receiveEvent.size = PacketReceiveEventHeaderSize + bytesRead;
      Game->postEvent(receiveEvent);
   }
}

NetSocket Net::openSocket()
{
   SOCKET retSocket;
   retSocket = socket(AF_INET, SOCK_STREAM, 0);

   if(retSocket == INVALID_SOCKET)
      return InvalidSocket;
   else
      return retSocket;
}

Net::Error Net::closeSocket(NetSocket socket)
{
   if(socket != InvalidSocket)
   {
      if(!closesocket(socket))
         return NoError;
      else
         return getLastError();
   }
   else
      return NotASocket;
}

Net::Error Net::connect(NetSocket socket, const NetAddress *address)
{
   if(address->type != NetAddress::IPAddress)
      return WrongProtocolType;
   SOCKADDR_IN socketAddress;
   netToIPSocketAddress(address, &socketAddress);
   if(!::connect(socket, (PSOCKADDR) &socketAddress, sizeof(socketAddress)))
      return NoError;
   return getLastError();
}

Net::Error Net::listen(NetSocket socket, S32 backlog)
{
   if(!::listen(socket, backlog))
      return NoError;
   return getLastError();
}

NetSocket Net::accept(NetSocket acceptSocket, NetAddress *remoteAddress)
{
   SOCKADDR_IN socketAddress;
   S32 addrLen = sizeof(socketAddress);

   SOCKET retVal = ::accept(acceptSocket, (PSOCKADDR) &socketAddress, &addrLen);
   if(retVal != INVALID_SOCKET)
   {
      IPSocketToNetAddress(&socketAddress, remoteAddress);
      return retVal;
   }
   return InvalidSocket;
}

Net::Error Net::bind(NetSocket socket, U16 port)
{
   S32 error;

   SOCKADDR_IN socketAddress;
   dMemset((char *)&socketAddress, 0, sizeof(socketAddress));
   socketAddress.sin_family = AF_INET;
   // It's entirely possible that there are two NIC cards.
   // We let the user specify which one the server runs on.

   // thanks to [TPG]P1aGu3 for the name
   const char* serverIP = Con::getVariable( "Pref::Net::BindAddress" );
   // serverIP is guaranteed to be non-0.
   AssertFatal( serverIP, "serverIP is NULL!" );

   if( serverIP[0] != '\0' ) {
      // we're not empty
      socketAddress.sin_addr.s_addr = inet_addr( serverIP );

      if( socketAddress.sin_addr.s_addr != INADDR_NONE ) {
	 Con::printf( "Binding server port to %s", serverIP );
      } else {
	 Con::warnf( ConsoleLogEntry::General,
		     "inet_addr() failed for %s while binding!",
		     serverIP );
	 socketAddress.sin_addr.s_addr = INADDR_ANY;
      }

   } else {
      Con::printf( "Binding server port to default IP" );
      socketAddress.sin_addr.s_addr = INADDR_ANY;
   }

   socketAddress.sin_port = htons(port);
   error = ::bind(socket, (PSOCKADDR) &socketAddress, sizeof(socketAddress));

   if(!error)
      return NoError;
   return getLastError();
}

Net::Error Net::setBufferSize(NetSocket socket, S32 bufferSize)
{
   S32 error;
   error = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *) &bufferSize, sizeof(bufferSize));
   if(!error)
      error = setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char *) &bufferSize, sizeof(bufferSize));
   if(!error)
      return NoError;
   return getLastError();
}

Net::Error Net::setBroadcast(NetSocket socket, bool broadcast)
{
   S32 bc = broadcast;
   S32 error = setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char*)&bc, sizeof(bc));
   if(!error)
      return NoError;
   return getLastError();
}

Net::Error Net::setBlocking(NetSocket socket, bool blockingIO)
{
   DWORD notblock = !blockingIO;
   S32 error = ioctlsocket(socket, FIONBIO, &notblock);
   if(!error)
      return NoError;
   return getLastError();
}

Net::Error Net::send(NetSocket socket, const U8 *buffer, S32 bufferSize)
{
   S32 error = ::send(socket, (const char*)buffer, bufferSize, 0);
   if(!error)
      return NoError;
   return getLastError();
}

Net::Error Net::recv(NetSocket socket, U8 *buffer, S32 bufferSize, S32 *bytesRead)
{
   *bytesRead = ::recv(socket, (char*)buffer, bufferSize, 0);
   if(*bytesRead == SOCKET_ERROR)
      return getLastError();
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

bool Net::stringToAddress(const char *addressString, NetAddress *address)
{
   if(dStrnicmp(addressString, "ipx:", 4))
   {
      // assume IP if it doesn't have ipx: at the front.

      if(!dStrnicmp(addressString, "ip:", 3))
         addressString += 3;  // eat off the ip:

      SOCKADDR_IN ipAddr;
      char remoteAddr[256];
      if(strlen(addressString) > 255)
         return false;

      dStrcpy(remoteAddr, addressString);

      char *portString = dStrchr(remoteAddr, ':');
      if(portString)
         *portString++ = 0;

      struct hostent *hp;

      if(!dStricmp(remoteAddr, "broadcast"))
         ipAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
      else
      {
         ipAddr.sin_addr.s_addr = inet_addr(remoteAddr);
         if(ipAddr.sin_addr.s_addr == INADDR_NONE)
         {
            if((hp = gethostbyname(remoteAddr)) == NULL)
               return false;
   	      else
   		      memcpy(&ipAddr.sin_addr.s_addr, hp->h_addr, sizeof(IN_ADDR));
         }
      }
      if(portString)
         ipAddr.sin_port = htons(dAtoi(portString));
      else
         ipAddr.sin_port = htons(defaultPort);
      ipAddr.sin_family = AF_INET;
      IPSocketToNetAddress(&ipAddr, address);
      return true;
   }
   else
   {
      S32 i;
      S32 port;

      address->type = NetAddress::IPXAddress;
      for(i = 0; i < 6; i++)
         address->nodeNum[i] = 0xFF;

      // it's an IPX string
      addressString += 4;
      if(!dStricmp(addressString, "broadcast"))
      {
         address->port = defaultPort;
         return true;
      }
      else if(sscanf(addressString, "broadcast:%d", &port) == 1)
      {
         address->port = port;
         return true;
      }
      else
      {
         S32 nodeNum[6];
         S32 netNum[4];
         S32 count = dSscanf(addressString, "%2x%2x%2x%2x:%2x%2x%2x%2x%2x%2x:%d",
            &netNum[0], &netNum[1], &netNum[2], &netNum[3],
            &nodeNum[0], &nodeNum[1], &nodeNum[2], &nodeNum[3], &nodeNum[4], &nodeNum[5],
            &port);

         if(count == 10)
         {
            port = defaultPort;
            count++;
         }
         if(count != 11)
            return false;

         for(i = 0; i < 6; i++)
            address->nodeNum[i] = nodeNum[i];
         for(i = 0; i < 4; i++)
            address->netNum[i] = netNum[i];
         address->port = port;
         return true;
      }
   }
}

void Net::addressToString(const NetAddress *address, char addressString[256])
{
   if(address->type == NetAddress::IPAddress)
   {
      SOCKADDR_IN ipAddr;
      netToIPSocketAddress(address, &ipAddr);

      if(ipAddr.sin_addr.s_addr == htonl(INADDR_BROADCAST))
         dSprintf(addressString, 256, "IP:Broadcast:%d", ntohs(ipAddr.sin_port));
      else
         dSprintf(addressString, 256, "IP:%d.%d.%d.%d:%d", ipAddr.sin_addr.s_net,
            ipAddr.sin_addr.s_host, ipAddr.sin_addr.s_lh,
            ipAddr.sin_addr.s_impno, ntohs(ipAddr.sin_port));
   }
   else
   {
      dSprintf(addressString, 256, "IPX:%.2X%.2X%.2X%.2X:%.2X%.2X%.2X%.2X%.2X%.2X:%d",
         address->netNum[0], address->netNum[1], address->netNum[2], address->netNum[3],
         address->nodeNum[0], address->nodeNum[1], address->nodeNum[2], address->nodeNum[3], address->nodeNum[4], address->nodeNum[5],
         address->port);
   }
}

Net::Error getLastError()
{
   S32 err = WSAGetLastError();
   switch(err)
   {
      case WSAEWOULDBLOCK:
         return Net::WouldBlock;
      default:
         return Net::UnknownError;
   }
}
