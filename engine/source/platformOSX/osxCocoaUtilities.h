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
#include "platform/nativeDialogs/fileDialog.h"
#include "memory/safeDelete.h"
#include "platform/menus/popupMenu.h"

class FileDialogFileExtList
{
public:
    Vector<UTF8*> list;
    UTF8* data;

    FileDialogFileExtList(const char* exts) { data = dStrdup(exts); }
    ~FileDialogFileExtList() { SAFE_DELETE(data); }
};

class FileDialogFileTypeList
{
public:
    UTF8* filterData;
    Vector<UTF8*> names;
    Vector<FileDialogFileExtList*> exts;
    bool any;

    FileDialogFileTypeList(const char* filter) { filterData = dStrdup(filter); any = false;}
    ~FileDialogFileTypeList()
    {
        SAFE_DELETE(filterData);
        for(U32 i = 0; i < exts.size(); i++)
            delete exts[i];
    }
};

@interface NSSavePanel (TorqueFileDialogs)
- (void)setFilters:(StringTableEntry)filters;
- (void)setAttributesFromData:(FileDialogData *)data;
- (FileDialogFileExtList *)getFileExtensionsFromString:(const char*) filter;
+ (NSArray *)showSavePanel:(FileDialogData *)withData;
@end

@interface NSOpenPanel (TorqueFileDialogs)
- (void)setAttributesFromData:(FileDialogData *)data;
+ (NSArray *)showOpenPanel:(FileDialogData *)withData;
@end

@interface osxPopupMenuController : NSObject
{
    NSMenu* _menu;
    NSMenuItem* _menuItem;
    PopupMenu* _owner;
}

@property (strong) NSMenu* menu;
@property (strong) NSMenuItem* menuItem;
@property (assign) PopupMenu* owner;

-(void)handleSelect:(id)sender;

@end