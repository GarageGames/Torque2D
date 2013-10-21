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


typedef struct ALmodulatorState {
    // Must be first in all effects!
    ALeffectState state;

    enum {
        SINUSOID,
        SAWTOOTH,
        SQUARE
    } Waveform;

    ALuint index;
    ALuint step;

    ALfp Gain[MAXCHANNELS];

    FILTER iirFilter;
    ALfp history[1];
} ALmodulatorState;

#define WAVEFORM_FRACBITS  16
#define WAVEFORM_FRACMASK  ((1<<WAVEFORM_FRACBITS)-1)

static __inline ALfp sin_func(ALuint index)
{
    return __sin(ALdfpMult(ALdfpDiv(int2ALdfp(index),double2ALdfp(1<<WAVEFORM_FRACBITS)), double2ALdfp(M_PI * 2.0f)));
}

static __inline ALfp saw_func(ALuint index)
{
    return (ALfpDiv(int2ALfp(index*2), int2ALfp(1<<WAVEFORM_FRACBITS)) - int2ALfp(1));
}

static __inline ALfp square_func(ALuint index)
{
    return int2ALfp((float)((index>>(WAVEFORM_FRACBITS-1))&1) ? -1 : 1);
}


static __inline ALfp hpFilter1P(FILTER *iir, ALuint offset, ALfp input)
{
    ALfp *history = &iir->history[offset];
    ALfp a = iir->coeff;
    ALfp output = input;

    output = (output + ALfpMult((history[0]-output),a));
    history[0] = output;

    return (input - output);
}


static ALvoid ModulatorDestroy(ALeffectState *effect)
{
    ALmodulatorState *state = (ALmodulatorState*)effect;
    free(state);
}

static ALboolean ModulatorDeviceUpdate(ALeffectState *effect, ALCdevice *Device)
{
    ALmodulatorState *state = (ALmodulatorState*)effect;
    ALuint index;

    for(index = 0;index < MAXCHANNELS;index++)
        state->Gain[index] = int2ALfp(0);
    for(index = 0;index < Device->NumChan;index++)
    {
        Channel chan = Device->Speaker2Chan[index];
        state->Gain[chan] = int2ALfp(1);
    }

    return AL_TRUE;
}

static ALvoid ModulatorUpdate(ALeffectState *effect, ALCcontext *Context, const ALeffect *Effect)
{
    ALmodulatorState *state = (ALmodulatorState*)effect;
	ALfp cw, a;
	a = int2ALfp(0);

    if(Effect->Modulator.Waveform == AL_RING_MODULATOR_SINUSOID)
        state->Waveform = SINUSOID;
    else if(Effect->Modulator.Waveform == AL_RING_MODULATOR_SAWTOOTH)
        state->Waveform = SAWTOOTH;
    else if(Effect->Modulator.Waveform == AL_RING_MODULATOR_SQUARE)
        state->Waveform = SQUARE;

    state->step = ALfp2int(ALfpDiv(ALfpMult(Effect->Modulator.Frequency,
                                            int2ALfp(1<<WAVEFORM_FRACBITS)),
                                   int2ALfp(Context->Device->Frequency)));
    if(!state->step)
        state->step = 1;

    cw = __cos(ALfpDiv(ALfpMult(float2ALfp(2.0*M_PI),
                                Effect->Modulator.HighPassCutoff),
                       int2ALfp(Context->Device->Frequency)));
    a = ((int2ALfp(2)-cw) -
         aluSqrt((aluPow((int2ALfp(2)-cw), int2ALfp(2)) - int2ALfp(1))));
    state->iirFilter.coeff = a;
}

static ALvoid ModulatorProcess(ALeffectState *effect, const ALeffectslot *Slot, ALuint SamplesToDo, const ALfp *SamplesIn, ALfp (*SamplesOut)[MAXCHANNELS])
{
    ALmodulatorState *state = (ALmodulatorState*)effect;
    const ALfp gain = Slot->Gain;
    const ALuint step = state->step;
    ALuint index = state->index;
    ALfp samp;
    ALuint i;

    switch(state->Waveform)
    {
    case SINUSOID:
        for(i = 0;i < SamplesToDo;i++)
        {
#ifdef APPORTABLE_OPTIMIZED_OUT
#define FILTER_OUT(func) do {                                                 \
    samp = SamplesIn[i];                                                      \
                                                                              \
    index += step;                                                            \
    index &= WAVEFORM_FRACMASK;                                               \
    samp *= func(index);                                                      \
                                                                              \
    samp = hpFilter1P(&state->iirFilter, 0, samp);                            \
                                                                              \
    /* Apply slot gain */                                                     \
    samp *= gain;                                                             \
                                                                              \
    SamplesOut[i][FRONT_LEFT]   += state->Gain[FRONT_LEFT]   * samp;          \
    SamplesOut[i][FRONT_RIGHT]  += state->Gain[FRONT_RIGHT]  * samp;          \
    SamplesOut[i][FRONT_CENTER] += state->Gain[FRONT_CENTER] * samp;          \
    SamplesOut[i][SIDE_LEFT]    += state->Gain[SIDE_LEFT]    * samp;          \
    SamplesOut[i][SIDE_RIGHT]   += state->Gain[SIDE_RIGHT]   * samp;          \
    SamplesOut[i][BACK_LEFT]    += state->Gain[BACK_LEFT]    * samp;          \
    SamplesOut[i][BACK_RIGHT]   += state->Gain[BACK_RIGHT]   * samp;          \
    SamplesOut[i][BACK_CENTER]  += state->Gain[BACK_CENTER]  * samp;          \
} while(0)
#else      	
//Apportable optimized version
#define FILTER_OUT(func) do {                                                 \
    samp = SamplesIn[i];                                                      \
                                                                              \
    index += step;                                                            \
    index &= WAVEFORM_FRACMASK;                                               \
    samp = ALfpMult(samp, func(index));                                       \
                                                                              \
    samp = hpFilter1P(&state->iirFilter, 0, samp);                            \
                                                                              \
    /* Apply slot gain */                                                     \
    samp = ALfpMult(samp, gain);                                              \
                                                                              \
    SamplesOut[i][FRONT_LEFT]   += ALfpMult(state->Gain[FRONT_LEFT], samp);   \
    SamplesOut[i][FRONT_RIGHT]  += ALfpMult(state->Gain[FRONT_RIGHT], samp);  \
} while(0)
#endif	
            FILTER_OUT(sin_func);
        }
        break;

    case SAWTOOTH:
        for(i = 0;i < SamplesToDo;i++)
        {
            FILTER_OUT(saw_func);
        }
        break;

    case SQUARE:
        for(i = 0;i < SamplesToDo;i++)
        {
            FILTER_OUT(square_func);
#undef FILTER_OUT
        }
        break;
    }
    state->index = index;
}

ALeffectState *ModulatorCreate(void)
{
    ALmodulatorState *state;

    state = malloc(sizeof(*state));
    if(!state)
        return NULL;

    state->state.Destroy = ModulatorDestroy;
    state->state.DeviceUpdate = ModulatorDeviceUpdate;
    state->state.Update = ModulatorUpdate;
    state->state.Process = ModulatorProcess;

    state->index = 0;
    state->step = 1;

    state->iirFilter.coeff = int2ALfp(0);
    state->iirFilter.history[0] = int2ALfp(0);

    return &state->state;
}
