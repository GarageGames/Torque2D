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
#include "network/connectionProtocol.h"
#include "io/bitStream.h"
#include "sim/simBase.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "network/netConnection.h"
#include "network/netObject.h"
#include "game/gameConnection.h"
#include "network/serverQuery.h"
#include "network/RemoteCommandEvent.h"

#include "RemoteCommandEvent_ScriptBinding.h"

//----------------------------------------------------------------
// remote procedure call console functions
//----------------------------------------------------------------

char RemoteCommandEvent::mBuf[1024];

IMPLEMENT_CO_NETEVENT_V1(RemoteCommandEvent);

static void sendRemoteCommand(NetConnection *conn, S32 argc, const char **argv)
{
   if(U8(argv[0][0]) != StringTagPrefixByte)
   {
      Con::errorf(ConsoleLogEntry::Script, "Remote Command Error - command must be a tag.");
      return;
   }
   S32 i;
   for(i = argc - 1; i >= 0; i--)
   {
      if(argv[i][0] != 0)
         break;
      argc = i;
   }
   for(i = 0; i < argc; i++)
      conn->validateSendString(argv[i]);
   RemoteCommandEvent *cevt = new RemoteCommandEvent(argc, argv, conn);
   conn->postNetEvent(cevt);
}
