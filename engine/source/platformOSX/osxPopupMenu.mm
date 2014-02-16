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
#include "platformOSX/platformOSX.h"
#include "platformOSX/osxCocoaUtilities.h"
#include "gui/guiCanvas.h"

class PlatformPopupMenuData
{
public:

    osxPopupMenuController* mController;
    S32 tag;

    PlatformPopupMenuData()
    {
        mController = NULL;

        tag = getTag();
    }

    ~PlatformPopupMenuData()
    {
        if (mController)
            [mController release];

        mController = NULL;
    }

    // We assign each new menu item an arbitrary integer tag.
    static S32 getTag()
    {
        static S32 lastTag = 'TORQ';
        return ++lastTag;
    }
};

//-----------------------------------------------------------------------------
// Allocates the OS X specific PopupMenuData
 void PopupMenu::createPlatformPopupMenuData()
{
    mData = new PlatformPopupMenuData;
}

//-----------------------------------------------------------------------------
// Deallocates the OS X specific PopupMenuData
void PopupMenu::deletePlatformPopupMenuData()
{
    SAFE_DELETE(mData);
}

//-----------------------------------------------------------------------------
// Called in PopupMenu's constructor, this handles platform specific menu setup
void PopupMenu::createPlatformMenu()
{
    mData->mController = [[osxPopupMenuController alloc] init];

    [mData->mController setOwner:this];
}

//-----------------------------------------------------------------------------
S32 PopupMenu::insertItem(S32 pos, const char *title, const char *accel)
{
    NSMenuItem *newItem;

    newItem = [[NSMenuItem allocWithZone:[NSMenu menuZone]]
            initWithTitle:[NSString stringWithUTF8String:title]
            action:NULL
            keyEquivalent:[NSString stringWithUTF8String:accel]];

    [newItem setTarget:mData->mController];
    [newItem setAction:@selector(handleSelect:)];

    [[mData->mController menu] insertItem:newItem atIndex:pos];

    [newItem release];

    return 0;
}

//-----------------------------------------------------------------------------
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

    NSMenuItem *newItem;

    newItem = [[NSMenuItem allocWithZone:[NSMenu menuZone]]
            initWithTitle:[NSString stringWithUTF8String:title] action:NULL
            keyEquivalent:[NSString stringWithUTF8String:""]];

    [newItem setSubmenu:[submenu->mData->mController menu]];
    [newItem setTarget:submenu->mData->mController];
    [newItem setAction:@selector(handleSelect:)];

    [[mData->mController menu] insertItem:newItem atIndex:pos];

    [newItem release];

    mSubmenus->addObject(submenu);

    return 0;
}

//-----------------------------------------------------------------------------
void PopupMenu::removeItem(S32 itemPos)
{
    [[mData->mController menu] removeItemAtIndex:itemPos];
}

//-----------------------------------------------------------------------------
void PopupMenu::enableItem(S32 pos, bool enable)
{
    [[[mData->mController menu] itemAtIndex:pos] setEnabled:enable];
}

//-----------------------------------------------------------------------------
void PopupMenu::checkItem(S32 pos, bool checked)
{
    [[[mData->mController menu] itemAtIndex:pos] setState:(checked ? NSOnState : NSOffState)];
}

//-----------------------------------------------------------------------------
void PopupMenu::checkRadioItem(S32 firstPos, S32 lastPos, S32 checkPos)
{
    for(int i = firstPos; i <= lastPos; i++)
        checkItem( i, false);

    // check the selected item
    checkItem( checkPos, true);
}

//-----------------------------------------------------------------------------
bool PopupMenu::isItemChecked(S32 pos)
{
    S32 state = (S32)[[[mData->mController menu] itemAtIndex:pos] state];

    return (state == NSOnState);
}

//-----------------------------------------------------------------------------
bool PopupMenu::handleSelect(U32 command, const char *text /* = NULL */)
{
    return dAtob(Con::executef(this, 4, "onSelectItem", Con::getIntArg(command), text ? text : ""));
}

//-----------------------------------------------------------------------------
// Not yet implemented or no longer necessary. Will resolve in the next platform update
void PopupMenu::showPopup(S32 x /* = -1 */, S32 y /* = -1 */)
{
    // Get the position of the cursor
    if(x < 0 || y < 0)
    {
        Point2I p = Canvas->getCursorPos();
        x = p.x;
        y = p.y;
    }

    // Convert to native coordinates
    //CGPoint native = MacCarbTorqueToNativeCoords(x, y);

    // Manually click the menu item
    //U32 result = PopUpMenuSelect(mData->mMenu, native.y, native.x, 0);
}

//-----------------------------------------------------------------------------
void PopupMenu::attachToMenuBar(S32 pos, const char *title)
{
    [[mData->mController menuItem] setTitle:[NSString stringWithUTF8String:title]];
    [[mData->mController menu] setTitle:[NSString stringWithUTF8String:title]];
    [[NSApp mainMenu] addItem:[mData->mController menuItem]];
}

//-----------------------------------------------------------------------------
void PopupMenu::removeFromMenuBar()
{
    if ([[NSApp mainMenu] indexOfItem:[mData->mController menuItem]] > -1)
        [[NSApp mainMenu] removeItem:[mData->mController menuItem]];
}