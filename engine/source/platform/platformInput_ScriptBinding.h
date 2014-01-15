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

#include "console/console.h"
#include "platform/platformInput.h"

//------------------------------------------------------------------------------
/*! Use the activateKeyboard function to enable directInput polling of the keyboard.
    @return Returns a true if polling was successfully enabled.
    @sa deactivateKeyboard
*/
ConsoleFunctionWithDocs( activateKeyboard, ConsoleBool, 1, 1, ())
{
	return Input::activateKeyboard();
}

//------------------------------------------------------------------------------
/*! Use the deactivateKeyboard function to disable directInput polling of the keyboard.
    @return No return value.
    @sa activateKeyboard
*/
ConsoleFunctionWithDocs( deactivateKeyboard, ConsoleVoid, 1, 1, ())
{
	Input::deactivateKeyboard();
}

//------------------------------------------------------------------------------
/*! Use the enableMouse function to enable mouse input.
    @return No return value.
    @sa disableMouse
*/
ConsoleFunctionWithDocs( enableMouse, ConsoleVoid, 1, 1, ())
{
	Input::enableMouse();
}

//------------------------------------------------------------------------------
/*! Use the disableMouse function to disable mouse input.
    @return No return value.
    @sa enableMouse
*/
ConsoleFunctionWithDocs( disableMouse, ConsoleVoid, 1, 1, ())
{
	Input::disableMouse();
}

//------------------------------------------------------------------------------
/*! Use the enableJoystick function to enable joystick input if it is present.
    @return Will return true if the joystick is present and was successfully enabled, false otherwise.
    @sa disableJoystick, getJoystickAxes, isJoystickDetected
*/
ConsoleFunctionWithDocs( enableJoystick, ConsoleBool, 1, 1, ())
{
	return Input::enableJoystick();
}

//------------------------------------------------------------------------------
/*! Use the disableJoystick function to disable joystick input.
    @return No return value.
    @sa enableJoystick, getJoystickAxes, isJoystickEnabled
*/
ConsoleFunctionWithDocs( disableJoystick, ConsoleVoid, 1, 1, ())
{
	Input::disableJoystick();
}

//------------------------------------------------------------------------------
/*! Use the echoInputState function to dump the input state of the mouse, keyboard, and joystick to the console.
    @return No return value.
    @sa activateDirectInput, deactivateDirectInput, activateKeyboard, deactivateKeyboard, disableJoystick, enableJoystick, enableMouse, disableMouse
*/
ConsoleFunctionWithDocs( echoInputState, ConsoleVoid, 1, 1, ())
{
	Input::echoInputState();
}

/*!	Enables XInput for Xbox 360 controllers.
	@return No return value.
*/
ConsoleFunctionWithDocs( enableXInput, ConsoleBool, 1, 1, ())
            
{
   return( DInputManager::enableXInput() );
}

/*! Disables XInput for Xbox 360 controllesr
	@return No return value.
*/

ConsoleFunctionWithDocs( disableXInput, ConsoleVoid, 1, 1, ())
{
   DInputManager::disableXInput();
}

/*!	Rebuilds the XInput section of the InputManager.
     @return No return value.
*/
ConsoleFunctionWithDocs( resetXInput, ConsoleVoid, 1, 1, ())
{
   // This function requests a full "refresh" of events for all controllers the
   // next time we go through the input processing loop. This is useful to call
   // at the beginning of your game code after your actionMap is set up to hook
   // all of the appropriate events
   
   DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );
   if ( mgr && mgr->isEnabled() ) 
      mgr->resetXInput();
}

/*!	Checks to see if an Xbox 360 controller is connected
	@param controllerID Zero-based index of the controller to check.
	@return 1 if the controller is connected, 0 if it isn't, and 205 if XInput
	hasn't been initialized.
*/

ConsoleFunctionWithDocs( isXInputConnected, ConsoleBool, 2, 2, ( int controllerID ))
{
   DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );
   if ( mgr && mgr->isEnabled() ) return mgr->isXInputConnected( atoi( argv[1] ) );
   return false;
}

/*!	Queries the current state of a connected Xbox 360 controller.
	@param controllerID Zero-based index of the controller to return information about.
    @param property Name of input action being queried, such as \"XI_THUMBLX\".
    @param current True checks current device in action.
    @return Button queried - 1 if the button is pressed, 0 if it's not.
    @return Thumbstick queried - Int representing displacement from rest position.
    @return %Trigger queried - Int from 0 to 255 representing how far the trigger is displaced.
*/

//------------------------------------------------------------------------------
ConsoleFunctionWithDocs( getXInputState, ConsoleInt, 3, 4, ( int controllerID, string property, bool current ) )
{
   DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );

   if ( !mgr || !mgr->isEnabled() ) 
      return -1;

   // Use a little bit of macro magic to simplify this otherwise monolothic
   // block of code.
#define GET_XI_STATE(constName) \
   if (!dStricmp(argv[2], #constName)) \
      return mgr->getXInputState( dAtoi( argv[1] ), constName, ( dAtoi ( argv[3] ) == 1) );
   
   GET_XI_STATE(XI_THUMBLX);
   GET_XI_STATE(XI_THUMBLY);
   GET_XI_STATE(XI_THUMBRX);
   GET_XI_STATE(XI_THUMBRY);
   GET_XI_STATE(XI_LEFT_TRIGGER);
   GET_XI_STATE(XI_RIGHT_TRIGGER);
   GET_XI_STATE(SI_UPOV);
   GET_XI_STATE(SI_DPOV);
   GET_XI_STATE(SI_LPOV);
   GET_XI_STATE(SI_RPOV);
   GET_XI_STATE(XI_START);
   GET_XI_STATE(XI_BACK);
   GET_XI_STATE(XI_LEFT_THUMB);
   GET_XI_STATE(XI_RIGHT_THUMB);
   GET_XI_STATE(XI_LEFT_SHOULDER);
   GET_XI_STATE(XI_RIGHT_SHOULDER);
   GET_XI_STATE(XI_A);
   GET_XI_STATE(XI_B);
   GET_XI_STATE(XI_X);
   GET_XI_STATE(XI_Y);
#undef GET_XI_STATE

   return -1;
}


/*!	Activates the vibration motors in the specified controller.
    The controller will constantly at it's xRumble and yRumble intensities until
    changed or told to stop.
     
	 Valid inputs for xRumble/yRumble are [0 - 1].

	 @param device Name of the device to rumble.
      @param xRumble Intensity to apply to the left motor.
      @param yRumble Intensity to apply to the right motor.

	 @note in an Xbox 360 controller, the left motor is low-frequency,
     while the right motor is high-frequency.

*/

ConsoleFunctionWithDocs( rumble, ConsoleVoid, 4, 4, (string device, float xRumble, float yRumble) )
{
   DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );
   if ( mgr && mgr->isEnabled() )
   {
      mgr->rumble(argv[1], dAtof(argv[2]), dAtof(argv[3]));
   }
   else
   {
      Con::printf( "DirectInput/XInput is not enabled." );
   }
}
