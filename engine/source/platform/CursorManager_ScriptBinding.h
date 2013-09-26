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

/*! @defgroup CursorManager Cursor Management
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Set's the current cursor to one specified
    @param cursorShapeName Name corresponding to enumerated shape value: "Arrow", "Wait", "Plus", "ResizeVert", "ResizeHorz", 
    "ResizeAll", "ibeam", "ResizeNESW", "ResizeNWSE"
    @return No Return Value
*/
ConsoleFunctionWithDocs( inputPushCursor, ConsoleVoid, 2, 2, (cursorShapeName))
{
   S32 val = 0;

   //*** Find the cursor shape
   if(argc == 2)
   {
      for (S32 i = 0; i < gCurManagerShapesTable.size; i++)
      {
         if (! dStricmp(argv[1], gCurManagerShapesTable.table[i].label))
         {
            val = gCurManagerShapesTable.table[i].index;
            break;
         }
      }
   }

   //*** Now set it
   CursorManager* cm = Input::getCursorManager();
   if(cm)
   {
      cm->pushCursor(val);
   }
}

/*! Pops the current cursor shape from manager stack
    @return No Return Value.
*/
ConsoleFunctionWithDocs( inputPopCursor, ConsoleVoid, 1, 1, ())
{
   CursorManager* cm = Input::getCursorManager();
   if(cm)
   {
      cm->popCursor();
   }
}

/*! @} */ // end group CursorManager
