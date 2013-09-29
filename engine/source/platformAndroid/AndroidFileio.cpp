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
#include "platformAndroid/platformAndroid.h"
#include "platform/platformFileIO.h"
#include "collection/vector.h"
#include "string/stringTable.h"
#include "string/stringUnit.h"
#include "console/console.h"
#include "debug/profiler.h"
#include "io/resource/resourceManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <utime.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_MAC_PATH_LONG     2048

//-----------------------------------------------------------------------------
bool Platform::fileDelete(const char * name)
{
   Con::warnf("Platform::FileDelete() - Not supported on android.");
   
   return false;
}


//-----------------------------------------------------------------------------
bool dFileTouch(const char *path)
{
	Con::warnf("Platform::dFileTouch() - Not supported on android.");

	return true;
}

//-----------------------------------------------------------------------------
// Constructors & Destructor
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// After construction, the currentStatus will be Closed and the capabilities
// will be 0.
//-----------------------------------------------------------------------------
File::File()
: currentStatus(Closed), capability(0)
{
   buffer = NULL;
   size = 0;
   filePointer = 0;
}

//-----------------------------------------------------------------------------
// insert a copy constructor here... (currently disabled)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
File::~File()
{
	if (buffer != NULL)
		close();
}


//-----------------------------------------------------------------------------
// Open a file in the mode specified by openMode (Read, Write, or ReadWrite).
// Truncate the file if the mode is either Write or ReadWrite and truncate is
// true.
//
// Sets capability appropriate to the openMode.
// Returns the currentStatus of the file.
//-----------------------------------------------------------------------------
File::Status File::open(const char *filename, const AccessMode openMode)
{
   if (dStrlen(filename) > MAX_MAC_PATH_LONG)
      Con::warnf("File::open: Filename length is pretty long...");
   
   // Close the file if it was already open...
   if (currentStatus != Closed || buffer != NULL)
      close();
   
   // create the appropriate type of file...
   switch (openMode)
   {
      case Read:
    	filePointer = 0;
    	buffer = (U8*)_AndroidLoadFile(filename, &size);
		if (buffer == NULL) {
			currentStatus = UnknownError;
			capability = FileRead;
			return currentStatus;
		}
		break;
      case Write:
    	  //AssertFatal(false, "File::open: Write not supported on Android");
    	  //Platform::getUserDataDirectory()
          return currentStatus;
      case ReadWrite:
    	  //AssertFatal(false, "File::open: ReadWrite not supported on Android");
    	  return currentStatus;
      case WriteAppend:
    	  //AssertFatal(false, "File::open: WriteAppend not supported on Android");
    	  return currentStatus;
      default:
         AssertFatal(false, "File::open: bad access mode");
   }
   
   capability = FileRead;
   
   // must set the file status before setting the position.
   currentStatus = Ok;

   // success!
   return currentStatus;                                
}

//-----------------------------------------------------------------------------
// Get the current position of the file pointer.
//-----------------------------------------------------------------------------
U32 File::getPosition() const
{
   AssertFatal(currentStatus != Closed , "File::getPosition: file closed");
   AssertFatal(buffer != NULL, "File::getPosition: invalid file buffer");
   
   return filePointer;
}

//-----------------------------------------------------------------------------
// Set the position of the file pointer.
// Absolute and relative positioning is supported via the absolutePos
// parameter.
//
// If positioning absolutely, position MUST be positive - an IOError results if
// position is negative.
// Position can be negative if positioning relatively, however positioning
// before the start of the file is an IOError.
//
// Returns the currentStatus of the file.
//-----------------------------------------------------------------------------
File::Status File::setPosition(S32 position, bool absolutePos)
{
   AssertFatal(Closed != currentStatus, "File::setPosition: file closed");
   AssertFatal(buffer != NULL, "File::setPosition: invalid file buffer");
   
   if (currentStatus != Ok && currentStatus != EOS )
      return currentStatus;
   
   U32 finalPos;
   if(absolutePos)
   {
      // absolute position
      AssertFatal(0 <= position, "File::setPosition: negative absolute position");
      // position beyond EOS is OK
      filePointer = position;
      finalPos = filePointer;
   }
   else
   {
      // relative position
      AssertFatal((getPosition() + position) >= 0, "File::setPosition: negative relative position");
      // position beyond EOS is OK
      filePointer += position;
      finalPos = filePointer;
   }
   
   // ftell returns -1 on error. set error status
   if (0xffffffff == finalPos)
      return setStatus();

   // success, at end of file
   else if (finalPos >= getSize())
      return currentStatus = EOS;

   // success!
   else
      return currentStatus = Ok;
}

