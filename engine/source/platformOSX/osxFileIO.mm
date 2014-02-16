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
#import "platformOSX/platformOSX.h"
#include "platform/platform.h"
#include "platform/platformFileIO.h"
#include "console/console.h"
#include "string/stringTable.h"
#include "collection/vector.h"
#include "io/resource/resourceManager.h"
#include "debug/profiler.h"

#include <sys/stat.h>
#include <sys/time.h>

// Maximum character length for file paths
#define MAX_MAC_PATH_LONG 2048


#pragma mark ---- File IO Helper Functions ----

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// utility for platform::hasSubDirectory() and platform::dumpDirectories()
// ensures that the entry is a directory, and isnt on the ignore lists.
inline bool isGoodDirectory(const char* path)
{
    return (Platform::isDirectory(path)                          // is a dir
            && dStrcmp(path,".") != 0                 // not here
            && dStrcmp(path,"..") != 0                // not parent
            && !Platform::isExcludedDirectory(path)); // not excluded
}

//-----------------------------------------------------------------------------

static bool isMainDotCsPresent(char *dir)
{
    // Create the full path using the dir and pre-determined script name of "main.cs"
    char maincsbuf[MAX_MAC_PATH_LONG];
    const char *maincsname = "/main.cs";
    U32 len = dStrlen(dir) + dStrlen(maincsname);
    
    // The length of the above file path is too long. Error out
    AssertISV(len < MAX_MAC_PATH_LONG, "Sorry, path is too long, I can't run from this folder.");
    
    // Store the main.cs file path
    dSprintf(maincsbuf,MAX_MAC_PATH_LONG,"%s%s", dir, maincsname);
    
    // Check to see if the main.cs exists and return the result
    return Platform::isFile(maincsbuf);
}

//-----------------------------------------------------------------------------

void recurseDumpDirectories(const char* basePath, const char* subPath, Vector<StringTableEntry> &directoryVector, S32 depth, bool noBasePath)
{
    // Is path a directory?
    if ( !Platform::isDirectory(basePath) )
    {
        // No, so warn.
        Con::warnf("recurseDumpDirectories: %s is not a directory", basePath);
        
        // Fnish.
        return;
    }
    
    // Fetch the shared file manager.
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    // Convert the path to a NSString, then to a URL
    NSString* directoryPath = [[NSString stringWithFormat:@"%s/%s", basePath, subPath] stringByStandardizingPath];
    NSURL* directoryToScan = [NSURL fileURLWithPath:directoryPath];
    
    // Fetch the contents of the directory.
    NSArray* directoryContents = [fileManager   contentsOfDirectoryAtURL:directoryToScan
                                                includingPropertiesForKeys:[NSArray arrayWithObjects:NSURLNameKey, NSURLIsDirectoryKey, NSURLIsPackageKey, nil]
                                                                 options:NSDirectoryEnumerationSkipsHiddenFiles
                                                                   error:nil];
    
    // Enumerate sub-directories.
    for( NSURL* itemURL in directoryContents )
    {
        // Is the item a directory?
        NSNumber* isDirectory;
        [itemURL getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:NULL];
        
        // Skip if this is NOT a directory.
        if ( [isDirectory boolValue] == false )
            continue;
        
        // Is the sub-directory a package directory?
        NSNumber* isPackageDirectory;
        [itemURL getResourceValue:&isPackageDirectory forKey:NSURLIsPackageKey error:NULL];
        
        // Skip if this is a package directory.
        if  ( [isPackageDirectory boolValue] == true )
            continue;
        
        // Fetch the sub-directory name.
        NSString* subDirectoryName;
        [itemURL getResourceValue:&subDirectoryName forKey:NSURLNameKey error:NULL];
        
        // Fetch UTF8 sub-directory name.
        const char* pSubDirectoryName = [subDirectoryName UTF8String];
        
        // Skip if this is an excluded directory.
        if ( Platform::isExcludedDirectory(pSubDirectoryName) )
            continue;
        
        // Format new sub-path.
        const char* pNewSubPath = dStrlen(subPath) > 0 ? [[NSString stringWithFormat:@"%s/%s", subPath, pSubDirectoryName] UTF8String] : pSubDirectoryName;
        
        // Format with base-path accordingly.
        if ( noBasePath )
        {
            // Store the directory excluding the base-path.
            directoryVector.push_back( StringTable->insert( pNewSubPath ) );
        }
        else
        {
            // Store the directory with the bath-path.
            directoryVector.push_back( StringTable->insert( [[itemURL path] UTF8String] ) );
        }
        
        // Skip if no further recursion is required.
        if ( depth == 0 )
            continue;
        
        // Recurse the sub-directory.
        recurseDumpDirectories( basePath, pNewSubPath, directoryVector, depth-1, noBasePath );
    }
}

