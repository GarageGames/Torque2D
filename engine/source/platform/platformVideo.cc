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

#include "platform/platformVideo.h"
#include "gui/guiCanvas.h"
#include "console/console.h"
#include "game/gameInterface.h"

extern void GameDeactivate( bool noRender );
extern void GameReactivate();

// Static class data:
Vector<DisplayDevice*>  Video::smDeviceList;
DisplayDevice*          Video::smCurrentDevice;
bool					Video::smCritical = false;
bool					Video::smNeedResurrect = false;

Resolution  DisplayDevice::smCurrentRes;
bool        DisplayDevice::smIsFullScreen;


ConsoleFunctionGroupBegin(Video, "Video control functions.");

//--------------------------------------------------------------------------
ConsoleFunction( setDisplayDevice, bool, 2, 6, "( deviceName [, width [ , height [, bpp [, fullScreen ]]]] ) Use the setDisplayDevice function to select a display device and to set the initial width, height and bits-per-pixel (bpp) setting, as well as whether the application is windowed or in fullScreen.\n"
                                                                "If no resolution information is specified, the first legal resolution on this device's resolution list will be used. Furthermore, for each optional argument if the subsequent arguments are not specified, the first matching case will be used. Lastly, if the application is not told to display in full screen, but the device only supports windowed, the application will be forced into windowed mode.\n"
                                                                "@param deviceName A supported display device name.\n"
                                                                "@param width Resolution width in pixels.\n"
                                                                "@param height Resolution height in pixels.\n"
                                                                "@param bpp Pixel resolution in bits-per-pixel (16 or 32).\n"
                                                                "@param fullScreen A boolean value. If set to true, the application displays in full- screen mode, otherwise it will attempt to display in windowed mode.\n"
                                                                "@return Returns true on success, false otherwise.\n"
                                                                "@sa getDesktopResolution, getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setRes, setScreenMode, switchBitDepth")
{
    Resolution currentRes = Video::getResolution();

    U32 width = ( argc > 2 ) ? dAtoi( argv[2] ) : currentRes.w;
    U32 height =  ( argc > 3 ) ? dAtoi( argv[3] ) : currentRes.h;
    U32 bpp = ( argc > 4 ) ? dAtoi( argv[4] ) : currentRes.bpp;
    bool fullScreen = ( argc > 5 ) ? dAtob( argv[5] ) : Video::isFullScreen();

   return( Video::setDevice( argv[1], width, height, bpp, fullScreen ) );
}


//--------------------------------------------------------------------------
ConsoleFunction( setScreenMode, bool, 5, 5, "( width , height , bpp , fullScreen ) Use the setScreenMode function to set the screen to the specified width, height, and bits-per-pixel (bpp). Additionally, if fullScreen is set to true the engine will attempt to display the application in full-screen mode, otherwise it will attempt to used windowed mode.\n"
                                                                "@param width Resolution width in pixels.\n"
                                                                "@param height Resolution height in pixels.\n"
                                                                "@param bpp Pixel resolution in bits-per-pixel (16 or 32).\n"
                                                                "@param fullScreen A boolean value. If set to true, the application displays in full- screen mode, otherwise it will attempt to display in windowed mode.\n"
                                                                "@return Returns true if successful, otherwise false.\n"
                                                                "@sa getDesktopResolution, getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setDisplayDevice, setRes, switchBitDepth")
{
   return( Video::setScreenMode( dAtoi( argv[1] ), dAtoi( argv[2] ), dAtoi( argv[3] ), dAtob( argv[4] ) ) );
}


//------------------------------------------------------------------------------
ConsoleFunction( toggleFullScreen, bool, 1, 1, "() Use the toggleFullScreen function to switch from full-screen mode to windowed, or vice versa.\n"
                                                                "@return Returns true on success, false otherwise")
{
   return( Video::toggleFullScreen() );
}


//------------------------------------------------------------------------------
ConsoleFunction( isFullScreen, bool, 1, 1, "() Use the isFullScreen function to determine if the current application is displayed in full-screen mode.\n"
                                                                "@return Returns true if the engine is currently displaying full-screen, otherwise returns false")
{
   return( Video::isFullScreen() );
}


