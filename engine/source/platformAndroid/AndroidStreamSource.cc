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

#include "AndroidStreamSource.h"
#include "SoundEngine.h"

#define BUFFERSIZE 32768

AndroidStreamSource::AndroidStreamSource(const char *filename)  {
	this->registerObject();
	int len = dStrlen( filename );
	mFilename = new char[len + 1];
	dStrcpy( mFilename, filename );
	//SoundEngine::SoundEngine_LoadBackgroundMusicTrack( mFilename, true, false );
}

AndroidStreamSource::~AndroidStreamSource() {
	stop();
	delete [] mFilename;
	//SoundEngine::SoundEngine_UnloadBackgroundMusicTrack();
}

bool AndroidStreamSource::isPlaying() {
	return true;
}

bool AndroidStreamSource::start( bool loop ) {
	//TODO: android
	/*
	SoundEngine::SoundEngine_LoadBackgroundMusicTrack( mFilename, true, false );
	SoundEngine::SoundEngine_StartBackgroundMusic();
	if( !loop ) {
		//stop at end
		SoundEngine::SoundEngine_StopBackgroundMusic( true );
		Con::executef(1,"onAndroidStreamEnd");
	}
	*/
	return true;
}

bool AndroidStreamSource::stop() {
	//false == stop now
	//TODO: android
	/*
	SoundEngine::SoundEngine_StopBackgroundMusic( false );
	SoundEngine::SoundEngine_UnloadBackgroundMusicTrack();
	*/
	return true;
}
    
bool AndroidStreamSource::setVolume( F32 volume) {
	//TODO: android
	/*
    SoundEngine::SoundEngine_SetBackgroundMusicVolume(volume);
    */
    return true;
}
