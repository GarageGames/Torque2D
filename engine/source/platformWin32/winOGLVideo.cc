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

#include "platformWin32/platformGL.h"
#include "platformWin32/platformWin32.h"
#include "platformWin32/winOGLVideo.h"
#include "console/console.h"
#include "math/mPoint.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "io/fileStream.h"

//------------------------------------------------------------------------------

struct CardProfile
{
   const char *vendor;     // manufacturer
   const char *renderer;   // driver name

   bool safeMode;          // destroy rendering context for resolution change
   bool lockArray;         // allow compiled vertex arrays
   bool subImage;          // allow glTexSubImage*
   bool fogTexture;        // require bound texture for combine extension
   bool noEnvColor;        // no texture environment color
   bool clipHigh;          // clip high resolutions
    bool deleteContext;		// delete rendering context
    bool texCompress;			// allow texture compression
    bool interiorLock;		// lock arrays for Interior render
    bool skipFirstFog;		// skip first two-pass fogging (dumb 3Dfx hack)
    bool only16;				// inhibit 32-bit resolutions
    bool noArraysAlpha;	// don't use glDrawArrays with a GL_ALPHA texture

    const char *proFile;		// explicit profile of graphic settings
};

struct OSCardProfile
{
   const char *vendor;     // manufacturer
   const char *renderer;   // driver name

   bool allowOpenGL;			// allow OpenGL driver
    bool preferOpenGL;		// prefer OpenGL driver
};

static Vector<CardProfile> sCardProfiles(__FILE__, __LINE__);
static Vector<OSCardProfile> sOSCardProfiles(__FILE__, __LINE__);

//------------------------------------------------------------------------------

#include "winOGLVideo_ScriptBinding.h"

//------------------------------------------------------------------------------

struct ProcessorProfile
{
    U16 clock;  // clock range max
    U16 adjust; // CPU adjust
};

static U8 sNumProcessors = 4;
static ProcessorProfile sProcessorProfiles[] =
{
    {  400,  0 },
    {  600,  5 },
    {  800, 10 },
    { 1000, 15 },
};

struct SettingProfile
{
    U16 performance;        // metric range max
    const char *settings;   // default file
};

static U8 sNumSettings = 3;
static SettingProfile sSettingProfiles[] =
{
    {  33, "LowProfile.cs" },
    {  66, "MediumProfile.cs" },
    { 100, "HighProfile.cs" },
};

//------------------------------------------------------------------------------

static void clearCardProfiles()
{
   while (sCardProfiles.size())
   {
      dFree((char *) sCardProfiles.last().vendor);
      dFree((char *) sCardProfiles.last().renderer);

        dFree((char *) sCardProfiles.last().proFile);

      sCardProfiles.decrement();
   }
}

static void clearOSCardProfiles()
{
   while (sOSCardProfiles.size())
   {
      dFree((char *) sOSCardProfiles.last().vendor);
      dFree((char *) sOSCardProfiles.last().renderer);

      sOSCardProfiles.decrement();
   }
}

static void execScript(const char *scriptFile)
{
    // execute the script
   FileStream str;

   if (!str.open(scriptFile, FileStream::Read))
      return;

   U32 size = str.getStreamSize();
   char *script = new char[size + 1];

   str.read(size, script);
   str.close();

   script[size] = 0;
   Con::executef(2, "eval", script);
   delete[] script;
}

