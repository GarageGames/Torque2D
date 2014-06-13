/**
 * OpenAL cross platform audio library
 * Copyright (C) 2009 by Chris Robinson.
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

#include "alMain.h"
#include "alFilter.h"
#include "alAuxEffectSlot.h"
#include "alError.h"
#include "alu.h"


typedef struct ALechoState {
    // Must be first in all effects!
    ALeffectState state;

    ALfp *SampleBuffer;
    ALuint BufferLength;

    // The echo is two tap. The delay is the number of samples from before the
    // current offset
    struct {
        ALuint delay;
    } Tap[2];
    ALuint Offset;
    // The LR gains for the first tap. The second tap uses the reverse
    ALfp GainL;
    ALfp GainR;

    ALfp FeedGain;

    ALfp Gain[MAXCHANNELS];

    FILTER iirFilter;
    ALfp history[2];
} ALechoState;

static ALvoid EchoDestroy(ALeffectState *effect)
{
    ALechoState *state = (ALechoState*)effect;
    if(state)
    {
        free(state->SampleBuffer);
        state->SampleBuffer = NULL;
        free(state);
    }
}

static ALboolean EchoDeviceUpdate(ALeffectState *effect, ALCdevice *Device)
{
    ALechoState *state = (ALechoState*)effect;
    ALuint maxlen, i;

    // Use the next power of 2 for the buffer length, so the tap offsets can be
    // wrapped using a mask instead of a modulo
    maxlen  = (ALuint)(AL_ECHO_MAX_DELAY * Device->Frequency) + 1;
    maxlen += (ALuint)(AL_ECHO_MAX_LRDELAY * Device->Frequency) + 1;
    maxlen  = NextPowerOf2(maxlen);

    if(maxlen != state->BufferLength)
    {
        void *temp;

        temp = realloc(state->SampleBuffer, maxlen * sizeof(ALfp));
        if(!temp)
            return AL_FALSE;
        state->SampleBuffer = temp;
        state->BufferLength = maxlen;
    }
    for(i = 0;i < state->BufferLength;i++)
        state->SampleBuffer[i] = int2ALfp(0);

    for(i = 0;i < MAXCHANNELS;i++)
        state->Gain[i] = int2ALfp(0);
    for(i = 0;i < Device->NumChan;i++)
    {
        Channel chan = Device->Speaker2Chan[i];
        state->Gain[chan] = int2ALfp(1);
    }

    return AL_TRUE;
}

static ALvoid EchoUpdate(ALeffectState *effect, ALCcontext *Context, const ALeffect *Effect)
{
    ALechoState *state = (ALechoState*)effect;
    ALuint frequency = Context->Device->Frequency;
    ALfp lrpan, cw, a, g;

    state->Tap[0].delay = (ALuint)ALfp2int((ALfpMult(Effect->Echo.Delay, int2ALfp(frequency)) + int2ALfp(1)));
    state->Tap[1].delay = (ALuint)ALfp2int(ALfpMult(Effect->Echo.LRDelay, int2ALfp(frequency)));
    state->Tap[1].delay += state->Tap[0].delay;

    lrpan = (ALfpMult(Effect->Echo.Spread, float2ALfp(0.5f)) + float2ALfp(0.5f));
    state->GainL = aluSqrt(     lrpan);
    state->GainR = aluSqrt((int2ALfp(1)-lrpan));

    state->FeedGain = Effect->Echo.Feedback;

    cw = __cos(ALfpDiv(float2ALfp(2.0*M_PI * LOWPASSFREQCUTOFF), int2ALfp(frequency)));
    g = (int2ALfp(1) - Effect->Echo.Damping);
    a = int2ALfp(0);
    if(g < float2ALfp(0.9999f)) /* 1-epsilon */ {
		// a = (1 - g*cw - aluSqrt(2*g*(1-cw) - g*g*(1 - cw*cw))) / (1 - g);
        a = ALfpDiv((int2ALfp(1) - ALfpMult(g,cw) - aluSqrt((ALfpMult(ALfpMult(int2ALfp(2),g),(int2ALfp(1)-cw)) -
									                        ALfpMult(ALfpMult(g,g),(int2ALfp(1) - ALfpMult(cw,cw)))))),
                    (int2ALfp(1) - g));
	}
    state->iirFilter.coeff = a;
}

static ALvoid EchoProcess(ALeffectState *effect, const ALeffectslot *Slot, ALuint SamplesToDo, const ALfp *SamplesIn, ALfp (*SamplesOut)[MAXCHANNELS])
{
    ALechoState *state = (ALechoState*)effect;
    const ALuint mask = state->BufferLength-1;
    const ALuint tap1 = state->Tap[0].delay;
    const ALuint tap2 = state->Tap[1].delay;
    ALuint offset = state->Offset;
    const ALfp gain = Slot->Gain;
    ALfp samp[2], smp;
    ALuint i;

    for(i = 0;i < SamplesToDo;i++,offset++)
    {
        // Sample first tap
        smp = state->SampleBuffer[(offset-tap1) & mask];
        samp[0] = ALfpMult(smp, state->GainL);
        samp[1] = ALfpMult(smp, state->GainR);
        // Sample second tap. Reverse LR panning
        smp = state->SampleBuffer[(offset-tap2) & mask];
        samp[0] += ALfpMult(smp, state->GainR);
        samp[1] += ALfpMult(smp, state->GainL);

        // Apply damping and feedback gain to the second tap, and mix in the
        // new sample
        smp = lpFilter2P(&state->iirFilter, 0, (smp+SamplesIn[i]));
        state->SampleBuffer[offset&mask] = ALfpMult(smp, state->FeedGain);

        // Apply slot gain
        samp[0] = ALfpMult(samp[0], gain);
        samp[1] = ALfpMult(samp[1], gain);

        SamplesOut[i][FRONT_LEFT]  += ALfpMult(state->Gain[FRONT_LEFT],  samp[0]);
        SamplesOut[i][FRONT_RIGHT] += ALfpMult(state->Gain[FRONT_RIGHT], samp[1]);
#ifdef APPORTABLE_OPTIMIZED_OUT
        SamplesOut[i][SIDE_LEFT]   += ALfpMult(state->Gain[SIDE_LEFT],   samp[0]);
        SamplesOut[i][SIDE_RIGHT]  += ALfpMult(state->Gain[SIDE_RIGHT],  samp[1]);
        SamplesOut[i][BACK_LEFT]   += ALfpMult(state->Gain[BACK_LEFT],   samp[0]);
        SamplesOut[i][BACK_RIGHT]  += ALfpMult(state->Gain[BACK_RIGHT],  samp[1]);
#endif

    }
    state->Offset = offset;
}

ALeffectState *EchoCreate(void)
{
    ALechoState *state;

    state = malloc(sizeof(*state));
    if(!state)
        return NULL;

    state->state.Destroy = EchoDestroy;
    state->state.DeviceUpdate = EchoDeviceUpdate;
    state->state.Update = EchoUpdate;
    state->state.Process = EchoProcess;

    state->BufferLength = 0;
    state->SampleBuffer = NULL;

    state->Tap[0].delay = 0;
    state->Tap[1].delay = 0;
    state->Offset = 0;
    state->GainL = int2ALfp(0);
    state->GainR = int2ALfp(0);

    state->iirFilter.coeff = int2ALfp(0);
    state->iirFilter.history[0] = int2ALfp(0);
    state->iirFilter.history[1] = int2ALfp(0);

    return &state->state;
}
