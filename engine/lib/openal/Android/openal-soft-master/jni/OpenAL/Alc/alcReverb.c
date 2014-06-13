/**
 * Reverb for the OpenAL cross platform audio library
 * Copyright (C) 2008-2009 by Christopher Fitzgerald.
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "AL/al.h"
#include "AL/alc.h"
#include "alMain.h"
#include "alAuxEffectSlot.h"
#include "alEffect.h"
#include "alError.h"
#include "alu.h"

typedef struct DelayLine
{
    // The delay lines use sample lengths that are powers of 2 to allow the
    // use of bit-masking instead of a modulus for wrapping.
    ALuint   Mask;
    ALfp *Line;
} DelayLine;

typedef struct ALverbState {
    // Must be first in all effects!
    ALeffectState state;

    // All delay lines are allocated as a single buffer to reduce memory
    // fragmentation and management code.
    ALfp  *SampleBuffer;
    ALuint    TotalSamples;
    // Master effect low-pass filter (2 chained 1-pole filters).
    FILTER    LpFilter;
    ALfp   LpHistory[2];
    struct {
        // Modulator delay line.
        DelayLine Delay;
        // The vibrato time is tracked with an index over a modulus-wrapped
        // range (in samples).
        ALuint    Index;
        ALuint    Range;
        // The depth of frequency change (also in samples) and its filter.
        ALfp   Depth;
        ALfp   Coeff;
        ALfp   Filter;
    } Mod;
    // Initial effect delay.
    DelayLine Delay;
    // The tap points for the initial delay.  First tap goes to early
    // reflections, the last to late reverb.
    ALuint    DelayTap[2];
    struct {
        // Output gain for early reflections.
        ALfp   Gain;
        // Early reflections are done with 4 delay lines.
        ALfp   Coeff[4];
        DelayLine Delay[4];
        ALuint    Offset[4];
        // The gain for each output channel based on 3D panning (only for the
        // EAX path).
        ALfp   PanGain[MAXCHANNELS];
    } Early;
    // Decorrelator delay line.
    DelayLine Decorrelator;
    // There are actually 4 decorrelator taps, but the first occurs at the
    // initial sample.
    ALuint    DecoTap[3];
    struct {
        // Output gain for late reverb.
        ALfp   Gain;
        // Attenuation to compensate for the modal density and decay rate of
        // the late lines.
        ALfp   DensityGain;
        // The feed-back and feed-forward all-pass coefficient.
        ALfp   ApFeedCoeff;
        // Mixing matrix coefficient.
        ALfp   MixCoeff;
        // Late reverb has 4 parallel all-pass filters.
        ALfp   ApCoeff[4];
        DelayLine ApDelay[4];
        ALuint    ApOffset[4];
        // In addition to 4 cyclical delay lines.
        ALfp   Coeff[4];
        DelayLine Delay[4];
        ALuint    Offset[4];
        // The cyclical delay lines are 1-pole low-pass filtered.
        ALfp   LpCoeff[4];
        ALfp   LpSample[4];
        // The gain for each output channel based on 3D panning (only for the
        // EAX path).
        ALfp   PanGain[MAXCHANNELS];
    } Late;
    struct {
        // Attenuation to compensate for the modal density and decay rate of
        // the echo line.
        ALfp   DensityGain;
        // Echo delay and all-pass lines.
        DelayLine Delay;
        DelayLine ApDelay;
        ALfp   Coeff;
        ALfp   ApFeedCoeff;
        ALfp   ApCoeff;
        ALuint    Offset;
        ALuint    ApOffset;
        // The echo line is 1-pole low-pass filtered.
        ALfp   LpCoeff;
        ALfp   LpSample;
        // Echo mixing coefficients.
        ALfp   MixCoeff[2];
    } Echo;
    // The current read offset for all delay lines.
    ALuint Offset;

    // The gain for each output channel (non-EAX path only; aliased from
    // Late.PanGain)
    ALfp *Gain;
} ALverbState;

/* This coefficient is used to define the maximum frequency range controlled
 * by the modulation depth.  The current value of 0.1 will allow it to swing
 * from 0.9x to 1.1x.  This value must be below 1.  At 1 it will cause the
 * sampler to stall on the downswing, and above 1 it will cause it to sample
 * backwards.
 */
static const ALfp MODULATION_DEPTH_COEFF = toALfpConst(0.1f);

/* A filter is used to avoid the terrible distortion caused by changing
 * modulation time and/or depth.  To be consistent across different sample
 * rates, the coefficient must be raised to a constant divided by the sample
 * rate:  coeff^(constant / rate).
 */
static const ALfp MODULATION_FILTER_COEFF = toALfpConst(0.048f);
static const ALfp MODULATION_FILTER_CONST = toALfpConst(100000.0f);

// When diffusion is above 0, an all-pass filter is used to take the edge off
// the echo effect.  It uses the following line length (in seconds).
static const ALfp ECHO_ALLPASS_LENGTH = toALfpConst(0.0133f);

// Input into the late reverb is decorrelated between four channels.  Their
// timings are dependent on a fraction and multiplier.  See the
// UpdateDecorrelator() routine for the calculations involved.
static const ALfp DECO_FRACTION = toALfpConst(0.15f);
static const ALfp DECO_MULTIPLIER = toALfpConst(2.0f);

// All delay line lengths are specified in seconds.

// The lengths of the early delay lines.
static const ALfp EARLY_LINE_LENGTH[4] =
{
    toALfpConst(0.0015f), toALfpConst(0.0045f), toALfpConst(0.0135f), toALfpConst(0.0405f)
};

// The lengths of the late all-pass delay lines.
static const ALfp ALLPASS_LINE_LENGTH[4] =
{
    toALfpConst(0.0151f), toALfpConst(0.0167f), toALfpConst(0.0183f), toALfpConst(0.0200f),
};

// The lengths of the late cyclical delay lines.
static const ALfp LATE_LINE_LENGTH[4] =
{
    toALfpConst(0.0211f), toALfpConst(0.0311f), toALfpConst(0.0461f), toALfpConst(0.0680f)
};

// The late cyclical delay lines have a variable length dependent on the
// effect's density parameter (inverted for some reason) and this multiplier.
static const ALfp LATE_LINE_MULTIPLIER = toALfpConst(4.0f);

// Calculate the length of a delay line and store its mask and offset.
static ALuint CalcLineLength(ALfp length, ALintptrEXT offset, ALuint frequency, DelayLine *Delay)
{
    ALuint samples;

    // All line lengths are powers of 2, calculated from their lengths, with
    // an additional sample in case of rounding errors.
    samples = NextPowerOf2((ALuint)(ALfp2int(ALfpMult(length, int2ALfp(frequency)))) + 1);
    // All lines share a single sample buffer.
    Delay->Mask = samples - 1;
    Delay->Line = (ALfp*)offset;
    // Return the sample count for accumulation.
    return samples;
}

// Given the allocated sample buffer, this function updates each delay line
// offset.
static __inline ALvoid RealizeLineOffset(ALfp * sampleBuffer, DelayLine *Delay)
{
    Delay->Line = &sampleBuffer[(ALintptrEXT)Delay->Line];
}

/* Calculates the delay line metrics and allocates the shared sample buffer
 * for all lines given a flag indicating whether or not to allocate the EAX-
 * related delays (eaxFlag) and the sample rate (frequency).  If an
 * allocation failure occurs, it returns AL_FALSE.
 */
