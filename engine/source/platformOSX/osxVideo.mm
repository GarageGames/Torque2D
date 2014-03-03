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
#import "platformOSX/platformOSX.h"
#include "platform/platformVideo.h"

Resolution Video::getDesktopResolution()
{
    // Get the screen the application window resides in and is receiving input
    NSScreen* mainScreen = [NSScreen mainScreen];
    
    // Get the visible boundaries
    NSRect screenRect = [mainScreen visibleFrame];

    // Get the screen depth. You cannot access depth directly. It must be passed
    // into a function that will return the bpp
    int bpp = (int)NSBitsPerPixelFromDepth([mainScreen depth]);
    
    // Build the return resolution
    Resolution resolution;
    resolution.w = (U32)screenRect.size.width;
    resolution.h = (U32)screenRect.size.height;
    resolution.bpp = (U32)bpp;
    
    osxPlatState * platState = [osxPlatState sharedPlatState];
    
    [platState setDesktopWidth:resolution.w];
    [platState setDesktopHeight:resolution.h];
    [platState setDesktopBitsPixel:resolution.bpp];
    
    // Return the new resolution
    return resolution;
}