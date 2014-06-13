//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _PLATFORMAL_H_
#define _PLATFORMAL_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#if defined(TORQUE_OS_OSX)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include "platform/eaxtypes.h"
#elif defined(TORQUE_OS_ANDROID)
//Android uses openal soft from https://github.com/AerialX/openal-soft-android
#include <AL/al.h>
#include <AL/alc.h>
#elif defined(TORQUE_OS_LINUX)
#include <AL/al.h>
#include <AL/alc.h>
#include "platform/eaxtypes.h"
#elif defined(TORQUE_OS_IOS)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined(DUMMY_PLATFORM)
#include "al/altypes.h"
#include "al/alctypes.h"
#include "al/eaxtypes.h"
#define AL_FUNCTION(fn_return,fn_name,fn_args, fn_value) extern fn_return (FN_CDECL *fn_name)fn_args;
#include "al/al_func.h"
#include "al/alc_func.h"
#include "al/eax_func.h"
#undef AL_FUNCTION
#elif defined(TORQUE_OS_EMSCRIPTEN)
#include <AL/al.h>
#include <AL/alc.h>
#include "platform/eaxtypes.h"
#else
// declare externs of the AL fns here.
#include "al/altypes.h"
#include "al/alctypes.h"
#include "al/eaxtypes.h"
#define AL_FUNCTION(fn_return,fn_name,fn_args, fn_value) extern fn_return (FN_CDECL *fn_name)fn_args;
#include "al/al_func.h"
#include "al/alc_func.h"
#include "al/eax_func.h"
#undef AL_FUNCTION
#endif

#if defined(TORQUE_OS_IOS)

#define AssertNoOALError(inMessage)				\
	result = alGetError();							\
	AssertFatal( result == AL_NO_ERROR, inMessage)	\
 
#endif

namespace Audio
{

bool OpenALInit();
void OpenALShutdown();

bool OpenALDLLInit();
void OpenALDLLShutdown();

// special alx flags
#define AL_GAIN_LINEAR                  0xFF01

// helpers
F32 DBToLinear(F32 value);
F32 linearToDB(F32 value);

}  // end namespace Audio


#endif  // _H_PLATFORMAL_
