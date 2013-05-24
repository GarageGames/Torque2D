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