static ALboolean AllocLines(ALboolean eaxFlag, ALuint frequency, ALverbState *State)
{
    ALuint totalSamples, index;
    ALfp length;
    ALfp *newBuffer = NULL;

    // All delay line lengths are calculated to accomodate the full range of
    // lengths given their respective paramters.
    totalSamples = 0;
    if(eaxFlag)
    {
        /* The modulator's line length is calculated from the maximum
         * modulation time and depth coefficient, and halfed for the low-to-
         * high frequency swing.  An additional sample is added to keep it
         * stable when there is no modulation.
         */
        length = ((ALfpDiv(ALfpMult(float2ALfp(AL_EAXREVERB_MAX_MODULATION_TIME), MODULATION_DEPTH_COEFF),
                           int2ALfp(2))) + 
                  ALfpDiv(int2ALfp(1), int2ALfp(frequency)));
        totalSamples += CalcLineLength(length, totalSamples, frequency,
                                       &State->Mod.Delay);
    }

    // The initial delay is the sum of the reflections and late reverb
    // delays.
    if(eaxFlag)
        length = float2ALfp(AL_EAXREVERB_MAX_REFLECTIONS_DELAY +
                            AL_EAXREVERB_MAX_LATE_REVERB_DELAY);
    else
        length = float2ALfp(AL_REVERB_MAX_REFLECTIONS_DELAY +
                            AL_REVERB_MAX_LATE_REVERB_DELAY);
    totalSamples += CalcLineLength(length, totalSamples, frequency,
                                   &State->Delay);

    // The early reflection lines.
    for(index = 0;index < 4;index++)
        totalSamples += CalcLineLength(EARLY_LINE_LENGTH[index], totalSamples,
                                       frequency, &State->Early.Delay[index]);

    // The decorrelator line is calculated from the lowest reverb density (a
    // parameter value of 1).
    length = ALfpMult(ALfpMult(ALfpMult(ALfpMult(DECO_FRACTION,
                                                 DECO_MULTIPLIER),
                                        DECO_MULTIPLIER),
                               LATE_LINE_LENGTH[0]),
                      (int2ALfp(1) + LATE_LINE_MULTIPLIER));
    totalSamples += CalcLineLength(length, totalSamples, frequency,
                                   &State->Decorrelator);

    // The late all-pass lines.
    for(index = 0;index < 4;index++)
        totalSamples += CalcLineLength(ALLPASS_LINE_LENGTH[index], totalSamples,
                                       frequency, &State->Late.ApDelay[index]);

    // The late delay lines are calculated from the lowest reverb density.
    for(index = 0;index < 4;index++)
    {
        length = ALfpMult(LATE_LINE_LENGTH[index], (int2ALfp(1) + LATE_LINE_MULTIPLIER));
        totalSamples += CalcLineLength(length, totalSamples, frequency,
                                       &State->Late.Delay[index]);
    }

    if(eaxFlag)
    {
        // The echo all-pass and delay lines.
        totalSamples += CalcLineLength(ECHO_ALLPASS_LENGTH, totalSamples,
                                       frequency, &State->Echo.ApDelay);
        totalSamples += CalcLineLength(float2ALfp(AL_EAXREVERB_MAX_ECHO_TIME), totalSamples,
                                       frequency, &State->Echo.Delay);
    }

    if(totalSamples != State->TotalSamples)
    {
        newBuffer = realloc(State->SampleBuffer, sizeof(ALfp) * totalSamples);
        if(newBuffer == NULL)
            return AL_FALSE;
        State->SampleBuffer = newBuffer;
        State->TotalSamples = totalSamples;
    }

    // Update all delays to reflect the new sample buffer.
    RealizeLineOffset(State->SampleBuffer, &State->Delay);
    RealizeLineOffset(State->SampleBuffer, &State->Decorrelator);
    for(index = 0;index < 4;index++)
    {
        RealizeLineOffset(State->SampleBuffer, &State->Early.Delay[index]);
        RealizeLineOffset(State->SampleBuffer, &State->Late.ApDelay[index]);
        RealizeLineOffset(State->SampleBuffer, &State->Late.Delay[index]);
    }
    if(eaxFlag)
    {
        RealizeLineOffset(State->SampleBuffer, &State->Mod.Delay);
        RealizeLineOffset(State->SampleBuffer, &State->Echo.ApDelay);
        RealizeLineOffset(State->SampleBuffer, &State->Echo.Delay);
    }

    // Clear the sample buffer.
    for(index = 0;index < State->TotalSamples;index++)
        State->SampleBuffer[index] = int2ALfp(0);

    return AL_TRUE;
}

// Calculate a decay coefficient given the length of each cycle and the time
// until the decay reaches -60 dB.
static __inline ALfp CalcDecayCoeff(ALfp length, ALfp decayTime)
{
    return aluPow(int2ALfp(10), ALfpDiv(length,
                                        ALfpDiv(ALfpMult(decayTime,
                                                         int2ALfp(-60)),
                                                int2ALfp(20))));
}

// Calculate a decay length from a coefficient and the time until the decay
// reaches -60 dB.
static __inline ALfp CalcDecayLength(ALfp coeff, ALfp decayTime)
{
    return ALfpMult(ALfpMult(ALfpDiv(__log10(coeff), int2ALfp(-60)), int2ALfp(20)), decayTime);
}

// Calculate the high frequency parameter for the I3DL2 coefficient
// calculation.
static __inline ALfp CalcI3DL2HFreq(ALfp hfRef, ALuint frequency)
{
    return __cos(ALfpDiv(ALfpMult(float2ALfp(2.0f * M_PI), hfRef), int2ALfp(frequency)));
}

// Calculate an attenuation to be applied to the input of any echo models to
// compensate for modal density and decay time.
static __inline ALfp CalcDensityGain(ALfp a)
{
    /* The energy of a signal can be obtained by finding the area under the
     * squared signal.  This takes the form of Sum(x_n^2), where x is the
     * amplitude for the sample n.
     *
     * Decaying feedback matches exponential decay of the form Sum(a^n),
     * where a is the attenuation coefficient, and n is the sample.  The area
     * under this decay curve can be calculated as:  1 / (1 - a).
     *
     * Modifying the above equation to find the squared area under the curve
     * (for energy) yields:  1 / (1 - a^2).  Input attenuation can then be
     * calculated by inverting the square root of this approximation,
     * yielding:  1 / sqrt(1 / (1 - a^2)), simplified to: sqrt(1 - a^2).
     */
    return aluSqrt((int2ALfp(1) - ALfpMult(a, a)));
}

// Calculate the mixing matrix coefficients given a diffusion factor.
static __inline ALvoid CalcMatrixCoeffs(ALfp diffusion, ALfp *x, ALfp *y)
{
    ALfp n, t;

    // The matrix is of order 4, so n is sqrt (4 - 1).
    n = aluSqrt(int2ALfp(3));
    t = ALfpMult(diffusion, __atan(n));

    // Calculate the first mixing matrix coefficient.
    *x = __cos(t);
    // Calculate the second mixing matrix coefficient.
    *y = ALfpDiv(__sin(t), n);
}

// Calculate the limited HF ratio for use with the late reverb low-pass
// filters.
static ALfp CalcLimitedHfRatio(ALfp hfRatio, ALfp airAbsorptionGainHF, ALfp decayTime)
{
    ALfp limitRatio;

    /* Find the attenuation due to air absorption in dB (converting delay
     * time to meters using the speed of sound).  Then reversing the decay
     * equation, solve for HF ratio.  The delay length is cancelled out of
     * the equation, so it can be calculated once for all lines.
     */
    limitRatio = ALfpDiv(int2ALfp(1),
                         ALfpMult(CalcDecayLength(airAbsorptionGainHF, decayTime),
                                  float2ALfp(SPEEDOFSOUNDMETRESPERSEC)));
    // Need to limit the result to a minimum of 0.1, just like the HF ratio
    // parameter.
    limitRatio = __max(limitRatio, float2ALfp(0.1f));

    // Using the limit calculated above, apply the upper bound to the HF
    // ratio.
    return __min(hfRatio, limitRatio);
}

