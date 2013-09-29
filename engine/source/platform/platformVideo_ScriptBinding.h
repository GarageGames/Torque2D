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

ConsoleFunctionGroupBegin(Video, "Video control functions.");

/*! @defgroup VideoFunctions Video
	@ingroup TorqueScriptFunctions
	@{
*/

//--------------------------------------------------------------------------
/*! Use the setDisplayDevice function to select a display device and to set the initial width, height and bits-per-pixel (bpp) setting, as well as whether the application is windowed or in fullScreen.
    If no resolution information is specified, the first legal resolution on this device's resolution list will be used. Furthermore, for each optional argument if the subsequent arguments are not specified, the first matching case will be used. Lastly, if the application is not told to display in full screen, but the device only supports windowed, the application will be forced into windowed mode.
    @param deviceName A supported display device name.
    @param width Resolution width in pixels.
    @param height Resolution height in pixels.
    @param bpp Pixel resolution in bits-per-pixel (16 or 32).
    @param fullScreen A boolean value. If set to true, the application displays in full- screen mode, otherwise it will attempt to display in windowed mode.
    @return Returns true on success, false otherwise.
    @sa getDesktopResolution, getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setRes, setScreenMode, switchBitDepth
*/
ConsoleFunctionWithDocs( setDisplayDevice, ConsoleBool, 2, 6, ( deviceName, [width]?, [height]?, [bpp]?, [fullScreen]? ))
{
    Resolution currentRes = Video::getResolution();

    U32 width = ( argc > 2 ) ? dAtoi( argv[2] ) : currentRes.w;
    U32 height =  ( argc > 3 ) ? dAtoi( argv[3] ) : currentRes.h;
    U32 bpp = ( argc > 4 ) ? dAtoi( argv[4] ) : currentRes.bpp;
    bool fullScreen = ( argc > 5 ) ? dAtob( argv[5] ) : Video::isFullScreen();

   return( Video::setDevice( argv[1], width, height, bpp, fullScreen ) );
}


//--------------------------------------------------------------------------
/*! Use the setScreenMode function to set the screen to the specified width, height, and bits-per-pixel (bpp). Additionally, if fullScreen is set to true the engine will attempt to display the application in full-screen mode, otherwise it will attempt to used windowed mode.
    @param width Resolution width in pixels.
    @param height Resolution height in pixels.
    @param bpp Pixel resolution in bits-per-pixel (16 or 32).
    @param fullScreen A boolean value. If set to true, the application displays in full- screen mode, otherwise it will attempt to display in windowed mode.
    @return Returns true if successful, otherwise false.
    @sa getDesktopResolution, getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setDisplayDevice, setRes, switchBitDepth
*/
ConsoleFunctionWithDocs( setScreenMode, ConsoleBool, 5, 5, ( width , height , bpp , fullScreen ))
{
   return( Video::setScreenMode( dAtoi( argv[1] ), dAtoi( argv[2] ), dAtoi( argv[3] ), dAtob( argv[4] ) ) );
}


//------------------------------------------------------------------------------
/*! Use the toggleFullScreen function to switch from full-screen mode to windowed, or vice versa.
    @return Returns true on success, false otherwise
*/
ConsoleFunctionWithDocs( toggleFullScreen, ConsoleBool, 1, 1, ())
{
   return( Video::toggleFullScreen() );
}


//------------------------------------------------------------------------------
/*! Use the isFullScreen function to determine if the current application is displayed in full-screen mode.
    @return Returns true if the engine is currently displaying full-screen, otherwise returns false
*/
ConsoleFunctionWithDocs( isFullScreen, ConsoleBool, 1, 1, ())
{
   return( Video::isFullScreen() );
}


//--------------------------------------------------------------------------
/*! Use the switchBitDepth function to toggle the bits-per-pixel (bpp) pixel resolution between 16 and 32.
    @return Returns true on success, false otherwise.
    @sa getDesktopResolution, getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setDisplayDevice, setRes
*/
ConsoleFunctionWithDocs( switchBitDepth, ConsoleBool, 1, 1, ())
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
/*! Use the prevResolution function to switch to the previous valid (lower) resolution for the current display device.
    @return Returns true if switch was successful, false otherwise.
    @sa getDesktopResolution, nextResolution, getResolutionList, setRes, setScreenMode, switchBitDepth
*/
ConsoleFunctionWithDocs( prevResolution, ConsoleBool, 1, 1, ())
{
   return( Video::prevRes() );
}


//--------------------------------------------------------------------------
/*! Use the nextResolution function to switch to the next valid (higher) resolution for the current display device.
    @return Returns true if switch was successful, false otherwise.
    @sa getDesktopResolution, prevResolution, getResolutionList, setRes, setScreenMode, switchBitDepth
*/
ConsoleFunctionWithDocs( nextResolution, ConsoleBool, 1, 1, ())
{
   return( Video::nextRes() );
}


