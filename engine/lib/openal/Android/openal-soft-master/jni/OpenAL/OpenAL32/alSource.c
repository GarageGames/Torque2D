/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
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

#include "config.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "alMain.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "alError.h"
#include "alSource.h"
#include "alBuffer.h"
#include "alThunk.h"
#include "alAuxEffectSlot.h"

#ifdef ANDROID
// Apportable: Defines a cap on the maximum number of playing sources
extern int alc_max_sources;
extern int alc_active_sources;
#define LOG_TAG "OpenAL_alSource.c"
#define LOGV(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#else
#define LOGV(...)
#endif

resampler_t DefaultResampler;
const ALsizei ResamplerPadding[RESAMPLER_MAX] = {
    0, /* Point */
    1, /* Linear */
    2, /* Cubic */
};
const ALsizei ResamplerPrePadding[RESAMPLER_MAX] = {
    0, /* Point */
    0, /* Linear */
    1, /* Cubic */
};


static ALvoid InitSourceParams(ALsource *Source);
static ALvoid GetSourceOffset(ALsource *Source, ALenum eName, ALdfp *Offsets, ALdfp updateLen);
static ALboolean ApplyOffset(ALsource *Source);
static ALint GetByteOffset(ALsource *Source);

#define LookupSource(m, k) ((ALsource*)LookupUIntMapKey(&(m), (k)))
#define LookupBuffer(m, k) ((ALbuffer*)LookupUIntMapKey(&(m), (k)))
#define LookupFilter(m, k) ((ALfilter*)LookupUIntMapKey(&(m), (k)))
#define LookupEffectSlot(m, k) ((ALeffectslot*)LookupUIntMapKey(&(m), (k)))

AL_API ALvoid AL_APIENTRY alGenSources(ALsizei n,ALuint *sources)
{
    ALCcontext *Context;
    ALCdevice *Device;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if(n < 0 || IsBadWritePtr((void*)sources, n * sizeof(ALuint)))
        alSetError(Context, AL_INVALID_VALUE);
    else if((ALuint)n > Device->MaxNoOfSources - Context->SourceMap.size)
        alSetError(Context, AL_INVALID_VALUE);
    else
    {
        ALenum err;
        ALsizei i;

        // Add additional sources to the list
        i = 0;
        while(i < n)
        {
            ALsource *source = calloc(1, sizeof(ALsource));
            if(!source)
            {
                alSetError(Context, AL_OUT_OF_MEMORY);
                alDeleteSources(i, sources);
                break;
            }

            source->source = (ALuint)ALTHUNK_ADDENTRY(source);
            err = InsertUIntMapEntry(&Context->SourceMap, source->source,
                                     source);
            if(err != AL_NO_ERROR)
            {
                ALTHUNK_REMOVEENTRY(source->source);
                memset(source, 0, sizeof(ALsource));
                free(source);

                alSetError(Context, err);
                alDeleteSources(i, sources);
                break;
            }

            sources[i++] = source->source;
            InitSourceParams(source);
        }
    }

    ProcessContext(Context);
}


AL_API ALvoid AL_APIENTRY alDeleteSources(ALsizei n, const ALuint *sources)
{
    ALCcontext *Context;
    ALsource *Source;
    ALsizei i, j;
    ALbufferlistitem *BufferList;
    ALboolean SourcesValid = AL_FALSE;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0)
        alSetError(Context, AL_INVALID_VALUE);
    else
    {
        SourcesValid = AL_TRUE;
        // Check that all Sources are valid (and can therefore be deleted)
        for(i = 0;i < n;i++)
        {
            if(LookupSource(Context->SourceMap, sources[i]) == NULL)
            {
                alSetError(Context, AL_INVALID_NAME);
                SourcesValid = AL_FALSE;
                break;
            }
        }
    }

    if(SourcesValid)
    {
        // All Sources are valid, and can be deleted
        for(i = 0;i < n;i++)
        {
            // Recheck that the Source is valid, because there could be duplicated Source names
            if((Source=LookupSource(Context->SourceMap, sources[i])) == NULL)
                continue;

            for(j = 0;j < Context->ActiveSourceCount;j++)
            {
                if(Context->ActiveSources[j] == Source)
                {
                    ALsizei end = --(Context->ActiveSourceCount);
                    Context->ActiveSources[j] = Context->ActiveSources[end];
                    break;
                }
            }

            // For each buffer in the source's queue...
            while(Source->queue != NULL)
            {
                BufferList = Source->queue;
                Source->queue = BufferList->next;

                if(BufferList->buffer != NULL)
                    BufferList->buffer->refcount--;
                free(BufferList);
            }

            for(j = 0;j < MAX_SENDS;++j)
            {
                if(Source->Send[j].Slot)
                    Source->Send[j].Slot->refcount--;
                Source->Send[j].Slot = NULL;
            }

            // Remove Source from list of Sources
            RemoveUIntMapKey(&Context->SourceMap, Source->source);
            ALTHUNK_REMOVEENTRY(Source->source);

            memset(Source,0,sizeof(ALsource));
            free(Source);
        }
    }

    ProcessContext(Context);
}


AL_API ALboolean AL_APIENTRY alIsSource(ALuint source)
{
    ALCcontext *Context;
    ALboolean  result;

    Context = GetContextSuspended();
    if(!Context) return AL_FALSE;

    result = (LookupSource(Context->SourceMap, source) ? AL_TRUE : AL_FALSE);

    ProcessContext(Context);

    return result;
}


