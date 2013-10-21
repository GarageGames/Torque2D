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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

#include "alMain.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "alSource.h"
#include "alBuffer.h"
#include "alListener.h"
#include "alAuxEffectSlot.h"
#include "alu.h"
#include "bs2b.h"

#ifdef MAX_SOURCES_LOW
// For throttling AlSource.c
int alc_max_sources = MAX_SOURCES_LOW;
int alc_active_sources = 0;
int alc_num_cores = 0;
#endif

static __inline ALvoid aluCrossproduct(const ALfp *inVector1, const ALfp *inVector2, ALfp *outVector)
{
    outVector[0] = (ALfpMult(inVector1[1],inVector2[2]) - ALfpMult(inVector1[2],inVector2[1]));
    outVector[1] = (ALfpMult(inVector1[2],inVector2[0]) - ALfpMult(inVector1[0],inVector2[2]));
    outVector[2] = (ALfpMult(inVector1[0],inVector2[1]) - ALfpMult(inVector1[1],inVector2[0]));
}

static __inline ALfp aluDotproduct(const ALfp *inVector1, const ALfp *inVector2)
{
	return (ALfpMult(inVector1[0],inVector2[0]) + ALfpMult(inVector1[1],inVector2[1]) +
            ALfpMult(inVector1[2],inVector2[2]));
}

static __inline ALvoid aluNormalize(ALfp *inVector)
{
    ALfp length, inverse_length;

    length = aluSqrt(aluDotproduct(inVector, inVector));
    if(length != int2ALfp(0))
    {
        inverse_length = ALfpDiv(int2ALfp(1),length);
        inVector[0] = ALfpMult(inVector[0], inverse_length);
        inVector[1] = ALfpMult(inVector[1], inverse_length);
        inVector[2] = ALfpMult(inVector[2], inverse_length);
    }
}

static __inline ALvoid aluMatrixVector(ALfp *vector,ALfp w,ALfp matrix[4][4])
{
    ALfp temp[4] = {
        vector[0], vector[1], vector[2], w
    };

    vector[0] = ALfpMult(temp[0],matrix[0][0]) + ALfpMult(temp[1],matrix[1][0]) + ALfpMult(temp[2],matrix[2][0]) + ALfpMult(temp[3],matrix[3][0]);
    vector[1] = ALfpMult(temp[0],matrix[0][1]) + ALfpMult(temp[1],matrix[1][1]) + ALfpMult(temp[2],matrix[2][1]) + ALfpMult(temp[3],matrix[3][1]);
    vector[2] = ALfpMult(temp[0],matrix[0][2]) + ALfpMult(temp[1],matrix[1][2]) + ALfpMult(temp[2],matrix[2][2]) + ALfpMult(temp[3],matrix[3][2]);
}


