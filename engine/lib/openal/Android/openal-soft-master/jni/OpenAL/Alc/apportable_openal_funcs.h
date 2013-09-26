#ifndef __APPORTABLE_OPENAL_FUNCS_H__
#define __APPORTABLE_OPENAL_FUNCS_H__
#include <jni.h>

typedef struct {
  void (*alc_android_suspend)();
  void (*alc_android_resume)();
  void (*alc_android_set_java_vm)(JavaVM*);
} ApportableOpenALFuncs;
ApportableOpenALFuncs apportableOpenALFuncs;

#endif
