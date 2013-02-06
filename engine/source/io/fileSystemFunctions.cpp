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
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "io/resource/resourceManager.h"
#include "io/fileStream.h"
#include "console/compiler.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platform/platformInput.h"
#include "torqueConfig.h"
#include "memory/frameAllocator.h"

// Buffer for expanding script filenames.
static char scriptFilenameBuffer[1024];

//-------------------------------------- Helper Functions
static void forwardslash(char *str)
{
   while(*str)
   {
      if(*str == '\\')
         *str = '/';
      str++;
   }
}

static void backslash(char *str)
{
   while(*str)
   {
      if(*str == '/')
         *str = '\\';
      str++;
   }
}


//----------------------------------------------------------------
ConsoleFunctionGroupBegin( FileSystem, "Functions allowing you to search for files, read them, write them, and access their properties.");

static ResourceObject *firstMatch = NULL;

ConsoleFunction(findFirstFile, const char *, 2, 2, "( strPattern )\n"
                "@param strPattern The string pattern to search for.\n"
                "@return Returns a string representing the first file in the directory system matching the given pattern.")
{
   TORQUE_UNUSED( argc );
   const char *fn = NULL;
   firstMatch = NULL;
   if(Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatch(scriptFilenameBuffer, &fn, NULL);

   if(firstMatch)
      return fn;
   else
      return "";
}

ConsoleFunction(findNextFile, const char *, 2, 2, "( strPattern )\n" 
                "@param strPattern The string pattern to search for.\n"
                "@return Returns the next file matching a search begun in findFirstFile.")
{
   TORQUE_UNUSED( argc );
   const char *fn = NULL;
   if(Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatch(scriptFilenameBuffer, &fn, firstMatch);
   else
      firstMatch = NULL;

   if(firstMatch)
      return fn;
   else
      return "";
}

ConsoleFunction(getFileCount, S32, 2, 2, "(strPattern) \n" 
                "@param strPattern The string pattern to search for.\n"
                "@return Returns the number of files in the directory tree that match the given pattern")
{
   TORQUE_UNUSED( argc );
   const char* fn;
   U32 count = 0;
   firstMatch = ResourceManager->findMatch(argv[1], &fn, NULL);
   if ( firstMatch )
   {
      count++;
      while ( 1 )
      {
         firstMatch = ResourceManager->findMatch(argv[1], &fn, firstMatch);
         if ( firstMatch )
            count++;
         else
            break;
      }
   }

   return( count );
}

ConsoleFunction(findFirstFileMultiExpr, const char *, 2, 2, "(strPattern)\n" 
                "@param strPattern The string pattern to search for.\n"
                "@return Returns the first file in the directory system matching the given pattern.")
{
   TORQUE_UNUSED( argc );
   const char *fn = NULL;
   firstMatch = NULL;
   if(Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatchMultiExprs(scriptFilenameBuffer, &fn, NULL);

   if(firstMatch)
      return fn;
   else
      return "";
}

ConsoleFunction(findNextFileMultiExpr, const char *, 2, 2, "(string pattern) Returns the next file matching a search begun in findFirstFile.")
{
   TORQUE_UNUSED( argc );
   const char *fn = NULL;
   if(Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatchMultiExprs(scriptFilenameBuffer, &fn, firstMatch);
   else
      firstMatch = NULL;

   if(firstMatch)
      return fn;
   else
      return "";
}

ConsoleFunction(getFileCountMultiExpr, S32, 2, 2, "(strPattern) \n" 
                "@param strPattern The string pattern to search for.\n"
                "@return Returns the number of files in the directory tree that match the given pattern")
{
   TORQUE_UNUSED( argc );
   const char* fn;
   U32 count = 0;
   firstMatch = ResourceManager->findMatchMultiExprs(argv[1], &fn, NULL);
   if ( firstMatch )
   {
      count++;
      while ( 1 )
      {
         firstMatch = ResourceManager->findMatchMultiExprs(argv[1], &fn, firstMatch);
         if ( firstMatch )
            count++;
         else
            break;
      }
   }

   return( count );
}

ConsoleFunction(getFileCRC, S32, 2, 2, "(filename)\n"
                "@param filename The string representing the file from which to get the CRC\n"
                "@return An integer")
{
   TORQUE_UNUSED( argc );
   U32 crcVal;
   Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);

   if(!ResourceManager->getCrc(scriptFilenameBuffer, crcVal))
      return(-1);
   return(S32(crcVal));
}

ConsoleFunction(isDirectory, bool, 2, 2, "(path)\n @param pathName Path to check.\n @return Returns true if the given path is a folder/directory, false otherwise")
{
    bool doesExist = Platform::isDirectory(argv[1]);
    if(doesExist)
        return true;

    return false;
}

ConsoleFunction(isFile, bool, 2, 2, "(fileName)\n"
                "@param fileName Filename to check.\n"
                "@return Returns true if the given filename is an existing file or false otherwise")
{
   Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   return bool(ResourceManager->find(scriptFilenameBuffer));
}

ConsoleFunction(isWriteableFileName, bool, 2, 2, "(fileName)\n"
                "@param fileName Filename to check.\n"
                "@return Returns true if the given filename is an existing file and is not Read-Only or false otherwise")
{
   TORQUE_UNUSED( argc );

   char filename[1024];
   Con::expandPath(filename, sizeof(filename), argv[1]);

   if (filename == NULL || *filename == 0)
      return false;

   // in a writeable directory?
   if(!ResourceManager->isValidWriteFileName(filename))
      return(false);

   // exists?
   FileStream fs;
   if(!fs.open(filename, FileStream::Read))
      return(true);

   // writeable? (ReadWrite will create file if it does not exist)
   fs.close();
   if(!fs.open(filename, FileStream::ReadWrite))
      return(false);

   return(true);
}

ConsoleFunction(getDirectoryList, const char*, 2, 3, "(strPath, [intDepth])\n"
                "@param strPath The path in which to check\n"
                "@param intDepth The depth in which to return (default 0 if not specified)\n"
                "@return The directory contents")
{
   // Grab the full path.
   char path[1024];
   Platform::makeFullPathName(dStrcmp(argv[1], "/") == 0 ? "" : argv[1], path, sizeof(path));

   // Append a trailing backslash if it's not present already.
   if (path[dStrlen(path) - 1] != '/')
   {
      S32 pos = dStrlen(path);
      path[pos] = '/';
      path[pos + 1] = '\0';
   }

   // Grab the depth to search.
   S32 depth = 0;
   if (argc > 2)
      depth = dAtoi(argv[2]);

   // Dump the directories.
   Vector<StringTableEntry> directories;
   Platform::dumpDirectories(path, directories, depth, true);

   if( directories.empty() )
      return "";

   // Grab the required buffer length.
   S32 length = 0;

   for (S32 i = 0; i < directories.size(); i++)
      length += dStrlen(directories[i]) + 1;

   // Get a return buffer.
   char* buffer = Con::getReturnBuffer(length);
   char* p = buffer;

   // Copy the directory names to the buffer.
   for (S32 i = 0; i < directories.size(); i++)
   {
      dStrcpy(p, directories[i]);
      p += dStrlen(directories[i]);
      // Tab separated.
      p[0] = '\t';
      p++;
   }
   p--;
   p[0] = '\0';

   return buffer;
}

ConsoleFunction(getFileList, const char*, 2, 2, "(strPath) - Gets all the files in the specified directory.\n"
                "@param strPath The path in which to check\n"
                "@return A list of files in the specified directory.")
{
   // Grab the full path.
   char basePath[1024];
   Con::expandPath( basePath, sizeof(basePath), argv[1] );

   Vector<Platform::FileInfo> files;
   if ( !Platform::dumpPath( basePath, files, 0 ) )
   {
       Con::warnf("Failed to get file list in directory '%s'", basePath );
       return "";
   }

   if ( files.size() == 0 )
       return "";

   // Grab the required buffer length.
   S32 length = 0;

   for (S32 i = 0; i < files.size(); i++)
       length += dStrlen(files[i].pFileName) + 1;

   // Get a return buffer.
   char* buffer = Con::getReturnBuffer(length);
   char* p = buffer;

   // Copy the directory names to the buffer.
   for (S32 i = 0; i < files.size(); i++)
   {
       dStrcpy(p, files[i].pFileName);
       p += dStrlen(files[i].pFileName);
      // Tab separated.
      p[0] = '\t';
      p++;
   }
   p--;
   p[0] = '\0';

   return buffer;
}

ConsoleFunction(fileSize, S32, 2, 2, "(fileName)\n"
                "@param fileName The name of the file to check.\n" 
                "@return Returns the size of the file as an integer or -1 if file not found")
{
   TORQUE_UNUSED( argc );
   Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   return Platform::getFileSize( scriptFilenameBuffer );
}

ConsoleFunction(fileDelete, bool, 2,2, "(fileName)\n"
                "@param fileName Name of the file to remove"
                "@return Returns true on success and false on failure")
{
   static char fileName[1024];
   static char sandboxFileName[1024];

   Con::expandPath( fileName, sizeof( fileName ), argv[1] );
   Platform::makeFullPathName(fileName, sandboxFileName, sizeof(sandboxFileName));

   return Platform::fileDelete(sandboxFileName);
}

ConsoleFunction(directoryDelete, bool, 2,2, "(directoryName)\n"
                "@param directoryName Name of the directory to remove"
                "@return Returns true on success and false on failure")
{
   static char directoryName[1024];
   static char sandboxdirectoryName[1024];

   Con::expandPath( directoryName, sizeof( directoryName ), argv[1] );
   Platform::makeFullPathName(directoryName, sandboxdirectoryName, sizeof(sandboxdirectoryName));

   return Platform::deleteDirectory(sandboxdirectoryName);
}

//----------------------------------------------------------------

ConsoleFunction(isValidImageFile, bool, 2, 2, "(string filePath)\n"
                "@param filePath Full path of file to check.\n"
                "@return Returns true if the given filename is a valid image file or false otherwise")
{
    Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
    
    // does file exist?
    if (!ResourceManager->find(scriptFilenameBuffer))
        return false;

    const char *ext = dStrrchr(argv[1], '.');
    if (!ext)
        return false;	// no extension

    Stream *stream = ResourceManager->openStream(scriptFilenameBuffer);
    if(stream == NULL)
        return false;

    bool ret = false;

    if (dStricmp(ext, ".jpg") == 0)
    {
        U8 bArray[2];
        stream->read(2, bArray);
        // check header signature
        ret = ((bArray[0] == 0xFF) && (bArray[1] == 0xD8));
    } 
    else if (dStricmp(ext, ".png") == 0)
    {
        int i = 0;
        U8 bArray[8];
        stream->read(8, bArray);
        // check header signature
        ret = ((bArray[i++] == 0x89) && (bArray[i++] == 0x50) && (bArray[i++] == 0x4E) && (bArray[i++] == 0x47));
    }

    ResourceManager->closeStream(stream);

    return ret;
}

ConsoleFunction(fileExt, const char *, 2, 2, "(fileName)\n "
                "@param Name of file from which to extract extension\n"
                "@return A string containing the file's extension (assuming all data after first '.' is the extension) or returns empty string on failure")
{
   TORQUE_UNUSED( argc );
   const char *ret = dStrrchr(argv[1], '.');
   if(ret)
      return ret;
   return "";
}

ConsoleFunction(fileBase, const char *, 2, 2, "fileBase(fileName)")
{

   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);
   
   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   TORQUE_UNUSED( argc );
   const char *path = dStrrchr(szPathCopy, '/');
   if(!path)
      path = szPathCopy;
   else
      path++;
   char *ret = Con::getReturnBuffer(dStrlen(path) + 1);
   dStrcpy(ret, path);
   char *ext = dStrrchr(ret, '.');
   if(ext)
      *ext = 0;
   return ret;
}

ConsoleFunction(fileName, const char *, 2, 2, "(filePathName) Extract the filename from the full path description")
{
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);

   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   TORQUE_UNUSED( argc );
   const char *name = dStrrchr(szPathCopy, '/');
   if(!name)
      name = szPathCopy;
   else
      name++;
   char *ret = Con::getReturnBuffer(dStrlen(name));
   dStrcpy(ret, name);
   return ret;
}

ConsoleFunction(filePath, const char *, 2, 2, "(fileName) Extract the file path from a file's full desciption")
{
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);

   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   TORQUE_UNUSED( argc );
   const char *path = dStrrchr(szPathCopy, '/');
   if(!path)
      return "";
   U32 len = path - (char*)szPathCopy;
   char *ret = Con::getReturnBuffer(len + 1);
   dStrncpy(ret, szPathCopy, len);
   ret[len] = 0;
   return ret;
}

