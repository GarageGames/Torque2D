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

#import <UIKit/UIKit.h>

#import "platformiOS/platformiOS.h"
#import "platformiOS/T2DAppDelegate.h"
#import "platformiOS/iOSEvents.h"
#import "platformiOS/iOSUtil.h"
#include "platform/threads/thread.h"
#include "game/gameInterface.h"
#include "io/fileObject.h"

extern void clearPendingMultitouchEvents( void );

S32 gLastStart = 0;

bool appIsRunning = true;

int _iOSRunTorqueMain( id appID, UIView * Window, T2DViewController *viewController)
{
    UIApplication *app = [UIApplication sharedApplication];
    platState.viewController = viewController;
    
	platState.appID = appID;
	platState.firstThreadId = ThreadManager::getCurrentThreadId();
	platState.Window = Window;
	platState.application = app;
	
	// Hidden by default.
	platState.application.statusBarHidden = YES;
	
	printf("performing mainInit()\n");
    
	platState.lastTimeTick = Platform::getRealMilliseconds();
    
	if(!Game->mainInitialize(platState.argc, platState.argv))
	{
		return 0;
	}

    return true;
}

void _iOSGameInnerLoop()
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

void _iOSGameResignActive()
{
    if ( Con::isFunction("oniOSResignActive") )
        Con::executef( 1, "oniOSResignActive" );
    
    appIsRunning = false;
}

void _iOSGameBecomeActive()
{
	clearPendingMultitouchEvents( );
    
    if ( Con::isFunction("oniOSBecomeActive") )
        Con::executef( 1, "oniOSBecomeActive" );
    
    appIsRunning = true;
}

void _iOSGameWillTerminate()
{
    if ( Con::isFunction("oniOSWillTerminate") )
        Con::executef( 1, "oniOSWillTerminate" );
    
	Con::executef( 1, "onExit" );
    
	Game->mainShutdown();
}

// Store current orientation for easy access
void _iOSGameChangeOrientation(S32 newOrientation)
{    
	_iOSGameSetCurrentOrientation(newOrientation);
    
    return;
    
    bool enableAutoOrientation = Con::getBoolVariable("$pref::iOS::EnableOrientationRotation");
    int screenOrientation = Con::getIntVariable("$pref::iOS::ScreenOrientation");
    bool allowOtherOrientation = Con::getBoolVariable("$pref::iOS::EnableOtherOrientationRotation");

    // The rotation matching the project orientation must be allowed for any to occur
    if (enableAutoOrientation)
    {
        // Start "collecting animations"
        [UIView beginAnimations: nil context: nil];
        
        //  If the project is designed for landscape or it allows landscape rotation
        if (screenOrientation == 0 || allowOtherOrientation)
        {
            if (newOrientation == UIDeviceOrientationLandscapeLeft)
            {
                platState.Window.transform = CGAffineTransformMakeRotation(M_PI*1.5);
                Con::executef(1, "oniOSOrientationToLandscapeLeft");
                //  Show animations
                [UIView commitAnimations];
                
                return;
            }
            
            if (newOrientation == UIDeviceOrientationLandscapeRight)
            {
                platState.Window.transform = CGAffineTransformMakeRotation(M_PI_2);
                Con::executef(1, "oniOSOrientationToLandscapeRight");
                //  Show animations
                [UIView commitAnimations];
                
                return;
            }
        }
        
        // If the project is designed for portrait or it allows portrait rotation
        if (screenOrientation == 1 ||  allowOtherOrientation)
        {
            if (newOrientation == UIDeviceOrientationPortrait)
            {
                platState.Window.transform = CGAffineTransformMakeRotation(M_PI*1.5);
                Con::executef(1, "oniOSOrientationToPortrait");
                //  Show animations
                [UIView commitAnimations];
                
                return;
            }
            
            if (newOrientation == UIDeviceOrientationPortraitUpsideDown)
            {
                platState.Window.transform = CGAffineTransformMakeRotation(M_PI_2);
                Con::executef(1, "oniOSOrientationToPortraitUpsideDown");
                //  Show animations
                [UIView commitAnimations];
                
                return;
            }
        }
        
        // Show animations
        [UIView commitAnimations];
    }
}

static void _iOSGetTxtFileArgs(int &argc, char** argv, int maxargc)
{
    argc = 0;
    
    const U32 kMaxTextLen = 2048;
    
    U32 textLen;
    
    char* text = new char[kMaxTextLen];
    
    // Open the file, kick out if we can't
    File cmdfile;
    
    File::Status err = cmdfile.open("iOSCmdLine.txt", cmdfile.Read);
    
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

int main(int argc, char *argv[])
{
    @autoreleasepool
    {
        int kMaxCmdlineArgs = 32; //Arbitrary
        
        printf("Initial Command Line\n");
        
        for( int i = 0; i < argc; i++ )
        {
            printf("%i : %s", i, argv[i] );
        }
        
        NSString *nsStrVersion = [UIDevice currentDevice ].systemVersion;
        const char *strVersion = [nsStrVersion UTF8String ];
        platState.osVersion = dAtof( strVersion);
        
        // Find Main.cs .
        const char* cwd = Platform::getMainDotCsDir();
        
        // Change to the directory that contains main.cs
        Platform::setCurrentDirectory(cwd);
        
        // get the actual command line args
        S32   newArgc = argc;
        
        const char* newArgv[kMaxCmdlineArgs];
        
        for(int i=0; i < argc && i < kMaxCmdlineArgs; i++)
            newArgv[i] = argv[i];
        
        // get the text file args
        S32 textArgc;
        char* textArgv[kMaxCmdlineArgs];
        
        _iOSGetTxtFileArgs(textArgc, textArgv, kMaxCmdlineArgs);
        
        // merge them
        int i=0;
        
        while(i < textArgc && newArgc < kMaxCmdlineArgs)
            newArgv[newArgc++] = textArgv[i++];
        
        // store them in platState
        platState.argc = newArgc;
        platState.argv = newArgv;
        
        printf("\nMerged Command Line\n");
        
        for( int i = 0; i < platState.argc; i++ )
        {
            printf("%i : %s", i, platState.argv[i] );
        }
        
        printf("\n");
        
        // now, we run UIApplication which calls back and starts thread or timer
        platState.appReturn = UIApplicationMain(argc, argv, nil, NSStringFromClass([T2DAppDelegate class]));
        
        printf("exiting...\n");
        return(platState.appReturn);
    }
}
