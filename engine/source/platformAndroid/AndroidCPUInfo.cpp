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

#include "platformAndroid/platformAndroid.h"
#include "console/console.h"
#include "string/stringTable.h"
#include <math.h>




void Processor::init()
{
	
   Con::printf("System & Processor Information:");

   Con::printf("   Android version: %0.0f", platState.osVersion );

	// -Mat FIXME: USE SYSTEM FUNCTION to get version number for 
	//just use OS version for now
   Con::printf("   CarbonLib version: %0.0f", platState.osVersion );
   
   Con::printf("   Physical RAM: %dMB", 128);

   Con::printf("   Logical RAM: %dMB", 128);

   PlatformSystemInfo.processor.mhz = 412;

   //PlatformSystemInfo.processor.type =  ARM_1176;
   PlatformSystemInfo.processor.name = StringTable->insert("ARM 1176");

   PlatformSystemInfo.processor.properties = CPU_PROP_PPCMIN;

	Con::printf("   %s, %d Mhz", PlatformSystemInfo.processor.name, PlatformSystemInfo.processor.mhz);
   if (PlatformSystemInfo.processor.properties & CPU_PROP_PPCMIN)
      Con::printf("   FPU detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_ALTIVEC)
      Con::printf("   AltiVec detected");

   Con::printf(" ");
}