ConsoleFunction(getCurrentDate, const char *, 2, 2, "(bool reverse) Gets the current date and time.\n"
                                                 "@param reverse Determines if the format will be Month/Day/Year/Hour:Min:Seconds or Year/Month/Day/Hour:Min:Seconds")
{
    Platform::LocalTime lt;
    Platform::getLocalTime(lt);

    char temp[128];
    
    if (dAtob(argv[1]))
        dSprintf(temp, 256, "%d/%d/%d:%d:%d:%d", lt.year+1900, lt.month+1, lt.monthday, lt.hour, lt.min, lt.sec);
    else
        dSprintf(temp, 256, "%d/%d/%d:%d:%d:%d", lt.month+1, lt.monthday, lt.year+1900, lt.hour, lt.min, lt.sec);

    char* returnBuffer = Con::getReturnBuffer(dStrlen(temp + 1));
    dStrcpy(returnBuffer, temp);

    return returnBuffer;
}

ConsoleFunction(openFolder, void, 2 ,2,"openFolder(%path);")
{
   Platform::openFolder( argv[1] );
}

ConsoleFunction(pathCopy, bool, 3, 4, "pathCopy(fromFile, toFile [, nooverwrite = true])")
{
   bool nooverwrite = true;

   if( argc > 3 )
      nooverwrite = dAtob( argv[3] );

   static char fromFile[1024];
   static char toFile[1024];

   static char qualifiedFromFile[1024];
   static char qualifiedToFile[1024];

   Con::expandPath( fromFile, sizeof( fromFile ), argv[1] );
   Con::expandPath( toFile, sizeof( toFile ), argv[2] );

   Platform::makeFullPathName(fromFile, qualifiedFromFile, sizeof(qualifiedFromFile));
   Platform::makeFullPathName(toFile, qualifiedToFile, sizeof(qualifiedToFile));

   return Platform::pathCopy( qualifiedFromFile, qualifiedToFile, nooverwrite );
}

