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

ConsoleMethodGroupBeginWithDocs(PopupMenu, SimObject)

/*! Insert a new menu item
    @param pos The position of the item
    @param title The title of the item to add (optional)
    @param accelerator A keyboard accelerator for the menu item
    @return Returns the id of the new menu item as a nonnegative integer (-1 on fail)
*/
ConsoleMethodWithDocs(PopupMenu, insertItem, ConsoleInt, 3, 5, (pos, [title]?, [accelerator]?))
{
   return object->insertItem(dAtoi(argv[2]), argc < 4 ? NULL : argv[3], argc < 5 ? "" : argv[4]);
}

/*! Removes the menu item at the given position
    @param pos The position of the desired menu item to obliterate
    @return No return value
*/
ConsoleMethodWithDocs(PopupMenu, removeItem, ConsoleVoid, 3, 3, (pos))
{
   object->removeItem(dAtoi(argv[2]));
}

/*! Inserts a menu item into the current object
    @param pos The position of the menu item
    @param The desired title of the sub menu
    @param subMenu The submenu you are inserting.
    @return Returns the ID of the inserted item or -1 on failure
*/
ConsoleMethodWithDocs(PopupMenu, insertSubMenu, ConsoleInt, 5, 5, (pos, title, subMenu))
{
   PopupMenu *mnu = dynamic_cast<PopupMenu *>(Sim::findObject(argv[4]));
   if(mnu == NULL)
   {
      Con::errorf("PopupMenu::insertSubMenu - Invalid PopupMenu object specified for submenu");
      return -1;
   }
   return object->insertSubMenu(dAtoi(argv[2]), argv[3], mnu);
}

//////////////////////////////////////////////////////////////////////////

/*! Enables the item at the given position
    @param pos The item's position
    @param enabled A boolean value to set its current status
    @return No return value
*/
ConsoleMethodWithDocs(PopupMenu, enableItem, ConsoleVoid, 4, 4, (pos, enabled))
{
   object->enableItem(dAtoi(argv[2]), dAtob(argv[3]));
}

/*! Sets the given item's \checked\ status
    @param pos The item's position
    @return No return value
*/
ConsoleMethodWithDocs(PopupMenu, checkItem, ConsoleVoid, 4, 4, (pos, checked))
{
   object->checkItem(dAtoi(argv[2]), dAtob(argv[3]));
}

/*! Checks the specified menu item and makes it a radio item
    @param firstPos The position of the first item in the group
    @param lastPos The position of the last item in the group
    @param checkPos The position of the item to check
    @return No return value.
*/
ConsoleMethodWithDocs(PopupMenu, checkRadioItem, ConsoleVoid, 5, 5, (firstPos, lastPos, checkPos))
{
   object->checkRadioItem(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]));
}

/*! Checks (no pun intended) the \checked\ flag of the given menu item
    @param pos The position of the desired item
    @return Returns true if checked, false otherwise
*/
ConsoleMethodWithDocs(PopupMenu, isItemChecked, ConsoleBool, 3, 3, (pos))
{
   return object->isItemChecked(dAtoi(argv[2]));
}

//////////////////////////////////////////////////////////////////////////

/*! Attaches given menu item to menu bar
    @param pos The position of the desired menu item
    @param title The menu item title
    @return No return value
*/
ConsoleMethodWithDocs(PopupMenu, attachToMenuBar, ConsoleVoid, 4, 4, (pos, title))
{
   object->attachToMenuBar(dAtoi(argv[2]), argv[3]);
}

/*! Removes current item from menu bar
    @return No return value
*/
ConsoleMethodWithDocs(PopupMenu, removeFromMenuBar, ConsoleVoid, 2, 2, ())
{
   object->removeFromMenuBar();
}

//////////////////////////////////////////////////////////////////////////

/*! Show the menu (optional: specify position)
    @param x,y Coodinates to display menu (upper left corner)
    @return No return value.
*/
ConsoleMethodWithDocs(PopupMenu, showPopup, ConsoleVoid, 2, 4, ([x, y]?))
{
   S32 x = argc >= 3 ? dAtoi(argv[2]) : -1;
   S32 y = argc >= 4 ? dAtoi(argv[3]) : -1;
   object->showPopup(x, y);
}

ConsoleMethodGroupEndWithDocs(PopupMenu)
