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

#ifndef _WINDDISPLAYVIDEO_H_
#define _WINDDISPLAYVIDEO_H_

#ifndef _PLATFORMVIDEO_H_
#include "platform/platformVideo.h"
#endif

#ifndef _PLATFORMGL_H_
#include "platformWin32/platformGL.h"
#endif

#ifndef _PLATFORMWIN32_H_
#include "platformWin32/platformWin32.h"
#endif


class WinDisplayDevice : public DisplayDevice
{
protected:
   bool mRestoreGamma;
   U16  mOriginalRamp[256*3];
   static bool smCanSwitchBitDepth;
   static bool smCanDo16Bit;
   static bool smCanDo32Bit;

public:
   WinDisplayDevice();
   virtual ~WinDisplayDevice() {}

   virtual void initDevice();

   static void enumerateBitDepths();
};

#endif // _H_WIND3DVIDEO


