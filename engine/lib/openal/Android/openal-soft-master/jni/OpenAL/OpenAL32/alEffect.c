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

#include "AL/al.h"
#include "AL/alc.h"
#include "alMain.h"
#include "alEffect.h"
#include "alThunk.h"
#include "alError.h"


ALboolean DisabledEffects[MAX_EFFECTS];


static void InitEffectParams(ALeffect *effect, ALenum type);

#define LookupEffect(m, k) ((ALeffect*)LookupUIntMapKey(&(m), (k)))

AL_API ALvoid AL_APIENTRY alGenEffects(ALsizei n, ALuint *effects)
{
    ALCcontext *Context;
    ALsizei i=0;

    Context = GetContextSuspended();
    if(!Context) return;

    if(n < 0 || IsBadWritePtr((void*)effects, n * sizeof(ALuint)))
        alSetError(Context, AL_INVALID_VALUE);
    else
    {
        ALCdevice *device = Context->Device;
        ALenum err;

        while(i < n)
        {
            ALeffect *effect = calloc(1, sizeof(ALeffect));
            if(!effect)
            {
                alSetError(Context, AL_OUT_OF_MEMORY);
                alDeleteEffects(i, effects);
                break;
            }

            effect->effect = ALTHUNK_ADDENTRY(effect);
            err = InsertUIntMapEntry(&device->EffectMap, effect->effect, effect);
            if(err != AL_NO_ERROR)
            {
                ALTHUNK_REMOVEENTRY(effect->effect);
                memset(effect, 0, sizeof(ALeffect));
                free(effect);

                alSetError(Context, err);
                alDeleteEffects(i, effects);
                break;
            }

            effects[i++] = effect->effect;
            InitEffectParams(effect, AL_EFFECT_NULL);
        }
    }

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alDeleteEffects(ALsizei n, ALuint *effects)
{
    ALCcontext *Context;
    ALCdevice *device;
    ALeffect *ALEffect;
    ALboolean Failed;
    ALsizei i;

    Context = GetContextSuspended();
    if(!Context) return;

    Failed = AL_TRUE;
    device = Context->Device;
    if(n < 0)
        alSetError(Context, AL_INVALID_VALUE);
    else
    {
        Failed = AL_FALSE;
        // Check that all effects are valid
        for(i = 0;i < n;i++)
        {
            if(!effects[i])
                continue;

            if(LookupEffect(device->EffectMap, effects[i]) == NULL)
            {
                alSetError(Context, AL_INVALID_NAME);
                Failed = AL_TRUE;
                break;
            }
        }
    }

    if(!Failed)
    {
        // All effects are valid
        for(i = 0;i < n;i++)
        {
            // Recheck that the effect is valid, because there could be duplicated names
            if((ALEffect=LookupEffect(device->EffectMap, effects[i])) == NULL)
                continue;

            RemoveUIntMapKey(&device->EffectMap, ALEffect->effect);
            ALTHUNK_REMOVEENTRY(ALEffect->effect);

            memset(ALEffect, 0, sizeof(ALeffect));
            free(ALEffect);
        }
    }

    ProcessContext(Context);
}

AL_API ALboolean AL_APIENTRY alIsEffect(ALuint effect)
{
    ALCcontext *Context;
    ALboolean  result;

    Context = GetContextSuspended();
    if(!Context) return AL_FALSE;

    result = ((!effect || LookupEffect(Context->Device->EffectMap, effect)) ?
              AL_TRUE : AL_FALSE);

    ProcessContext(Context);

    return result;
}

AL_API ALvoid AL_APIENTRY alEffecti(ALuint effect, ALenum param, ALint iValue)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALeffect   *ALEffect;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALEffect=LookupEffect(Device->EffectMap, effect)) != NULL)
    {
        if(param == AL_EFFECT_TYPE)
        {
            ALboolean isOk = (iValue == AL_EFFECT_NULL ||
                (iValue == AL_EFFECT_EAXREVERB && !DisabledEffects[EAXREVERB]) ||
                (iValue == AL_EFFECT_REVERB && !DisabledEffects[REVERB]) ||
                (iValue == AL_EFFECT_ECHO && !DisabledEffects[ECHO]) ||
                (iValue == AL_EFFECT_RING_MODULATOR && !DisabledEffects[MODULATOR]));

            if(isOk)
                InitEffectParams(ALEffect, iValue);
            else
                alSetError(Context, AL_INVALID_VALUE);
        }
        else if(ALEffect->type == AL_EFFECT_EAXREVERB)
        {
            switch(param)
            {
            case AL_EAXREVERB_DECAY_HFLIMIT:
                if(iValue >= AL_EAXREVERB_MIN_DECAY_HFLIMIT &&
                   iValue <= AL_EAXREVERB_MAX_DECAY_HFLIMIT)
                    ALEffect->Reverb.DecayHFLimit = iValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_REVERB)
        {
            switch(param)
            {
            case AL_REVERB_DECAY_HFLIMIT:
                if(iValue >= AL_REVERB_MIN_DECAY_HFLIMIT &&
                   iValue <= AL_REVERB_MAX_DECAY_HFLIMIT)
                    ALEffect->Reverb.DecayHFLimit = iValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_ECHO)
        {
            switch(param)
            {
            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_RING_MODULATOR)
        {
            switch(param)
            {
            case AL_RING_MODULATOR_FREQUENCY:
            case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
                alEffectf(effect, param, (ALfloat)iValue);
                break;

            case AL_RING_MODULATOR_WAVEFORM:
                if(iValue >= AL_RING_MODULATOR_MIN_WAVEFORM &&
                   iValue <= AL_RING_MODULATOR_MAX_WAVEFORM)
                    ALEffect->Modulator.Waveform = iValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else
            alSetError(Context, AL_INVALID_ENUM);
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alEffectiv(ALuint effect, ALenum param, ALint *piValues)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALeffect   *ALEffect;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALEffect=LookupEffect(Device->EffectMap, effect)) != NULL)
    {
        if(param == AL_EFFECT_TYPE)
        {
            alEffecti(effect, param, piValues[0]);
        }
        else if(ALEffect->type == AL_EFFECT_EAXREVERB)
        {
            switch(param)
            {
            case AL_EAXREVERB_DECAY_HFLIMIT:
                alEffecti(effect, param, piValues[0]);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_REVERB)
        {
            switch(param)
            {
            case AL_REVERB_DECAY_HFLIMIT:
                alEffecti(effect, param, piValues[0]);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_ECHO)
        {
            switch(param)
            {
            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_RING_MODULATOR)
        {
            switch(param)
            {
            case AL_RING_MODULATOR_FREQUENCY:
            case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
            case AL_RING_MODULATOR_WAVEFORM:
                alEffecti(effect, param, piValues[0]);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else
            alSetError(Context, AL_INVALID_ENUM);
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alEffectf(ALuint effect, ALenum param, ALfloat flArg)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALeffect   *ALEffect;
	ALfp     flValue = float2ALfp(flArg);

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALEffect=LookupEffect(Device->EffectMap, effect)) != NULL)
    {
        if(ALEffect->type == AL_EFFECT_EAXREVERB)
        {
            switch(param)
            {
            case AL_EAXREVERB_DENSITY:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_DENSITY) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_DENSITY))
                    ALEffect->Reverb.Density = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_DIFFUSION:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_DIFFUSION) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_DIFFUSION))
                    ALEffect->Reverb.Diffusion = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_GAIN:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_GAIN) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_GAIN))
                    ALEffect->Reverb.Gain = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_GAINHF:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_GAINHF) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_GAIN))
                    ALEffect->Reverb.GainHF = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_GAINLF:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_GAINLF) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_GAINLF))
                    ALEffect->Reverb.GainLF = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_DECAY_TIME:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_DECAY_TIME) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_DECAY_TIME))
                    ALEffect->Reverb.DecayTime = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_DECAY_HFRATIO:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_DECAY_HFRATIO) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_DECAY_HFRATIO))
                    ALEffect->Reverb.DecayHFRatio = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_DECAY_LFRATIO:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_DECAY_LFRATIO) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_DECAY_LFRATIO))
                    ALEffect->Reverb.DecayLFRatio = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_REFLECTIONS_GAIN:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_REFLECTIONS_GAIN) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_REFLECTIONS_GAIN))
                    ALEffect->Reverb.ReflectionsGain = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_REFLECTIONS_DELAY:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_REFLECTIONS_DELAY) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_REFLECTIONS_DELAY))
                    ALEffect->Reverb.ReflectionsDelay = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_LATE_REVERB_GAIN:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_LATE_REVERB_GAIN) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_LATE_REVERB_GAIN))
                    ALEffect->Reverb.LateReverbGain = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_LATE_REVERB_DELAY:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_LATE_REVERB_DELAY) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_LATE_REVERB_DELAY))
                    ALEffect->Reverb.LateReverbDelay = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_AIR_ABSORPTION_GAINHF:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_AIR_ABSORPTION_GAINHF) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_AIR_ABSORPTION_GAINHF))
                    ALEffect->Reverb.AirAbsorptionGainHF = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_ECHO_TIME:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_ECHO_TIME) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_ECHO_TIME))
                    ALEffect->Reverb.EchoTime = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_ECHO_DEPTH:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_ECHO_DEPTH) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_ECHO_DEPTH))
                    ALEffect->Reverb.EchoDepth = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_MODULATION_TIME:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_MODULATION_TIME) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_MODULATION_TIME))
                    ALEffect->Reverb.ModulationTime = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_MODULATION_DEPTH:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_MODULATION_DEPTH) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_MODULATION_DEPTH))
                    ALEffect->Reverb.ModulationDepth = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_HFREFERENCE:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_HFREFERENCE) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_HFREFERENCE))
                    ALEffect->Reverb.HFReference = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_LFREFERENCE:
                if(flValue >= float2ALfp(AL_EAXREVERB_MIN_LFREFERENCE) &&
                   flValue <= float2ALfp(AL_EAXREVERB_MAX_LFREFERENCE))
                    ALEffect->Reverb.LFReference = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_EAXREVERB_ROOM_ROLLOFF_FACTOR:
                if(flValue >= float2ALfp(0.0f) && flValue <= float2ALfp(10.0f))
                    ALEffect->Reverb.RoomRolloffFactor = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_REVERB)
        {
            switch(param)
            {
            case AL_REVERB_DENSITY:
                if(flValue >= float2ALfp(AL_REVERB_MIN_DENSITY) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_DENSITY))
                    ALEffect->Reverb.Density = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_DIFFUSION:
                if(flValue >= float2ALfp(AL_REVERB_MIN_DIFFUSION) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_DIFFUSION))
                    ALEffect->Reverb.Diffusion = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_GAIN:
                if(flValue >= float2ALfp(AL_REVERB_MIN_GAIN) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_GAIN))
                    ALEffect->Reverb.Gain = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_GAINHF:
                if(flValue >= float2ALfp(AL_REVERB_MIN_GAINHF) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_GAINHF))
                    ALEffect->Reverb.GainHF = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_DECAY_TIME:
                if(flValue >= float2ALfp(AL_REVERB_MIN_DECAY_TIME) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_DECAY_TIME))
                    ALEffect->Reverb.DecayTime = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_DECAY_HFRATIO:
                if(flValue >= float2ALfp(AL_REVERB_MIN_DECAY_HFRATIO) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_DECAY_HFRATIO))
                    ALEffect->Reverb.DecayHFRatio = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_REFLECTIONS_GAIN:
                if(flValue >= float2ALfp(AL_REVERB_MIN_REFLECTIONS_GAIN) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_REFLECTIONS_GAIN))
                    ALEffect->Reverb.ReflectionsGain = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_REFLECTIONS_DELAY:
                if(flValue >= float2ALfp(AL_REVERB_MIN_REFLECTIONS_DELAY) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_REFLECTIONS_DELAY))
                    ALEffect->Reverb.ReflectionsDelay = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_LATE_REVERB_GAIN:
                if(flValue >= float2ALfp(AL_REVERB_MIN_LATE_REVERB_GAIN) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_LATE_REVERB_GAIN))
                    ALEffect->Reverb.LateReverbGain = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_LATE_REVERB_DELAY:
                if(flValue >= float2ALfp(AL_REVERB_MIN_LATE_REVERB_DELAY) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_LATE_REVERB_DELAY))
                    ALEffect->Reverb.LateReverbDelay = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_AIR_ABSORPTION_GAINHF:
                if(flValue >= float2ALfp(AL_REVERB_MIN_AIR_ABSORPTION_GAINHF) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_AIR_ABSORPTION_GAINHF))
                    ALEffect->Reverb.AirAbsorptionGainHF = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_REVERB_ROOM_ROLLOFF_FACTOR:
                if(flValue >= float2ALfp(AL_REVERB_MIN_ROOM_ROLLOFF_FACTOR) &&
                   flValue <= float2ALfp(AL_REVERB_MAX_ROOM_ROLLOFF_FACTOR))
                    ALEffect->Reverb.RoomRolloffFactor = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_ECHO)
        {
            switch(param)
            {
            case AL_ECHO_DELAY:
                if(flValue >= float2ALfp(AL_ECHO_MIN_DELAY) && flValue <= float2ALfp(AL_ECHO_MAX_DELAY))
                    ALEffect->Echo.Delay = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_ECHO_LRDELAY:
                if(flValue >= float2ALfp(AL_ECHO_MIN_LRDELAY) && flValue <= float2ALfp(AL_ECHO_MAX_LRDELAY))
                    ALEffect->Echo.LRDelay = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_ECHO_DAMPING:
                if(flValue >= float2ALfp(AL_ECHO_MIN_DAMPING) && flValue <= float2ALfp(AL_ECHO_MAX_DAMPING))
                    ALEffect->Echo.Damping = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_ECHO_FEEDBACK:
                if(flValue >= float2ALfp(AL_ECHO_MIN_FEEDBACK) && flValue <= float2ALfp(AL_ECHO_MAX_FEEDBACK))
                    ALEffect->Echo.Feedback = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_ECHO_SPREAD:
                if(flValue >= float2ALfp(AL_ECHO_MIN_SPREAD) && flValue <= float2ALfp(AL_ECHO_MAX_SPREAD))
                    ALEffect->Echo.Spread = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_RING_MODULATOR)
        {
            switch(param)
            {
            case AL_RING_MODULATOR_FREQUENCY:
                if(flValue >= float2ALfp(AL_RING_MODULATOR_MIN_FREQUENCY) &&
                   flValue <= float2ALfp(AL_RING_MODULATOR_MAX_FREQUENCY))
                    ALEffect->Modulator.Frequency = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
                if(flValue >= float2ALfp(AL_RING_MODULATOR_MIN_HIGHPASS_CUTOFF) &&
                   flValue <= float2ALfp(AL_RING_MODULATOR_MAX_HIGHPASS_CUTOFF))
                    ALEffect->Modulator.HighPassCutoff = flValue;
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else
            alSetError(Context, AL_INVALID_ENUM);
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alEffectfv(ALuint effect, ALenum param, ALfloat *pflValues)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALeffect   *ALEffect;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALEffect=LookupEffect(Device->EffectMap, effect)) != NULL)
    {
        if(ALEffect->type == AL_EFFECT_EAXREVERB)
        {
            switch(param)
            {
            case AL_EAXREVERB_DENSITY:
            case AL_EAXREVERB_DIFFUSION:
            case AL_EAXREVERB_GAIN:
            case AL_EAXREVERB_GAINHF:
            case AL_EAXREVERB_GAINLF:
            case AL_EAXREVERB_DECAY_TIME:
            case AL_EAXREVERB_DECAY_HFRATIO:
            case AL_EAXREVERB_DECAY_LFRATIO:
            case AL_EAXREVERB_REFLECTIONS_GAIN:
            case AL_EAXREVERB_REFLECTIONS_DELAY:
            case AL_EAXREVERB_LATE_REVERB_GAIN:
            case AL_EAXREVERB_LATE_REVERB_DELAY:
            case AL_EAXREVERB_AIR_ABSORPTION_GAINHF:
            case AL_EAXREVERB_ECHO_TIME:
            case AL_EAXREVERB_ECHO_DEPTH:
            case AL_EAXREVERB_MODULATION_TIME:
            case AL_EAXREVERB_MODULATION_DEPTH:
            case AL_EAXREVERB_HFREFERENCE:
            case AL_EAXREVERB_LFREFERENCE:
            case AL_EAXREVERB_ROOM_ROLLOFF_FACTOR:
                alEffectf(effect, param, pflValues[0]);
                break;

            case AL_EAXREVERB_REFLECTIONS_PAN:
                if(!__isnan(pflValues[0]) && !__isnan(pflValues[1]) && !__isnan(pflValues[2]))
                {
                    ALEffect->Reverb.ReflectionsPan[0] = float2ALfp(pflValues[0]);
                    ALEffect->Reverb.ReflectionsPan[1] = float2ALfp(pflValues[1]);
                    ALEffect->Reverb.ReflectionsPan[2] = float2ALfp(pflValues[2]);
                }
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;
            case AL_EAXREVERB_LATE_REVERB_PAN:
                if(!__isnan(pflValues[0]) && !__isnan(pflValues[1]) && !__isnan(pflValues[2]))
                {
                    ALEffect->Reverb.LateReverbPan[0] = float2ALfp(pflValues[0]);
                    ALEffect->Reverb.LateReverbPan[1] = float2ALfp(pflValues[1]);
                    ALEffect->Reverb.LateReverbPan[2] = float2ALfp(pflValues[2]);
                }
                else
                    alSetError(Context, AL_INVALID_VALUE);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_REVERB)
        {
            switch(param)
            {
            case AL_REVERB_DENSITY:
            case AL_REVERB_DIFFUSION:
            case AL_REVERB_GAIN:
            case AL_REVERB_GAINHF:
            case AL_REVERB_DECAY_TIME:
            case AL_REVERB_DECAY_HFRATIO:
            case AL_REVERB_REFLECTIONS_GAIN:
            case AL_REVERB_REFLECTIONS_DELAY:
            case AL_REVERB_LATE_REVERB_GAIN:
            case AL_REVERB_LATE_REVERB_DELAY:
            case AL_REVERB_AIR_ABSORPTION_GAINHF:
            case AL_REVERB_ROOM_ROLLOFF_FACTOR:
                alEffectf(effect, param, pflValues[0]);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_ECHO)
        {
            switch(param)
            {
            case AL_ECHO_DELAY:
            case AL_ECHO_LRDELAY:
            case AL_ECHO_DAMPING:
            case AL_ECHO_FEEDBACK:
            case AL_ECHO_SPREAD:
                alEffectf(effect, param, pflValues[0]);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_RING_MODULATOR)
        {
            switch(param)
            {
            case AL_RING_MODULATOR_FREQUENCY:
            case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
                alEffectf(effect, param, pflValues[0]);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else
            alSetError(Context, AL_INVALID_ENUM);
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alGetEffecti(ALuint effect, ALenum param, ALint *piValue)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALeffect   *ALEffect;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALEffect=LookupEffect(Device->EffectMap, effect)) != NULL)
    {
        if(param == AL_EFFECT_TYPE)
        {
            *piValue = ALEffect->type;
        }
        else if(ALEffect->type == AL_EFFECT_EAXREVERB)
        {
            switch(param)
            {
            case AL_EAXREVERB_DECAY_HFLIMIT:
                *piValue = ALEffect->Reverb.DecayHFLimit;
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_REVERB)
        {
            switch(param)
            {
            case AL_REVERB_DECAY_HFLIMIT:
                *piValue = ALEffect->Reverb.DecayHFLimit;
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_ECHO)
        {
            switch(param)
            {
            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_RING_MODULATOR)
        {
            switch(param)
            {
            case AL_RING_MODULATOR_FREQUENCY:
                *piValue = (ALint)ALfp2float(ALEffect->Modulator.Frequency);
                break;
            case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
                *piValue = (ALint)ALfp2float(ALEffect->Modulator.HighPassCutoff);
                break;
            case AL_RING_MODULATOR_WAVEFORM:
                *piValue = ALEffect->Modulator.Waveform;
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else
            alSetError(Context, AL_INVALID_ENUM);
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alGetEffectiv(ALuint effect, ALenum param, ALint *piValues)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALeffect   *ALEffect;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALEffect=LookupEffect(Device->EffectMap, effect)) != NULL)
    {
        if(param == AL_EFFECT_TYPE)
        {
            alGetEffecti(effect, param, piValues);
        }
        else if(ALEffect->type == AL_EFFECT_EAXREVERB)
        {
            switch(param)
            {
            case AL_EAXREVERB_DECAY_HFLIMIT:
                alGetEffecti(effect, param, piValues);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_REVERB)
        {
            switch(param)
            {
            case AL_REVERB_DECAY_HFLIMIT:
                alGetEffecti(effect, param, piValues);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_ECHO)
        {
            switch(param)
            {
            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_RING_MODULATOR)
        {
            switch(param)
            {
            case AL_RING_MODULATOR_FREQUENCY:
            case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
            case AL_RING_MODULATOR_WAVEFORM:
                alGetEffecti(effect, param, piValues);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else
            alSetError(Context, AL_INVALID_ENUM);
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alGetEffectf(ALuint effect, ALenum param, ALfloat *pflValue)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALeffect   *ALEffect;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALEffect=LookupEffect(Device->EffectMap, effect)) != NULL)
    {
        if(ALEffect->type == AL_EFFECT_EAXREVERB)
        {
            switch(param)
            {
            case AL_EAXREVERB_DENSITY:
                *pflValue = ALfp2float(ALEffect->Reverb.Density);
                break;

            case AL_EAXREVERB_DIFFUSION:
                *pflValue = ALfp2float(ALEffect->Reverb.Diffusion);
                break;

            case AL_EAXREVERB_GAIN:
                *pflValue = ALfp2float(ALEffect->Reverb.Gain);
                break;

            case AL_EAXREVERB_GAINHF:
                *pflValue = ALfp2float(ALEffect->Reverb.GainHF);
                break;

            case AL_EAXREVERB_GAINLF:
                *pflValue = ALfp2float(ALEffect->Reverb.GainLF);
                break;

            case AL_EAXREVERB_DECAY_TIME:
                *pflValue = ALfp2float(ALEffect->Reverb.DecayTime);
                break;

            case AL_EAXREVERB_DECAY_HFRATIO:
                *pflValue = ALfp2float(ALEffect->Reverb.DecayHFRatio);
                break;

            case AL_EAXREVERB_DECAY_LFRATIO:
                *pflValue = ALfp2float(ALEffect->Reverb.DecayLFRatio);
                break;

            case AL_EAXREVERB_REFLECTIONS_GAIN:
                *pflValue = ALfp2float(ALEffect->Reverb.ReflectionsGain);
                break;

            case AL_EAXREVERB_REFLECTIONS_DELAY:
                *pflValue = ALfp2float(ALEffect->Reverb.ReflectionsDelay);
                break;

            case AL_EAXREVERB_LATE_REVERB_GAIN:
                *pflValue = ALfp2float(ALEffect->Reverb.LateReverbGain);
                break;

            case AL_EAXREVERB_LATE_REVERB_DELAY:
                *pflValue = ALfp2float(ALEffect->Reverb.LateReverbDelay);
                break;

            case AL_EAXREVERB_AIR_ABSORPTION_GAINHF:
                *pflValue = ALfp2float(ALEffect->Reverb.AirAbsorptionGainHF);
                break;

            case AL_EAXREVERB_ECHO_TIME:
                *pflValue = ALfp2float(ALEffect->Reverb.EchoTime);
                break;

            case AL_EAXREVERB_ECHO_DEPTH:
                *pflValue = ALfp2float(ALEffect->Reverb.EchoDepth);
                break;

            case AL_EAXREVERB_MODULATION_TIME:
                *pflValue = ALfp2float(ALEffect->Reverb.ModulationTime);
                break;

            case AL_EAXREVERB_MODULATION_DEPTH:
                *pflValue = ALfp2float(ALEffect->Reverb.ModulationDepth);
                break;

            case AL_EAXREVERB_HFREFERENCE:
                *pflValue = ALfp2float(ALEffect->Reverb.HFReference);
                break;

            case AL_EAXREVERB_LFREFERENCE:
                *pflValue = ALfp2float(ALEffect->Reverb.LFReference);
                break;

            case AL_EAXREVERB_ROOM_ROLLOFF_FACTOR:
                *pflValue = ALfp2float(ALEffect->Reverb.RoomRolloffFactor);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_REVERB)
        {
            switch(param)
            {
            case AL_REVERB_DENSITY:
                *pflValue = ALfp2float(ALEffect->Reverb.Density);
                break;

            case AL_REVERB_DIFFUSION:
                *pflValue = ALfp2float(ALEffect->Reverb.Diffusion);
                break;

            case AL_REVERB_GAIN:
                *pflValue = ALfp2float(ALEffect->Reverb.Gain);
                break;

            case AL_REVERB_GAINHF:
                *pflValue = ALfp2float(ALEffect->Reverb.GainHF);
                break;

            case AL_REVERB_DECAY_TIME:
                *pflValue = ALfp2float(ALEffect->Reverb.DecayTime);
                break;

            case AL_REVERB_DECAY_HFRATIO:
                *pflValue = ALfp2float(ALEffect->Reverb.DecayHFRatio);
                break;

            case AL_REVERB_REFLECTIONS_GAIN:
                *pflValue = ALfp2float(ALEffect->Reverb.ReflectionsGain);
                break;

            case AL_REVERB_REFLECTIONS_DELAY:
                *pflValue = ALfp2float(ALEffect->Reverb.ReflectionsDelay);
                break;

            case AL_REVERB_LATE_REVERB_GAIN:
                *pflValue = ALfp2float(ALEffect->Reverb.LateReverbGain);
                break;

            case AL_REVERB_LATE_REVERB_DELAY:
                *pflValue = ALfp2float(ALEffect->Reverb.LateReverbDelay);
                break;

            case AL_REVERB_AIR_ABSORPTION_GAINHF:
                *pflValue = ALfp2float(ALEffect->Reverb.AirAbsorptionGainHF);
                break;

            case AL_REVERB_ROOM_ROLLOFF_FACTOR:
                *pflValue = ALfp2float(ALEffect->Reverb.RoomRolloffFactor);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_ECHO)
        {
            switch(param)
            {
            case AL_ECHO_DELAY:
                *pflValue = ALfp2float(ALEffect->Echo.Delay);
                break;

            case AL_ECHO_LRDELAY:
                *pflValue = ALfp2float(ALEffect->Echo.LRDelay);
                break;

            case AL_ECHO_DAMPING:
                *pflValue = ALfp2float(ALEffect->Echo.Damping);
                break;

            case AL_ECHO_FEEDBACK:
                *pflValue = ALfp2float(ALEffect->Echo.Feedback);
                break;

            case AL_ECHO_SPREAD:
                *pflValue = ALfp2float(ALEffect->Echo.Spread);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_RING_MODULATOR)
        {
            switch(param)
            {
            case AL_RING_MODULATOR_FREQUENCY:
                *pflValue = ALfp2float(ALEffect->Modulator.Frequency);
                break;
            case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
                *pflValue = ALfp2float(ALEffect->Modulator.HighPassCutoff);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else
            alSetError(Context, AL_INVALID_ENUM);
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}

AL_API ALvoid AL_APIENTRY alGetEffectfv(ALuint effect, ALenum param, ALfloat *pflValues)
{
    ALCcontext *Context;
    ALCdevice  *Device;
    ALeffect   *ALEffect;

    Context = GetContextSuspended();
    if(!Context) return;

    Device = Context->Device;
    if((ALEffect=LookupEffect(Device->EffectMap, effect)) != NULL)
    {
        if(ALEffect->type == AL_EFFECT_EAXREVERB)
        {
            switch(param)
            {
            case AL_EAXREVERB_DENSITY:
            case AL_EAXREVERB_DIFFUSION:
            case AL_EAXREVERB_GAIN:
            case AL_EAXREVERB_GAINHF:
            case AL_EAXREVERB_GAINLF:
            case AL_EAXREVERB_DECAY_TIME:
            case AL_EAXREVERB_DECAY_HFRATIO:
            case AL_EAXREVERB_DECAY_LFRATIO:
            case AL_EAXREVERB_REFLECTIONS_GAIN:
            case AL_EAXREVERB_REFLECTIONS_DELAY:
            case AL_EAXREVERB_LATE_REVERB_GAIN:
            case AL_EAXREVERB_LATE_REVERB_DELAY:
            case AL_EAXREVERB_AIR_ABSORPTION_GAINHF:
            case AL_EAXREVERB_ECHO_TIME:
            case AL_EAXREVERB_ECHO_DEPTH:
            case AL_EAXREVERB_MODULATION_TIME:
            case AL_EAXREVERB_MODULATION_DEPTH:
            case AL_EAXREVERB_HFREFERENCE:
            case AL_EAXREVERB_LFREFERENCE:
            case AL_EAXREVERB_ROOM_ROLLOFF_FACTOR:
                alGetEffectf(effect, param, pflValues);
                break;

            case AL_EAXREVERB_REFLECTIONS_PAN:
                pflValues[0] = ALfp2float(ALEffect->Reverb.ReflectionsPan[0]);
                pflValues[1] = ALfp2float(ALEffect->Reverb.ReflectionsPan[1]);
                pflValues[2] = ALfp2float(ALEffect->Reverb.ReflectionsPan[2]);
                break;
            case AL_EAXREVERB_LATE_REVERB_PAN:
                pflValues[0] = ALfp2float(ALEffect->Reverb.LateReverbPan[0]);
                pflValues[1] = ALfp2float(ALEffect->Reverb.LateReverbPan[1]);
                pflValues[2] = ALfp2float(ALEffect->Reverb.LateReverbPan[2]);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_REVERB)
        {
            switch(param)
            {
            case AL_REVERB_DENSITY:
            case AL_REVERB_DIFFUSION:
            case AL_REVERB_GAIN:
            case AL_REVERB_GAINHF:
            case AL_REVERB_DECAY_TIME:
            case AL_REVERB_DECAY_HFRATIO:
            case AL_REVERB_REFLECTIONS_GAIN:
            case AL_REVERB_REFLECTIONS_DELAY:
            case AL_REVERB_LATE_REVERB_GAIN:
            case AL_REVERB_LATE_REVERB_DELAY:
            case AL_REVERB_AIR_ABSORPTION_GAINHF:
            case AL_REVERB_ROOM_ROLLOFF_FACTOR:
                alGetEffectf(effect, param, pflValues);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_ECHO)
        {
            switch(param)
            {
            case AL_ECHO_DELAY:
            case AL_ECHO_LRDELAY:
            case AL_ECHO_DAMPING:
            case AL_ECHO_FEEDBACK:
            case AL_ECHO_SPREAD:
                alGetEffectf(effect, param, pflValues);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else if(ALEffect->type == AL_EFFECT_RING_MODULATOR)
        {
            switch(param)
            {
            case AL_RING_MODULATOR_FREQUENCY:
            case AL_RING_MODULATOR_HIGHPASS_CUTOFF:
                alGetEffectf(effect, param, pflValues);
                break;

            default:
                alSetError(Context, AL_INVALID_ENUM);
                break;
            }
        }
        else
            alSetError(Context, AL_INVALID_ENUM);
    }
    else
        alSetError(Context, AL_INVALID_NAME);

    ProcessContext(Context);
}


ALvoid ReleaseALEffects(ALCdevice *device)
{
    ALsizei i;
    for(i = 0;i < device->EffectMap.size;i++)
    {
        ALeffect *temp = device->EffectMap.array[i].value;
        device->EffectMap.array[i].value = NULL;

        // Release effect structure
        ALTHUNK_REMOVEENTRY(temp->effect);
        memset(temp, 0, sizeof(ALeffect));
        free(temp);
    }
}


static void InitEffectParams(ALeffect *effect, ALenum type)
{
    effect->type = type;
    switch(type)
    {
    /* NOTE: Standard reverb and EAX reverb use the same defaults for the
     *       shared parameters, and EAX's additional parameters default to
     *       values assumed by standard reverb.
     */
    case AL_EFFECT_EAXREVERB:
    case AL_EFFECT_REVERB:
        effect->Reverb.Density = float2ALfp(AL_EAXREVERB_DEFAULT_DENSITY);
        effect->Reverb.Diffusion = float2ALfp(AL_EAXREVERB_DEFAULT_DIFFUSION);
        effect->Reverb.Gain = float2ALfp(AL_EAXREVERB_DEFAULT_GAIN);
        effect->Reverb.GainHF = float2ALfp(AL_EAXREVERB_DEFAULT_GAINHF);
        effect->Reverb.GainLF = float2ALfp(AL_EAXREVERB_DEFAULT_GAINLF);
        effect->Reverb.DecayTime = float2ALfp(AL_EAXREVERB_DEFAULT_DECAY_TIME);
        effect->Reverb.DecayHFRatio = float2ALfp(AL_EAXREVERB_DEFAULT_DECAY_HFRATIO);
        effect->Reverb.DecayLFRatio = float2ALfp(AL_EAXREVERB_DEFAULT_DECAY_LFRATIO);
        effect->Reverb.ReflectionsGain = float2ALfp(AL_EAXREVERB_DEFAULT_REFLECTIONS_GAIN);
        effect->Reverb.ReflectionsDelay = float2ALfp(AL_EAXREVERB_DEFAULT_REFLECTIONS_DELAY);
        effect->Reverb.ReflectionsPan[0] = float2ALfp(AL_EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ);
        effect->Reverb.ReflectionsPan[1] = float2ALfp(AL_EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ);
        effect->Reverb.ReflectionsPan[2] = float2ALfp(AL_EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ);
        effect->Reverb.LateReverbGain = float2ALfp(AL_EAXREVERB_DEFAULT_LATE_REVERB_GAIN);
        effect->Reverb.LateReverbDelay = float2ALfp(AL_EAXREVERB_DEFAULT_LATE_REVERB_DELAY);
        effect->Reverb.LateReverbPan[0] = float2ALfp(AL_EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ);
        effect->Reverb.LateReverbPan[1] = float2ALfp(AL_EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ);
        effect->Reverb.LateReverbPan[2] = float2ALfp(AL_EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ);
        effect->Reverb.EchoTime = float2ALfp(AL_EAXREVERB_DEFAULT_ECHO_TIME);
        effect->Reverb.EchoDepth = float2ALfp(AL_EAXREVERB_DEFAULT_ECHO_DEPTH);
        effect->Reverb.ModulationTime = float2ALfp(AL_EAXREVERB_DEFAULT_MODULATION_TIME);
        effect->Reverb.ModulationDepth = float2ALfp(AL_EAXREVERB_DEFAULT_MODULATION_DEPTH);
        effect->Reverb.AirAbsorptionGainHF = float2ALfp(AL_EAXREVERB_DEFAULT_AIR_ABSORPTION_GAINHF);
        effect->Reverb.HFReference = float2ALfp(AL_EAXREVERB_DEFAULT_HFREFERENCE);
        effect->Reverb.LFReference = float2ALfp(AL_EAXREVERB_DEFAULT_LFREFERENCE);
        effect->Reverb.RoomRolloffFactor = float2ALfp(AL_EAXREVERB_DEFAULT_ROOM_ROLLOFF_FACTOR);
        effect->Reverb.DecayHFLimit = AL_EAXREVERB_DEFAULT_DECAY_HFLIMIT;
        break;
    case AL_EFFECT_ECHO:
        effect->Echo.Delay = float2ALfp(AL_ECHO_DEFAULT_DELAY);
        effect->Echo.LRDelay = float2ALfp(AL_ECHO_DEFAULT_LRDELAY);
        effect->Echo.Damping = float2ALfp(AL_ECHO_DEFAULT_DAMPING);
        effect->Echo.Feedback = float2ALfp(AL_ECHO_DEFAULT_FEEDBACK);
        effect->Echo.Spread = float2ALfp(AL_ECHO_DEFAULT_SPREAD);
        break;
    case AL_EFFECT_RING_MODULATOR:
        effect->Modulator.Frequency = float2ALfp(AL_RING_MODULATOR_DEFAULT_FREQUENCY);
        effect->Modulator.HighPassCutoff = float2ALfp(AL_RING_MODULATOR_DEFAULT_HIGHPASS_CUTOFF);
        effect->Modulator.Waveform = AL_RING_MODULATOR_DEFAULT_WAVEFORM;
        break;
    }
}
