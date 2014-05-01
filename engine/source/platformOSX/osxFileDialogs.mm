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

#import "platformOSX/platformOSX.h"
#import "platformOSX/osxCocoaUtilities.h"
#include "platform/nativeDialogs/fileDialog.h"
#include "io/resource/resourceManager.h"

//-----------------------------------------------------------------------------
bool FileDialog::Execute()
{
    NSArray* nsFileArray;

    if (mData.mStyle & FileDialogData::FDS_OPEN)
    {
        nsFileArray = [NSOpenPanel showOpenPanel:&mData];
    }
    else if (mData.mStyle & FileDialogData::FDS_SAVE)
    {
        nsFileArray = [NSSavePanel showSavePanel:&mData];
    }
    else
    {
        Con::errorf("Bad File Dialog Setup.");
        return false;
    }

    if ([nsFileArray count] < 1)
        return false;

    // If multiple file selection was allowed and the dialog did grab multiple files
    // loop through and add them
    if ((mData.mStyle & FileDialogData::FDS_MULTIPLEFILES) && [nsFileArray count] >= 1)
    {
        for(U32 i = 0; i < [nsFileArray count]; i++)
        {
            NSURL* fileURL = [nsFileArray objectAtIndex:i];

            const UTF8* file = [[fileURL path] UTF8String];
            setDataField(StringTable->insert("files"), Con::getIntArg(i), StringTable->insert(file));
        }

        setDataField(StringTable->insert("fileCount"), NULL, Con::getIntArg((S32)[nsFileArray count]));
    }
    else
    {
        NSURL* fileURL = [nsFileArray objectAtIndex:0];

        const UTF8* file = [[fileURL path] UTF8String];

        mData.mFile = StringTable->insert(file);
    }

    return true;
}


//-----------------------------------------------------------------------------
// Default Path Property - String Validated on Write
bool FileDialog::setDefaultPath(void* obj, const char* data)
{
    if (!data || !dStrncmp(data, "", 1))
        return true;

    // Expand the path to something fully qualified
    static char szPathValidate[512];

    Platform::makeFullPathName(data, szPathValidate, sizeof(szPathValidate));

    // Check to make sure the path is valid
    ResourceManager->addPath(szPathValidate, true);
    if (Platform::isDirectory(szPathValidate))
    {
        // Finally, assign in proper format.
        FileDialog *pDlg = static_cast<FileDialog*>(obj);
        pDlg->mData.mDefaultPath = StringTable->insert(szPathValidate);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Default Path Property - String Validated on Write
bool FileDialog::setDefaultFile(void* obj, const char* data)
{
    if (!data || !dStrncmp(data, "", 1))
        return true;

    // Copy and Backslash the path (Windows dialogs are VERY picky about this format)
    static char szPathValidate[512];
    Platform::makeFullPathName(data,szPathValidate, sizeof(szPathValidate));

    // Finally, assign in proper format.
    FileDialog *pDlg = static_cast<FileDialog*>(obj);
    pDlg->mData.mDefaultFile = StringTable->insert(szPathValidate);

    return false;
};

struct _NSStringMap
{
    S32 num;
    NSString* ok;
    NSString* cancel;
    NSString* third;
};

static _NSStringMap sgButtonTextMap[] =
{
        { MBOk,                 @"Ok",    nil,        nil },
        { MBOkCancel,           @"Ok",    @"Cancel",  nil },
        { MBRetryCancel,        @"Retry", @"Cancel",  nil },
        { MBSaveDontSave,       @"Yes",  @"No", nil },
        { MBSaveDontSaveCancel, @"Save",  @"Cancel",  @"Don't Save" },
        { -1, nil, nil, nil }
};

//-----------------------------------------------------------------------------
// Standard message box with a single "OK" button
void Platform::AlertOK(const char *windowTitle, const char *message)
{
    Platform::messageBox(windowTitle, message, MBOk, MIWarning);
}

//-----------------------------------------------------------------------------
// Displays an alert with Cancel and OK as buttons
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
    S32 result = Platform::messageBox(windowTitle, message, MBOkCancel, MIWarning);

    return (result == NSAlertFirstButtonReturn);
}

//-----------------------------------------------------------------------------
// Standard message box with Cancel and Retry buttons
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
    S32 result = Platform::messageBox(windowTitle, message, MBRetryCancel, MIWarning);

    return (result == NSAlertFirstButtonReturn);
}

//-----------------------------------------------------------------------------
// Standard message box with No and Yes buttons
bool Platform::AlertYesNo(const char *windowTitle, const char *message)
{
    S32 result = Platform::messageBox(windowTitle, message, MBSaveDontSave, MIWarning);

    return (result == NSAlertFirstButtonReturn);
}


//-----------------------------------------------------------------------------
// Responsible for creating, building, showing, and catching the return of
// a NSAlert. The messageBox ConsoleFunction calls into this
S32 Platform::messageBox(const UTF8 *title, const UTF8 *message, MBButtons buttons, MBIcons icon)
{
    NSString *okBtn      = nil;
    NSString *cancelBtn  = nil;
    NSString *thirdBtn   = nil;

    U32 i;

    for (i = 0; sgButtonTextMap[i].num != -1; i++)
    {
        if (sgButtonTextMap[i].num != buttons)
            continue;

        okBtn = sgButtonTextMap[i].ok;
        cancelBtn = sgButtonTextMap[i].cancel;
        thirdBtn = sgButtonTextMap[i].third;

        break;
    }

    if(sgButtonTextMap[i].num == -1)
        Con::errorf("Unknown message box button set requested. Mac Platform::messageBox() probably needs to be updated.");

    // convert title and message to NSStrings
    NSString *nsTitle = [NSString stringWithUTF8String:title];
    NSString *nsMessage = [NSString stringWithUTF8String:message];

    Input::setCursorShape(CursorManager::curIBeam);

    NSAlert *alert = [[[NSAlert alloc] init] autorelease];
    [alert setAlertStyle:NSWarningAlertStyle];
    [alert addButtonWithTitle:okBtn];
    [alert addButtonWithTitle:cancelBtn];
    [alert addButtonWithTitle:thirdBtn];
    [alert setMessageText:nsTitle];
    [alert setInformativeText:nsMessage];

    S32 result = (S32)[alert runModal];

    return result;
}
