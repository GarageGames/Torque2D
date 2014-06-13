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

#include "audio/audioDataBlock.h"
#include "console/consoleTypes.h"
#include "platform/platformAL.h"
#include "network/netConnection.h"

//--------------------------------------------------------------------------
namespace
{
   void writeRangedF32(BitStream * bstream, F32 val, F32 min, F32 max, U32 numBits)
   {
      val = (mClampF(val, min, max) - min) / (max - min);
      bstream->writeInt((S32)(val * ((1 << numBits) - 1)), numBits);
   }

   F32 readRangedF32(BitStream * bstream, F32 min, F32 max, U32 numBits)
   {
      return(min + (F32(bstream->readInt(numBits)) / F32((1 << numBits) - 1)) * (max - min));
   }

   void writeRangedS32(BitStream * bstream, S32 val, S32 min, S32 max)
   {
      bstream->writeRangedU32((val - min), 0, (max - min));
   }

   S32 readRangedS32(BitStream * bstream, S32 min, S32 max)
   {
      return(bstream->readRangedU32(0, (max - min)) + min);
   }
}

//--------------------------------------------------------------------------
// Class AudioEnvironment:
//--------------------------------------------------------------------------
IMPLEMENT_CO_DATABLOCK_V1(AudioEnvironment);

AudioEnvironment::AudioEnvironment()
{
   mUseRoom = true;
#if !defined(TORQUE_OS_IOS) && !defined(TORQUE_OS_ANDROID)
   mRoom = EAX_ENVIRONMENT_GENERIC;
#endif
   mRoomHF = 0;
   mReflections = 0;
   mReverb = 0;
   mRoomRolloffFactor = 0.1f;
   mDecayTime = 0.1f;
   mDecayHFRatio = 0.1f;
   mReflectionsDelay = 0.f;
   mReverbDelay = 0.f;
   mRoomVolume = 0;
   mEffectVolume = 0.f;
   mDamping = 0.f;
   mEnvironmentSize = 10.f;
   mEnvironmentDiffusion = 1.f;
   mAirAbsorption = 0.f;
   mFlags = 0;
}

#if !defined(TORQUE_OS_IOS) && !defined(TORQUE_OS_ANDROID)
static EnumTable::Enums roomEnums[] =
{
   { EAX_ENVIRONMENT_GENERIC,           "GENERIC" },               // 0
   { EAX_ENVIRONMENT_PADDEDCELL,        "PADDEDCELL" },
   { EAX_ENVIRONMENT_ROOM,              "ROOM" },
   { EAX_ENVIRONMENT_BATHROOM,          "BATHROOM" },
   { EAX_ENVIRONMENT_LIVINGROOM,        "LIVINGROOM" },
   { EAX_ENVIRONMENT_STONEROOM,         "STONEROOM" },             // 5
   { EAX_ENVIRONMENT_AUDITORIUM,        "AUDITORIUM" },
   { EAX_ENVIRONMENT_CONCERTHALL,       "CONCERTHALL" },
   { EAX_ENVIRONMENT_CAVE,              "CAVE" },
   { EAX_ENVIRONMENT_ARENA,             "ARENA" },
   { EAX_ENVIRONMENT_HANGAR,            "HANGAR" },                // 10
   { EAX_ENVIRONMENT_CARPETEDHALLWAY,   "CARPETEDHALLWAY" },
   { EAX_ENVIRONMENT_HALLWAY,           "HALLWAY" },
   { EAX_ENVIRONMENT_STONECORRIDOR,     "STONECORRIDOR" },
   { EAX_ENVIRONMENT_ALLEY,             "ALLEY" },
   { EAX_ENVIRONMENT_FOREST,            "FOREST" },                // 15
   { EAX_ENVIRONMENT_CITY,              "CITY" },
   { EAX_ENVIRONMENT_MOUNTAINS,         "MOUNTAINS" },
   { EAX_ENVIRONMENT_QUARRY,            "QUARRY" },
   { EAX_ENVIRONMENT_PLAIN,             "PLAIN" },
   { EAX_ENVIRONMENT_PARKINGLOT,        "PARKINGLOT" },            // 20
   { EAX_ENVIRONMENT_SEWERPIPE,         "SEWERPIPE" },
   { EAX_ENVIRONMENT_UNDERWATER,        "UNDERWATER" },
   { EAX_ENVIRONMENT_DRUGGED,           "DRUGGED" },
   { EAX_ENVIRONMENT_DIZZY,             "DIZZY" },
   { EAX_ENVIRONMENT_PSYCHOTIC,         "PSYCHOTIC" }              // 25
};
static EnumTable gAudioEnvironmentRoomTypes(sizeof(roomEnums) / sizeof(roomEnums[0]), &roomEnums[0]);
#endif

//--------------------------------------------------------------------------
IMPLEMENT_CONSOLETYPE(AudioEnvironment)
IMPLEMENT_GETDATATYPE(AudioEnvironment)
IMPLEMENT_SETDATATYPE(AudioEnvironment)

