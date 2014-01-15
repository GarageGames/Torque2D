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

#include "platform/menus/popupMenu.h"
#include "console/consoleTypes.h"
#include "gui/guiCanvas.h"
#include "memory/safeDelete.h"

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

PopupMenu::PopupMenu()
{
   createPlatformPopupMenuData();

   mSubmenus = new SimSet;
   mSubmenus->registerObject();

   mIsPopup = false;
}

PopupMenu::~PopupMenu()
{
   // This searches the menu bar so is safe to call for menus
   // that aren't on it, since nothing will happen.
   removeFromMenuBar();

   SimSet::iterator i;
   while((i = mSubmenus->begin()) != mSubmenus->end())
   {
      (*i)->deleteObject();
   }

   mSubmenus->deleteObject();
   deletePlatformPopupMenuData();
}

IMPLEMENT_CONOBJECT(PopupMenu);

//////////////////////////////////////////////////////////////////////////

void PopupMenu::initPersistFields()
{
   Parent::initPersistFields();

   // [neo, 5/7/2007 - #2973]
   // isPopup field corrected to TypeBool instead of TypeString which caused
   // a crash when reading field value e.g. in dump()   
   addField( "isPopup", TypeBool, Offset(mIsPopup, PopupMenu));
}

//////////////////////////////////////////////////////////////////////////

bool PopupMenu::onAdd()
{
   if(! Parent::onAdd())
      return false;

   createPlatformMenu();

   return true;
}

void PopupMenu::onRemove()
{
   Parent::onRemove();
}

//////////////////////////////////////////////////////////////////////////

bool PopupMenu::onMessageReceived(StringTableEntry queue, const char* event, const char* data)
{
   return Con::executef(this, 4, "onMessageReceived", queue, event, data);
}


bool PopupMenu::onMessageObjectReceived(StringTableEntry queue, Message *msg )
{
   return Con::executef(this, 4, "onMessageReceived", queue, Con::getIntArg(msg->getId()));
}

//////////////////////////////////////////////////////////////////////////
// Console Methods
//////////////////////////////////////////////////////////////////////////

ConsoleMethod(PopupMenu, insertItem, S32, 3, 5, "(pos[, title][, accelerator) Insert a new menu item\n"
              "@param pos The position of the item\n"
              "@param title The title of the item to add (optional)\n"
              "@param accelerator A keyboard accelerator for the menu item\n"
              "@return Returns the id of the new menu item as a nonnegative integer (-1 on fail)")
{
   return object->insertItem(dAtoi(argv[2]), argc < 4 ? NULL : argv[3], argc < 5 ? "" : argv[4]);
}

ConsoleMethod(PopupMenu, removeItem, void, 3, 3, "(pos) Removes the menu item at the given position\n"
              "@param pos The position of the desired menu item to obliterate\n"
              "@return No return value")
{
   object->removeItem(dAtoi(argv[2]));
}

ConsoleMethod(PopupMenu, insertSubMenu, S32, 5, 5, "(pos, title, subMenu) Inserts a menu item into the current object\n"
              "@param pos The position of the menu item\n"
              "@param The desired title of the sub menu\n"
              "@param subMenu The submenu you are inserting.\n"
              "@return Returns the ID of the inserted item or -1 on failure")
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

ConsoleMethod(PopupMenu, enableItem, void, 4, 4, "(pos, enabled) Enables the item at the given position\n"
              "@param pos The item's position\n"
              "@param enabled A boolean value to set its current status\n"
              "@return No return value")
{
   object->enableItem(dAtoi(argv[2]), dAtob(argv[3]));
}

ConsoleMethod(PopupMenu, checkItem, void, 4, 4, "(pos, checked) Sets the given item's \"checked\" status\n"
              "@param pos The item's position\n"
              "@return No return value")
{
   object->checkItem(dAtoi(argv[2]), dAtob(argv[3]));
}

ConsoleMethod(PopupMenu, checkRadioItem, void, 5, 5, "(firstPos, lastPos, checkPos) Checks the specified menu item and makes it a radio item\n"
              "@param firstPos The position of the first item in the group\n"
              "@param lastPos The position of the last item in the group\n"
              "@param checkPos The position of the item to check\n"
              "@return No return value.")
{
   object->checkRadioItem(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]));
}

ConsoleMethod(PopupMenu, isItemChecked, bool, 3, 3, "(pos) Checks (no pun intended) the \"checked\" flag of the given menu item\n"
              "@param pos The position of the desired item\n"
              "@return Returns true if checked, false otherwise")
{
   return object->isItemChecked(dAtoi(argv[2]));
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(PopupMenu, attachToMenuBar, void, 4, 4, "(pos, title) Attaches given menu item to menu bar\n"
              "@param pos The position of the desired menu item\n"
              "@param title The menu item title\n"
              "@return No return value")
{
   object->attachToMenuBar(dAtoi(argv[2]), argv[3]);
}

ConsoleMethod(PopupMenu, removeFromMenuBar, void, 2, 2, "() Removes current item from menu bar"
              "@return No return value")
{
   object->removeFromMenuBar();
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(PopupMenu, showPopup, void, 2, 4, "([x, y]) Show the menu (optional: specify position)\n"
              "@param x,y Coodinates to display menu (upper left corner)\n"
              "@return No return value.")
{
   S32 x = argc >= 3 ? dAtoi(argv[2]) : -1;
   S32 y = argc >= 4 ? dAtoi(argv[3]) : -1;
   object->showPopup(x, y);
}