//-----------------------------------------------------------------------------
// Get the size of the file in bytes.
// It is an error to query the file size for a Closed file, or for one with an
// error status.
//-----------------------------------------------------------------------------
U32 File::getSize() const
{
   AssertWarn(Closed != currentStatus, "File::getSize: file closed");
   AssertFatal(buffer != NULL, "File::getSize: invalid file buffer");
   
   if (Ok == currentStatus || EOS == currentStatus)
   {
      return size;
   }
   
   return 0;
}

//-----------------------------------------------------------------------------
// Flush the file.
// It is an error to flush a read-only file.
// Returns the currentStatus of the file.
//-----------------------------------------------------------------------------
File::Status File::flush()
{
   AssertFatal(Closed != currentStatus, "File::flush: file closed");
   AssertFatal(buffer != NULL, "File::flush: invalid file buffer");
   AssertFatal(true == hasCapability(FileWrite), "File::flush: cannot flush a read-only file");
   
   return setStatus();
}

//-----------------------------------------------------------------------------
// Close the File.
//
// Returns the currentStatus
//-----------------------------------------------------------------------------
File::Status File::close()
{
   // check if it's already closed...
   if (Closed == currentStatus)
      return currentStatus;
   
   // it's not, so close it...
   if (buffer != NULL)
   {
		delete[] buffer;
		buffer = NULL;
		size = 0;
		filePointer = 0;
   }

   return currentStatus = Closed;
}

//-----------------------------------------------------------------------------
// Self-explanatory.
//-----------------------------------------------------------------------------
File::Status File::getStatus() const
{
   return currentStatus;
}

//-----------------------------------------------------------------------------
// Sets and returns the currentStatus when an error has been encountered.
//-----------------------------------------------------------------------------
File::Status File::setStatus()
{
   currentStatus = UnknownError;
   return currentStatus;
}

//-----------------------------------------------------------------------------
// Sets and returns the currentStatus to status.
//-----------------------------------------------------------------------------
File::Status File::setStatus(File::Status status)
{
   return currentStatus = status;
}

//-----------------------------------------------------------------------------
// Read from a file.
// The number of bytes to read is passed in size, the data is returned in src.
// The number of bytes read is available in bytesRead if a non-Null pointer is
// provided.
//-----------------------------------------------------------------------------
File::Status File::read(U32 _size, char *dst, U32 *bytesRead)
{
   AssertFatal(Closed != currentStatus, "File::read: file closed");
   AssertFatal(buffer != NULL, "File::read: invalid file buffer");
   AssertFatal(NULL != dst, "File::read: NULL destination pointer");
   AssertFatal(true == hasCapability(FileRead), "File::read: file lacks capability");
   AssertWarn(0 != size, "File::read: size of zero");
   
   if (Ok != currentStatus || 0 == size)
      return currentStatus;
   
   // read from stream
   U32 nBytes = 0;
   
   if ((size-filePointer) > (_size))
   {
   		memcpy(dst, buffer+filePointer, _size);
   		nBytes = _size;
   }
   else if (size-filePointer <= 0)
   {
	    nBytes = 0;
   }
   else
   {
	   memcpy(dst, buffer+filePointer, size-filePointer);
	   nBytes = size-filePointer;
   }

   //Advanced the pointer
   filePointer += nBytes;

   // did we hit the end of the stream?
   if( nBytes != _size)
      currentStatus = EOS;
   
   // if bytesRead is a valid pointer, send number of bytes read there.
   if(bytesRead)
      *bytesRead = nBytes;
     
   // successfully read size bytes
   return currentStatus;
}

//-----------------------------------------------------------------------------
// Write to a file.
// The number of bytes to write is passed in size, the data is passed in src.
// The number of bytes written is available in bytesWritten if a non-Null
// pointer is provided.
//-----------------------------------------------------------------------------
File::Status File::write(U32 size, const char *src, U32 *bytesWritten)
{
   AssertFatal(0, "File::write: Not supported on Android.");
   return setStatus();
}


//-----------------------------------------------------------------------------
// Self-explanatory.
//-----------------------------------------------------------------------------
bool File::hasCapability(Capability cap) const
{
   return (0 != (U32(cap) & capability));
}

//-----------------------------------------------------------------------------
S32 Platform::compareFileTimes(const FileTime &a, const FileTime &b)
{
   if(a > b)
      return 1;
   if(a < b)
      return -1;
   return 0;
}


