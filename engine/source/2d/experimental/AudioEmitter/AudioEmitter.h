#ifndef _AUDIO_EMITTER_H
#define _AUDIO_EMITTER_H

#ifndef _SCENEOBJECT_H_
#include "2d/sceneobject/sceneobject.h"
#endif

class AudioEmitter: public SceneObject
{

protected:
	typedef SceneObject Parent;

private:
	AUDIOHANDLE mHandle;
	AssetPtr<AudioAsset>        mAudioAsset;
	Audio::Description mDescription;

public:
	AudioEmitter();
	virtual ~AudioEmitter();

	 static void initPersistFields();

	//-------------------------------------------

    virtual void preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void interpolateObject( const F32 timeDelta );

    virtual void copyTo( SimObject* object );

	AUDIOHANDLE alxCreateSourceEmitter(const AudioEmitter *Emitter, const AudioAsset *profile, const MatrixF *transform=NULL);
	//-------------------------------------------

	AUDIOHANDLE getHandle( void );
	bool setHandle(AUDIOHANDLE pHandle);

	void setAudioAsset( const char* pAudioAsset );	
	inline StringTableEntry getAudioAsset( void ) const { return mAudioAsset.getAssetId(); }

    void setVolume( const F32 volume );
    inline F32 getVolume( void ) const { return mDescription.mVolume; }

    void setVolumeChannel( const S32 volumeChannel );
    inline S32 getVolumeChannel( void ) const { return mDescription.mVolumeChannel; }

    void setLooping( const bool looping );
    inline bool getLooping( void ) const { return mDescription.mIsLooping; }

    void setStreaming( const bool streaming );
    inline bool getStreaming( void ) const { return mDescription.mIsStreaming; }

    void setDescription( const Audio::Description& audioDescription );
    inline const Audio::Description& getAudioDescription( void ) const { return mDescription; }

	void setis3D( const bool is3D );
	inline bool getis3D( void ) const { return mDescription.mIs3D; }

    void setRefDistance( const F32 refDistance );
    inline F32 getRefDistance( void ) const { return  mDescription.mReferenceDistance; }
   
    void setMaxDistance( const F32 MaxDistance );
    inline F32 getMaxDistance( void ) const { return mDescription.mMaxDistance; }

    void setConeInsideAngle( const S32 ConeInsideAngle );
    inline S32 getConeInsideAngle( void ) const { return mDescription.mConeInsideAngle; }
   
    void setConeOutsideAngle( const S32 ConeOutsideAngle );
    inline S32 getConeOutsideAngle( void ) const { return mDescription.mConeOutsideAngle; }

    void setConeOutsideVolume( const F32 ConeOutsideVolume );
	inline F32 getConeOutsideVolume( void ) const { return mDescription.mConeOutsideVolume; }

	void setEnvironmentLevel( const F32 EnvironmentLevel );
	inline F32 getEnvironmentLevel( void ) const { return mDescription.mEnvironmentLevel; }

   
	//Missing ConeVector, no conversion function from const char* to Point3F

	DECLARE_CONOBJECT(AudioEmitter);
		
	protected : 

		static bool setAudioAsset( void* obj, const char* data ) { static_cast<AudioEmitter*>(obj)->setAudioAsset(data); return false; }
		static const char* getAudioAsset(void* obj, const char* data) { return static_cast<AudioEmitter*>(obj)->getAudioAsset(); }

    static bool setVolume( void* obj, const char* data )                        { static_cast<AudioEmitter*>(obj)->setVolume(dAtof(data)); return false; }
    static bool writeVolume( void* obj, StringTableEntry pFieldName )           { return mNotEqual(static_cast<AudioEmitter*>(obj)->getVolume(), 1.0f); }

    static bool setVolumeChannel( void* obj, const char* data )                 { static_cast<AudioEmitter*>(obj)->setVolumeChannel(dAtoi(data)); return false; }
    static bool writeVolumeChannel( void* obj, StringTableEntry pFieldName )    { return static_cast<AudioEmitter*>(obj)->getVolumeChannel() != 0; }

    static bool setLooping( void* obj, const char* data )                       { static_cast<AudioEmitter*>(obj)->setLooping(dAtob(data)); return false; }
    static bool writeLooping( void* obj, StringTableEntry pFieldName )          { return static_cast<AudioEmitter*>(obj)->getLooping() == true; }

    static bool setStreaming( void* obj, const char* data )                     { static_cast<AudioEmitter*>(obj)->setStreaming(dAtob(data)); return false; }
    static bool writeStreaming( void* obj, StringTableEntry pFieldName )        { return static_cast<AudioEmitter*>(obj)->getStreaming() == true; }

		static bool setis3D( void* obj, const char* data )                        { static_cast<AudioEmitter*>(obj)->setis3D(dAtob(data)); return false; }
		static bool writeis3D( void* obj, StringTableEntry pFieldName )           { return mNotEqual(static_cast<AudioEmitter*>(obj)->getis3D(), false); }

		static bool setRefDistance( void* obj, const char* data )                 { static_cast<AudioEmitter*>(obj)->setRefDistance(dAtof(data)); return false; }
		static bool writeRefDistance( void* obj, StringTableEntry pFieldName )    { return static_cast<AudioEmitter*>(obj)->getRefDistance() != 0.0; }

		static bool setMaxDistance( void* obj, const char* data )                 { static_cast<AudioEmitter*>(obj)->setMaxDistance(dAtof(data)); return false; }
		static bool writeMaxDistance( void* obj, StringTableEntry pFieldName )    { return static_cast<AudioEmitter*>(obj)->getMaxDistance() != 0.0; }

		static bool setConeInsideAngle( void* obj, const char* data )                 { static_cast<AudioEmitter*>(obj)->setConeInsideAngle(dAtoi(data)); return false; }
		static bool writeConeInsideAngle( void* obj, StringTableEntry pFieldName )    { return static_cast<AudioEmitter*>(obj)->getConeInsideAngle() != 0; }

		static bool setConeOutsideAngle( void* obj, const char* data )                 { static_cast<AudioEmitter*>(obj)->setConeOutsideAngle(dAtoi(data)); return false; }
		static bool writeConeOutsideAngle( void* obj, StringTableEntry pFieldName )    { return static_cast<AudioEmitter*>(obj)->getConeOutsideAngle() != 0; }

		static bool setConeOutsideVolume( void* obj, const char* data )                 { static_cast<AudioEmitter*>(obj)->setConeOutsideVolume(dAtof(data)); return false; }
		static bool writeConeOutsideVolume( void* obj, StringTableEntry pFieldName )    { return static_cast<AudioEmitter*>(obj)->getConeOutsideVolume() != 0.0; }

		static bool setEnvironmentLevel( void* obj, const char* data )                 { static_cast<AudioEmitter*>(obj)->setEnvironmentLevel(dAtof(data)); return false; }
		static bool writeEnvironmentLevel( void* obj, StringTableEntry pFieldName )    { return static_cast<AudioEmitter*>(obj)->getEnvironmentLevel() != 0.0; }

};

#endif