ALvoid CalcNonAttnSourceParams(ALsource *ALSource, const ALCcontext *ALContext)
{
    ALfp SourceVolume,ListenerGain,MinVolume,MaxVolume;
    ALbufferlistitem *BufferListItem;
    enum DevFmtChannels DevChans;
    enum FmtChannels Channels;
    ALfp DryGain, DryGainHF;
    ALfp WetGain[MAX_SENDS];
    ALfp WetGainHF[MAX_SENDS];
    ALint NumSends, Frequency;
    ALboolean DupStereo;
    ALfp Pitch;
    ALfp cw;
    ALint i;

    /* Get device properties */
    DevChans  = ALContext->Device->FmtChans;
    DupStereo = ALContext->Device->DuplicateStereo;
    NumSends  = ALContext->Device->NumAuxSends;
    Frequency = ALContext->Device->Frequency;

    /* Get listener properties */
    ListenerGain = ALContext->Listener.Gain;

    /* Get source properties */
    SourceVolume = ALSource->flGain;
    MinVolume    = ALSource->flMinGain;
    MaxVolume    = ALSource->flMaxGain;
    Pitch        = ALSource->flPitch;

    /* Calculate the stepping value */
    Channels = FmtMono;
    BufferListItem = ALSource->queue;
    while(BufferListItem != NULL)
    {
        ALbuffer *ALBuffer;
        if((ALBuffer=BufferListItem->buffer) != NULL)
        {
            ALint maxstep = STACK_DATA_SIZE / FrameSizeFromFmt(ALBuffer->FmtChannels,
                                                               ALBuffer->FmtType);
            maxstep -= ResamplerPadding[ALSource->Resampler] +
                       ResamplerPrePadding[ALSource->Resampler] + 1;
            maxstep = min(maxstep, INT_MAX>>FRACTIONBITS);

            Pitch = ALfpDiv(ALfpMult(Pitch, int2ALfp(ALBuffer->Frequency)), int2ALfp(Frequency));
            if(Pitch > int2ALfp(maxstep))
                ALSource->Params.Step = maxstep<<FRACTIONBITS;
            else
            {
                ALSource->Params.Step = ALfp2int(ALfpMult(Pitch, int2ALfp(FRACTIONONE)));
                if(ALSource->Params.Step == 0)
                    ALSource->Params.Step = 1;
            }

            Channels = ALBuffer->FmtChannels;
            break;
        }
        BufferListItem = BufferListItem->next;
    }

    /* Calculate gains */
    DryGain = SourceVolume;
    DryGain = __min(DryGain,MaxVolume);
    DryGain = __max(DryGain,MinVolume);
    DryGainHF = int2ALfp(1);

    switch(ALSource->DirectFilter.type)
    {
        case AL_FILTER_LOWPASS:
            DryGain = ALfpMult(DryGain, ALSource->DirectFilter.Gain);
            DryGainHF = ALfpMult(DryGainHF, ALSource->DirectFilter.GainHF);
            break;
    }

    for(i = 0;i < MAXCHANNELS;i++)
    {
        ALuint i2;
        for(i2 = 0;i2 < MAXCHANNELS;i2++)
            ALSource->Params.DryGains[i][i2] = int2ALfp(0);
    }

    switch(Channels)
    {
    case FmtMono:
        ALSource->Params.DryGains[0][FRONT_CENTER] = ALfpMult(DryGain, ListenerGain);
        break;
    case FmtStereo:
        if(DupStereo == AL_FALSE)
        {
            ALSource->Params.DryGains[0][FRONT_LEFT]  = ALfpMult(DryGain, ListenerGain);
            ALSource->Params.DryGains[1][FRONT_RIGHT] = ALfpMult(DryGain, ListenerGain);
        }
        else
        {
            switch(DevChans)
            {
            case DevFmtMono:
            case DevFmtStereo:
                ALSource->Params.DryGains[0][FRONT_LEFT]  = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[1][FRONT_RIGHT] = ALfpMult(DryGain, ListenerGain);
                break;

#ifdef STEREO_ONLY
            case DevFmtQuad:
            case DevFmtX51:
            case DevFmtX61:
            case DevFmtX71:
                break;
#else
            case DevFmtQuad:
            case DevFmtX51:
                DryGain = ALfpMult(DryGain, aluSqrt(float2ALfp(2.0f/4.0f)));
                ALSource->Params.DryGains[0][FRONT_LEFT]  = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[1][FRONT_RIGHT] = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[0][BACK_LEFT]   = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[1][BACK_RIGHT]  = ALfpMult(DryGain, ListenerGain);
                break;

            case DevFmtX61:
                DryGain = ALfpMult(DryGain, aluSqrt(float2ALfp(2.0f/4.0f)));
                ALSource->Params.DryGains[0][FRONT_LEFT]  = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[1][FRONT_RIGHT] = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[0][SIDE_LEFT]   = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[1][SIDE_RIGHT]  = ALfpMult(DryGain, ListenerGain);
                break;

            case DevFmtX71:
                DryGain = ALfpMult(DryGain, aluSqrt(float2ALfp(2.0f/6.0f)));
                ALSource->Params.DryGains[0][FRONT_LEFT]  = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[1][FRONT_RIGHT] = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[0][BACK_LEFT]   = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[1][BACK_RIGHT]  = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[0][SIDE_LEFT]   = ALfpMult(DryGain, ListenerGain);
                ALSource->Params.DryGains[1][SIDE_RIGHT]  = ALfpMult(DryGain, ListenerGain);
                break;
#endif
            }
        }
        break;

    case FmtRear:
#ifndef STEREO_ONLY
        ALSource->Params.DryGains[0][BACK_LEFT]  = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[1][BACK_RIGHT] = ALfpMult(DryGain, ListenerGain);
#endif
        break;

    case FmtQuad:
        ALSource->Params.DryGains[0][FRONT_LEFT]  = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[1][FRONT_RIGHT] = ALfpMult(DryGain, ListenerGain);
#ifndef STEREO_ONLY
        ALSource->Params.DryGains[2][BACK_LEFT]   = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[3][BACK_RIGHT]  = ALfpMult(DryGain, ListenerGain);
#endif
        break;

    case FmtX51:
        ALSource->Params.DryGains[0][FRONT_LEFT]   = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[1][FRONT_RIGHT]  = ALfpMult(DryGain, ListenerGain);
#ifndef STEREO_ONLY
        ALSource->Params.DryGains[2][FRONT_CENTER] = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[3][LFE]          = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[4][BACK_LEFT]    = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[5][BACK_RIGHT]   = ALfpMult(DryGain, ListenerGain);
#endif
        break;

    case FmtX61:
        ALSource->Params.DryGains[0][FRONT_LEFT]   = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[1][FRONT_RIGHT]  = ALfpMult(DryGain, ListenerGain);
#ifndef STEREO_ONLY
        ALSource->Params.DryGains[2][FRONT_CENTER] = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[3][LFE]          = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[4][BACK_CENTER]  = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[5][SIDE_LEFT]    = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[6][SIDE_RIGHT]   = ALfpMult(DryGain, ListenerGain);
#endif
        break;

    case FmtX71:
        ALSource->Params.DryGains[0][FRONT_LEFT]   = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[1][FRONT_RIGHT]  = ALfpMult(DryGain, ListenerGain);
#ifndef STEREO_ONLY
        ALSource->Params.DryGains[2][FRONT_CENTER] = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[3][LFE]          = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[4][BACK_LEFT]    = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[5][BACK_RIGHT]   = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[6][SIDE_LEFT]    = ALfpMult(DryGain, ListenerGain);
        ALSource->Params.DryGains[7][SIDE_RIGHT]   = ALfpMult(DryGain, ListenerGain);
#endif
        break;
    }

    for(i = 0;i < NumSends;i++)
    {
        WetGain[i] = SourceVolume;
        WetGain[i] = __min(WetGain[i],MaxVolume);
        WetGain[i] = __max(WetGain[i],MinVolume);
        WetGainHF[i] = int2ALfp(1);

        switch(ALSource->Send[i].WetFilter.type)
        {
            case AL_FILTER_LOWPASS:
                WetGain[i] = ALfpMult(WetGain[i], ALSource->Send[i].WetFilter.Gain);
                WetGainHF[i] = ALfpMult(WetGainHF[i], ALSource->Send[i].WetFilter.GainHF);
                break;
        }

        ALSource->Params.Send[i].WetGain = ALfpMult(WetGain[i], ListenerGain);
    }

    /* Update filter coefficients. Calculations based on the I3DL2
     * spec. */
    cw = float2ALfp(cos(2.0*M_PI * LOWPASSFREQCUTOFF / Frequency));

    /* We use two chained one-pole filters, so we need to take the
     * square root of the squared gain, which is the same as the base
     * gain. */
    ALSource->Params.iirFilter.coeff = lpCoeffCalc(DryGainHF, cw);

    for(i = 0;i < NumSends;i++)
    {
        /* We use a one-pole filter, so we need to take the squared gain */
        ALfp a = lpCoeffCalc(ALfpMult(WetGainHF[i],WetGainHF[i]), cw);
        ALSource->Params.Send[i].iirFilter.coeff = a;
    }
}