ConsoleFunction(getCurrentDirectory, const char *, 1, 1, "getCurrentDirectory()")
{
   return Platform::getCurrentDirectory();
}

ConsoleFunction( setCurrentDirectory, bool, 2, 2, "setCurrentDirectory(absolutePathName)" )
{
   return Platform::setCurrentDirectory( StringTable->insert( argv[1] ) );

}

ConsoleFunction(getExecutableName, const char *, 1, 1, "getExecutableName()")
{
   return Platform::getExecutableName();
}

ConsoleFunction(getMainDotCsDir, const char *, 1, 1, "getMainDotCsDir()")
{
   return Platform::getMainDotCsDir();
}

ConsoleFunction(makeFullPath, const char *, 2, 3, "(string path, [string currentWorkingDir])")
{
   char *buf = Con::getReturnBuffer(512);
   Platform::makeFullPathName(argv[1], buf, 512, argc > 2 ? argv[2] : NULL);
   return buf;
}

ConsoleFunction(makeRelativePath, const char *, 3, 3, "(string path, string to)")
{
   return Platform::makeRelativePathName(argv[1], argv[2]);
}

ConsoleFunction(pathConcat, const char *, 3, 0, "(string path, string file1, [... fileN])")
{
   char *buf = Con::getReturnBuffer(1024);
   char pathBuf[1024];
   dStrcpy(buf, argv[1]);

   for(S32 i = 2;i < argc;++i)
   {
      Platform::makeFullPathName(argv[i], pathBuf, 1024, buf);
      dStrcpy(buf, pathBuf);
   }
   return buf;
}

ConsoleFunction(restartInstance, void, 1, 1, "restartInstance()")
{
   Game->setRestart( true );
   Platform::postQuitMessage( 0 );
}

ConsoleFunction( createPath, bool, 2,2, "createPath(\"file name or path name\");  creates the path or path to the file name")
{
   static char pathName[1024];

   Con::expandPath( pathName, sizeof( pathName ), argv[1] );

   return Platform::createPath( pathName );
}

ConsoleFunctionGroupEnd( FileSystem );
