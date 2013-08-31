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

ConsoleMethodGroupBeginWithDocs(NetObject, SimObject)

/*! Use the scopeToClient method to force this object to be SCOPE_ALWAYS on client.
    When an object is SCOPE_ALWAYS it is always ghosted. Therefore, if you have an object that should always be ghosted to a client, use this method.
    @param client The ID of the client to force this object to be SCOPE_ALWAYS for.
    @return No return value.
    @sa clearScopeToClient, setScopeAlways
*/
ConsoleMethodWithDocs(NetObject,scopeToClient, ConsoleVoid,3,3, ( client ))
{
   NetConnection *conn;
   if(!Sim::findObject(argv[2], conn))
   {
      Con::errorf(ConsoleLogEntry::General, "NetObject::scopeToClient: Couldn't find connection %s", argv[2]);
      return;
   }
   conn->objectLocalScopeAlways(object);
}

/*! Use the clearScopeToClient method to undo the effects of a previous call to scopeToClient.
    @param client The ID of the client to stop forcing scoping this object for.
    @return No return value.
    @sa scopeToClient
*/
ConsoleMethodWithDocs(NetObject,clearScopeToClient, ConsoleVoid,3,3, ( client ))
{
   NetConnection *conn;
   if(!Sim::findObject(argv[2], conn))
   {
      Con::errorf(ConsoleLogEntry::General, "NetObject::clearScopeToClient: Couldn't find connection %s", argv[2]);
      return;
   }
   conn->objectLocalClearAlways(object);
}

/*! Use the setScopeAlways method to force an object to be SCOPE_ALWAYS for all clients.
    When an object is SCOPE_ALWAYS it is always ghosted. Therefore, if you have an object that should always be ghosted to all clients, use this method.
    @return No return value.
    @sa scopeToClient
*/
ConsoleMethodWithDocs(NetObject,setScopeAlways, ConsoleVoid,2,2, ())
{
   object->setScopeAlways();
}

/*!  @return Returns the ghost ID of the object
*/
ConsoleMethodWithDocs( NetObject, getGhostID, ConsoleInt, 2, 2, ())
{
   return object->getNetIndex();
}

ConsoleMethodGroupEndWithDocs(NetObject)