//-----------------------------------------------------------------------------

static void recurseDumpPath(const char* curPath, Vector<Platform::FileInfo>& fileVector, U32 depth)
{
    // Fetch the shared file manager.
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    // Convert the path to a NSString, then to a URL
    NSString* directoryPath = [[NSString stringWithUTF8String:curPath] stringByStandardizingPath];
    NSURL* directoryToScan = [NSURL fileURLWithPath:directoryPath];
    const char* pDirectoryPath = [directoryPath UTF8String];

    // Fetch the contents of the directory.
    NSArray* directoryContents = [fileManager   contentsOfDirectoryAtURL:directoryToScan
                                                includingPropertiesForKeys:[NSArray arrayWithObjects:NSURLNameKey, NSURLIsDirectoryKey, NSURLIsPackageKey, NSURLFileSizeKey, nil]
                                                                 options:NSDirectoryEnumerationSkipsHiddenFiles
                                                                   error:nil];
    
    // Enumerate files.
    for( NSURL* itemURL in directoryContents )
    {
        // Is the item a directory?
        NSNumber* isDirectory;
        [itemURL getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:NULL];
        
        // Skip if this is a directory.
        if ( [isDirectory boolValue] == true )
            continue;
        
        // Fetch the file-name.
        NSString* fileName;
        [itemURL getResourceValue:&fileName forKey:NSURLNameKey error:NULL];

        // Fetch the file size.
        NSNumber* fileSize;
        [itemURL getResourceValue:&fileSize forKey:NSURLFileSizeKey error:NULL];
        
        // Allocate a file entry.
        fileVector.increment();
        Platform::FileInfo& info = fileVector.last();
        
        // Populate file entry.
        info.pFullPath = StringTable->insert( pDirectoryPath );
        info.pFileName = StringTable->insert( [fileName UTF8String] );
        info.fileSize  = [fileSize intValue];
    }
    
    // Finish if no further recursion is required.
    if ( depth == 0 )
        return;
    
    // Enumerate sub-directories.
    for( NSURL* itemURL in directoryContents )
    {
        // Is the item a directory?
        NSNumber* isDirectory;
        [itemURL getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:NULL];
        
        // Skip if this is NOT a directory.
        if ( [isDirectory boolValue] == false )
            continue;
        
        // Is the sub-directory a package directory?
        NSNumber* isPackageDirectory;
        [itemURL getResourceValue:&isPackageDirectory forKey:NSURLIsPackageKey error:NULL];
        
        // Skip if this is a package directory.
        if  ( [isPackageDirectory boolValue] == true )
            continue;
        
        // Fetch the sub-directory name.
        NSString* subDirectoryName;
        [itemURL getResourceValue:&subDirectoryName forKey:NSURLNameKey error:NULL];
        
        // Skip if this is an excluded directory.
        if ( Platform::isExcludedDirectory([subDirectoryName UTF8String]) )
            continue;
        
        // Recurse the sub-directory.
        recurseDumpPath( [[itemURL path] UTF8String], fileVector, depth-1 );
    }
}

//-----------------------------------------------------------------------------

#pragma mark ---- File Class Methods ----

//-----------------------------------------------------------------------------

File::File() : currentStatus(Closed), capability(0)
{
    handle = NULL;
}

//-----------------------------------------------------------------------------

File::~File()
{
    close();
    handle = NULL;
}

//-----------------------------------------------------------------------------

File::Status File::open(const char *filename, const AccessMode openMode)
{
    if (dStrlen(filename) > MAX_MAC_PATH_LONG)
        Con::warnf("File::open: Filename length is pretty long...");
    
    // Close the file if it was already open...
    if (currentStatus != Closed)
        close();
    
    // create the appropriate type of file...
    switch (openMode)
    {
        case Read:
            handle = (void *)fopen(filename, "rb"); // read only
            break;
        case Write:
            handle = (void *)fopen(filename, "wb"); // write only
            break;
        case ReadWrite:
            handle = (void *)fopen(filename, "ab+"); // write(append) and read
            break;
        case WriteAppend:
            handle = (void *)fopen(filename, "ab"); // write(append) only
            break;
        default:
            AssertFatal(false, "File::open: bad access mode");
    }
    
    // handle not created successfully
    if (handle == NULL)
        return setStatus();
    
    // successfully created file, so set the file capabilities...
    switch (openMode)
    {
        case Read:
            capability = FileRead;
            break;
        case Write:
        case WriteAppend:
            capability = FileWrite;
            break;
        case ReadWrite:
            capability = FileRead | FileWrite;
            break;
        default:
            AssertFatal(false, "File::open: bad access mode");
    }
    
    // must set the file status before setting the position.
    currentStatus = Ok;
    
    if (openMode == ReadWrite)
        setPosition(0);
    
    // success!
    return currentStatus;
}