//--------------------------------------------------------------------------
ConsoleFunction( switchBitDepth, bool, 1, 1, "() Use the switchBitDepth function to toggle the bits-per-pixel (bpp) pixel resolution between 16 and 32.\n"
                                                                "@return Returns true on success, false otherwise.\n"
                                                                "@sa getDesktopResolution, getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setDisplayDevice, setRes")
{
   if ( !Video::isFullScreen() )
   {
      Con::warnf( ConsoleLogEntry::General, "Can only switch bit depth in full-screen mode!" );
      return( false );
   }

   Resolution res = Video::getResolution();
   return( Video::setResolution( res.w, res.h, ( res.bpp == 16 ? 32 : 16 ) ) );
}


//--------------------------------------------------------------------------
ConsoleFunction( prevResolution, bool, 1, 1, "() Use the prevResolution function to switch to the previous valid (lower) resolution for the current display device.\n"
                                                                "@return Returns true if switch was successful, false otherwise.\n"
                                                                "@sa getDesktopResolution, nextResolution, getResolutionList, setRes, setScreenMode, switchBitDepth")
{
   return( Video::prevRes() );
}


//--------------------------------------------------------------------------
ConsoleFunction( nextResolution, bool, 1, 1, "() Use the nextResolution function to switch to the next valid (higher) resolution for the current display device.\n"
                                                                "@return Returns true if switch was successful, false otherwise.\n"
                                                                "@sa getDesktopResolution, prevResolution, getResolutionList, setRes, setScreenMode, switchBitDepth")
{
   return( Video::nextRes() );
}


//--------------------------------------------------------------------------
ConsoleFunction( getRes, const char*, 1, 1, "Get the width, height, and bitdepth of the screen."
                "@return A string formatted as \"<width> <height> <bitdepth>\"")
{
   static char resBuf[16];
   Resolution res = Video::getResolution();
   dSprintf( resBuf, sizeof(resBuf), "%d %d %d", res.w, res.h, res.bpp );
   return( resBuf );
}

//--------------------------------------------------------------------------
ConsoleFunction( setRes, bool, 3, 4, "( width , height , bpp ) Use the setRes function to set the screen to the specified width, height, and bits-per-pixel (bpp).\n"
                                                                "@param width Resolution width in pixels.\n"
                                                                "@param height Resolution height in pixels.\n"
                                                                "@param bpp Pixel resolution in bits-per-pixel (16 or 32).\n"
                                                                "@return Returns true if successful, otherwise false.\n"
                                                                "@sa getDesktopResolution, getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setDisplayDevice, setScreenMode, switchBitDepth")
{
   U32 width = dAtoi( argv[1] );
   U32 height = dAtoi( argv[2] );
   U32 bpp = 0;
   if ( argc == 4 )
   {
      bpp = dAtoi( argv[3] );
      if ( bpp != 16 && bpp != 32 )
         bpp = 0;
   }

   return( Video::setResolution( width, height, bpp ) );
}

//------------------------------------------------------------------------------
ConsoleFunction( getDesktopResolution, const char*, 1, 1, "() Use the getDesktopResolution function to determine the current resolution of the desktop (not the application).\n"
                                                                "To get the current resolution of a windowed display of the torque game engine, simply examine the global variable '$pref::Video::resolution'.\n"
                                                                "@return Returns a string containing the current desktop resolution, including the width height and the current bits per pixel.\n"
                                                                "@sa getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setDisplayDevice, setRes, setScreenMode, switchBitDepth")
{
   static char resBuf[16];
   Resolution res = Video::getDesktopResolution();
   dSprintf( resBuf, sizeof(resBuf), "%d %d %d", res.w, res.h, res.bpp );
   return( resBuf );
}


//------------------------------------------------------------------------------
ConsoleFunction( getDisplayDeviceList, const char*, 1, 1, "() Use the getDisplayDeviceList function to get a list of valid display devices.\n"
                                                                "@return Returns a tab separated list of valid display devices.\n"
                                                                "@sa getDesktopResolution, getResolutionList, setRes, setScreenMode, switchBitDepth")
{
    return( Video::getDeviceList() );	
}


