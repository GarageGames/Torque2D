#ifndef _AUDIO_EMITTER_H
#include "AudioEmitter.h"
#endif

#include "AudioEmitter_ScriptBinding.h"


IMPLEMENT_CONOBJECT(AudioEmitter);

AudioEmitter::AudioEmitter()
{
   mDescription.mVolume              = 1.0f;
   mDescription.mVolumeChannel       = 0;
   mDescription.mIsLooping           = false;
   mDescription.mIsStreaming		 = false;

   mDescription.mIs3D                = false;
   mDescription.mReferenceDistance   = 1.0f;
   mDescription.mMaxDistance         = 100.0f;
   mDescription.mEnvironmentLevel    = 0.0f;
   mDescription.mConeInsideAngle     = 360;
   mDescription.mConeOutsideAngle    = 360;
   mDescription.mConeOutsideVolume   = 1.0f;
   mDescription.mConeVector.set(0, 0, 1);

};

AudioEmitter::~AudioEmitter(){}

void AudioEmitter::initPersistFields()
{
	Parent::initPersistFields();

	addProtectedField( "AudioAsset",TypeAudioAssetPtr, Offset(mAudioAsset, AudioEmitter), &setAudioAsset, &getAudioAsset, "Oh yeah");
	addProtectedField("Volume", TypeF32, Offset(mDescription.mVolume, AudioEmitter), &setVolume, &defaultProtectedGetFn, &writeVolume, "");
    addProtectedField("VolumeChannel", TypeS32, Offset(mDescription.mVolumeChannel, AudioEmitter), &setVolumeChannel, &defaultProtectedGetFn, &writeVolumeChannel, "");
    addProtectedField("Looping", TypeBool, Offset(mDescription.mIsLooping, AudioEmitter), &setLooping, &defaultProtectedGetFn, &writeLooping, "");
    addProtectedField("Streaming", TypeBool, Offset(mDescription.mIsStreaming, AudioEmitter), &setStreaming, &defaultProtectedGetFn, &writeStreaming, "");
	addProtectedField( "is3D", TypeBool, Offset(mDescription.mIs3D, AudioEmitter), &setis3D, &defaultProtectedGetFn, &writeis3D, "");
	addProtectedField( "referenceDistance", TypeF32, Offset(mDescription.mReferenceDistance, AudioEmitter), &setRefDistance, &defaultProtectedGetFn, &writeRefDistance,"");
	addProtectedField( "maxDistance", TypeF32, Offset(mDescription.mMaxDistance, AudioEmitter), &setMaxDistance, &defaultProtectedGetFn, &writeMaxDistance,"");
	addProtectedField( "coneInsideAngle",   TypeS32, Offset(mDescription.mConeInsideAngle, AudioEmitter), &setConeInsideAngle, &defaultProtectedGetFn, &writeConeInsideAngle,"");
	addProtectedField("coneOutsideAngle",  TypeS32, Offset(mDescription.mConeOutsideAngle, AudioEmitter), &setConeOutsideAngle, &defaultProtectedGetFn, &writeConeOutsideAngle, "");
	addProtectedField("coneOutsideVolume", TypeF32, Offset(mDescription.mConeOutsideVolume, AudioEmitter), &setConeOutsideVolume, &defaultProtectedGetFn, &writeConeOutsideVolume,"");	
	addProtectedField("environmentLevel",  TypeF32, Offset(mDescription.mEnvironmentLevel, AudioEmitter), &setEnvironmentLevel, &defaultProtectedGetFn, &writeEnvironmentLevel,"");

}


void AudioEmitter::preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{

    // Call parent.
    Parent::preIntegrate( totalTime, elapsedTime, pDebugStats );
}

//-----------------------------------------------------------------------------

void AudioEmitter::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Call Parent.
    Parent::integrateObject( totalTime, elapsedTime, pDebugStats );

    // Finish if the spatials are NOT dirty.

	if(getSpatialDirty())
	{
	if(mHandle)
	{
		alxSource3f(getHandle(), AL_POSITION, getPosition().x, getPosition().y, -0.25);
	}
	}
}
//-----------------------------------------------------------------------------