ALvoid CalcSourceParams(ALsource *ALSource, const ALCcontext *ALContext)
{
    const ALCdevice *Device = ALContext->Device;
    ALfp InnerAngle,OuterAngle,Angle,Distance,OrigDist;
    ALfp Direction[3],Position[3],SourceToListener[3];
    ALfp Velocity[3],ListenerVel[3];
    ALfp MinVolume,MaxVolume,MinDist,MaxDist,Rolloff,OuterGainHF;
    ALfp ConeVolume,ConeHF,SourceVolume,ListenerGain;
    ALfp DopplerFactor, DopplerVelocity, SpeedOfSound;
    ALfp AirAbsorptionFactor;
    ALbufferlistitem *BufferListItem;
    ALfp Attenuation, EffectiveDist;
    ALfp RoomAttenuation[MAX_SENDS];
    ALfp MetersPerUnit;
    ALfp RoomRolloff[MAX_SENDS];
    ALfp DryGain;
    ALfp DryGainHF;
    ALfp WetGain[MAX_SENDS];
    ALfp WetGainHF[MAX_SENDS];
    ALfp DirGain, AmbientGain;
    const ALfp *SpeakerGain;
    ALfp Pitch;
    ALfp length;
    ALuint Frequency;
    ALint NumSends;
    ALint pos, s, i;
    ALfp cw;

    DryGainHF = int2ALfp(1);
    for(i = 0;i < MAX_SENDS;i++)
        WetGainHF[i] = int2ALfp(1);

    //Get context properties
    DopplerFactor   = ALfpMult(ALContext->DopplerFactor, ALSource->DopplerFactor);
    DopplerVelocity = ALContext->DopplerVelocity;
    SpeedOfSound    = ALContext->flSpeedOfSound;
    NumSends        = Device->NumAuxSends;
    Frequency       = Device->Frequency;

    //Get listener properties
    ListenerGain = ALContext->Listener.Gain;
    MetersPerUnit = ALContext->Listener.MetersPerUnit;
    memcpy(ListenerVel, ALContext->Listener.Velocity, sizeof(ALContext->Listener.Velocity));

    //Get source properties
    SourceVolume = ALSource->flGain;
    memcpy(Position,  ALSource->vPosition,    sizeof(ALSource->vPosition));
    memcpy(Direction, ALSource->vOrientation, sizeof(ALSource->vOrientation));
    memcpy(Velocity,  ALSource->vVelocity,    sizeof(ALSource->vVelocity));
    MinVolume    = ALSource->flMinGain;
    MaxVolume    = ALSource->flMaxGain;
    MinDist      = ALSource->flRefDistance;
    MaxDist      = ALSource->flMaxDistance;
    Rolloff      = ALSource->flRollOffFactor;
    InnerAngle   = ALSource->flInnerAngle;
    OuterAngle   = ALSource->flOuterAngle;
    OuterGainHF  = ALSource->OuterGainHF;
    AirAbsorptionFactor = ALSource->AirAbsorptionFactor;

    //1. Translate Listener to origin (convert to head relative)
    if(ALSource->bHeadRelative == AL_FALSE)
    {
        ALfp U[3],V[3],N[3];
        ALfp Matrix[4][4];

        // Build transform matrix
        memcpy(N, ALContext->Listener.Forward, sizeof(N));  // At-vector
        aluNormalize(N);  // Normalized At-vector
        memcpy(V, ALContext->Listener.Up, sizeof(V));  // Up-vector
        aluNormalize(V);  // Normalized Up-vector
        aluCrossproduct(N, V, U); // Right-vector
        aluNormalize(U);  // Normalized Right-vector
        Matrix[0][0] = U[0]; Matrix[0][1] = V[0]; Matrix[0][2] = -1*N[0]; Matrix[0][3] = int2ALfp(0);
        Matrix[1][0] = U[1]; Matrix[1][1] = V[1]; Matrix[1][2] = -1*N[1]; Matrix[1][3] = int2ALfp(0);
        Matrix[2][0] = U[2]; Matrix[2][1] = V[2]; Matrix[2][2] = -1*N[2]; Matrix[2][3] = int2ALfp(0);
        Matrix[3][0] = int2ALfp(0); Matrix[3][1] = int2ALfp(0); Matrix[3][2] = int2ALfp(0); Matrix[3][3] = int2ALfp(1);

        // Translate position
        Position[0] -= ALContext->Listener.Position[0];
        Position[1] -= ALContext->Listener.Position[1];
        Position[2] -= ALContext->Listener.Position[2];

        // Transform source position and direction into listener space
        aluMatrixVector(Position, int2ALfp(1), Matrix);
        aluMatrixVector(Direction, int2ALfp(0), Matrix);
        // Transform source and listener velocity into listener space
        aluMatrixVector(Velocity, int2ALfp(0), Matrix);
        aluMatrixVector(ListenerVel, int2ALfp(0), Matrix);
    }
    else
        ListenerVel[0] = ListenerVel[1] = ListenerVel[2] = int2ALfp(0);

    SourceToListener[0] = -1*Position[0];
    SourceToListener[1] = -1*Position[1];
    SourceToListener[2] = -1*Position[2];
    aluNormalize(SourceToListener);
    aluNormalize(Direction);

    //2. Calculate distance attenuation
    Distance = aluSqrt(aluDotproduct(Position, Position));
    OrigDist = Distance;

    Attenuation = int2ALfp(1);
    for(i = 0;i < NumSends;i++)
    {
        RoomAttenuation[i] = int2ALfp(1);

        RoomRolloff[i] = ALSource->RoomRolloffFactor;
        if(ALSource->Send[i].Slot &&
           (ALSource->Send[i].Slot->effect.type == AL_EFFECT_REVERB ||
            ALSource->Send[i].Slot->effect.type == AL_EFFECT_EAXREVERB))
            RoomRolloff[i] += ALSource->Send[i].Slot->effect.Reverb.RoomRolloffFactor;
    }

    switch(ALContext->SourceDistanceModel ? ALSource->DistanceModel :
                                            ALContext->DistanceModel)
    {
        case AL_INVERSE_DISTANCE_CLAMPED:
            Distance=__max(Distance,MinDist);
            Distance=__min(Distance,MaxDist);
            if(MaxDist < MinDist)
                break;
            //fall-through
        case AL_INVERSE_DISTANCE:
            if(MinDist > int2ALfp(0))
            {
                if((MinDist + ALfpMult(Rolloff, (Distance - MinDist))) > int2ALfp(0))
                    Attenuation = ALfpDiv(MinDist, (MinDist + ALfpMult(Rolloff, (Distance - MinDist))));
                for(i = 0;i < NumSends;i++)
                {
                    if((MinDist + ALfpMult(RoomRolloff[i], (Distance - MinDist))) > int2ALfp(0))
                        RoomAttenuation[i] = ALfpDiv(MinDist, (MinDist + ALfpMult(RoomRolloff[i], (Distance - MinDist))));
                }
            }
            break;

        case AL_LINEAR_DISTANCE_CLAMPED:
            Distance=__max(Distance,MinDist);
            Distance=__min(Distance,MaxDist);
            if(MaxDist < MinDist)
                break;
            //fall-through
        case AL_LINEAR_DISTANCE:
            if(MaxDist != MinDist)
            {
                Attenuation = int2ALfp(1) - ALfpDiv(ALfpMult(Rolloff,(Distance-MinDist)), (MaxDist - MinDist));
                Attenuation = __max(Attenuation, int2ALfp(0));
                for(i = 0;i < NumSends;i++)
                {
                    RoomAttenuation[i] = int2ALfp(1) - ALfpDiv(ALfpMult(RoomRolloff[i],(Distance-MinDist)),(MaxDist - MinDist));
                    RoomAttenuation[i] = __max(RoomAttenuation[i], int2ALfp(0));
                }
            }
            break;

        case AL_EXPONENT_DISTANCE_CLAMPED:
            Distance=__max(Distance,MinDist);
            Distance=__min(Distance,MaxDist);
            if(MaxDist < MinDist)
                break;
            //fall-through
        case AL_EXPONENT_DISTANCE:
            if(Distance > int2ALfp(0) && MinDist > int2ALfp(0))
            {
                Attenuation = aluPow(ALfpDiv(Distance,MinDist), (-1*Rolloff));
                for(i = 0;i < NumSends;i++)
                    RoomAttenuation[i] = aluPow(ALfpDiv(Distance,MinDist), (-1*RoomRolloff[i]));
            }
            break;

        case AL_NONE:
            break;
    }

    // Source Gain + Attenuation
    DryGain = ALfpMult(SourceVolume, Attenuation);
    for(i = 0;i < NumSends;i++)
        WetGain[i] = ALfpMult(SourceVolume, RoomAttenuation[i]);

    EffectiveDist = int2ALfp(0);
    if(MinDist > int2ALfp(0) && Attenuation < int2ALfp(1))
        EffectiveDist = ALfpMult((ALfpDiv(MinDist,Attenuation) - MinDist),MetersPerUnit);

    // Distance-based air absorption
    if(AirAbsorptionFactor > int2ALfp(0) && EffectiveDist > int2ALfp(0))
    {
        ALfp absorb;

        // Absorption calculation is done in dB
        absorb = ALfpMult(ALfpMult(AirAbsorptionFactor,float2ALfp(AIRABSORBGAINDBHF)),
                 EffectiveDist);
        // Convert dB to linear gain before applying
        absorb = aluPow(int2ALfp(10), ALfpDiv(absorb,int2ALfp(20)));

        DryGainHF = ALfpMult(DryGainHF,absorb);
    }

    //3. Apply directional soundcones
    Angle = ALfpMult(aluAcos(aluDotproduct(Direction,SourceToListener)), float2ALfp(180.0f/M_PI));
    if(Angle >= InnerAngle && Angle <= OuterAngle)
    {
		ALfp scale; scale = ALfpDiv((Angle-InnerAngle), (OuterAngle-InnerAngle));
        ConeVolume = int2ALfp(1) + ALfpMult((ALSource->flOuterGain - int2ALfp(1)),scale);
        ConeHF = (int2ALfp(1)+ALfpMult((OuterGainHF-int2ALfp(1)),scale));
    }
    else if(Angle > OuterAngle)
    {
        ConeVolume = (int2ALfp(1)+(ALSource->flOuterGain-int2ALfp(1)));
        ConeHF = (int2ALfp(1)+(OuterGainHF-int2ALfp(1)));
    }
    else
    {
        ConeVolume = int2ALfp(1);
        ConeHF = int2ALfp(1);
    }

    // Apply some high-frequency attenuation for sources behind the listener
    // NOTE: This should be aluDotproduct({0,0,-1}, ListenerToSource), however
    // that is equivalent to aluDotproduct({0,0,1}, SourceToListener), which is
    // the same as SourceToListener[2]
    Angle = ALfpMult(aluAcos(SourceToListener[2]), float2ALfp(180.0f/M_PI));
    // Sources within the minimum distance attenuate less
    if(OrigDist < MinDist)
        Angle = ALfpMult(Angle, ALfpDiv(OrigDist,MinDist));
    if(Angle > int2ALfp(90))
    {
		ALfp scale; scale = ALfpDiv((Angle-int2ALfp(90)), float2ALfp(180.1f-90.0f)); // .1 to account for fp errors
        ConeHF = ALfpMult(ConeHF, (int2ALfp(1) - ALfpMult(Device->HeadDampen,scale)));
    }

    DryGain = ALfpMult(DryGain, ConeVolume);
    if(ALSource->DryGainHFAuto)
        DryGainHF = ALfpMult(DryGainHF, ConeHF);

    // Clamp to Min/Max Gain
    DryGain = __min(DryGain,MaxVolume);
    DryGain = __max(DryGain,MinVolume);

    for(i = 0;i < NumSends;i++)
    {
        ALeffectslot *Slot = ALSource->Send[i].Slot;

        if(!Slot || Slot->effect.type == AL_EFFECT_NULL)
        {
            ALSource->Params.Send[i].WetGain = int2ALfp(0);
            WetGainHF[i] = int2ALfp(1);
            continue;
        }

        if(Slot->AuxSendAuto)
        {
            if(ALSource->WetGainAuto)
                WetGain[i] = ALfpMult(WetGain[i], ConeVolume);
            if(ALSource->WetGainHFAuto)
                WetGainHF[i] = ALfpMult(WetGainHF[i], ConeHF);

            // Clamp to Min/Max Gain
            WetGain[i] = __min(WetGain[i],MaxVolume);
            WetGain[i] = __max(WetGain[i],MinVolume);

            if(Slot->effect.type == AL_EFFECT_REVERB ||
               Slot->effect.type == AL_EFFECT_EAXREVERB)
            {
                /* Apply a decay-time transformation to the wet path, based on
                 * the attenuation of the dry path.
                 *
                 * Using the approximate (effective) source to listener
                 * distance, the initial decay of the reverb effect is
                 * calculated and applied to the wet path.
                 */
                WetGain[i] = ALfpMult(WetGain[i],
                                      aluPow(int2ALfp(10),
                                             ALfpDiv(ALfpMult(ALfpDiv(EffectiveDist,
                                                                      ALfpMult(float2ALfp(SPEEDOFSOUNDMETRESPERSEC), Slot->effect.Reverb.DecayTime)),
                                                              int2ALfp(-60)),
                                                     int2ALfp(20))));

                WetGainHF[i] = ALfpMult(WetGainHF[i],
                                        aluPow(Slot->effect.Reverb.AirAbsorptionGainHF,
                                               ALfpMult(AirAbsorptionFactor, EffectiveDist)));
            }
        }
        else
        {
            /* If the slot's auxiliary send auto is off, the data sent to the
             * effect slot is the same as the dry path, sans filter effects */
            WetGain[i] = DryGain;
            WetGainHF[i] = DryGainHF;
        }

        switch(ALSource->Send[i].WetFilter.type)
        {
            case AL_FILTER_LOWPASS:
                WetGain[i] = ALfpMult(WetGain[i], ALSource->Send[i].WetFilter.Gain);
                WetGainHF[i] = ALfpMult(WetGainHF[i], ALSource->Send[i].WetFilter.GainHF);
                break;
        }
        ALSource->Params.Send[i].WetGain = ALfpMult(WetGain[i], ListenerGain);
    }

    // Apply filter gains and filters
    switch(ALSource->DirectFilter.type)
    {
        case AL_FILTER_LOWPASS:
            DryGain = ALfpMult(DryGain, ALSource->DirectFilter.Gain);
            DryGainHF = ALfpMult(DryGainHF, ALSource->DirectFilter.GainHF);
            break;
    }
    DryGain = ALfpMult(DryGain, ListenerGain);

    // Calculate Velocity
    Pitch = ALSource->flPitch;
    if(DopplerFactor != int2ALfp(0))
    {
        ALfp VSS, VLS;
		ALfp MaxVelocity; MaxVelocity = ALfpDiv(ALfpMult(SpeedOfSound,DopplerVelocity),
                                                DopplerFactor);

        VSS = aluDotproduct(Velocity, SourceToListener);
        if(VSS >= MaxVelocity)
            VSS = (MaxVelocity - int2ALfp(1));
        else if(VSS <= -MaxVelocity)
            VSS = (-MaxVelocity + int2ALfp(1));

        VLS = aluDotproduct(ListenerVel, SourceToListener);
        if(VLS >= MaxVelocity)
            VLS = (MaxVelocity - int2ALfp(1));
        else if(VLS <= -MaxVelocity)
            VLS = -MaxVelocity + int2ALfp(1);

        Pitch = ALfpMult(Pitch,
                         ALfpDiv((ALfpMult(SpeedOfSound,DopplerVelocity) - ALfpMult(DopplerFactor,VLS)),
                                 (ALfpMult(SpeedOfSound,DopplerVelocity) - ALfpMult(DopplerFactor,VSS))));
    }

    BufferListItem = ALSource->queue;
    while(BufferListItem != NULL)
    {
        ALbuffer *ALBuffer;
        if((ALBuffer=BufferListItem->buffer) != NULL)
        {
            ALint maxstep = STACK_DATA_SIZE / FrameSizeFromFmt(ALBuffer->FmtChannels,
                                                               ALBuffer->FmtType);
            maxstep -= ResamplerPadding[ALSource->Resampler] +
                       ResamplerPrePadding[ALSource->Resampler] + 1;
            maxstep = min(maxstep, INT_MAX>>FRACTIONBITS);

            Pitch = ALfpDiv(ALfpMult(Pitch, int2ALfp(ALBuffer->Frequency)), int2ALfp(Frequency));
            if(Pitch > int2ALfp(maxstep))
                ALSource->Params.Step = maxstep<<FRACTIONBITS;
            else
            {
                ALSource->Params.Step = ALfp2int(ALfpMult(Pitch,float2ALfp(FRACTIONONE)));
                if(ALSource->Params.Step == 0)
                    ALSource->Params.Step = 1;
            }
            break;
        }
        BufferListItem = BufferListItem->next;
    }

    // Use energy-preserving panning algorithm for multi-speaker playback
    length = __max(OrigDist, MinDist);
    if(length > int2ALfp(0))
    {
        ALfp invlen = ALfpDiv(int2ALfp(1), length);
        Position[0] = ALfpMult(Position[0],invlen);
        Position[1] = ALfpMult(Position[1],invlen);
        Position[2] = ALfpMult(Position[2],invlen);
    }

    pos = aluCart2LUTpos((-1*Position[2]), Position[0]);
    SpeakerGain = &Device->PanningLUT[MAXCHANNELS * pos];

    DirGain = aluSqrt((ALfpMult(Position[0],Position[0]) + ALfpMult(Position[2],Position[2])));
    // elevation adjustment for directional gain. this sucks, but
    // has low complexity
    AmbientGain = aluSqrt(float2ALfp(1.0f/Device->NumChan));
    for(s = 0;s < MAXCHANNELS;s++)
    {
        ALuint s2;
        for(s2 = 0;s2 < MAXCHANNELS;s2++)
            ALSource->Params.DryGains[s][s2] = int2ALfp(0);
    }
    for(s = 0;s < (ALsizei)Device->NumChan;s++)
    {
        Channel chan = Device->Speaker2Chan[s];
		ALfp gain; gain = AmbientGain + ALfpMult((SpeakerGain[chan]-AmbientGain),DirGain);
        ALSource->Params.DryGains[0][chan] = ALfpMult(DryGain, gain);
    }

    /* Update filter coefficients. */
    cw = __cos(ALfpDiv(float2ALfp(2.0*M_PI*LOWPASSFREQCUTOFF), int2ALfp(Frequency)));

    /* Spatialized sources use four chained one-pole filters, so we need to
     * take the fourth root of the squared gain, which is the same as the
     * square root of the base gain. */
    ALSource->Params.iirFilter.coeff = lpCoeffCalc(aluSqrt(DryGainHF), cw);

    for(i = 0;i < NumSends;i++)
    {
        /* The wet path uses two chained one-pole filters, so take the
         * base gain (square root of the squared gain) */
        ALSource->Params.Send[i].iirFilter.coeff = lpCoeffCalc(WetGainHF[i], cw);
    }
}


