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

//
//  iOSUserMusicLibrary.mm
//  iTorque2D
//
//  Created by Sven Bergstrom on 2010/05/10.
//  Copyright 2010 Luma Arcade. All rights reserved.
//

#ifdef TORQUE_ALLOW_MUSICPLAYER

#import "iOSUserMusicLibrary.h"
#import "MediaPlayer/MPMusicPlayerController.h"

#include "console/console.h"
#include "console/consoleTypes.h"

// Global master volume for stock music player
static F32 gMusicVolume = 0.5; 

@interface iOSUserMusicLibrary : NSObject {
	
	MPMusicPlayerController* musicPlayer;
	
}

@property (nonatomic, retain) MPMusicPlayerController *musicPlayer;

@end


@implementation iOSUserMusicLibrary

	@synthesize musicPlayer;

@end


static iOSUserMusicLibrary* iT2dMusicManager;
static bool iOSMusicManagerIsActive = false;

void createMusicPlayer()
{
	// Specify a media query; this one matches the entire iPod library because it
	// does not contain a media property predicate
	MPMediaQuery *everything = [[MPMediaQuery alloc] init];
	
	// Configure the media query to group its media items; here, grouped by artist
	[everything setGroupingType: MPMediaGroupingArtist];
	
	iT2dMusicManager = [[[iOSUserMusicLibrary alloc] init] retain]; 
	iT2dMusicManager.musicPlayer = [MPMusicPlayerController iPodMusicPlayer];
	
	[iT2dMusicManager.musicPlayer setQueueWithQuery: everything];
	
	Con::printf("iOSCreateMusicPlayer success, music player is enabled.");
	
	iOSMusicManagerIsActive = true;
	iT2dMusicManager.musicPlayer.volume = gMusicVolume;
	
	Con::addVariable("iPodMusicVolume", TypeF32, &gMusicVolume);
	Con::setVariable("$iOSMusicTrack", "Now Playing: ");
}

void destroyMusicPlayer()
{
    iT2dMusicManager.musicPlayer = nil;
    [iT2dMusicManager release];
    iT2dMusicManager = nil;
    
    iOSMusicManagerIsActive = false;
}

void updateVolume()
{
	if(iOSMusicManagerIsActive)
	{
		// Set the iT2DMusicManager.musicPlayer volume property
		iT2dMusicManager.musicPlayer.volume = gMusicVolume;
	}
}

ConsoleFunction(iOSCreateMusicPlayer, void, 1, 1, "")
{
	if(!iOSMusicManagerIsActive)
	{
		createMusicPlayer();
	}
	else
	{
		Con::printf("iOSCreateMusicPlayer was already called.");
	}

}

ConsoleFunction(iOSMusicPlay, void, 1, 1, "iOSMusicPlay()")
{
	if(iOSMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer play];
		
		MPMediaItem *currentItem = [iT2dMusicManager.musicPlayer nowPlayingItem];
		
		// Display the artist and song name for the now-playing media item
		NSString* output = [
								   NSString stringWithFormat: @"%@ %@ %@ %@",
								   NSLocalizedString (@"Now Playing:", @"Label for introducing the now-playing song title and artist"),
								   [currentItem valueForProperty: MPMediaItemPropertyTitle],
								   NSLocalizedString (@"by", @"Article between song name and artist name"),
								   [currentItem valueForProperty: MPMediaItemPropertyArtist]];
		//Hand it off to the compass variable in torque.
		const char* tOutput = [output UTF8String];
		Con::printf(tOutput);
		Con::setVariable("$iOSMusicTrack", tOutput );
	}
	else 
	{
		Con::printf("iOSMusicPlayer is not active, did you call iOSCreateMusicPlayer(); ? ");
	}

}

ConsoleFunction(iOSMusicPause, void, 1, 1, "iOSMusicPause()")
{
	if(iOSMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer pause];		
	}
	else 
	{
		Con::printf("iOSMusicPlayer is not active, did you call iOSCreateMusicPlayer(); ? ");
	}
	
}

ConsoleFunction(iOSMusicStop, void, 1, 1, "iOSMusicStop()")
{
	if(iOSMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer stop];
	}
	else 
	{
		Con::printf("iOSMusicPlayer is not active, did you call iOSCreateMusicPlayer(); ? ");
	}
	
}

ConsoleFunction(iOSMusicNext, void, 1, 1, "iOSMusicNext()")
{
	if(iOSMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer skipToNextItem];
		MPMediaItem *currentItem = [iT2dMusicManager.musicPlayer nowPlayingItem];
		
		// Display the artist and song name for the now-playing media item
		NSString* output = [
							NSString stringWithFormat: @"%@ %@ %@ %@",
							NSLocalizedString (@"Now Playing:", @"Label for introducing the now-playing song title and artist"),
							[currentItem valueForProperty: MPMediaItemPropertyTitle],
							NSLocalizedString (@"by", @"Article between song name and artist name"),
							[currentItem valueForProperty: MPMediaItemPropertyArtist]];
		//Hand it off to the compass variable in torque.
		const char* tOutput = [output UTF8String];
		Con::printf(tOutput);
		Con::setVariable("$iOSMusicTrack", tOutput );
	}
	else 
	{
		Con::printf("iOSMusicPlayer is not active, did you call iOSCreateMusicPlayer(); ? ");
	}
	
}

ConsoleFunction(iOSMusicPrevious, void, 1, 1, "iOSMusicPrevious()")
{
	if(iOSMusicManagerIsActive)
	{
		[iT2dMusicManager.musicPlayer skipToPreviousItem];
		MPMediaItem *currentItem = [iT2dMusicManager.musicPlayer nowPlayingItem];
		
		// Display the artist and song name for the now-playing media item
		NSString* output = [
							NSString stringWithFormat: @"%@ %@ %@ %@",
							NSLocalizedString (@"Now Playing:", @"Label for introducing the now-playing song title and artist"),
							[currentItem valueForProperty: MPMediaItemPropertyTitle],
							NSLocalizedString (@"by", @"Article between song name and artist name"),
							[currentItem valueForProperty: MPMediaItemPropertyArtist]];
		//Hand it off to the compass variable in torque.
		const char* tOutput = [output UTF8String];
		Con::printf(tOutput);
		Con::setVariable("$iOSMusicTrack", tOutput );
	}
	else 
	{
		Con::printf("iOSMusicPlayer is not active, did you call iOSCreateMusicPlayer(); ? ");
	}
	
}

#endif //TORQUE_ALLOW_MUSICPLAYER
