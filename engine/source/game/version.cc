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
#include "game/version.h"
#include "console/console.h"
#include "string/stringTable.h"
#include "io/resource/resourceManager.h"

//-----------------------------------------------------------------------------

static const U32 csgVersionNumber = TORQUE_GAME_ENGINE;
static StringTableEntry sgCompanyName = NULL;
static StringTableEntry sgProductName = NULL;

//-----------------------------------------------------------------------------

U32 getVersionNumber()
{
   return csgVersionNumber;
}

//-----------------------------------------------------------------------------

const char* getVersionString()
{
   return TORQUE_GAME_VERSION_STRING;
}

//-----------------------------------------------------------------------------

const char* getCompileTimeString()
{
   return __DATE__ " at " __TIME__;
}
//----------------------------------------------------------------

ConsoleFunction(isDebugBuild, bool, 1, 1, "() Use the isDebugBuild function to determine if this is a debug build.\n"
                                                                "@return Returns true if this is a debug build, otherwise false.\n"
                                                                "@sa getBuildString, getCompileTimeString, getVersionNumber, getVersionString")
{
#ifdef TORQUE_DEBUG
   return true;
#else
   return false;
#endif
}

ConsoleFunction( getVersionNumber, S32, 1, 1, "() Use the getVersionNumber function to get the version number of the currently executing engine.\n"
                                                                "@return Returns an integer representing the engine's version number.\n"
                                                                "@sa getBuildString, getCompileTimeString, getVersionString, isDebugBuild")
{
   return getVersionNumber();
}

ConsoleFunction( getVersionString, const char*, 1, 1, "() Use the getVersionString function to get the version name and number for the currently executing engine.\n"
                                                                "@return Returns a string containing a name and an integer representing the engine's version type and version number.\n"
                                                                "@sa getBuildString, getCompileTimeString, getVersionNumber, isDebugBuild")
{
   return getVersionString();
}

ConsoleFunction( getCompileTimeString, const char*, 1, 1, "() Use the getCompileTimeString function to determine when the currently running engine was built.\n"
                                                                "@return Returns a string containing \"Month Day Year at Hour:Minute:Second\" showing when this executable was built.\n"
                                                                "@sa getBuildString, getVersionNumber, getVersionString, isDebugBuild")
{
   return getCompileTimeString();
}

ConsoleFunction( getBuildString, const char*, 1, 1, "() Use the getBuildString function to determine if this build is a \"Debug\" release, or a \"Release\" build.\n"
                                                                "@return Returns a string, either \"Debug\" for a debug build, or \"Release\" for a release build.\n"
                                                                "@sa getCompileTimeString, getVersionNumber, getVersionString, isDebugBuild")
{
#ifdef TORQUE_DEBUG
   return "Debug";
#else
   return "Release";
#endif
}

//-----------------------------------------------------------------------------

ConsoleFunction( getEngineVersion, const char*, 1, 1, "() - Gets the engine version.")
{
    return T2D_ENGINE_VERSION;
}

//-----------------------------------------------------------------------------

ConsoleFunction( getiPhoneToolsVersion, const char*, 1, 1, "Returns iPhone Tools Version")
{
    return T2D_IPHONETOOLS_VERSION;
}

//-----------------------------------------------------------------------------

ConsoleFunction(setCompanyAndProduct, void, 3, 3, "(company, product) Sets the company and product information.")
{
   sgCompanyName = StringTable->insert(argv[1]);
   sgProductName = StringTable->insert(argv[2]);

   Con::setVariable("$Game::CompanyName", sgCompanyName);
   Con::setVariable("$Game::ProductName", sgProductName);

   char appDataPath[1024];
   dSprintf(appDataPath, sizeof(appDataPath), "%s/%s/%s", Platform::getUserDataDirectory(), sgCompanyName, sgProductName);
   
   ResourceManager->addPath(appDataPath);
}

