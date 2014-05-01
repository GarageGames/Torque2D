#ifndef _AL_FILTER_H_
#define _AL_FILTER_H_

#include "AL/al.h"
#include "alu.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ALfp coeff;
#ifndef _MSC_VER
    ALfp history[0];
#else
    ALfp history[1];
#endif
} FILTER;

static __inline ALfp lpFilter4P(FILTER *iir, ALuint offset, ALfp input)
{
    ALfp *history = &iir->history[offset];
	ALfp a = iir->coeff;
	ALfp output = input;

    output = output + ALfpMult((history[0]-output),a);
    history[0] = output;
    output = output + ALfpMult((history[1]-output),a);
    history[1] = output;
    output = output + ALfpMult((history[2]-output),a);
    history[2] = output;
    output = output + ALfpMult((history[3]-output),a);
    history[3] = output;

    return output;
}

static __inline ALfp lpFilter2P(FILTER *iir, ALuint offset, ALfp input)
{
    ALfp *history = &iir->history[offset];
    ALfp a = iir->coeff;
    ALfp output = input;

    output = output + ALfpMult((history[0]-output),a);
    history[0] = output;
    output = output + ALfpMult((history[1]-output),a);
    history[1] = output;

    return output;
}

static __inline ALfp lpFilter1P(FILTER *iir, ALuint offset, ALfp input)
{
    ALfp *history = &iir->history[offset];
    ALfp a = iir->coeff;
    ALfp output = input;

    output = output + ALfpMult((history[0]-output),a);
    history[0] = output;

    return output;
}

static __inline ALfp lpFilter4PC(const FILTER *iir, ALuint offset, ALfp input)
{
    const ALfp *history = &iir->history[offset];
    ALfp a = iir->coeff;
    ALfp output = input;

    output = output + ALfpMult((history[0]-output),a);
    output = output + ALfpMult((history[1]-output),a);
    output = output + ALfpMult((history[2]-output),a);
    output = output + ALfpMult((history[3]-output),a);

    return output;
}

static __inline ALfp lpFilter2PC(const FILTER *iir, ALuint offset, ALfp input)
{
    const ALfp *history = &iir->history[offset];
    ALfp a = iir->coeff;
    ALfp output = input;

    output = output + ALfpMult((history[0]-output),a);
    output = output + ALfpMult((history[1]-output),a);

    return output;
}

static __inline ALfp lpFilter1PC(FILTER *iir, ALuint offset, ALfp input)
{
    const ALfp *history = &iir->history[offset];
    ALfp a = iir->coeff;
    ALfp output = input;

    output = output + ALfpMult((history[0]-output),a);

    return output;
}

/* Calculates the low-pass filter coefficient given the pre-scaled gain and
 * cos(w) value. Note that g should be pre-scaled (sqr(gain) for one-pole,
 * sqrt(gain) for four-pole, etc) */
static __inline ALfp lpCoeffCalc(ALfp g, ALfp cw)
{
    ALfp a = int2ALfp(0);

    /* Be careful with gains < 0.01, as that causes the coefficient
     * head towards 1, which will flatten the signal */
    g = __max(g, float2ALfp(0.01f));
    if(g < float2ALfp(0.9999f)) /* 1-epsilon */ {
		ALfp tmp; tmp = ALfpMult(ALfpMult(int2ALfp(2),g),(int2ALfp(1)-cw)) - ALfpMult(ALfpMult(g,g),(int2ALfp(1) - ALfpMult(cw,cw)));
        a = ALfpDiv((int2ALfp(1) - ALfpMult(g,cw) - aluSqrt(tmp)), (int2ALfp(1) - g));
	}

    return a;
}


typedef struct ALfilter
{
    // Filter type (AL_FILTER_NULL, ...)
    ALenum type;

    ALfp Gain;
    ALfp GainHF;

    // Index to itself
    ALuint filter;
} ALfilter;


ALvoid ReleaseALFilters(ALCdevice *device);

#ifdef __cplusplus
}
#endif

#endif
