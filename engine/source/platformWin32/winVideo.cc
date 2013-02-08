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

#include "winVideo.h"


bool WinDisplayDevice::smCanSwitchBitDepth = false;
bool WinDisplayDevice::smCanDo16Bit        = false;
bool WinDisplayDevice::smCanDo32Bit        = false;


WinDisplayDevice::WinDisplayDevice()
{
   mRestoreGamma      = false;
}

void WinDisplayDevice::enumerateBitDepths()
{
   // Enumerate all available resolutions:
   DEVMODE devMode;
   U32 modeNum = 0;
   U32 foundDisplayMode = true;

   while ( foundDisplayMode )
   {
      // Clear out our display mode settings structure.
      dMemset( &devMode, 0, sizeof( devMode ) );
      devMode.dmSize = sizeof( devMode );

      // Check for display mode settings
      foundDisplayMode = EnumDisplaySettings( NULL, modeNum++, &devMode );

      //MIN_RESOLUTION defined in platformWin32/platformGL.h
      if ( devMode.dmPelsWidth >= MIN_RESOLUTION_X && devMode.dmPelsHeight >= MIN_RESOLUTION_Y && ( devMode.dmBitsPerPel == 16 || devMode.dmBitsPerPel == 32 ) )
      {
         // Note BPP for can switch bit depth.
         if( devMode.dmBitsPerPel == 16 )
            smCanDo16Bit = true;
         else if( devMode.dmBitsPerPel == 32 )
            smCanDo32Bit = true;
      }
   }

   // Set can switch bit depth based on found resolutions.
   if( smCanDo16Bit && smCanDo32Bit )
      smCanSwitchBitDepth = true;
   else
      smCanSwitchBitDepth = false;
}

void WinDisplayDevice::initDevice()
{
   // Set some initial conditions:
   mResolutionList.clear();

   // Enumerate all available resolutions:
   DEVMODE devMode;
   U32 modeNum = 0;
   U32 foundDisplayMode = true;

   while ( foundDisplayMode )
   {
      // Clear out our display mode settings structure.
      dMemset( &devMode, 0, sizeof( devMode ) );
      devMode.dmSize = sizeof( devMode );

      // Check for display mode settings
      foundDisplayMode = EnumDisplaySettings( NULL, modeNum++, &devMode );

      if ( devMode.dmPelsWidth >= MIN_RESOLUTION_X && devMode.dmPelsHeight >= MIN_RESOLUTION_Y && ( devMode.dmBitsPerPel == 16 || devMode.dmBitsPerPel == 32 ) )
      {
         // Only add this resolution if it is not already in the list:
         bool alreadyInList = false;
         for ( U32 i = 0; i < (U32)mResolutionList.size(); i++ )
         {
            if ( devMode.dmPelsWidth == mResolutionList[i].w
               && devMode.dmPelsHeight == mResolutionList[i].h
               && devMode.dmBitsPerPel == mResolutionList[i].bpp )
            {
               alreadyInList = true;
               break;
            }
         }

         // If we've not already added this resolution, add it to our resolution list.
         if ( !alreadyInList )
         {
            Resolution newRes( devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel );
            mResolutionList.push_back( newRes );
         }
      }
   }
}
