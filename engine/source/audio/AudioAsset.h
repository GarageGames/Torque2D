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

#ifndef _AUDIO_ASSET_H_
#define _AUDIO_ASSET_H_

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

#ifndef _ASSET_BASE_H_
#include "assets/assetBase.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

//----------------------------------------------------------------------------

DefineConsoleType( TypeAudioAssetPtr )

//----------------------------------------------------------------------------

class AudioAsset: public AssetBase
{
private:
   typedef AssetBase Parent;

   StringTableEntry mAudioFile;
   Audio::Description mDescription;

public:
   AudioAsset();
   static void initPersistFields();
   virtual void copyTo(SimObject* object);

   void setAudioFile( const char* pAudioFile );
   inline StringTableEntry getAudioFile( void ) const { return mAudioFile; }

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

   DECLARE_CONOBJECT(AudioAsset);

protected:
    virtual void initializeAsset( void );

    /// Taml callbacks.
    virtual void onTamlPreWrite( void );
    virtual void onTamlPostWrite( void );

protected:
    static bool setAudioFile( void* obj, const char* data )                 { static_cast<AudioAsset*>(obj)->setAudioFile(data); return false; }
    static const char* getAudioFile(void* obj, const char* data)            { return static_cast<AudioAsset*>(obj)->getAudioFile(); }

    static bool setVolume( void* obj, const char* data )                        { static_cast<AudioAsset*>(obj)->setVolume(dAtof(data)); return false; }
    static bool writeVolume( void* obj, StringTableEntry pFieldName )           { return mNotEqual(static_cast<AudioAsset*>(obj)->getVolume(), 1.0f); }

    static bool setVolumeChannel( void* obj, const char* data )                 { static_cast<AudioAsset*>(obj)->setVolumeChannel(dAtoi(data)); return false; }
    static bool writeVolumeChannel( void* obj, StringTableEntry pFieldName )    { return static_cast<AudioAsset*>(obj)->getVolumeChannel() != 0; }

    static bool setLooping( void* obj, const char* data )                       { static_cast<AudioAsset*>(obj)->setLooping(dAtob(data)); return false; }
    static bool writeLooping( void* obj, StringTableEntry pFieldName )          { return static_cast<AudioAsset*>(obj)->getLooping() == true; }

    static bool setStreaming( void* obj, const char* data )                     { static_cast<AudioAsset*>(obj)->setStreaming(dAtob(data)); return false; }
    static bool writeStreaming( void* obj, StringTableEntry pFieldName )        { return static_cast<AudioAsset*>(obj)->getStreaming() == true; }
};

#endif  // _AUDIO_ASSET_H_
