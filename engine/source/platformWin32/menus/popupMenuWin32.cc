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
#include "platformWin32/platformWin32.h"
#include "platformWin32/winWindow.h"
#include "memory/safeDelete.h"

//////////////////////////////////////////////////////////////////////////
// Platform Menu Data
//////////////////////////////////////////////////////////////////////////

struct PlatformPopupMenuData
{
   static U32 mLastPopupMenuID;
   static const U32 PopupMenuIDRange;

   HMENU mMenu;
   U32 mMenuID;
   U32 mLastID;

   PlatformPopupMenuData()
   {
      mMenu = NULL;
      mMenuID = mLastPopupMenuID++;
      mLastID = 0;
   }

   ~PlatformPopupMenuData()
   {
      if(mMenu)
         DestroyMenu(mMenu);
   }
};

U32 PlatformPopupMenuData::mLastPopupMenuID = 0;
const U32 PlatformPopupMenuData::PopupMenuIDRange = 100;

void PopupMenu::createPlatformPopupMenuData()
{
   mData = new PlatformPopupMenuData;
}

void PopupMenu::deletePlatformPopupMenuData()
{
   SAFE_DELETE(mData);
}
void PopupMenu::createPlatformMenu()
{
   mData->mMenu = mIsPopup ? CreatePopupMenu() : CreateMenu();
}

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

S32 PopupMenu::insertItem(S32 pos, const char *title, const char* accelerator)
{
   MENUITEMINFOA mi;
   mi.cbSize = sizeof(mi);
   mi.fMask = MIIM_ID|MIIM_TYPE;
   mi.wID = (mData->mMenuID * PlatformPopupMenuData::PopupMenuIDRange) + mData->mLastID + 1;
   mData->mLastID++;
   if(title && *title)
      mi.fType = MFT_STRING;
   else
      mi.fType = MFT_SEPARATOR;
   char buf[1024];
   dSprintf(buf, sizeof(buf), "%s\t%s", title, accelerator);
   mi.dwTypeData = (LPSTR)buf;
   if(InsertMenuItemA(mData->mMenu, pos, TRUE, &mi))
   {
      DrawMenuBar(winState.appWindow);
      return mi.wID;
   }

   return -1;
}

S32 PopupMenu::insertSubMenu(S32 pos, const char *title, PopupMenu *submenu)
{
   for(S32 i = 0;i < mSubmenus->size();i++)
   {
      if(submenu == (*mSubmenus)[i])
      {
         Con::errorf("PopupMenu::insertSubMenu - Attempting to add submenu twice");
         return -1;
      }
   }

   MENUITEMINFOA mi;
   mi.cbSize = sizeof(mi);
   mi.fMask = MIIM_ID|MIIM_TYPE|MIIM_SUBMENU|MIIM_DATA;
   mi.wID = (mData->mMenuID * PlatformPopupMenuData::PopupMenuIDRange) + mData->mLastID + 1;
   if(title && *title)
      mi.fType = MFT_STRING;
   else
      mi.fType = MFT_SEPARATOR;
   mi.dwTypeData = (LPSTR)title;
   mi.hSubMenu = submenu->mData->mMenu;
   mi.dwItemData = (ULONG_PTR)submenu;
   if(InsertMenuItemA(mData->mMenu, pos, TRUE, &mi))
   {
      mSubmenus->addObject(submenu);
      
      DrawMenuBar(winState.appWindow);
      return mi.wID;
   }

   return -1;
}

void PopupMenu::removeItem(S32 itemPos)
{
   MENUITEMINFOA mi;
   mi.cbSize = sizeof(mi);
   mi.fMask = MIIM_DATA;
   if(GetMenuItemInfoA(mData->mMenu, itemPos, TRUE, &mi))
   {
      if(mi.fMask & MIIM_DATA)
      {
         PopupMenu *mnu = (PopupMenu *)mi.dwItemData;
         if (mnu)
            mSubmenus->removeObject(mnu);
      }
   }

   RemoveMenu(mData->mMenu, itemPos, MF_BYPOSITION);
   DrawMenuBar(winState.appWindow);
}

//////////////////////////////////////////////////////////////////////////

void PopupMenu::enableItem(S32 pos, bool enable)
{
   U32 flags = enable ? MF_ENABLED : MF_GRAYED;
   EnableMenuItem(mData->mMenu, pos, MF_BYPOSITION|flags);
}

void PopupMenu::checkItem(S32 pos, bool checked)
{
//    U32 flags = checked ? MF_CHECKED : MF_UNCHECKED;
//    CheckMenuItem(mData->mMenu, pos, MF_BYPOSITION|flags);

   MENUITEMINFOA mi;
   mi.cbSize = sizeof(mi);
   mi.fMask = MIIM_STATE;
   mi.fState = checked ? MFS_CHECKED : MFS_UNCHECKED;
   SetMenuItemInfoA(mData->mMenu, pos, TRUE, &mi);
}

void PopupMenu::checkRadioItem(S32 firstPos, S32 lastPos, S32 checkPos)
{
   CheckMenuRadioItem(mData->mMenu, firstPos, lastPos, checkPos, MF_BYPOSITION);
}

