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

#ifndef _ASSET_MANAGER_H_
#include "assets/assetManager.h"
#endif

#include "platform/platform.h"

#ifndef _PLATFORMAUDIO_H_
#include "platform/platformAudio.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _AUDIODATABLOCK_H_
#include "audio/audioDataBlock.h"
#endif

#ifndef _AUDIO_ASSET_H_
#include "audio/audioAsset.h"
#endif

#ifdef TORQUE_OS_IOS
#include "platformiOS/iOSStreamSource.h"
#endif


extern ALuint alxGetWaveLen(ALuint buffer);

extern F32 mAudioChannelVolumes[Audio::AudioVolumeChannels];

//--------------------------------------------------------------------------
// Expose all al get/set methods...
//--------------------------------------------------------------------------
enum AL_GetSetBits{
   Source      =  BIT(0),
   Listener    =  BIT(1),
   Context     =  BIT(2),
   Environment =  BIT(3),
   Get         =  BIT(4),
   Set         =  BIT(5),
   Int         =  BIT(6),
   Float       =  BIT(7),
   Float3      =  BIT(8),
   Float6      =  BIT(9)
};

static ALenum getEnum(const char * name, U32 flags)
{
   AssertFatal(name, "Audio getEnum: bad param");

   static struct {
      const char *   mName;
      ALenum   mAlenum;
      U32      mFlags;
   } table[] = {
      //-----------------------------------------------------------------------------------------------------------------
      // "name"                           ENUM                             Flags
      //-----------------------------------------------------------------------------------------------------------------
      { "AL_GAIN",                        AL_GAIN,                         (Source|Listener|Get|Set|Float) },
      { "AL_GAIN_LINEAR",                 AL_GAIN_LINEAR,                  (Source|Listener|Get|Set|Float) },
      { "AL_PITCH",                       AL_PITCH,                        (Source|Get|Set|Float) },
      { "AL_REFERENCE_DISTANCE",          AL_REFERENCE_DISTANCE,           (Source|Get|Set|Float) },
      { "AL_MAX_DISTANCE",                AL_MAX_DISTANCE,                 (Source|Get|Set|Float) },
      { "AL_CONE_OUTER_GAIN",             AL_CONE_OUTER_GAIN,              (Source|Get|Set|Float) },
      { "AL_POSITION",                    AL_POSITION,                     (Source|Listener|Get|Set|Float3) },
      { "AL_DIRECTION",                   AL_DIRECTION,                    (Source|Get|Set|Float3) },
      { "AL_VELOCITY",                    AL_VELOCITY,                     (Source|Listener|Get|Set|Float3) },
      { "AL_ORIENTATION",                 AL_ORIENTATION,                  (Listener|Set|Float6) },
      { "AL_CONE_INNER_ANGLE",            AL_CONE_INNER_ANGLE,             (Source|Get|Set|Int) },
      { "AL_CONE_OUTER_ANGLE",            AL_CONE_OUTER_ANGLE,             (Source|Get|Set|Int) },
      { "AL_LOOPING",                     AL_LOOPING,                      (Source|Get|Set|Int) },
      //{ "AL_STREAMING",                   AL_STREAMING,                    (Source|Get|Set|Int) },
      //{ "AL_BUFFER",                      AL_BUFFER,                       (Source|Get|Set|Int) },

      { "AL_VENDOR",                      AL_VENDOR,                       (Context|Get) },
      { "AL_VERSION",                     AL_VERSION,                      (Context|Get) },
      { "AL_RENDERER",                    AL_RENDERER,                     (Context|Get) },
      { "AL_EXTENSIONS",                  AL_EXTENSIONS,                   (Context|Get) },

      /*
      // environment
      { "AL_ENV_ROOM_IASIG",                       AL_ENV_ROOM_IASIG,                        (Environment|Get|Set|Int) },
      { "AL_ENV_ROOM_HIGH_FREQUENCY_IASIG",        AL_ENV_ROOM_HIGH_FREQUENCY_IASIG,         (Environment|Get|Set|Int) },
      { "AL_ENV_REFLECTIONS_IASIG",                AL_ENV_REFLECTIONS_IASIG,                 (Environment|Get|Set|Int) },
      { "AL_ENV_REVERB_IASIG",                     AL_ENV_REVERB_IASIG,                      (Environment|Get|Set|Int) },
      { "AL_ENV_ROOM_ROLLOFF_FACTOR_IASIG",        AL_ENV_ROOM_ROLLOFF_FACTOR_IASIG,         (Environment|Get|Set|Float) },
      { "AL_ENV_DECAY_TIME_IASIG",                 AL_ENV_DECAY_TIME_IASIG,                  (Environment|Get|Set|Float) },
      { "AL_ENV_DECAY_HIGH_FREQUENCY_RATIO_IASIG", AL_ENV_DECAY_HIGH_FREQUENCY_RATIO_IASIG,  (Environment|Get|Set|Float) },
      { "AL_ENV_REFLECTIONS_DELAY_IASIG",          AL_ENV_REFLECTIONS_DELAY_IASIG,           (Environment|Get|Set|Float) },
      { "AL_ENV_REVERB_DELAY_IASIG",               AL_ENV_REVERB_DELAY_IASIG,                (Environment|Get|Set|Float) },
      { "AL_ENV_DIFFUSION_IASIG",                  AL_ENV_DIFFUSION_IASIG,                   (Environment|Get|Set|Float) },
      { "AL_ENV_DENSITY_IASIG",                    AL_ENV_DENSITY_IASIG,                     (Environment|Get|Set|Float) },
      { "AL_ENV_HIGH_FREQUENCY_REFERENCE_IASIG",   AL_ENV_HIGH_FREQUENCY_REFERENCE_IASIG,    (Environment|Get|Set|Float) },

      { "AL_ENV_ROOM_VOLUME_EXT",                  AL_ENV_ROOM_VOLUME_EXT,                   (Environment|Get|Set|Int) },
      { "AL_ENV_FLAGS_EXT",                        AL_ENV_FLAGS_EXT,                         (Environment|Get|Set|Int) },
      { "AL_ENV_EFFECT_VOLUME_EXT",                AL_ENV_EFFECT_VOLUME_EXT,                 (Environment|Get|Set|Float) },
      { "AL_ENV_DAMPING_EXT",                      AL_ENV_DAMPING_EXT,                       (Environment|Get|Set|Float) },
      { "AL_ENV_ENVIRONMENT_SIZE_EXT",             AL_ENV_ENVIRONMENT_SIZE_EXT,              (Environment|Get|Set|Float) },

      // sample environment
      { "AL_ENV_SAMPLE_DIRECT_EXT",                AL_ENV_SAMPLE_DIRECT_EXT,                 (Source|Get|Set|Int) },
      { "AL_ENV_SAMPLE_DIRECT_HF_EXT",             AL_ENV_SAMPLE_DIRECT_HF_EXT,              (Source|Get|Set|Int) },
      { "AL_ENV_SAMPLE_ROOM_EXT",                  AL_ENV_SAMPLE_ROOM_EXT,                   (Source|Get|Set|Int) },
      { "AL_ENV_SAMPLE_ROOM_HF_EXT",               AL_ENV_SAMPLE_ROOM_HF_EXT,                (Source|Get|Set|Int) },
      { "AL_ENV_SAMPLE_OUTSIDE_VOLUME_HF_EXT",     AL_ENV_SAMPLE_OUTSIDE_VOLUME_HF_EXT,      (Source|Get|Set|Int) },
      { "AL_ENV_SAMPLE_FLAGS_EXT",                 AL_ENV_SAMPLE_FLAGS_EXT,                  (Source|Get|Set|Int) },

      { "AL_ENV_SAMPLE_REVERB_MIX_EXT",            AL_ENV_SAMPLE_REVERB_MIX_EXT,             (Source|Get|Set|Float) },
      { "AL_ENV_SAMPLE_OBSTRUCTION_EXT",           AL_ENV_SAMPLE_OBSTRUCTION_EXT,            (Source|Get|Set|Float) },
      { "AL_ENV_SAMPLE_OBSTRUCTION_LF_RATIO_EXT",  AL_ENV_SAMPLE_OBSTRUCTION_LF_RATIO_EXT,   (Source|Get|Set|Float) },
      { "AL_ENV_SAMPLE_OCCLUSION_EXT",             AL_ENV_SAMPLE_OCCLUSION_EXT,              (Source|Get|Set|Float) },
      { "AL_ENV_SAMPLE_OCCLUSION_LF_RATIO_EXT",    AL_ENV_SAMPLE_OCCLUSION_LF_RATIO_EXT,     (Source|Get|Set|Float) },
      { "AL_ENV_SAMPLE_OCCLUSION_ROOM_RATIO_EXT",  AL_ENV_SAMPLE_OCCLUSION_ROOM_RATIO_EXT,   (Source|Get|Set|Float) },
      { "AL_ENV_SAMPLE_ROOM_ROLLOFF_EXT",          AL_ENV_SAMPLE_ROOM_ROLLOFF_EXT,           (Source|Get|Set|Float) },
      { "AL_ENV_SAMPLE_AIR_ABSORPTION_EXT",        AL_ENV_SAMPLE_AIR_ABSORPTION_EXT,         (Source|Get|Set|Float) },
   */
   };
   for(U32 i = 0; i < (sizeof(table) / sizeof(table[0])); i++)
   {
      if((table[i].mFlags & flags) != flags)
         continue;

      if(dStricmp(table[i].mName, name) == 0)
         return(table[i].mAlenum);
   }

   return(AL_INVALID);
}


