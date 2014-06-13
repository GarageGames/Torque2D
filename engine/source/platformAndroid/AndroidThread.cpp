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

#include <pthread.h>
#include "platform/threads/thread.h"
#include "platform/platformSemaphore.h"
#include "platform/threads/mutex.h"
#include "platform/platformTLS.h"
#include "memory/safeDelete.h"
#include <stdlib.h>

struct PlatformThreadData
{
   ThreadRunFunction       mRunFunc;
   void*                   mRunArg;
   Thread*                 mThread;
   Semaphore               mGateway; // default count is 1
   U32                     mThreadID;
};

//-----------------------------------------------------------------------------
// Function:    ThreadRunHandler
// Summary:     Calls Thread::run() with the thread's specified run argument.
//               Neccesary because Thread::run() is provided as a non-threaded
//               way to execute the thread's run function. So we have to keep
//               track of the thread's lock here.
static void *ThreadRunHandler(void * arg)
{
   PlatformThreadData *mData = reinterpret_cast<PlatformThreadData*>(arg);
   Thread *thread = mData->mThread;

   mData->mThreadID = ThreadManager::getCurrentThreadId();
   ThreadManager::addThread(thread);
   thread->run(mData->mRunArg);

	mData->mGateway.release();
   // we could delete the Thread here, if it wants to be auto-deleted...
   if(thread->autoDelete)
   {
      ThreadManager::removeThread(thread);
      delete thread;
   }
   // return value for pthread lib's benefit
   return NULL;
   // the end of this function is where the created pthread will die.
}
   
//-----------------------------------------------------------------------------
Thread::Thread(ThreadRunFunction func, void* arg, bool start_thread, bool autodelete)
{
   mData = new PlatformThreadData;
   mData->mRunFunc = func;
   mData->mRunArg = arg;
   mData->mThread = this;
   mData->mThreadID = 0;
   autoDelete = autodelete;
   
   if(start_thread)
      start();
}

Thread::~Thread()
{
   stop();
   join();

   SAFE_DELETE(mData);
}

void Thread::start()
{
   if(isAlive())
      return;

   // cause start to block out other pthreads from using this Thread, 
   // at least until ThreadRunHandler exits.
   mData->mGateway.acquire();

   // reset the shouldStop flag, so we'll know when someone asks us to stop.
   shouldStop = false;

   pthread_create((pthread_t*)(&mData->mThreadID), NULL, ThreadRunHandler, mData);
}

bool Thread::join()
{
   if(!isAlive())
      return true;

   // not using pthread_join here because pthread_join cannot deal
   // with multiple simultaneous calls.
   mData->mGateway.acquire();
   mData->mGateway.release();
   return true;
}

void Thread::run(void* arg)
{
   if(mData->mRunFunc)
      mData->mRunFunc(arg);
}

bool Thread::isAlive()
{
   if(mData->mThreadID == 0)
      return false;

   if( mData->mGateway.acquire(false) ) 
   {
     mData->mGateway.release();
     return false; // we got the lock, it aint alive.
   }
   else
     return true; // we could not get the lock, it must be alive.
}

U32 Thread::getId()
{
   return mData->mThreadID;
}

U32 ThreadManager::getCurrentThreadId()
{
   return (U32)pthread_self();
}

bool ThreadManager::compare(U32 threadId_1, U32 threadId_2)
{
   return (bool)pthread_equal((pthread_t)threadId_1, (pthread_t)threadId_2);
}



class PlatformThreadStorage
{
public:
   pthread_key_t mThreadKey;
};

ThreadStorage::ThreadStorage()
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

