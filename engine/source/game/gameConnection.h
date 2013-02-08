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

#ifndef _GAMECONNECTION_H_
#define _GAMECONNECTION_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif
#ifndef _NETCONNECTION_H_
#include "network/netConnection.h"
#endif
#ifndef _BITVECTOR_H_
#include "collection/bitVector.h"
#endif

enum GameConnectionConstants {
   MaxClients = 126,
   DataBlockQueueCount = 16
};

class AudioAsset;
class MatrixF;
class MatrixF;
class Point3F;
struct AuthInfo;

#define GameString "Torque"


/// Formerly contained a certificate, showing that something was valid.
class Auth2Certificate
{
   U32 xxx;
};

/// Formerly contained data indicating whether a user is valid.
struct AuthInfo
{
   enum {
      MaxNameLen = 31,
   };

   bool valid;
   char name[MaxNameLen + 1];
};

/// Formerly validated the server's authentication info.
inline bool validateAuthenticatedServer()
{
   return true;
}

/// Formerly validated the client's authentication info.
inline bool validateAuthenticatedClient()
{
   return true;
}


class GameConnection : public NetConnection
{
private:
   typedef NetConnection Parent;


   char mDisconnectReason[256];
   
   U32  mMissionCRC;             // crc of the current mission file from the server

public:
   
   /// @name Protocol Versions
   ///
   /// Protocol versions are used to indicated changes in network traffic.
   /// These could be changes in how any object transmits or processes
   /// network information. You can specify backwards compatability by
   /// specifying a MinRequireProtocolVersion.  If the client
   /// protocol is >= this min value, the connection is accepted.
   ///
   /// Torque (V12) SDK 1.0 uses protocol  =  1
   ///
   /// Torque SDK 1.1 uses protocol = 2
   /// @{
   static const U32 CurrentProtocolVersion;
   static const U32 MinRequiredProtocolVersion;
   /// @}

   /// Configuration
   enum Constants {
      GameConnectionBlockTypeCount,
      MaxConnectArgs = 16,
   };

   /// Set connection arguments; these are passed to the server when we connect.
   void setConnectArgs(U32 argc, const char **argv);

   /// Set the server password to use when we join.
   void setJoinPassword(const char *password);

   /// @name Event Handling
   /// @{

   virtual void onTimedOut();
   virtual void onConnectTimedOut();
   virtual void onDisconnect(const char *reason);
   virtual void onConnectionRejected(const char *reason);
   virtual void onConnectionEstablished(bool isInitiator);
   virtual void handleStartupError(const char *errorString);
   /// @}

   /// @name Packet I/O
   /// @{

   virtual void writeConnectRequest(BitStream *stream);
   virtual bool readConnectRequest(BitStream *stream, const char **errorString);
   virtual void writeConnectAccept(BitStream *stream);
   virtual bool readConnectAccept(BitStream *stream, const char **errorString);
   /// @}

   bool canRemoteCreate();

private:
   /// @name Connection State
   /// This data is set with setConnectArgs() and setJoinPassword(), and
   /// sent across the wire when we connect.
   /// @{

   U32      mConnectArgc;
   char *mConnectArgv[MaxConnectArgs];
   char *mJoinPassword;
   /// @}

protected:
   struct GamePacketNotify : public NetConnection::PacketNotify
   {
      GamePacketNotify();
   };
   PacketNotify *allocNotify();

   AuthInfo *  mAuthInfo;

   static S32  mLagThresholdMS;
   S32         mLastPacketTime;
   bool        mLagging;

   /// @}

   /// @name Packet I/O
   /// @{

   void readPacket      (BitStream *bstream);
   void writePacket     (BitStream *bstream, PacketNotify *note);
   void packetReceived  (PacketNotify *note);
   void packetDropped   (PacketNotify *note);
   void connectionError (const char *errorString);
   /// @}

public:

   DECLARE_CONOBJECT(GameConnection);
   void handleConnectionMessage(U32 message, U32 sequence, U32 ghostCount);
   static void consoleInit();

   void setDisconnectReason(const char *reason);
   GameConnection();
   ~GameConnection();

   bool onAdd();
   void onRemove();

   static GameConnection *getServerConnection() { return dynamic_cast<GameConnection*>((NetConnection *) mServerConnection); }
   static GameConnection *getLocalClientConnection() { return dynamic_cast<GameConnection*>((NetConnection *) mLocalClientConnection); }

   /// @}

   void detectLag();

  

   /// @name Authentication
   ///
   /// This is remnant code from Tribes 2.
   /// @{

   void            setAuthInfo(const AuthInfo *info);
   const AuthInfo *getAuthInfo();
   /// @}

};

#endif
