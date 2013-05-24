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

ConsoleFunctionGroupBegin( ServerQuery, "Functions which allow you to query the LAN or a master server for online games.");

/*! @defgroup ServerQueryFunctions Server Query
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the queryLANServers function to establish whether any game servers of the required specification(s) are available on the local area network (LAN).
    @param port Look for any game servers advertising at this port. Set to 0 if you don't care what port the game server is using.
    @param flags Look for any game servers with these special flags set. Set to 0 for no flags.
    @param gametype Look for any game servers playing a game type that matches this string. Set to the NULL string to look for any game type.
    @param missiontype Look for any game servers playing a mission type that matches this string. Set to the NULL string to look for any mission type.
    @param minplayers Look for any game servers with this number of players or more. Set to 0 for no lower limit.
    @param maxplayers Look for any game servers with this number of players or fewer. Set to 0 for no upper limit.
    @param maxbots Look for any game servers with this number of AI controlled players or fewer. Set to 0 for no limit.
    @param regionmask Look for any master servers, on our master server list, in this region. Set to 0 to examine all regions.
    @param maxping Look for any game servers with a PING rate equal to or lower than this. Set to 0 for no upper PING limit.
    @param mincpu Look for any game servers with a CPU (clock speed) equal or greater than this. Set to 0 for no CPU (clock speed) limit.
    @param filterflags Look for any game servers with this game version number or higher. Set to 0 to find all versions.
    @return No return value.
    @sa getServerCount, queryMasterServer, setServerInfo, stopServerQuery
*/
ConsoleFunctionWithDocs( queryLanServers, ConsoleVoid, 12, 12, ( port , flags , gametype , missiontype , minplayers , maxplayers , maxbots , regionmask , maxping , mincpu , filterflags ))
{
   U32 lanPort = dAtoi(argv[1]);
   U8 flags = dAtoi(argv[2]);

   // It's not a good idea to hold onto args, recursive calls to
   // console exec will trash them.
   char* gameType = dStrdup(argv[3]);
   char* missionType = dStrdup(argv[4]);

   U8 minPlayers = dAtoi(argv[5]);
   U8 maxPlayers = dAtoi(argv[6]);
   U8 maxBots = dAtoi(argv[7]);
   U32 regionMask = dAtoi(argv[8]);
   U32 maxPing = dAtoi(argv[9]);
   U16 minCPU = dAtoi(argv[10]);
   U8 filterFlags = dAtoi(argv[11]);

   queryLanServers(lanPort, flags, gameType, missionType, minPlayers, maxPlayers, maxBots,
       regionMask, maxPing, minCPU, filterFlags);

   dFree(gameType);
   dFree(missionType);
}

/*! Use the queryMasterServer function to query all master servers in the master server list and to establish if they are aware of any game servers that meet the specified requirements, as established by the arguments passed to this function.
    In order for this function to do anything, a list of master servers must have been previously specified. This list may contain one or more server addresses. A call to this function will search all servers in the list. To specify a list, simply create a set of array entries like this:$pref::Master[0] = \2:192.168.123.15:28002\;$pref::Master[1] = \2:192.168.123.2:28002\;The format of these values is ==> Region Number : IP Address : Port NumberThese values should be specified in either the client's or the server's preferences file (prefs.cs). You may specifiy it elsewhere, however be sure that it is specified prior to this function being called and before any other functions that rely on it.
    @param flags Look for any game servers with these special flags set. Set to 0 for no flags.
    @param gametype Look for any game servers playing a game type that matches this string. Set to the NULL string to look for any game type.
    @param missiontype Look for any game servers playing a mission type that matches this string. Set to the NULL string to look for any mission type.
    @param minplayers Look for any game servers with this number of players or more. Set to 0 for no lower limit.
    @param maxplayers Look for any game servers with this number of players or fewer. Set to 0 for no upper limit.
    @param maxbots Look for any game servers with this number of AI controlled players or fewer. Set to 0 for no limit.
    @param regionmask Look for any master servers, on our master server list, in this region. Set to 0 to examine all regions.
    @param maxping Look for any game servers with a PING rate equal to or lower than this. Set to 0 for no upper PING limit.
    @param mincpu Look for any game servers with a CPU (clock speed) equal or greater than this. Set to 0 for no CPU (clock speed) limit.
    @param filterflags Look for any game servers with this game version number or higher. Set to 0 to find all versions.
    @return No return value.
    @sa getServerCount, queryLANServers, setServerInfo, startHeartbeat, stopServerQuery
*/
ConsoleFunctionWithDocs( queryMasterServer, ConsoleVoid, 11, 11, ( flags , gametype , missiontype , minplayers , maxplayers , maxbots , regionmask , maxping , mincpu , filterflags ))
{
   U8 flags = dAtoi(argv[1]);

   // It's not a good idea to hold onto args, recursive calls to
   // console exec will trash them.
   char* gameType = dStrdup(argv[2]);
   char* missionType = dStrdup(argv[3]);

   U8 minPlayers = dAtoi(argv[4]);
   U8 maxPlayers = dAtoi(argv[5]);
   U8 maxBots = dAtoi(argv[6]);
   U32 regionMask = dAtoi(argv[7]);
   U32 maxPing = dAtoi(argv[8]);
   U16 minCPU = dAtoi(argv[9]);
   U8 filterFlags = dAtoi(argv[10]);
   U32 buddyList = 0;

   queryMasterServer(flags,gameType,missionType,minPlayers,maxPlayers,
      maxBots,regionMask,maxPing,minCPU,filterFlags,0,&buddyList);

   dFree(gameType);
   dFree(missionType);
}

