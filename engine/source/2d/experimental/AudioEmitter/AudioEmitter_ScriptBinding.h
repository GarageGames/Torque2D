#include "AudioEmitter.h"

#ifndef _ASSET_MANAGER_H_
#include "assets/assetManager.h"
#endif

ConsoleMethod(AudioEmitter, playSource, S32, 2, 2, "")
{

	AudioAsset* myAudio =  AssetDatabase.acquireAsset<AudioAsset>(object->getAudioAsset());
	
	MatrixF transform(true);
	
	F32 x = object->getPosition().x;
	F32 y = object->getPosition().y;
	transform.setColumn(3, Point3F(x,y,-0.25));
	
	   
	AUDIOHANDLE handle = object->alxCreateSourceEmitter( object, myAudio, &transform);
	

   if(handle != NULL_AUDIOHANDLE)
   {
	  object->setHandle(handle);
      return(alxPlay(handle));
   }
   return(NULL_AUDIOHANDLE);
}