//------------------------------------------------------------------------------
ConsoleFunction( getResolutionList, const char*, 2, 2, "( devicename ) Use the getResolutionList function to get a semicolon separated list of legal resolutions for a specified device.\n"
                                                                "Resolutions are always in the form: width height bpp, where width and height are in pixels and bpp is bits-per-pixel.\n"
                                                                "@param deviceName A string containing a supported display device.\n"
                                                                "@return Returns a tab separated list of valid display resolutions for devicename.\n"
                                                                "@sa getDesktopResolution, getDisplayDeviceList, setRes, setScreenMode, switchBitDepth")
{
    DisplayDevice* device = Video::getDevice( argv[1] );
    if ( !device )
    {
        Con::warnf( ConsoleLogEntry::General, "\"%s\" display device not found!", argv[1] );
        return( NULL );
    }
    
    return( device->getResolutionList() );		
}

//------------------------------------------------------------------------------
ConsoleFunction( getVideoDriverInfo, const char*, 1, 1, "() Use the getVideoDriverInfo function to dump information on the video driver to the console.\n"
                                                                "@return No return value")
{
    return( Video::getDriverInfo() );
}

//------------------------------------------------------------------------------
ConsoleFunction( isDeviceFullScreenOnly, bool, 2, 2, "( devicename ) Use the isDeviceFullScreenOnly function to determine if the device specified in devicename is for full screen display only, or whether it supports windowed mode too.\n"
                                                                "@param deviceName A string containing a supported display device.\n"
                                                                "@return Returns true if the device can only display full scree, false otherwise.\n"
                                                                "@sa getResolutionList")
{
    DisplayDevice* device = Video::getDevice( argv[1] );
    if ( !device )
    {
        Con::warnf( ConsoleLogEntry::General, "\"%s\" display device not found!", argv[1] );
        return( false );
    }
    
    return( device->isFullScreenOnly() );		
}


//------------------------------------------------------------------------------
static F32 sgOriginalGamma = -1.0;
static F32 sgGammaCorrection = 0.0;

ConsoleFunction(videoSetGammaCorrection, void, 2, 2, "( gamma ) Use the videoSetGammaCorrection function to adjust the gamma for the video card.\n"
                                                                "The card will revert to it's default gamma setting as long as the application closes normally\n"
                                                                "@param gamma A floating-point value between 0.0 and 1.0.\n"
                                                                "@return No return value.")
{
   F32 g = mClampF(dAtof(argv[1]),0.0,1.0);
   F32 d = -(g - 0.5f);

   if (d != sgGammaCorrection &&
     (sgOriginalGamma != -1.0f || Video::getGammaCorrection(sgOriginalGamma)))
      Video::setGammaCorrection(sgOriginalGamma+d);
    sgGammaCorrection = d;
}

//------------------------------------------------------------------------------
void Video::init()
{
    Con::printSeparator();
    
    Con::printf("Video initialization:");
    
    destroy();
}

//------------------------------------------------------------------------------
void Video::destroy()
{
   if ( smCurrentDevice )
   {
      smCritical = true;
      smCurrentDevice->shutdown();
      smCritical = false;
   }

   smCurrentDevice = NULL;

   for ( U32 i = 0; i < (U32)smDeviceList.size(); i++ )
      delete smDeviceList[i];

   smDeviceList.clear();
}


//------------------------------------------------------------------------------
bool Video::installDevice( DisplayDevice *dev )
{
   if ( dev )
   {
      smDeviceList.push_back( dev );
      return true;
   }
   return false;
}


