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

#include "tcpObject.h"

#include "platform/platform.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "sim/simBase.h"
#include "console/consoleInternal.h"
#include "game/defaultGame.h"
#include "collection/vector.h"

#ifdef TORQUE_OS_IOS
#include "platformiOS/iOSUtil.h"
#endif //TORQUE_OS_IOS

#include "tcpObject_ScriptBinding.h"

TCPObject *TCPObject::table[TCPObject::TableSize] = {0, };

IMPLEMENT_CONOBJECT(TCPObject);

TCPObject *TCPObject::find(NetSocket tag)
{
   for(TCPObject *walk = table[tag.getHash() & TableMask]; walk; walk = walk->mNext)
      if(walk->mTag.getHash() == tag.getHash())
         return walk;
   return NULL;
}

void TCPObject::addToTable(NetSocket newTag)
{
   removeFromTable();
   mTag = newTag;
   mNext = table[mTag.getHash() & TableMask];
   table[mTag.getHash() & TableMask] = this;
}

void TCPObject::removeFromTable()
{
   for(TCPObject **walk = &table[mTag.getHash() & TableMask]; *walk; walk = &((*walk)->mNext))
   {
      if(*walk == this)
      {
         *walk = mNext;
         return;
      }
   }
}

TCPObject::TCPObject()
{
   mBuffer = NULL;
   mBufferSize = 0;
   mPort = 0;
   mTag = NetSocket::INVALID;
   mNext = NULL;
   mState = Disconnected;
}

TCPObject::~TCPObject()
{
   disconnect();
   dFree(mBuffer);
}

bool TCPObject::processArguments(S32 argc, const char **argv)
{
   if(argc == 0)
      return true;
   else if(argc == 1)
   {
      addToTable(NetSocket::fromHandle(dAtoi(argv[0])));
      return true;
   }
   return false;
}

bool TCPObject::onAdd()
{
   if(!Parent::onAdd())
      return false;

   const char *name = getName();

   if(name && name[0] && getClassRep())
   {
      Namespace *parent = getClassRep()->getNameSpace();
      Con::linkNamespaces(parent->mName, name);
      mNameSpace = Con::lookupNamespace(name);

   }

   Sim::getTCPGroup()->addObject(this);

   return true;
}

U32 TCPObject::onReceive(U8 *buffer, U32 bufferLen)
{
   // we got a raw buffer event
   // default action is to split the buffer into lines of text
   // and call processLine on each
   // for any incomplete lines we have mBuffer
   U32 start = 0;
   parseLine(buffer, &start, bufferLen);
   return start;
}

void TCPObject::parseLine(U8 *buffer, U32 *start, U32 bufferLen)
{
   // find the first \n in buffer
   U32 i;
   U8 *line = buffer + *start;

   for(i = *start; i < bufferLen; i++)
      if(buffer[i] == '\n' || buffer[i] == 0)
         break;
   U32 len = i - *start;

   if(i == bufferLen || mBuffer)
   {
      // we've hit the end with no newline
      mBuffer = (U8 *) dRealloc(mBuffer, mBufferSize + len + 1);
      dMemcpy(mBuffer + mBufferSize, line, len);
      mBufferSize += len;
      *start = i;

      // process the line
      if(i != bufferLen)
      {
         mBuffer[mBufferSize] = 0;
         if(mBufferSize && mBuffer[mBufferSize-1] == '\r')
            mBuffer[mBufferSize - 1] = 0;
         U8 *temp = mBuffer;
         mBuffer = 0;
         mBufferSize = 0;

         processLine(temp);
         dFree(temp);
      }
   }
   else if(i != bufferLen)
   {
      line[len] = 0;
      if(len && line[len-1] == '\r')
         line[len-1] = 0;
      processLine(line);
   }
   if(i != bufferLen)
      *start = i + 1;
}

void TCPObject::onConnectionRequest(const NetAddress *addr, U32 connectId)
{
   char idBuf[16];
   char addrBuf[256];
   Net::addressToString(addr, addrBuf);
   dSprintf(idBuf, sizeof(idBuf), "%d", connectId);
   Con::executef(this, 3, "onConnectRequest", addrBuf, idBuf);
}

bool TCPObject::processLine(U8 *line)
{
   Con::executef(this, 2, "onLine", line);
   return true;
}

void TCPObject::onDNSResolved()
{
   mState = DNSResolved;
   Con::executef(this, 1, "onDNSResolved");
}

void TCPObject::onDNSFailed()
{
   mState = Disconnected;
   Con::executef(this, 1, "onDNSFailed");
}

void TCPObject::onConnected()
{
   mState = Connected;
   Con::executef(this, 1, "onConnected");
}

void TCPObject::onConnectFailed()
{
   mState = Disconnected;
   Con::executef(this, 1, "onConnectFailed");
}

void TCPObject::finishLastLine()
{
   if(mBufferSize)
   {
      mBuffer[mBufferSize] = 0;
      processLine(mBuffer);
      dFree(mBuffer);
      mBuffer = 0;
      mBufferSize = 0;
   }
}

bool TCPObject::isBufferEmpty()
{
   return (mBufferSize <= 0);
}

void TCPObject::emptyBuffer()
{
   if(mBufferSize)
   {
      dFree(mBuffer);
      mBuffer = 0;
      mBufferSize = 0;
   }
}