// Calculate the coefficient for a HF (and eventually LF) decay damping
// filter.
static __inline ALfp CalcDampingCoeff(ALfp hfRatio, ALfp length, ALfp decayTime, ALfp decayCoeff, ALfp cw)
{
    ALfp coeff, g;

    // Eventually this should boost the high frequencies when the ratio
    // exceeds 1.
    coeff = int2ALfp(0);
    if (hfRatio < int2ALfp(1))
    {
        // Calculate the low-pass coefficient by dividing the HF decay
        // coefficient by the full decay coefficient.
        g = ALfpDiv(CalcDecayCoeff(length, ALfpMult(decayTime, hfRatio)), decayCoeff);

        // Damping is done with a 1-pole filter, so g needs to be squared.
        g = ALfpMult(g, g);
        coeff = lpCoeffCalc(g, cw);

        // Very low decay times will produce minimal output, so apply an
        // upper bound to the coefficient.
        coeff = __min(coeff, float2ALfp(0.98f));
    }
    return coeff;
}

// Update the EAX modulation index, range, and depth.  Keep in mind that this
// kind of vibrato is additive and not multiplicative as one may expect.  The
// downswing will sound stronger than the upswing.
static ALvoid UpdateModulator(ALfp modTime, ALfp modDepth, ALuint frequency, ALverbState *State)
{
    ALfp length;

    /* Modulation is calculated in two parts.
     *
     * The modulation time effects the sinus applied to the change in
     * frequency.  An index out of the current time range (both in samples)
     * is incremented each sample.  The range is bound to a reasonable
     * minimum (1 sample) and when the timing changes, the index is rescaled
     * to the new range (to keep the sinus consistent).
     */
    length = ALfpMult(modTime, int2ALfp(frequency));
    if (length >= int2ALfp(1)) {
       State->Mod.Index = (ALuint)(ALfp2int(ALfpDiv(ALfpMult(int2ALfp(State->Mod.Index),
                                                             length),
                                                    int2ALfp(State->Mod.Range))));
       State->Mod.Range = (ALuint)ALfp2int(length);
    } else {
       State->Mod.Index = 0;
       State->Mod.Range = 1;
    }

    /* The modulation depth effects the amount of frequency change over the
     * range of the sinus.  It needs to be scaled by the modulation time so
     * that a given depth produces a consistent change in frequency over all
     * ranges of time.  Since the depth is applied to a sinus value, it needs
     * to be halfed once for the sinus range and again for the sinus swing
     * in time (half of it is spent decreasing the frequency, half is spent
     * increasing it).
     */
    State->Mod.Depth = ALfpMult(ALfpDiv(ALfpDiv(ALfpMult(ALfpMult(modDepth,
                                                                  MODULATION_DEPTH_COEFF),
                                                         modTime),
                                                int2ALfp(2)),
                                        int2ALfp(2)),
                                int2ALfp(frequency));
}

// Update the offsets for the initial effect delay line.
static ALvoid UpdateDelayLine(ALfp earlyDelay, ALfp lateDelay, ALuint frequency, ALverbState *State)
{
    // Calculate the initial delay taps.
    State->DelayTap[0] = (ALuint)(ALfp2int(ALfpMult(earlyDelay, int2ALfp(frequency))));
    State->DelayTap[1] = (ALuint)(ALfp2int(ALfpMult((earlyDelay + lateDelay), int2ALfp(frequency))));
}

// Update the early reflections gain and line coefficients.
static ALvoid UpdateEarlyLines(ALfp reverbGain, ALfp earlyGain, ALfp lateDelay, ALverbState *State)
{
    ALuint index;

    // Calculate the early reflections gain (from the master effect gain, and
    // reflections gain parameters) with a constant attenuation of 0.5.
    State->Early.Gain = ALfpMult(ALfpMult(float2ALfp(0.5f), reverbGain), earlyGain);

    // Calculate the gain (coefficient) for each early delay line using the
    // late delay time.  This expands the early reflections to the start of
    // the late reverb.
    for(index = 0;index < 4;index++)
        State->Early.Coeff[index] = CalcDecayCoeff(EARLY_LINE_LENGTH[index],
                                                   lateDelay);
}

// Update the offsets for the decorrelator line.
static ALvoid UpdateDecorrelator(ALfp density, ALuint frequency, ALverbState *State)
{
    ALuint index;
    ALfp length;

    /* The late reverb inputs are decorrelated to smooth the reverb tail and
     * reduce harsh echos.  The first tap occurs immediately, while the
     * remaining taps are delayed by multiples of a fraction of the smallest
     * cyclical delay time.
     *
     * offset[index] = (FRACTION (MULTIPLIER^index)) smallest_delay
     */
    for(index = 0;index < 3;index++)
    {
        length = ALfpMult(ALfpMult(ALfpMult(DECO_FRACTION, 
                                            aluPow(DECO_MULTIPLIER, int2ALfp(index))),
                                   LATE_LINE_LENGTH[0]),
                          (int2ALfp(1) + ALfpMult(density, LATE_LINE_MULTIPLIER)));
        State->DecoTap[index] = (ALuint)ALfp2int(ALfpMult(length, int2ALfp(frequency)));
    }
}

// Update the late reverb gains, line lengths, and line coefficients.
static ALvoid UpdateLateLines(ALfp reverbGain, ALfp lateGain, ALfp xMix, ALfp density, ALfp decayTime, ALfp diffusion, ALfp hfRatio, ALfp cw, ALuint frequency, ALverbState *State)
{
    ALfp length;
    ALuint index;

    /* Calculate the late reverb gain (from the master effect gain, and late
     * reverb gain parameters).  Since the output is tapped prior to the
     * application of the next delay line coefficients, this gain needs to be
     * attenuated by the 'x' mixing matrix coefficient as well.
     */
    State->Late.Gain = ALfpMult(ALfpMult(reverbGain, lateGain), xMix);

    /* To compensate for changes in modal density and decay time of the late
     * reverb signal, the input is attenuated based on the maximal energy of
     * the outgoing signal.  This approximation is used to keep the apparent
     * energy of the signal equal for all ranges of density and decay time.
     *
     * The average length of the cyclcical delay lines is used to calculate
     * the attenuation coefficient.
     */
    length = ALfpDiv((LATE_LINE_LENGTH[0] + LATE_LINE_LENGTH[1] +
                      LATE_LINE_LENGTH[2] + LATE_LINE_LENGTH[3]),
                     int2ALfp(4));
    length = ALfpMult(length, (int2ALfp(1) + ALfpMult(density, LATE_LINE_MULTIPLIER)));
    State->Late.DensityGain = CalcDensityGain(CalcDecayCoeff(length,
                                                             decayTime));

    // Calculate the all-pass feed-back and feed-forward coefficient.
    State->Late.ApFeedCoeff = ALfpMult(float2ALfp(0.5f), aluPow(diffusion, int2ALfp(2)));

    for(index = 0;index < 4;index++)
    {
        // Calculate the gain (coefficient) for each all-pass line.
        State->Late.ApCoeff[index] = CalcDecayCoeff(ALLPASS_LINE_LENGTH[index],
                                                    decayTime);

        // Calculate the length (in seconds) of each cyclical delay line.
        length = ALfpMult(LATE_LINE_LENGTH[index],
                          (int2ALfp(1) + ALfpMult(density, LATE_LINE_MULTIPLIER)));

        // Calculate the delay offset for each cyclical delay line.
        State->Late.Offset[index] = (ALuint)(ALfp2int(ALfpMult(length, int2ALfp(frequency))));

        // Calculate the gain (coefficient) for each cyclical line.
        State->Late.Coeff[index] = CalcDecayCoeff(length, decayTime);

        // Calculate the damping coefficient for each low-pass filter.
        State->Late.LpCoeff[index] =
            CalcDampingCoeff(hfRatio, length, decayTime,
                             State->Late.Coeff[index], cw);

        // Attenuate the cyclical line coefficients by the mixing coefficient
        // (x).
        State->Late.Coeff[index] = ALfpMult(State->Late.Coeff[index], xMix);
    }
}