//-----------------------------------------------
ConsoleFunctionGroupBegin(Audio, "Functions dealing with the OpenAL audio layer.\n\n"
                          "@see www.OpenAL.org for what these functions do. Variances from posted"
                          "     behaviour is described below.");

ConsoleFunction(OpenALInitDriver, bool, 1, 1, "() Use the OpenALInitDriver function to initialize the OpenAL driver.\n"
                                                                "This must be done before all other OpenAL operations.\n"
                                                                "@return Returns true on successful initialization, false otherwise.\n"
                                                                "@sa OpenALShutdownDriver")
{
   if (Audio::OpenALInit())
   {
      static bool registered = false;
      if (!registered) {
         ResourceManager->registerExtension(".wav", AudioBuffer::construct);
      }
      registered = true;
      return true;
   }
   return false;
}

//-----------------------------------------------
ConsoleFunction(OpenALShutdownDriver, void, 1, 1, "() Use the OpenALShutdownDriver function to stop/shut down the OpenAL driver.\n"
                                                                "After this is called, you must restart the driver with OpenALInitDriver to execute any new sound operations.\n"
                                                                "@return No return value.\n"
                                                                "@sa OpenALInitDriver")
{
   Audio::OpenALShutdown();
}


//-----------------------------------------------
ConsoleFunction(OpenALRegisterExtensions, void, 1, 1, "OpenALRegisterExtensions()"
                "@note Currently does nothing (possibly deprecated)")
{
}

