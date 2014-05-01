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

extern void android_LoadMusicTrack( const char *mFilename );
extern void android_UnLoadMusicTrack();
extern bool android_isMusicTrackPlaying();
extern void android_StartMusicTrack();
extern void android_StopMusicTrack();
extern void android_setMusicTrackVolume(F32 volume);

AndroidStreamSource::AndroidStreamSource(const char *filename)  {
	this->registerObject();
	int len = dStrlen( filename );
	mFilename = new char[len + 1];
	dStrcpy( mFilename, filename );
}

AndroidStreamSource::~AndroidStreamSource() {
	stop();
	delete [] mFilename;
	android_UnLoadMusicTrack();
}

bool AndroidStreamSource::isPlaying() {
	return android_isMusicTrackPlaying();
}

bool AndroidStreamSource::start( bool loop ) {

	Con::printf("Starting music file: %s", mFilename);
	android_LoadMusicTrack( mFilename );
	android_StartMusicTrack();
	return true;
}

bool AndroidStreamSource::stop() {

	android_StopMusicTrack();
	android_UnLoadMusicTrack();

	return true;
}
    
bool AndroidStreamSource::setVolume( F32 volume) {

	android_setMusicTrackVolume(volume);

    return true;
}
AndroidStreamSource* stream = NULL;
ConsoleFunction(playStreamingMusic, void, 2, 2, "")
{
	if (stream != NULL)
		delete stream;

	stream = new AndroidStreamSource(argv[1]);
	stream->start();
}