//-----------------------------------------------------------------------------

/*! Use the querySingleServer function to re-query a previously queried lan server, OR a game server found with queryLANServers or with queryMasterServer and selected with setServerInfo. This will refresh the information stored by TGE about this server. It will not however modify the values of the $ServerInfo::* global variables.
    @param address The IP address and Port to re-query, i.e. \192.168.123.2:28000\.
    @param flags No longer used.
    @return No return value.
    @sa getServerCount, queryLANServers, queryMasterServer, setServerInfo, stopServerQuery
*/
ConsoleFunctionWithDocs( querySingleServer, ConsoleVoid, 3, 3, ( address [ , flags ] ))
{
   NetAddress addr;
   char* addrText;

   addrText = dStrdup(argv[1]);
   U8 flags = dAtoi(argv[2]);


   Net::stringToAddress( addrText, &addr );
   querySingleServer(&addr,flags);
}

/*! Use the cancelServerQuery function to cancel a previous query*() call.
    @return No return value.
    @sa queryLANServers, queryMasterServer, querySingleServer
*/
ConsoleFunctionWithDocs( cancelServerQuery, ConsoleVoid, 1, 1, ())
{
   cancelServerQuery();
}

/*! Use the stopServerQuery function to cancel any outstanding server queries.
    @return No return value.
    @sa queryLANServers, queryMasterServer, querySingleServer
*/
ConsoleFunctionWithDocs( stopServerQuery, ConsoleVoid, 1, 1, ())
{
   stopServerQuery();
}

//-----------------------------------------------------------------------------

/*! Use the startHeartbeat function to start advertising this game serer to any master servers on the master server list.
    In order for this function to do anything, a list of master servers must have been previously specified. This list may contain one or more server addresses. Once this function is called, the game server will re-advertise itself to all the master servers on its master server lits every two minutes. To specify a list, simply create a set of array entries like this:$pref::Master[0] = \2:192.168.123.15:28002\;$pref::Master[1] = \2:192.168.123.2:28002\;The format of these values is ==> Region Number : IP Address : Port NumberThese values should be specified in either the client's or the server's preferences file (prefs.cs). You may specifiy it elsewhere, however be sure that it is specified prior to this function being called and before any other functions that rely on it.
    @return No return value.
    @sa queryMasterServer, stopHeartbeat
*/
ConsoleFunctionWithDocs(startHeartbeat, ConsoleVoid, 1, 1, ())
{
   startHeartbeat();
}

/*! Use the startHeartbeat function to stop advertising this game serer to any master servers on the master server list.
    @return No return value.
    @sa queryMasterServer, startHeartbeat
*/
ConsoleFunctionWithDocs(stopHeartbeat, ConsoleVoid, 1, 1, ())
{
	stopHeartBeat();
}

//-----------------------------------------------------------------------------

/*! Use the getServerCount function to determine the number of game servers found on the last queryLANServers() or queryMasterServer() call.
    This value is important because it allows us to properly index when calling setServerInfo().
    @return Returns a numeric value equal to the number of game servers found on the last queryLANServers() or queryMasterServer() call. Returns 0 if the function was not called, or none were found.
    @sa queryLANServers, queryMasterServer, setServerInfo
*/
ConsoleFunctionWithDocs( getServerCount, int, 1, 1, ())
{
   return gServerList.size();
}

/*! Use the setServerInfo function to set the values of the $ServerInfo::* global variables with information for a server found with queryLANServers or with queryMasterServer.
    @param index The index of the server to get information about.
    @return Will return true if the information was successfully set, false otherwise.
    @sa getServerCount, queryLANServers, queryMasterServer, querySingleServer
*/
ConsoleFunctionWithDocs( setServerInfo, ConsoleBool, 2, 2, ( index ))
{
   S32 index = dAtoi(argv[1]);
   
   if (index >= 0 && index < gServerList.size())
   {
      ServerInfo& info = gServerList[index];

      char addrString[256];
      Net::addressToString( &info.address, addrString );

      Con::setIntVariable("ServerInfo::Status",info.status);
      Con::setVariable("ServerInfo::Address",addrString);
      Con::setVariable("ServerInfo::Name",info.name);
      Con::setVariable("ServerInfo::GameType",info.gameType);
      Con::setVariable("ServerInfo::MissionName",info.missionName);
      Con::setVariable("ServerInfo::MissionType",info.missionType);
      Con::setVariable("ServerInfo::State",info.statusString);
      Con::setVariable("ServerInfo::Info",info.infoString);
      Con::setIntVariable("ServerInfo::PlayerCount",info.numPlayers);
      Con::setIntVariable("ServerInfo::MaxPlayers",info.maxPlayers);
      Con::setIntVariable("ServerInfo::BotCount",info.numBots);
      Con::setIntVariable("ServerInfo::Version",info.version);
      Con::setIntVariable("ServerInfo::Ping",info.ping);
      Con::setIntVariable("ServerInfo::CPUSpeed",info.cpuSpeed);
      Con::setBoolVariable("ServerInfo::Favorite",info.isFavorite);
      Con::setBoolVariable("ServerInfo::Dedicated",info.isDedicated());
      Con::setBoolVariable("ServerInfo::Password",info.isPassworded());
      return true;
   }
   return false;
}

ConsoleFunctionGroupEnd( ServerQuery);

/*! @} */ // group ServerQueryFunctions