//------------------------------------------------------------------------------
bool Video::setDevice( const char *renderName, U32 width, U32 height, U32 bpp, bool fullScreen )
{
   S32 deviceIndex = NO_DEVICE;
    S32 iOpenGL = -1;
    S32 iD3D = -1;

   bool bOpenglRender   = true; //(bool)(dStricmp(renderName,"OpenGL") == 0);
   bool bD3DRender      = false; //(bool)(dStricmp(renderName,"D3D") == 0);
   bool bAllowD3D       = false; //Con::getBoolVariable("$pref::Video::allowD3D");
   bool bAllowOpengl    = true; //Con::getBoolVariable("$pref::Video::allowOpenGL");


   for ( S32 i = 0; i < smDeviceList.size(); i++ )
   {
      if ( dStrcmp( smDeviceList[i]->mDeviceName, renderName ) == 0 )
         deviceIndex = i;

        if ( dStrcmp( smDeviceList[i]->mDeviceName, "OpenGL" ) == 0 )
            iOpenGL = i;
        if ( dStrcmp( smDeviceList[i]->mDeviceName, "D3D" ) == 0 )
            iD3D = i;
   }

   if ( deviceIndex == NO_DEVICE )
   {
      Con::warnf( ConsoleLogEntry::General, "\"%s\" display device not found!", renderName );
      return false;
   }

   // Change the display device:
   if ( smDeviceList[deviceIndex] == NULL )
      return false;


   if (smCurrentDevice && smCurrentDevice != smDeviceList[deviceIndex])
   {
      Con::printf( "Deactivating the previous display device..." );
      Game->textureKill();
      smNeedResurrect = true;
      smCurrentDevice->shutdown();
   }
   if (iOpenGL != -1 && !bAllowOpengl )
   {
      // change to D3D, delete OpenGL in the recursive call
      if ( bOpenglRender )
      {
         U32 w, h, d;

         if( fullScreen )
            dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
         else
            dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);

         return setDevice("D3D",w,h,d,fullScreen);
      }
      else
      {
         delete smDeviceList[iOpenGL];
         smDeviceList.erase(iOpenGL);
      }
   }
   else if (iD3D != -1 && !bAllowD3D )
   {
      // change to OpenGL, delete D3D in the recursive call
      if ( bD3DRender )
      {
         U32 w, h, d;
         if( fullScreen )
            dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
         else
            dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);

         return setDevice("OpenGL",w,h,d,fullScreen);
      }
      else
      {
         delete smDeviceList[iD3D];
         smDeviceList.erase(iD3D);
      }
   }
   else if (iD3D != -1 && bOpenglRender &&
      !Con::getBoolVariable("$pref::Video::preferOpenGL") &&
      !Con::getBoolVariable("$pref::Video::appliedPref"))
   {
      U32 w, h, d;
      if( fullScreen )
         dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
      else
         dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);
      Con::setBoolVariable("$pref::Video::appliedPref", true);

      return setDevice("D3D",w,h,d,fullScreen);
   }
   else
      Con::setBoolVariable("$pref::Video::appliedPref", true);

   Con::printf( "Activating the %s display device...", renderName );
   smCurrentDevice = smDeviceList[deviceIndex];

   smCritical = true;
   bool result = smCurrentDevice->activate( width, height, bpp, fullScreen );
   smCritical = false;

   if ( result )
   {
      if (smNeedResurrect)
      {
         Game->textureResurrect();
         smNeedResurrect = false;
      }
      if (sgOriginalGamma != -1.0 || Video::getGammaCorrection(sgOriginalGamma))
         Video::setGammaCorrection(sgOriginalGamma + sgGammaCorrection);
      Con::evaluate("resetCanvas();");
   }

    // Show Maximum Texture Size reported by the graphics hardware.
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    if ( maxTextureSize > 0 )
        Con::printf("Max Texture Size reported as: %d", maxTextureSize);
    else
        Con::warnf("Max Texture Size reported as: %d !", maxTextureSize);

   // The video mode activate may have failed above, return that status
   return( result );
}

extern bool retinaEnabled;

//------------------------------------------------------------------------------
bool Video::setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen )
{
   if ( smCurrentDevice )
   {
       //MIN_RESOLUTION defined in platformWin32/platformGL.h
#ifdef TORQUE_OS_IOS
      if(width == 0)
         width = IOS_DEFAULT_RESOLUTION_X;
      if(height == 0)
         height = IOS_DEFAULT_RESOLUTION_Y;
      if(bpp == 0)
         bpp = IOS_DEFAULT_RESOLUTION_BIT_DEPTH;
      
      //if(retinaEnabled)
      //{
      //    width *=2;
      //    height *=2;
      //}
       
#else
      if(width == 0)
         width = MIN_RESOLUTION_X;
      if(height == 0)
         height = MIN_RESOLUTION_Y;
      if(bpp == 0)
         bpp = MIN_RESOLUTION_BIT_DEPTH;
#endif// TORQUE_OS_IOS
      
      smCritical = true;
      bool result = smCurrentDevice->setScreenMode( width, height, bpp, fullScreen );		
    smCritical = false;

       return( result );
   }

   return( false );	
}


//------------------------------------------------------------------------------
void Video::deactivate( bool force )
{
   if ( smCritical ) return;
   
   bool doDeactivate = force ? true : DisplayDevice::isFullScreen();

   Game->gameDeactivate( doDeactivate );
   if ( smCurrentDevice && doDeactivate )
   {
       smCritical = true;

      Game->textureKill();
      smCurrentDevice->shutdown();

      Platform::minimizeWindow();
      smCritical = false;
   }
}