static __inline ALfloat aluF2F(ALfp val)
{
	return ALfp2float(val);
}
static __inline ALushort aluF2US(ALfp val)
{
    if(val > int2ALfp(1)) return 65535;
    if(val < int2ALfp(-1)) return 0;
    return (ALushort)(ALfp2int(ALfpMult(val,int2ALfp(32767))) + 32768);
}
static __inline ALshort aluF2S(ALfp val)
{
    if(val > int2ALfp(1)) return 32767;
    if(val < int2ALfp(-1)) return -32768;
    return (ALshort)(ALfp2int(ALfpMult(val,int2ALfp(32767))));
}
static __inline ALubyte aluF2UB(ALfp val)
{
    ALushort i = aluF2US(val);
    return i>>8;
}
static __inline ALbyte aluF2B(ALfp val)
{
    ALshort i = aluF2S(val);
    return i>>8;
}

static const Channel MonoChans[] = { FRONT_CENTER };
static const Channel StereoChans[] = { FRONT_LEFT, FRONT_RIGHT };
static const Channel QuadChans[] = { FRONT_LEFT, FRONT_RIGHT,
                                     BACK_LEFT, BACK_RIGHT };
static const Channel X51Chans[] = { FRONT_LEFT, FRONT_RIGHT,
                                    FRONT_CENTER, LFE,
                                    BACK_LEFT, BACK_RIGHT };