void AudioEmitter::interpolateObject( const F32 timeDelta )
{
    // Call parent.
    Parent::interpolateObject( timeDelta );
}

//------------------------------------------------------------------------------

void AudioEmitter::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Fetch object.
	AudioEmitter* pEmitter = dynamic_cast<AudioEmitter*>(object);

    // Sanity!
    AssertFatal(pEmitter != NULL, "AudioEmitter::copyTo() - Object is not the correct type.");

    // Copy state.
	pEmitter->setAudioAsset(mAudioAsset.getAssetId());
	  
    pEmitter->setVolume( getVolume() );
    pEmitter->setVolumeChannel( getVolumeChannel() );
    pEmitter->setLooping( getLooping() );
    pEmitter->setStreaming( getStreaming() );
	pEmitter->setis3D( getis3D());
	pEmitter->setRefDistance(getRefDistance());
	pEmitter->setMaxDistance(getMaxDistance());
	pEmitter->setConeInsideAngle(getConeInsideAngle());
	pEmitter->setConeOutsideAngle(getConeOutsideAngle());
	pEmitter->setConeOutsideVolume(getConeOutsideVolume());
	pEmitter->setEnvironmentLevel(getEnvironmentLevel());
}

void AudioEmitter::setAudioAsset( const char* mAudioAssetID )
{
	AssertFatal( mAudioAssetID!= NULL, "Cannot use a NULL asset Id." );

    // Fetch the asset Id.
	mAudioAsset = mAudioAssetID;	
}

void AudioEmitter::setVolume( const F32 volume )
{
    // Ignore no change.
    if ( mIsEqual( volume, mDescription.mVolume ) )
        return;

    // Update.
    mDescription.mVolume = mClampF(volume, 0.0f, 1.0f);
}

void AudioEmitter::setVolumeChannel( const S32 volumeChannel )
{
    // Ignore no change.
    if ( volumeChannel == mDescription.mVolumeChannel )
        return;

    // Update.
    mDescription.mVolumeChannel = mClamp( volumeChannel, 0, Audio::AudioVolumeChannels-1 );

}

//--------------------------------------------------------------------------

void AudioEmitter::setLooping( const bool looping )
{
    // Ignore no change.
    if ( looping == mDescription.mIsLooping )
        return;

    // Update.
    mDescription.mIsLooping = looping;

}


//--------------------------------------------------------------------------

void AudioEmitter::setStreaming( const bool streaming )
{
    // Ignore no change.
    if ( streaming == mDescription.mIsStreaming )
        return;

    // UPdate.
    mDescription.mIsStreaming = streaming;

}


void AudioEmitter::setis3D( const bool is3d )
{
	mDescription.mIs3D = is3d;
}
	
void AudioEmitter::setRefDistance( const F32 refDistance )
{
	mDescription.mReferenceDistance = refDistance;
}   

void AudioEmitter::setMaxDistance( const F32 MaxDistance )
{
	mDescription.mMaxDistance = MaxDistance;
}
   
void AudioEmitter::setConeInsideAngle( const S32 ConeInsideAngle )
{
	mDescription.mConeInsideAngle = ConeInsideAngle;
}
   
void AudioEmitter::setConeOutsideAngle( const S32 ConeOutsideAngle )
{
	mDescription.mConeOutsideAngle = ConeOutsideAngle;
}
   
void AudioEmitter::setConeOutsideVolume( const F32 ConeOutsideVolume )
{
	mDescription.mConeOutsideVolume = ConeOutsideVolume;
}
	
void AudioEmitter::setEnvironmentLevel( const F32 EnvironmentLevel )
{
	mDescription.mEnvironmentLevel = EnvironmentLevel;
}

void AudioEmitter::setDescription( const Audio::Description& audioDescription )
{
    // Update.
    mDescription = audioDescription;
}


AUDIOHANDLE AudioEmitter::getHandle( void )
{
	return (this->mHandle);
}

bool AudioEmitter::setHandle(AUDIOHANDLE pHandle)
{
	this->mHandle = pHandle;
	if(pHandle != NULL_AUDIOHANDLE) return true;
	else return false;
}
