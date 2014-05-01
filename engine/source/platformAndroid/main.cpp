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
#include "platformAndroid/platformAndroid.h"
#include "platformAndroid/AndroidEvents.h"
#include "platformAndroid/AndroidUtil.h"
#include "platform/threads/thread.h"
#include "game/gameInterface.h"
#include "io/fileObject.h"

extern void clearPendingMultitouchEvents( void );

S32 gLastStart = 0;

bool appIsRunning = true;

int _AndroidRunTorqueMain(engine *eng)
{
    platState.firstThreadId = ThreadManager::getCurrentThreadId();
    
	printf("performing mainInit()\n");
    
	platState.lastTimeTick = Platform::getRealMilliseconds();
    
	if(!Game->mainInitialize(platState.argc, platState.argv))
	{
		return 0;
	}

	return 1;
}


void _AndroidGameInnerLoop()
{
    if (!appIsRunning)
    {
        return;
    }
    else if (Game->isRunning())
    {
		S32 start = Platform::getRealMilliseconds();
		
        Game->mainLoop();
				
        gLastStart = start;
        
	}
	else
	{
		Game->mainShutdown();
        
		// Need to actually exit the application now
		exit(0);
	}
}

void _AndroidGameResignActive()
{
    if ( Con::isFunction("onAndroidResignActive") )
        Con::executef( 1, "onAndroidResignActive" );
    
    appIsRunning = false;
}

void _AndroidGameBecomeActive()
{
	clearPendingMultitouchEvents( );
    
    if ( Con::isFunction("onAndroidBecomeActive") )
        Con::executef( 1, "onAndroidBecomeActive" );
    
    appIsRunning = true;
}

void _AndroidGameWillTerminate()
{
    if ( Con::isFunction("onAndroidWillTerminate") )
        Con::executef( 1, "onAndroidWillTerminate" );
    
	Con::executef( 1, "onExit" );
    
	Game->mainShutdown();
}

static void _AndroidGetTxtFileArgs(int &argc, char** argv, int maxargc)
{
    argc = 0;
    
    const U32 kMaxTextLen = 2048;
    
    U32 textLen;
    
    char* text = new char[kMaxTextLen];
    
    // Open the file, kick out if we can't
    File cmdfile;
    
    File::Status err = cmdfile.open("AndroidCmdLine.txt", cmdfile.Read);
    
    // Re-organise function to handle memory deletion better
    if (err == File::Ok)
    {
        // read in the first kMaxTextLen bytes, kick out if we get errors or no data
        err = cmdfile.read(kMaxTextLen-1, text, &textLen);
        
        if (((err == File::Ok || err == File::EOS) || textLen > 0))
        {
            // Null terminate
            text[textLen++] = '\0';
            
            // Truncate to the 1st line of the file
            for(int i = 0; i < textLen; i++)
            {
                if( text[i] == '\n' || text[i] == '\r' )
                {
                    text[i] = '\0';
                    textLen = i+1;
                    break;
                }
            }
            
            // Tokenize the args with nulls, save them in argv, count them in argc
            char* tok;
            
            for(tok = dStrtok(text, " "); tok && argc < maxargc; tok = dStrtok(NULL, " "))
                argv[argc++] = tok;
		}
	}
	
	// Close file and delete memory before returning
    cmdfile.close();
    
	delete[] text;
    
	text = NULL;
}

