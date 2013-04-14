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

#ifndef _FILESTREAM_H_
#define _FILESTREAM_H_

#ifndef _PLATFORM_FILEIO_H_
#include "platform/platformFileIO.h"
#endif

#ifndef _STREAM_H_
#include "io/stream.h"
#endif

class FileStream : public Stream
{
public:
   enum AccessMode
   {
      Read = File::Read,
      Write = File::Write,
      ReadWrite = File::ReadWrite,
      WriteAppend = File::WriteAppend
   };
   enum
   {
      BUFFER_SIZE = 8 * 1024,         // this can be changed to anything appropriate
      BUFFER_INVALID = 0xffffffff      // file offsets must all be less than this
   };

protected:
   File mFile;                         // file being streamed
   U32  mStreamCaps;                   // dependent on access mode
   U8   mBuffer[BUFFER_SIZE];
   U32  mBuffHead;                     // first valid position of buffer (from start-of-file)
   U32  mBuffPos;                      // next read or write will occur here
   U32  mBuffTail;                     // last valid position in buffer (inclusive)
   bool mDirty;                        // whether buffer has been written to
   bool mEOF;                          // whether disk reads have reached the end-of-file

   FileStream(const FileStream &i_fileStrm);             // disable copy constructor
   FileStream& operator=(const FileStream &i_fileStrm);  // disable assignment operator

public:
   FileStream();                       // default constructor
   virtual ~FileStream();              // destructor

   // mandatory methods from Stream base class...
   virtual bool hasCapability(const Capability i_cap) const;

   virtual U32  getPosition() const;
   virtual bool setPosition(const U32 i_newPosition);
   virtual U32  getStreamSize();

   // additional methods needed for a file stream...
   virtual bool open(const char *i_pFilename, AccessMode i_openMode);
   virtual void close();

   bool Flush();


protected:
   // more mandatory methods from Stream base class...
   virtual bool _read(const U32 i_numBytes, void *o_pBuffer);
   virtual bool _write(const U32 i_numBytes, const void* i_pBuffer);

   void init();
   bool fillBuffer(const U32 i_startPosition);
   void clearBuffer();
   static void calcBlockHead(const U32 i_position, U32 *o_blockHead);
   static void calcBlockBounds(const U32 i_position, U32 *o_blockHead, U32 *o_blockTail);
   void setStatus();
};

#endif // _FILE_STREAM_H
