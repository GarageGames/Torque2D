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

ConsoleFunctionGroupBegin( Net, "Functions for use with the network; tagged strings and remote commands.");

/*! @defgroup NetworkFunctions Network
	@ingroup TorqueScriptFunctions
	@{
*/



#ifdef TORQUE_DEBUG

ConsoleFunction(dumpResources, void, 2, 2, "(onlyLoaded?) Use the dumpLoadedResources function to dump a listing of the currently in-use resources to the console. This will include such things as sound files, font files, etc.\n"
                                                                "For this to work, the engine must have been compiled with TORQUE_DEBUG defined.\n"
                                                                "@return No return value.\n"
                                                                "@sa purgeResources")
{
    const bool onlyLoaded = argc == 2 ? dAtob(argv[1]) : true;
    ResourceManager->dumpResources(onlyLoaded);
}

#endif

ConsoleFunction(addResPath, void, 2, 3, "(path, [ignoreZips=false]) Add a path to the resource manager")
{
   if( argc > 2 )
      ResourceManager->addPath(argv[1], dAtob(argv[2]));
   else
      ResourceManager->addPath(argv[1]);
}


ConsoleFunction(removeResPath, void, 2, 2, "(pathExpression) Remove a path from the resource manager. Path is an expression as in findFirstFile()")
{
   ResourceManager->removePath(argv[1]);
}

// Mod paths aren't used in tools applications.  
// See : addResPath/removeResPath console functions
ConsoleFunction( setModPaths, void, 2, 2, "( path ) Use the setModPaths function to set the current mod path to the value specified in path.\n"
                                                                "@param path A string containing a semi-colon (;) separated list of game and mod paths.\n"
                                                                "@return No return value.\n"
                                                                "@sa getModPaths")
{
   char buf[512];
   dStrncpy(buf, argv[1], sizeof(buf) - 1);
   buf[511] = '\0';

   Vector<char *> paths;
   char* temp = dStrtok( buf, ";" );
   while ( temp )
   {
      if ( temp[0] )
         paths.push_back(temp);
      
      temp = dStrtok( NULL, ";" );
   }

   ResourceManager->setModPaths( paths.size(), (const char**) paths.address() );
}

ConsoleFunction( getModPaths, const char*, 1, 1, "() Use the getModPaths function to get the current mod path information.\n"
                                                                "@return Returns a string equivalent to the complete current mod path, that is all pads that are visible to the file manager.\n"
                                                                "@sa setModPaths")
{
   return( ResourceManager->getModPaths() );
}

ConsoleFunction( purgeResources, void, 1, 1, "() Use the purgeResources function to purge all game resources.\n"
                                                                "@return No return value.\n"
                                                                "@sa clearTextureHolds, dumpResourceStats, dumpTextureStats, flushTextureCache")
{
   ResourceManager->purge();
}

ConsoleFunction(isUsingVFS, bool, 1, 1, "()\n"
                "@return Returns true if using Virtual File System")
{
   return ResourceManager->isUsingVFS();
}


/*! @} */ // group NetworkFunctions
