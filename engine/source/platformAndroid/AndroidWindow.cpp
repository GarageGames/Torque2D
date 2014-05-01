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
#include "platform/platformVideo.h"
#include "platformAndroid/AndroidOGLVideo.h"
#include "platformAndroid/AndroidConsole.h"
#include "platform/platformInput.h"
#include "game/gameInterface.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "platformAndroid/AndroidEvents.h"
#include "platform/threads/thread.h"

#include "platformAndroid/AndroidWindow.h"

#include "platformAndroid/platformGL.h"

bool getStatusBarHidden();
bool setStatusBarHidden(bool);
void setStatusBarType(S32);

//------------------------------------------------------------------------------
#pragma mark ---- PlatState ----
AndroidPlatState platState;

AndroidPlatState::AndroidPlatState()
{


    captureDisplay = true;
    fadeWindows = true;
    backgrounded = false;
    minimized = false;

    quit = false;

    portrait = true;//-Mat Android is in portrait mode by default


    // start with something reasonable.
    desktopBitsPixel = ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH;
    desktopWidth = ANDROID_DEFAULT_RESOLUTION_X;
    desktopHeight = ANDROID_DEFAULT_RESOLUTION_Y;
    fullscreen = true;

    osVersion = 0;

    dStrcpy(appWindowTitle, "Android Torque Game Engine");

    // Semaphore for alerts. We put the app in a modal state by blocking the main
    alertSemaphore = Semaphore::createSemaphore(0);

    // directory that contains main.cs . This will help us detect whether we are
    // running with the scripts in the bundle or not.
    mainDotCsDir = NULL;
}


//------------------------------------------------------------------------------
// DGL, the Gui, and TS use this for various purposes.
const Point2I &Platform::getWindowSize()
{
    return platState.windowSize;
}


//------------------------------------------------------------------------------
// save the window size, for DGL's use
void Platform::setWindowSize(U32 newWidth, U32 newHeight)
{
    platState.windowSize.set(newWidth, newHeight);
}

//------------------------------------------------------------------------------
// Issue a minimize event. The standard handler will handle it.
void Platform::minimizeWindow()
{
    //no minimizing on Android
}

void Platform::restoreWindow()
{
    //no minimizing on Android
}

//------------------------------------------------------------------------------
void Platform::setWindowTitle(const char *title)
{
    //no window titles on Android
}



#pragma mark ---- Init funcs  ----
//------------------------------------------------------------------------------
void Platform::init()
{
    Con::setVariable("$platform", "Android");
    
    AndroidConsole::create();

    //if ( !AndroidConsole::isEnabled() )
    Input::init();

    // allow users to specify whether to capture the display or not when going fullscreen
    Con::addVariable("pref::Android::captureDisplay", TypeBool, &platState.captureDisplay);
    Con::addVariable("pref::Android::fadeWindows", TypeBool, &platState.fadeWindows);

    // create the opengl display device
    DisplayDevice *dev = NULL;
    Con::printf("Video Init:");
    Video::init();
    dev = OpenGLDevice::create();
    if (dev)
        Con::printf("   Accelerated OpenGL display device detected.");
    else
        Con::printf("   Accelerated OpenGL display device not detected.");

    // and now we can install the device.
    Video::installDevice(dev);
    Con::printf("");
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
    setMouseLock(false);
    Video::destroy();
    Input::destroy();
    AndroidConsole::destroy();
}


//Hidden by Default. 1 Black Opaque, 2 Black Translucent
S32 gScreenOrientation = 0;
S32 gStatusBarType = 0;
bool gStatusBarHidden = true;

//------------------------------------------------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
    S32 resolutionWidth = ANDROID_DEFAULT_RESOLUTION_X;
    S32 resolutionHeight = ANDROID_DEFAULT_RESOLUTION_Y;

    dSprintf(platState.appWindowTitle, sizeof(platState.appWindowTitle), name);

    platState.windowSize.x = _AndroidGetScreenWidth();
    platState.windowSize.y = _AndroidGetScreenHeight();

    //Default to landscape, and run into portrait if requested.
    S32 orientation = _AndroidGameGetOrientation();
    if (orientation == ACONFIGURATION_ORIENTATION_PORT)
    {
    	gScreenOrientation = 1;
    	platState.portrait = true;
    }
    else
    {
    	gScreenOrientation = 0;
    	platState.portrait = false;
    }

    bool fullScreen;
    U32 bpp = Con::getIntVariable("$pref::Android::ScreenDepth"); //ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH;
    if (!bpp)
    {
        Con::printf("Default BPP Chosen , $pref::Android::ScreenDepth was not found.");
        bpp = ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH;
    }
    
    fullScreen = true;
    //
    DisplayDevice::init();
    
    // this will create a rendering context & window
    bool ok = Video::setDevice("OpenGL", platState.windowSize.x, platState.windowSize.y, bpp, fullScreen);
    if (!ok)
    {
        AssertFatal( false, "Could not find a compatible display device!" );
    }
    
    //Luma:	Clear frame buffer to BLACK to start with
    //NOTE:	This should probably be set by the user to be the color closest to Default.png in order to minimize any popping effect... $pref:: anyone? Are $pref::s even valid at this point in the Init process?
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

//--------------------------------------
// run app function: not applicable to Android
//--------------------------------------
// run other apps
int runApp(const char *fileName, const char *batchArgs, bool blocking = false)
{
    return 0;
}

bool appIsRunning(int batchId)
{
    return false;
}

ConsoleFunction(setScreenOrientation, bool, 3, 3, "Sets the orientation of the screen ( portrait/landscape, upside down or right-side up )\n"
        "@(bool portrait, bool upside_down)"){
    adprintf("screen orientation is set via the manifest file on android");
	return false;
}
