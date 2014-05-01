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

#ifndef _H_REMOTECOMMANDEVENT
#define _H_REMOTECOMMANDEVENT

class RemoteCommandEvent : public NetEvent
{
public:
   enum {
      MaxRemoteCommandArgs = 20,
      CommandArgsBits = 5
   };

private:
   S32 mArgc;
   char *mArgv[MaxRemoteCommandArgs + 1];
   NetStringHandle mTagv[MaxRemoteCommandArgs + 1];
   static char mBuf[1024];
public:
   RemoteCommandEvent(S32 argc=0, const char **argv=NULL, NetConnection *conn = NULL)
   {
      mArgc = argc;
      for(S32 i = 0; i < argc; i++)
      {
         if(argv[i][0] == StringTagPrefixByte)
         {
            char buffer[256];
            mTagv[i+1] = NetStringHandle(dAtoi(argv[i]+1));
            if(conn)
            {
               dSprintf(buffer + 1, sizeof(buffer) - 1, "%d", conn->getNetSendId(mTagv[i+1]));
               buffer[0] = StringTagPrefixByte;
               mArgv[i+1] = dStrdup(buffer);
            }
         }
         else
            mArgv[i+1] = dStrdup(argv[i]);
      }
   }

#ifdef TORQUE_DEBUG_NET
   const char *getDebugName()
   {
      static char buffer[256];
      dSprintf(buffer, sizeof(buffer), "%s [%s]", getClassName(), gNetStringTable->lookupString(dAtoi(mArgv[1] + 1)) );
      return buffer;
   }
#endif

   ~RemoteCommandEvent()
   {
      for(S32 i = 0; i < mArgc; i++)
         dFree(mArgv[i+1]);
   }

   virtual void pack(NetConnection* conn, BitStream *bstream)
   {
      bstream->writeInt(mArgc, CommandArgsBits);
      // write it out reversed... why?
      // automatic string substitution with later arguments -
      // handled automatically by the system.

      for(S32 i = 0; i < mArgc; i++)
         conn->packString(bstream, mArgv[i+1]);
   }

   virtual void write(NetConnection* conn, BitStream *bstream)
   {
      pack(conn, bstream);
   }

   virtual void unpack(NetConnection* conn, BitStream *bstream)
   {

      mArgc = bstream->readInt(CommandArgsBits);
      // read it out backwards
      for(S32 i = 0; i < mArgc; i++)
      {
         conn->unpackString(bstream, mBuf);
         mArgv[i+1] = dStrdup(mBuf);
      }
   }

   virtual void process(NetConnection *conn)
   {
      static char idBuf[10];

      // de-tag the command name

      for(S32 i = mArgc - 1; i >= 0; i--)
      {
         char *arg = mArgv[i+1];
         if(*arg == StringTagPrefixByte)
         {
            // it's a tag:
            U32 localTag = dAtoi(arg + 1);
            NetStringHandle tag = conn->translateRemoteStringId(localTag);
            NetStringTable::expandString( tag,
                                          mBuf,
                                          sizeof(mBuf),
                                          (mArgc - 1) - i,
                                          (const char**)(mArgv + i + 2) );
            dFree(mArgv[i+1]);
            mArgv[i+1] = dStrdup(mBuf);
         }
      }
      const char *rmtCommandName = dStrchr(mArgv[1], ' ') + 1;
      if(conn->isConnectionToServer())
      {
         dStrcpy(mBuf, "clientCmd");
         dStrcat(mBuf, rmtCommandName);

         char *temp = mArgv[1];
         mArgv[1] = mBuf;

         Con::execute(mArgc, (const char **) mArgv+1);
         mArgv[1] = temp;
      }
      else
      {
         dStrcpy(mBuf, "serverCmd");
         dStrcat(mBuf, rmtCommandName);
         char *temp = mArgv[1];

         dSprintf(idBuf, sizeof(idBuf), "%d", conn->getId());
         mArgv[0] = mBuf;
         mArgv[1] = idBuf;

         Con::execute(mArgc+1, (const char **) mArgv);
         mArgv[1] = temp;
      }
   }

   DECLARE_CONOBJECT(RemoteCommandEvent);
};

#endif