//-----------------------------------------------------------------------------
// either time param COULD be null.
//-----------------------------------------------------------------------------
bool Platform::getFileTimes(const char *path, FileTime *createTime, FileTime *modifyTime)
{
   // MacOSX is NOT guaranteed to be running off a HFS volume,
   // and UNIX does not keep a record of a file's creation time anywhere.
   // So instead of creation time we return changed time,
   // just like the Linux platform impl does.
   
   if (!path || !*path) 
      return false;
   
   Con::warnf("Platform::getFileTimes - Not supported on android.");
   
   return true;
}


//-----------------------------------------------------------------------------
bool Platform::createPath(const char *file)
{
   Con::warnf("Platform::createPath() - Not supported on android.");
   return false;
}


#pragma mark ---- Directories ----
//-----------------------------------------------------------------------------
StringTableEntry Platform::getCurrentDirectory()
{
   // get the current directory, the one that would be opened if we did a fopen(".")
   char* cwd = getcwd(NULL, 0);
   StringTableEntry ret = StringTable->insert(cwd);
   free(cwd);
   return ret;
}

//-----------------------------------------------------------------------------
bool Platform::setCurrentDirectory(StringTableEntry newDir)
{
   return (chdir(newDir) == 0);
}

//-----------------------------------------------------------------------------
void Platform::openFolder(const char* path )
{
   // TODO: users can still run applications by calling openfolder on an app bundle.
   // this may be a bad thing.
   if(!Platform::isDirectory(path))
   {
      Con::errorf(avar("Error: not a directory: %s",path));
      return;
   }
   
   const char* arg = avar("open '%s'", path);
   U32 ret = system(arg);
   if(ret != 0)
      Con::printf(strerror(errno));
}

static bool isMainDotCsPresent(char *dir)
{ 
   char maincsbuf[MAX_MAC_PATH_LONG];
   const char *maincsname = "/main.cs";
   const U32 len = dStrlen(dir) + dStrlen(maincsname)+1;
   AssertISV(len < MAX_MAC_PATH_LONG, "Sorry, path is too long, I can't run from this folder.");
   
   dSprintf(maincsbuf,MAX_MAC_PATH_LONG,"%s%s", dir, maincsname);
   
   return Platform::isFile(maincsbuf);
}

//-----------------------------------------------------------------------------
/// Finds and sets the current working directory.
/// Torque tries to automatically detect whether you have placed the game files 
/// inside or outside the application's bundle. It checks for the presence of 
/// the file 'main.cs'. If it finds it, Torque will assume that the other game
/// files are there too. If Torque does not see 'main.cs' inside its bundle, it
/// will assume the files are outside the bundle.
/// Since you probably don't want to copy the game files into the app every time
/// you build, you will want to leave them outside the bundle for development.
///
/// Android reads all assets out of compressed bundle so we dont realy have an executable path
StringTableEntry Platform::getExecutablePath()
{
   if(platState.mainDotCsDir) 
      return platState.mainDotCsDir;
      
   char* ret = NULL;
   if(StringTable)
      platState.mainDotCsDir = StringTable->insert(".");
   else
      ret = dStrdup(".");
   
   return ret ? ret : platState.mainDotCsDir;
}

//-----------------------------------------------------------------------------
StringTableEntry Platform::getExecutableName()
{
   return StringTable->insert("Torque2D");
}

//-----------------------------------------------------------------------------
bool Platform::isFile(const char *path)
{
   if (!path || !*path) 
      return false;

   return android_IsFile(path);
}


//-----------------------------------------------------------------------------
bool Platform::isDirectory(const char *path)
{
   if (!path || !*path) 
      return false;
   
   return android_IsDir(path);
}


S32 Platform::getFileSize(const char* pFilePath)
{
   if (!pFilePath || !*pFilePath) 
      return 0;
   
   return android_GetFileSize(pFilePath);
}


//-----------------------------------------------------------------------------
bool Platform::isSubDirectory(const char *pathParent, const char *pathSub)
{
   char fullpath[MAX_MAC_PATH_LONG];
   dStrcpyl(fullpath, MAX_MAC_PATH_LONG, pathParent, "/", pathSub, NULL);
   return isDirectory((const char *)fullpath);
}

void getDirectoryName(const char* path, char* name)
{
	int cnt = StringUnit::getUnitCount(path, "/");
	strcpy(name,StringUnit::getUnit(path, cnt-1, "/"));
}

