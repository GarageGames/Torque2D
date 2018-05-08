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

ConsoleMethodGroupBeginWithDocs(NetConnection, SimGroup)

/*! Use the getAddress method to get the address and port that this NetConnection is currently attached to.
    @return Returns the address and port that this NetConnection is currently attached to, where the addres will be of the form: A.B.C.D:Port. A .. B are standard IP numbers between 0 and 255 and Port can be between 1000 and 65536. If the connection is local, the string 'local' will be returned. If a this NetConnection is not currently connected the method will return a NULL string.
    @sa connect, connectLocal
*/
ConsoleMethodWithDocs(NetConnection,getAddress, ConsoleString,2,2, ())
{
   if(object->isLocalConnection())
      return "local";
   char *buffer = Con::getReturnBuffer(256);
   Net::addressToString(object->getNetAddress(), buffer);
   return buffer;
}

/*! Use the setSimulatedNetParams method to force a connection to experience a certain degree of packet-loss and/or latency. This is a debug feature to allow us to see how a distributed game will behave in the face of poor connection quality.
    @param packetLoss A floating-point value between 0.0 (0%) and 1.0 (100%) dictating the percentage of packets to be artificially lost.
    @param delay An integer value specifying the number of milliseconds to insert into transmission latencies.
    @return No return value.
    @sa getPacketLoss, getPing
*/
ConsoleMethodWithDocs(NetConnection,setSimulatedNetParams, ConsoleVoid,4, 4, ( packetLoss , delay ))
{
   object->setSimulatedNetParams(dAtof(argv[2]), dAtoi(argv[3]));
}

/*! Use the getPing method to determine the round-trip travel time from this connection to the agent on the other end and back again.
    @return Returns an integer value representing the total time in milliseconds it takes for a ping request to travel to the agent on the other end of a connection and back to this agent.
    @sa getPacketLoss
*/
ConsoleMethodWithDocs( NetConnection, getPing, ConsoleInt, 2, 2, ())
{
   return( S32( object->getRoundTripTime() ) );
}

/*! Use the getPacketLoss method to determine the current packetLoss count for this connection.
    @return Returns an integer value between 0 and inf, indicating the number of packets that have been lost to date on this net connection.
    @sa getPing
*/
ConsoleMethodWithDocs( NetConnection, getPacketLoss, ConsoleInt, 2, 2, ())
{
   return( S32( 100 * object->getPacketLoss() ) );
}

/*! Use the checkMaxRate method to retrieve the current maximum packet rate for this connection.
    The period may not neccesarily be one second. To adjust packet rates, see the preference variables above
    @return Returns an integer value representing the maximum number of packets that can be transmitted by this connection per transmission period.
*/
ConsoleMethodWithDocs( NetConnection, checkMaxRate, ConsoleVoid, 2, 2, ())
{
   object->checkMaxRate();
}

#ifdef TORQUE_DEBUG_NET

/*! 
*/
ConsoleMethodWithDocs( NetConnection, setLogging, ConsoleVoid, 3, 3, conn.setLogging(bool))
{
   object->setLogging(dAtob(argv[2]));
}

#endif

/*! Convert a ghost id from this connection to a real id.
    @return The ID as an integer
*/
ConsoleMethodWithDocs(NetConnection, resolveGhostID, ConsoleInt, 3, 3, ( S32 ghostID ))
{
   S32 gID = dAtoi(argv[2]);

   // Safety check
   if(gID < 0 || gID > NetConnection::MaxGhostCount) return 0;

   NetObject *foo = object->resolveGhost(gID);

   if(foo)
      return foo->getId();
   else
      return 0;
}

/*! Convert a ghost index from this connection to a real id.
    @return The ID as an integer
*/
ConsoleMethodWithDocs(NetConnection, resolveObjectFromGhostIndex, ConsoleInt, 3, 3, ( S32 ghostIdx))
{
   S32 gID = dAtoi(argv[2]);

   // Safety check
   if(gID < 0 || gID > NetConnection::MaxGhostCount) return 0;

   NetObject *foo = object->resolveObjectFromGhostIndex(gID);

   if(foo)
      return foo->getId();
   else
      return 0;
}

/*! Convert a real id to the ghost id for this connection.
    @return The ID as an integer
*/
ConsoleMethodWithDocs(NetConnection, getGhostID, ConsoleInt, 3, 3, ( S32 realID ))
{
   NetObject * foo;

   if(Sim::findObject(argv[2], foo))
   {
      return object->getGhostIndex(foo);
   }
   else
   {
      Con::errorf("NetConnection::serverToGhostID - could not find specified object");
      return -1;
   }
}

/*! Use the connect method to request a connection to a remote server at the address remoteAddress.
    @param remoteAddress A string containing an address of the form: A.B.C.D:Port, where A .. B are standard IP numbers between 0 and 255 and Port can be between 1000 and 65536.
    @return No return value.
    @sa connectLocal, getAddress
*/
ConsoleMethodWithDocs(NetConnection, connect, ConsoleVoid, 3, 3, ( remoteAddress ))
{
   NetAddress addr;
   if(Net::stringToAddress(argv[2], &addr) != Net::NoError)
   {
      Con::errorf("NetConnection::connect: invalid address - %s", argv[2]);
      return;
   }
   object->connect(&addr);
}

/*! Use the connectLocal method to connect the current client-side connection to a local NetConnection, that is to create an internal connection from this client to the internal server. This is accomplished through the use of a back door mechanism and has an extremely high bandwidth.
    @return No return value.
    @sa connect, getAddress
*/
ConsoleMethodWithDocs(NetConnection, connectLocal, ConsoleString, 2, 2, ())
{
   ConsoleObject *co = ConsoleObject::create(object->getClassName());
   NetConnection *client = object;
   NetConnection *server = dynamic_cast<NetConnection *>(co);
   BitStream *stream = BitStream::getPacketStream();

   if(!server || !server->canRemoteCreate())
	{
		delete co;
		return "error";
	}

   server->registerObject();
   server->setIsLocalClientConnection();

   server->setSequence(0);
   client->setSequence(0);
   client->setRemoteConnectionObject(server);
   server->setRemoteConnectionObject(client);

	//We need to reset the maxrate's here, because we
	// can't test if it is a local connection until RemoteConnectionObject
	// has been set
	server->checkMaxRate();
	client->checkMaxRate();

   stream->setPosition(0);
   client->writeConnectRequest(stream);
   stream->setPosition(0);

	const char* error;
   if(!server->readConnectRequest(stream, &error))
	{
		client->onConnectionRejected(error);
		server->deleteObject();
		return "error";
	}

   stream->setPosition(0);
   server->writeConnectAccept(stream);
   stream->setPosition(0);

   if(!client->readConnectAccept(stream, &error))
	{
		client->handleStartupError(error);
		server->deleteObject();
		return "error";
	}

   client->onConnectionEstablished(true);
   server->onConnectionEstablished(false);
   client->setEstablished();
   server->setEstablished();
   client->setConnectSequence(0);
   server->setConnectSequence(0);
   NetConnection::setLocalClientConnection(server);
   server->assignName("LocalClientConnection");

   return "";
}

/*! Use the getGhostsActive method to determine how many ghosts are active on a particular connection.
	@return Returns an integer value between 0 and inf, specifying how many objects are being ghosted
	to a client on the other side of a specific connection
*/
ConsoleMethodWithDocs( NetConnection, getGhostsActive, S32, 2, 2, ())
{
    return object->getGhostsActive();
}

ConsoleMethodGroupEndWithDocs(NetConnection)