//-----------------------------------------------
ConsoleFunction(alGetString, const char *, 2, 2, "( ALEnum ) Use the alGetString function to get the string equivalent to the specified OpenAL enumerated value.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@return Returns a string corresponding to the passed ALEnum")
{
   ALenum e = getEnum(argv[1], (Context|Get));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "alGetString: invalid enum name '%s'", argv[1]);
      return "";
   }

   return (const char*)alGetString(e);
}


//--------------------------------------------------------------------------
// Soundfile
//--------------------------------------------------------------------------
ConsoleFunction(alxGetAudioLength, S32, 2, 2,   "( audio-assetId ) Use the alxGetAudioLength function to get the play-length of a specified resource sound file in milliseconds.\n"
                                            "@param audio-assetId  The asset Id that specifies the audio file to check.\n"
                                            "@return Returns play-length of the audio file specified by audio-assetId in milliseconds.\n"
                                            "@sa alxGetStreamDuration, alxGetStreamPosition")
{
    // Fetch asset Id.
    const char* pAssetId = argv[1];

    // Acquire audio asset.
    AudioAsset* pAudioAsset = AssetDatabase.acquireAsset<AudioAsset>( pAssetId );

    // Did we get the audio asset?
    if ( pAudioAsset == NULL )
    {
        // No, so warn.
        Con::warnf( "alxGetAudioLength() - Could not find audio asset '%s'.", pAssetId );
        return 0;
    }

    Resource<AudioBuffer> buffer = AudioBuffer::find( pAudioAsset->getAudioFile() );

    if ( !buffer.isNull() )
    {
        ALuint alBuffer = buffer->getALBuffer();
        return alxGetWaveLen( alBuffer );
    }

    // Warn.
    Con::warnf( "alxGetAudioLength() - Could not find audio file '%s' for asset '%s'.", pAudioAsset->getAudioFile(), pAssetId );

    return 0;
}