//-----------------------------------------------------------------------------

U32 File::getPosition() const
{
    // Before proceeding, make sure the file is open and the handle is valid
    AssertFatal(currentStatus != Closed , "File::getPosition: file closed");
    AssertFatal(handle != NULL, "File::getPosition: invalid file handle");
        
    // Return the position (aka, offset)
    return (U32)ftell((FILE*)handle);
}

//-----------------------------------------------------------------------------

File::Status File::setPosition( S32 position, bool absolutePos )
{
    // Before proceeding, make sure the file is open and the handle is valid
    AssertFatal(Closed != currentStatus, "File::setPosition: file closed");
    AssertFatal(handle != NULL, "File::setPosition: invalid file handle");
    
    // Check for valid status and not end of the file stream
    if (currentStatus != Ok && currentStatus != EOS )
        return currentStatus;
    
    // This will hold the final position of the current stream
    U32 finalPos;
    
    // Is the position absolute?
    if( absolutePos )
    {
        // Yes, so sanity!
        AssertFatal(0 <= position, "File::setPosition: negative absolute position");
        
        // Position beyond EOS is OK
        fseek((FILE*)handle, position, SEEK_SET);
        finalPos = (U32)ftell((FILE*)handle);
    }
    else
    {
        // No, so sanity!
        AssertFatal((getPosition() + position) >= 0, "File::setPosition: negative relative position");
        
        // Position beyond EOS is OK
        fseek((FILE*)handle, position, SEEK_CUR);
        finalPos = (U32)ftell((FILE*)handle);
    }
    
    // ftell returns -1 on error. set error status
    if (0xffffffff == finalPos)
    {
        return setStatus(IOError);
    }
    else if ( finalPos >= getSize() )
    {
        // Success, at end of file
        return currentStatus = EOS;
    }
    else
    {
        // Success!
        return currentStatus = Ok;
    }
}


//-----------------------------------------------------------------------------

U32 File::getSize() const
{
    // Before proceeding, make sure the file is open and the handle is valid
    AssertWarn(Closed != currentStatus, "File::getSize: file closed");
    AssertFatal(handle != NULL, "File::getSize: invalid file handle");
    
    if ( Ok == currentStatus || EOS == currentStatus )
    {
        struct stat statData;
        
        if(fstat(fileno((FILE*)handle), &statData) != 0)
            return 0;
        
        // return the size in bytes
        return (U32)statData.st_size;
    }
    
    return 0;
}

//-----------------------------------------------------------------------------

File::Status File::flush()
{
    // Before proceeding, make sure the file is open, the handle is valid, and the file can
    // be written to.
    AssertFatal(Closed != currentStatus, "File::flush: file closed");
    AssertFatal(handle != NULL, "File::flush: invalid file handle");
    AssertFatal(true == hasCapability(FileWrite), "File::flush: cannot flush a read-only file");
    
    if (fflush((FILE*)handle) != 0)
        return setStatus();
    else
        return currentStatus = Ok;
}

//-----------------------------------------------------------------------------

File::Status File::close()
{
    // check if it's already closed...
    if (Closed == currentStatus)
        return currentStatus;
    
    // it's not, so close it...
    if (handle != NULL)
    {
        if (fclose((FILE*)handle) != 0)
            return setStatus();
    }
    handle = NULL;
    return currentStatus = Closed;
}

//-----------------------------------------------------------------------------

File::Status File::getStatus() const
{
    return currentStatus;
}

//-----------------------------------------------------------------------------

File::Status File::setStatus()
{
    strerror(errno);
    switch (errno)
    {
        case EACCES:   // permission denied
            currentStatus = IOError;
            break;
        case EBADF:   // Bad File Pointer
        case EINVAL:   // Invalid argument
        case ENOENT:   // file not found
        case ENAMETOOLONG:
        default:
            currentStatus = UnknownError;
    }
    
    return currentStatus;
}

//-----------------------------------------------------------------------------


File::Status File::setStatus(File::Status status)
{
    return currentStatus = status;
}

//-----------------------------------------------------------------------------

