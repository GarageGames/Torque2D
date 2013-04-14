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

#import "platformiOS/T2DAppDelegate.h"

#include "platform/platformInput.h"
#include "platformiOS/iOSUtil.h"
#include "console/console.h"

extern void _iOSGameInnerLoop();
extern void _iOSGameResignActive();
extern void _iOSGameBecomeActive();
extern void _iOSGameWillTerminate();

// Store current orientation for easy access
extern void _iOSGameChangeOrientation(S32 newOrientation);
UIDeviceOrientation currentOrientation;

bool _iOSTorqueFatalError = false;

@implementation T2DAppDelegate

@synthesize window = _window;


- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	//Also we set the currentRotation up so its not invalid
	currentOrientation = [UIDevice currentDevice].orientation;
	//So we make a selector to handle that, called didRotate (lower down in the code)
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(didRotate:)
												 name:UIDeviceOrientationDidChangeNotification
											   object:nil];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    _iOSGameResignActive();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    if(!_iOSTorqueFatalError)
        _iOSGameBecomeActive();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    _iOSGameWillTerminate();
	
	[[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
}

- (void)didRotate:(NSNotification *)notification
{
    //Default to landscape left
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	if(currentOrientation != orientation)
	{
		//Change the orientation
		currentOrientation = orientation;
		//Tell the rest of the engine
		_iOSGameChangeOrientation(currentOrientation);
	}
}

- (void) runMainLoop
{
	_iOSGameInnerLoop();
}

@end