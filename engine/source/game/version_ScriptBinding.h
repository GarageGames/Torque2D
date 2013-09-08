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

/*! @defgroup BuildInfoFunctions Build Info
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the isDebugBuild function to determine if this is a debug build.
    @return Returns true if this is a debug build, otherwise false.
    @sa getBuildString, getCompileTimeString, getVersionNumber, getVersionString
*/
ConsoleFunctionWithDocs(isDebugBuild, ConsoleBool, 1, 1, ())
{
#ifdef TORQUE_DEBUG
   return true;
#else
   return false;
#endif
}

/*! Use the getVersionNumber function to get the version number of the currently executing engine.
    @return Returns an integer representing the engine's version number.
    @sa getBuildString, getCompileTimeString, getVersionString, isDebugBuild
*/
ConsoleFunctionWithDocs( getVersionNumber, ConsoleInt, 1, 1, ())
{
   return getVersionNumber();
}

/*! Use the getVersionString function to get the version name and number for the currently executing engine.
    @return Returns a string containing a name and an integer representing the engine's version type and version number.
    @sa getBuildString, getCompileTimeString, getVersionNumber, isDebugBuild
*/
ConsoleFunctionWithDocs( getVersionString, ConsoleString, 1, 1, ())
{
   return getVersionString();
}

/*! Use the getCompileTimeString function to determine when the currently running engine was built.
    @return Returns a string containing \Month Day Year at Hour:Minute:Second\ showing when this executable was built.
    @sa getBuildString, getVersionNumber, getVersionString, isDebugBuild
*/
ConsoleFunctionWithDocs( getCompileTimeString, ConsoleString, 1, 1, ())
{
   return getCompileTimeString();
}

/*! Use the getBuildString function to determine if this build is a \Debug\ release, or a \Release\ build.
    @return Returns a string, either \Debug\ for a debug build, or \Release\ for a release build.
    @sa getCompileTimeString, getVersionNumber, getVersionString, isDebugBuild
*/
ConsoleFunctionWithDocs( getBuildString, ConsoleString, 1, 1, ())
{
#ifdef TORQUE_DEBUG
   return "Debug";
#else
   return "Release";
#endif
}

//-----------------------------------------------------------------------------

/*! Gets the engine version.
*/
ConsoleFunctionWithDocs( getEngineVersion, ConsoleString, 1, 1, ())
{
    return T2D_ENGINE_VERSION;
}

//-----------------------------------------------------------------------------

/*! Returns iPhone Tools Version
*/
ConsoleFunctionWithDocs( getiPhoneToolsVersion, ConsoleString, 1, 1, ())
{
    return T2D_IPHONETOOLS_VERSION;
}

//-----------------------------------------------------------------------------

/*! Sets the company and product information.
*/
ConsoleFunctionWithDocs(setCompanyAndProduct, ConsoleVoid, 3, 3, (company, product))
{
   setCompanyName(StringTable->insert(argv[1]));
   setProductName(StringTable->insert(argv[2]));

   Con::setVariable("$Game::CompanyName", getCompanyName());
   Con::setVariable("$Game::ProductName", getProductName());

   char appDataPath[1024];
   dSprintf(appDataPath, sizeof(appDataPath), "%s/%s/%s", Platform::getUserDataDirectory(), getCompanyName(), getProductName());
   
   ResourceManager->addPath(appDataPath);
}

/*! @{ */ // group BuildInfoFunctions
