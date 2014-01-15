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
ConsoleFunction( activateKeyboard, bool, 1, 1, "() Use the activateKeyboard function to enable directInput polling of the keyboard.\n"
																"@return Returns a true if polling was successfully enabled.\n"
																"@sa deactivateKeyboard")
{
	return Input::activateKeyboard();
}

//------------------------------------------------------------------------------
ConsoleFunction( deactivateKeyboard, void, 1, 1, "() Use the deactivateKeyboard function to disable directInput polling of the keyboard.\n"
																"@return No return value.\n"
																"@sa activateKeyboard")
{
	Input::deactivateKeyboard();
}

//------------------------------------------------------------------------------
ConsoleFunction( enableMouse, void, 1, 1, "() Use the enableMouse function to enable mouse input.\n"
																"@return No return value.\n"
																"@sa disableMouse")
{
	Input::enableMouse();
}

//------------------------------------------------------------------------------
ConsoleFunction( disableMouse, void, 1, 1, "() Use the disableMouse function to disable mouse input.\n"
																"@return No return value.\n"
																"@sa enableMouse")
{
	Input::disableMouse();
}

//------------------------------------------------------------------------------
ConsoleFunction( enableJoystick, bool, 1, 1, "() Use the enableJoystick function to enable joystick input if it is present.\n"
																"@return Will return true if the joystick is present and was successfully enabled, false otherwise.\n"
																"@sa disableJoystick, getJoystickAxes, isJoystickDetected")
{
	return Input::enableJoystick();
}

//------------------------------------------------------------------------------
ConsoleFunction( disableJoystick, void, 1, 1, "() Use the disableJoystick function to disable joystick input.\n"
																"@return No return value.\n"
																"@sa enableJoystick, getJoystickAxes, isJoystickEnabled")
{
	Input::disableJoystick();
}

//------------------------------------------------------------------------------
ConsoleFunction( echoInputState, void, 1, 1, "() Use the echoInputState function to dump the input state of the mouse, keyboard, and joystick to the console.\n"
																"@return No return value.\n"
																"@sa activateDirectInput, deactivateDirectInput, activateKeyboard, deactivateKeyboard, disableJoystick, enableJoystick, enableMouse, disableMouse")
{
	Input::echoInputState();
}
