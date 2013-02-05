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

/*
 ===== IMPORTANT =====
 This is sample code demonstrating API, technology or techniques in development.
 Although this sample code has been reviewed for technical accuracy, it is not
 final. Apple is supplying this information to help you plan for the adoption of
 the technologies and programming interfaces described herein. This information
 is subject to change, and software implemented based on this sample code should
 be tested with final operating system software and final documentation. Newer
 versions of this sample code may be provided with future seeds of the API or
 technology. For information about updates to this and other developer
 documentation, view the New & Updated sidebars in subsequent documentation
 seeds.
 =====================
 File: SoundEngine.h
 Abstract: This C API is a sound engine intended for games and applications that
 want to do more than casual UI sounds playback e.g. background music track,
 multiple sound effects, stereo panning... while ensuring low-latency response at
 the same time.

 Version: 1.1
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
 ("Apple") in consideration of your agreement to the following terms, and your
 use, installation, modification or redistribution of this Apple software
 constitutes acceptance of these terms.  If you do not agree with these terms,
 please do not use, install, modify or redistribute this Apple software.
 In consideration of your agreement to abide by the following terms, and subject
 to these terms, Apple grants you a personal, non-exclusive license, under
 Apple's copyrights in this original Apple software (the "Apple Software"), to
 use, reproduce, modify and redistribute the Apple Software, with or without
 modifications, in source and/or binary forms; provided that if you redistribute
 the Apple Software in its entirety and without modifications, you must retain
 this notice and the following text and disclaimers in all such redistributions
 of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may be used
 to endorse or promote products derived from the Apple Software without specific
 prior written permission from Apple.  Except as expressly stated in this notice,
 no other rights or licenses, express or implied, are granted by Apple herein,
 including but not limited to any patent rights that may be infringed by your
 derivative works or by other works in which the Apple Software may be
 incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
  WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
  COMBINATION WITH YOUR PRODUCTS.
  IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
  DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
  CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
  APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  Copyright (C) 2008 Apple Inc. All Rights Reserved.
  */

/*============================================================================= =====================
 SoundEngine.h
 =============================================================================== ===================*/

#ifndef SOUND_ENGINE_H
#define SOUND_ENGINE_H

extern"C"
{
namespace SoundEngine {
	
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioToolbox/AudioToolbox.h>
#include <OpenAL/al.h>

//============================================================================= =====================
//Sound Engine
//============================================================================= =====================
	
/*!
	 @enum SoundEngine error codes
	 @abstract   These are the error codes returned from the SoundEngine API.
	 @constant   kSoundEngineErrUnitialized 	 The SoundEngine has not been initialized. Use SoundEngine_Initialize().
	 @constant   kSoundEngineErrInvalidID 	 The specified EffectID was not found. This can occur if the effect has not been loaded, or
	 if an unloaded is trying to be accessed.
	 @constant   kSoundEngineErrFileNotFound 	 The specified file was not found.
	 @constant   kSoundEngineErrInvalidFileFormat 	 The format of the file is invalid. Effect data must be little-endian 8 or 16 bit LPCM.
	 @constant   kSoundEngineErrDeviceNotFound 	 The output device was not found.
 */
	
	enum {
		kSoundEngineErrUnitialized = 1,
		kSoundEngineErrInvalidID = 2,
		kSoundEngineErrFileNotFound = 3,
		kSoundEngineErrInvalidFileFormat = 4,
		kSoundEngineErrDeviceNotFound = 5
	};
	
	/*!
	 @function       SoundEngine_Initialize
	 @abstract       Initializes and sets up the sound engine. Calling after a previous initialize will
	 reset the state of the SoundEngine, with all previous effects and music tracks
	 erased. Note: This is not required, loading an effect or background track will 
	 initialize as necessary.
	 @param          inMixerOutputRate
	 & nbsp;          A Float32 that represents the output sample rate of the mixer unit. Setting this to 
	 0 will use the default rate (the sample rate of the device)
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_Initialize(Float32 inMixerOutputRate);
	
	/*!
	 @function       SoundEngine_Teardown
	 @abstract       Tearsdown the sound engine.
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_Teardown();
	
	/*!
	 @function       SoundEngine_SetMasterVolume
	 @abstract       Sets the overall volume of all sounds coming from the process
	 @param          inValue
	 & nbsp;          A Float32 that represents the level. The range is between 0.0 and 1.0 (inclusive).
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_SetMasterVolume(Float32 inValue);
	/*!
	 @function       SoundEngine_SetListenerPosition
	 @abstract       Sets the position of the listener in the 3D space
	 @param          inX
	 & nbsp;          A Float32 that represents the listener's position along the X axis.
	 @param          inY
	 & nbsp;          A Float32 that represents the listener's position along the Y axis.
	 @param          inZ
	 & nbsp;          A Float32 that represents the listener's position along the Z axis.
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_SetListenerPosition(Float32 inX, Float32 inY, Float32 inZ);
	
	/*!
	 @function       SoundEngine_SetListenerGain
	 @abstract       Sets the gain of the listener. Must be >= 0.0
	 @param          inValue
	 & nbsp;          A Float32 that represents the listener's gain
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_SetListenerGain(Float32 inValue);
	
	/*!
	 @function       SoundEngine_LoadBackgroundMusicTrack
	 @abstract       Tells the background music player which file to play
	 @param          inPath
	 & nbsp;          The absolute path to the file to play.
	 @param          inAddToQueue
	 & nbsp;          If true, file will be added to the current background music queue. If
	 false, queue will be cleared and only loop the specified file.
	 @param          inLoadAtOnce
	 & nbsp;          If true, file will be loaded completely into memory. If false, data will be 
	 streamed from the file as needed. For games without large memory pressure and/or
	 small background music files, this can save memory access and improve power efficiency
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_LoadBackgroundMusicTrack(const char* inPath, Boolean inAddToQueue, Boolean inLoadAtOnce);
	
	/*!
	 @function       SoundEngine_UnloadBackgroundMusicTrack
	 @abstract       Tells the background music player to release all resources and stop playing.
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_UnloadBackgroundMusicTrack();
	
	/*!
	 @function       SoundEngine_StartBackgroundMusic
	 @abstract       Tells the background music player to start playing.
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_StartBackgroundMusic();
	
	/*!
	 @function       SoundEngine_StopBackgroundMusic
	 @abstract       Tells the background music player to stop playing.
	 @param          inAddToQueue
	 & nbsp;          If true, playback will stop when all tracks have completed. If false, playback
	 will stop immediately.
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_StopBackgroundMusic(Boolean inStopAtEnd);
	
	/*!
	 @function       SoundEngine_SetBackgroundMusicVolume
	 @abstract       Sets the volume for the background music player
	 @param          inValue
	 & nbsp;          A Float32 that represents the level. The range is between 0.0 and 1.0 (inclusive).
	 @result         A OSStatus indicating success or failure.
	 */
	OSStatus  SoundEngine_SetBackgroundMusicVolume(Float32 inValue);
	
	//-Mat for loading caf files, this creates a soundeffect and sets outEffectID to it's alGenBuffer ID
	bool LoadSoundFile(const char *inFilePath, UInt32 *outEffectID);
}
}
#endif //SOUND_ENGINE_H
