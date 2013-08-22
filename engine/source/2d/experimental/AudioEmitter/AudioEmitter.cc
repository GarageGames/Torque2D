#ifndef _AUDIO_EMITTER_H
#include "AudioEmitter.h"
#endif

#include "AudioEmitter_ScriptBinding.h"


IMPLEMENT_CONOBJECT(AudioEmitter);

AudioEmitter::AudioEmitter(){};

AudioEmitter::~AudioEmitter(){}

void AudioEmitter::initPersistFields()
{
	Parent::initPersistFields();
	addProtectedField( "AudioAsset",TypeAudioAssetPtr, Offset(mAudioAsset, AudioEmitter), &setAudioAsset, &getAudioAsset, "Oh yeah");
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
	//here we should change the handle's transform, I think
	if(getSpatialDirty())
	{
	if(mHandle)
	{
		alxSource3f(getHandle(), AL_POSITION, getPosition().x, getPosition().y,1.0);
		
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
}

void AudioEmitter::setAudioAsset( const char* mAudioAssetID )
{
	AssertFatal( mAudioAssetID!= NULL, "Cannot use a NULL asset Id." );

    // Fetch the asset Id.
	mAudioAsset = mAudioAssetID;

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