static const Channel X61Chans[] = { FRONT_LEFT, FRONT_LEFT,
                                    FRONT_CENTER, LFE, BACK_CENTER,
                                    SIDE_LEFT, SIDE_RIGHT };
static const Channel X71Chans[] = { FRONT_LEFT, FRONT_RIGHT,
                                    FRONT_CENTER, LFE,
                                    BACK_LEFT, BACK_RIGHT,
                                    SIDE_LEFT, SIDE_RIGHT };

#define DECL_TEMPLATE(T, chans,N, func)                                       \
static void Write_##T##_##chans(ALCdevice *device, T *buffer, ALuint SamplesToDo)\
{                                                                             \
    ALfp (*DryBuffer)[MAXCHANNELS] = device->DryBuffer;                       \
    ALfp (*Matrix)[MAXCHANNELS] = device->ChannelMatrix;                      \
    const ALuint *ChanMap = device->DevChannels;                              \
    ALuint i, j, c;                                                           \
                                                                              \
    for(i = 0;i < SamplesToDo;i++)                                            \
    {                                                                         \
        for(j = 0;j < N;j++)                                                  \
        {                                                                     \
			ALfp samp; samp = int2ALfp(0);                                    \
            for(c = 0;c < MAXCHANNELS;c++) {                                  \
				ALfp m = Matrix[c][chans[j]];                                 \
                if (m != 0)                                                   \
                    samp += ALfpMult(DryBuffer[i][c], m);                     \
            }                                                                 \
            ((T*)buffer)[ChanMap[chans[j]]] = func(samp);                     \
        }                                                                     \
        buffer = ((T*)buffer) + N;                                            \
    }                                                                         \
}