AL_API ALvoid AL_APIENTRY alSourcef(ALuint source, ALenum eParam, ALfloat flValue)
{
    ALCcontext    *pContext;
    ALsource    *Source;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
    {
        switch(eParam)
        {
            case AL_PITCH:
                if(flValue >= 0.0f)
                {
                    Source->flPitch = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_CONE_INNER_ANGLE:
                if(flValue >= 0.0f && flValue <= 360.0f)
                {
                    Source->flInnerAngle = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_CONE_OUTER_ANGLE:
                if(flValue >= 0.0f && flValue <= 360.0f)
                {
                    Source->flOuterAngle = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_GAIN:
                if(flValue >= 0.0f)
                {
                    Source->flGain = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_MAX_DISTANCE:
                if(flValue >= 0.0f)
                {
                    Source->flMaxDistance = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_ROLLOFF_FACTOR:
                if(flValue >= 0.0f)
                {
                    Source->flRollOffFactor = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_REFERENCE_DISTANCE:
                if(flValue >= 0.0f)
                {
                    Source->flRefDistance = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_MIN_GAIN:
                if(flValue >= 0.0f && flValue <= 1.0f)
                {
                    Source->flMinGain = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_MAX_GAIN:
                if(flValue >= 0.0f && flValue <= 1.0f)
                {
                    Source->flMaxGain = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_CONE_OUTER_GAIN:
                if(flValue >= 0.0f && flValue <= 1.0f)
                {
                    Source->flOuterGain = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_CONE_OUTER_GAINHF:
                if(flValue >= 0.0f && flValue <= 1.0f)
                {
                    Source->OuterGainHF = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_AIR_ABSORPTION_FACTOR:
                if(flValue >= 0.0f && flValue <= 10.0f)
                {
                    Source->AirAbsorptionFactor = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_ROOM_ROLLOFF_FACTOR:
                if(flValue >= 0.0f && flValue <= 10.0f)
                {
                    Source->RoomRolloffFactor = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_DOPPLER_FACTOR:
                if(flValue >= 0.0f && flValue <= 1.0f)
                {
                    Source->DopplerFactor = float2ALfp(flValue);
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_SEC_OFFSET:
            case AL_SAMPLE_OFFSET:
            case AL_BYTE_OFFSET:
                if(flValue >= 0.0f)
                {
                    Source->lOffsetType = eParam;

                    // Store Offset (convert Seconds into Milliseconds)
                    if(eParam == AL_SEC_OFFSET)
                        Source->lOffset = (ALint)(flValue * 1000.0f);
                    else
                        Source->lOffset = (ALint)flValue;

                    if ((Source->state == AL_PLAYING) || (Source->state == AL_PAUSED))
                    {
                        if(ApplyOffset(Source) == AL_FALSE)
                            alSetError(pContext, AL_INVALID_VALUE);
                    }
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            default:
                alSetError(pContext, AL_INVALID_ENUM);
                break;
        }
    }
    else
    {
        // Invalid Source Name
        alSetError(pContext, AL_INVALID_NAME);
    }

    ProcessContext(pContext);
}


AL_API ALvoid AL_APIENTRY alSource3f(ALuint source, ALenum eParam, ALfloat flValue1,ALfloat flValue2,ALfloat flValue3)
{
    ALCcontext    *pContext;
    ALsource    *Source;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
    {
        switch(eParam)
        {
            case AL_POSITION:
                Source->vPosition[0] = float2ALfp(flValue1);
                Source->vPosition[1] = float2ALfp(flValue2);
                Source->vPosition[2] = float2ALfp(flValue3);
                Source->NeedsUpdate = AL_TRUE;
                break;

            case AL_VELOCITY:
                Source->vVelocity[0] = float2ALfp(flValue1);
                Source->vVelocity[1] = float2ALfp(flValue2);
                Source->vVelocity[2] = float2ALfp(flValue3);
                Source->NeedsUpdate = AL_TRUE;
                break;

            case AL_DIRECTION:
                Source->vOrientation[0] = float2ALfp(flValue1);
                Source->vOrientation[1] = float2ALfp(flValue2);
                Source->vOrientation[2] = float2ALfp(flValue3);
                Source->NeedsUpdate = AL_TRUE;
                break;

            default:
                alSetError(pContext, AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(pContext, AL_INVALID_NAME);

    ProcessContext(pContext);
}


AL_API ALvoid AL_APIENTRY alSourcefv(ALuint source, ALenum eParam, const ALfloat *pflValues)
{
    ALCcontext    *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(pflValues)
    {
        if(LookupSource(pContext->SourceMap, source) != NULL)
        {
            switch(eParam)
            {
                case AL_PITCH:
                case AL_CONE_INNER_ANGLE:
                case AL_CONE_OUTER_ANGLE:
                case AL_GAIN:
                case AL_MAX_DISTANCE:
                case AL_ROLLOFF_FACTOR:
                case AL_REFERENCE_DISTANCE:
                case AL_MIN_GAIN:
                case AL_MAX_GAIN:
                case AL_CONE_OUTER_GAIN:
                case AL_CONE_OUTER_GAINHF:
                case AL_SEC_OFFSET:
                case AL_SAMPLE_OFFSET:
                case AL_BYTE_OFFSET:
                case AL_AIR_ABSORPTION_FACTOR:
                case AL_ROOM_ROLLOFF_FACTOR:
                    alSourcef(source, eParam, pflValues[0]);
                    break;

                case AL_POSITION:
                case AL_VELOCITY:
                case AL_DIRECTION:
                    alSource3f(source, eParam, pflValues[0], pflValues[1], pflValues[2]);
                    break;

                default:
                    alSetError(pContext, AL_INVALID_ENUM);
                    break;
            }
        }
        else
            alSetError(pContext, AL_INVALID_NAME);
    }
    else
        alSetError(pContext, AL_INVALID_VALUE);

    ProcessContext(pContext);
}


AL_API ALvoid AL_APIENTRY alSourcei(ALuint source,ALenum eParam,ALint lValue)
{
    ALCcontext          *pContext;
    ALsource            *Source;
    ALbufferlistitem    *BufferListItem;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
    {
        ALCdevice *device = pContext->Device;

        switch(eParam)
        {
            case AL_MAX_DISTANCE:
            case AL_ROLLOFF_FACTOR:
            case AL_CONE_INNER_ANGLE:
            case AL_CONE_OUTER_ANGLE:
            case AL_REFERENCE_DISTANCE:
                alSourcef(source, eParam, (ALfloat)lValue);
                break;

            case AL_SOURCE_RELATIVE:
                if(lValue == AL_FALSE || lValue == AL_TRUE)
                {
                    Source->bHeadRelative = (ALboolean)lValue;
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_LOOPING:
                if(lValue == AL_FALSE || lValue == AL_TRUE)
                    Source->bLooping = (ALboolean)lValue;
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_BUFFER:
                if(Source->state == AL_STOPPED || Source->state == AL_INITIAL)
                {
                    ALbuffer *buffer = NULL;

                    if(lValue == 0 ||
                       (buffer=LookupBuffer(device->BufferMap, lValue)) != NULL)
                    {
                        // Remove all elements in the queue
                        while(Source->queue != NULL)
                        {
                            BufferListItem = Source->queue;
                            Source->queue = BufferListItem->next;

                            if(BufferListItem->buffer)
                                BufferListItem->buffer->refcount--;
                            free(BufferListItem);
                        }
                        Source->BuffersInQueue = 0;

                        // Add the buffer to the queue (as long as it is NOT the NULL buffer)
                        if(buffer != NULL)
                        {
                            // Source is now in STATIC mode
                            Source->lSourceType = AL_STATIC;

                            // Add the selected buffer to the queue
                            BufferListItem = malloc(sizeof(ALbufferlistitem));
                            BufferListItem->buffer = buffer;
                            BufferListItem->next = NULL;
                            BufferListItem->prev = NULL;

                            Source->queue = BufferListItem;
                            Source->BuffersInQueue = 1;

                            if(buffer->FmtChannels == FmtMono)
                                Source->Update = CalcSourceParams;
                            else
                                Source->Update = CalcNonAttnSourceParams;

                            // Increment reference counter for buffer
                            buffer->refcount++;
                        }
                        else
                        {
                            // Source is now in UNDETERMINED mode
                            Source->lSourceType = AL_UNDETERMINED;
                        }
                        Source->BuffersPlayed = 0;

                        // Update AL_BUFFER parameter
                        Source->Buffer = buffer;
                        Source->NeedsUpdate = AL_TRUE;
                    }
                    else
                        alSetError(pContext, AL_INVALID_VALUE);
                }
                else
                    alSetError(pContext, AL_INVALID_OPERATION);
                break;

            case AL_SOURCE_STATE:
                // Query only
                alSetError(pContext, AL_INVALID_OPERATION);
                break;

            case AL_SEC_OFFSET:
            case AL_SAMPLE_OFFSET:
            case AL_BYTE_OFFSET:
                if(lValue >= 0)
                {
                    Source->lOffsetType = eParam;

                    // Store Offset (convert Seconds into Milliseconds)
                    if(eParam == AL_SEC_OFFSET)
                        Source->lOffset = lValue * 1000;
                    else
                        Source->lOffset = lValue;

                    if(Source->state == AL_PLAYING || Source->state == AL_PAUSED)
                    {
                        if(ApplyOffset(Source) == AL_FALSE)
                            alSetError(pContext, AL_INVALID_VALUE);
                    }
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_DIRECT_FILTER: {
                ALfilter *filter = NULL;

                if(lValue == 0 ||
                   (filter=LookupFilter(pContext->Device->FilterMap, lValue)) != NULL)
                {
                    if(!filter)
                    {
                        Source->DirectFilter.type = AL_FILTER_NULL;
                        Source->DirectFilter.filter = 0;
                    }
                    else
                        memcpy(&Source->DirectFilter, filter, sizeof(*filter));
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
            }   break;

            case AL_DIRECT_FILTER_GAINHF_AUTO:
                if(lValue == AL_TRUE || lValue == AL_FALSE)
                {
                    Source->DryGainHFAuto = lValue;
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_AUXILIARY_SEND_FILTER_GAIN_AUTO:
                if(lValue == AL_TRUE || lValue == AL_FALSE)
                {
                    Source->WetGainAuto = lValue;
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO:
                if(lValue == AL_TRUE || lValue == AL_FALSE)
                {
                    Source->WetGainHFAuto = lValue;
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_DISTANCE_MODEL:
                if(lValue == AL_NONE ||
                   lValue == AL_INVERSE_DISTANCE ||
                   lValue == AL_INVERSE_DISTANCE_CLAMPED ||
                   lValue == AL_LINEAR_DISTANCE ||
                   lValue == AL_LINEAR_DISTANCE_CLAMPED ||
                   lValue == AL_EXPONENT_DISTANCE ||
                   lValue == AL_EXPONENT_DISTANCE_CLAMPED)
                {
                    Source->DistanceModel = lValue;
                    if(pContext->SourceDistanceModel)
                        Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
                break;

            case AL_PRIORITY:
                Source->priority = lValue;
                break;

            default:
                alSetError(pContext, AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(pContext, AL_INVALID_NAME);

    ProcessContext(pContext);
}


AL_API void AL_APIENTRY alSource3i(ALuint source, ALenum eParam, ALint lValue1, ALint lValue2, ALint lValue3)
{
    ALCcontext *pContext;
    ALsource   *Source;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
    {
        ALCdevice *device = pContext->Device;

        switch (eParam)
        {
            case AL_POSITION:
            case AL_VELOCITY:
            case AL_DIRECTION:
                alSource3f(source, eParam, (ALfloat)lValue1, (ALfloat)lValue2, (ALfloat)lValue3);
                break;

            case AL_AUXILIARY_SEND_FILTER: {
                ALeffectslot *ALEffectSlot = NULL;
                ALfilter     *ALFilter = NULL;

                if((ALuint)lValue2 < device->NumAuxSends &&
                   (lValue1 == 0 ||
                    (ALEffectSlot=LookupEffectSlot(pContext->EffectSlotMap, lValue1)) != NULL) &&
                   (lValue3 == 0 ||
                    (ALFilter=LookupFilter(device->FilterMap, lValue3)) != NULL))
                {
                    /* Release refcount on the previous slot, and add one for
                     * the new slot */
                    if(Source->Send[lValue2].Slot)
                        Source->Send[lValue2].Slot->refcount--;
                    Source->Send[lValue2].Slot = ALEffectSlot;
                    if(Source->Send[lValue2].Slot)
                        Source->Send[lValue2].Slot->refcount++;

                    if(!ALFilter)
                    {
                        /* Disable filter */
                        Source->Send[lValue2].WetFilter.type = 0;
                        Source->Send[lValue2].WetFilter.filter = 0;
                    }
                    else
                        memcpy(&Source->Send[lValue2].WetFilter, ALFilter, sizeof(*ALFilter));
                    Source->NeedsUpdate = AL_TRUE;
                }
                else
                    alSetError(pContext, AL_INVALID_VALUE);
            }    break;

            default:
                alSetError(pContext, AL_INVALID_ENUM);
                break;
        }
    }
    else
        alSetError(pContext, AL_INVALID_NAME);

    ProcessContext(pContext);
}


AL_API void AL_APIENTRY alSourceiv(ALuint source, ALenum eParam, const ALint* plValues)
{
    ALCcontext    *pContext;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(plValues)
    {
        if(LookupSource(pContext->SourceMap, source) != NULL)
        {
            switch(eParam)
            {
                case AL_SOURCE_RELATIVE:
                case AL_CONE_INNER_ANGLE:
                case AL_CONE_OUTER_ANGLE:
                case AL_LOOPING:
                case AL_BUFFER:
                case AL_SOURCE_STATE:
                case AL_SEC_OFFSET:
                case AL_SAMPLE_OFFSET:
                case AL_BYTE_OFFSET:
                case AL_MAX_DISTANCE:
                case AL_ROLLOFF_FACTOR:
                case AL_REFERENCE_DISTANCE:
                case AL_DIRECT_FILTER:
                case AL_DIRECT_FILTER_GAINHF_AUTO:
                case AL_AUXILIARY_SEND_FILTER_GAIN_AUTO:
                case AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO:
                case AL_DISTANCE_MODEL:
                    alSourcei(source, eParam, plValues[0]);
                    break;

                case AL_POSITION:
                case AL_VELOCITY:
                case AL_DIRECTION:
                case AL_AUXILIARY_SEND_FILTER:
                    alSource3i(source, eParam, plValues[0], plValues[1], plValues[2]);
                    break;

                default:
                    alSetError(pContext, AL_INVALID_ENUM);
                    break;
            }
        }
        else
            alSetError(pContext, AL_INVALID_NAME);
    }
    else
        alSetError(pContext, AL_INVALID_VALUE);

    ProcessContext(pContext);
}


AL_API ALvoid AL_APIENTRY alGetSourcef(ALuint source, ALenum eParam, ALfloat *pflValue)
{
    ALCcontext  *pContext;
    ALsource    *Source;
    ALdfp    Offsets[2];
    ALdfp    updateLen;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(pflValue)
    {
        if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
        {
            switch(eParam)
            {
                case AL_PITCH:
                    *pflValue = ALfp2float(Source->flPitch);
                    break;

                case AL_GAIN:
                    *pflValue = ALfp2float(Source->flGain);
                    break;

                case AL_MIN_GAIN:
                    *pflValue = ALfp2float(Source->flMinGain);
                    break;

                case AL_MAX_GAIN:
                    *pflValue = ALfp2float(Source->flMaxGain);
                    break;

                case AL_MAX_DISTANCE:
                    *pflValue = ALfp2float(Source->flMaxDistance);
                    break;

                case AL_ROLLOFF_FACTOR:
                    *pflValue = ALfp2float(Source->flRollOffFactor);
                    break;

                case AL_CONE_OUTER_GAIN:
                    *pflValue = ALfp2float(Source->flOuterGain);
                    break;

                case AL_CONE_OUTER_GAINHF:
                    *pflValue = ALfp2float(Source->OuterGainHF);
                    break;

                case AL_SEC_OFFSET:
                case AL_SAMPLE_OFFSET:
                case AL_BYTE_OFFSET:
                    updateLen = ALdfpDiv(int2ALdfp(pContext->Device->UpdateSize),
                                         int2ALdfp(pContext->Device->Frequency));
                    GetSourceOffset(Source, eParam, Offsets, updateLen);
                    *pflValue = (ALfloat)ALdfp2double(Offsets[0]);
                    break;

                case AL_CONE_INNER_ANGLE:
                    *pflValue = ALfp2float(Source->flInnerAngle);
                    break;

                case AL_CONE_OUTER_ANGLE:
                    *pflValue = ALfp2float(Source->flOuterAngle);
                    break;

                case AL_REFERENCE_DISTANCE:
                    *pflValue = ALfp2float(Source->flRefDistance);
                    break;

                case AL_AIR_ABSORPTION_FACTOR:
                    *pflValue = ALfp2float(Source->AirAbsorptionFactor);
                    break;

                case AL_ROOM_ROLLOFF_FACTOR:
                    *pflValue = ALfp2float(Source->RoomRolloffFactor);
                    break;

                case AL_DOPPLER_FACTOR:
                    *pflValue = ALfp2float(Source->DopplerFactor);
                    break;

                default:
                    alSetError(pContext, AL_INVALID_ENUM);
                    break;
            }
        }
        else
            alSetError(pContext, AL_INVALID_NAME);
    }
    else
        alSetError(pContext, AL_INVALID_VALUE);

    ProcessContext(pContext);
}


AL_API ALvoid AL_APIENTRY alGetSource3f(ALuint source, ALenum eParam, ALfloat* pflValue1, ALfloat* pflValue2, ALfloat* pflValue3)
{
    ALCcontext    *pContext;
    ALsource    *Source;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(pflValue1 && pflValue2 && pflValue3)
    {
        if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
        {
            switch(eParam)
            {
                case AL_POSITION:
                    *pflValue1 = ALfp2float(Source->vPosition[0]);
                    *pflValue2 = ALfp2float(Source->vPosition[1]);
                    *pflValue3 = ALfp2float(Source->vPosition[2]);
                    break;

                case AL_VELOCITY:
                    *pflValue1 = ALfp2float(Source->vVelocity[0]);
                    *pflValue2 = ALfp2float(Source->vVelocity[1]);
                    *pflValue3 = ALfp2float(Source->vVelocity[2]);
                    break;

                case AL_DIRECTION:
                    *pflValue1 = ALfp2float(Source->vOrientation[0]);
                    *pflValue2 = ALfp2float(Source->vOrientation[1]);
                    *pflValue3 = ALfp2float(Source->vOrientation[2]);
                    break;

                default:
                    alSetError(pContext, AL_INVALID_ENUM);
                    break;
            }
        }
        else
            alSetError(pContext, AL_INVALID_NAME);
    }
    else
        alSetError(pContext, AL_INVALID_VALUE);

    ProcessContext(pContext);
}


AL_API ALvoid AL_APIENTRY alGetSourcefv(ALuint source, ALenum eParam, ALfloat *pflValues)
{
    ALCcontext  *pContext;
    ALsource    *Source;
    ALdfp    Offsets[2];
    ALdfp    updateLen;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(pflValues)
    {
        if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
        {
            switch(eParam)
            {
                case AL_PITCH:
                case AL_GAIN:
                case AL_MIN_GAIN:
                case AL_MAX_GAIN:
                case AL_MAX_DISTANCE:
                case AL_ROLLOFF_FACTOR:
                case AL_DOPPLER_FACTOR:
                case AL_CONE_OUTER_GAIN:
                case AL_SEC_OFFSET:
                case AL_SAMPLE_OFFSET:
                case AL_BYTE_OFFSET:
                case AL_CONE_INNER_ANGLE:
                case AL_CONE_OUTER_ANGLE:
                case AL_REFERENCE_DISTANCE:
                case AL_CONE_OUTER_GAINHF:
                case AL_AIR_ABSORPTION_FACTOR:
                case AL_ROOM_ROLLOFF_FACTOR:
                    alGetSourcef(source, eParam, pflValues);
                    break;

                case AL_POSITION:
                case AL_VELOCITY:
                case AL_DIRECTION:
                    alGetSource3f(source, eParam, pflValues+0, pflValues+1, pflValues+2);
                    break;

                case AL_SAMPLE_RW_OFFSETS_SOFT:
                case AL_BYTE_RW_OFFSETS_SOFT:
                    updateLen = ALdfpDiv(int2ALdfp(pContext->Device->UpdateSize),
                                         int2ALdfp(pContext->Device->Frequency));
                    GetSourceOffset(Source, eParam, Offsets, updateLen);
                    pflValues[0] = (ALfloat)ALdfp2double(Offsets[0]);
                    pflValues[1] = (ALfloat)ALdfp2double(Offsets[1]);
                    break;

                default:
                    alSetError(pContext, AL_INVALID_ENUM);
                    break;
            }
        }
        else
            alSetError(pContext, AL_INVALID_NAME);
    }
    else
        alSetError(pContext, AL_INVALID_VALUE);

    ProcessContext(pContext);
}


AL_API ALvoid AL_APIENTRY alGetSourcei(ALuint source, ALenum eParam, ALint *plValue)
{
    ALCcontext *pContext;
    ALsource   *Source;
    ALdfp   Offsets[2];
    ALdfp   updateLen;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(plValue)
    {
        if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
        {
            switch(eParam)
            {
                case AL_MAX_DISTANCE:
                    *plValue = (ALint)ALfp2int(Source->flMaxDistance);
                    break;

                case AL_ROLLOFF_FACTOR:
                    *plValue = (ALint)ALfp2int(Source->flRollOffFactor);
                    break;

                case AL_REFERENCE_DISTANCE:
                    *plValue = (ALint)ALfp2int(Source->flRefDistance);
                    break;

                case AL_SOURCE_RELATIVE:
                    *plValue = Source->bHeadRelative;
                    break;

                case AL_CONE_INNER_ANGLE:
                    *plValue = (ALint)ALfp2int(Source->flInnerAngle);
                    break;

                case AL_CONE_OUTER_ANGLE:
                    *plValue = (ALint)ALfp2int(Source->flOuterAngle);
                    break;

                case AL_LOOPING:
                    *plValue = Source->bLooping;
                    break;

                case AL_BUFFER:
                    *plValue = (Source->Buffer ? Source->Buffer->buffer : 0);
                    break;

                case AL_SOURCE_STATE:
                    *plValue = Source->state;
                    break;

                case AL_BUFFERS_QUEUED:
                    *plValue = Source->BuffersInQueue;
                    break;

                case AL_BUFFERS_PROCESSED:
                    if(Source->bLooping || Source->lSourceType != AL_STREAMING)
                    {
                        /* Buffers on a looping source are in a perpetual state
                         * of PENDING, so don't report any as PROCESSED */
                        *plValue = 0;
                    }
                    else
                        *plValue = Source->BuffersPlayed;
                    break;

                case AL_SOURCE_TYPE:
                    *plValue = Source->lSourceType;
                    break;

                case AL_SEC_OFFSET:
                case AL_SAMPLE_OFFSET:
                case AL_BYTE_OFFSET:
                    updateLen = ALdfpDiv(int2ALdfp(pContext->Device->UpdateSize),
                                         int2ALdfp(pContext->Device->Frequency));
                    GetSourceOffset(Source, eParam, Offsets, updateLen);
                    *plValue = (ALint)ALfp2int(Offsets[0]);
                    break;

                case AL_DIRECT_FILTER:
                    *plValue = Source->DirectFilter.filter;
                    break;

                case AL_DIRECT_FILTER_GAINHF_AUTO:
                    *plValue = Source->DryGainHFAuto;
                    break;

                case AL_AUXILIARY_SEND_FILTER_GAIN_AUTO:
                    *plValue = Source->WetGainAuto;
                    break;

                case AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO:
                    *plValue = Source->WetGainHFAuto;
                    break;

                case AL_DOPPLER_FACTOR:
                    *plValue = (ALint)ALfp2int(Source->DopplerFactor);
                    break;

                case AL_DISTANCE_MODEL:
                    *plValue = Source->DistanceModel;
                    break;

                default:
                    alSetError(pContext, AL_INVALID_ENUM);
                    break;
            }
        }
        else
            alSetError(pContext, AL_INVALID_NAME);
    }
    else
        alSetError(pContext, AL_INVALID_VALUE);

    ProcessContext(pContext);
}


AL_API void AL_APIENTRY alGetSource3i(ALuint source, ALenum eParam, ALint* plValue1, ALint* plValue2, ALint* plValue3)
{
    ALCcontext  *pContext;
    ALsource    *Source;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(plValue1 && plValue2 && plValue3)
    {
        if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
        {
            switch(eParam)
            {
                case AL_POSITION:
                    *plValue1 = (ALint)ALfp2int(Source->vPosition[0]);
                    *plValue2 = (ALint)ALfp2int(Source->vPosition[1]);
                    *plValue3 = (ALint)ALfp2int(Source->vPosition[2]);
                    break;

                case AL_VELOCITY:
                    *plValue1 = (ALint)ALfp2int(Source->vVelocity[0]);
                    *plValue2 = (ALint)ALfp2int(Source->vVelocity[1]);
                    *plValue3 = (ALint)ALfp2int(Source->vVelocity[2]);
                    break;

                case AL_DIRECTION:
                    *plValue1 = (ALint)ALfp2int(Source->vOrientation[0]);
                    *plValue2 = (ALint)ALfp2int(Source->vOrientation[1]);
                    *plValue3 = (ALint)ALfp2int(Source->vOrientation[2]);
                    break;

                default:
                    alSetError(pContext, AL_INVALID_ENUM);
                    break;
            }
        }
        else
            alSetError(pContext, AL_INVALID_NAME);
    }
    else
        alSetError(pContext, AL_INVALID_VALUE);

    ProcessContext(pContext);
}


AL_API void AL_APIENTRY alGetSourceiv(ALuint source, ALenum eParam, ALint* plValues)
{
    ALCcontext  *pContext;
    ALsource    *Source;
    ALdfp    Offsets[2];
    ALdfp    updateLen;

    pContext = GetContextSuspended();
    if(!pContext) return;

    if(plValues)
    {
        if((Source=LookupSource(pContext->SourceMap, source)) != NULL)
        {
            switch(eParam)
            {
                case AL_SOURCE_RELATIVE:
                case AL_CONE_INNER_ANGLE:
                case AL_CONE_OUTER_ANGLE:
                case AL_LOOPING:
                case AL_BUFFER:
                case AL_SOURCE_STATE:
                case AL_BUFFERS_QUEUED:
                case AL_BUFFERS_PROCESSED:
                case AL_SEC_OFFSET:
                case AL_SAMPLE_OFFSET:
                case AL_BYTE_OFFSET:
                case AL_MAX_DISTANCE:
                case AL_ROLLOFF_FACTOR:
                case AL_DOPPLER_FACTOR:
                case AL_REFERENCE_DISTANCE:
                case AL_SOURCE_TYPE:
                case AL_DIRECT_FILTER:
                case AL_DIRECT_FILTER_GAINHF_AUTO:
                case AL_AUXILIARY_SEND_FILTER_GAIN_AUTO:
                case AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO:
                case AL_DISTANCE_MODEL:
                    alGetSourcei(source, eParam, plValues);
                    break;

                case AL_POSITION:
                case AL_VELOCITY:
                case AL_DIRECTION:
                    alGetSource3i(source, eParam, plValues+0, plValues+1, plValues+2);
                    break;

                case AL_SAMPLE_RW_OFFSETS_SOFT:
                case AL_BYTE_RW_OFFSETS_SOFT:
                    updateLen = ALdfpDiv(int2ALdfp(pContext->Device->UpdateSize),
                                         int2ALdfp(pContext->Device->Frequency));
                    GetSourceOffset(Source, eParam, Offsets, updateLen);
                    plValues[0] = (ALint)ALdfp2int(Offsets[0]);
                    plValues[1] = (ALint)ALdfp2int(Offsets[1]);
                    break;

                default:
                    alSetError(pContext, AL_INVALID_ENUM);
                    break;
            }
        }
        else
            alSetError(pContext, AL_INVALID_NAME);
    }
    else
        alSetError(pContext, AL_INVALID_VALUE);

    ProcessContext(pContext);
}


AL_API ALvoid AL_APIENTRY alSourcePlay(ALuint source)
{
    alSourcePlayv(1, &source);
}

AL_API ALvoid AL_APIENTRY alSourcePlayv(ALsizei n, const ALuint *sources)
{
    ALCcontext       *Context;
    ALsource         *Source;
    ALbufferlistitem *BufferList;
    ALsizei          i, j;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }
    if(n > 0 && !sources)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }

    // Check that all the Sources are valid
    for(i = 0;i < n;i++)
    {
        if(!LookupSource(Context->SourceMap, sources[i]))
        {
            alSetError(Context, AL_INVALID_NAME);
            goto done;
        }
    }

    
    while(Context->MaxActiveSources-Context->ActiveSourceCount < n)
    {
        void *temp = NULL;
        ALsizei newcount;

        newcount = Context->MaxActiveSources << 1;
        if(newcount > 0)
            temp = realloc(Context->ActiveSources,
                           sizeof(*Context->ActiveSources) * newcount);
        if(!temp)
        {
            alSetError(Context, AL_OUT_OF_MEMORY);
            goto done;
        }

        Context->ActiveSources = temp;
        Context->MaxActiveSources = newcount;
    }

    
    for(i = 0;i < n;i++)
    {
        Source = (ALsource*)ALTHUNK_LOOKUPENTRY(sources[i]);

#ifdef MAX_SOURCES_LOW
        if (Context->ActiveSourceCount >= (alc_max_sources - Context->PrioritySlots) && Source->priority < 127) {
            LOGV("Skipping starting source %d due to lack of CPU time.", sources[i]);
            continue;
        }
#endif

        // Check that there is a queue containing at least one non-null, non zero length AL Buffer
        BufferList = Source->queue;
        while(BufferList)
        {
            if(BufferList->buffer != NULL && BufferList->buffer->size)
                break;
            BufferList = BufferList->next;
        }

        if(!BufferList)
        {
            Source->state = AL_STOPPED;
            Source->BuffersPlayed = Source->BuffersInQueue;
            Source->position = 0;
            Source->position_fraction = 0;
            Source->lOffset = 0;
            continue;
        }

        if(Source->state != AL_PAUSED)
        {
            Source->state = AL_PLAYING;
            Source->position = 0;
            Source->position_fraction = 0;
            Source->BuffersPlayed = 0;

            Source->Buffer = Source->queue->buffer;
        }
        else
            Source->state = AL_PLAYING;

        // Check if an Offset has been set
        if(Source->lOffset)
            ApplyOffset(Source);

        // If device is disconnected, go right to stopped
        if(!Context->Device->Connected)
        {
            Source->state = AL_STOPPED;
            Source->BuffersPlayed = Source->BuffersInQueue;
            Source->position = 0;
            Source->position_fraction = 0;
        }
        else
        {
            for(j = 0;j < Context->ActiveSourceCount;j++)
            {
                if(Context->ActiveSources[j] == Source)
                    break;
            }
            if(j == Context->ActiveSourceCount)
                Context->ActiveSources[Context->ActiveSourceCount++] = Source;
        }
    }

#ifdef MAX_SOURCES_LOW
    //Apportable Need to give the ALC platform code a hint for setting Source limit based on performance
    alc_active_sources = Context->ActiveSourceCount;
#endif

done:
    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alSourcePause(ALuint source)
{
    alSourcePausev(1, &source);
}

AL_API ALvoid AL_APIENTRY alSourcePausev(ALsizei n, const ALuint *sources)
{
    ALCcontext *Context;
    ALsource *Source;
    ALsizei i;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }
    if(n > 0 && !sources)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }

    // Check all the Sources are valid
    for(i = 0;i < n;i++)
    {
        if(!LookupSource(Context->SourceMap, sources[i]))
        {
            alSetError(Context, AL_INVALID_NAME);
            goto done;
        }
    }

    for(i = 0;i < n;i++)
    {
        Source = (ALsource*)ALTHUNK_LOOKUPENTRY(sources[i]);
        if(Source->state == AL_PLAYING)
            Source->state = AL_PAUSED;
    }

done:
    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alSourceStop(ALuint source)
{
    alSourceStopv(1, &source);
}

AL_API ALvoid AL_APIENTRY alSourceStopv(ALsizei n, const ALuint *sources)
{
    ALCcontext *Context;
    ALsource *Source;
    ALsizei i;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }
    if(n > 0 && !sources)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }

    // Check all the Sources are valid
    for(i = 0;i < n;i++)
    {
        if(!LookupSource(Context->SourceMap, sources[i]))
        {
            alSetError(Context, AL_INVALID_NAME);
            goto done;
        }
    }

    for(i = 0;i < n;i++)
    {
        Source = (ALsource*)ALTHUNK_LOOKUPENTRY(sources[i]);
        if(Source->state != AL_INITIAL)
        {
            Source->state = AL_STOPPED;
            Source->BuffersPlayed = Source->BuffersInQueue;
        }
        Source->lOffset = 0;
    }

done:
    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alSourceRewind(ALuint source)
{
    alSourceRewindv(1, &source);
}

AL_API ALvoid AL_APIENTRY alSourceRewindv(ALsizei n, const ALuint *sources)
{
    ALCcontext *Context;
    ALsource *Source;
    ALsizei i;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }
    if(n > 0 && !sources)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }

    // Check all the Sources are valid
    for(i = 0;i < n;i++)
    {
        if(!LookupSource(Context->SourceMap, sources[i]))
        {
            alSetError(Context, AL_INVALID_NAME);
            goto done;
        }
    }

    for(i = 0;i < n;i++)
    {
        Source = (ALsource*)ALTHUNK_LOOKUPENTRY(sources[i]);
        if(Source->state != AL_INITIAL)
        {
            Source->state = AL_INITIAL;
            Source->position = 0;
            Source->position_fraction = 0;
            Source->BuffersPlayed = 0;
            if(Source->queue)
                Source->Buffer = Source->queue->buffer;
        }
        Source->lOffset = 0;
    }

done:
    ProcessContext(Context);
}


AL_API ALvoid AL_APIENTRY alSourceQueueBuffers(ALuint source, ALsizei n, const ALuint *buffers)
{
    ALCcontext *Context;
    ALCdevice *device;
    ALsource *Source;
    ALbuffer *buffer;
    ALsizei i;
    ALbufferlistitem *BufferListStart;
    ALbufferlistitem *BufferList;
    ALbuffer *BufferFmt;

    if(n == 0)
        return;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }

    // Check that all buffers are valid or zero and that the source is valid

    // Check that this is a valid source
    if((Source=LookupSource(Context->SourceMap, source)) == NULL)
    {
        alSetError(Context, AL_INVALID_NAME);
        goto done;
    }

    // Check that this is not a STATIC Source
    if(Source->lSourceType == AL_STATIC)
    {
        // Invalid Source Type (can't queue on a Static Source)
        alSetError(Context, AL_INVALID_OPERATION);
        goto done;
    }

    device = Context->Device;

    BufferFmt = NULL;

    // Check existing Queue (if any) for a valid Buffers and get its frequency and format
    BufferList = Source->queue;
    while(BufferList)
    {
        if(BufferList->buffer)
        {
            BufferFmt = BufferList->buffer;
            break;
        }
        BufferList = BufferList->next;
    }

    for(i = 0;i < n;i++)
    {
        if(!buffers[i])
            continue;

        if((buffer=LookupBuffer(device->BufferMap, buffers[i])) == NULL)
        {
            alSetError(Context, AL_INVALID_NAME);
            goto done;
        }

        if(BufferFmt == NULL)
        {
            BufferFmt = buffer;

            if(buffer->FmtChannels == FmtMono)
                Source->Update = CalcSourceParams;
            else
                Source->Update = CalcNonAttnSourceParams;

            Source->NeedsUpdate = AL_TRUE;
        }
        else if(BufferFmt->Frequency != buffer->Frequency ||
                BufferFmt->OriginalChannels != buffer->OriginalChannels ||
                BufferFmt->OriginalType != buffer->OriginalType)
        {
            alSetError(Context, AL_INVALID_OPERATION);
            goto done;
        }
    }

    // Change Source Type
    Source->lSourceType = AL_STREAMING;

    buffer = (ALbuffer*)ALTHUNK_LOOKUPENTRY(buffers[0]);

    // All buffers are valid - so add them to the list
    BufferListStart = malloc(sizeof(ALbufferlistitem));
    BufferListStart->buffer = buffer;
    BufferListStart->next = NULL;
    BufferListStart->prev = NULL;

    // Increment reference counter for buffer
    if(buffer) buffer->refcount++;

    BufferList = BufferListStart;

    for(i = 1;i < n;i++)
    {
        buffer = (ALbuffer*)ALTHUNK_LOOKUPENTRY(buffers[i]);

        BufferList->next = malloc(sizeof(ALbufferlistitem));
        BufferList->next->buffer = buffer;
        BufferList->next->next = NULL;
        BufferList->next->prev = BufferList;

        // Increment reference counter for buffer
        if(buffer) buffer->refcount++;

        BufferList = BufferList->next;
    }

    if(Source->queue == NULL)
    {
        Source->queue = BufferListStart;
        // Update Current Buffer
        Source->Buffer = BufferListStart->buffer;
    }
    else
    {
        // Find end of queue
        BufferList = Source->queue;
        while(BufferList->next != NULL)
            BufferList = BufferList->next;

        BufferList->next = BufferListStart;
        BufferList->next->prev = BufferList;
    }

    // Update number of buffers in queue
    Source->BuffersInQueue += n;

done:
    ProcessContext(Context);
}


// Implementation assumes that n is the number of buffers to be removed from the queue and buffers is
// an array of buffer IDs that are to be filled with the names of the buffers removed
AL_API ALvoid AL_APIENTRY alSourceUnqueueBuffers( ALuint source, ALsizei n, ALuint* buffers )
{
    ALCcontext *Context;
    ALsource *Source;
    ALsizei i;
    ALbufferlistitem *BufferList;

    if(n == 0)
        return;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0)
    {
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }

    if((Source=LookupSource(Context->SourceMap, source)) == NULL)
    {
        alSetError(Context, AL_INVALID_NAME);
        goto done;
    }

    if(Source->bLooping || Source->lSourceType != AL_STREAMING ||
       (ALuint)n > Source->BuffersPlayed)
    {
        // Some buffers can't be unqueue because they have not been processed
        alSetError(Context, AL_INVALID_VALUE);
        goto done;
    }

    for(i = 0;i < n;i++)
    {
        BufferList = Source->queue;
        Source->queue = BufferList->next;

        if(BufferList->buffer)
        {
            // Record name of buffer
            buffers[i] = BufferList->buffer->buffer;
            // Decrement buffer reference counter
            BufferList->buffer->refcount--;
        }
        else
            buffers[i] = 0;

        // Release memory for buffer list item
        free(BufferList);
        Source->BuffersInQueue--;
    }
    if(Source->queue)
        Source->queue->prev = NULL;

    if(Source->state != AL_PLAYING)
    {
        if(Source->queue)
            Source->Buffer = Source->queue->buffer;
        else
            Source->Buffer = NULL;
    }
    Source->BuffersPlayed -= n;

done:
    ProcessContext(Context);
}


static ALvoid InitSourceParams(ALsource *Source)
{
    Source->flInnerAngle = int2ALfp(360);
    Source->flOuterAngle = int2ALfp(360);
    Source->flPitch = int2ALfp(1);
    Source->vPosition[0] = int2ALfp(0);
    Source->vPosition[1] = int2ALfp(0);
    Source->vPosition[2] = int2ALfp(0);
    Source->vOrientation[0] = int2ALfp(0);
    Source->vOrientation[1] = int2ALfp(0);
    Source->vOrientation[2] = int2ALfp(0);
    Source->vVelocity[0] = int2ALfp(0);
    Source->vVelocity[1] = int2ALfp(0);
    Source->vVelocity[2] = int2ALfp(0);
    Source->flRefDistance = int2ALfp(1);
    Source->flMaxDistance = int2ALfp(FLT_MAX);
    Source->flRollOffFactor = int2ALfp(1);
    Source->bLooping = AL_FALSE;
    Source->flGain = int2ALfp(1);
    Source->flMinGain = int2ALfp(0);
    Source->flMaxGain = int2ALfp(1);
    Source->flOuterGain = int2ALfp(0);
    Source->OuterGainHF = int2ALfp(1);

    Source->DryGainHFAuto = AL_TRUE;
    Source->WetGainAuto = AL_TRUE;
    Source->WetGainHFAuto = AL_TRUE;
    Source->AirAbsorptionFactor = int2ALfp(0);
    Source->RoomRolloffFactor = int2ALfp(0);
    Source->DopplerFactor = int2ALfp(1);

    Source->DistanceModel = AL_INVERSE_DISTANCE_CLAMPED;

    Source->Resampler = DefaultResampler;

    Source->state = AL_INITIAL;
    Source->lSourceType = AL_UNDETERMINED;

    Source->NeedsUpdate = AL_TRUE;

    Source->Buffer = NULL;
    Source->priority = 0;
}


/*
    GetSourceOffset

    Gets the current playback position in the given Source, in the appropriate format (Bytes, Samples or MilliSeconds)
    The offset is relative to the start of the queue (not the start of the current buffer)
*/
static ALvoid GetSourceOffset(ALsource *Source, ALenum name, ALdfp *offset, ALdfp updateLen)
{
    const ALbufferlistitem *BufferList;
    const ALbuffer         *Buffer = NULL;
    enum UserFmtType OriginalType;
    ALsizei BufferFreq;
    ALint   Channels, Bytes;
    ALuint  readPos, writePos;
    ALuint  TotalBufferDataSize;
    ALuint  i;

    // Find the first non-NULL Buffer in the Queue
    BufferList = Source->queue;
    while(BufferList)
    {
        if(BufferList->buffer)
        {
            Buffer = BufferList->buffer;
            break;
        }
        BufferList = BufferList->next;
    }

    if((Source->state != AL_PLAYING && Source->state != AL_PAUSED) || !Buffer)
    {
        offset[0] = int2ALdfp(0);
        offset[1] = int2ALdfp(0);
        return;
    }

    // Get Current Buffer Size and frequency (in milliseconds)
    BufferFreq = Buffer->Frequency;
    OriginalType = Buffer->OriginalType;
    Channels = ChannelsFromFmt(Buffer->FmtChannels);
    Bytes = BytesFromFmt(Buffer->FmtType);

    // Get Current BytesPlayed (NOTE : This is the byte offset into the *current* buffer)
    readPos = Source->position * Channels * Bytes;
    // Add byte length of any processed buffers in the queue
    TotalBufferDataSize = 0;
    BufferList = Source->queue;
    for(i = 0;BufferList;i++)
    {
        if(BufferList->buffer)
        {
            if(i < Source->BuffersPlayed)
                readPos += BufferList->buffer->size;
            TotalBufferDataSize += BufferList->buffer->size;
        }
        BufferList = BufferList->next;
    }
    if(Source->state == AL_PLAYING)
        writePos = readPos + ((ALuint)(ALdfp2int(ALdfpMult(updateLen,int2ALdfp(BufferFreq)))) * Channels * Bytes);
    else
        writePos = readPos;

    if(Source->bLooping)
    {
        readPos %= TotalBufferDataSize;
        writePos %= TotalBufferDataSize;
    }
    else
    {
        // Wrap positions back to 0
        if(readPos >= TotalBufferDataSize)
            readPos = 0;
        if(writePos >= TotalBufferDataSize)
            writePos = 0;
    }

    switch(name)
    {
        case AL_SEC_OFFSET:
            offset[0] = ALdfpDiv(readPos, int2ALdfp(Channels * Bytes * BufferFreq));
            offset[1] = ALdfpDiv(writePos, int2ALdfp(Channels * Bytes * BufferFreq));
            break;
        case AL_SAMPLE_OFFSET:
        case AL_SAMPLE_RW_OFFSETS_SOFT:
            offset[0] = int2ALdfp(readPos / (Channels * Bytes));
            offset[1] = int2ALdfp(writePos / (Channels * Bytes));
            break;
        case AL_BYTE_OFFSET:
        case AL_BYTE_RW_OFFSETS_SOFT:
            // Take into account the original format of the Buffer
            if(OriginalType == UserFmtIMA4)
            {
                ALuint FrameBlockSize = 65 * Bytes * Channels;
                ALuint BlockSize = 36 * Channels;

                // Round down to nearest ADPCM block
                offset[0] = int2ALdfp(readPos / FrameBlockSize * BlockSize);
                if(Source->state != AL_PLAYING)
                    offset[1] = offset[0];
                else
                {
                    // Round up to nearest ADPCM block
                    offset[1] = int2ALdfp((writePos+FrameBlockSize-1) /
                                          (FrameBlockSize * BlockSize));
                }
            }
            else
            {
                ALuint OrigBytes = BytesFromUserFmt(OriginalType);
                offset[0] = int2ALdfp(readPos / Bytes * OrigBytes);
                offset[1] = int2ALdfp(writePos / Bytes * OrigBytes);
            }
            break;
    }
}


/*
    ApplyOffset

    Apply a playback offset to the Source.  This function will update the queue (to correctly
    mark buffers as 'pending' or 'processed' depending upon the new offset.
*/
static ALboolean ApplyOffset(ALsource *Source)
{
    const ALbufferlistitem *BufferList;
    const ALbuffer         *Buffer;
    ALint lBufferSize, lTotalBufferSize;
    ALint BuffersPlayed;
    ALint lByteOffset;

    // Get true byte offset
    lByteOffset = GetByteOffset(Source);

    // If the offset is invalid, don't apply it
    if(lByteOffset == -1)
        return AL_FALSE;

    // Sort out the queue (pending and processed states)
    BufferList = Source->queue;
    lTotalBufferSize = 0;
    BuffersPlayed = 0;

    while(BufferList)
    {
        Buffer = BufferList->buffer;
        lBufferSize = Buffer ? Buffer->size : 0;

        if(lBufferSize <= lByteOffset-lTotalBufferSize)
        {
            // Offset is past this buffer so increment BuffersPlayed
            BuffersPlayed++;
        }
        else if(lTotalBufferSize <= lByteOffset)
        {
            // Offset is within this buffer
            // Set Current Buffer
            Source->Buffer = BufferList->buffer;
            Source->BuffersPlayed = BuffersPlayed;

            // SW Mixer Positions are in Samples
            Source->position = (lByteOffset - lTotalBufferSize) /
                                FrameSizeFromFmt(Buffer->FmtChannels, Buffer->FmtType);
            return AL_TRUE;
        }

        // Increment the TotalBufferSize
        lTotalBufferSize += lBufferSize;

        // Move on to next buffer in the Queue
        BufferList = BufferList->next;
    }
    // Offset is out of range of the buffer queue
    return AL_FALSE;
}


/*
    GetByteOffset

    Returns the 'true' byte offset into the Source's queue (from the Sample, Byte or Millisecond
    offset supplied by the application).   This takes into account the fact that the buffer format
    may have been modifed by AL (e.g 8bit samples are converted to float)
*/
static ALint GetByteOffset(ALsource *Source)
{
    const ALbuffer *Buffer = NULL;
    const ALbufferlistitem *BufferList;
    ALint ByteOffset = -1;

    // Find the first non-NULL Buffer in the Queue
    BufferList = Source->queue;
    while(BufferList)
    {
        if(BufferList->buffer)
        {
            Buffer = BufferList->buffer;
            break;
        }
        BufferList = BufferList->next;
    }

    if(!Buffer)
    {
        Source->lOffset = 0;
        return -1;
    }

    // Determine the ByteOffset (and ensure it is block aligned)
    switch(Source->lOffsetType)
    {
    case AL_BYTE_OFFSET:
        // Take into consideration the original format
        ByteOffset = Source->lOffset;
        if(Buffer->OriginalType == UserFmtIMA4)
        {
            // Round down to nearest ADPCM block
            ByteOffset /= 36 * ChannelsFromUserFmt(Buffer->OriginalChannels);
            // Multiply by compression rate (65 sample frames per block)
            ByteOffset *= 65;
        }
        else
            ByteOffset /= FrameSizeFromUserFmt(Buffer->OriginalChannels, Buffer->OriginalType);
        ByteOffset *= FrameSizeFromFmt(Buffer->FmtChannels, Buffer->FmtType);
        break;

    case AL_SAMPLE_OFFSET:
        ByteOffset = Source->lOffset * FrameSizeFromFmt(Buffer->FmtChannels, Buffer->FmtType);
        break;

    case AL_SEC_OFFSET:
        // Note - lOffset is internally stored as Milliseconds
        ByteOffset  = (ALint)(Source->lOffset / 1000.0 * Buffer->Frequency);
        ByteOffset *= FrameSizeFromFmt(Buffer->FmtChannels, Buffer->FmtType);
        break;
    }
    // Clear Offset
    Source->lOffset = 0;

    return ByteOffset;
}


ALvoid ReleaseALSources(ALCcontext *Context)
{
    ALsizei pos;
    ALuint j;
    for(pos = 0;pos < Context->SourceMap.size;pos++)
    {
        ALsource *temp = Context->SourceMap.array[pos].value;
        Context->SourceMap.array[pos].value = NULL;

        // For each buffer in the source's queue, decrement its reference counter and remove it
        while(temp->queue != NULL)
        {
            ALbufferlistitem *BufferList = temp->queue;
            temp->queue = BufferList->next;

            if(BufferList->buffer != NULL)
                BufferList->buffer->refcount--;
            free(BufferList);
        }

        for(j = 0;j < MAX_SENDS;++j)
        {
            if(temp->Send[j].Slot)
                temp->Send[j].Slot->refcount--;
            temp->Send[j].Slot = NULL;
        }

        // Release source structure
        ALTHUNK_REMOVEENTRY(temp->source);
        memset(temp, 0, sizeof(ALsource));
        free(temp);
    }
}
