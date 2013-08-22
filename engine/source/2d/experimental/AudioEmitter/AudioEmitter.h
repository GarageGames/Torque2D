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

public:
	AudioEmitter();
	virtual ~AudioEmitter();

	 static void initPersistFields();



	//-------------------------------------------

    virtual void preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    virtual void interpolateObject( const F32 timeDelta );

    virtual void copyTo( SimObject* object );

    //virtual bool canPrepareRender( void ) const { return true; }
    //virtual bool validRender( void ) const { return mImageAsset.notNull(); }
    //virtual bool shouldRender( void ) const { return true; }
    //virtual void scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue );    
    //virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );

	//-------------------------------------------
	AUDIOHANDLE getHandle( void );
	bool setHandle(AUDIOHANDLE pHandle);

	void setAudioAsset( const char* pAudioAsset );	
	inline StringTableEntry getAudioAsset( void ) const { return mAudioAsset.getAssetId(); }

	DECLARE_CONOBJECT(AudioEmitter);
		
	protected : 
		static bool setAudioAsset( void* obj, const char* data ) { static_cast<AudioEmitter*>(obj)->setAudioAsset(data); return false; }
		static const char* getAudioAsset(void* obj, const char* data) { return static_cast<AudioEmitter*>(obj)->getAudioAsset(); }
	
};

#endif