DECL_TEMPLATE(ALfloat, MonoChans,1, aluF2F)
DECL_TEMPLATE(ALfloat, QuadChans,4, aluF2F)
DECL_TEMPLATE(ALfloat, X51Chans,6, aluF2F)
DECL_TEMPLATE(ALfloat, X61Chans,7, aluF2F)
DECL_TEMPLATE(ALfloat, X71Chans,8, aluF2F)

DECL_TEMPLATE(ALushort, MonoChans,1, aluF2US)
DECL_TEMPLATE(ALushort, QuadChans,4, aluF2US)
DECL_TEMPLATE(ALushort, X51Chans,6, aluF2US)
DECL_TEMPLATE(ALushort, X61Chans,7, aluF2US)
DECL_TEMPLATE(ALushort, X71Chans,8, aluF2US)

DECL_TEMPLATE(ALshort, MonoChans,1, aluF2S)
DECL_TEMPLATE(ALshort, QuadChans,4, aluF2S)
DECL_TEMPLATE(ALshort, X51Chans,6, aluF2S)
DECL_TEMPLATE(ALshort, X61Chans,7, aluF2S)
DECL_TEMPLATE(ALshort, X71Chans,8, aluF2S)

DECL_TEMPLATE(ALubyte, MonoChans,1, aluF2UB)
DECL_TEMPLATE(ALubyte, QuadChans,4, aluF2UB)
DECL_TEMPLATE(ALubyte, X51Chans,6, aluF2UB)
DECL_TEMPLATE(ALubyte, X61Chans,7, aluF2UB)
DECL_TEMPLATE(ALubyte, X71Chans,8, aluF2UB)

