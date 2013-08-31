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

class Trigger;

ConsoleMethodGroupBeginWithDocs(Trigger, SceneObject)

/*! Set whether trigger checks onEnter events
	@param setting Default is true.
	@return No return value.
*/
ConsoleMethodWithDocs(Trigger, setEnterCallback, ConsoleVoid, 2, 3, ([setting]?))
{
   // If the value isn't specified, the default is true.
   bool callback = true;
   if (argc > 2)
      callback = dAtob(argv[2]);

   object->setEnterCallback(callback);
}

//-----------------------------------------------------------------------------

/*! Set whether trigger checks onStay events
	@param setting Default is true.
	@return No return value.
*/
ConsoleMethodWithDocs(Trigger, setStayCallback, ConsoleVoid, 2, 3, ([setting]?))
{
   // If the value isn't specified, the default is true.
   bool callback = true;
   if (argc > 2)
      callback = dAtob(argv[2]);

   object->setStayCallback(callback);
}

//-----------------------------------------------------------------------------

/*! Set whether trigger checks onLeave events
    @param setting Default is true.
    @return No return value.
*/
ConsoleMethodWithDocs(Trigger, setLeaveCallback, ConsoleVoid, 2, 3, ([setting]?))
{
   // If the value isn't specified, the default is true.
   bool callback = true;
   if (argc > 2)
      callback = dAtob(argv[2]);

   object->setLeaveCallback(callback);
}

//-----------------------------------------------------------------------------

/*!
	@return Returns whether trigger checks onEnter events
*/
ConsoleMethodWithDocs(Trigger, getEnterCallback, ConsoleBool, 2, 2, ())
{
   return object->getEnterCallback();
}

//-----------------------------------------------------------------------------

/*!
	@return Returns whether trigger checks onStay events
*/
ConsoleMethodWithDocs(Trigger, getStayCallback, ConsoleBool, 2, 2, ())
{
   return object->getStayCallback();
}

//-----------------------------------------------------------------------------

/*!
	@return Returns whether trigger checks onLeave events
*/
ConsoleMethodWithDocs(Trigger, getLeaveCallback, ConsoleBool, 2, 2, ())
{
   return object->getLeaveCallback();
}

//-----------------------------------------------------------------------------

ConsoleMethodGroupEndWithDocs(Trigger)
