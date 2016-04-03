#ifndef _AUDIODESCRIPTION_H_
#include "audio/audioDescriptions.h"
#endif

#include "audio/audio.h"

#include "console/consoleTypes.h"
#include "platform/platformAL.h"

IMPLEMENT_CO_DATABLOCK_V1(AudioDescription);

AudioDescription::AudioDescription()
{
    mVolume = 1.f;  // 0-1    1=loudest volume
    mVolumeChannel = 1;
    mIsLooping = false;
    mIsStreaming = false;

    mIs3D = true; //The default class is used with SceneObject.Playsound

    mReferenceDistance = 25.f;
    mRollOffFactor = 5.0f;
    mMaxDistance = 50.f;

    mConeInsideAngle = 360;
    mConeOutsideAngle = 360;
    mConeOutsideVolume = 1.0f;
    mConeVector = { 0.f, 0.f, 1.f };

    // environment info
    mEnvironmentLevel = 0.f;
}

void AudioDescription::initPersistFields()
{
    Parent::initPersistFields();

    addField("Volume", TypeF32, Offset(mVolume, AudioDescription));
    addField("VolumeChannel", TypeS32, Offset(mVolumeChannel, AudioDescription));
    addField("isLooping", TypeBool, Offset(mIsLooping, AudioDescription));
    addField("isStreaming", TypeBool, Offset(mIsStreaming, AudioDescription));

    addField("is3D", TypeBool, Offset(mIs3D, AudioDescription));
    addField("ReferenceDistance", TypeF32, Offset(mReferenceDistance, AudioDescription));
    addField("RolloffFactor", TypeF32, Offset(mRollOffFactor, AudioDescription));
    addField("MaxDistance", TypeF32, Offset(mMaxDistance, AudioDescription));
    addField("ConeInsideAngle", TypeS32, Offset(mConeInsideAngle, AudioDescription));
    addField("ConeOutsideAngle", TypeS32, Offset(mConeOutsideAngle, AudioDescription));
    addField("ConeOutsideVolume", TypeF32, Offset(mConeOutsideVolume, AudioDescription));
    addField("ConeVector", TypeF32Vector, Offset(mConeVector, AudioDescription));
    addField("EnvironmentLevel", TypeF32, Offset(mEnvironmentLevel, AudioDescription));
}

//--------------------------------------------------------------------------
IMPLEMENT_CONSOLETYPE(AudioDescription)
IMPLEMENT_GETDATATYPE(AudioDescription)
IMPLEMENT_SETDATATYPE(AudioDescription)