// Update the echo gain, line offset, line coefficients, and mixing
// coefficients.
static ALvoid UpdateEchoLine(ALfp reverbGain, ALfp lateGain, ALfp echoTime, ALfp decayTime, ALfp diffusion, ALfp echoDepth, ALfp hfRatio, ALfp cw, ALuint frequency, ALverbState *State)
{
    // Update the offset and coefficient for the echo delay line.
    State->Echo.Offset = (ALuint)(ALfp2int(ALfpMult(echoTime, int2ALfp(frequency))));

    // Calculate the decay coefficient for the echo line.
    State->Echo.Coeff = CalcDecayCoeff(echoTime, decayTime);

    // Calculate the energy-based attenuation coefficient for the echo delay
    // line.
    State->Echo.DensityGain = CalcDensityGain(State->Echo.Coeff);

    // Calculate the echo all-pass feed coefficient.
    State->Echo.ApFeedCoeff = ALfpMult(float2ALfp(0.5f), aluPow(diffusion, int2ALfp(2)));

    // Calculate the echo all-pass attenuation coefficient.
    State->Echo.ApCoeff = CalcDecayCoeff(ECHO_ALLPASS_LENGTH, decayTime);

    // Calculate the damping coefficient for each low-pass filter.
    State->Echo.LpCoeff = CalcDampingCoeff(hfRatio, echoTime, decayTime,
                                           State->Echo.Coeff, cw);

    /* Calculate the echo mixing coefficients.  The first is applied to the
     * echo itself.  The second is used to attenuate the late reverb when
     * echo depth is high and diffusion is low, so the echo is slightly
     * stronger than the decorrelated echos in the reverb tail.
     */
    State->Echo.MixCoeff[0] = ALfpMult(ALfpMult(reverbGain, lateGain), echoDepth);
    State->Echo.MixCoeff[1] = (int2ALfp(1) - ALfpMult(ALfpMult(echoDepth, float2ALfp(0.5f)), (int2ALfp(1) - diffusion)));
}

// Update the early and late 3D panning gains.
static ALvoid Update3DPanning(const ALCdevice *Device, const ALfp *ReflectionsPan, const ALfp *LateReverbPan, ALverbState *State)
{
    ALfp earlyPan[3] = { ReflectionsPan[0], ReflectionsPan[1],
                            ReflectionsPan[2] };
    ALfp latePan[3] = { LateReverbPan[0], LateReverbPan[1],
                           LateReverbPan[2] };
    const ALfp *speakerGain;
    ALfp dirGain;
    ALfp length;
    ALuint index;
    ALint pos;

    // Calculate the 3D-panning gains for the early reflections and late
    // reverb.
    length = (ALfpMult(earlyPan[0],earlyPan[0]) + ALfpMult(earlyPan[1],earlyPan[1]) + ALfpMult(earlyPan[2],earlyPan[2]));
    if(length > int2ALfp(1))
    {
        length = ALfpDiv(int2ALfp(1), aluSqrt(length));
        earlyPan[0] = ALfpMult(earlyPan[0], length);
        earlyPan[1] = ALfpMult(earlyPan[1], length);
        earlyPan[2] = ALfpMult(earlyPan[2], length);
    }
    length = (ALfpMult(latePan[0],latePan[0]) + ALfpMult(latePan[1],latePan[1]) + ALfpMult(latePan[2],latePan[2]));
    if(length > int2ALfp(1))
    {
        length = ALfpDiv(int2ALfp(1), aluSqrt(length));
        latePan[0] = ALfpMult(latePan[0], length);
        latePan[1] = ALfpMult(latePan[1], length);
        latePan[2] = ALfpMult(latePan[2], length);
    }

    /* This code applies directional reverb just like the mixer applies
     * directional sources.  It diffuses the sound toward all speakers as the
     * magnitude of the panning vector drops, which is only a rough
     * approximation of the expansion of sound across the speakers from the
     * panning direction.
     */
    pos = aluCart2LUTpos(earlyPan[2], earlyPan[0]);
    speakerGain = &Device->PanningLUT[MAXCHANNELS * pos];
    dirGain = aluSqrt((ALfpMult(earlyPan[0], earlyPan[0]) + ALfpMult(earlyPan[2], earlyPan[2])));

    for(index = 0;index < MAXCHANNELS;index++)
        State->Early.PanGain[index] = int2ALfp(0);
    for(index = 0;index < Device->NumChan;index++)
    {
        Channel chan = Device->Speaker2Chan[index];
        State->Early.PanGain[chan] = (int2ALfp(1) + ALfpMult((speakerGain[chan]-int2ALfp(1)),dirGain));
    }


    pos = aluCart2LUTpos(latePan[2], latePan[0]);
    speakerGain = &Device->PanningLUT[MAXCHANNELS * pos];
    dirGain = aluSqrt((ALfpMult(latePan[0], latePan[0]) + ALfpMult(latePan[2], latePan[2])));

    for(index = 0;index < MAXCHANNELS;index++)
         State->Late.PanGain[index] = int2ALfp(0);
    for(index = 0;index < Device->NumChan;index++)
    {
        Channel chan = Device->Speaker2Chan[index];
        State->Late.PanGain[chan] = (int2ALfp(1) + ALfpMult((speakerGain[chan]-int2ALfp(1)),dirGain));
    }
}

// Basic delay line input/output routines.
static __inline ALfp DelayLineOut(DelayLine *Delay, ALuint offset)
{
    return Delay->Line[offset&Delay->Mask];
}

static __inline ALvoid DelayLineIn(DelayLine *Delay, ALuint offset, ALfp in)
{
    Delay->Line[offset&Delay->Mask] = in;
}

// Attenuated delay line output routine.
static __inline ALfp AttenuatedDelayLineOut(DelayLine *Delay, ALuint offset, ALfp coeff)
{
    return ALfpMult(coeff, Delay->Line[offset&Delay->Mask]);
}

// Basic attenuated all-pass input/output routine.
static __inline ALfp AllpassInOut(DelayLine *Delay, ALuint outOffset, ALuint inOffset, ALfp in, ALfp feedCoeff, ALfp coeff)
{
    ALfp out, feed;

    out = DelayLineOut(Delay, outOffset);
    feed = ALfpMult(feedCoeff, in);
    DelayLineIn(Delay, inOffset, (ALfpMult(feedCoeff, (out - feed)) + in));

    // The time-based attenuation is only applied to the delay output to
    // keep it from affecting the feed-back path (which is already controlled
    // by the all-pass feed coefficient).
    return (ALfpMult(coeff, out) - feed);
}

