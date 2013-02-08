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
#include "memstream.h"

MemStream::MemStream(const U32 in_bufferSize,
                     void*        io_pBuffer,
                     const bool   in_allowRead,
                     const bool   in_allowWrite)
 : cm_bufferSize(in_bufferSize),
   m_pBufferBase(io_pBuffer),
   m_instCaps(0),
   m_currentPosition(0)
{
   AssertFatal(io_pBuffer != NULL, "Invalid buffer pointer");
   AssertFatal(in_bufferSize > 0,  "Invalid buffer size");
   AssertFatal(in_allowRead || in_allowWrite, "Either write or read must be allowed");

   if (in_allowRead)
      m_instCaps |= Stream::StreamRead;
   if (in_allowWrite)
      m_instCaps |= Stream::StreamWrite;

   setStatus(Ok);
}

MemStream::~MemStream()
{
   m_pBufferBase     = NULL;
   m_currentPosition = 0;

   setStatus(Closed);
}

U32 MemStream::getStreamSize()
{
   AssertFatal(getStatus() != Closed, "Stream not open, size undefined");

   return cm_bufferSize;
}

bool MemStream::hasCapability(const Capability in_cap) const
{
   // Closed streams can't do anything
   //
   if (getStatus() == Closed)
      return false;

   U32 totalCaps = U32(Stream::StreamPosition) | m_instCaps;

   return (U32(in_cap) & totalCaps) != 0;
}

U32 MemStream::getPosition() const
{
   AssertFatal(getStatus() != Closed, "Position of a closed stream is undefined");

   return m_currentPosition;
}

bool MemStream::setPosition(const U32 in_newPosition)
{
   AssertFatal(getStatus() != Closed, "SetPosition of a closed stream is not allowed");
   AssertFatal(in_newPosition <= cm_bufferSize, "Invalid position");

   m_currentPosition = in_newPosition;
   if (m_currentPosition > cm_bufferSize) {
      // Never gets here in debug version, this is for the release builds...
      //
      setStatus(UnknownError);
      return false;
   } else if (m_currentPosition == cm_bufferSize) {
      setStatus(EOS);
   } else {
      setStatus(Ok);
   }

   return true;
}

bool MemStream::_read(const U32 in_numBytes, void *out_pBuffer)
{
   AssertFatal(getStatus() != Closed, "Attempted read from a closed stream");

   if (in_numBytes == 0)
      return true;

   AssertFatal(out_pBuffer != NULL, "Invalid output buffer");

   if (hasCapability(StreamRead) == false) {
      AssertWarn(false, "Reading is disallowed on this stream");
      setStatus(IllegalCall);
      return false;
   }

   bool success     = true;
   U32  actualBytes = in_numBytes;
   if ((m_currentPosition + in_numBytes) > cm_bufferSize) {
      success = false;
      actualBytes = cm_bufferSize - m_currentPosition;
   }

   // Obtain a current pointer, and do the copy
   const void* pCurrent = (const void*)((const U8*)m_pBufferBase + m_currentPosition);
   dMemcpy(out_pBuffer, pCurrent, actualBytes);

   // Advance the stream position
   m_currentPosition += actualBytes;

   if (!success)
      setStatus(EOS);
   else
      setStatus(Ok);

   return success;
}

bool MemStream::_write(const U32 in_numBytes, const void *in_pBuffer)
{
   AssertFatal(getStatus() != Closed, "Attempted write to a closed stream");

   if (in_numBytes == 0)
      return true;

   AssertFatal(in_pBuffer != NULL, "Invalid input buffer");

   if (hasCapability(StreamWrite) == false) {
      AssertWarn(0, "Writing is disallowed on this stream");
      setStatus(IllegalCall);
      return false;
   }

   bool success     = true;
   U32  actualBytes = in_numBytes;
   if ((m_currentPosition + in_numBytes) > cm_bufferSize) {
      success = false;
      actualBytes = cm_bufferSize - m_currentPosition;
   }

   // Obtain a current pointer, and do the copy
   void* pCurrent = (void*)((U8*)m_pBufferBase + m_currentPosition);
   dMemcpy(pCurrent, in_pBuffer, actualBytes);

   // Advance the stream position
   m_currentPosition += actualBytes;

   if (m_currentPosition == cm_bufferSize)
      setStatus(EOS);
   else
      setStatus(Ok);

   return success;
}

