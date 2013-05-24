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

extern S32 getIDFromName(EnumTable::Enums *table, const char *name, S32 def);
extern EnumTable::Enums sgButtonEnums[];
extern EnumTable::Enums sgIconEnums[];

/*! @defgroup MessageBox Message Box
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Pops up a message box
    @param title The message box's title to display
    @param message The message to display in the box
    @param buttons The buttons to include on box (default MBOkCancel)
    @param icon The displayed icon (default MIInformation)
    @return Returns the ID of the box
*/
ConsoleFunctionWithDocs(messageBox, ConsoleInt, 3, 5, (title, message, [buttons]?, [icon]?))
{
   S32 btns = MBOkCancel;
   S32 icns = MIInformation;
   
   if(argc > 3)
      btns = getIDFromName(sgButtonEnums, argv[3], btns);
   if(argc > 4)
      icns = getIDFromName(sgIconEnums, argv[4], icns);

   return Platform::messageBox(argv[1], argv[2], (MBButtons)btns, (MBIcons)icns);
}

/*! @} */ // end group MessageBox