//------------------------------------------------------------------------------
void Video::reactivate( bool force )
{
   if ( smCritical ) return;
   
   bool doReactivate = force ? true : DisplayDevice::isFullScreen();

   if ( smCurrentDevice && doReactivate )
   {
       Resolution res = DisplayDevice::getResolution();

      smCritical = true;
      smCurrentDevice->activate(res.w,res.h,res.bpp,DisplayDevice::isFullScreen());
       Game->textureResurrect();

       smCritical = false;
      if (sgOriginalGamma != -1.0)
        Video::setGammaCorrection(sgOriginalGamma + sgGammaCorrection);
   }
   Game->gameReactivate();
}


//------------------------------------------------------------------------------
bool Video::setResolution( U32 width, U32 height, U32 bpp )
{
   if ( smCurrentDevice )
   {
      if ( bpp == 0 )
         bpp = DisplayDevice::getResolution().bpp;

      smCritical = true;
      bool result = smCurrentDevice->setResolution( width, height, bpp );
      smCritical = false;

      return( result );
   }
   return( false );	
}


//------------------------------------------------------------------------------
bool Video::toggleFullScreen()
{
   if ( smCurrentDevice )
   {
      smCritical = true;
      bool result = smCurrentDevice->toggleFullScreen();
      smCritical = false;

      return( result );
   }
   return( false );	
}


//------------------------------------------------------------------------------
DisplayDevice* Video::getDevice( const char* renderName )
{
   for ( S32 i = 0; i < smDeviceList.size(); i++ )
   {
      if ( dStrcmp( smDeviceList[i]->mDeviceName, renderName ) == 0 )
            return( smDeviceList[i] );
   }

    return( NULL );	
}


//------------------------------------------------------------------------------
bool Video::prevRes()
{
   if ( smCurrentDevice )
   {
      smCritical = true;
      bool result = smCurrentDevice->prevRes();
      smCritical = false;

      return( result );
   }
   return( false );	
}


//------------------------------------------------------------------------------
bool Video::nextRes()
{
   if ( smCurrentDevice )
   {
      smCritical = true;
      bool result = smCurrentDevice->nextRes();
      smCritical = false;

      return( result );
   }
   return( false );	
}


//------------------------------------------------------------------------------
Resolution Video::getResolution()
{
   return DisplayDevice::getResolution();
}


//------------------------------------------------------------------------------
const char* Video::getDeviceList()
{
    U32 deviceCount = smDeviceList.size();
    if ( deviceCount > 0 ) // It better be...
    {		
        U32 strLen = 0, i;
        for ( i = 0; i < deviceCount; i++ )
            strLen += ( dStrlen( smDeviceList[i]->mDeviceName ) + 1 );

        char* returnString = Con::getReturnBuffer( strLen );
        dStrcpy( returnString, smDeviceList[0]->mDeviceName );
        for ( i = 1; i < deviceCount; i++ )
        {
            dStrcat( returnString, "\t" );
            dStrcat( returnString, smDeviceList[i]->mDeviceName );
        }

        return( returnString );				
    }

    return( NULL );
}


//------------------------------------------------------------------------------
const char* Video::getResolutionList()
{
   if ( smCurrentDevice )
      return smCurrentDevice->getResolutionList();
   else
      return NULL;
}


//------------------------------------------------------------------------------
const char* Video::getDriverInfo()
{
   if ( smCurrentDevice )
      return smCurrentDevice->getDriverInfo();
   else
      return NULL;
}


//------------------------------------------------------------------------------
bool Video::isFullScreen()
{
   return DisplayDevice::isFullScreen();
}


//------------------------------------------------------------------------------
void Video::swapBuffers()
{
   if ( smCurrentDevice )
      smCurrentDevice->swapBuffers();
}


//------------------------------------------------------------------------------
bool Video::getGammaCorrection(F32 &g)
{
   if (smCurrentDevice)
      return smCurrentDevice->getGammaCorrection(g);

   return false;	
}


//------------------------------------------------------------------------------
bool Video::setGammaCorrection(F32 g)
{
   if (smCurrentDevice)
      return smCurrentDevice->setGammaCorrection(g);

   return false;	
}

