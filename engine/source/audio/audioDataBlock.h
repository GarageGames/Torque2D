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

#ifndef _AUDIODATABLOCK_H_
#define _AUDIODATABLOCK_H_

#ifndef _PLATFORMAUDIO_H_
#include "platform/platformAudio.h"
#endif
#ifndef _AUDIOBUFFER_H_
#include "audio/audioBuffer.h"
#endif
#ifndef _BITSTREAM_H_
#include "io/bitStream.h"
#endif
#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

//--------------------------------------------------------------------------

class AudioEnvironment : public SimDataBlock
{
   typedef SimDataBlock Parent;

   public:

      bool  mUseRoom;
      S32   mRoom;
      S32   mRoomHF;
      S32   mReflections;
      S32   mReverb;
      F32   mRoomRolloffFactor;
      F32   mDecayTime;
      F32   mDecayHFRatio;
      F32   mReflectionsDelay;
      F32   mReverbDelay;
      S32   mRoomVolume;
      F32   mEffectVolume;
      F32   mDamping;
      F32   mEnvironmentSize;
      F32   mEnvironmentDiffusion;
      F32   mAirAbsorption;
      S32   mFlags;

      AudioEnvironment();

      static void initPersistFields();
      void packData(BitStream* stream);
      void unpackData(BitStream* stream);

      DECLARE_CONOBJECT(AudioEnvironment);
};
DECLARE_CONSOLETYPE(AudioEnvironment)

//--------------------------------------------------------------------------
class AudioSampleEnvironment : public SimDataBlock
{
   typedef SimDataBlock Parent;

   public:

      S32      mDirect;
      S32      mDirectHF;
      S32      mRoom;
      S32      mRoomHF;
      F32      mObstruction;
      F32      mObstructionLFRatio;
      F32      mOcclusion;
      F32      mOcclusionLFRatio;
      F32      mOcclusionRoomRatio;
      F32      mRoomRolloff;
      F32      mAirAbsorption;
      S32      mOutsideVolumeHF;
      S32      mFlags;

      AudioSampleEnvironment();
      static void initPersistFields();

      void packData(BitStream* stream);
      void unpackData(BitStream* stream);

      DECLARE_CONOBJECT(AudioSampleEnvironment);
};
DECLARE_CONSOLETYPE(AudioSampleEnvironment)

#endif  // _H_AUDIODATABLOCK_