void AudioEnvironment::initPersistFields()
{
   Parent::initPersistFields();

   addField("useRoom",              TypeBool,   Offset(mUseRoom, AudioEnvironment));
#if !defined(TORQUE_OS_IOS) && !defined(TORQUE_OS_ANDROID)
   addField("room",                 TypeEnum,   Offset(mRoom, AudioEnvironment), 1, &gAudioEnvironmentRoomTypes);
#endif
   addField("roomHF",               TypeS32,    Offset(mRoomHF, AudioEnvironment));
   addField("reflections",          TypeS32,    Offset(mReflections, AudioEnvironment));
   addField("reverb",               TypeS32,    Offset(mReverb, AudioEnvironment));
   addField("roomRolloffFactor",    TypeF32,    Offset(mRoomRolloffFactor, AudioEnvironment));
   addField("decayTime",            TypeF32,    Offset(mDecayTime, AudioEnvironment));
   addField("decayHFRatio",         TypeF32,    Offset(mDecayHFRatio, AudioEnvironment));
   addField("reflectionsDelay",     TypeF32,    Offset(mReflectionsDelay, AudioEnvironment));
   addField("reverbDelay",          TypeF32,    Offset(mReverbDelay, AudioEnvironment));
   addField("roomVolume",           TypeS32,    Offset(mRoomVolume, AudioEnvironment));
   addField("effectVolume",         TypeF32,    Offset(mEffectVolume, AudioEnvironment));
   addField("damping",              TypeF32,    Offset(mDamping, AudioEnvironment));
   addField("environmentSize",      TypeF32,    Offset(mEnvironmentSize, AudioEnvironment));
   addField("environmentDiffusion", TypeF32,    Offset(mEnvironmentDiffusion, AudioEnvironment));
   addField("airAbsorption",        TypeF32,    Offset(mAirAbsorption, AudioEnvironment));
   addField("flags",                TypeS32,    Offset(mFlags, AudioEnvironment));
}


void AudioEnvironment::packData(BitStream* stream)
{
   Parent::packData(stream);
#if !defined(TORQUE_OS_IOS)  && !defined(TORQUE_OS_ANDROID)
   if(stream->writeFlag(mUseRoom))
      stream->writeRangedU32(mRoom, EAX_ENVIRONMENT_GENERIC, EAX_ENVIRONMENT_COUNT);
   else
#endif
   {
      writeRangedS32(stream, mRoomHF, -10000, 0);
      writeRangedS32(stream, mReflections, -10000, 10000);
      writeRangedS32(stream, mReverb, -10000, 2000);
      writeRangedF32(stream, mRoomRolloffFactor, 0.1f, 10.f, 8);
      writeRangedF32(stream, mDecayTime, 0.1f, 20.f, 8);
      writeRangedF32(stream, mDecayHFRatio, 0.1f, 20.f, 8);
      writeRangedF32(stream, mReflectionsDelay, 0.f, 0.3f, 9);
      writeRangedF32(stream, mReverbDelay, 0.f, 0.1f, 7);
      writeRangedS32(stream, mRoomVolume, -10000, 0);
      writeRangedF32(stream, mEffectVolume, 0.f, 1.f, 8);
      writeRangedF32(stream, mDamping, 0.f, 2.f, 9);
      writeRangedF32(stream, mEnvironmentSize, 1.f, 100.f, 10);
      writeRangedF32(stream, mEnvironmentDiffusion, 0.f, 1.f, 8);
      writeRangedF32(stream, mAirAbsorption, -100.f, 0.f, 10);
      stream->writeInt(mFlags, 6);
   }
}

void AudioEnvironment::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   mUseRoom = stream->readFlag();
#if !defined(TORQUE_OS_IOS)  && !defined(TORQUE_OS_ANDROID)
   if(mUseRoom)
      mRoom = stream->readRangedU32(EAX_ENVIRONMENT_GENERIC, EAX_ENVIRONMENT_COUNT);
   else
#endif
   {
      mRoomHF = readRangedS32(stream, -10000, 0);
      mReflections = readRangedS32(stream, -10000, 10000);
      mReverb = readRangedS32(stream, -10000, 2000);
      mRoomRolloffFactor = readRangedF32(stream, 0.1f, 10.f, 8);
      mDecayTime = readRangedF32(stream, 0.1f, 20.f, 8);
      mDecayHFRatio = readRangedF32(stream, 0.1f, 20.f, 8);
      mReflectionsDelay = readRangedF32(stream, 0.f, 0.3f, 9);
      mReverbDelay = readRangedF32(stream, 0.f, 0.1f, 7);
      mRoomVolume = readRangedS32(stream, -10000, 0);
      mEffectVolume = readRangedF32(stream, 0.f, 1.f, 8);
      mDamping = readRangedF32(stream, 0.f, 2.f, 9);
      mEnvironmentSize = readRangedF32(stream, 1.f, 100.f, 10);
      mEnvironmentDiffusion = readRangedF32(stream, 0.f, 1.f, 8);
      mAirAbsorption = readRangedF32(stream, -100.f, 0.f, 10);
      mFlags = stream->readInt(6);
   }
}

