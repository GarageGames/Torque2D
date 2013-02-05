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

#include "platform/threads/mutex.h"
#include "platformWin32/platformWin32.h"
#include "memory/safeDelete.h"

//////////////////////////////////////////////////////////////////////////
// Mutex Data
//////////////////////////////////////////////////////////////////////////

struct PlatformMutexData
{
   HANDLE mMutex;

   PlatformMutexData()
   {
      mMutex = NULL;
   }
};

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

Mutex::Mutex()
{
   mData = new PlatformMutexData;

   mData->mMutex = CreateMutex(NULL, FALSE, NULL);
}

Mutex::~Mutex()
{
   if(mData && mData->mMutex)
      CloseHandle(mData->mMutex);
   
   SAFE_DELETE(mData);
}

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

bool Mutex::lock(bool block /* = true */)
{
   if(mData == NULL || mData->mMutex == NULL)
      return false;

   return (bool)WaitForSingleObject(mData->mMutex, block ? INFINITE : 0) == WAIT_OBJECT_0;
}

void Mutex::unlock()
{
   if(mData == NULL || mData->mMutex == NULL)
      return;

   ReleaseMutex(mData->mMutex);
}

//void Mutex::set( void*data )
//{
//   if(mData && mData->mMutex)
//      CloseHandle(mData->mMutex);
//
//   if( mData == NULL )
//      mData = new PlatformMutexData;
//
//   mData->mMutex = (HANDLE)data;
//
//}