static void profileSystem(const char *vendor, const char *renderer)
{

   //Con::executef(2, "exec", "scripts/CardProfiles.cs");
    execScript("CardProfiles.cs");

    const char *arch;
    OSVERSIONINFO OSVersionInfo;
    const char *os = NULL;
    char osProfiles[64];

    if (dStrstr(PlatformSystemInfo.processor.name,"AMD") != NULL)
        arch = "AMD";
    else
        arch = "Intel";

    
    dMemset( &OSVersionInfo, 0, sizeof( OSVERSIONINFO ) );
   OSVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
   if ( GetVersionEx( &OSVersionInfo )	)
    {
        if (OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
            if ( OSVersionInfo.dwMinorVersion == 0 )
            {
                if (dStrcmp(arch,"Intel") == 0)
                    os = "W95";
            }
            else if ( OSVersionInfo.dwMinorVersion == 10 )	
                if ( OSVersionInfo.szCSDVersion[1] != 'A' )
                    os = "W98";
                else
                    os = "W98SE";
            else
                os = "WME";
        else
            if ( OSVersionInfo.dwMajorVersion >= 5 )
                os = "W2K";
        
        if ( os != NULL )
        {
            dSprintf(osProfiles,64,"%s%sCardProfiles.cs",arch,os);
            //Con::executef(2, "exec", osProfiles);
            execScript(osProfiles);
        }
    }

    const char *proFile = NULL;
   U32 i;

   for (i = 0; i < (U32)sCardProfiles.size(); ++i)
      if (dStrstr(vendor, sCardProfiles[i].vendor) &&
          (!dStrcmp(sCardProfiles[i].renderer, "*") ||
           dStrstr(renderer, sCardProfiles[i].renderer)))
      {
         Con::setBoolVariable("$pref::Video::safeModeOn", sCardProfiles[i].safeMode);
         Con::setBoolVariable("$pref::OpenGL::disableEXTCompiledVertexArray", !sCardProfiles[i].lockArray);
         Con::setBoolVariable("$pref::OpenGL::disableSubImage", !sCardProfiles[i].subImage);
         Con::setBoolVariable("$pref::TS::fogTexture", sCardProfiles[i].fogTexture);
         Con::setBoolVariable("$pref::OpenGL::noEnvColor", sCardProfiles[i].noEnvColor);
         Con::setBoolVariable("$pref::Video::clipHigh", sCardProfiles[i].clipHigh);
            if (!sCardProfiles[i].deleteContext)
            {
                OSVERSIONINFO OSVersionInfo;

            // HACK: The Voodoo3/5 on W2K crash when deleting a rendering context
                // So we're not deleting it.
                // Oh, and the Voodoo3 returns a Banshee renderer string under W2K
            dMemset( &OSVersionInfo, 0, sizeof( OSVERSIONINFO ) );
            OSVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
            if ( GetVersionEx( &OSVersionInfo ) &&
                      OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
                      OSVersionInfo.dwMajorVersion == 5)
                    Con::setBoolVariable("$pref::Video::deleteContext", false);
                else
                    Con::setBoolVariable("$pref::Video::deleteContext", true);
            }
            else
                Con::setBoolVariable("$pref::Video::deleteContext", true);
            Con::setBoolVariable("$pref::OpenGL::disableARBTextureCompression", !sCardProfiles[i].texCompress);
            Con::setBoolVariable("$pref::Interior::lockArrays", sCardProfiles[i].interiorLock);
            Con::setBoolVariable("$pref::TS::skipFirstFog", sCardProfiles[i].skipFirstFog);
            Con::setBoolVariable("$pref::Video::only16", sCardProfiles[i].only16);
            Con::setBoolVariable("$pref::OpenGL::noDrawArraysAlpha", sCardProfiles[i].noArraysAlpha);

            proFile = sCardProfiles[i].proFile;

         break;
      }

   // defaults
    U16 glProfile;

   if (!proFile)
   {
      // no driver GL profile -- make one via weighting GL extensions
      glProfile = 25;

      glProfile += gGLState.suppARBMultitexture * 25;
      glProfile += gGLState.suppLockedArrays * 15;
      glProfile += gGLState.suppVertexArrayRange * 10;
      glProfile += gGLState.suppTextureEnvCombine * 5;
      glProfile += gGLState.suppPackedPixels * 5;
      glProfile += gGLState.suppTextureCompression * 5;
      glProfile += gGLState.suppS3TC * 5;
      glProfile += gGLState.suppFXT1 * 5;

      Con::setBoolVariable("$pref::Video::safeModeOn", true);
      Con::setBoolVariable("$pref::OpenGL::disableEXTCompiledVertexArray", false);
      Con::setBoolVariable("$pref::OpenGL::disableSubImage", false);
      Con::setBoolVariable("$pref::TS::fogTexture", false);
      Con::setBoolVariable("$pref::OpenGL::noEnvColor", false);
      Con::setBoolVariable("$pref::Video::clipHigh", false);
        Con::setBoolVariable("$pref::Video::deleteContext", true);
        Con::setBoolVariable("$pref::OpenGL::disableARBTextureCompression", false);
        Con::setBoolVariable("$pref::Interior::lockArrays", true);
        Con::setBoolVariable("$pref::TS::skipFirstFog", false);
        Con::setBoolVariable("$pref::Video::only16", false);
        Con::setBoolVariable("$pref::OpenGL::noDrawArraysAlpha", false);
   }

    Con::setVariable("$pref::Video::profiledVendor", vendor);
   Con::setVariable("$pref::Video::profiledRenderer", renderer);

    if (!Con::getBoolVariable("$DisableSystemProfiling") &&
         ( dStrcmp(vendor, Con::getVariable("$pref::Video::defaultsVendor")) ||
        dStrcmp(renderer, Con::getVariable("$pref::Video::defaultsRenderer")) ))
    {
        if (proFile)
        {
            char settings[64];

            dSprintf(settings,64,"%s.cs",proFile);
            //Con::executef(2, "exec", settings);
            execScript(settings);
        }
        else
        {
        U16 adjust;

        // match clock with profile
        for (i = 0; i < sNumProcessors; ++i)
        {
            adjust = sProcessorProfiles[i].adjust;

            if (PlatformSystemInfo.processor.mhz < sProcessorProfiles[i].clock) break;
        }

        const char *settings;

        // match performance metric with profile
        for (i = 0; i < sNumSettings; ++i)
        {
            settings = sSettingProfiles[i].settings;

            if (glProfile+adjust <= sSettingProfiles[i].performance) break;
        }

        //Con::executef(2, "exec", settings);
            execScript(settings);
        }

        bool match = false;

        for (i = 0; i < (U32)sOSCardProfiles.size(); ++i)
            if (dStrstr(vendor, sOSCardProfiles[i].vendor) &&
                 (!dStrcmp(sOSCardProfiles[i].renderer, "*") ||
              dStrstr(renderer, sOSCardProfiles[i].renderer)))
            {
                Con::setBoolVariable("$pref::Video::allowOpenGL", sOSCardProfiles[i].allowOpenGL);
                Con::setBoolVariable("$pref::Video::preferOpenGL", sOSCardProfiles[i].preferOpenGL);
            
                match = true;

                break;
            }

        if (!match)
        {
            Con::setBoolVariable("$pref::Video::allowOpenGL", true);
            Con::setBoolVariable("$pref::Video::preferOpenGL", true);
        }

        Con::setVariable("$pref::Video::defaultsVendor", vendor);
    Con::setVariable("$pref::Video::defaultsRenderer", renderer);
    }

   clearCardProfiles();
    clearOSCardProfiles();
}

//------------------------------------------------------------------------------
OpenGLDevice::OpenGLDevice()
{
   initDevice();
}


//------------------------------------------------------------------------------
void OpenGLDevice::initDevice()
{
   // Set the device name:
   mDeviceName = "OpenGL";

   // Let Parent enumerate valid resolutions.
   Parent::initDevice();
}


//------------------------------------------------------------------------------
bool OpenGLDevice::activate( U32 width, U32 height, U32 bpp, bool fullScreen )
{
   Con::printf( "Activating the OpenGL display device..." );

   bool needResurrect = false;

   // If the rendering context exists, delete it:
   if ( winState.hGLRC )
   {
      Con::printf( "Killing the texture manager..." );
      Game->textureKill();
      needResurrect = true;

      Con::printf( "Making the rendering context not current..." );
      if ( !dwglMakeCurrent( NULL, NULL ) )
      {
         AssertFatal( false, "OpenGLDevice::activate\ndwglMakeCurrent( NULL, NULL ) failed!" );
         return false;
      }

      Con::printf( "Deleting the rendering context ..." );
      if ( !dwglDeleteContext( winState.hGLRC ) )
      {
         AssertFatal( false, "OpenGLDevice::activate\ndwglDeleteContext failed!" );
         return false;
      }
      winState.hGLRC = NULL;
   }

   // [neo, 5/31/2007 - #3174]
   if( winState.appMenu )
   {
      DestroyMenu( winState.appMenu );

      winState.appMenu = NULL;
   }

   // If the window already exists, kill it so we can start fresh:
   if ( winState.appWindow )
   {
      if ( winState.appDC )
      {
         Con::printf( "Releasing the device context..." );
         ReleaseDC( winState.appWindow, winState.appDC );
         winState.appDC = NULL;
      }
   
      Con::printf( "Destroying the window..." );
      DestroyWindow( winState.appWindow );
      winState.appWindow = NULL;
   }

   // If OpenGL library already loaded, shut it down and reload it:
   if ( winState.hinstOpenGL )
      GL_Shutdown();

   GL_Init( "opengl32", "glu32" );

    static bool onceAlready = false;
    bool profiled = false;

    if ( !mFullScreenOnly && fullScreen && !onceAlready )
    {	
        OSVERSIONINFO OSVersionInfo;

        // HACK: The Voodoo5 on W2K will only work if the initial rendering
        // context is windowed.  Can you believe this crap?
        dMemset( &OSVersionInfo, 0, sizeof( OSVERSIONINFO ) );
        OSVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
        if ( GetVersionEx( &OSVersionInfo ) &&
              OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
              OSVersionInfo.dwMajorVersion == 5 )

              //MIN_RESOLUTION defined in platformWin32/platformGL.h
            if ( !setScreenMode(	MIN_RESOLUTION_X, MIN_RESOLUTION_Y, bpp, false, true, false ) )
                return false;
            else
            {
                const char* vendorString   = (const char*) glGetString( GL_VENDOR );
                const char* rendererString = (const char*) glGetString( GL_RENDERER );

                // only do this for the first session
                if (!Con::getBoolVariable("$DisableSystemProfiling") &&
                     ( dStrcmp(vendorString, Con::getVariable("$pref::Video::profiledVendor")) ||
                    dStrcmp(rendererString, Con::getVariable("$pref::Video::profiledRenderer")) ))
                {
                profileSystem(vendorString, rendererString);
                    profiled = true;
                }
            }

        onceAlready = true;
    }
    // Set the resolution:
   if ( !setScreenMode( width, height, bpp, ( fullScreen || mFullScreenOnly ), true, false ) )
      return false;

   // Get original gamma ramp
   mRestoreGamma = GetDeviceGammaRamp(winState.appDC, mOriginalRamp);

   // Output some driver info to the console:
   const char* vendorString   = (const char*) glGetString( GL_VENDOR );
   const char* rendererString = (const char*) glGetString( GL_RENDERER );
   const char* versionString  = (const char*) glGetString( GL_VERSION );
   Con::printf( "OpenGL driver information:" );
   if ( vendorString )
      Con::printf( "  Vendor: %s", vendorString );
   if ( rendererString )
      Con::printf( "  Renderer: %s", rendererString );
   if ( versionString )
      Con::printf( "  Version: %s", versionString );

   if ( needResurrect )
   {
      // Reload the textures:
      Con::printf( "Resurrecting the texture manager..." );
      Game->textureResurrect();
   }

   GL_EXT_Init();

   Con::setVariable( "$pref::Video::displayDevice", mDeviceName );

   // only do this for the first session
   if (!profiled &&
     !Con::getBoolVariable("$DisableSystemProfiling") &&
     (	dStrcmp(vendorString, Con::getVariable("$pref::Video::profiledVendor")) ||
        dStrcmp(rendererString, Con::getVariable("$pref::Video::profiledRenderer")) ))
   {
      profileSystem(vendorString, rendererString);
        profiled = true;
    }

    if (profiled)
    {
      U32 width, height, bpp;

        if (Con::getBoolVariable("$pref::Video::clipHigh", false))
            for (S32 i = mResolutionList.size()-1; i >= 0; --i)
                if (mResolutionList[i].w > 1152 || mResolutionList[i].h > 864)
                    mResolutionList.erase(i);

        if (Con::getBoolVariable("$pref::Video::only16", false))
            for (S32 i = mResolutionList.size()-1; i >= 0; --i)
                if (mResolutionList[i].bpp == 32)
                    mResolutionList.erase(i);

        bool bPrefFullScreen = Con::getBoolVariable("$pref::Video::fullScreen", true);
      
      if( bPrefFullScreen || fullScreen )
         dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &width, &height, &bpp);
      else
         dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &width, &height, &bpp);
      setScreenMode(width, height, bpp,
                    bPrefFullScreen || fullScreen, false, false);
   }

   // Do this here because we now know about the extensions:
   if ( gGLState.suppSwapInterval )
      setVerticalSync( !Con::getBoolVariable( "$pref::Video::disableVerticalSync" ) );
    Con::setBoolVariable("$pref::OpenGL::allowTexGen", true);

   return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::shutdown()
{
   Con::printf( "Shutting down the OpenGL display device..." );

   if ( winState.hGLRC )
   {
      if (mRestoreGamma)
         SetDeviceGammaRamp(winState.appDC, mOriginalRamp);

      Con::printf( "Making the GL rendering context not current..." );
      dwglMakeCurrent( NULL, NULL );
        if ( Con::getBoolVariable("$pref::Video::deleteContext", true) )
      {
         Con::printf( "Deleting the GL rendering context..." );
            dwglDeleteContext( winState.hGLRC );
      }
      winState.hGLRC = NULL;
   }

   if ( winState.appDC )
   {
      Con::printf( "Releasing the device context..." );
      ReleaseDC( winState.appWindow, winState.appDC );
      winState.appDC = NULL;
   }

   if ( smIsFullScreen )
   {
      Con::printf( "Restoring the desktop display settings (%dx%dx%d)...", winState.desktopWidth, winState.desktopHeight, winState.desktopBitsPixel );
      ChangeDisplaySettings( NULL, 0 );
   }
}


