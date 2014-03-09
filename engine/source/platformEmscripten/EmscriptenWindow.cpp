//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
// Portions Copyright (c) 2014 James S Urquhart
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
#include "io/fileStream.h"
#include "game/resource.h"
#include "game/version.h"
#include "math/mRandom.h"
#include "platformEmscripten/platformEmscripten.h"
#include "platformEmscripten/EmscriptenConsole.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platform/platform.h"
#include "platform/platformAL.h"
#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "debug/profiler.h"
#include "platformEmscripten/platformGL.h"
#include "platformEmscripten/EmscriptenOGLVideo.h"

#ifndef DEDICATED
#include "platformEmscripten/EmscriptenInputManager.h"
#endif

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h> // fork, execvp, chdir
#include <time.h> // nanosleep

#ifndef DEDICATED
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <SDL/SDL_version.h>

extern bool InitOpenGL();
extern void Cleanup(bool minimal=false);
#endif

extern "C"
{
    extern U32 _EmscriptenGetDesktopHeight();
    extern U32 _EmscriptenGetDesktopWidth();
    extern U32 _EmscriptenGetDesktopBpp();
}

EmscriptenPlatState::EmscriptenPlatState()
{
    captureDisplay = true;
    fadeWindows = true;
    backgrounded = false;
    minimized = false;

    quit = false;

    portrait = true;//-Mat Android is in portrait mode by default


    // start with something reasonable.
    fullscreen = true;

    osVersion = 0;

    dStrcpy(appWindowTitle, "Emscripten Torque Game Engine");

    // directory that contains main.cs . This will help us detect whether we are
    // running with the scripts in the bundle or not.
    mainDotCsDir = NULL;

    useRedirect = true;
    windowCreated = false;
    dedicated = false;

    printf("platstate init\n");
}

//------------------------------------------------------------------------------
static void InitWindow(const Point2I &initialSize, const char *name)
{
    gPlatState.windowSize = initialSize;
    gPlatState.setWindowTitle(name);
}

#ifndef DEDICATED
//------------------------------------------------------------------------------
static bool InitSDL()
{
   if (SDL_Init(SDL_INIT_VIDEO) != 0)
      return false;

   atexit(SDL_Quit);

   SDL_SysWMinfo sysinfo;
   SDL_VERSION(&sysinfo.version);
   //if (SDL_GetWMInfo(&sysinfo) == 0)
   //   return false;

   return true;
}

//------------------------------------------------------------------------------
static void SetAppState()
{
   U8 state = SDL_GetAppState();

   // if we're not active but we have appactive and inputfocus, set window
   // active and reactivate input
   if ((gPlatState.backgrounded || !Input::isActive()) &&
      state & SDL_APPACTIVE &&
      state & SDL_APPINPUTFOCUS)
   {
      gPlatState.backgrounded = false;
      Input::reactivate();
      Con::printf("Input activated by SetAppState");
   }
   // if we are active, but we don't have appactive or input focus,
   // deactivate input (if window not locked) and clear windowActive
   else if (!gPlatState.backgrounded && 
      !(state & SDL_APPACTIVE && state & SDL_APPINPUTFOCUS))
   {
      if (gPlatState.mouseLocked)
         Input::deactivate();
      gPlatState.backgrounded = true;
      Con::printf("Input deactivated by SetAppState");
   }
}

//------------------------------------------------------------------------------
static S32 NumEventsPending()
{
   static const int MaxEvents = 255;
   static SDL_Event events[MaxEvents];

   SDL_PumpEvents();
#ifdef DUMMY_PLATFORM
   return 0;
#else
   return SDL_PeepEvents(events, MaxEvents, SDL_PEEKEVENT, 0, SDL_LASTEVENT);
#endif
}