bool PopupMenu::isItemChecked(S32 pos)
{
   MENUITEMINFOA mi;
   mi.cbSize = sizeof(mi);
   mi.fMask = MIIM_STATE;
   if(GetMenuItemInfoA(mData->mMenu, pos, TRUE, &mi) && (mi.fState & MFS_CHECKED))
      return true;
   return false;
}

//////////////////////////////////////////////////////////////////////////

bool PopupMenu::canHandleID(U32 iD)
{
   for(S32 i = 0;i < mSubmenus->size();i++)
   {
      PopupMenu *subM = dynamic_cast<PopupMenu *>((*mSubmenus)[i]);
      if(subM == NULL)
         continue;

      if(subM->canHandleID(iD))
         return true;
   }

   if(iD >= mData->mMenuID * PlatformPopupMenuData::PopupMenuIDRange &&
      iD < (mData->mMenuID+1) * PlatformPopupMenuData::PopupMenuIDRange)
   {
      return true;
   }

   return false;
}

bool PopupMenu::handleSelect(U32 command, const char *text /* = NULL */)
{
   // [tom, 8/20/2006] Pass off to a sub menu if it's for them
   for(S32 i = 0;i < mSubmenus->size();i++)
   {
      PopupMenu *subM = dynamic_cast<PopupMenu *>((*mSubmenus)[i]);
      if(subM == NULL)
         continue;

      if(subM->canHandleID(command))
      {
         return subM->handleSelect(command, text);
      }
   }

   // [tom, 8/21/2006] Cheesey hack to find the position based on ID
   char buf[512];
   MENUITEMINFOA mi;
   mi.cbSize = sizeof(mi);
   mi.dwTypeData = NULL;

   S32 numItems = GetMenuItemCount(mData->mMenu);
   S32 pos = -1;
   for(S32 i = 0;i < numItems;i++)
   {
      mi.fMask = MIIM_ID|MIIM_STRING|MIIM_STATE;
      if(GetMenuItemInfoA(mData->mMenu, i, TRUE, &mi))
      {
         if(mi.wID == command)
         {
            if(text == NULL)
            {
               mi.dwTypeData = buf;
               mi.cch++;
               GetMenuItemInfoA(mData->mMenu, i, TRUE, &mi);
               
               // [tom, 5/11/2007] Don't do anything if the menu item is disabled
               if(mi.fState & MFS_DISABLED)
                  return false;

               text = StringTable->insert(mi.dwTypeData);
            }
            pos = i;
            break;
         }
      }
   }

   if(pos == -1)
   {
      Con::errorf("PopupMenu::handleSelect - Could not find menu item position for ID %d ... this shouldn't happen!", command);
      return false;
   }



   // [tom, 8/20/2006] Wasn't handled by a submenu, pass off to script
   return dAtob(Con::executef(this, 4, "onSelectItem", Con::getIntArg(pos), text ? text : ""));
}

//////////////////////////////////////////////////////////////////////////

void PopupMenu::showPopup(S32 x /* = -1 */, S32 y /* = -1 */)
{
   POINT p;
   if(x == -1 && y == -1)
      GetCursorPos(&p);
   else
   {
      p.x = x;
      p.y = y;
      ClientToScreen(winState.appWindow, &p);
   }

   winState.renderThreadBlocked = true;
   U32 opt = (int)TrackPopupMenu(mData->mMenu, TPM_NONOTIFY|TPM_RETURNCMD, p.x, p.y, 0, winState.appWindow, NULL);
   if(opt > 0)
      handleSelect(opt, NULL);
   winState.renderThreadBlocked = false;
}

//////////////////////////////////////////////////////////////////////////

void PopupMenu::attachToMenuBar(S32 pos, const char *title)
{
   if(winState.appMenu == NULL)
   {
      CreateWin32MenuBar();
   }

   MENUITEMINFOA mii;

   mii.cbSize = sizeof(MENUITEMINFOA);

   mii.fMask = MIIM_STRING|MIIM_DATA;
   mii.dwTypeData = (LPSTR)title;
   mii.fMask |= MIIM_ID;
   mii.wID = mData->mMenuID;
   mii.fMask |= MIIM_SUBMENU;
   mii.hSubMenu = mData->mMenu;
   mii.dwItemData = (ULONG_PTR)this;

   InsertMenuItemA(winState.appMenu, pos, TRUE, &mii);
   DrawMenuBar(winState.appWindow);
}

void PopupMenu::removeFromMenuBar()
{
   // [tom, 8/21/2006] Find us on the menu bar (lame)
   S32 numItems = GetMenuItemCount(winState.appMenu);
   S32 pos = -1;
   for(S32 i = 0;i < numItems;i++)
   {
      MENUITEMINFOA mi;
      mi.cbSize = sizeof(mi);
      mi.fMask = MIIM_DATA;
      if(GetMenuItemInfoA(winState.appMenu, i, TRUE, &mi))
      {
         if(mi.fMask & MIIM_DATA)
         {
            PopupMenu *mnu = (PopupMenu *)mi.dwItemData;
            if(mnu == this)
            {
               pos = i;
               break;
            }
         }
      }
   }

   if(pos == -1)
      return;

   RemoveMenu(winState.appMenu, pos, MF_BYPOSITION);
   DrawMenuBar(winState.appWindow);
}
