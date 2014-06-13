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

#include "platform/platform.h"
#include "sim/simBase.h"
#include "platform/event.h"
#include "network/netConnection.h"
#include "io/bitStream.h"
#include "network/netObject.h"

class SimpleMessageEvent : public NetEvent
{
   typedef NetEvent Parent;
   char *msg;
public:
   SimpleMessageEvent(const char *message = NULL)
      {
         if(message)
            msg = dStrdup(message);
         else
            msg = NULL;
      }
   ~SimpleMessageEvent()
      { dFree(msg); }

   virtual void pack(NetConnection* /*ps*/, BitStream *bstream)
      { bstream->writeString(msg); }
   virtual void write(NetConnection*, BitStream *bstream)
      { bstream->writeString(msg); }
   virtual void unpack(NetConnection* /*ps*/, BitStream *bstream)
      { char buf[256]; bstream->readString(buf); msg = dStrdup(buf); }
   virtual void process(NetConnection *)
      { Con::printf("RMSG %d  %s", mSourceId, msg); }

   DECLARE_CONOBJECT(SimpleMessageEvent);
};

IMPLEMENT_CO_NETEVENT_V1(SimpleMessageEvent);

class SimpleNetObject : public NetObject
{
   typedef NetObject Parent;
public:
   char message[256];
   SimpleNetObject()
   {
      mNetFlags.set(ScopeAlways | Ghostable);
      dStrcpy(message, "Hello World!");
   }
   U32 packUpdate(NetConnection *conn, U32 mask, BitStream *stream)
   {
      stream->writeString(message);
      return 0;
   }
   void unpackUpdate(NetConnection *conn, BitStream *stream)
   {
      stream->readString(message);
      Con::printf("Got message: %s", message);
   }
   void setMessage(const char *msg)
   {
      setMaskBits(1);
      dStrcpy(message, msg);
   }

   DECLARE_CONOBJECT(SimpleNetObject);
};

IMPLEMENT_CO_NETOBJECT_V1(SimpleNetObject);

/*! Sets the object message to the provide string
	@param msg The string you wish as the message
	@return No return value
*/
ConsoleMethodWithDocs( SimpleNetObject, setMessage, void, 3, 3, (string msg)) 
{
   object->setMessage(argv[2]);
}

/*! Send a SimpleNetObject message to the specified connection
*/

ConsoleFunctionWithDocs( msg, void, 3, 3, (NetConnection id, string message))
{
   NetConnection *con = (NetConnection *) Sim::findObject(argv[1]);
   if(con)
      con->postNetEvent(new SimpleMessageEvent(argv[2]));
}