// Given an input sample, this function produces modulation for the late
// reverb.
static __inline ALfp EAXModulation(ALverbState *State, ALfp in)
{
    ALfp sinus, frac;
    ALuint offset;
    ALfp out0, out1;

    // Calculate the sinus rythm (dependent on modulation time and the
    // sampling rate).  The center of the sinus is moved to reduce the delay
    // of the effect when the time or depth are low.
    sinus = (int2ALfp(1) - __cos(ALfpDiv(ALfpMult(float2ALfp(2.0f * M_PI), int2ALfp(State->Mod.Index)), int2ALfp(State->Mod.Range))));

    // The depth determines the range over which to read the input samples
    // from, so it must be filtered to reduce the distortion caused by even
    // small parameter changes.
    State->Mod.Filter = lerp(State->Mod.Filter, State->Mod.Depth,
                             State->Mod.Coeff);

    // Calculate the read offset and fraction between it and the next sample.
    frac   = (int2ALfp(1) + ALfpMult(State->Mod.Filter, sinus));
    offset = (ALuint)ALfp2int(frac);
    frac  = (frac - int2ALfp(offset));

    // Get the two samples crossed by the offset, and feed the delay line
    // with the next input sample.
    out0 = DelayLineOut(&State->Mod.Delay, State->Offset - offset);
    out1 = DelayLineOut(&State->Mod.Delay, State->Offset - offset - 1);
    DelayLineIn(&State->Mod.Delay, State->Offset, in);

    // Step the modulation index forward, keeping it bound to its range.
    State->Mod.Index = (State->Mod.Index + 1) % State->Mod.Range;

    // The output is obtained by linearly interpolating the two samples that
    // were acquired above.
    return lerp(out0, out1, frac);
}

// Delay line output routine for early reflections.
static __inline ALfp EarlyDelayLineOut(ALverbState *State, ALuint index)
{
    return AttenuatedDelayLineOut(&State->Early.Delay[index],
                                  State->Offset - State->Early.Offset[index],
                                  State->Early.Coeff[index]);
}

// Given an input sample, this function produces four-channel output for the
// early reflections.
static __inline ALvoid EarlyReflection(ALverbState *State, ALfp in, ALfp *out)
{
    ALfp d[4], v, f[4];

    // Obtain the decayed results of each early delay line.
    d[0] = EarlyDelayLineOut(State, 0);
    d[1] = EarlyDelayLineOut(State, 1);
    d[2] = EarlyDelayLineOut(State, 2);
    d[3] = EarlyDelayLineOut(State, 3);

    /* The following uses a lossless scattering junction from waveguide
     * theory.  It actually amounts to a householder mixing matrix, which
     * will produce a maximally diffuse response, and means this can probably
     * be considered a simple feed-back delay network (FDN).
     *          N
     *         ---
     *         \
     * v = 2/N /   d_i
     *         ---
     *         i=1
     */
    v = ALfpMult((d[0] + d[1] + d[2] + d[3]), float2ALfp(0.5f));
    // The junction is loaded with the input here.
    v = (v + in);

    // Calculate the feed values for the delay lines.
    f[0] = (v - d[0]);
    f[1] = (v - d[1]);
    f[2] = (v - d[2]);
    f[3] = (v - d[3]);

    // Re-feed the delay lines.
    DelayLineIn(&State->Early.Delay[0], State->Offset, f[0]);
    DelayLineIn(&State->Early.Delay[1], State->Offset, f[1]);
    DelayLineIn(&State->Early.Delay[2], State->Offset, f[2]);
    DelayLineIn(&State->Early.Delay[3], State->Offset, f[3]);

    // Output the results of the junction for all four channels.
    out[0] = ALfpMult(State->Early.Gain, f[0]);
    out[1] = ALfpMult(State->Early.Gain, f[1]);
    out[2] = ALfpMult(State->Early.Gain, f[2]);
    out[3] = ALfpMult(State->Early.Gain, f[3]);
}

// All-pass input/output routine for late reverb.
static __inline ALfp LateAllPassInOut(ALverbState *State, ALuint index, ALfp in)
{
    return AllpassInOut(&State->Late.ApDelay[index],
                        State->Offset - State->Late.ApOffset[index],
                        State->Offset, in, State->Late.ApFeedCoeff,
                        State->Late.ApCoeff[index]);
}

// Delay line output routine for late reverb.
static __inline ALfp LateDelayLineOut(ALverbState *State, ALuint index)
{
    return AttenuatedDelayLineOut(&State->Late.Delay[index],
                                  State->Offset - State->Late.Offset[index],
                                  State->Late.Coeff[index]);
}

// Low-pass filter input/output routine for late reverb.
static __inline ALfp LateLowPassInOut(ALverbState *State, ALuint index, ALfp in)
{
    in = lerp(in, State->Late.LpSample[index], State->Late.LpCoeff[index]);
    State->Late.LpSample[index] = in;
    return in;
}

// Given four decorrelated input samples, this function produces four-channel
// output for the late reverb.
static __inline ALvoid LateReverb(ALverbState *State, ALfp *in, ALfp *out)
{
    ALfp d[4], f[4];

    // Obtain the decayed results of the cyclical delay lines, and add the
    // corresponding input channels.  Then pass the results through the
    // low-pass filters.

    // This is where the feed-back cycles from line 0 to 1 to 3 to 2 and back
    // to 0.
    d[0] = LateLowPassInOut(State, 2, (in[2] + LateDelayLineOut(State, 2)));
    d[1] = LateLowPassInOut(State, 0, (in[0] + LateDelayLineOut(State, 0)));
    d[2] = LateLowPassInOut(State, 3, (in[3] + LateDelayLineOut(State, 3)));
    d[3] = LateLowPassInOut(State, 1, (in[1] + LateDelayLineOut(State, 1)));

    // To help increase diffusion, run each line through an all-pass filter.
    // When there is no diffusion, the shortest all-pass filter will feed the
    // shortest delay line.
    d[0] = LateAllPassInOut(State, 0, d[0]);
    d[1] = LateAllPassInOut(State, 1, d[1]);
    d[2] = LateAllPassInOut(State, 2, d[2]);
    d[3] = LateAllPassInOut(State, 3, d[3]);

    /* Late reverb is done with a modified feed-back delay network (FDN)
     * topology.  Four input lines are each fed through their own all-pass
     * filter and then into the mixing matrix.  The four outputs of the
     * mixing matrix are then cycled back to the inputs.  Each output feeds
     * a different input to form a circlular feed cycle.
     *
     * The mixing matrix used is a 4D skew-symmetric rotation matrix derived
     * using a single unitary rotational parameter:
     *
     *  [  d,  a,  b,  c ]          1 = a^2 + b^2 + c^2 + d^2
     *  [ -a,  d,  c, -b ]
     *  [ -b, -c,  d,  a ]
     *  [ -c,  b, -a,  d ]
     *
     * The rotation is constructed from the effect's diffusion parameter,
     * yielding:  1 = x^2 + 3 y^2; where a, b, and c are the coefficient y
     * with differing signs, and d is the coefficient x.  The matrix is thus:
     *
     *  [  x,  y, -y,  y ]          n = sqrt(matrix_order - 1)
     *  [ -y,  x,  y,  y ]          t = diffusion_parameter * atan(n)
     *  [  y, -y,  x,  y ]          x = cos(t)
     *  [ -y, -y, -y,  x ]          y = sin(t) / n
     *
     * To reduce the number of multiplies, the x coefficient is applied with
     * the cyclical delay line coefficients.  Thus only the y coefficient is
     * applied when mixing, and is modified to be:  y / x.
     */
    f[0] = (d[0] + ALfpMult(State->Late.MixCoeff, (                 d[1] + -1*d[2] + d[3])));
    f[1] = (d[1] + ALfpMult(State->Late.MixCoeff, (-1*d[0]               +    d[2] + d[3])));
    f[2] = (d[2] + ALfpMult(State->Late.MixCoeff, (   d[0] + -1*d[1]               + d[3])));
    f[3] = (d[3] + ALfpMult(State->Late.MixCoeff, (-1*d[0] + -1*d[1] + -1*d[2]           )));

    // Output the results of the matrix for all four channels, attenuated by
    // the late reverb gain (which is attenuated by the 'x' mix coefficient).
    out[0] = ALfpMult(State->Late.Gain, f[0]);
    out[1] = ALfpMult(State->Late.Gain, f[1]);
    out[2] = ALfpMult(State->Late.Gain, f[2]);
    out[3] = ALfpMult(State->Late.Gain, f[3]);

    // Re-feed the cyclical delay lines.
    DelayLineIn(&State->Late.Delay[0], State->Offset, f[0]);
    DelayLineIn(&State->Late.Delay[1], State->Offset, f[1]);
    DelayLineIn(&State->Late.Delay[2], State->Offset, f[2]);
    DelayLineIn(&State->Late.Delay[3], State->Offset, f[3]);
}