File::Status File::read(U32 size, char *dst, U32 *bytesRead)
{
    // Before proceeding, make sure the file is not closed, the handle is valid,
    // the destination is not null, the file can be read, and the read size is
    // greater than 0.
    AssertFatal(Closed != currentStatus, "File::read: file closed");
    AssertFatal(handle != NULL, "File::read: invalid file handle");
    AssertFatal(NULL != dst, "File::read: NULL destination pointer");
    AssertFatal(true == hasCapability(FileRead), "File::read: file lacks capability");
    AssertWarn(0 != size, "File::read: size of zero");
    
    if (Ok != currentStatus || 0 == size)
        return currentStatus;
    
    // read from stream
    U32 nBytes = (U32)fread(dst, 1, size, (FILE*)handle);
    
    // did we hit the end of the stream?
    if( nBytes != size)
        currentStatus = EOS;
    
    // if bytesRead is a valid pointer, send number of bytes read there.
    if(bytesRead)
        *bytesRead = nBytes;
    
    // successfully read size bytes
    return currentStatus;
}

//-----------------------------------------------------------------------------

File::Status File::write(U32 size, const char *src, U32 *bytesWritten)
{
    // Before proceeding, make sure the file is not closed, the handle is valid,
    // the source is not null, the file can be written to, and the write size is
    // greater than 0.    
    AssertFatal(Closed != currentStatus, "File::write: file closed");
    AssertFatal(handle != NULL, "File::write: invalid file handle");
    AssertFatal(NULL != src, "File::write: NULL source pointer");
    AssertFatal(true == hasCapability(FileWrite), "File::write: file lacks capability");
    AssertWarn(0 != size, "File::write: size of zero");
    
    if ((Ok != currentStatus && EOS != currentStatus) || 0 == size)
        return currentStatus;
    
    // write bytes to the stream
    U32 nBytes = (U32)fwrite(src, 1, size,(FILE*)handle);
    
    // if we couldn't write everything, we've got a problem. set error status.
    if(nBytes != size)
        setStatus();
    
    // if bytesWritten is a valid pointer, put number of bytes read there.
    if(bytesWritten)
        *bytesWritten = nBytes;
    
    // return current File status, whether good or ill.
    return currentStatus;
}

//-----------------------------------------------------------------------------


bool File::hasCapability(Capability cap) const
{
    return (0 != (U32(cap) & capability));
}

#pragma mark ---- Platform Namespace Methods ----

//-----------------------------------------------------------------------------

bool Platform::fileDelete(const char * name)
{
    // Invalid pointer, return false
    if (!name)
        return false;
    
    // Name is longer than our limit, throw a warning
    if (dStrlen(name) > MAX_MAC_PATH_LONG)
        Con::warnf("Platform::fileDelete() - Filename length is pretty long...");
    
    // Atandard auto-release pool
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Get the default NSFileManager
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    // Convert the name char* to a NSString
    NSString* filePath = [[NSString stringWithUTF8String:name] stringByStandardizingPath];
    
    // Delete the file
    // Returns YES if the item was removed successfully or if path was nil.
    // Returns NO if an error occurred. If the delegate aborts the operation for a file, this method returns YES.
    // However, if the delegate aborts the operation for a directory, this method returns NO.
    NSError* errorResponse = nil;
    
    BOOL result = [fileManager removeItemAtPath:filePath error:&errorResponse];
    
    // Could not delete the file, so print the error
    if (!result)
    {
        Con::errorf("Platform::fileDelete: %s", [[errorResponse localizedDescription] UTF8String]);
    }
    
    // Drain the memory and release the pool
    [pool drain];
    
    // Return results of the deletion
    return result;
}

//-----------------------------------------------------------------------------

bool Platform::fileTouch(const char *path)
{
    if (!path || !*path)
        return false;
    
    // set file at path's modification and access times to now.
    return( utimes( path, NULL) == 0); // utimes returns 0 on success.
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
    if (!path || !*path)
        return false;

    struct stat statData;

    if (stat(path, &statData) == -1)
        return false;

    if (createTime)
        *createTime = statData.st_ctime;

    if (modifyTime)
        *modifyTime = statData.st_mtime;

    return true;

    // Make sure the path pointer is valid
    /*if (!path || !*path)
        return false;

    struct stat statData;

    // Standard auto-release pool
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Convert the path to a NSString
    NSString* convertedPath = [[NSString stringWithUTF8String:path] stringByStandardizingPath];
    
    // Error catcher
    NSError* errorResponse = nil;
    
    // Get the file attributes
    NSDictionary* attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:convertedPath error:&errorResponse];
    
    // If we failed to get the file attributes, something is wrong with the file or logic
    if (attributes == nil)
    {
        // Report the error and assign 0 file times
        Con::errorf("File::getSize: %s", [[errorResponse localizedDescription] UTF8String]);
        *createTime = 0;
        *modifyTime = 0;
        return false;
    }
    else
    {
        // Assign the creation and modified file times
        *createTime = [[attributes objectForKey:NSFileCreationDate] unsignedLongLongValue];
        *modifyTime = [[attributes objectForKey:NSFileModificationDate] unsignedLongLongValue];
    }
    
    // Auto-release memory
    [pool drain];
    
    return true;*/
}


