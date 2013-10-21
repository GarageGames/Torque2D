#ifndef _AL_SOURCE_H_
#define _AL_SOURCE_H_

#define MAX_SENDS                 4

#include "alFilter.h"
#include "alu.h"
#include "AL/al.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    POINT_RESAMPLER = 0,
    LINEAR_RESAMPLER,
    CUBIC_RESAMPLER,

    RESAMPLER_MAX,
    RESAMPLER_MIN = -1,
    RESAMPLER_DEFAULT = LINEAR_RESAMPLER
} resampler_t;
extern resampler_t DefaultResampler;

extern const ALsizei ResamplerPadding[RESAMPLER_MAX];
extern const ALsizei ResamplerPrePadding[RESAMPLER_MAX];


typedef struct ALbufferlistitem
{
    struct ALbuffer         *buffer;
    struct ALbufferlistitem *next;
    struct ALbufferlistitem *prev;
} ALbufferlistitem;

typedef struct ALsource
{
    ALfp      flPitch;
    ALfp      flGain;
    ALfp      flOuterGain;
    ALfp      flMinGain;
    ALfp      flMaxGain;
    ALfp      flInnerAngle;
    ALfp      flOuterAngle;
    ALfp      flRefDistance;
    ALfp      flMaxDistance;
    ALfp      flRollOffFactor;
    ALfp      vPosition[3];
    ALfp      vVelocity[3];
    ALfp      vOrientation[3];
    ALboolean    bHeadRelative;
    ALboolean    bLooping;
    ALenum       DistanceModel;

    // Apportably Extension
    ALuint    priority;

    resampler_t  Resampler;

    ALenum       state;
    ALuint       position;
    ALuint       position_fraction;

    struct ALbuffer *Buffer;

    ALbufferlistitem *queue; // Linked list of buffers in queue
    ALuint BuffersInQueue;   // Number of buffers in queue
    ALuint BuffersPlayed;    // Number of buffers played on this loop

    ALfilter DirectFilter;

    struct {
        struct ALeffectslot *Slot;
        ALfilter WetFilter;
    } Send[MAX_SENDS];

    ALboolean DryGainHFAuto;
    ALboolean WetGainAuto;
    ALboolean WetGainHFAuto;
    ALfp   OuterGainHF;

    ALfp AirAbsorptionFactor;
    ALfp RoomRolloffFactor;
    ALfp DopplerFactor;

    ALint  lOffset;
    ALint  lOffsetType;

    // Source Type (Static, Streaming, or Undetermined)
    ALint  lSourceType;

    // Current target parameters used for mixing
    ALboolean NeedsUpdate;
    struct {
        ALint Step;

        /* A mixing matrix. First subscript is the channel number of the input
         * data (regardless of channel configuration) and the second is the
         * channel target (eg. FRONT_LEFT) */
        ALfp DryGains[MAXCHANNELS][MAXCHANNELS];
        FILTER iirFilter;
        ALfp history[MAXCHANNELS*2];

        struct {
            ALfp WetGain;
            FILTER iirFilter;
            ALfp history[MAXCHANNELS];
        } Send[MAX_SENDS];
    } Params;

    ALvoid (*Update)(struct ALsource *self, const ALCcontext *context);

    // Index to itself
    ALuint source;
} ALsource;
#define ALsource_Update(s,a)  ((s)->Update(s,a))

ALvoid ReleaseALSources(ALCcontext *Context);

#ifdef __cplusplus
}
#endif

#endif
