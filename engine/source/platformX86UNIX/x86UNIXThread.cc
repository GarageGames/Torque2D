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



#include "platform/platformThread.h"
#include "platformX86UNIX/platformX86UNIX.h"
#include "platform/platformSemaphore.h"

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include "pthread.h"

//--------------------------------------------------------------------------
struct x86UNIXThreadData
{
  ThreadRunFunction       mRunFunc;
  S32                     mRunArg;
  Thread *                mThread;
  void *                  mSemaphore;
  SDL_Thread              *mTheThread;

   x86UNIXThreadData()
   {
      mRunFunc    = 0;
      mRunArg     = 0;
      mThread     = 0;
      mSemaphore  = 0;
   };
};

//--------------------------------------------------------------------------
Thread::Thread(ThreadRunFunction func, S32 arg, bool start_thread)
{
  x86UNIXThreadData * threadData = new x86UNIXThreadData();
  threadData->mRunFunc   = func;
  threadData->mRunArg    = arg;
  threadData->mThread    = this;
  threadData->mSemaphore = Semaphore::createSemaphore();
  threadData->mTheThread = NULL;

  mData = reinterpret_cast<void*>(threadData);
  if (start_thread)
    start();
}

Thread::~Thread()
{
  join();
  
  x86UNIXThreadData * threadData = reinterpret_cast<x86UNIXThreadData*>(mData);
  Semaphore::destroySemaphore(threadData->mSemaphore);
  delete threadData;
}

static int ThreadRunHandler(void * arg)
{
   x86UNIXThreadData * threadData = reinterpret_cast<x86UNIXThreadData*>(arg);
   threadData->mThread->run(threadData->mRunArg);
   Semaphore::releaseSemaphore(threadData->mSemaphore);
   return 0;
}

void Thread::start()
{
   if(isAlive())
      return;
   x86UNIXThreadData *threadData = reinterpret_cast<x86UNIXThreadData*>(mData);
   Semaphore::acquireSemaphore(threadData->mSemaphore);
   threadData->mTheThread = SDL_CreateThread(ThreadRunHandler, mData);
}

bool Thread::join()
{ 
  if(!isAlive())
    return(false);


  x86UNIXThreadData * threadData = reinterpret_cast<x86UNIXThreadData*>(mData);
  //
  // (SDL 1.2.7) Even after the procedure started in the thread returns, there 
  // still exist some resources allocated to the thread. To free these 
  // resources, use SDL_WaitThread to wait for the thread to finish and 
  // obtain the status code of the thread. If not done so, SDL_CreateThread 
  // will hang after about 1010 successfully created threads 
  // (tested on GNU/Linux).
  //
  SDL_WaitThread(threadData->mTheThread, NULL);
  threadData->mTheThread = NULL;
  return (Semaphore::acquireSemaphore(threadData->mSemaphore, true));
}

void Thread::run(S32 arg)
{
  x86UNIXThreadData * threadData = reinterpret_cast<x86UNIXThreadData*>(mData);
  if(threadData->mRunFunc)
    threadData->mRunFunc(arg);
}

bool Thread::isAlive()
{
  x86UNIXThreadData * threadData = reinterpret_cast<x86UNIXThreadData*>(mData);
  if (threadData->mTheThread)
    {
      bool signal = Semaphore::acquireSemaphore(threadData->mSemaphore, false);
      if (signal)
	Semaphore::releaseSemaphore(threadData->mSemaphore);
      return (!signal);
    }
  return false;
}

U32 Thread::getCurrentThreadId()
{
   return (U32)SDL_ThreadID();
}

class PlatformThreadStorage
{
	public:
		  pthread_key_t mThreadKey;
};

/*ThreadStorage::ThreadStorage()
{
	  mThreadStorage = (PlatformThreadStorage *) mStorage;
	    constructInPlace(mThreadStorage);

	      pthread_key_create(&mThreadStorage->mThreadKey, NULL);
}

ThreadStorage::~ThreadStorage()
{
	  pthread_key_delete(mThreadStorage->mThreadKey);
}

void *ThreadStorage::get()
{
	  return pthread_getspecific(mThreadStorage->mThreadKey);
}

void ThreadStorage::set(void *value)
{
	  pthread_setspecific(mThreadStorage->mThreadKey, value);
}
*/

