/**
 * OpenAL cross platform audio library
 *
 * Copyright 2013 Apportable Inc.
 *
 * Copyright (C) 2010 by Chris Robinson
 *
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#import "config.h"

#import <stdlib.h>
#import <jni.h>
#import <pthread.h>
#import "alMain.h"
#import "AL/al.h"
#import "AL/alc.h"

#import "apportable_openal_funcs.h"
#import <BridgeKit/JavaClass.h>
#import <BridgeKit/AndroidAudioTrack.h>

static const ALCchar android_device[] = "Android Default";

static int suspended = 0;
static int audioTrackPlaying = 0;
static int audioTrackWasPlaying = 0;

typedef struct
{
    pthread_t thread;
    volatile int running;
} AndroidData;

#define STREAM_MUSIC 3
#define CHANNEL_CONFIGURATION_MONO 2
#define CHANNEL_CONFIGURATION_STEREO 3
#define ENCODING_PCM_8BIT 3
#define ENCODING_PCM_16BIT 2
#define MODE_STREAM 1

static void* thread_function(void* arg)
{
    @autoreleasepool
    {
        ALCdevice* device = (ALCdevice*)arg;
        AndroidData* data = (AndroidData*)device->ExtraData;

        int sampleRateInHz = device->Frequency;
        int channelConfig = ChannelsFromDevFmt(device->FmtChans) == 1 ? CHANNEL_CONFIGURATION_MONO : CHANNEL_CONFIGURATION_STEREO;
        int audioFormat = BytesFromDevFmt(device->FmtType) == 1 ? ENCODING_PCM_8BIT : ENCODING_PCM_16BIT;
        int bufferSizeInBytes = [AndroidAudioTrack minBufferSizeWithSampleRate:sampleRateInHz channelConfig:channelConfig audioFormat:audioFormat];

        // Suggestion from Eric Wing <ewmailing@gmail.com>
        /* According to the author Martins Mozelko, I should multiply bufferSizeInBytes to tune performance.
           Say, multiply by 2.
           But making this number smaller seems to reduce latency...
           I have tried dividing by 2, 4, and 8. 8 refuses to play any sound.
           It seems that this just divides out the multiplication of NumUpdates (default=4)
           which returns it to min buffer size.
           bufferSizeInBytes is used in multiple places and
           bufferSizeInSamples is tied directly to bufferSizeInBytes though, so we need to be careful
           about what we want to change.
           I'm assuming Martins is correct and this is the indeed the place we want to change it.
           Dividing out the bufferSizeInSamples separately and skipping the multiply did not work.
           Omitting the multiply and not dividing did work, but the buffers may be unnecessarily large.
          */
        bufferSizeInBytes = bufferSizeInBytes / device->NumUpdates;

        int bufferSizeInSamples = bufferSizeInBytes / FrameSizeFromDevFmt(device->FmtChans, device->FmtType);

        AndroidAudioTrack *track = [[AndroidAudioTrack alloc]
            initWithStreamType:STREAM_MUSIC
            sampleRate:sampleRateInHz
            channelConfig:channelConfig
            audioFormat:audioFormat
            bufferSize:device->NumUpdates * bufferSizeInBytes
            mode:MODE_STREAM];

        [track play];
        audioTrackPlaying = 1;

        // Use JNI to avoid an extra copy of the audio buffer.  Calling write through BridgeKit
        // would require using an NSMutableData for the buffer, which would be copied to a new
        // byte[] object on every write call.

        JNIEnv *env = [[NSThread currentThread] env];
        (*env)->PushLocalFrame(env, 1);
        jarray buffer = (*env)->NewByteArray(env, bufferSizeInBytes);
        jclass trackClass = [track javaClass].javaClass;
        jmethodID writeMethod = (*env)->GetMethodID(env, trackClass, "write", "([BII)I");

        while (data->running)
        {
            if (suspended) {
                if (audioTrackPlaying) {
                    [track pause];
                    audioTrackPlaying = 0;
                }
                usleep(5000);
                continue;
            } else if (!audioTrackPlaying) {
                [track play];
                audioTrackPlaying = 1;
            }

            void* pBuffer = (*env)->GetPrimitiveArrayCritical(env, buffer, NULL);

            if (pBuffer)
            {
                aluMixData(device, pBuffer, bufferSizeInSamples);
                (*env)->ReleasePrimitiveArrayCritical(env, buffer, pBuffer, 0);

                (*env)->CallNonvirtualIntMethod(env, track->_object, trackClass, writeMethod, buffer, 0, bufferSizeInBytes);
            }
            else
            {
                AL_PRINT("Failed to get pointer to array bytes");
            }
        }

        [track stop];
        [track release];
        audioTrackPlaying = 0;

        (*env)->PopLocalFrame(env, NULL);

        return NULL;
    }
}

