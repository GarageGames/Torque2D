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
#ifndef _PLATFORM_FILEIO_H_
#define _PLATFORM_FILEIO_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifdef TORQUE_OS_LINUX
// Need to remove this once Xlib stops leaking
#undef Status
#endif

class File
{
public:
   /// What is the status of our file handle?
   enum Status
   {
      Ok = 0,           ///< Ok!
      IOError,          ///< Read or Write error
      EOS,              ///< End of Stream reached (mostly for reads)
      IllegalCall,      ///< An unsupported operation used.  Always accompanied by AssertWarn
      Closed,           ///< Tried to operate on a closed stream (or detached filter)
      UnknownError      ///< Catchall
   };

   /// How are we accessing the file?
   enum AccessMode
   {
      Read         = 0,  ///< Open for read only, starting at beginning of file.
      Write        = 1,  ///< Open for write only, starting at beginning of file; will blast old contents of file.
      ReadWrite    = 2,  ///< Open for read-write.
      WriteAppend  = 3   ///< Write-only, starting at end of file.
   };

   /// Flags used to indicate what we can do to the file.
   enum Capability
   {
      FileRead         = BIT(0),
      FileWrite        = BIT(1)
   };

private:
   void *handle;           ///< Pointer to the file handle.
   Status currentStatus;   ///< Current status of the file (Ok, IOError, etc.).
   U32 capability;         ///< Keeps track of file capabilities.

#ifdef TORQUE_OS_ANDROID
    U8* buffer;
   	U32 size;
   	U32 filePointer;
#endif

   File(const File&);              ///< This is here to disable the copy constructor.
   File& operator=(const File&);   ///< This is here to disable assignment.

public:
   File();                     ///< Default constructor
   virtual ~File();            ///< Destructor

   /// Opens a file for access using the specified AccessMode
   ///
   /// @returns The status of the file
   Status open(const char *filename, const AccessMode openMode);

   /// Gets the current position in the file
   ///
   /// This is in bytes from the beginning of the file.
   U32 getPosition() const;

   /// Sets the current position in the file.
   ///
   /// You can set either a relative or absolute position to go to in the file.
   ///
   /// @code
   /// File *foo;
   ///
   /// ... set up file ...
   ///
   /// // Go to byte 32 in the file...
   /// foo->setPosition(32);
   ///
   /// // Now skip back 20 bytes...
   /// foo->setPosition(-20, false);
   ///
   /// // And forward 17...
   /// foo->setPosition(17, false);
   /// @endcode
   ///
   /// @returns The status of the file
   Status setPosition(S32 position, bool absolutePos = true);

   /// Returns the size of the file
   U32 getSize() const;

   /// Make sure everything that's supposed to be written to the file gets written.
   ///
   /// @returns The status of the file.
   Status flush();

   /// Closes the file
   ///
   /// @returns The status of the file.
   Status close();

   /// Gets the status of the file
   Status getStatus() const;

   /// Reads "size" bytes from the file, and dumps data into "dst".
   /// The number of actual bytes read is returned in bytesRead
   /// @returns The status of the file
   Status read(U32 size, char *dst, U32 *bytesRead = NULL);

   /// Writes "size" bytes into the file from the pointer "src".
   /// The number of actual bytes written is returned in bytesWritten
   /// @returns The status of the file
   Status write(U32 size, const char *src, U32 *bytesWritten = NULL);

   /// Returns whether or not this file is capable of the given function.
   bool hasCapability(Capability cap) const;

protected:
   Status setStatus();                 ///< Called after error encountered.
   Status setStatus(Status status);    ///< Setter for the current status.
};

#endif // _FILE_IO_H_
