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
#include <unistd.h>
#include "platform/platform.h"
#include "console/console.h"
#include "string/stringTable.h"
#include "platform/platformInput.h"
#include "platform/threads/thread.h"
#include "platformAndroid/T2DActivity.h"

#pragma mark ---- Various Directories ----
//-----------------------------------------------------------------------------
const char* Platform::getUserDataDirectory() 
{
    //on android we will be using the cache dir to store user data
	return StringTable->insert(activity.getCacheDir());

}

//-----------------------------------------------------------------------------
const char* Platform::getUserHomeDirectory() 
{
	return StringTable->insert("~/");
}

//-----------------------------------------------------------------------------
StringTableEntry Platform::osGetTemporaryDirectory()
{
	//Android has no global temp folder, each application is expected to use cache dir
	return StringTable->insert(activity.getCacheDir());
}

//-----------------------------------------------------------------------------
S32 Platform::messageBox(const UTF8 *title, const UTF8 *message, MBButtons buttons, MBIcons icon)
{
    Platform::AlertOK( title, message );//<Mat> maybe add in a a way to do other buttons
    return 1;
}

#pragma mark ---- File IO ----
//-----------------------------------------------------------------------------
bool Platform::pathCopy(const char* source, const char* dest, bool nooverwrite)
{
	Con::errorf("pathCopy is not supported on Android due to assets being in a zip file");
	return false;
   
}

//-----------------------------------------------------------------------------
bool Platform::fileRename(const char *source, const char *dest)
{
	Con::errorf("fileRename is not supported on Android due to assets being in a zip file");
   return false;
}


#pragma mark ---- ShellExecute ----
class ExecuteThread : public Thread
{
   const char* zargs;
   const char* directory;
   const char* executable;
public:
   ExecuteThread(const char *_executable, const char *_args /* = NULL */, const char *_directory /* = NULL */) : Thread(0, NULL, false, true)
   {
      zargs = dStrdup(_args);
      directory = dStrdup(_directory);
      executable = dStrdup(_executable);
      start();
   }
   
   virtual void run(void* arg);
};

static char* _unDoubleQuote(char* arg)
{
   U32 len = dStrlen(arg);
   if(!len)
      return arg;

   if(arg[0] == '"' && arg[len-1] == '"')
   {
      arg[len - 1] = '\0';
      return arg + 1;
   }
   return arg;
}

void ExecuteThread::run(void* arg)
{

}

ConsoleFunction(shellExecute, bool, 2, 4, "(executable, [args], [directory])")
{
   return true; // Bug: BPNC error: need feedback on whether the command was sucessful
}


void Input::setCursorShape(U32 cursorID)
{
    //no cursors on Android except Torque cursors
}

void Input::setCursorState(bool on)
{
}