static ALCboolean android_open_playback(ALCdevice *device, const ALCchar *deviceName)
{
    AndroidData* data;
    int channels;
    int bytes;

    if (!deviceName)
    {
        deviceName = android_device;
    }
    else if (strcmp(deviceName, android_device) != 0)
    {
        return ALC_FALSE;
    }

    data = (AndroidData*)calloc(1, sizeof(*data));
    device->szDeviceName = strdup(deviceName);
    device->ExtraData = data;
    return ALC_TRUE;
}

static void android_close_playback(ALCdevice *device)
{
    AndroidData* data = (AndroidData*)device->ExtraData;
    if (data != NULL)
    {
        free(data);
        device->ExtraData = NULL;
    }
}

static ALCboolean android_reset_playback(ALCdevice *device)
{
    AndroidData* data = (AndroidData*)device->ExtraData;

    // if (ChannelsFromDevFmt(device->FmtChans) >= 2)
    // {
    //     device->Format = BytesFromDevFmt(device->FmtType) >= 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
    // }
    // else
    // {
    //     device->Format = BytesFromDevFmt(device->FmtType) >= 2 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
    // }

    SetDefaultChannelOrder(device);

    data->running = 1;
    pthread_create(&data->thread, NULL, thread_function, device);

    return ALC_TRUE;
}

static void android_stop_playback(ALCdevice *device)
{
    AndroidData* data = (AndroidData*)device->ExtraData;

    if (data->running)
    {
        data->running = 0;
        pthread_join(data->thread, NULL);
    }
    suspended = 0;
}

static ALCboolean android_open_capture(ALCdevice *pDevice, const ALCchar *deviceName)
{
    (void)pDevice;
    (void)deviceName;
    return ALC_FALSE;
}

static void android_close_capture(ALCdevice *pDevice)
{
    (void)pDevice;
}

static void android_start_capture(ALCdevice *pDevice)
{
    (void)pDevice;
}

static void android_stop_capture(ALCdevice *pDevice)
{
    (void)pDevice;
}

static void android_capture_samples(ALCdevice *pDevice, ALCvoid *pBuffer, ALCuint lSamples)
{
    (void)pDevice;
    (void)pBuffer;
    (void)lSamples;
}

static ALCuint android_available_samples(ALCdevice *pDevice)
{
    (void)pDevice;
    return 0;
}

static const BackendFuncs android_funcs = {
    android_open_playback,
    android_close_playback,
    android_reset_playback,
    android_stop_playback,
    android_open_capture,
    android_close_capture,
    android_start_capture,
    android_stop_capture,
    android_capture_samples,
    android_available_samples
};

static void alc_audiotrack_suspend()
{
    suspended = 1;
    audioTrackWasPlaying = audioTrackPlaying;
}

static void alc_audiotrack_resume()
{
    suspended = 0;
    if (audioTrackWasPlaying)
    {
        while (!audioTrackPlaying)
        {
            sched_yield();
        }
        audioTrackWasPlaying = 0;
    }
}


void alc_audiotrack_init(BackendFuncs *func_list)
{
    *func_list = android_funcs;

    if (apportableOpenALFuncs.alc_android_suspend == NULL
		&& apportableOpenALFuncs.alc_android_set_java_vm == NULL) {
        apportableOpenALFuncs.alc_android_suspend = alc_audiotrack_suspend;
        apportableOpenALFuncs.alc_android_resume = alc_audiotrack_resume;
    }
}

void alc_audiotrack_deinit(void)
{
}

void alc_audiotrack_probe(int type)
{
    if (type == DEVICE_PROBE)
    {
        AppendDeviceList(android_device);
    }
    else if (type == ALL_DEVICE_PROBE)
    {
        AppendAllDeviceList(android_device);
    }
}