//-----------------------------------------------------------------------------

bool Platform::createPath(const char *file)
{
    // Make sure the file pointer is valid
    if (!file || !*file)
        return false;

    // Ensure that the path has no file on the end.
    char pathBuffer[1024];
    dSprintf(pathBuffer, sizeof(pathBuffer), "%s", file );
    const S32 pathLength = dStrlen(pathBuffer);
    if ( pathLength == sizeof(pathBuffer)-1 )
    {
        Con::warnf("Could not create path as it was too long: '%s", file);
        return false;
    }
    char* pFinalSlash = dStrrchr(pathBuffer, '/');
    if ( pFinalSlash != pathBuffer+pathLength-1 )
    {
        pFinalSlash[1] = 0;
    }
    file = pathBuffer;
    
    // Standard auto-release pool
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Conver the file path to a NSString
    NSString* convertedPath = [[NSString stringWithUTF8String:file] stringByStandardizingPath];

    // Get the shared file manager
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    // Error catcher
    NSError* createError = nil;
    
    // Try to create the path
    BOOL result = [fileManager createDirectoryAtPath:convertedPath withIntermediateDirectories:YES attributes:nil error:&createError];
    
    // If the create path failed, report the error and return
    if (!result)
    {
        Con::errorf("Platform::createPath error: %s", [[createError localizedDescription] UTF8String]);
        [pool drain];
        return false;
    }
    
    // Auto-release memory
    [pool drain];
    
    // Success
    return true;
}

//-----------------------------------------------------------------------------

StringTableEntry Platform::getCurrentDirectory()
{
    // Use the shared file manager to get the current path
    return [[[NSFileManager defaultManager] currentDirectoryPath] UTF8String];
}

//-----------------------------------------------------------------------------

bool Platform::setCurrentDirectory(StringTableEntry newDir)
{
    // Standard auto-release pool
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Get the shared file manager
    NSFileManager* fileManager = [NSFileManager defaultManager];
    
    // Convert the newDir to a NSString
    NSString* convertedString = [[NSString stringWithUTF8String:newDir] stringByStandardizingPath];
    
    // Attempt to set the current directory to what was passed
    BOOL result = [fileManager changeCurrentDirectoryPath:convertedString];
    
    // Auto-release memory
    [pool drain];
    
    // Return the result
    return result;
}

//-----------------------------------------------------------------------------