//--------------------------------------------------------------------------
// Class AudioEnvironmentProfile:
//--------------------------------------------------------------------------
IMPLEMENT_CO_DATABLOCK_V1(AudioSampleEnvironment);

AudioSampleEnvironment::AudioSampleEnvironment()
{
   mDirect = 0;
   mDirectHF = 0;
   mRoom = 0;
   mRoomHF = 0;
   mObstruction = 0.f;
   mObstructionLFRatio = 0.f;
   mOcclusion = 0.f;
   mOcclusionLFRatio = 0.f;
   mOcclusionRoomRatio = 0.f;
   mRoomRolloff = 0.f;
   mAirAbsorption = 0.f;
   mOutsideVolumeHF = 0;
   mFlags = 0;
}

//--------------------------------------------------------------------------
IMPLEMENT_CONSOLETYPE(AudioSampleEnvironment)
IMPLEMENT_GETDATATYPE(AudioSampleEnvironment)
IMPLEMENT_SETDATATYPE(AudioSampleEnvironment)

void AudioSampleEnvironment::initPersistFields()
{
   Parent::initPersistFields();

   addField("direct",              TypeS32,    Offset(mDirect, AudioSampleEnvironment));
   addField("directHF",            TypeS32,    Offset(mDirectHF, AudioSampleEnvironment));
   addField("room",                TypeS32,    Offset(mRoom, AudioSampleEnvironment));
   addField("obstruction",         TypeF32,    Offset(mObstruction, AudioSampleEnvironment));
   addField("obstructionLFRatio",  TypeF32,    Offset(mObstructionLFRatio, AudioSampleEnvironment));
   addField("occlusion",           TypeF32,    Offset(mOcclusion, AudioSampleEnvironment));
   addField("occlusionLFRatio",    TypeF32,    Offset(mOcclusionLFRatio, AudioSampleEnvironment));
   addField("occlusionRoomRatio",  TypeF32,    Offset(mOcclusionRoomRatio, AudioSampleEnvironment));
   addField("roomRolloff",         TypeF32,    Offset(mRoomRolloff, AudioSampleEnvironment));
   addField("airAbsorption",       TypeF32,    Offset(mAirAbsorption, AudioSampleEnvironment));
   addField("outsideVolumeHF",     TypeS32,    Offset(mOutsideVolumeHF, AudioSampleEnvironment));
   addField("flags",               TypeS32,    Offset(mFlags, AudioSampleEnvironment));
}

void AudioSampleEnvironment::packData(BitStream* stream)
{
   Parent::packData(stream);
   writeRangedS32(stream, mDirect, -10000, 1000);
   writeRangedS32(stream, mDirectHF, -10000, 0);
   writeRangedS32(stream, mRoom, -10000, 1000);
   writeRangedS32(stream, mRoomHF, -10000, 0);
   writeRangedF32(stream, mObstruction, 0.f, 1.f, 9);
   writeRangedF32(stream, mObstructionLFRatio, 0.f, 1.f, 8);
   writeRangedF32(stream, mOcclusion, 0.f, 1.f, 9);
   writeRangedF32(stream, mOcclusionLFRatio, 0.f, 1.f, 8);
   writeRangedF32(stream, mOcclusionRoomRatio, 0.f, 10.f, 9);
   writeRangedF32(stream, mRoomRolloff, 0.f, 10.f, 9);
   writeRangedF32(stream, mAirAbsorption, 0.f, 10.f, 9);
   writeRangedS32(stream, mOutsideVolumeHF, -10000, 0);
   stream->writeInt(mFlags, 3);
}

void AudioSampleEnvironment::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   mDirect = readRangedS32(stream, -10000, 1000);
   mDirectHF = readRangedS32(stream, -10000, 0);
   mRoom = readRangedS32(stream, -10000, 1000);
   mRoomHF = readRangedS32(stream, -10000, 0);
   mObstruction = readRangedF32(stream, 0.f, 1.f, 9);
   mObstructionLFRatio = readRangedF32(stream, 0.f, 1.f, 8);
   mOcclusion = readRangedF32(stream, 0.f, 1.f, 9);
   mOcclusionLFRatio = readRangedF32(stream, 0.f, 1.f, 8);
   mOcclusionRoomRatio = readRangedF32(stream, 0.f, 10.f, 9);
   mRoomRolloff = readRangedF32(stream, 0.f, 10.f, 9);
   mAirAbsorption = readRangedF32(stream, 0.f, 10.f, 9);
   mOutsideVolumeHF = readRangedS32(stream, -10000, 0);
   mFlags = stream->readInt(3);
}