//--------------------------------------------------------------------------
// Source
//--------------------------------------------------------------------------
ConsoleFunction(alxCreateSource, S32, 2, 2, "(audio-assetId) - Create a source from the specified asset Id.\n"
                                            "@param audio-assetId The asset Id to create the source from.\n"
                                            "@return The handle of the created source or 0 on error." )
{
    // Fetch asset Id.
    const char* pAssetId = argv[1];

    // Acquire audio asset.
    AudioAsset* pAudioAsset = AssetDatabase.acquireAsset<AudioAsset>( pAssetId );

    // Did we get the audio asset?
    if ( pAudioAsset == NULL )
    {
        // No, so warn.
        Con::warnf( "alxCreateSource() - Could not find audio asset '%s'.", pAssetId );
        return NULL_AUDIOHANDLE;
    }

    // Fetch audio handle.
    AUDIOHANDLE handle = alxCreateSource( pAudioAsset );

    // Release asset.
    AssetDatabase.releaseAsset( pAssetId );

    return handle;
}


//-----------------------------------------------
ConsoleFunction(alxSourcef, void, 4, 4, "( handle , ALEnum , value ) Use the alxSource* function to set a source parameter(s) as specified by the OpenAL enumerated type ALEnum.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@param value An ALEnum type specific value corresponding to the new value for this enumerated parameters.\n"
                                                                "@return No return value.\n"
                                                                "@sa alxGetSource*, al*Listener*")
{
   ALenum e = getEnum(argv[2], (Source|Set|Float));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "cAudio_alxSourcef: invalid enum name '%s'", argv[2]);
      return;
   }

   alxSourcef(dAtoi(argv[1]), e, dAtof(argv[3]));
}


//-----------------------------------------------
ConsoleFunction(alxSource3f, void, 3, 6, "( handle , ALEnum , x , y , z ) Use the alxSource* function to set a source parameter(s) as specified by the OpenAL enumerated type ALEnum.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@param x,y,z XYZ floating-point coordinates.\n"
                                                                "@return No return value.\n"
                                                                "@sa alxGetSource*, al*Listener*")
{
   ALenum e = getEnum(argv[2], (Source|Set|Float3));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "cAudio_alxSource3f: invalid enum name '%s'", argv[2]);
      return;
   }

   if((argc != 4 && argc != 6))
   {
      Con::errorf(ConsoleLogEntry::General, "cAudio_alxSource3f: wrong number of args");
      return;
   }

   Point3F pos;
   if(argc == 4)
      dSscanf(argv[3], "%g %g %g", &pos.x, &pos.y, &pos.z);
   else
   {
      pos.x = dAtof(argv[3]);
      pos.y = dAtof(argv[4]);
      pos.z = dAtof(argv[5]);
   }

   alxSource3f(dAtoi(argv[1]), e, pos.x, pos.y, pos.z);
}