//-----------------------------------------------------------------------------
// utility for platform::hasSubDirectory() and platform::dumpDirectories()
// ensures that the entry is a directory, and isnt on the ignore lists.
inline bool isGoodDirectory(const char* path)
{
   char name[80];
   getDirectoryName(path, name);
   return (Platform::isDirectory(path)                          // is a dir
           && dStrcmp(name,".") != 0                 // not here
           && dStrcmp(name,"..") != 0                // not parent
           && !Platform::isExcludedDirectory(name)); // not excluded
}

//-----------------------------------------------------------------------------
bool Platform::hasSubDirectory(const char *path) 
{
	android_InitDirList(path);
	char dir[80];
	char pdir[255];
	strcpy(dir,"");
	android_GetNextDir(path, dir);
	while(strcmp(dir,"") != 0)
	{
		sprintf(pdir, "%s/%s", path, dir);
		if (isGoodDirectory(pdir))
			return true;
		android_GetNextDir(path, dir);
	}

    return false;
}

//-----------------------------------------------------------------------------
bool recurseDumpDirectories(const char *basePath, const char *path, Vector<StringTableEntry> &directoryVector, S32 depth, bool noBasePath)
{
	char aPath[80];
   if (basePath[0] == '/')
   {
	   strcpy(aPath, basePath+1);
   }
   else
   {
	   strcpy(aPath, basePath);
   }

   const U32 len = dStrlen(aPath) + dStrlen(path) + 2;
   char pathbuf[len];
   
   // construct the file path
   if (strcmp(path,"") != 0)
	   dSprintf(pathbuf, len, "%s/%s", aPath, path);
   else
	   strcpy(pathbuf, aPath);
   
   // be sure it opens.
   android_InitDirList(pathbuf);
   
   // look inside the current directory
   char dir[80];
   char pdir[255];
   strcpy(dir,"");
   android_GetNextDir(pathbuf, dir);
   while(strcmp(dir,"") != 0)
   {
		sprintf(pdir, "%s/%s", pathbuf, dir);
		if (!isGoodDirectory(pdir))
			return false;
      
		// construct the new path string, we'll need this below.
        const U32 newpathlen = dStrlen(path) + dStrlen(dir) + 2;
        char newpath[newpathlen];
        if(dStrlen(path) > 0)
        {
            dSprintf(newpath, newpathlen, "%s/%s", path, dir);
        }
        else
        {
           dSprintf(newpath, newpathlen, "%s", dir);
        }
      
        // we have a directory, add it to the list.
        if( noBasePath )
        {
           directoryVector.push_back(StringTable->insert(newpath));
        }
        else
        {
           const U32 fullpathlen = dStrlen(aPath) + dStrlen(newpath) + 2;
           char fullpath[fullpathlen];
           dSprintf(fullpath, fullpathlen, "%s/%s",aPath,newpath);
           directoryVector.push_back(StringTable->insert(fullpath));
        }
      
        // and recurse into it, unless we've run out of depth
        if( depth != 0) // passing a val of -1 as the recurse depth means go forever
           recurseDumpDirectories(aPath, newpath, directoryVector, depth-1, noBasePath);

        android_GetNextDir(pathbuf, dir);
     }
     return true;
}

//-----------------------------------------------------------------------------
bool Platform::dumpDirectories(const char *path, Vector<StringTableEntry> &directoryVector, S32 depth, bool noBasePath)
{
   PROFILE_START(dumpDirectories);

   ResourceManager->initExcludedDirectories();

   const S32 len = dStrlen(path)+1;
   char newpath[len];
   if (path[0] == '/')
   {
	   dSprintf(newpath, len-1, "%s", path+1);
   }
   else
   {
	   dSprintf(newpath, len, "%s", path);
   }

   if(newpath[len - 1] == '/')
      newpath[len - 1] = '\0'; // cut off the trailing slash, if there is one
   
    // Insert base path to follow what Windows does.
    if ( !noBasePath )
        directoryVector.push_back(StringTable->insert(newpath));
   
    bool ret = recurseDumpDirectories(newpath, "", directoryVector, depth, noBasePath);
   PROFILE_END();
   
   return ret;
}