//--------------------------------------------------------------------------
/*! Get the width, height, and bitdepth of the screen.
    @return A string formatted as \<width> <height> <bitdepth>\
*/
ConsoleFunctionWithDocs( getRes, ConsoleString, 1, 1, ())
{
   static char resBuf[16];
   Resolution res = Video::getResolution();
   dSprintf( resBuf, sizeof(resBuf), "%d %d %d", res.w, res.h, res.bpp );
   return( resBuf );
}

//--------------------------------------------------------------------------
/*! Use the setRes function to set the screen to the specified width, height, and bits-per-pixel (bpp).
    @param width Resolution width in pixels.
    @param height Resolution height in pixels.
    @param bpp Pixel resolution in bits-per-pixel (16 or 32).
    @return Returns true if successful, otherwise false.
    @sa getDesktopResolution, getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setDisplayDevice, setScreenMode, switchBitDepth
*/
ConsoleFunctionWithDocs( setRes, ConsoleBool, 3, 4, ( width , height , bpp ))
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
/*! Use the getDesktopResolution function to determine the current resolution of the desktop (not the application).
    To get the current resolution of a windowed display of the torque game engine, simply examine the global variable '$pref::Video::resolution'.
    @return Returns a string containing the current desktop resolution, including the width height and the current bits per pixel.
    @sa getDisplayDeviceList, getResolutionList, nextResolution, prevResolution, setDisplayDevice, setRes, setScreenMode, switchBitDepth
*/
ConsoleFunctionWithDocs( getDesktopResolution, ConsoleString, 1, 1, ())
{
   static char resBuf[16];
   Resolution res = Video::getDesktopResolution();
   dSprintf( resBuf, sizeof(resBuf), "%d %d %d", res.w, res.h, res.bpp );
   return( resBuf );
}


//------------------------------------------------------------------------------
/*! Use the getDisplayDeviceList function to get a list of valid display devices.
    @return Returns a tab separated list of valid display devices.
    @sa getDesktopResolution, getResolutionList, setRes, setScreenMode, switchBitDepth
*/
ConsoleFunctionWithDocs( getDisplayDeviceList, ConsoleString, 1, 1, ())
{
    return( Video::getDeviceList() );	
}


//------------------------------------------------------------------------------
/*! Use the getResolutionList function to get a semicolon separated list of legal resolutions for a specified device.
    Resolutions are always in the form: width height bpp, where width and height are in pixels and bpp is bits-per-pixel.
    @param deviceName A string containing a supported display device.
    @return Returns a tab separated list of valid display resolutions for devicename.
    @sa getDesktopResolution, getDisplayDeviceList, setRes, setScreenMode, switchBitDepth
*/
ConsoleFunctionWithDocs( getResolutionList, ConsoleString, 2, 2, ( devicename ))
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
/*! Use the getVideoDriverInfo function to dump information on the video driver to the console.
    @return No return value
*/
ConsoleFunctionWithDocs( getVideoDriverInfo, ConsoleString, 1, 1, ())
{
    return( Video::getDriverInfo() );
}

//------------------------------------------------------------------------------
/*! Use the isDeviceFullScreenOnly function to determine if the device specified in devicename is for full screen display only, or whether it supports windowed mode too.
    @param deviceName A string containing a supported display device.
    @return Returns true if the device can only display full scree, false otherwise.
    @sa getResolutionList
*/
ConsoleFunctionWithDocs( isDeviceFullScreenOnly, ConsoleBool, 2, 2, ( devicename ))
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

/*! Use the videoSetGammaCorrection function to adjust the gamma for the video card.
    The card will revert to it's default gamma setting as long as the application closes normally
    @param gamma A floating-point value between 0.0 and 1.0.
    @return No return value.
*/
ConsoleFunctionWithDocs(videoSetGammaCorrection, ConsoleVoid, 2, 2, ( gamma ))
{
   F32 g = mClampF(dAtof(argv[1]),0.0,1.0);
   F32 d = -(g - 0.5f);

   if (d != sgGammaCorrection &&
     (sgOriginalGamma != -1.0f || Video::getGammaCorrection(sgOriginalGamma)))
      Video::setGammaCorrection(sgOriginalGamma+d);
    sgGammaCorrection = d;
}

/*! Use the setVerticalSync function to force the framerate to sync up with the vertical refresh rate.
    This is used to reduce excessive swapping/rendering. There is generally no purpose in rendering any faster than the monitor will support. Those extra 'ergs' can be used for something else
    @param enable A boolean value. If set to true, the engine will only swap front and back buffers on or before a vertical refresh pass.
    @return Returns true on success, false otherwise.
*/
ConsoleFunctionWithDocs( setVerticalSync, ConsoleBool, 2, 2, ( enable ))
{
   return( Video::setVerticalSync( dAtob( argv[1] ) ) );
}

/*! Minimize the game window
*/
ConsoleFunctionWithDocs( minimizeWindow, ConsoleVoid, 1, 1, ())
{
    Platform::minimizeWindow();
}

/*! Restore the game window
*/
ConsoleFunctionWithDocs( restoreWindow, ConsoleVoid, 1, 1, ())
{
   Platform::restoreWindow();
}

ConsoleFunctionGroupEnd(Video)

/*! @} */ // group VideoFunctions
