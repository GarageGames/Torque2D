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

/*! @defgroup TelnetDebuggerFunctions Telnet Debugger
	@ingroup TorqueScriptFunctions
	@{
*/

ConsoleFunctionGroupBegin( Net, "Functions for use with the network; tagged strings and remote commands.");

ConsoleFunction( commandToServer, void, 2, RemoteCommandEvent::MaxRemoteCommandArgs + 1, "( func [ , arg1, ... , argn ] ) Use the commandToServer function to issue a remote procedure call the server.\n"
                                                                "All arguments may be in tagged or non-tagged format. See 'Remote Procedure Call Samples' below\n"
                                                                "@param func The suffix of the remote procedure name to be executed on the client.\n"
                                                                "@param arg1 ... argn - Optional arguments to be passed to the remote procedure.\n"
                                                                "@return No return value.")
{
   NetConnection *conn = NetConnection::getConnectionToServer();
   if(!conn)
      return;
   sendRemoteCommand(conn, argc - 1, argv + 1);
}

ConsoleFunction( commandToClient, void, 3, RemoteCommandEvent::MaxRemoteCommandArgs + 2, "( client, func [ , arg1, ... , argn ] ) Use the commandToClient function to issue a remote procedure call on a client.\n"
                                                                "All arguments (excluding client) may be in tagged or non-tagged format. See 'Remote Procedure Call Samples' below\n"
                                                                "@param client The numeric ID of a client gameConnection.\n"
                                                                "@param func The suffix of the remote procedure name to be executed on the client.\n"
                                                                "@param arg1 ... argn - Optional arguments to be passed to the remote procedure.\n"
                                                                "@return No return value.")
{
   NetConnection *conn;
   if(!Sim::findObject(argv[1], conn))
      return;
   sendRemoteCommand(conn, argc - 2, argv + 2);
}

ConsoleFunction(removeTaggedString, void, 2, 2, "( tag ) Use the removeTaggedSTring function to remove a previously tagged string from the NetStringTable.\n"
                                                                "@param tag A number tag ID.\n"
                                                                "@return No return value")
{
   gNetStringTable->removeString(dAtoi(argv[1]+1), true);
}

ConsoleFunction( addTaggedString, const char*, 2, 2, "( string ) Use the addTaggedString function to tag a new string and add it to the NetStringTable.\n"
                                                                "@param string The string to tagged and placed in the NetStringTable. Tagging ignores case, so tagging the same string (excluding case differences) will be ignored as a duplicated tag.\n"
                                                                "@return Returns a string (containing a numeric value) equivalent to the string ID for the newly tagged string")
{
   NetStringHandle s(argv[1]);
   gNetStringTable->incStringRefScript(s.getIndex());

   char *ret = Con::getReturnBuffer(10);
   ret[0] = StringTagPrefixByte;
   dSprintf(ret + 1, 9, "%d", s.getIndex());
   return ret;
}

ConsoleFunction( getTaggedString, const char*, 2, 2, "( tag ) Use the getTaggedString function to convert a tag to a string. This is not the same a detag() which can only be used within the context of a function that receives a tag. This function can be used any time and anywhere to convert a tag to a string.\n"
                                                                "@param tag A numeric tag ID.\n"
                                                                "@return Returns the string corresponding to the tag ID")
{
   const char *indexPtr = argv[1];
   if (*indexPtr == StringTagPrefixByte)
      indexPtr++;
   return gNetStringTable->lookupString(dAtoi(indexPtr));
}

ConsoleFunction( buildTaggedString, const char*, 2, 11, "( format , <arg1, ...arg9> ) Use the buildTaggedString function to build a tagged string using the specified format.\n"
                                                                "@param enable A boolean value. If set to true, network packet logging is enabled, otherwise it is disabled.\n"
                                                                "@return No return value")
{
   const char *indexPtr = argv[1];
   if (*indexPtr == StringTagPrefixByte)
      indexPtr++;
   const char *fmtString = gNetStringTable->lookupString(dAtoi(indexPtr));
   char *strBuffer = Con::getReturnBuffer(512);
   const char *fmtStrPtr = fmtString;
   char *strBufPtr = strBuffer;
   S32 strMaxLength = 511;
   if (!fmtString)
      goto done;

   //build the string
   while (*fmtStrPtr)
   {
      //look for an argument tag
      if (*fmtStrPtr == '%')
      {
         if (fmtStrPtr[1] >= '1' && fmtStrPtr[1] <= '9')
         {
            S32 argIndex = S32(fmtStrPtr[1] - '0') + 1;
            if (argIndex >= argc)
               goto done;
            const char *argStr = argv[argIndex];
            if (!argStr)
               goto done;
            S32 strLength = dStrlen(argStr);
            if (strLength > strMaxLength)
               goto done;
            dStrcpy(strBufPtr, argStr);
            strBufPtr += strLength;
            strMaxLength -= strLength;
            fmtStrPtr += 2;
            continue;
         }
      }

      //if we don't continue, just copy the character
      if (strMaxLength <= 0)
         goto done;
      *strBufPtr++ = *fmtStrPtr++;
      strMaxLength--;
   }

done:
   *strBufPtr = '\0';
   return strBuffer;
}

ConsoleFunctionGroupEnd( Net );

/*! @} */ // group TelnetDebuggerFunctions
