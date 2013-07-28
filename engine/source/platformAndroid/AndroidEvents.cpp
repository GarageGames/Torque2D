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

#include "platformAndroid/platformAndroid.h"
#include "platform/event.h"
#include "platform/platformInput.h"
#include "game/gameInterface.h"
#include "platform/threads/thread.h"
#include "platform/platformVideo.h"
#include <pthread.h>

#include "platformAndroid/AndroidUtil.h"
#include "platformAndroid/AndroidEvents.h"
#include "platformAndroid/AndroidAlerts.h"
#include "gui/guiCanvas.h"



/*
EventHandlerRef gWinMouseEventHandlerRef  = NULL;
EventHandlerRef gAppMouseEventHandlerRef  = NULL;
*/



//-----------------------------------------------------------------------------
static void _OnActivate(bool activating)
{
   if(activating)
   {
      Input::activate();
      Game->refreshWindow();
      platState.backgrounded = false;
      pthread_kill((pthread_t)platState.torqueThreadId, SIGALRM);
   }
   else
   {
      Input::deactivate();
      platState.backgrounded = true;
   }
}

//-----------------------------------------------------------------------------
// here we manually poll for events, and send them to the dispatcher.
// we only use this in single-threaded mode.
static void _AndroidPollEvents()
{
}



//-----------------------------------------------------------------------------
void Platform::enableKeyboardTranslation(void)
{
	platState.tsmActive=true;
}

//-----------------------------------------------------------------------------
void Platform::disableKeyboardTranslation(void)
{
	platState.tsmActive=false;
}


void Platform::setMouseLock(bool locked)
{
}




//-----------------------------------------------------------------------------
void Platform::process()
{
	// TODO: HID input
	
	// ProcessMessages() manually polls for events when we are single threaded
	//   if(ThreadManager::isCurrentThread(platState.firstThreadId))
	//     _AndroidPollEvents();
	
	// Some things do not get carbon events, we must always poll for them.
	// HID ( usb gamepad et al ) input, for instance.
	Input::process();
	
	if(platState.ctxNeedsUpdate)
	{
		//aglUpdateContext(platState.ctx);
		platState.ctxNeedsUpdate=false;
	}
}

