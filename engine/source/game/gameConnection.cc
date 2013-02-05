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
#include "console/consoleTypes.h"
#include "sim/simBase.h"
#include "io/bitStream.h"
#include "game/gameConnection.h"
#include "io/resource/resourceManager.h"

//----------------------------------------------------------------------------
#define MAX_MOVE_PACKET_SENDS 4

#define ControlRequestTime 5000

const U32 GameConnection::CurrentProtocolVersion = 12;
const U32 GameConnection::MinRequiredProtocolVersion = 12;

//----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GameConnection);
S32 GameConnection::mLagThresholdMS = 0;

//----------------------------------------------------------------------------
GameConnection::GameConnection()
{
   mLagging = false;
   mAuthInfo = NULL;
   mConnectArgc = 0;
   for(U32 i = 0; i < MaxConnectArgs; i++)
      mConnectArgv[i] = 0;

   mJoinPassword = NULL;

   mDisconnectReason[0] = 0;
   
}

GameConnection::~GameConnection()
{
   delete mAuthInfo;
   for(U32 i = 0; i < mConnectArgc; i++)
      dFree(mConnectArgv[i]);
   dFree(mJoinPassword);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

bool GameConnection::canRemoteCreate()
{
   return true;
}

void GameConnection::setConnectArgs(U32 argc, const char **argv)
{
   if(argc > MaxConnectArgs)
      argc = MaxConnectArgs;
   mConnectArgc = argc;
   for(U32 i = 0; i < mConnectArgc; i++)
      mConnectArgv[i] = dStrdup(argv[i]);
}

void GameConnection::setJoinPassword(const char *password)
{
   mJoinPassword = dStrdup(password);
}

ConsoleMethod(GameConnection, setJoinPassword, void, 3, 3, "( password ) Use the setJoinPassword method to set the password required to connect to this server-side GameConnection.\n"
                                                                "Pass a NULL string to clear the password.\n"
                                                                "@param password A string representing the case insensitive password to use for this server-side GameConnection.\n"
                                                                "@return No return value.\n"
                                                                "@sa setConnectArgs")
{
   object->setJoinPassword(argv[2]);
}

ConsoleMethod(GameConnection, setConnectArgs, void, 3, 17, "( name [ , arg1 , ... , arg15 ] ) Use the setConnectArgs method to set the connection arguments for this client-side GameConnection. These values will be passed to the server upon establishing a connection.\n"
                                                                "@param name Generally, the first argument is the name of the player.\n"
                                                                "@param arg1 ... , arg15 - 15 additional arguments may be passed.\n"
                                                                "@return No return value.\n"
                                                                "@sa setJoinPassword")
{
   object->setConnectArgs(argc - 2, argv + 2);
}

void GameConnection::onTimedOut()
{
   if(isConnectionToServer())
   {
      Con::printf("Connection to server timed out");
      Con::executef(this, 1, "onConnectionTimedOut");
   }
   else
   {
      Con::printf("Client %d timed out.", getId());
      setDisconnectReason("TimedOut");
   }

}

void GameConnection::onConnectionEstablished(bool isInitiator)
{
   if(isInitiator)
   {
      setGhostFrom(false);
      setGhostTo(true);
      setSendingEvents(true);
      setTranslatesStrings(true);
      setIsConnectionToServer();
      mServerConnection = this;
      Con::printf("Connection established %d", getId());
      Con::executef(this, 1, "onConnectionAccepted");
   }
   else
   {
      setGhostFrom(true);
      setGhostTo(false);
      setSendingEvents(true);
      setTranslatesStrings(true);
      Sim::getClientGroup()->addObject(this);

      const char *argv[MaxConnectArgs + 2];
      argv[0] = "onConnect";
      for(U32 i = 0; i < mConnectArgc; i++)
         argv[i + 2] = mConnectArgv[i];
      Con::execute(this, mConnectArgc + 2, argv);
   }
}

void GameConnection::onConnectTimedOut()
{
   Con::executef(this, 1, "onConnectRequestTimedOut");
}

void GameConnection::onDisconnect(const char *reason)
{
   if(isConnectionToServer())
   {
      Con::printf("Connection with server lost.");
      Con::executef(this, 2, "onConnectionDropped", reason);
   }
   else
   {
      Con::printf("Client %d disconnected.", getId());
      setDisconnectReason(reason);
   }
}

void GameConnection::onConnectionRejected(const char *reason)
{
   Con::executef(this, 2, "onConnectRequestRejected", reason);
}

void GameConnection::handleStartupError(const char *errorString)
{
   Con::executef(this, 2, "onConnectRequestRejected", errorString);
}

void GameConnection::writeConnectAccept(BitStream *stream)
{
   Parent::writeConnectAccept(stream);
   stream->write(getProtocolVersion());
}

bool GameConnection::readConnectAccept(BitStream *stream, const char **errorString)
{
   if(!Parent::readConnectAccept(stream, errorString))
      return false;

   U32 protocolVersion;
   stream->read(&protocolVersion);
   if(protocolVersion < MinRequiredProtocolVersion || protocolVersion > CurrentProtocolVersion)
   {
      *errorString = "CHR_PROTOCOL"; // this should never happen unless someone is faking us out.
      return false;
   }
   return true;
}

void GameConnection::writeConnectRequest(BitStream *stream)
{
   Parent::writeConnectRequest(stream);
   stream->writeString(GameString);
   stream->write(CurrentProtocolVersion);
   stream->write(MinRequiredProtocolVersion);
   stream->writeString(mJoinPassword);

   stream->write(mConnectArgc);
   for(U32 i = 0; i < mConnectArgc; i++)
      stream->writeString(mConnectArgv[i]);
}

bool GameConnection::readConnectRequest(BitStream *stream, const char **errorString)
{
   if(!Parent::readConnectRequest(stream, errorString))
      return false;
   U32 currentProtocol, minProtocol;
   char gameString[256];
   stream->readString(gameString);
   if(dStrcmp(gameString, GameString))
   {
      *errorString = "CHR_GAME";
      return false;
   }

   stream->read(&currentProtocol);
   stream->read(&minProtocol);

   char joinPassword[256];
   stream->readString(joinPassword);

   if(currentProtocol < MinRequiredProtocolVersion)
   {
      *errorString = "CHR_PROTOCOL_LESS";
      return false;
   }
   if(minProtocol > CurrentProtocolVersion)
   {
      *errorString = "CHR_PROTOCOL_GREATER";
      return false;
   }
   setProtocolVersion(currentProtocol < CurrentProtocolVersion ? currentProtocol : CurrentProtocolVersion);

   const char *serverPassword = Con::getVariable("Pref::Server::Password");
   if(serverPassword[0])
   {
      if(dStrcmp(joinPassword, serverPassword))
      {
         *errorString = "CHR_PASSWORD";
         return false;
      }
   }

   stream->read(&mConnectArgc);
   if(mConnectArgc > MaxConnectArgs)
   {
      *errorString = "CR_INVALID_ARGS";
      return false;
   }
   const char *connectArgv[MaxConnectArgs + 3];
   for(U32 i = 0; i < mConnectArgc; i++)
   {
      char argString[256];
      stream->readString(argString);
      mConnectArgv[i] = dStrdup(argString);
      connectArgv[i + 3] = mConnectArgv[i];
   }
   connectArgv[0] = "onConnectRequest";
   char buffer[256];
   Net::addressToString(getNetAddress(), buffer);
   connectArgv[2] = buffer;

   const char *ret = Con::execute(this, mConnectArgc + 3, connectArgv);
   if(ret[0])
   {
      *errorString = ret;
      return false;
   }
   return true;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GameConnection::connectionError(const char *errorString)
{
   if(isConnectionToServer())
   {
      Con::printf("Connection error: %s.", errorString);
      Con::executef(this, 2, "onConnectionError", errorString);
   }
   else
   {
      Con::printf("Client %d packet error: %s.", getId(), errorString);
      setDisconnectReason("Packet Error.");
   }
   deleteObject();
}


void GameConnection::setAuthInfo(const AuthInfo *info)
{
   mAuthInfo = new AuthInfo;
   *mAuthInfo = *info;
}

const AuthInfo *GameConnection::getAuthInfo()
{
   return mAuthInfo;
}

//----------------------------------------------------------------------------

bool GameConnection::onAdd()
{
   if (!Parent::onAdd())
      return false;

   return true;
}

void GameConnection::onRemove()
{
   if(isNetworkConnection())
   {
       sendDisconnectPacket(mDisconnectReason);
   }
   if(!isConnectionToServer())
      Con::executef(this, 2, "onDrop", mDisconnectReason);

   Parent::onRemove();
}

void GameConnection::setDisconnectReason(const char *str)
{
   dStrncpy(mDisconnectReason, str, sizeof(mDisconnectReason) - 1);
   mDisconnectReason[sizeof(mDisconnectReason) - 1] = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


void GameConnection::readPacket(BitStream *bstream)
{
    //Con::printf("GameConnection::handlePacket - readPacket ---");
   char stringBuf[256];
   stringBuf[0] = 0;
   bstream->setStringBuffer(stringBuf);

   bstream->clearCompressionPoint();
   
   Parent::readPacket(bstream);
   bstream->clearCompressionPoint();
   bstream->setStringBuffer(NULL);
}

void GameConnection::writePacket(BitStream *bstream, PacketNotify *note)
{
   char stringBuf[256];
   bstream->clearCompressionPoint();
   stringBuf[0] = 0;
   bstream->setStringBuffer(stringBuf);
                                                   
   Parent::writePacket(bstream, note);
   bstream->clearCompressionPoint();
   bstream->setStringBuffer(NULL);
}


void GameConnection::detectLag()
{
   //see if we're lagging...
   S32 curTime = Sim::getCurrentTime();
   if (curTime - mLastPacketTime > mLagThresholdMS)
   {
      if (!mLagging)
      {
         mLagging = true;
         Con::executef(this, 2, "setLagIcon", "true");
      }
   }
   else if (mLagging)
   {
      mLagging = false;
      Con::executef(this, 2, "setLagIcon", "false");
   }
}

GameConnection::GamePacketNotify::GamePacketNotify()
{
   // need to fill in empty notifes for demo start block
 
}

NetConnection::PacketNotify *GameConnection::allocNotify()
{
   return new GamePacketNotify;
}

void GameConnection::packetReceived(PacketNotify *note)
{
   //record the time so we can tell if we're lagging...
   mLastPacketTime = Sim::getCurrentTime();

   Parent::packetReceived(note);
}

void GameConnection::packetDropped(PacketNotify *note)
{
   Parent::packetDropped(note);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

 
void GameConnection::handleConnectionMessage(U32 message, U32 sequence, U32 ghostCount)
{
   Parent::handleConnectionMessage(message, sequence, ghostCount);
}

//----------------------------------------------------------------------------


ConsoleMethod( GameConnection, activateGhosting, void, 2, 2, "() Use the activateGhosting method to GameConnection instance to start ghosting objects to the client.\n"
                                                                "This is called on each client connection by the server.\n"
                                                                "@return No return value.\n"
                                                                "@sa resetGhosting")
{
   object->activateGhosting();
}

ConsoleMethod( GameConnection, resetGhosting, void, 2, 2, "() Use the resetGhosting method to reset ghosting. This in effect tells the server to resend each ghost to insure that all objects which should be ghosts and are in fact ghosted.\n"
                                                                "@return No return value.\n"
                                                                "@sa activateGhosting")
{
   object->resetGhosting();
}


ConsoleMethod( GameConnection, delete, void, 2, 3, "( [ reason ] ) Use the delete method to destroy and disconnect the current connection, giving an optional reason. If reason is specified, it will be transmitted to the client/server on the other end of the connection.\n"
                                                                "@param reason A string explaining while the connection is being severed.\n"
                                                                "@return No return value")
{
   if (argc == 3)
      object->setDisconnectReason(argv[2]);
   object->deleteObject();
}


//--------------------------------------------------------------------------
void GameConnection::consoleInit()
{
   Con::addVariable("Pref::Net::LagThreshold", TypeS32, &mLagThresholdMS);
}

ConsoleStaticMethod(GameConnection, getServerConnection, S32, 2, 2, "() Get the server connection if any.")
{
   if(GameConnection::getConnectionToServer())
      return GameConnection::getConnectionToServer()->getId();
   else
   {
      Con::errorf("GameConnection::getServerConnection - no connection available.");
      return -1;
   }
}

//added for a lack of a better place
ConsoleFunction( purgeResources, void, 1, 1, "() Purge resources from the resource manager.")
{
   ResourceManager->purge();
}