void Platform::openFolder(const char* path )
{
    // Standard auto-release pool
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Convert the path to a NSString
    NSString* convertedPath = [[NSString stringWithUTF8String:path] stringByStandardizingPath];
    
    // Try to open the folder
    BOOL result = [[NSWorkspace sharedWorkspace] openFile:convertedPath];
    
    // Auto-release the memory
    [pool drain];
    
    // If the operation failed, print an error
    if (!result)
        Con::errorf("Platform::openFolder: Failed to open folder %s", path);
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
/// Placing all content inside the application bundle gives a much better user
/// experience when you distribute your app.
StringTableEntry Platform::getExecutablePath()
{
    // Get the shared OSX platform state
    osxPlatState * platState = [osxPlatState sharedPlatState];
    
    // If we already have a main.cs file, return that
    if (platState.mainCSDirectory)
        return [[platState mainCSDirectory] UTF8String];
    
    // Get the main application bundle
    char cwd_buf[MAX_MAC_PATH_LONG];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef bundleUrl = CFBundleCopyBundleURL(mainBundle);
    
    // Used to determine where the executable exists.
    bool inside = true;
    bool outside = false;
    bool done = false;
    
    while(!done)
    {
        // first look for game content inside the application bundle.
        // then we look outside the bundle
        // then we assume it's a tool, and the "bundle" = the binary file.
        CFURLRef workingUrl;
        if (inside)
            workingUrl = CFURLCreateCopyAppendingPathComponent(kCFAllocatorSystemDefault,bundleUrl,CFSTR("Contents/Resources"),true);
        else if(outside)
            workingUrl = CFURLCreateCopyDeletingLastPathComponent(kCFAllocatorSystemDefault, bundleUrl);
        else
        {
            workingUrl = bundleUrl;
            CFRetain(workingUrl); // so that we can release bundleUrl twice.
        }
        
        CFStringRef workingString = CFURLCopyFileSystemPath(workingUrl, kCFURLPOSIXPathStyle);
        CFMutableStringRef normalizedString = CFStringCreateMutableCopy(NULL, 0, workingString);
        CFStringNormalize(normalizedString,kCFStringNormalizationFormC);
        CFStringGetCString(normalizedString, cwd_buf, sizeof(cwd_buf)-1, kCFStringEncodingUTF8);
        
        // if we dont see main.cs inside the bundle, try again looking outside
        // we're done if we find it, or if we find it neither inside or outside.
        if (isMainDotCsPresent(cwd_buf) || (!inside && !outside))
            done = true;
        
        if (inside)
            inside = false, outside = true;
        else if (outside)
            outside = false;
        
        CFRelease(workingUrl);
        CFRelease(workingString);
        CFRelease(normalizedString);
    }
    
    CFRelease(bundleUrl);
    
    char* ret = NULL;
    
    if (StringTable)
        platState.mainCSDirectory = [NSString stringWithUTF8String: StringTable->insert(cwd_buf)];
    else
        ret = dStrdup(cwd_buf);
    
    return ret ? ret : [platState.mainCSDirectory UTF8String];
}

//-----------------------------------------------------------------------------

StringTableEntry Platform::getExecutableName()
{
    char path_buf[MAX_MAC_PATH_LONG];
    
    // get a cfurl to the executable name
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef bundleUrl = CFBundleCopyBundleURL(mainBundle);
    
    // get a cfstring of just the app name
    CFStringRef workingString = CFURLCopyLastPathComponent(bundleUrl);
    CFMutableStringRef normalizedString = CFStringCreateMutableCopy(NULL, 0, workingString);
    CFStringNormalize(normalizedString,kCFStringNormalizationFormC);
    CFStringGetCString(normalizedString, path_buf, sizeof(path_buf)-1, kCFStringEncodingUTF8);
    
    CFRelease(bundleUrl);
    CFRelease(workingString);
    CFRelease(normalizedString);
    
    return StringTable->insert(path_buf);
}

//-----------------------------------------------------------------------------

bool Platform::isFile(const char *path)
{
    // Make sure a valid pointer was passed
    if (!path || !*path)
        return false;
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Convert the path to a temp NSString
    NSString* filePath = [[NSString stringWithUTF8String:path] stringByStandardizingPath];
    
    // This is required to explicitly tell the file manager to determine if the file
    // in question is a directory
    BOOL isDirectory;
    
    // Scan the path location
    bool exists = [[NSFileManager defaultManager] fileExistsAtPath:filePath isDirectory:&isDirectory];
    
    // Clean up the temp string
    [pool drain];
    
    // Return the results of the scan
    return !isDirectory && exists;
    
}

//-----------------------------------------------------------------------------

bool Platform::isDirectory(const char *path)
{
    // Make sure a valid pointer was passed
    if (!path || !*path)
        return false;
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Convert the path to a temp NSString
    NSString* folderPath = [[NSString stringWithUTF8String:path] stringByStandardizingPath];
    
    // This is required to explicitly tell the file manager to determine if the file
    // in question is a directory
    BOOL isDirectory;
    
    // Scan the path location
    bool exists = [[NSFileManager defaultManager] fileExistsAtPath:folderPath isDirectory:&isDirectory];
    
    // Clean up the temp string
    [pool drain];
    
    // Return the results of the scan
    return isDirectory && exists;
}

//-----------------------------------------------------------------------------

S32 Platform::getFileSize(const char* pFilePath)
{
    // Make sure a valid pointer was passed
    if (!pFilePath || !*pFilePath)
        return 0;
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // This will catch an error if the file manager fails to gather attributes
    NSError* fileError = nil;
    
    // Convert pFilePath to a NSString
    NSString* path = [[NSString stringWithUTF8String:pFilePath] stringByStandardizingPath];
    
    // Get all the attributes of the file
    NSDictionary* attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&fileError];
    
    // File manager could not gather attributes
    if (attributes == nil)
    {
        // Get the error message as a char*
        const char* errorMessage = [[fileError localizedDescription] UTF8String];
        
        // Print the error to the console
        Con::errorf("Platform::getFileSize: %s", errorMessage);
        
        [pool drain];
        
        // Return a 0 filesize
        return 0;
    }
    
    // Get the file size
    U32 fileLength = (U32)[[attributes objectForKey:NSFileSize] integerValue];
    
    // Clean up temp string
    [pool drain];
    
    // Return the result
    return fileLength;
}

//-----------------------------------------------------------------------------

bool Platform::isSubDirectory(const char *pathParent, const char *pathSub)
{
    // Concatenate the parent and sub directories
    char fullpath[MAX_MAC_PATH_LONG];
    dStrcpyl(fullpath, MAX_MAC_PATH_LONG, pathParent, "/", pathSub, NULL);
    
    // Return the isDirectory check
    return isDirectory((const char *)fullpath);
}