//------------------------------------------------------------------------------
// This is the real workhorse function of the DisplayDevice...
//
bool OpenGLDevice::setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint )
{
   HWND curtain = NULL;
   char errorMessage[256];
   Resolution newRes( width, height, bpp );
   bool newFullScreen = fullScreen;
   bool safeModeOn = Con::getBoolVariable( "$pref::Video::safeModeOn" );

   if ( !newFullScreen && mFullScreenOnly )
   {
      Con::warnf( ConsoleLogEntry::General, "OpenGLDevice::setScreenMode - device or desktop color depth does not allow windowed mode!" );
      newFullScreen = true;
   }

   if ( !newFullScreen && ( (S32)newRes.w >= winState.desktopClientWidth || (S32)newRes.h >= winState.desktopClientHeight ) )
   {
      Con::warnf( ConsoleLogEntry::General, "OpenGLDevice::setScreenMode -- can't switch to resolution larger than desktop in windowed mode!" );
      S32 resIndex = 0;
      S32 bestScore = 0, thisScore = 0;
      for ( S32 i = 0; i < mResolutionList.size(); i++ )
      {
         if ( newRes == mResolutionList[i] )
         {
            resIndex = i;
            break;
         }
         else if( (S32)newRes.h <= winState.desktopClientHeight && (S32)newRes.w <= winState.desktopClientWidth )
         {
            thisScore = abs( S32( newRes.w ) - S32( mResolutionList[i].w ) )
               + abs( S32( newRes.h ) - S32( mResolutionList[i].h ) )
               + ( newRes.bpp == mResolutionList[i].bpp ? 0 : 1 );

            if ( !bestScore || ( thisScore < bestScore ) )
            {
               bestScore = thisScore;
               resIndex = i;
            }
         }
      }
      
   }

   //MIN_RESOLUTION defined in platformWin32/platformGL.h
   if ( newRes.w < MIN_RESOLUTION_X || newRes.h < MIN_RESOLUTION_Y )
   {
      Con::warnf( ConsoleLogEntry::General, "OpenGLDevice::setScreenMode -- can't go smaller than 640x480!" );
      return false;
   }

   if ( newFullScreen )
   {
      if (newRes.bpp != 16 && mFullScreenOnly)
         newRes.bpp = 16;

      // Match the new resolution to one in the list:
      U32 resIndex = 0;
      U32 bestScore = 0, thisScore = 0;
      for ( int i = 0; i < mResolutionList.size(); i++ )
      {
         if ( newRes == mResolutionList[i] )
         {
            resIndex = i;
            break;
         }
         else
         {
            thisScore = abs( S32( newRes.w ) - S32( mResolutionList[i].w ) )
                      + abs( S32( newRes.h ) - S32( mResolutionList[i].h ) )
                      + ( newRes.bpp == mResolutionList[i].bpp ? 0 : 1 );

            if ( !bestScore || ( thisScore < bestScore ) )
            {
               bestScore = thisScore;
               resIndex = i;
            }
         }
      }

      newRes = mResolutionList[resIndex];
   }
   else
   {
      // Basically ignore the bit depth parameter:
      newRes.bpp = winState.desktopBitsPixel;
   }

   // Return if already at this resolution:
   if ( !forceIt && newRes == smCurrentRes && newFullScreen == smIsFullScreen )
      return true;

   Con::printf( "Setting screen mode to %dx%dx%d (%s)...", newRes.w, newRes.h, newRes.bpp, ( newFullScreen ? "fs" : "w" ) );

   bool needResurrect = false;

   if ( ( newRes.bpp != smCurrentRes.bpp ) || ( safeModeOn && ( ( smIsFullScreen != newFullScreen ) || newFullScreen ) ) )
   {
      // Delete the rendering context:
      if ( winState.hGLRC )
      {
         if (!Video::smNeedResurrect)
            {
            Con::printf( "Killing the texture manager..." );
            Game->textureKill();
            needResurrect = true;
            }

         Con::printf( "Making the rendering context not current..." );
         if ( !dwglMakeCurrent( NULL, NULL ) )
         {
            AssertFatal( false, "OpenGLDevice::setScreenMode\ndwglMakeCurrent( NULL, NULL ) failed!" );
            return false;
         }

         Con::printf( "Deleting the rendering context..." );
         if ( Con::getBoolVariable("$pref::Video::deleteContext",true) &&
              !dwglDeleteContext( winState.hGLRC ) )
         {
            AssertFatal( false, "OpenGLDevice::setScreenMode\ndwglDeleteContext failed!" );
            return false;
         }
         winState.hGLRC = NULL;
      }

      // Release the device context:
      if ( winState.appDC )
      {
         Con::printf( "Releasing the device context..." );
         ReleaseDC( winState.appWindow, winState.appDC );
         winState.appDC = NULL;
      }
      
      // [neo, 5/31/2007 - #3174]
      if( winState.appMenu )
      {
         DestroyMenu( winState.appMenu );

         winState.appMenu = NULL;
      }

      // Destroy the window:
      if ( winState.appWindow )
      {
         Con::printf( "Destroying the window..." );
         DestroyWindow( winState.appWindow );
         winState.appWindow = NULL;
      }
   }
   else if ( smIsFullScreen != newFullScreen )
   {
      // Change the window style:
      Con::printf( "Changing the window style..." );
      S32 windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      if ( newFullScreen )
         windowStyle |= ( WS_MAXIMIZE | WS_VISIBLE);
      else
         windowStyle |= ( WS_OVERLAPPEDWINDOW );

      if ( winState.appWindow && !SetWindowLong( winState.appWindow, GWL_STYLE, windowStyle ) )
         Con::errorf( "SetWindowLong failed to change the window style!" );
   }

   if( winState.appWindow && !newFullScreen )
      ShowWindow( winState.appWindow, SW_NORMAL );
   else
      ShowWindow( winState.appWindow, SW_MAXIMIZE );


   U32 test;
   if ( newFullScreen )
   {
      // Change the display settings:
      DEVMODE devMode;
      dMemset( &devMode, 0, sizeof( devMode ) );
      devMode.dmSize       = sizeof( devMode );
      devMode.dmPelsWidth  = newRes.w;
      devMode.dmPelsHeight = newRes.h;
      devMode.dmBitsPerPel = newRes.bpp;
      devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

      Con::printf( "Changing the display settings to %dx%dx%d...", newRes.w, newRes.h, newRes.bpp );
      curtain = CreateCurtain( newRes.w, newRes.h );
      test = ChangeDisplaySettings( &devMode, CDS_FULLSCREEN );
      if ( test != DISP_CHANGE_SUCCESSFUL )
      {
         smIsFullScreen = false;
         Con::setBoolVariable( "$pref::Video::fullScreen", false );
         
         ChangeDisplaySettings( NULL, 0 );
         Con::errorf( ConsoleLogEntry::General, "OpenGLDevice::setScreenMode - ChangeDisplaySettings failed." );
         switch( test )
         {
            case DISP_CHANGE_RESTART:
               Platform::AlertOK( "Display Change Failed", "You must restart your machine to get the specified mode." );
               break;

            case DISP_CHANGE_BADMODE:
               Platform::AlertOK( "Display Change Failed", "The specified mode is not supported by this device." );
               break;

            default:
               Platform::AlertOK( "Display Change Failed", "Hardware failed to switch to the specified mode." );
               break;
         };
         DestroyWindow( curtain );
         return false;
      }
      else
         smIsFullScreen = true;
   }
   else if ( smIsFullScreen )
   {
      Con::printf( "Changing to the desktop display settings (%dx%dx%d)...", winState.desktopWidth, winState.desktopHeight, winState.desktopBitsPixel );
      ChangeDisplaySettings( NULL, 0 );
      smIsFullScreen = false;
   }

   Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );
  

   bool newWindow = false;
   if ( !winState.appWindow )
   {
      Con::printf( "Creating a new %swindow...", ( fullScreen ? "full-screen " : "" ) );
      winState.appWindow = CreateOpenGLWindow( newRes.w, newRes.h, newFullScreen, true );
      if ( !winState.appWindow )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\nFailed to create a new window!" );
         return false;
      }
      newWindow = true;
   }

   // Move the window:
   if ( !newFullScreen )
   {
      // Adjust the window rect to compensate for the window style:
      RECT windowRect;
      windowRect.left = windowRect.top = 0;
      windowRect.right = newRes.w;
      windowRect.bottom = newRes.h;

      AdjustWindowRect( &windowRect, GetWindowLong( winState.appWindow, GWL_STYLE ), false );
      U32 adjWidth = windowRect.right - windowRect.left;
      U32 adjHeight = windowRect.bottom - windowRect.top;

      // Center the window on the desktop:
      U32 xPos = 0, yPos = 0;
      
      if(adjWidth < (U32)winState.desktopClientWidth)
         xPos = ( winState.desktopClientWidth - adjWidth ) / 2;

      if(adjHeight < (U32)winState.desktopClientHeight)
         yPos = ( winState.desktopClientHeight - adjHeight ) / 2;

      test = SetWindowPos( winState.appWindow, 0, xPos, yPos, adjWidth, adjHeight, SWP_NOZORDER );
      if ( !test )
      {
         dSprintf( errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed trying to move the window to (%d,%d) and size it to %dx%d.", xPos, yPos, newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }

      AnimateWindow( winState.appWindow, 90, AW_BLEND );
   }
   else if ( !newWindow )
   {
      // Move and size the window to take up the whole screen:
      if ( !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOACTIVATE)
        || !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOZORDER) )
      {
         dSprintf( errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed to move the window to (0,0) and size it to %dx%d.", newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }
   }

   bool newDeviceContext = false;
   if ( !winState.appDC )
   {
      // Get a new device context:
      Con::printf( "Acquiring a new device context..." );
      winState.appDC = GetDC( winState.appWindow );
      if ( !winState.appDC )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\nGetDC failed to get a valid device context!" );
         return false;
      }
      newDeviceContext = true;
   }

   if ( newWindow )
   {
      // Set the pixel format of the new window:
      PIXELFORMATDESCRIPTOR pfd;
      CreatePixelFormat( &pfd, newRes.bpp, 24, 8, false );
      S32 chosenFormat = ChooseBestPixelFormat( winState.appDC, &pfd );
      if ( !chosenFormat )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\nNo valid pixel formats found!" );
         return false;
      }
      dwglDescribePixelFormat( winState.appDC, chosenFormat, sizeof( pfd ), &pfd );
      if ( !SetPixelFormat( winState.appDC, chosenFormat, &pfd ) )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\nFailed to set the pixel format!" );
         return false;
      }
      Con::printf( "Pixel format set:" );
      Con::printf( "  %d color bits, %d depth bits, %d stencil bits", pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits );
   }

   if ( !winState.hGLRC )
   {
      // Create a new rendering context:
      Con::printf( "Creating a new rendering context..." );
      winState.hGLRC = dwglCreateContext( winState.appDC );
      if ( !winState.hGLRC )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\ndwglCreateContext failed to create an OpenGL rendering context!" );
         return false;
      }

      // Make the new rendering context current:
      Con::printf( "Making the new rendering context current..." );
      if ( !dwglMakeCurrent( winState.appDC, winState.hGLRC ) )
      {
            AssertFatal( false, "OpenGLDevice::setScreenMode\ndwglMakeCurrent failed to make the rendering context current!" );
            return false;
      }

      // Just for kicks.  Seems a relatively central place to put this...
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if ( needResurrect )
      {
         // Reload the textures:
         Con::printf( "Resurrecting the texture manager..." );
         Game->textureResurrect();
      }
   }

   // Just for kicks.  Seems a relatively central place to put this...
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   if ( newDeviceContext && gGLState.suppSwapInterval )
      setVerticalSync( !Con::getBoolVariable( "$pref::Video::disableVerticalSync" ) );

   smCurrentRes = newRes;
   Platform::setWindowSize( newRes.w, newRes.h );
   char tempBuf[15];
   dSprintf( tempBuf, sizeof( tempBuf ), "%d %d %d", smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp );
   if( fullScreen )
      Con::setVariable( "$pref::Video::resolution", tempBuf );
   else
      Con::setVariable( "$pref::Video::windowedRes", tempBuf );

   if ( curtain )
      DestroyWindow( curtain );

   // Doesn't hurt to do this even it isn't necessary:
   ShowWindow( winState.appWindow, SW_SHOW );
   SetForegroundWindow( winState.appWindow );
   SetFocus( winState.appWindow );

   if (newFullScreen)
   {
      // Doesn't hurt to do this an extra time, and seems to help in some cases
      if ( !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOACTIVATE)
        || !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOZORDER) )
      {
         dSprintf( errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed to move the window to (0,0) and size it to %dx%d.", newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }
   }

    //Luma:	Clear window at first, as it is showing previous gl color buffer stuff.
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if ( repaint )
      Con::evaluate( "resetCanvas();" );

   return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::swapBuffers()
{
   dwglSwapBuffers( winState.appDC );
}


//------------------------------------------------------------------------------
const char* OpenGLDevice::getDriverInfo()
{
   // Output some driver info to the console:
   const char* vendorString   = (const char*) glGetString( GL_VENDOR );
   const char* rendererString = (const char*) glGetString( GL_RENDERER );
   const char* versionString  = (const char*) glGetString( GL_VERSION );
   const char* extensionsString = (const char*) glGetString( GL_EXTENSIONS );

   U32 bufferLen = ( vendorString ? dStrlen( vendorString ) : 0 )
                 + ( rendererString ? dStrlen( rendererString ) : 0 )
                 + ( versionString  ? dStrlen( versionString ) : 0 )
                 + ( extensionsString ? dStrlen( extensionsString ) : 0 )
                 + 4;

   char* returnString = Con::getReturnBuffer( bufferLen );
   dSprintf( returnString, bufferLen, "%s\t%s\t%s\t%s",
         ( vendorString ? vendorString : "" ),
         ( rendererString ? rendererString : "" ),
         ( versionString ? versionString : "" ),
         ( extensionsString ? extensionsString : "" ) );

   return( returnString );
}


//------------------------------------------------------------------------------
bool OpenGLDevice::getGammaCorrection(F32 &g)
{
   U16 ramp[256*3];

   if (!GetDeviceGammaRamp(winState.appDC, ramp))
      return false;

   F32 csum = 0.0;
   U32 ccount = 0;

   for (U16 i = 0; i < 256; ++i)
   {
      if (i != 0 && ramp[i] != 0 && ramp[i] != 65535)
      {
         F64 b = (F64) i/256.0;
         F64 a = (F64) ramp[i]/65535.0;
         F32 c = (F32) (mLog(a)/mLog(b));

         csum += c;
         ++ccount;
      }
   }
   g = csum/ccount;

   return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setGammaCorrection(F32 g)
{
   U16 ramp[256*3];

   for (U16 i = 0; i < 256; ++i)
      ramp[i] = (U16)((U16)mPow((F32) i/256.0f, g) * 65535.0f);
   dMemcpy(&ramp[256],ramp,256*sizeof(U16));
   dMemcpy(&ramp[512],ramp,256*sizeof(U16));

   return SetDeviceGammaRamp(winState.appDC, ramp);
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setVerticalSync( bool on )
{
   if ( !gGLState.suppSwapInterval )
      return( false );

   return( dwglSwapIntervalEXT( on ? 1 : 0 ) );
}

//------------------------------------------------------------------------------
DisplayDevice* OpenGLDevice::create()
{

   // Get Bit Depth Switching Info.
   enumerateBitDepths();

   bool result = false;
   bool fullScreenOnly = false;

   //------------------------------------------------------------------------------
   // This shouldn't happen, but just to be safe...
   //------------------------------------------------------------------------------
   if ( winState.hinstOpenGL )
      GL_Shutdown();

   //------------------------------------------------------------------------------
   // Initialize GL
   //------------------------------------------------------------------------------
   if (!GL_Init( "opengl32", "glu32" ))
      return NULL;

   //------------------------------------------------------------------------------
   // Create a test window to see if OpenGL hardware acceleration is available:
   //------------------------------------------------------------------------------
   WNDCLASS wc;
   dMemset(&wc, 0, sizeof(wc));
   wc.style         = CS_OWNDC;
   wc.lpfnWndProc   = DefWindowProc;
   wc.hInstance     = winState.appInstance;
   wc.lpszClassName = dT("OGLTest");
   RegisterClass( &wc );

   //------------------------------------------------------------------------------
   // Create the Test Window
   //------------------------------------------------------------------------------
   //MIN_RESOLUTION defined in platformWin32/platformGL.h
   HWND testWindow = CreateWindow( dT("OGLTest"),dT(""), WS_POPUP, 0, 0, MIN_RESOLUTION_X, MIN_RESOLUTION_Y, NULL, NULL, winState.appInstance, NULL );
   if ( !testWindow )
   {
      // Unregister the Window Class
      UnregisterClass( dT("OGLTest"), winState.appInstance );

      // Shutdown GL
      GL_Shutdown();

      // Return Failure
      return NULL;
   }

   //------------------------------------------------------------------------------
   // Attempt to Grab a handle to the DeviceContext of our window.
   //------------------------------------------------------------------------------
   HDC testDC = GetDC( testWindow );
   if ( !testDC )
   {
      // Destroy the Window
      DestroyWindow( testWindow );

      // Unregister the Window Class
      UnregisterClass( dT("OGLTest"), winState.appInstance );

      // Shutdown GL
      GL_Shutdown();

      // Return Failure.
      return NULL;
   }

   //------------------------------------------------------------------------------
   // Create Pixel Format ( Default 16bpp )
   //------------------------------------------------------------------------------
   PIXELFORMATDESCRIPTOR pfd;
   CreatePixelFormat( &pfd, 16, 16, 8, false );


   U32 chosenFormat = ChooseBestPixelFormat( testDC, &pfd );
   if ( chosenFormat != 0 )
   {
      dwglDescribePixelFormat( testDC, chosenFormat, sizeof( pfd ), &pfd );

      result = !( pfd.dwFlags & PFD_GENERIC_FORMAT );

      if ( result && winState.desktopBitsPixel < 16 && !smCanDo32Bit)
      {
         // If Windows 95 cannot switch bit depth, it should only attempt 16-bit cards
         // with a 16-bit desktop

         // See if we can get a 32-bit pixel format:
         PIXELFORMATDESCRIPTOR pfd;

         CreatePixelFormat( &pfd, 32, 24, 8, false );
         S32 chosenFormat = ChooseBestPixelFormat( testDC, &pfd );
         if ( chosenFormat != 0 )
         {
            dwglDescribePixelFormat( winState.appDC, chosenFormat, sizeof( pfd ), &pfd );

            if (pfd.cColorBits == 16)
            {
               Platform::AlertOK("Requires 16-Bit Desktop",
                  "You must run in 16-bit color to run a Torque game.\nPlease quit the game, set your desktop color depth to 16-bit\nand then restart the application.");

               result = false;
            }
         }
      }
   }
   else if ( winState.desktopBitsPixel < 16 && smCanSwitchBitDepth )
   {
      // Try again after changing the display to 16-bit:
      ReleaseDC( testWindow, testDC );
      DestroyWindow( testWindow );

      DEVMODE devMode;
      dMemset( &devMode, 0, sizeof( devMode ) );
      devMode.dmSize       = sizeof( devMode );
      devMode.dmBitsPerPel = 16;
      devMode.dmFields     = DM_BITSPERPEL;

      U32 test = ChangeDisplaySettings( &devMode, 0 );
      if ( test == DISP_CHANGE_SUCCESSFUL )
      {
    //MIN_RESOLUTION defined in platformWin32/platformGL.h
         testWindow = CreateWindow( dT("OGLTest"), dT(""), WS_OVERLAPPED | WS_CAPTION, 0, 0, MIN_RESOLUTION_X, MIN_RESOLUTION_Y, NULL, NULL, winState.appInstance, NULL );
         if ( testWindow )
         {
            testDC = GetDC( testWindow );
            if ( testDC )
            {
               CreatePixelFormat( &pfd, 16, 16, 8, false );
               chosenFormat = ChooseBestPixelFormat( testDC, &pfd );
               if ( chosenFormat != 0 )
               {
                  dwglDescribePixelFormat( testDC, chosenFormat, sizeof( pfd ), &pfd );

                  result = !( pfd.dwFlags & PFD_GENERIC_FORMAT );
                  if ( result )
                     fullScreenOnly = true;
               }
            }
         }
      }
      ChangeDisplaySettings( NULL, 0 );
   }
   //------------------------------------------------------------------------------
   // Can't do even 16 bit, alert user they need to upgrade.
   //------------------------------------------------------------------------------
   else if ( winState.desktopBitsPixel < 16 && !smCanSwitchBitDepth )
   {
      Platform::AlertOK("Requires 16-Bit Desktop", "You must run in 16-bit color to run a Torque game.\nPlease quit the game, set your desktop color depth to 16-bit\nand then restart the application.");
   }

   ReleaseDC( testWindow, testDC );
   DestroyWindow( testWindow );

   UnregisterClass( dT("OGLTest"), winState.appInstance );

   GL_Shutdown();

   if ( result )
   {
      OpenGLDevice* newOGLDevice = new OpenGLDevice();
      if ( newOGLDevice )
      {
         newOGLDevice->mFullScreenOnly = fullScreenOnly;
         return (DisplayDevice*) newOGLDevice;
      }
      else
         return NULL;
   }
   else
      return NULL;
}
