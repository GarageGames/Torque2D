#ifndef _AL_AUXEFFECTSLOT_H_
#define _AL_AUXEFFECTSLOT_H_

#include "AL/al.h"
#include "alEffect.h"
#include "alFilter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ALeffectState ALeffectState;

typedef struct ALeffectslot
{
    ALeffect effect;

    ALfp Gain;
    ALboolean AuxSendAuto;

    ALeffectState *EffectState;

    ALfp WetBuffer[BUFFERSIZE];

    ALfp ClickRemoval[1];
    ALfp PendingClicks[1];

    ALuint refcount;

    // Index to itself
    ALuint effectslot;

    struct ALeffectslot *next;
} ALeffectslot;


ALvoid ReleaseALAuxiliaryEffectSlots(ALCcontext *Context);


struct ALeffectState {
    ALvoid (*Destroy)(ALeffectState *State);
    ALboolean (*DeviceUpdate)(ALeffectState *State, ALCdevice *Device);
    ALvoid (*Update)(ALeffectState *State, ALCcontext *Context, const ALeffect *Effect);
    ALvoid (*Process)(ALeffectState *State, const ALeffectslot *Slot, ALuint SamplesToDo, const ALfp *SamplesIn, ALfp (*SamplesOut)[MAXCHANNELS]);
};

ALeffectState *NoneCreate(void);
ALeffectState *EAXVerbCreate(void);
ALeffectState *VerbCreate(void);
ALeffectState *EchoCreate(void);
ALeffectState *ModulatorCreate(void);

#define ALEffect_Destroy(a)         ((a)->Destroy((a)))
#define ALEffect_DeviceUpdate(a,b)  ((a)->DeviceUpdate((a),(b)))
#define ALEffect_Update(a,b,c)      ((a)->Update((a),(b),(c)))
#define ALEffect_Process(a,b,c,d,e) ((a)->Process((a),(b),(c),(d),(e)))


#ifdef __cplusplus
}
#endif

#endif
