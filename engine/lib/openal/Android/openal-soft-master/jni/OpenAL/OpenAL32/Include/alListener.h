#ifndef _AL_LISTENER_H_
#define _AL_LISTENER_H_

#include "AL/al.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ALlistener_struct
{
    ALfp Position[3];
    ALfp Velocity[3];
    ALfp Forward[3];
    ALfp Up[3];
    ALfp Gain;
    ALfp MetersPerUnit;
} ALlistener;

#ifdef __cplusplus
}
#endif

#endif
