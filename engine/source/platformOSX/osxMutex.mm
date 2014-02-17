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

#import <pthread.h>
#import <stdlib.h>
#import <errno.h>
#import "memory/safeDelete.h"
#import "platform/threads/mutex.h"
#import "platform/threads/thread.h"
#import "platform/platform.h"
#import "platformOSX.h"

//-----------------------------------------------------------------------------

struct PlatformMutexData
{
    pthread_mutex_t   mMutex;
    bool              locked;
    ThreadIdent       lockedByThread;
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
    bool ok;
    ok = pthread_mutex_destroy( &(mData->mMutex) );
    
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
        int ok;
        ok = pthread_mutex_lock( &(mData->mMutex) );
        
        // Sanity!
        AssertFatal( ok != EINVAL, "Mutex::lockMutex() failed: invalid mutex.");
        AssertFatal( ok != EDEADLK, "Mutex::lockMutex() failed: system detected a deadlock!");
        AssertFatal( ok == 0, "Mutex::lockMutex() failed: pthread_mutex_lock() failed -- unknown reason.");
    }
    else
    {
        // No, so attempt to lock the thread without blocking.
        const int ok = pthread_mutex_trylock( &(mData->mMutex) );
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
    bool ok;
    ok = pthread_mutex_unlock( &(mData->mMutex) );
    
    // Sanity!
    AssertFatal( ok == 0, "Mutex::unlockMutex() failed: pthread_mutex_unlock() failed.");
    
    // Flag as unlocked.
    mData->locked = false;
    mData->lockedByThread = 0;
}