DECL_TEMPLATE(ALbyte, MonoChans,1, aluF2B)
DECL_TEMPLATE(ALbyte, QuadChans,4, aluF2B)
DECL_TEMPLATE(ALbyte, X51Chans,6, aluF2B)
DECL_TEMPLATE(ALbyte, X61Chans,7, aluF2B)
DECL_TEMPLATE(ALbyte, X71Chans,8, aluF2B)

#undef DECL_TEMPLATE

#define DECL_TEMPLATE(T, chans,N, func)                                       \
static void Write_##T##_##chans(ALCdevice *device, T *buffer, ALuint SamplesToDo)\
{                                                                             \
    ALfp (*DryBuffer)[MAXCHANNELS] = device->DryBuffer;                       \
    ALfp (*Matrix)[MAXCHANNELS] = device->ChannelMatrix;                      \
    const ALuint *ChanMap = device->DevChannels;                              \
    ALuint i, j, c;                                                           \
                                                                              \
    if(device->Bs2b)                                                          \
    {                                                                         \
        for(i = 0;i < SamplesToDo;i++)                                        \
        {                                                                     \
            ALfp samples[2] = { int2ALfp(0), int2ALfp(0) };                   \
            for(c = 0;c < MAXCHANNELS;c++)                                    \
            {                                                                 \
                samples[0] += ALfpMult(DryBuffer[i][c],Matrix[c][FRONT_LEFT]); \
                samples[1] += ALfpMult(DryBuffer[i][c],Matrix[c][FRONT_RIGHT]); \
            }                                                                 \
            bs2b_cross_feed(device->Bs2b, samples);                           \
            ((T*)buffer)[ChanMap[FRONT_LEFT]]  = func(samples[0]);            \
            ((T*)buffer)[ChanMap[FRONT_RIGHT]] = func(samples[1]);            \
            buffer = ((T*)buffer) + 2;                                        \
        }                                                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
        for(i = 0;i < SamplesToDo;i++)                                        \
        {                                                                     \
            for(j = 0;j < N;j++)                                              \
            {                                                                 \
                ALfp samp = int2ALfp(0);                                      \
                for(c = 0;c < MAXCHANNELS;c++)                                \
                    samp += ALfpMult(DryBuffer[i][c], Matrix[c][chans[j]]);   \
                ((T*)buffer)[ChanMap[chans[j]]] = func(samp);                 \
            }                                                                 \
            buffer = ((T*)buffer) + N;                                        \
        }                                                                     \
    }                                                                         \
}

DECL_TEMPLATE(ALfloat, StereoChans,2, aluF2F)
DECL_TEMPLATE(ALushort, StereoChans,2, aluF2US)
DECL_TEMPLATE(ALshort, StereoChans,2, aluF2S)
DECL_TEMPLATE(ALubyte, StereoChans,2, aluF2UB)
DECL_TEMPLATE(ALbyte, StereoChans,2, aluF2B)

#undef DECL_TEMPLATE

#define DECL_TEMPLATE(T, func)                                                \
static void Write_##T(ALCdevice *device, T *buffer, ALuint SamplesToDo)       \
{                                                                             \
    switch(device->FmtChans)                                                  \
    {                                                                         \
        case DevFmtMono:                                                      \
            Write_##T##_MonoChans(device, buffer, SamplesToDo);               \
            break;                                                            \
        case DevFmtStereo:                                                    \
            Write_##T##_StereoChans(device, buffer, SamplesToDo);             \
            break;                                                            \
        case DevFmtQuad:                                                      \
            Write_##T##_QuadChans(device, buffer, SamplesToDo);               \
            break;                                                            \
        case DevFmtX51:                                                       \
            Write_##T##_X51Chans(device, buffer, SamplesToDo);                \
            break;                                                            \
        case DevFmtX61:                                                       \
            Write_##T##_X61Chans(device, buffer, SamplesToDo);                \
            break;                                                            \
        case DevFmtX71:                                                       \
            Write_##T##_X71Chans(device, buffer, SamplesToDo);                \
            break;                                                            \
    }                                                                         \
}

DECL_TEMPLATE(ALfloat, aluF2F)
DECL_TEMPLATE(ALushort, aluF2US)
DECL_TEMPLATE(ALshort, aluF2S)
DECL_TEMPLATE(ALubyte, aluF2UB)
DECL_TEMPLATE(ALbyte, aluF2B)

#undef DECL_TEMPLATE

