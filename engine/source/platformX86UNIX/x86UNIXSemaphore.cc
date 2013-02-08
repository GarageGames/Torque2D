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

#include "platformX86UNIX/platformX86UNIX.h"
#include "platform/platformSemaphore.h"
// Instead of that mess that was here before, lets use the SDL lib to deal
// with the semaphores.

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

void *Semaphore::createSemaphore(U32 initialCount)
{
  SDL_sem *semaphore;
  semaphore = SDL_CreateSemaphore(initialCount);
  AssertFatal(semaphore, "Semaphore::createSemaphore - Failed.");
  return semaphore;
}

void Semaphore::destroySemaphore(void *semaphore)
{
  AssertFatal(semaphore, "Semaphore::destroySemaphore - Invalid semaphore");
  SDL_DestroySemaphore((SDL_sem *)semaphore);
}

bool Semaphore::acquireSemaphore(void *semaphore, bool block)
{
  AssertFatal(semaphore, "Semaphore::acquireSemaphore - Invalid semaphore");
  if (block)
    {
      if (SDL_SemWait((SDL_sem *)semaphore) < 0)
	AssertFatal(false, "Semaphore::acquieSemaphore - Wait failed.");
      return (true);
    }
  else
    {
      int res = SDL_SemTryWait((SDL_sem *)semaphore);
      if (res < 0)
	AssertFatal(false, "Semaphore::acquireSemaphore - Wait failed.");
      return (res == 0);
    }
}

void Semaphore::releaseSemaphore(void *semaphore)
{
  AssertFatal(semaphore, "Semaphore::releaseSemaphore - Invalid semaphore");
  SDL_SemPost((SDL_sem *)semaphore);
}

