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

#ifndef _WINDIRECTINPUT_H_
#define _WINDIRECTINPUT_H_

#ifndef _PLATFORMWIN32_H_
#include "platformWin32/platformWin32.h"
#endif
#ifndef _PLATFORMINPUT_H_
#include "platform/platformInput.h"
#endif
#ifndef _WINDINPUTDEVICE_H_
#include "platformWin32/winDInputDevice.h"
#endif

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

struct InputEvent;

//------------------------------------------------------------------------------
class DInputManager : public InputManager
{
   private:
      typedef SimGroup Parent;

      HMODULE        mDInputLib;
      LPDIRECTINPUT8 mDInputInterface;

      static bool smKeyboardEnabled;
      static bool smMouseEnabled;
      static bool smJoystickEnabled;

      bool mKeyboardActive;
      bool mMouseActive;
      bool mJoystickActive;

      void  enumerateDevices();

      static BOOL CALLBACK EnumDevicesProc( const DIDEVICEINSTANCE *pddi, LPVOID pvRef );

      bool acquire( U8 deviceType, U8 deviceID );
      void unacquire( U8 deviceType, U8 deviceID );

   public:
      DInputManager();

      bool enable();
      void disable();

      void onDeleteNotify( SimObject* object );
      bool onAdd();
      void onRemove();

      void process();

      // DirectInput functions:
      static void init();

      static bool enableKeyboard();
      static void disableKeyboard();
      static bool isKeyboardEnabled();
      bool activateKeyboard();
      void deactivateKeyboard();
      bool isKeyboardActive()       { return( mKeyboardActive ); }

      static bool enableMouse();
      static void disableMouse();
      static bool isMouseEnabled();
      bool activateMouse();
      void deactivateMouse();
      bool isMouseActive()          { return( mMouseActive ); }

      static bool enableJoystick();
      static void disableJoystick();
      static bool isJoystickEnabled();
      bool activateJoystick();
      void deactivateJoystick();
      bool isJoystickActive()       { return( mJoystickActive ); }

      // Console interface:
      const char* getJoystickAxesString( U32 deviceID );
};

#endif  // _H_WINDIRECTINPUT_