//------------------------------------------------------------------------------
bool Video::setVerticalSync( bool on )
{
   if ( smCurrentDevice )
      return( smCurrentDevice->setVerticalSync( on ) );

   return( false );
}

ConsoleFunction( setVerticalSync, bool, 2, 2, "( enable ) Use the setVerticalSync function to force the framerate to sync up with the vertical refresh rate.\n"
                                                                "This is used to reduce excessive swapping/rendering. There is generally no purpose in rendering any faster than the monitor will support. Those extra 'ergs' can be used for something else\n"
                                                                "@param enable A boolean value. If set to true, the engine will only swap front and back buffers on or before a vertical refresh pass.\n"
                                                                "@return Returns true on success, false otherwise.")
{
   return( Video::setVerticalSync( dAtob( argv[1] ) ) );
}

ConsoleFunction( minimizeWindow, void, 1, 1, "minimizeWindow() - Minimize the game window" )
{
    Platform::minimizeWindow();
}

ConsoleFunction( restoreWindow, void, 1, 1, "restoreWindow() - Restore the game window" )
{
   Platform::restoreWindow();
}

ConsoleFunctionGroupEnd(Video);

//------------------------------------------------------------------------------
DisplayDevice::DisplayDevice()
{
   mDeviceName = NULL;
}


//------------------------------------------------------------------------------
void DisplayDevice::init()
{
    smCurrentRes = Resolution( 0, 0, 0 );
    smIsFullScreen = false;
}


//------------------------------------------------------------------------------
bool DisplayDevice::prevRes()
{
   U32 resIndex;
   for ( resIndex = mResolutionList.size() - 1; resIndex > 0; resIndex-- )
   {
      if ( mResolutionList[resIndex].bpp == smCurrentRes.bpp
        && mResolutionList[resIndex].w <= smCurrentRes.w
        && mResolutionList[resIndex].h != smCurrentRes.h )
         break;
   }

   if ( mResolutionList[resIndex].bpp == smCurrentRes.bpp )
      return( Video::setResolution( mResolutionList[resIndex].w, mResolutionList[resIndex].h, mResolutionList[resIndex].bpp ) );

    return( false );	
}


//------------------------------------------------------------------------------
bool DisplayDevice::nextRes()
{
   U32 resIndex;
   for ( resIndex = 0; resIndex < (U32)mResolutionList.size() - 1; resIndex++ )
   {
      if ( mResolutionList[resIndex].bpp == smCurrentRes.bpp
        && mResolutionList[resIndex].w >= smCurrentRes.w
        && mResolutionList[resIndex].h != smCurrentRes.h )
         break;
   }

   if ( mResolutionList[resIndex].bpp == smCurrentRes.bpp )
      return( Video::setResolution( mResolutionList[resIndex].w, mResolutionList[resIndex].h, mResolutionList[resIndex].bpp ) );

    return( false );	
}


//------------------------------------------------------------------------------
// This function returns a string containing all of the available resolutions for this device
// in the format "<bit depth> <width> <height>", separated by tabs.
//
const char* DisplayDevice::getResolutionList()
{
   if (Con::getBoolVariable("$pref::Video::clipHigh", false))
        for (S32 i = mResolutionList.size()-1; i >= 0; --i)
            if (mResolutionList[i].w > 1152 || mResolutionList[i].h > 864)
                mResolutionList.erase(i);

    if (Con::getBoolVariable("$pref::Video::only16", false))
        for (S32 i = mResolutionList.size()-1; i >= 0; --i)
            if (mResolutionList[i].bpp == 32)
                mResolutionList.erase(i);

   U32 resCount = mResolutionList.size();
   if ( resCount > 0 )
   {
      char* tempBuffer = new char[resCount * 15];
      tempBuffer[0] = 0;
      for ( U32 i = 0; i < resCount; i++ )
      {
         char newString[15];
         dSprintf( newString, sizeof( newString ), "%d %d %d\t", mResolutionList[i].w, mResolutionList[i].h, mResolutionList[i].bpp );
         dStrcat( tempBuffer, newString );
      }
      tempBuffer[dStrlen( tempBuffer ) - 1] = 0;

      char* returnString = Con::getReturnBuffer( dStrlen( tempBuffer ) + 1 );
      dStrcpy( returnString, tempBuffer );
      delete [] tempBuffer;

      return returnString;
   }

   return NULL;
}
