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

#ifdef TORQUE_ALLOW_MOVIEPLAYER
//iOS media stuff
#include "iOSMoviePlayback.h"
#import "MediaPlayer/MPMoviePlayerController.h"
#import "MediaPlayer/MPMoviePlayerViewController.h"
#import "AudioToolbox/AudioServices.h"

//#include "audio/audio.h"

//#include "core/strings/stringFunctions.h"

//Torque Stuff
#include "console/console.h"

#include "platformiOS.h"
extern iOSPlatState platState;


///ObjC section below
//Declaring the interface here, so that i can use the header for inclusion in C++ code
@interface iOSMoviePlaybackDelegate : NSObject {
	
	MPMoviePlayerViewController* movieViewController;
	
}

@property (nonatomic, retain) MPMoviePlayerController*  movieController;

-(NSURL *) urlForLocalFile:(NSString*)fileLocation withFileType:(NSString*)fileType;
-(BOOL) playAVideo:(NSString*)fileName withFileType:(NSString*)fileType withScalingMode:(MPMovieScalingMode)scalingMode withControlStyle:(MPMovieControlStyle)controlStyle;
-(BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;

@end


@implementation iOSMoviePlaybackDelegate 

@synthesize movieController;

//Wanted to keep the ObjC stuff neat and not littered with C++, implemented the C++ inside the playVideo function instead (the one exposed to the engine).
-(NSURL *)urlForLocalFile:(NSString*)fileLocation withFileType:(NSString*)typeName
{
	NSURL* movieURL = nil;
	//Find the file location within our application bundle
        NSBundle *bundle = [NSBundle mainBundle];
        if (bundle)
        {
			//Actually, we need to use inDirectory or else it only looks in the toplevel directories.
			//Cut the file name from the end of the string
			NSString* fnameOnly = [fileLocation lastPathComponent];
			NSString* pathOnly = [fileLocation stringByDeletingLastPathComponent];
			
			//Ask the bundle for the path to the file
            NSString *moviePath = [bundle pathForResource:fnameOnly ofType:typeName inDirectory:pathOnly];
			
            if (moviePath)
            {
                movieURL = [NSURL fileURLWithPath:moviePath];
            }
        }
    
	//Check if its nil when you use it, if there was some kind of error
    return movieURL;
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
	return YES;
}

//This is the last step of the video call, the C++ hands in the values we want here (scaling etc). NO EXTENSION on the filename.
-(BOOL) playAVideo:(NSString*)fileName withFileType:fileType withScalingMode:(MPMovieScalingMode)scalingMode withControlStyle:(MPMovieControlStyle)controlStyle
{
	NSString* defaultFileType = fileType; 
	NSURL* filePath = [self urlForLocalFile:fileName withFileType:defaultFileType];
	if(filePath == nil)
	{
		return NO;
	}
	
	movieViewController = [[MPMoviePlayerViewController alloc] initWithContentURL: filePath];

	[[NSNotificationCenter defaultCenter]
			addObserver: self
			selector: @selector(moviePlaybackDidFinish:)
			name: MPMoviePlayerPlaybackDidFinishNotification 
			object: nil];
	
	//Disable audio session 
	AudioSessionSetActive(NO);
	//Audio::OpenALShutdown();
	
	[movieController setScalingMode:MPMovieScalingModeAspectFit];
	[movieController setControlStyle:MPMovieControlStyleNone];

	[platState.Window addSubview:movieViewController.view];
	
	//[movieViewController setControlStyle:MPMovieControlStyleNone];	
		
	[movieController prepareToPlay];
	[movieController play];
	//[[movieViewController moviePlayer] play];
	
	return YES;
}

//Notification of when the video has done playing now, and we can move on.
- (void) moviePlaybackDidFinish:(NSNotification*)notification
{
	//Remove our observer
	[[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:nil];
	[movieViewController.view removeFromSuperview];

	// Reactivate the current audio session
	AudioSessionSetActive(YES);
	//Audio::OpenALInit();
	
	//We can dealloc this whole delegate once we are done as the playback was asynchronous, which releases the movieController
	[self dealloc];
}

- (void)dealloc
{
	//clean up
	//[movieController release];
	[movieViewController release];
	[super dealloc];
}

@end



//C++ Section below
bool playMovie(const char* fileName, const char* extension, MPMovieScalingMode scalingMode, MPMovieControlStyle controlStyle)
{	
	if((fileName != NULL) && (strlen(fileName) > 0))
	{
		//For now we assume the m4v format, as that is how iTunes exports it. 
		// todo : make this try the other supported extensions, and or load without one. - Sven
		NSString* filePath = [[NSString alloc] initWithUTF8String:fileName];
		NSString* fileType = [[NSString alloc] initWithUTF8String:extension];
		
		iOSMoviePlaybackDelegate* playMovieDelegate = [iOSMoviePlaybackDelegate alloc];
		
		BOOL success = [playMovieDelegate playAVideo:filePath withFileType:fileType withScalingMode:scalingMode withControlStyle:controlStyle];
		
		if(success == NO)
		{
			Con::errorf("playiOSMovie : cannot find the file %s in the bundle", fileName);
		}
		
		[filePath release];
		
		//Continue with success
		return true;
	}
	
	return false;
}


//Make it accessible to scripts!
ConsoleFunction(playiOSMovie, bool, 5, 5, "playiOSMovie(%filename, %filetype, %scaleMode, %controlMode) Returns True if the file was found, false otherwise")
{
	MPMovieScalingMode scale = (MPMovieScalingMode)dAtoi(argv[3]);
	MPMovieControlStyle style = (MPMovieControlStyle)dAtoi(argv[4]);
	bool worked = playMovie(argv[1], argv[2], scale, style);
	return worked;
}

#endif //TORQUE_ALLOW_MOVIEPLAYER