// Given an input sample, this function mixes echo into the four-channel late
// reverb.
static __inline ALvoid EAXEcho(ALverbState *State, ALfp in, ALfp *late)
{
    ALfp out, feed;

    // Get the latest attenuated echo sample for output.
    feed = AttenuatedDelayLineOut(&State->Echo.Delay,
                                  State->Offset - State->Echo.Offset,
                                  State->Echo.Coeff);

    // Mix the output into the late reverb channels.
    out = ALfpMult(State->Echo.MixCoeff[0], feed);
    late[0] = (ALfpMult(State->Echo.MixCoeff[1], late[0]) + out);
    late[1] = (ALfpMult(State->Echo.MixCoeff[1], late[1]) + out);
    late[2] = (ALfpMult(State->Echo.MixCoeff[1], late[2]) + out);
    late[3] = (ALfpMult(State->Echo.MixCoeff[1], late[3]) + out);

    // Mix the energy-attenuated input with the output and pass it through
    // the echo low-pass filter.
    feed = (feed + ALfpMult(State->Echo.DensityGain, in));
    feed = lerp(feed, State->Echo.LpSample, State->Echo.LpCoeff);
    State->Echo.LpSample = feed;

    // Then the echo all-pass filter.
    feed = AllpassInOut(&State->Echo.ApDelay,
                        State->Offset - State->Echo.ApOffset,
                        State->Offset, feed, State->Echo.ApFeedCoeff,
                        State->Echo.ApCoeff);

    // Feed the delay with the mixed and filtered sample.
    DelayLineIn(&State->Echo.Delay, State->Offset, feed);
}

// Perform the non-EAX reverb pass on a given input sample, resulting in
// four-channel output.
static __inline ALvoid VerbPass(ALverbState *State, ALfp in, ALfp *early, ALfp *late)
{
    ALfp feed, taps[4];

    // Low-pass filter the incoming sample.
    in = lpFilter2P(&State->LpFilter, 0, in);

    // Feed the initial delay line.
    DelayLineIn(&State->Delay, State->Offset, in);

    // Calculate the early reflection from the first delay tap.
    in = DelayLineOut(&State->Delay, State->Offset - State->DelayTap[0]);
    EarlyReflection(State, in, early);

    // Feed the decorrelator from the energy-attenuated output of the second
    // delay tap.
    in = DelayLineOut(&State->Delay, State->Offset - State->DelayTap[1]);
    feed = ALfpMult(in, State->Late.DensityGain);
    DelayLineIn(&State->Decorrelator, State->Offset, feed);

    // Calculate the late reverb from the decorrelator taps.
    taps[0] = feed;
    taps[1] = DelayLineOut(&State->Decorrelator, State->Offset - State->DecoTap[0]);
    taps[2] = DelayLineOut(&State->Decorrelator, State->Offset - State->DecoTap[1]);
    taps[3] = DelayLineOut(&State->Decorrelator, State->Offset - State->DecoTap[2]);
    LateReverb(State, taps, late);

    // Step all delays forward one sample.
    State->Offset++;
}

// Perform the EAX reverb pass on a given input sample, resulting in four-
// channel output.
static __inline ALvoid EAXVerbPass(ALverbState *State, ALfp in, ALfp *early, ALfp *late)
{
    ALfp feed, taps[4];

    // Low-pass filter the incoming sample.
    in = lpFilter2P(&State->LpFilter, 0, in);

    // Perform any modulation on the input.
    in = EAXModulation(State, in);

    // Feed the initial delay line.
    DelayLineIn(&State->Delay, State->Offset, in);

    // Calculate the early reflection from the first delay tap.
    in = DelayLineOut(&State->Delay, State->Offset - State->DelayTap[0]);
    EarlyReflection(State, in, early);

    // Feed the decorrelator from the energy-attenuated output of the second
    // delay tap.
    in = DelayLineOut(&State->Delay, State->Offset - State->DelayTap[1]);
    feed = ALfpMult(in, State->Late.DensityGain);
    DelayLineIn(&State->Decorrelator, State->Offset, feed);

    // Calculate the late reverb from the decorrelator taps.
    taps[0] = feed;
    taps[1] = DelayLineOut(&State->Decorrelator, State->Offset - State->DecoTap[0]);
    taps[2] = DelayLineOut(&State->Decorrelator, State->Offset - State->DecoTap[1]);
    taps[3] = DelayLineOut(&State->Decorrelator, State->Offset - State->DecoTap[2]);
    LateReverb(State, taps, late);

    // Calculate and mix in any echo.
    EAXEcho(State, in, late);

    // Step all delays forward one sample.
    State->Offset++;
}

// This destroys the reverb state.  It should be called only when the effect
// slot has a different (or no) effect loaded over the reverb effect.
static ALvoid VerbDestroy(ALeffectState *effect)
{
    ALverbState *State = (ALverbState*)effect;
    if(State)
    {
        free(State->SampleBuffer);
        State->SampleBuffer = NULL;
        free(State);
    }
}

// This updates the device-dependant reverb state.  This is called on
// initialization and any time the device parameters (eg. playback frequency,
// or format) have been changed.
static ALboolean VerbDeviceUpdate(ALeffectState *effect, ALCdevice *Device)
{
    ALverbState *State = (ALverbState*)effect;
    ALuint frequency = Device->Frequency;
    ALuint index;

    // Allocate the delay lines.
    if(!AllocLines(AL_FALSE, frequency, State))
        return AL_FALSE;

    // The early reflection and late all-pass filter line lengths are static,
    // so their offsets only need to be calculated once.
    for(index = 0;index < 4;index++)
    {
        State->Early.Offset[index] = ALfp2int(ALfpMult(EARLY_LINE_LENGTH[index],
                                                       int2ALfp(frequency)));
        State->Late.ApOffset[index] = ALfp2int(ALfpMult(ALLPASS_LINE_LENGTH[index],
                                                        int2ALfp(frequency)));
    }

    for(index = 0;index < MAXCHANNELS;index++)
         State->Gain[index] = int2ALfp(0);
    for(index = 0;index < Device->NumChan;index++)
    {
        Channel chan = Device->Speaker2Chan[index];
        State->Gain[chan] = int2ALfp(1);
    }

    return AL_TRUE;
}

// This updates the device-dependant EAX reverb state.  This is called on
// initialization and any time the device parameters (eg. playback frequency,
// format) have been changed.
static ALboolean EAXVerbDeviceUpdate(ALeffectState *effect, ALCdevice *Device)
{
    ALverbState *State = (ALverbState*)effect;
    ALuint frequency = Device->Frequency, index;

    // Allocate the delay lines.
    if(!AllocLines(AL_TRUE, frequency, State))
        return AL_FALSE;

    // Calculate the modulation filter coefficient.  Notice that the exponent
    // is calculated given the current sample rate.  This ensures that the
    // resulting filter response over time is consistent across all sample
    // rates.
    State->Mod.Coeff = aluPow(MODULATION_FILTER_COEFF,
                              ALfpDiv(MODULATION_FILTER_CONST, int2ALfp(frequency)));

    // The early reflection and late all-pass filter line lengths are static,
    // so their offsets only need to be calculated once.
    for(index = 0;index < 4;index++)
    {
        State->Early.Offset[index] = ALfp2int(ALfpMult(EARLY_LINE_LENGTH[index],
                                                       int2ALfp(frequency)));
        State->Late.ApOffset[index] = ALfp2int(ALfpMult(ALLPASS_LINE_LENGTH[index],
                                                        int2ALfp(frequency)));
    }

    // The echo all-pass filter line length is static, so its offset only
    // needs to be calculated once.
    State->Echo.ApOffset = ALfp2int(ALfpMult(ECHO_ALLPASS_LENGTH, int2ALfp(frequency)));

    return AL_TRUE;
}