//-----------------------------------------------
ConsoleFunction(alxSourcei, void, 4, 4, "( handle , ALEnum , value ) Use the alxSource* function to set a source parameter(s) as specified by the OpenAL enumerated type ALEnum.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@param value An ALEnum type specific value corresponding to the new value for this enumerated parameters.\n"
                                                                "@return No return value.\n"
                                                                "@sa alxGetSource*, al*Listener*")
{
   ALenum e = getEnum(argv[2], (Source|Set|Int));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "cAudio_alxSourcei: invalid enum name '%s'", argv[2]);
      return;
   }

   alxSourcei(dAtoi(argv[1]), e, dAtoi(argv[3]));
}


//-----------------------------------------------
ConsoleFunction(alxGetSourcef, F32, 3, 3, "( handle , ALEnum ) Use the alxGetSource* function to get the current value of a source parameter, as specified by ALEnum.\n"
                                                                "Depending on the ALEnum you need to acquire, be sure to use the correct version (i.e. correct return type) of alxGetSource*.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@return Returns current value of parameter specified by ALEnum for source identified by handle.\n"
                                                                "@sa alxSource*, al*GetListener*")
{
   ALenum e = getEnum(argv[2], (Source|Get|Float));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "cAudio_alxGetSourcef: invalid enum name '%s'", argv[2]);
      return(0.f);
   }

   F32 value;
   alxGetSourcef(dAtoi(argv[1]), e, &value);
   return(value);
}


//-----------------------------------------------
ConsoleFunction(alxGetSource3f, const char *, 3, 3, "( handle , ALEnum ) Use the alxGetSource* function to get the current value of a source parameter, as specified by ALEnum.\n"
                                                                "Depending on the ALEnum you need to acquire, be sure to use the correct version (i.e. correct return type) of alxGetSource*.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name.\n"
                                                                "@return Returns current value of parameter specified by ALEnum for source identified by handle.\n"
                                                                "@sa alxSource*, al*GetListener*")
{
   ALenum e = getEnum(argv[2], (Source|Get|Float));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "cAudio_alxGetSource3f: invalid enum name '%s'", argv[2]);
      return("0 0 0");
   }

   F32 value1, value2, value3;
   alxGetSource3f(dAtoi(argv[1]), e, &value1, &value2, &value3);

   char * ret = Con::getReturnBuffer(64);
   dSprintf(ret, 64, "%7.3f %7.3 %7.3", value1, value2, value3);
   return(ret);
}


//-----------------------------------------------
ConsoleFunction(alxGetSourcei, S32, 3, 3, "( handle , ALEnum ) Use the alxGetSource* function to get the current value of a source parameter, as specified by ALEnum.\n"
                                                                "Depending on the ALEnum you need to acquire, be sure to use the correct version (i.e. correct return type) of alxGetSource*.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@return Returns current value of parameter specified by ALEnum for source identified by handle.\n"
                                                                "@sa alxSource*, al*GetListener*")
{
   ALenum e = getEnum(argv[2], (Source|Get|Int));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "cAudio_alxGetSourcei: invalid enum name '%s'", argv[2]);
      return(0);
   }

   S32 value;
   alxGetSourcei(dAtoi(argv[1]), e, &value);
   return(value);
}


//-----------------------------------------------
ConsoleFunction(alxPlay, S32, 2, 2, "(audio-assetId) - Play the audio asset Id.\n"
                                    "@param audio-assetId The asset Id to play.\n"
                                    "@return The handle to the playing audio asset Id or 0 on error." )
{
    // Fetch asset Id.
    const char* pAssetId = argv[1];

    // Acquire audio asset.
    AudioAsset* pAudioAsset = AssetDatabase.acquireAsset<AudioAsset>( pAssetId );

    // Did we get the audio asset?
    if ( pAudioAsset == NULL )
    {
        // No, so warn.
        Con::warnf( "alxPlay() - Could not find audio asset '%s'.", pAssetId );
        return NULL_AUDIOHANDLE;
    }

    // Fetch audio handle.
    AUDIOHANDLE handle = alxPlay( pAudioAsset );

    // Release asset.
    AssetDatabase.releaseAsset( pAssetId );

    return handle;
}