static __inline ALvoid aluMixDataPrivate(ALCdevice *device, ALvoid *buffer, ALsizei size)
{
    ALuint SamplesToDo;
    ALeffectslot *ALEffectSlot;
    ALCcontext **ctx, **ctx_end;
    ALsource **src, **src_end;
    int fpuState;
    ALuint i, c;
    ALsizei e;

#if defined(HAVE_FESETROUND)
    fpuState = fegetround();
    fesetround(FE_TOWARDZERO);
#elif defined(HAVE__CONTROLFP)
    fpuState = _controlfp(_RC_CHOP, _MCW_RC);
#else
    (void)fpuState;
#endif

    while(size > 0)
    {
        /* Setup variables */
        SamplesToDo = min(size, BUFFERSIZE);

        /* Clear mixing buffer */
        memset(device->DryBuffer, 0, SamplesToDo*MAXCHANNELS*sizeof(ALfp));

        SuspendContext(NULL);
        ctx = device->Contexts;
        ctx_end = ctx + device->NumContexts;
        while(ctx != ctx_end)
        {
            SuspendContext(*ctx);

            src = (*ctx)->ActiveSources;
            src_end = src + (*ctx)->ActiveSourceCount;
            while(src != src_end)
            {
                if((*src)->state != AL_PLAYING)
                {
                    --((*ctx)->ActiveSourceCount);
                    *src = *(--src_end);
                    continue;
                }

                if((*src)->NeedsUpdate)
                {
                    ALsource_Update(*src, *ctx);
                    (*src)->NeedsUpdate = AL_FALSE;
                }

                MixSource(*src, device, SamplesToDo);
                src++;
            }

            /* effect slot processing */
            for(e = 0;e < (*ctx)->EffectSlotMap.size;e++)
            {
                ALEffectSlot = (*ctx)->EffectSlotMap.array[e].value;

                for(i = 0;i < SamplesToDo;i++)
                {
                    ALEffectSlot->ClickRemoval[0] -= ALfpDiv(ALEffectSlot->ClickRemoval[0], int2ALfp(256));
                    ALEffectSlot->WetBuffer[i] += ALEffectSlot->ClickRemoval[0];
                }
                for(i = 0;i < 1;i++)
                {
                    ALEffectSlot->ClickRemoval[i] += ALEffectSlot->PendingClicks[i];
                    ALEffectSlot->PendingClicks[i] = int2ALfp(0);
                }

                ALEffect_Process(ALEffectSlot->EffectState, ALEffectSlot,
                                 SamplesToDo, ALEffectSlot->WetBuffer,
                                 device->DryBuffer);

                for(i = 0;i < SamplesToDo;i++)
                    ALEffectSlot->WetBuffer[i] = int2ALfp(0);
            }

            ProcessContext(*ctx);
            ctx++;
        }
        ProcessContext(NULL);

        //Post processing loop
        for(i = 0;i < SamplesToDo;i++)
        {
            for(c = 0;c < MAXCHANNELS;c++)
            {
                device->ClickRemoval[c] -= ALfpDiv(device->ClickRemoval[c], int2ALfp(256));
                device->DryBuffer[i][c] += device->ClickRemoval[c];
            }
        }
        for(i = 0;i < MAXCHANNELS;i++)
        {
            device->ClickRemoval[i] += device->PendingClicks[i];
            device->PendingClicks[i] = int2ALfp(0);
        }

        switch(device->FmtType)
        {
            case DevFmtByte:
                Write_ALbyte(device, buffer, SamplesToDo);
                break;
            case DevFmtUByte:
                Write_ALubyte(device, buffer, SamplesToDo);
                break;
            case DevFmtShort:
                Write_ALshort(device, buffer, SamplesToDo);
                break;
            case DevFmtUShort:
                Write_ALushort(device, buffer, SamplesToDo);
                break;
            case DevFmtFloat:
                Write_ALfloat(device, buffer, SamplesToDo);
                break;
        }

        size -= SamplesToDo;
    }

#if defined(HAVE_FESETROUND)
    fesetround(fpuState);
#elif defined(HAVE__CONTROLFP)
    _controlfp(fpuState, _MCW_RC);
#endif
}

static inline long timespecdiff(struct timespec *starttime, struct timespec *finishtime)
{
  long usec;
  usec=(finishtime->tv_sec-starttime->tv_sec)*1000000;
  usec+=(finishtime->tv_nsec-starttime->tv_nsec)/1000;
  return usec;
}

ALvoid aluMixData(ALCdevice *device, ALvoid *buffer, ALsizei size)
{
#ifdef MAX_SOURCES_LOW
    // Profile aluMixDataPrivate to set admission control parameters
    static struct timespec ts_start;
    static struct timespec ts_end;
    long ts_diff;
    int time_per_source;
    int max_sources_within_deadline;
	int mix_deadline_usec;
	int max;
	
	if (alc_num_cores == 0) {
		// FIXME(Apportable) this is Linux specific
		alc_num_cores = sysconf( _SC_NPROCESSORS_ONLN );
		LOGI("_SC_NPROCESSORS_ONLN=%d", alc_num_cores);
	}

	if (alc_num_cores > 1) {
		// Allow OpenAL to monopolize one core
		mix_deadline_usec = ((size*1000000) / device->Frequency) / 2;
	} else {
		// Try to cap mixing at 20% CPU
		mix_deadline_usec = ((size*1000000) / device->Frequency) / 5;
	}

    clock_gettime(CLOCK_MONOTONIC, &ts_start);
	aluMixDataPrivate(device, buffer,  size);
    clock_gettime(CLOCK_MONOTONIC, &ts_end);

    // Time in micro-seconds that aluMixData has taken to run
    ts_diff = timespecdiff(&ts_start, &ts_end);

    // Try to adjust the max sources limit adaptively, within a range
    if (alc_active_sources > 0) {
		time_per_source = max(1, ts_diff / alc_active_sources);
		max_sources_within_deadline = mix_deadline_usec / time_per_source;
		max = min(max(max_sources_within_deadline, MAX_SOURCES_LOW), MAX_SOURCES_HIGH);
		if (max > alc_max_sources) {
			alc_max_sources++;
		} else if (max < alc_max_sources) {
			alc_max_sources = max;
		}
    } else {
    	alc_max_sources = MAX_SOURCES_START;
    }
#else
	aluMixDataPrivate(device, buffer,  size);
#endif
}

ALvoid aluHandleDisconnect(ALCdevice *device)
{
    ALuint i;

    SuspendContext(NULL);
    for(i = 0;i < device->NumContexts;i++)
    {
        ALCcontext *Context = device->Contexts[i];
        ALsource *source;
        ALsizei pos;

        SuspendContext(Context);

        for(pos = 0;pos < Context->SourceMap.size;pos++)
        {
            source = Context->SourceMap.array[pos].value;
            if(source->state == AL_PLAYING)
            {
                source->state = AL_STOPPED;
                source->BuffersPlayed = source->BuffersInQueue;
                source->position = 0;
                source->position_fraction = 0;
            }
        }
        ProcessContext(Context);
    }

    device->Connected = ALC_FALSE;
    ProcessContext(NULL);
}