//-----------------------------------------------------------------------------
// Check to see if the path has sub-directories
// First pass at the OSX Cocoa implementation started with [NSFileManager subpathsAtPath]
// That implementation was very simple, but the performance cost would be unacceptable
// with large file structures. Since Torque 2D has many sub-directories, it makes more
// sense to manually walk through directory contents using [NSFileManager enumeratorAtPath]
// This approach is used in other functions where directories are dumped recursively
bool Platform::hasSubDirectory(const char *path)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Convert the path to a NSString
    NSString* filePath = [[NSString stringWithUTF8String:path] stringByStandardizingPath];
    
    // Check to make sure we are scannning a directory
    if (Platform::isDirectory(path))
    {
        // This will hold a list of subpaths
        NSDirectoryEnumerator* dirEnum = nil;
        
        // Get the default NSFileManager
        NSFileManager *fileManager = [NSFileManager defaultManager];
        
        // Initially, assume there is not a sub-directory
        BOOL foundDirectory = NO;
        
        // Enumerate the contes of the file path. 
        dirEnum = [fileManager enumeratorAtPath:filePath];
        
        // This will walk the contents of the enumerated object
        NSString* entry;
        
        // Start walking through the enumerator
        while (entry = [dirEnum nextObject])
        {
            // Is the current entry a directory?
            if (Platform::isDirectory([entry UTF8String]))
            {
                // Success. Store the result and exit the loop
                foundDirectory = YES;
                break;
            }
        }
        
        // Clean up the temp string
        [pool drain];
        
        // Return the result of the scan
        return foundDirectory;
    }
    else
    {
        // Path was not a directory or did not exist
        Con::errorf("Platform::hasSubDirectory: path is not a directory");
        
        // Clean up the temp string
        [pool drain];
        
        // Return false
        return false;
    }
    
    // Clean up temp string
    [pool drain];
    
    // Something went wrong, so return false
    return false;
}

//-----------------------------------------------------------------------------

bool Platform::dumpDirectories(const char *path, Vector<StringTableEntry> &directoryVector, S32 depth, bool noBasePath)
{
    // Start profiling
    PROFILE_START(dumpDirectories);
    
    // Initialize the excluded directories so they are ignored
    ResourceManager->initExcludedDirectories();
    
    // Get the length of the path and pre-allocate the new path
    int len = dStrlen(path);
    char newpath[len+1];
    
    // Copy in the original path to the new path variable
    dSprintf(newpath, len, "%s", path );
    
    // cut off the trailing slash, if there is one
    if (newpath[len - 1] == '/')
        newpath[len - 1] = '\0';
    
    // Insert base path to follow what Windows does.
    if (!noBasePath )
        directoryVector.push_back(StringTable->insert(newpath));
    
    // Standard auto-release pool
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Recursively dump the directories
    recurseDumpDirectories(newpath, "", directoryVector, depth, noBasePath);
    
    // Drain the pool.
    [pool drain];
    
    // End profiling
    PROFILE_END();
    
    // Return the result of recursively dumping the directories
    return true;
}

//-----------------------------------------------------------------------------

bool Platform::dumpPath(const char *path, Vector<Platform::FileInfo>& fileVector, S32 depth)
{
    // Start profiling
    PROFILE_START(dumpPath);
    
    // Initialize the excluded directories so they are ignored
    ResourceManager->initExcludedDirectories();
    
    // Get the length of the path and pre-allocate the new path variable
    int len = dStrlen(path);
    char newpath[len+1];
    
    // Copy the source path into the new path
    dSprintf(newpath, len, "%s", path);
    
    // Cut off the trailing slash, if there is one
    if (newpath[len - 1] == '/')
        newpath[len - 1] = '\0';
    
    // Is the initial directory excluded?
    if ( Platform::isExcludedDirectory(path) )
    {
        // Yes, so stop profiling
        PROFILE_END();
 
        return false;
    }
    
    // Standard auto-release pool
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
    // Recursively dump the contents of the path
    recurseDumpPath(newpath, fileVector, depth);
    
    // Drain the pool.
    [pool drain];
    
    // Stop profiling
    PROFILE_END();
    
    return true;
}

//-----------------------------------------------------------------------------