ConsoleFunction(alxPause, bool, 2, 2, "( handle ) Use the alxPause function to pause a currently playing sound as specified by handle.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@return No return value.\n"
                                                                "@sa alxIsPlaying, alxPlay, alxStopAll")
{
   AUDIOHANDLE handle = dAtoi(argv[1]);
   if(handle == NULL_AUDIOHANDLE)
      return false;
   return alxPause( handle );
}

ConsoleFunction(alxUnpause, void, 2, 2, "( handle ) Use the alxUnpause function to resume playing a currently paused sound as specified by handle.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@return No return value.\n"
                                                                "@sa alxIsPlaying, alxPlay, alxStopAll")
{
   AUDIOHANDLE handle = dAtoi(argv[1]);
   if(handle == NULL_AUDIOHANDLE)
      return;
   alxUnPause( handle );
}

//-----------------------------------------------
ConsoleFunction(alxStop, void, 2, 2, "( handle ) Use the alxStop function to stop a currently playing sound as specified by handle.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@return No return value.\n"
                                                                "@sa alxIsPlaying, alxPlay, alxStopAll")
{
   AUDIOHANDLE handle = dAtoi(argv[1]);
   if(handle == NULL_AUDIOHANDLE)
      return;
   alxStop(handle);
}

//-----------------------------------------------
ConsoleFunction(alxStopAll, void, 1, 1, "() Use the alxStopAll function to stop all currently playing sounds associated with registered handles.\n"
                                                                "@return No return.\n"
                                                                "@sa alxIsPlaying, alxPlay, alxStop")
{
   alxStopAll();
}

//-----------------------------------------------
ConsoleFunction(alxIsPlaying, bool, 2, 5, "( handle ) Use the alxIsPlaying function to determine if the sound associated with a previously set-up sound handle is playing or not.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@return Returns 1 if specified handle is being played, 0 otherwise.\n"
                                                                "@sa alxPlay, alxStop, alxStopAll")
{
   AUDIOHANDLE handle = dAtoi(argv[1]);
   if(handle == NULL_AUDIOHANDLE)
      return false;
   return alxIsPlaying(handle);
}


//--------------------------------------------------------------------------
// Listener
//--------------------------------------------------------------------------
ConsoleFunction(alxListenerf, void, 3, 3, "( AlEnum , value ) Use the al*Listener* function to set a listener parameter(s) as specified by the OpenAL enumerated type ALEnum.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@param value An ALEnum type specific value corresponding to the new value for this enumerated parameters.\n"
                                                                "@return No return value.\n"
                                                                "@sa al*GetListener*, alxSource*")
{
   ALenum e = getEnum(argv[1], (Listener|Set|Float));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "alxListenerf: invalid enum name '%s'", argv[1]);
      return;
   }

   alxListenerf(e, dAtof(argv[2]));
}


//-----------------------------------------------
ConsoleFunction(alListener3f, void, 3, 5, "( ALEnum , x , y , z) Use the al*Listener* function to set a listener parameter(s) as specified by the OpenAL enumerated type ALEnum.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@param x,y,z XYZ floating-point coordinates.\n"
                                                                "@return No return value.\n"
                                                                "@sa al*GetListener*, alxSource*")
{
   ALenum e = getEnum(argv[1], (Listener|Set|Float3));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "alListener3f: invalid enum name '%s'", argv[1]);
      return;
   }

   if(argc != 3 && argc != 5)
   {
      Con::errorf(ConsoleLogEntry::General, "alListener3f: wrong number of arguments");
      return;
   }

   Point3F pos;
   if(argc == 3)
      dSscanf(argv[2], "%g %g %g", &pos.x, &pos.y, &pos.z);
   else
   {
      pos.x = dAtof(argv[2]);
      pos.y = dAtof(argv[3]);
      pos.z = dAtof(argv[4]);
   }

   alListener3f(e, pos.x, pos.y, pos.z);
}


