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

/*! Use the isJoystickDetected function to determine if one or more joysticks are connected to the system.
    This doesn't tell us how many joysticks there are, just that there are joysticks. It is our job to find out how many and to attach them.
    @return Returns true if one or more joysticks are attached and detected, false otherwise.
    @sa disableJoystick, enableJoystick, getJoystickAxes
*/
ConsoleFunctionWithDocs( isJoystickDetected, ConsoleBool, 1, 1, () )
{
   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (manager)
      return manager->joystickDetected();
   else
      return false;
}

//------------------------------------------------------------------------------
/*! Use the getJoystickAxes function to get the current axes position (x and y ) of any intance of a joystick.
    @param instance A non-negative number value selecting a specific joystick instance attached to this computer.
    @return Returns a string containing the \x y\ position of the joystick.
    @sa disableJoystick, enableJoystick, isJoystickDetected
*/
ConsoleFunctionWithDocs( getJoystickAxes, ConsoleString, 2, 2, (instance) )
{
   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (manager)
      return manager->getJoystickAxesString(dAtoi(argv[1]));
   else
      return "";
}