//-----------------------------------------------------------------------------
static bool recurseDumpPath(const char* curPath, Vector<Platform::FileInfo>& fileVector, U32 depth)
{
   android_InitDirList(curPath);
   
   // look inside the current directory
   char dir[80];
   char file[80];
   strcpy(dir,"");
   strcpy(file,"");
   android_GetNextDir(curPath, dir);
   android_GetNextFile(curPath, file);

   while(strcmp(file,"") != 0)
   {
  	 // construct the full file path. we need this to get the file size and to recurse
  	 const U32 len = dStrlen(curPath) + dStrlen(file) + 2;
  	 char pathbuf[len];
  	 dSprintf( pathbuf, len, "%s/%s", curPath, file);

  	 //add the file entry to the list
  	 // unlike recurseDumpDirectories(), we need to return more complex info here.
  	  //<Mat> commented this out in case we ever want a dir file printout again
  	  //printf( "File Name: %s ", entry->d_name );
  	 const U32 fileSize = Platform::getFileSize(pathbuf);
  	 fileVector.increment();
  	 Platform::FileInfo& rInfo = fileVector.last();
  	 rInfo.pFullPath = StringTable->insert(curPath);
  	 rInfo.pFileName = StringTable->insert(file);
  	 rInfo.fileSize  = fileSize;

  	 android_GetNextFile(curPath, file);
   }

   while(strcmp(dir,"") != 0)
   {
      // construct the full file path. we need this to get the file size and to recurse
      const U32 len = dStrlen(curPath) + dStrlen(dir) + 2;
      char pathbuf[len];
      if (strcmp(curPath,"") == 0)
    	  dSprintf( pathbuf, len, "%s", dir);
      else
    	  dSprintf( pathbuf, len, "%s/%s", curPath, dir);
      
	  if( depth == 0)
	  {
		  android_GetNextDir(curPath, dir);
		continue;
	  }

	  // filter out dirs we dont want.
	  if( !isGoodDirectory(pathbuf) )
	  {
		android_GetNextDir(curPath, dir);
		continue;
	  }

	  // recurse into the dir
	  recurseDumpPath( pathbuf, fileVector, depth-1);

	  android_GetNextDir(curPath, dir);
   }

   return true;
   
}


//-----------------------------------------------------------------------------
bool Platform::dumpPath(const char *path, Vector<Platform::FileInfo>& fileVector, S32 depth)
{
   PROFILE_START(dumpPath);
   char apath[80];
   if (path[0] == '/')
   {
	   strcpy(apath, path+1);
   }
   else
   {
	   strcpy(apath, path);
   }
    const S32 len = dStrlen(apath) + 1;
   char newpath[len];
   
    dSprintf(newpath, len, "%s", apath);
    
   if(newpath[len - 2] == '/')
      newpath[len - 2] = '\0'; // cut off the trailing slash, if there is one
   
   bool ret = recurseDumpPath( newpath, fileVector, depth);
   PROFILE_END();
   
   return ret;
}

//-----------------------------------------------------------------------------
#if defined(TORQUE_DEBUG)
ConsoleFunction(testHasSubdir,void,2,2,"tests platform::hasSubDirectory") {
   Con::printf("testing %s",argv[1]);
   Platform::addExcludedDirectory(".svn");
   if(Platform::hasSubDirectory(argv[1]))
      Con::printf(" has subdir");
   else
      Con::printf(" does not have subdir");
}

ConsoleFunction(testDumpDirectories,void,4,4,"testDumpDirectories('path', int depth, bool noBasePath)") {
   Vector<StringTableEntry> paths;
   S32 depth = dAtoi(argv[2]);
   
   Platform::addExcludedDirectory(".svn");
   
   Platform::dumpDirectories(argv[1],paths,dAtoi(argv[2]),dAtob(argv[3]));
   
   Con::printf("Dumping directories starting from %s with depth %i", argv[1],depth);
   
   for(Vector<StringTableEntry>::iterator itr = paths.begin(); itr != paths.end(); itr++) {
      Con::printf(*itr);
   }
   
}

ConsoleFunction(testDumpPaths, void, 3, 3, "testDumpPaths('path', int depth)")
{
   Vector<Platform::FileInfo> files;
   S32 depth = dAtoi(argv[2]);
   
   Platform::addExcludedDirectory(".svn");
   
   Platform::dumpPath(argv[1], files, depth);
   
   for(Vector<Platform::FileInfo>::iterator itr = files.begin(); itr != files.end(); itr++) {
      Con::printf("%s/%s",itr->pFullPath, itr->pFileName);
   }
}

//-----------------------------------------------------------------------------
ConsoleFunction(testFileTouch, bool , 2,2, "testFileTouch('path')")
{
   return dFileTouch(argv[1]);
}

ConsoleFunction(testGetFileTimes, bool, 2,2, "testGetFileTimes('path')")
{
   FileTime create, modify;
   bool ok;
   ok = Platform::getFileTimes(argv[1],&create, &modify);
   Con::printf("%s Platform::getFileTimes %i, %i", ok ? "+OK" : "-FAIL", create, modify);
    return ok;
}

#endif
