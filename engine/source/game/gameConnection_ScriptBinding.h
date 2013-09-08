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

ConsoleMethodGroupBeginWithDocs(GameConnection, NetConnection)

/*! Use the setJoinPassword method to set the password required to connect to this server-side GameConnection.
    Pass a NULL string to clear the password.
    @param password A string representing the case insensitive password to use for this server-side GameConnection.
    @return No return value.
    @sa setConnectArgs
*/
ConsoleMethodWithDocs(GameConnection, setJoinPassword, ConsoleVoid, 3, 3, ( password ))
{
   object->setJoinPassword(argv[2]);
}

/*! Use the setConnectArgs method to set the connection arguments for this client-side GameConnection. These values will be passed to the server upon establishing a connection.
    @param name Generally, the first argument is the name of the player.
    @param arg1 ... , arg15 - 15 additional arguments may be passed.
    @return No return value.
    @sa setJoinPassword
*/
ConsoleMethodWithDocs(GameConnection, setConnectArgs, ConsoleVoid, 3, 17, ( name, [arg1, ..., arg15 ]? ))
{
   object->setConnectArgs(argc - 2, argv + 2);
}

/*! Use the activateGhosting method to GameConnection instance to start ghosting objects to the client.
    This is called on each client connection by the server.
    @return No return value.
    @sa resetGhosting
*/
ConsoleMethodWithDocs( GameConnection, activateGhosting, ConsoleVoid, 2, 2, ())
{
   object->activateGhosting();
}

/*! Use the resetGhosting method to reset ghosting. This in effect tells the server to resend each ghost to insure that all objects which should be ghosts and are in fact ghosted.
    @return No return value.
    @sa activateGhosting
*/
ConsoleMethodWithDocs( GameConnection, resetGhosting, ConsoleVoid, 2, 2, ())
{
   object->resetGhosting();
}


/*! Use the delete method to destroy and disconnect the current connection, giving an optional reason. If reason is specified, it will be transmitted to the client/server on the other end of the connection.
    @param reason A string explaining while the connection is being severed.
    @return No return value
*/
ConsoleMethodWithDocs( GameConnection, delete, ConsoleVoid, 2, 3, ( [ reason ]? ))
{
   if (argc == 3)
      object->setDisconnectReason(argv[2]);
   object->deleteObject();
}

/*! Get the server connection if any.
*/
ConsoleStaticMethodWithDocs(GameConnection, getServerConnection, ConsoleInt, 2, 2, ())
{
   if(GameConnection::getConnectionToServer())
      return GameConnection::getConnectionToServer()->getId();
   else
   {
      Con::errorf("GameConnection::getServerConnection - no connection available.");
      return -1;
   }
}

ConsoleMethodGroupEndWithDocs(GameConnection)

//---------------------------------------

//added for a lack of a better place
/*! Purge resources from the resource manager.
*/
ConsoleFunctionWithDocs( purgeResources, ConsoleVoid, 1, 1, ())
{
   ResourceManager->purge();
}
