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

ConsoleFunctionGroupBegin( InputManagement, "Functions that let you deal with input from scripts" );

/*! @defgroup InputManagement Input Management
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the deactivateDirectInput function to de-activate polling of direct input devices (keyboard, mouse, joystick, et cetera).
    @return No return value.
    @sa activateDirectInput
*/
ConsoleFunctionWithDocs( deactivateDirectInput, ConsoleVoid, 1, 1, ())
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   if ( Input::isActive() )
      Input::deactivate();
}

//--------------------------------------------------------------------------
/*! Use the activateDirectInput function to activate polling of direct input devices (keyboard, mouse, joystick, et cetera).
    @return No return value.
    @sa deactivateDirectInput
*/
ConsoleFunctionWithDocs( activateDirectInput, ConsoleVoid, 1, 1, ())
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   if ( !Input::isActive() )
      Input::activate();
}

ConsoleFunctionGroupEnd( InputManagement );

/*! @} */ // end group InputManagement