//------------------------------------------------------------------------------
static bool ProcessMessages()
{
#ifndef DUMMY_PLATFORM
   SDL_PumpEvents();

   // We're going to have to maintain our own list of events here, since the emscripten API
   // is a bit broken for our purposes...
   gPlatState.eventList.clear();
   gPlatState.eventList.reserve(255);

   SDL_Event e;
   while (SDL_PollEvent(&e))
   {
      gPlatState.eventList.push_back(e);
   }

   S32 numEvents = gPlatState.eventList.size();
   //if (numEvents > 0)
   //   Con::printf("ProcessMessages: %i events pending", numEvents);

   if (numEvents == 0)
      return true;

   for (int i = 0; i < numEvents; ++i)
   {
      SDL_Event& event = gPlatState.eventList[i];
      //Con::printf("Event.type == %u", event.type);
      switch (event.type)
      {
         case SDL_QUIT:
            return false;
            break;
         case SDL_VIDEORESIZE:
         case SDL_VIDEOEXPOSE:
            Game->refreshWindow();
            break;
         case SDL_USEREVENT:
            Con::printf("User event handled");
            if (event.user.code == TORQUE_SETVIDEOMODE)
            {
               SetAppState();
               // SDL will send a motion event to restore the mouse position
               // on the new window.  Ignore that if the window is locked.
               if (gPlatState.mouseLocked)
               {
                  SDL_Event tempEvent;
                  SDL_PeepEvents(&tempEvent, 1, SDL_GETEVENT, SDL_MOUSEMOTION,
                     SDL_MOUSEMOTION);
               }
            }
            break;
         case SDL_ACTIVEEVENT:
         Con::printf("Active event");
            SetAppState();
            break;
      }
   }
#endif
   return true;
}

//------------------------------------------------------------------------------
// send a destroy window event to the window.  assumes
// window is created.
void SendQuitEvent()
{
   SDL_Event quitevent;
   quitevent.type = SDL_QUIT;
   SDL_PushEvent(&quitevent);
}
#endif // DEDICATED

//------------------------------------------------------------------------------
static inline void Sleep(int secs, int nanoSecs)
{
   timespec sleeptime;
   sleeptime.tv_sec = secs;
   sleeptime.tv_nsec = nanoSecs;
   nanosleep(&sleeptime, NULL);
}

#ifndef DEDICATED
struct AlertWinState
{
      bool fullScreen;
      bool cursorHidden;
      bool inputGrabbed;
};

//------------------------------------------------------------------------------
void DisplayErrorAlert(const char* errMsg, bool showSDLError)
{
   char fullErrMsg[2048];
   dStrncpy(fullErrMsg, errMsg, sizeof(fullErrMsg));
   
   if (showSDLError)
   {
      const char* sdlerror = SDL_GetError();
      if (sdlerror != NULL && dStrlen(sdlerror) > 0)
      {
         dStrcat(fullErrMsg, "  (Error: ");
         dStrcat(fullErrMsg, sdlerror);
         dStrcat(fullErrMsg, ")");
      }
   }
   
   Platform::AlertOK("Error", fullErrMsg);
}


//------------------------------------------------------------------------------
static inline void AlertDisableVideo(AlertWinState& state)
{

   state.fullScreen = Video::isFullScreen();
   state.cursorHidden = (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE);
   state.inputGrabbed = (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON);

   if (state.fullScreen)
      SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
   if (state.cursorHidden)
      SDL_ShowCursor(SDL_ENABLE);
   if (state.inputGrabbed)
      SDL_WM_GrabInput(SDL_GRAB_OFF);
}

//------------------------------------------------------------------------------
static inline void AlertEnableVideo(AlertWinState& state)
{
   if (state.fullScreen)
      SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
   if (state.cursorHidden)
      SDL_ShowCursor(SDL_DISABLE);
   if (state.inputGrabbed)
      SDL_WM_GrabInput(SDL_GRAB_ON);
}
#endif // DEDICATED

//------------------------------------------------------------------------------
void Platform::setMouseLock(bool locked)
{
#ifndef DEDICATED
   gPlatState.mouseLocked = locked;

   UInputManager* uInputManager = 
      dynamic_cast<UInputManager*>( Input::getManager() );

   if ( uInputManager && uInputManager->isEnabled() && 
      Input::isActive() )
      uInputManager->setWindowLocked(locked);
#endif
}

//------------------------------------------------------------------------------
void Platform::process()
{
   PROFILE_START(XUX_PlatformProcess);

   if (gPlatState.windowCreated)
   {
#ifndef DEDICATED
      // process window events
      PROFILE_START(XUX_ProcessMessages);
      bool quit = !ProcessMessages();
      PROFILE_END();
      if(quit)
      {
         // generate a quit event
         Event quitEvent;
         quitEvent.type = QuitEventType;
         Game->postEvent(quitEvent);
      }

      // process input events
      PROFILE_START(XUX_InputProcess);
      Input::process();
      PROFILE_END();

      // if we're not the foreground window, sleep for 1 ms
      if (gPlatState.backgrounded)
         Sleep(0, getBackgroundSleepTime() * 1000000);
#endif
   }
   else
   {
     Sleep(0, getBackgroundSleepTime() * 1000000);
   }
   PROFILE_END();
}