// This updates the reverb state.  This is called any time the reverb effect
// is loaded into a slot.
static ALvoid VerbUpdate(ALeffectState *effect, ALCcontext *Context, const ALeffect *Effect)
{
    ALverbState *State = (ALverbState*)effect;
    ALuint frequency = Context->Device->Frequency;
    ALfp cw, x, y, hfRatio;

    // Calculate the master low-pass filter (from the master effect HF gain).
    cw = CalcI3DL2HFreq(Effect->Reverb.HFReference, frequency);
    // This is done with 2 chained 1-pole filters, so no need to square g.
    State->LpFilter.coeff = lpCoeffCalc(Effect->Reverb.GainHF, cw);

    // Update the initial effect delay.
    UpdateDelayLine(Effect->Reverb.ReflectionsDelay,
                    Effect->Reverb.LateReverbDelay, frequency, State);

    // Update the early lines.
    UpdateEarlyLines(Effect->Reverb.Gain, Effect->Reverb.ReflectionsGain,
                     Effect->Reverb.LateReverbDelay, State);

    // Update the decorrelator.
    UpdateDecorrelator(Effect->Reverb.Density, frequency, State);

    // Get the mixing matrix coefficients (x and y).
    CalcMatrixCoeffs(Effect->Reverb.Diffusion, &x, &y);
    // Then divide x into y to simplify the matrix calculation.
    State->Late.MixCoeff = ALfpDiv(y, x);

    // If the HF limit parameter is flagged, calculate an appropriate limit
    // based on the air absorption parameter.
    hfRatio = Effect->Reverb.DecayHFRatio;
    if(Effect->Reverb.DecayHFLimit && Effect->Reverb.AirAbsorptionGainHF < int2ALfp(1))
        hfRatio = CalcLimitedHfRatio(hfRatio, Effect->Reverb.AirAbsorptionGainHF,
                                     Effect->Reverb.DecayTime);

    // Update the late lines.
    UpdateLateLines(Effect->Reverb.Gain, Effect->Reverb.LateReverbGain,
                    x, Effect->Reverb.Density, Effect->Reverb.DecayTime,
                    Effect->Reverb.Diffusion, hfRatio, cw, frequency, State);
}

// This updates the EAX reverb state.  This is called any time the EAX reverb
// effect is loaded into a slot.
static ALvoid EAXVerbUpdate(ALeffectState *effect, ALCcontext *Context, const ALeffect *Effect)
{
    ALverbState *State = (ALverbState*)effect;
    ALuint frequency = Context->Device->Frequency;
    ALfp cw, x, y, hfRatio;

    // Calculate the master low-pass filter (from the master effect HF gain).
    cw = CalcI3DL2HFreq(Effect->Reverb.HFReference, frequency);
    // This is done with 2 chained 1-pole filters, so no need to square g.
    State->LpFilter.coeff = lpCoeffCalc(Effect->Reverb.GainHF, cw);

    // Update the modulator line.
    UpdateModulator(Effect->Reverb.ModulationTime,
                    Effect->Reverb.ModulationDepth, frequency, State);

    // Update the initial effect delay.
    UpdateDelayLine(Effect->Reverb.ReflectionsDelay,
                    Effect->Reverb.LateReverbDelay, frequency, State);

    // Update the early lines.
    UpdateEarlyLines(Effect->Reverb.Gain, Effect->Reverb.ReflectionsGain,
                     Effect->Reverb.LateReverbDelay, State);

    // Update the decorrelator.
    UpdateDecorrelator(Effect->Reverb.Density, frequency, State);

    // Get the mixing matrix coefficients (x and y).
    CalcMatrixCoeffs(Effect->Reverb.Diffusion, &x, &y);
    // Then divide x into y to simplify the matrix calculation.
    State->Late.MixCoeff = ALfpDiv(y, x);

    // If the HF limit parameter is flagged, calculate an appropriate limit
    // based on the air absorption parameter.
    hfRatio = Effect->Reverb.DecayHFRatio;
    if(Effect->Reverb.DecayHFLimit && Effect->Reverb.AirAbsorptionGainHF < int2ALfp(1))
        hfRatio = CalcLimitedHfRatio(hfRatio, Effect->Reverb.AirAbsorptionGainHF,
                                     Effect->Reverb.DecayTime);

    // Update the late lines.
    UpdateLateLines(Effect->Reverb.Gain, Effect->Reverb.LateReverbGain,
                    x, Effect->Reverb.Density, Effect->Reverb.DecayTime,
                    Effect->Reverb.Diffusion, hfRatio, cw, frequency, State);

    // Update the echo line.
    UpdateEchoLine(Effect->Reverb.Gain, Effect->Reverb.LateReverbGain,
                   Effect->Reverb.EchoTime, Effect->Reverb.DecayTime,
                   Effect->Reverb.Diffusion, Effect->Reverb.EchoDepth,
                   hfRatio, cw, frequency, State);

    // Update early and late 3D panning.
    Update3DPanning(Context->Device, Effect->Reverb.ReflectionsPan,
                    Effect->Reverb.LateReverbPan, State);
}

// This processes the reverb state, given the input samples and an output
// buffer.
static ALvoid VerbProcess(ALeffectState *effect, const ALeffectslot *Slot, ALuint SamplesToDo, const ALfp *SamplesIn, ALfp (*SamplesOut)[MAXCHANNELS])
{
    ALverbState *State = (ALverbState*)effect;
    ALuint index;
    ALfp early[4], late[4], out[4];
    ALfp gain = Slot->Gain;
    const ALfp *panGain = State->Gain;

    for(index = 0;index < SamplesToDo;index++)
    {
        // Process reverb for this sample.
        VerbPass(State, SamplesIn[index], early, late);

        // Mix early reflections and late reverb.
        out[0] = ALfpMult((early[0] + late[0]), gain);
        out[1] = ALfpMult((early[1] + late[1]), gain);
#ifdef APPORTABLE_OPTIMIZED_OUT
        out[2] = ALfpMult((early[2] + late[2]), gain);
        out[3] = ALfpMult((early[3] + late[3]), gain);
#endif

        // Output the results.
        SamplesOut[index][FRONT_LEFT]   = (SamplesOut[index][FRONT_LEFT]   + ALfpMult(panGain[FRONT_LEFT]   , out[0]));
        SamplesOut[index][FRONT_RIGHT]  = (SamplesOut[index][FRONT_RIGHT]  + ALfpMult(panGain[FRONT_RIGHT]  , out[1]));
#ifdef APPORTABLE_OPTIMIZED_OUT
        SamplesOut[index][FRONT_CENTER] = (SamplesOut[index][FRONT_CENTER] + ALfpMult(panGain[FRONT_CENTER] , out[3]));
        SamplesOut[index][SIDE_LEFT]    = (SamplesOut[index][SIDE_LEFT]    + ALfpMult(panGain[SIDE_LEFT]    , out[0]));
        SamplesOut[index][SIDE_RIGHT]   = (SamplesOut[index][SIDE_RIGHT]   + ALfpMult(panGain[SIDE_RIGHT]   , out[1]));
        SamplesOut[index][BACK_LEFT]    = (SamplesOut[index][BACK_LEFT]    + ALfpMult(panGain[BACK_LEFT]    , out[0]));
        SamplesOut[index][BACK_RIGHT]   = (SamplesOut[index][BACK_RIGHT]   + ALfpMult(panGain[BACK_RIGHT]   , out[1]));
        SamplesOut[index][BACK_CENTER]  = (SamplesOut[index][BACK_CENTER]  + ALfpMult(panGain[BACK_CENTER]  , out[2]));
#endif
    }
}

