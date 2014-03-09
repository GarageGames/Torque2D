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

#include "platformEmscripten/platformEmscripten.h"
#include "platform/menus/popupMenu.h"
#include "memory/safeDelete.h"
#include "gui/guiCanvas.h"

S32 PopupMenu::insertItem(S32 pos, const char *title, const char* accel)
{ return 0; }

S32 PopupMenu::insertSubMenu(S32 pos, const char *title, PopupMenu *submenu)
{ return 0; }

void PopupMenu::removeItem(S32 itemPos)
{}

void PopupMenu::enableItem(S32 pos, bool enable)
{}

void PopupMenu::checkItem(S32 pos, bool checked)
{}

void PopupMenu::checkRadioItem(S32 firstPos, S32 lastPos, S32 checkPos)
{}

bool PopupMenu::isItemChecked(S32 pos)
{ return false; }

bool PopupMenu::canHandleID(U32 iD)
{ return false; }

bool PopupMenu::handleSelect(U32 command, const char *text /* = NULL */)
{ return false; }

void PopupMenu::showPopup(S32 x /* = -1 */, S32 y /* = -1 */)
{}

void PopupMenu::attachToMenuBar(S32 pos, const char *title)
{}

void PopupMenu::removeFromMenuBar()
{}


void PopupMenu::createPlatformPopupMenuData()
{}

void PopupMenu::deletePlatformPopupMenuData()
{}

void PopupMenu::createPlatformMenu()
{}