//------------------------------------------------------------------------------
const Point2I &Platform::getWindowSize()
{
   return gPlatState.windowSize;
}

//------------------------------------------------------------------------------
void Platform::setWindowSize( U32 newWidth, U32 newHeight )
{
    gPlatState.windowSize = Point2I(newWidth, newHeight);
}

//------------------------------------------------------------------------------
void Platform::minimizeWindow()
{
#ifndef DEDICATED
   if (gPlatState.windowCreated)
      SDL_WM_IconifyWindow();
#endif
}

//------------------------------------------------------------------------------
void Platform::restoreWindow()
{
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
   Cleanup();
}

//------------------------------------------------------------------------------
void Platform::init()
{
   Con::printf("Platform::init");

   // Set the platform variable for the scripts
   Con::setVariable( "$platform", "x86UNIX" );
   Con::setVariable( "$platformUnixType", "emscripten" );

   EmscriptenConsole::create();
   
#ifndef DEDICATED
   Con::printf("Dedicated == %i", gPlatState.dedicated);
   // if we're not dedicated do more initialization
   //if (!gPlatState.dedicated)
   {
     printf("Init non-dedicated\n");
      // init SDL
      if (!InitSDL())
      {
         Con::printf( "   Unable to initialize SDL." );
         Platform::AlertOK("Error", "Unable to initialize SDL.");
         exit(1);
      }

      // initialize input
      Input::init();

      Con::printf( "Video Init:" );

      // initialize video
      Video::init();
      if ( Video::installDevice( OpenGLDevice::create() ) )
         Con::printf( "   OpenGL display device detected." );
      else
         Con::printf( "   OpenGL display device not detected." );
      
      Con::printf(" ");
   }
#endif
}

//------------------------------------------------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
#ifndef DEDICATED
   // initialize window
   InitWindow(initialSize, name);
   if (!InitOpenGL()) {
      Con::printf( "   Unable to init OpenGL window." );
      exit(1);
   }
#endif
}

//------------------------------------------------------------------------------
// Web browser function:
//------------------------------------------------------------------------------
bool Platform::openWebBrowser( const char* webAddress )
{
    return false;
}

//------------------------------------------------------------------------------
ConsoleFunction( getDesktopResolution, const char*, 1, 1, 
   "getDesktopResolution()" )
{
   if (!gPlatState.windowCreated)
      return "0 0 0";

   char buffer[256];
   char* returnString = Con::getReturnBuffer( dStrlen( buffer ) + 1 );

   Resolution res = Video::getDesktopResolution();

   dSprintf( buffer, sizeof( buffer ), "%d %d %d", 
      res.w,
      res.h, 
      res.bpp );
   dStrcpy( returnString, buffer );
   return( returnString );
}

//------------------------------------------------------------------------------
// Silly Korean registry key checker:
//------------------------------------------------------------------------------
ConsoleFunction( isKoreanBuild, bool, 1, 1, "isKoreanBuild()" )
{
   Con::printf("WARNING: isKoreanBuild() is unimplemented");
   return false;
}

//------------------------------------------------------------------------------

void Platform::setWindowTitle( const char* title )
{
#ifndef DEDICATED
   gPlatState.setWindowTitle(title);
   SDL_WM_SetCaption(gPlatState.appWindowTitle, NULL);
#endif
}

//------------------------------------------------------------------------------

Resolution Video::getDesktopResolution()
{
   Resolution  Result;

   Result.h   = _EmscriptenGetDesktopHeight();
   Result.w   = _EmscriptenGetDesktopWidth();
   Result.bpp  = _EmscriptenGetDesktopBpp();

  return Result;
}

//-----------------------------------------------------------------------------
void Platform::outputDebugString( const char *string )
{
    fprintf(stderr, "%s", string);
    fprintf(stderr, "\n" );
    fflush(stderr);
}
