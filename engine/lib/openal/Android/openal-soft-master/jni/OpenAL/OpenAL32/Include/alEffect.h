// NOTE:  The effect structure is getting too large, it may be a good idea to
//        start using a union or another form of unified storage.
#ifndef _AL_EFFECT_H_
#define _AL_EFFECT_H_

#include "AL/al.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EAXREVERB = 0,
    REVERB,
    ECHO,
    MODULATOR,

    MAX_EFFECTS
};
extern ALboolean DisabledEffects[MAX_EFFECTS];

typedef struct ALeffect
{
    // Effect type (AL_EFFECT_NULL, ...)
    ALenum type;

    struct {
        // Shared Reverb Properties
        ALfp Density;
        ALfp Diffusion;
        ALfp Gain;
        ALfp GainHF;
        ALfp DecayTime;
        ALfp DecayHFRatio;
        ALfp ReflectionsGain;
        ALfp ReflectionsDelay;
        ALfp LateReverbGain;
        ALfp LateReverbDelay;
        ALfp AirAbsorptionGainHF;
        ALfp RoomRolloffFactor;
        ALboolean DecayHFLimit;

        // Additional EAX Reverb Properties
        ALfp GainLF;
        ALfp DecayLFRatio;
        ALfp ReflectionsPan[3];
        ALfp LateReverbPan[3];
        ALfp EchoTime;
        ALfp EchoDepth;
        ALfp ModulationTime;
        ALfp ModulationDepth;
        ALfp HFReference;
        ALfp LFReference;
    } Reverb;

    struct {
        ALfp Delay;
        ALfp LRDelay;

        ALfp Damping;
        ALfp Feedback;

        ALfp Spread;
    } Echo;

    struct {
        ALfp Frequency;
        ALfp HighPassCutoff;
        ALint Waveform;
    } Modulator;

    // Index to itself
    ALuint effect;
} ALeffect;


ALvoid ReleaseALEffects(ALCdevice *device);

#ifdef __cplusplus
}
#endif

#endif
