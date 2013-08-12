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

#ifndef _AUDIOBUFFER_H_
#define _AUDIOBUFFER_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _PLATFORMAL_H_
#include "platform/platformAL.h"
#endif
#ifndef _RESMANAGER_H_
#include "io/resource/resourceManager.h"
#endif

//--------------------------------------------------------------------------

class AudioBuffer: public ResourceInstance
{
   friend class AudioThread;

private:
   StringTableEntry  mFilename;
   bool              mLoading;
   ALuint            malBuffer;

   bool readRIFFchunk(Stream &s, const char *seekLabel, U32 *size);
   bool readWAV(ResourceObject *obj);

public:
   AudioBuffer(StringTableEntry filename);
   ~AudioBuffer();
   ALuint getALBuffer();
   bool isLoading() {return(mLoading);}

   static Resource<AudioBuffer> find(const char *filename);
   static ResourceInstance* construct(Stream& stream);

};


#endif  // _H_AUDIOBUFFER_
