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

#include "console/console.h"
#include "platformX86UNIX/platformX86UNIX.h"
#include "platform/threads/thread.h"
#include "platform/threads/mutex.h"
#include "platformX86UNIX/x86UNIXMutex.h"
#include "memory/safeDelete.h"

#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

ProcessMutex::ProcessMutex()
{
   mFD = -1;
}

ProcessMutex::~ProcessMutex()
{
   release();
}

bool ProcessMutex::acquire(const char *mutexName)
{
   if (mFD != -1)
      return false;

   dSprintf(
      mLockFileName, LockFileNameSize, 
      "/tmp/%s.lock", mutexName);

   mFD = open(mLockFileName, 
      O_CREAT | O_RDWR, 
      S_IRUSR | S_IRGRP | S_IROTH |
      S_IWUSR | S_IWGRP | S_IWOTH);

   if (mFD == -1)
   {
      Con::printf("Couldn't open file: %s", mLockFileName);
      return false;
   }

   struct flock lock;
   //lock.l_type = F_RDLCK;
   lock.l_type = F_WRLCK;
   lock.l_whence = SEEK_SET;
   lock.l_start = 0;
   lock.l_len = 0;
   lock.l_pid = getpid();
   
   if (fcntl(mFD, F_SETLK, &lock) == -1)
   {
      Con::printf("Couldn't lock file: %s, %s", 
         mLockFileName, strerror(errno));
      if (fcntl(mFD, F_GETLK, &lock) != -1)
         Con::printf("Lock owned by pid: %d", lock.l_pid);
      Con::printf("Remove the file if lock is stale");
      close(mFD);
      mFD = -1;
      return false;
   }
   return true;
}

void ProcessMutex::release()
{
   if (mFD != -1)
   {
      close(mFD);
      mFD = -1;
      unlink(mLockFileName);
   }
}

//-----------------------------------------------------------------------------

struct PlatformMutexData
{
    pthread_mutex_t   mMutex;
    bool              locked;
    U32         lockedByThread;
};

//-----------------------------------------------------------------------------

Mutex::Mutex()
{
    bool ok;
    
    // Create the mutex data.
    mData = new PlatformMutexData;

    // Initialize the system mutex.
    pthread_mutexattr_t attr;
    ok = pthread_mutexattr_init(&attr);
    ok = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    ok = pthread_mutex_init(&(mData->mMutex),&attr);
    
    // Sanity!
    AssertFatal(ok == 0, "Mutex() failed: pthread_mutex_init() failed.");
    
    // Set the initial mutex state.
    mData->locked = false;
    mData->lockedByThread = 0;
}

//-----------------------------------------------------------------------------

Mutex::~Mutex()
{
    // Destroy the system mutex.
    const bool ok = pthread_mutex_destroy( &(mData->mMutex) );
    
    // Sanity!
    AssertFatal(ok == 0, "~Mutex() failed: pthread_mutex_destroy() failed.");
    
    // Delete the mutex data.
    SAFE_DELETE( mData );
}

//-----------------------------------------------------------------------------

bool Mutex::lock( bool block )
{
    // Is this a blocking lock?
    if( block )
    {
        // Yes, so block until mutex can be locked.
        const bool ok = pthread_mutex_lock( &(mData->mMutex) );
        
        // Sanity!
        AssertFatal( ok != EINVAL, "Mutex::lockMutex() failed: invalid mutex.");
        AssertFatal( ok != EDEADLK, "Mutex::lockMutex() failed: system detected a deadlock!");
        AssertFatal( ok == 0, "Mutex::lockMutex() failed: pthread_mutex_lock() failed -- unknown reason.");
    }
    else
    {
        // No, so attempt to lock the thread without blocking.
        const bool ok = pthread_mutex_trylock( &(mData->mMutex) );
        // returns EBUSY if mutex was locked by another thread,
        // returns EINVAL if mutex was not a valid mutex pointer,
        // returns 0 if lock succeeded.
        
        // Sanity!
        AssertFatal( ok != EINVAL, "Mutex::lockMutex(non blocking) failed: invalid mutex.");
        
        // Finish if we couldn't lock the mutex.
        if( ok != 0 )
            return false;
        
        AssertFatal( ok == 0, "Mutex::lockMutex(non blocking) failed: pthread_mutex_trylock() failed -- unknown reason.");
    }
    
    // Flag as locked by the current thread.
    mData->locked = true;
    mData->lockedByThread = ThreadManager::getCurrentThreadId();
    return true;
}

//-----------------------------------------------------------------------------

void Mutex::unlock()
{
    // Unlock the thread.
    const bool ok = pthread_mutex_unlock( &(mData->mMutex) );
    
    // Sanity!
    AssertFatal( ok == 0, "Mutex::unlockMutex() failed: pthread_mutex_unlock() failed.");
    
    // Flag as unlocked.
    mData->locked = false;
    mData->lockedByThread = 0;
}


