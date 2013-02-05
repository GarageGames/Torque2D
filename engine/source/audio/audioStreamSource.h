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

//--------------------------------------------
// audioStreamSource.h
// header for streaming audio source
//
// Kurtis Seebaldt
//--------------------------------------------

#ifndef _AUDIOSTREAMSOURCE_H_
#define _AUDIOSTREAMSOURCE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _PLATFORMAUDIO_H_
#include "platform/platformAudio.h"
#endif
#ifndef _PLATFORMAL_H_
#include "platform/platformAL.h"
#endif
#ifndef _AUDIOBUFFER_H_
#include "audio/audioBuffer.h"
#endif
#ifndef _RESMANAGER_H_
#include "io/resource/resourceManager.h"
#endif

#define NUMBUFFERS 16

class AudioStreamSource
{
    public:
        //need this because subclasses are deleted through base pointer
        virtual ~AudioStreamSource() {}
        virtual bool initStream() = 0;
        virtual bool updateBuffers() = 0;
        virtual void freeStream() = 0;
      virtual F32 getElapsedTime() = 0;
      virtual F32 getTotalTime() = 0;
        //void clear();

        AUDIOHANDLE             mHandle;
        ALuint				    mSource;

        Audio::Description      mDescription;
        AudioSampleEnvironment *mEnvironment;

        Point3F                 mPosition;
        Point3F                 mDirection;
        F32                     mPitch;
        F32                     mScore;
        U32                     mCullTime;

        bool					bFinishedPlaying;
        bool					bIsValid;

#ifdef TORQUE_OS_LINUX
                void checkPosition();
#endif

    protected:
        const char* mFilename;
};

#endif // _AUDIOSTREAMSOURCE_H_