//-----------------------------------------------
ConsoleFunction(alxGetListenerf, F32, 2, 2, "( ALEnum ) Use the al*GetListener* function to get the current value of a listener parameter, as specified by ALEnum.\n"
                                                                "Depending on the ALEnum you need to acquire, be sure to use the correct version (i.e. correct return type) of al*GetListener*.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@return Returns a float (alxGetListenerf), a vector of three floats (alGetListener3f), or an integer value respectively (alGetListeneri).\n"
                                                                "@sa alxGetSource*")
{
   ALenum e = getEnum(argv[1], (Source|Get|Float));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "alxGetListenerf: invalid enum name '%s'", argv[1]);
      return(0.f);
   }

   F32 value;
   alxGetListenerf(e, &value);
   return(value);
}


//-----------------------------------------------
ConsoleFunction(alGetListener3f, const char *, 2, 2, "( ALEnum ) Use the al*GetListener* function to get the current value of a listener parameter, as specified by ALEnum.\n"
                                                                "Depending on the ALEnum you need to acquire, be sure to use the correct version (i.e. correct return type) of al*GetListener*.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@return Returns a float (alxGetListenerf), a vector of three floats (alGetListener3f), or an integer value respectively (alGetListeneri).\n"
                                                                "@sa alxGetSource*")
{
   ALenum e = getEnum(argv[2], (Source|Get|Float));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "alGetListener3f: invalid enum name '%s'", argv[1]);
      return("0 0 0");
   }

   F32 value1, value2, value3;
   alGetListener3f(e, &value1, &value2, &value3);

   char * ret = Con::getReturnBuffer(64);
   dSprintf(ret, 64, "%7.3f %7.3 %7.3", value1, value2, value3);
   return(ret);
}


//-----------------------------------------------
ConsoleFunction(alGetListeneri, S32, 2, 2, "( ALEnum ) Use the al*GetListener* function to get the current value of a listener parameter, as specified by ALEnum.\n"
                                                                "Depending on the ALEnum you need to acquire, be sure to use the correct version (i.e. correct return type) of al*GetListener*.\n"
                                                                "@param ALEnum A string containing an OpenAL enumerated type name. See (above) table of ALEnum values for legal values.\n"
                                                                "@return Returns a float (alxGetListenerf), a vector of three floats (alGetListener3f), or an integer value respectively (alGetListeneri).\n"
                                                                "@sa alxGetSource*")
{
   ALenum e = getEnum(argv[1], (Source|Get|Int));
   if(e == AL_INVALID)
   {
      Con::errorf(ConsoleLogEntry::General, "alGetListeneri: invalid enum name '%s'", argv[1]);
      return(0);
   }

   S32 value;
   alGetListeneri(e, &value);
   return(value);
}


//--------------------------------------------------------------------------
// Channel Volumes
//--------------------------------------------------------------------------
ConsoleFunction(alxGetChannelVolume, F32, 2, 2, "( channelID ) Use the alxGetChannelVolume function to get the volume setting for a specified channel.\n"
                                                                "@param channelID An integer value, equal to or greater than 0, corresponding to a valid audio channel.\n"
                                                                "@return Returns volume [ 0.0, 1.0 ] for channel specified by channelID.\n"
                                                                "@sa alxSetChannelVolume")
{
   U32 channel = dAtoi(argv[1]);
   if(channel >= Audio::AudioVolumeChannels)
   {
      Con::errorf(ConsoleLogEntry::General, "alxGetChannelVolume: invalid channel '%d'", dAtoi(argv[1]));
      return(0.f);
   }

   return(mAudioChannelVolumes[channel]);
}

//-----------------------------------------------
ConsoleFunction(alxSetChannelVolume, bool, 3, 3, "( channelD , volume ) Use the alxSetChannelVolume function to set a volume [ 0.0, 1.0 ] for the channel specified by channelID.\n"
                                                                "@param channelID An integer value, equal to or greater than 0, corresponding to a valid audio channel.\n"
                                                                "@param volume A value between 0.0 and 1.0 specifying the new volume for the specified channel.\n"
                                                                "@return Returns true on success and false on failure.\n"
                                                                "@sa alxGetChannelVolume")
{
   U32 channel = dAtoi(argv[1]);
   F32 volume = mClampF(dAtof(argv[2]), 0.f, 1.f);

   if(channel >= Audio::AudioVolumeChannels)
   {
      Con::errorf(ConsoleLogEntry::General, "alxSetChannelVolume: channel '%d' out of range [0, %d]", channel, Audio::AudioVolumeChannels);
      return false;
   }

   mAudioChannelVolumes[channel] = volume;
   alxUpdateTypeGain(channel);
   return true;
}