// This processes the EAX reverb state, given the input samples and an output
// buffer.
static ALvoid EAXVerbProcess(ALeffectState *effect, const ALeffectslot *Slot, ALuint SamplesToDo, const ALfp *SamplesIn, ALfp (*SamplesOut)[MAXCHANNELS])
{
    ALverbState *State = (ALverbState*)effect;
    ALuint index;
    ALfp early[4], late[4];
    ALfp gain = Slot->Gain;

    for(index = 0;index < SamplesToDo;index++)
    {
        // Process reverb for this sample.
        EAXVerbPass(State, SamplesIn[index], early, late);

        // Unfortunately, while the number and configuration of gains for
        // panning adjust according to MAXCHANNELS, the output from the
        // reverb engine is not so scalable.
        SamplesOut[index][FRONT_LEFT] = (SamplesOut[index][FRONT_LEFT] +
           ALfpMult((ALfpMult(State->Early.PanGain[FRONT_LEFT],early[0]) +
            ALfpMult(State->Late.PanGain[FRONT_LEFT],late[0])), gain));
        SamplesOut[index][FRONT_RIGHT] = (SamplesOut[index][FRONT_RIGHT] +
           ALfpMult((ALfpMult(State->Early.PanGain[FRONT_RIGHT],early[1]) +
            ALfpMult(State->Late.PanGain[FRONT_RIGHT],late[1])), gain));
#ifdef APPORTABLE_OPTIMIZED_OUT
        SamplesOut[index][FRONT_CENTER] = (SamplesOut[index][FRONT_CENTER] +
           ALfpMult((ALfpMult(State->Early.PanGain[FRONT_LEFT],early[3]) +
            ALfpMult(State->Late.PanGain[FRONT_CENTER],late[3])), gain));
        SamplesOut[index][SIDE_LEFT] = (SamplesOut[index][SIDE_LEFT] +
           ALfpMult((ALfpMult(State->Early.PanGain[SIDE_LEFT],early[0]) +
            ALfpMult(State->Late.PanGain[SIDE_LEFT],late[0])), gain));
        SamplesOut[index][SIDE_RIGHT] = (SamplesOut[index][SIDE_RIGHT] +
           ALfpMult((ALfpMult(State->Early.PanGain[SIDE_RIGHT],early[1]) +
            ALfpMult(State->Late.PanGain[SIDE_RIGHT],late[1])), gain));
        SamplesOut[index][BACK_LEFT] = (SamplesOut[index][BACK_LEFT] +
           ALfpMult((ALfpMult(State->Early.PanGain[BACK_LEFT],early[0]) +
            ALfpMult(State->Late.PanGain[BACK_LEFT],late[0])), gain));
        SamplesOut[index][BACK_RIGHT] = (SamplesOut[index][BACK_RIGHT] +
           ALfpMult((ALfpMult(State->Early.PanGain[BACK_RIGHT],early[1]) +
            ALfpMult(State->Late.PanGain[BACK_RIGHT],late[1])), gain));
        SamplesOut[index][BACK_CENTER] = (SamplesOut[index][BACK_CENTER] +
           ALfpMult((ALfpMult(State->Early.PanGain[BACK_CENTER],early[2]) +
            ALfpMult(State->Late.PanGain[BACK_CENTER],late[2])), gain));
#endif

    }
}

// This creates the reverb state.  It should be called only when the reverb
// effect is loaded into a slot that doesn't already have a reverb effect.
ALeffectState *VerbCreate(void)
{
    ALverbState *State = NULL;
    ALuint index;

    State = malloc(sizeof(ALverbState));
    if(!State)
        return NULL;

    State->state.Destroy = VerbDestroy;
    State->state.DeviceUpdate = VerbDeviceUpdate;
    State->state.Update = VerbUpdate;
    State->state.Process = VerbProcess;

    State->TotalSamples = 0;
    State->SampleBuffer = NULL;

    State->LpFilter.coeff = int2ALfp(0);
    State->LpFilter.history[0] = int2ALfp(0);
    State->LpFilter.history[1] = int2ALfp(0);

    State->Mod.Delay.Mask = 0;
    State->Mod.Delay.Line = NULL;
    State->Mod.Index = 0;
    State->Mod.Range = 1;
    State->Mod.Depth = int2ALfp(0);
    State->Mod.Coeff = int2ALfp(0);
    State->Mod.Filter = int2ALfp(0);

    State->Delay.Mask = 0;
    State->Delay.Line = NULL;
    State->DelayTap[0] = 0;
    State->DelayTap[1] = 0;

    State->Early.Gain = int2ALfp(0);
    for(index = 0;index < 4;index++)
    {
        State->Early.Coeff[index] = int2ALfp(0);
        State->Early.Delay[index].Mask = 0;
        State->Early.Delay[index].Line = NULL;
        State->Early.Offset[index] = 0;
    }

    State->Decorrelator.Mask = 0;
    State->Decorrelator.Line = NULL;
    State->DecoTap[0] = 0;
    State->DecoTap[1] = 0;
    State->DecoTap[2] = 0;

    State->Late.Gain = int2ALfp(0);
    State->Late.DensityGain = int2ALfp(0);
    State->Late.ApFeedCoeff = int2ALfp(0);
    State->Late.MixCoeff = int2ALfp(0);
    for(index = 0;index < 4;index++)
    {
        State->Late.ApCoeff[index] = int2ALfp(0);
        State->Late.ApDelay[index].Mask = 0;
        State->Late.ApDelay[index].Line = NULL;
        State->Late.ApOffset[index] = 0;

        State->Late.Coeff[index] = int2ALfp(0);
        State->Late.Delay[index].Mask = 0;
        State->Late.Delay[index].Line = NULL;
        State->Late.Offset[index] = 0;

        State->Late.LpCoeff[index] = int2ALfp(0);
        State->Late.LpSample[index] = int2ALfp(0);
    }

    for(index = 0;index < MAXCHANNELS;index++)
    {
        State->Early.PanGain[index] = int2ALfp(0);
        State->Late.PanGain[index] = int2ALfp(0);
    }

    State->Echo.DensityGain = int2ALfp(0);
    State->Echo.Delay.Mask = 0;
    State->Echo.Delay.Line = NULL;
    State->Echo.ApDelay.Mask = 0;
    State->Echo.ApDelay.Line = NULL;
    State->Echo.Coeff = int2ALfp(0);
    State->Echo.ApFeedCoeff = int2ALfp(0);
    State->Echo.ApCoeff = int2ALfp(0);
    State->Echo.Offset = 0;
    State->Echo.ApOffset = 0;
    State->Echo.LpCoeff = int2ALfp(0);
    State->Echo.LpSample = int2ALfp(0);
    State->Echo.MixCoeff[0] = int2ALfp(0);
    State->Echo.MixCoeff[1] = int2ALfp(0);

    State->Offset = 0;

    State->Gain = State->Late.PanGain;

    return &State->state;
}

ALeffectState *EAXVerbCreate(void)
{
    ALeffectState *State = VerbCreate();
    if(State)
    {
        State->DeviceUpdate = EAXVerbDeviceUpdate;
        State->Update = EAXVerbUpdate;
        State->Process = EAXVerbProcess;
    }
    return State;
}
