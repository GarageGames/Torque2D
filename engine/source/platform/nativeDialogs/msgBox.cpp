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

#include "platform/platform.h"
#include "console/console.h"

#include "platform/nativeDialogs/msgBox.h"

// these are the return values for message box dialog buttons
void initMessageBoxVars()
{
   Con::setIntVariable("$MROk",        MROk);
   Con::setIntVariable("$MRCancel",    MRCancel);
   Con::setIntVariable("$MRRetry",     MRRetry);
   Con::setIntVariable("$MRDontSave",  MRDontSave);
}

//////////////////////////////////////////////////////////////////////////

static EnumTable::Enums sgButtonEnums[] =
{
   { MBOk,                 "Ok" },
   { MBOkCancel,           "OkCancel" },
   { MBRetryCancel,        "RetryCancel" },
   { MBSaveDontSave,       "SaveDontSave" }, // maps to yes/no on win, to save/discard on mac.
   { MBSaveDontSaveCancel, "SaveDontSaveCancel" }, // maps to yes/no/cancel on win, to save/cancel/don'tsave on mac.
   { 0, NULL }
};

static EnumTable::Enums sgIconEnums[] =
{
   { MIInformation,        "Information" },// win: blue i, mac: app icon or talking head
   { MIWarning,            "Warning" },    // win & mac: yellow triangle with exclamation pt
   { MIStop,               "Stop" },       // win: red x, mac: app icon or stop icon, depending on version
   { MIQuestion,           "Question" },   // win: blue ?, mac: app icon
   { 0, NULL }
};

//////////////////////////////////////////////////////////////////////////

static S32 getIDFromName(EnumTable::Enums *table, const char *name, S32 def = -1)
{
   for(S32 i = 0;table[i].label != NULL;++i)
   {
      if(dStricmp(table[i].label, name) == 0)
         return table[i].index;
   }
   AssertWarn(false,"getIDFromName(): didn't find that name" );
   return def;
}

//-------------------------------------------------------------------------

#include "msgBox_ScriptBinding.h"