void TCPObject::onDisconnect()
{
   finishLastLine();
   mState = Disconnected;
   Con::executef(this, 1, "onDisconnect");
}

void TCPObject::listen(U16 port)
{
   mState = Listening;
	NetSocket newTag = Net::openListenPort(port);
   addToTable(newTag);
}

void TCPObject::connect(const char *address)
{
   NetSocket newTag = Net::openConnectTo(address);
   addToTable(newTag);
}

//Luma:	Used to force networking to be opened before connecting... written specifically to handle GPRS/EDGE/3G situation on iPhone, but can be expanded to other platforms too
void TCPObject::openAndConnect(const char *address)
{
#ifdef	TORQUE_OS_IOS
    if(IsDeviceiPhone())
    {
        //on the iPhone, we need to make sure that the radio is "open" first, then call the connect CB
        OpeniOSNetworkingAndConnectToTCPObject(this, address);
    }
    else
#endif	//TORQUE_OS_IOS
    {
        //just do straight connect on non-iPhone builds for now
        connect(address);
    }
}

void TCPObject::disconnect()
{
   if( mTag != NetSocket::INVALID ) {
      Net::closeConnectTo(mTag);
   }
   removeFromTable();
   mTag = NetSocket::INVALID;
}


//Luma:	Encode data before sending via TCP so that only valid URL characters are sent
U8	*TCPObject::URLEncodeData(U8 *pData, U32 iDataSize, U32 *piNewDataSize)
{
    U8		szValidChars[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:/.?=_-$(){}~&";
    U8		*pEncodedData;
    U32		iCurEncodedCharacter = 0;



    //set initial new data size
    *piNewDataSize = iDataSize;

    //the maximum size of our encoded data is 3x the base data!
    pEncodedData = (U8 *)dMalloc(sizeof(U8) * iDataSize * 3);

    for(U32 i=0;i<iDataSize;i++)
    {
        if(!dStrchr((char *)szValidChars, pData[i]))
        {
            if(pData[i] == ' ')
            {
                //spaces become '+'
                pEncodedData[iCurEncodedCharacter++] = '+';
            }
            else
            {
                char	szHexVal[3];
                dSprintf(szHexVal, 3, "%X", pData[i]);
                if(dStrlen(szHexVal) == 1)
                {
                    //if only 1 digit was turned into text, we need to manually place the preceeding '0'
                    szHexVal[2] = '\0';
                    szHexVal[1] = szHexVal[0];
                    szHexVal[0] = '0';
                }

                //invalid character... need to encode it!
                pEncodedData[iCurEncodedCharacter++] = '%';
                pEncodedData[iCurEncodedCharacter++] = szHexVal[0];
                pEncodedData[iCurEncodedCharacter++] = szHexVal[1];

                //add on 2 more to the length of the data
                *piNewDataSize += 2;
            }
        }
        else
        {
            //valid character, so leave it!
            pEncodedData[iCurEncodedCharacter++] = pData[i];
        }
    }
    return	pEncodedData;
}

void TCPObject::send(const U8 *buffer, U32 len)
{
   Net::sendtoSocket(mTag, buffer, S32(len));
}

void DefaultGame::processConnectedReceiveEvent(ConnectedReceiveEvent* event )
{
   TCPObject *tcpo = TCPObject::find(event->tag);
   if(!tcpo)
   {
      Con::printf("Got bad connected receive event.");
      return;
   }
   U32 size = U32(event->size - ConnectedReceiveEventHeaderSize);
   U8 *buffer = event->data;

   while(size)
   {
      U32 ret = tcpo->onReceive(buffer, size);
      AssertFatal(ret <= size, "Invalid return size");
      size -= ret;
      buffer += ret;
   }

   //If our buffer now has something in it then it's probably a web socket packet and lets handle it
   if(!tcpo->isBufferEmpty())
   {
      //Copy all the data into a string (may be a quicker way of doing this)
      U8 *data = event->data;

      //Send the packet to script
		bool handled = Con::executef(tcpo, 2, "onPacket", data);

      //If the script did something with it, clear the buffer
      if(handled)
      {
         tcpo->emptyBuffer();
      }
   }

	 Con::executef(tcpo, 1, "onEndReceive");
}

void DefaultGame::processConnectedAcceptEvent( ConnectedAcceptEvent* event )
{
   TCPObject *tcpo = TCPObject::find(event->portTag);
   if(!tcpo)
      return;
   tcpo->onConnectionRequest(&event->address, event->connectionTag.getHandle());
}

void DefaultGame::processConnectedNotifyEvent( ConnectedNotifyEvent* event )
{
   TCPObject *tcpo = TCPObject::find(event->tag);
   if(!tcpo)
      return;
   switch(event->state)
   {
      case ConnectedNotifyEvent::DNSResolved:
         tcpo->onDNSResolved();
         break;
      case ConnectedNotifyEvent::DNSFailed:
         tcpo->onDNSFailed();
         break;
      case ConnectedNotifyEvent::Connected:
         tcpo->onConnected();
         break;
      case ConnectedNotifyEvent::ConnectFailed:
         tcpo->onConnectFailed();
         break;
      case ConnectedNotifyEvent::Disconnected:
         tcpo->onDisconnect();
         break;
   }
}
