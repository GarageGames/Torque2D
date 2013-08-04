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

//
//  AndroidMiniProfiler.h
//  Torque2D
//
//  Created by puap on 9/26/08.
//  Copyright 2008  PUAP. All rights reserved.
//

#ifndef _ANDROID_PROFILER_H
#define _ANDROID_PROFILER_H

#include "torqueConfig.h"

#ifdef TORQUE_OS_ANDROID
//#define TORQUE_OS_ANDROID_PROFILE

#define ANDROID_PROFILER_START( _x ) AndroidProfilerProfileStartName( #_x );
#define ANDROID_PROFILER_END( _x )	AndroidProfilerProfileEndName( #_x );

//--------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include "collection/vector.h"
#include "console/console.h"

#define ANDROID_PROFILER_MAX_CALLS 256

class AndroidProfilerData {
public:
	//make an array of these s we can still count in recursive functions
	U32 timeMach;//time stamp
	U32 timeMS;//time stamp
	const char *name;//name of this profile
	int type; //0 for start 1 for end
	int invokes;//Number of times we have recursed into this profile. Used with start[]
};


//PUAP
//--------------------------------------------------------------------------------------------------------------------------------------------

void AndroidProfilerResetAll();
void AndroidProfilerProfilerInit();
int AndroidProfilerGetCount();

void AndroidProfilerStart( const char *name );
void AndroidProfilerEnd( const char *name );

int AndroidProfilerFindProfileEnd( const char *name, int startCount);

void AndroidProfilerPrintResult( int item );
void AndroidProfilerPrintAllResults();

//-Mat to get instruction names
const char *getInstructionName( int index );


#endif//TORQUE_OS_ANDROID

#endif// _ANDROID_PROFILER_H
