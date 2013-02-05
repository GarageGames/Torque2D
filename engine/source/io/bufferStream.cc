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

#include "bufferStream.h"
#include "platform/platform.h"

//-----------------------------------------------------------------------------
// BufferStream methods...
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
BufferStream::BufferStream()
{
   // initialize the buffer stream
   init();
}

//-----------------------------------------------------------------------------
BufferStream::~BufferStream()
{
   // make sure the file stream is closed
   close();
}

//-----------------------------------------------------------------------------
void BufferStream::close() {
	mReadPosition = 0;//we are done reading for this go-around
}

void BufferStream::open() {
	init();
}
//-----------------------------------------------------------------------------
bool BufferStream::_read(const U32 i_numBytes, void *o_pBuffer)
{
	if( mReadPosition + i_numBytes > mBufferLen ) {
		//-Mat reading too far, error our here
		Platform::debugBreak();//we are about to have a buffer overrun
		return false;
	}
	
	//is off by 16 bytes the second time through (byte alignment after 0 is off?)
	U8 *bufferStart = &mBuffer[mReadPosition * sizeof(U8)];
	dMemcpy( o_pBuffer, bufferStart, i_numBytes );
	mReadPosition += i_numBytes;

	return true;
}

//-----------------------------------------------------------------------------
bool BufferStream::_write(const U32 i_numBytes, const void *i_pBuffer)
{
	if( (mBufferLen + i_numBytes) >= BUFFER_SIZE ) {
		//too many bytes, we'll have to try resizing (ugh!)
		return false;
	}
	//copy to "fresh" part of mBuffer
	U8 *bufferStart = &mBuffer[mBufferLen] ;
	dMemcpy( bufferStart, i_pBuffer, i_numBytes );
	mBufferLen += i_numBytes;
	return true;
}

//-----------------------------------------------------------------------------
void BufferStream::init()
{
	mBuffer[0] = '\0';
	mBufferLen = 0;
	mReadPosition = 0;
}