//-----------------------------------------------
ConsoleFunction(alxGetStreamPosition, F32, 2, 2, "( handle ) Use the alxGetStreamPosition function to get the current play position for a playing sound. Note, this value is a percentage equivalent to the percent of the sound that as already played.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@return Returns -1 for invalid handle, and 0.0 to 1.0 for valid handle indicating what percentage of the sound file has been played.\n"
                                                                "@sa alxGetStreamDuration")
{
   AUDIOHANDLE handle = dAtoi(argv[1]);

   if(handle == NULL_AUDIOHANDLE)
      return -1;

   return alxGetStreamPosition( handle );
}

//-----------------------------------------------
ConsoleFunction(alxGetStreamDuration, F32, 2, 2, "( handle ) Use the alxGetStreamDuration function to determine the length of a previously set up sound in seconds.\n"
                                                                "@param handle The ID (a non-negative integer) corresponding to a previously set up sound source.\n"
                                                                "@return Returns -1 for invalid handle, and 0.0 to N.M for valid handle indicating length of scheduled sound in seconds.\n"
                                                                "@sa alxGetStreamPosition")
{
   AUDIOHANDLE handle = dAtoi(argv[1]);

   if(handle == NULL_AUDIOHANDLE)
      return -1;

   return alxGetStreamDuration( handle );
}

#ifdef TORQUE_OS_IOS
ConsoleFunction(startiOSAudioStream, S32, 2, 2,  "(audio-assetId) - Play the audio asset Id.\n"
                                                    "@param audio-assetId The asset Id to play.  This *must* be an MP3 to work correctly.\n"
                                                    "@return The playing stream Id." )
{
    // Fetch asset Id.
    const char* pAssetId = argv[1];

    // Acquire audio asset.
    AudioAsset* pAudioAsset = AssetDatabase.acquireAsset<AudioAsset>( pAssetId );

    // Did we get the audio asset?
    if ( pAudioAsset == NULL )
    {
        // No, so warn.
        Con::warnf( "startiOSAudioStream() - Could not find audio asset '%s'.", pAssetId );
        return 0;
    }

    // Fetch the audio filename,
    char fileName[1024];
    Con::expandPath( fileName, sizeof(fileName), pAudioAsset->getAudioFile() );
    iOSStreamSource* pStream = new iOSStreamSource( fileName );
    pStream->start( pAudioAsset->getLooping() );

    // Release asset.
    AssetDatabase.releaseAsset( pAssetId );

    return pStream->getId();
}

//-----------------------------------------------
ConsoleFunction(stopiOSAudioStream, void, 2, 2, "( streamId ) - Stops playing the audio stream Id." )
{
    SimObjectId streamId = dAtoi( argv[1] );
    iOSStreamSource* pStream = Sim::findObject<iOSStreamSource>( streamId );

    if( pStream != NULL )
    {
        if( pStream->isPlaying() )
        {
            pStream->stop();
        }
        pStream->deleteObject();
    }
}

ConsoleFunction(setiOSAudioStreamVolume, void, 3, 3, "setiPhoneAudioVolume( Stream ID, float volume )" )
{
    SimObjectId streamId = dAtoi( argv[1] );
    iOSStreamSource* pStream = Sim::findObject<iOSStreamSource>( streamId );
    
    F32 volume = dAtof( argv[2] );
    
    if( pStream ) {
        if( pStream->isPlaying() ) {
            pStream->setVolume(volume);
        }
    }
}
#endif


ConsoleFunctionGroupEnd(Audio);
