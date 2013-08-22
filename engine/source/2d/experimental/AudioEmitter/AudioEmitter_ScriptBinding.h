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

      transform.setColumn(3, Point3F(x,y,1));

	if(myAudio != NULL)
		{
		AUDIOHANDLE handle = alxPlay(myAudio, &transform);
		object->setHandle(handle);
		return handle;

		}
		

	return NULL_AUDIOHANDLE;
 }


