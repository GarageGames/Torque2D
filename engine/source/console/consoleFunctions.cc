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
#include "console/console.h"
#include "io/resource/resourceManager.h"
#include "platform/platformInput.h"

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif

#ifdef TORQUE_ALLOW_JOURNALING
   static U32 journalDepth = 1;
#endif

//----------------------------------------------------------------

#include "string/stringUnit.h"

ConsoleFunctionGroupBegin( TaggedStrings, "Functions dealing with tagging/detagging strings.");

ConsoleFunction(detag, const char *, 2, 2, "( tagID ) Use the detag function to convert a tag to a string. This can only be used in the proper context, i.e. to parse values passed to a client command or to a server command. See 'Remote Procedure Call Samples' below.\n"
                                                                "@param tagID A numeric tag ID corresponding to a previously tagged string.\n"
                                                                "@return Returns the string associated with the tag ID.\n"
                                                                "@sa commandToClient, commandToServer")
{
   TORQUE_UNUSED( argc );
   if(argv[1][0] == StringTagPrefixByte)
   {
      const char *word = dStrchr(argv[1], ' ');
      if(word == NULL)
         return "";
      char *ret = Con::getReturnBuffer(dStrlen(word + 1) + 1);
      dStrcpy(ret, word + 1);
      return ret;
   }
   else
      return argv[1];
}

ConsoleFunction(getTag, const char *, 2, 2, "( taggedString ) Use the getTag function to retrieve the tag ID associated with a previously tagged string.\n"
                                                                "@param taggedString A previously tagged string.\n"
                                                                "@return Returns the tag ID of the string. If the string was not previously tagged, it gets tagged and the new tag ID is returned")
{
   TORQUE_UNUSED( argc );
   if(argv[1][0] == StringTagPrefixByte)
   {
      const char * space = dStrchr(argv[1], ' ');

      U32 len;
      if(space)
         len = space - argv[1];
      else
         len = dStrlen(argv[1]) + 1;

      char * ret = Con::getReturnBuffer(len);
      dStrncpy(ret, argv[1] + 1, len - 1);
      ret[len - 1] = 0;

      return(ret);
   }
   else
      return(argv[1]);
}

ConsoleFunctionGroupEnd( TaggedStrings );

//----------------------------------------------------------------

ConsoleFunctionGroupBegin( InputManagement, "Functions that let you deal with input from scripts" );

ConsoleFunction( deactivateDirectInput, void, 1, 1, "() Use the deactivateDirectInput function to de-activate polling of direct input devices (keyboard, mouse, joystick, et cetera).\n"
                                                                "@return No return value.\n"
                                                                "@sa activateDirectInput")
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   if ( Input::isActive() )
      Input::deactivate();
}

//--------------------------------------------------------------------------
ConsoleFunction( activateDirectInput, void, 1, 1, "() Use the activateDirectInput function to activate polling of direct input devices (keyboard, mouse, joystick, et cetera).\n"
                                                                "@return No return value.\n"
                                                                "@sa deactivateDirectInput")
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   if ( !Input::isActive() )
      Input::activate();
}

ConsoleFunctionGroupEnd( InputManagement );

ConsoleFunction(openFolder, void, 2, 2, "(folder) Opens a folder of the platform")
{
    Platform::openFolder(argv[1]);
}

//Luma:	Console function to tell if this is a TORQUE_OS_IOS build
ConsoleFunction(isiPhoneBuild, bool, 1, 1, "Returns true if this is a iPhone build, false otherwise")
{
#ifdef	TORQUE_OS_IOS
   return true;
#else
   return false;
#endif	//TORQUE_OS_IOS
}