bool Platform::fileRename(const char *source, const char *dest)
{
    // Make sure the pointers are valid
    if (source == NULL || dest == NULL)
        return false;
    
    // Get the shared file manager
    NSFileManager *manager = [NSFileManager defaultManager];
    
    // Conver the source and dest pointers to NSString
    NSString *nsource = [manager stringWithFileSystemRepresentation:source length:dStrlen(source)];
    NSString *ndest = [manager stringWithFileSystemRepresentation:dest length:dStrlen(dest)];
    
    // If the source file doesn't exist, error out
    if (![manager fileExistsAtPath:nsource])
    {
        Con::errorf("Platform::fileRename: no file exists at %s",source);
        return false;
    }
    
    // If the destination file already exists, throw a warning. We are about to overwrite it
    if ([manager fileExistsAtPath:ndest] )
    {
        Con::warnf("Platform::fileRename: Deleting files at path: %s", dest);
    }
    
    // Error catcher
    NSError* errorResponse = nil;
    
    // Rename the file
    bool result = [manager moveItemAtPath:nsource toPath:ndest error:&errorResponse];
    
    // Rename operation failed
    if (!result)
        Con::errorf("Platform::fileRename: %S", [[errorResponse localizedDescription] UTF8String] );
    
    // Return the result
    return result;
}

bool Platform::pathCopy(const char* source, const char* dest, bool nooverwrite)
{
    // Standard auto-release pool
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // Get the shared file manager
    NSFileManager *manager = [NSFileManager defaultManager];
    
    // Convert the char pointers to NSString
    NSString *nsource = [[NSString stringWithUTF8String:source] stringByStandardizingPath];
    NSString *ndest = [[NSString stringWithUTF8String:dest] stringByStandardizingPath];
    NSString *ndestFolder = [ndest stringByDeletingLastPathComponent];
    
    // If the source file does not exist, error out
    if (![manager fileExistsAtPath:nsource])
    {
        Con::errorf("Platform::pathCopy: no file exists at %s",source);
        return false;
    }
    
    // If the destination file already exists
    if ([manager fileExistsAtPath:ndest])
    {
        // If we can't overwrite, error out
        if(nooverwrite)
        {
            Con::errorf("Platform::pathCopy: file already exists at %s",dest);
            return false;
        }
        
        // Warn that we are about to overwrite the file
        Con::warnf("Deleting files at path: %s", dest);
        
        // Error catcher
        NSError* errorResponse = nil;
        
        // Try to delete the item
        bool deleted = [manager removeItemAtPath:ndest error:&errorResponse];
        
        // Failed to delete, so print the error and return
        if(!deleted)
        {
            Con::errorf("Platform::pathCopy failed at %s. %s", dest, [[errorResponse localizedDescription] UTF8String]);
            return false;
        }
    }
    
    if ([manager fileExistsAtPath:ndestFolder] == NO)
    {
        ndestFolder = [ndestFolder stringByAppendingString:@"/"]; // createpath requires a trailing slash
        Platform::createPath([ndestFolder UTF8String]);
    }
    
    NSError* errorResponse = nil;
    
    bool ret = [manager copyItemAtPath:nsource toPath:ndest error:&errorResponse];
    
    if (errorResponse != nil)
    {
        Con::errorf("Platform::pathCopy: %s", [[errorResponse localizedDescription] UTF8String]);
    }
    
    [pool drain];
    
    return ret;
}

StringTableEntry Platform::getUserHomeDirectory()
{
    return StringTable->insert([[@"~/Documents" stringByStandardizingPath] UTF8String]);
}

const char* Platform::getUserDataDirectory()
{
	// orb: Getting the path of the Application Support directory is only the first step.
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    
	if ([paths count] == 0)
	{
		// orb: This is a catastrophic failure - the system doesn't know where ~/Library/Application Support is!
		return NULL;
	}
    
	NSString* fullPath = [paths objectAtIndex:0];
    
	BOOL exists;
	BOOL isDir;
    
	NSFileManager* fileManager = [NSFileManager defaultManager];
    
	exists = [fileManager fileExistsAtPath:fullPath isDirectory:&isDir];
    
	if (!exists || !isDir)
	{
		// orb: This is probably an extremely rare occurence, but I have seen a few disk checks
		// converting directories into files before.
		if (exists)
		{
            NSError* errorResponse = nil;
            
			BOOL success = [fileManager removeItemAtPath:fullPath error:&errorResponse];
            
            if (!success)
            {
                Con::errorf("Platform::getUserDataDirectory: %s", [[errorResponse localizedDescription] UTF8String]);
            }
		}
        
        NSError* errorResponse = nil;
        
		BOOL success = [fileManager createDirectoryAtPath:fullPath withIntermediateDirectories:YES attributes:nil error:&errorResponse];
        
		if (!success)
        {
            Con::errorf("Platform::getUserDataDirectory: %s", [[errorResponse localizedDescription] UTF8String]);
            return NULL;
        }
    }
    
	// The directory exists and can be returned.
    return StringTable->insert([fullPath UTF8String]);
}

StringTableEntry Platform::osGetTemporaryDirectory()
{
    NSString *tdir = NSTemporaryDirectory();
    
    const char *path = [tdir UTF8String];
    
    return StringTable->insert(path);
}
