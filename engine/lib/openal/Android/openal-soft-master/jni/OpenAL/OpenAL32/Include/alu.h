#ifndef _ALU_H_
#define _ALU_H_

#include "config.h"

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"

#include <limits.h>
#include <math.h>
#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif

#ifdef HAVE_POWF
#define aluPow(x,y) (float2ALfp(powf(ALfp2float(x), ALfp2float(y))))
#else
#define aluPow(x,y) (float2ALfp((float)pow((double)ALfp2float(x), (double)ALfp2float(y))))
#endif

#ifdef HAVE_SQRTF
#define aluSqrt(x) (float2ALfp(sqrtf(ALfp2float(x))))
#else
#define aluSqrt(x) (float2ALfp((float)sqrt((double)ALfp2float(x))))
#endif

#ifdef HAVE_ACOSF
#define aluAcos(x) (float2ALfp(acosf(ALfp2float(x))))
#else
#define aluAcos(x) (float2ALfp((float)acos((double)ALfp2float(x))))
#endif

#ifdef HAVE_ATANF
#define aluAtan(x) (float2ALfp(atanf(ALfp2float(x))))
#else
#define aluAtan(x) (float2ALfp((float)atan((double)ALfp2float(x))))
#endif

#ifdef HAVE_FABSF
#define aluFabs(x) (float2ALfp(fabsf(ALfp2float(x))))
#else
#define aluFabs(x) (float2ALfp((float)fabs((double)ALfp2float(x))))
#endif

// FIXME make this better
#if defined(max) && !defined(__max)
#define __max(x,y) float2ALfp(max(ALfp2float(x),ALfp2float(y)))
#endif
#if defined(min) && !defined(__min)
#define __min(x,y) float2ALfp(min(ALfp2float(x),ALfp2float(y)))
#endif

#define QUADRANT_NUM  128
#define LUT_NUM       (4 * QUADRANT_NUM)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FRONT_LEFT = 0,
    FRONT_RIGHT,
    FRONT_CENTER,
    LFE,
    BACK_LEFT,
    BACK_RIGHT,
    BACK_CENTER,
    SIDE_LEFT,
    SIDE_RIGHT,
} Channel;

#ifndef MAXCHANNELS
#define MAXCHANNELS (SIDE_RIGHT+1)
#error MAXCHANNELS
#endif

#define BUFFERSIZE 4096

#define FRACTIONBITS (14)
#define FRACTIONONE  (1<<FRACTIONBITS)
#define FRACTIONMASK (FRACTIONONE-1)

/* Size for temporary stack storage of buffer data. Larger values need more
 * stack, while smaller values may need more iterations. The value needs to be
 * a sensible size, however, as it constrains the max stepping value used for
 * mixing.
 * The mixer requires being able to do two samplings per mixing loop. A 16KB
 * buffer can hold 512 sample frames for a 7.1 float buffer. With the cubic
 * resampler (which requires 3 padding sample frames), this limits the maximum
 * step to about 508. This means that buffer_freq*source_pitch cannot exceed
 * device_freq*508 for an 8-channel 32-bit buffer. */
#ifndef STACK_DATA_SIZE
#define STACK_DATA_SIZE  16384
#endif

//FIXME this code assumes ALfp==ALdfp
static __inline ALdfp lerp(ALdfp val1, ALdfp val2, ALdfp mu)
{
	ALdfp retval;
	retval = val1 + ALfpMult((val2-val1),mu);
	return retval;
}
static __inline ALdfp cubic(ALdfp val0, ALdfp val1, ALdfp val2, ALdfp val3, ALdfp mu)
{
	ALdfp retval;
	ALdfp mu2;mu2 = ALfpMult(mu,mu);
	ALdfp a0;a0 = ALfpMult(float2ALfp(-0.5f),val0) + ALfpMult(float2ALfp( 1.5f),val1) + ALfpMult(float2ALfp(-1.5f),val2) + ALfpMult(float2ALfp( 0.5), val3);
	ALdfp a1;a1 =                            val0  + ALfpMult(float2ALfp(-2.5f),val1) + ALfpMult(float2ALfp( 2.0), val2) + ALfpMult(float2ALfp(-0.5f),val3);
	ALdfp a2;a2 = ALfpMult(float2ALfp(-0.5f),val0)                                    + ALfpMult(float2ALfp( 0.5), val2);
	ALdfp a3;a3 =                                                               val1;

    retval = ALfpMult(ALfpMult(a0,mu),mu2) + ALfpMult(a1,mu2) + ALfpMult(a2,mu) + a3;
	return retval;
}

struct ALsource;

ALvoid aluInitPanning(ALCdevice *Device);
ALint aluCart2LUTpos(ALfp re, ALfp im);

ALvoid CalcSourceParams(struct ALsource *ALSource, const ALCcontext *ALContext);
ALvoid CalcNonAttnSourceParams(struct ALsource *ALSource, const ALCcontext *ALContext);

ALvoid MixSource(struct ALsource *Source, ALCdevice *Device, ALuint SamplesToDo);

ALvoid aluMixData(ALCdevice *device, ALvoid *buffer, ALsizei size);
ALvoid aluHandleDisconnect(ALCdevice *device);

#ifdef __cplusplus
}
#endif

#endif

