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

#ifndef _PLATFORM_CPU_H_
#define _PLATFORM_CPU_H_

//-----------------------------------------------------------------------------

enum ProcessorType
{
   // x86
   CPU_X86Compatible,
   CPU_Intel_Unknown,
   CPU_Intel_486,
   CPU_Intel_Pentium,
   CPU_Intel_PentiumMMX,
   CPU_Intel_PentiumPro,
   CPU_Intel_PentiumII,
   CPU_Intel_PentiumCeleron,
   CPU_Intel_PentiumIII,
   CPU_Intel_Pentium4,
   CPU_AMD_K6,
   CPU_AMD_K6_2,
   CPU_AMD_K6_3,
   CPU_AMD_Athlon,
   CPU_AMD_Unknown,
   CPU_Cyrix_6x86,
   CPU_Cyrix_MediaGX,
   CPU_Cyrix_6x86MX,
   CPU_Cyrix_GXm,          // Media GX w/ MMX
   CPU_Cyrix_Unknown,
   // PowerPC
   CPU_PowerPC_Unknown,
   CPU_PowerPC_601,
   CPU_PowerPC_603,
   CPU_PowerPC_603e,
   CPU_PowerPC_603ev,
   CPU_PowerPC_604,
   CPU_PowerPC_604e,
   CPU_PowerPC_604ev,
   CPU_PowerPC_G3,
   CPU_PowerPC_G4,
   CPU_PowerPC_G4_7450,
   CPU_PowerPC_G4_7455,
   CPU_PowerPC_G4_7447, 
   CPU_PowerPC_G5
};

//-----------------------------------------------------------------------------

enum x86Properties
{
    // x86 properties
    CPU_PROP_C         = (1<<0),
    CPU_PROP_FPU       = (1<<1),
    CPU_PROP_MMX       = (1<<2),     // Integer-SIMD
    CPU_PROP_3DNOW     = (1<<3),     // AMD Float-SIMD
    CPU_PROP_SSE       = (1<<4),     // PentiumIII SIMD
    CPU_PROP_RDTSC     = (1<<5)     // Read Time Stamp Counter
    //   CPU_PROP_SSE2      = (1<<6),   // Pentium4 SIMD
    //   CPU_PROP_MP        = (1<<7)      // Multi-processor system
};

//-----------------------------------------------------------------------------

enum PPCProperties
{
    // PowerPC properties
    CPU_PROP_PPCMIN    = (1<<0),
    CPU_PROP_ALTIVEC   = (1<<1),     // Float-SIMD
    CPU_PROP_PPCMP     = (1<<7)      // Multi-processor system
};

//-----------------------------------------------------------------------------

struct Processor
{
   static void init();
};

//-----------------------------------------------------------------------------

struct TorqueSystemInfo
{
    struct Processor
    {
        ProcessorType type;
        const char *name;
        U32         mhz;
        U32         properties;      // CPU type specific enum
    } processor;
};

extern TorqueSystemInfo PlatformSystemInfo;

#endif // _PLATFORM_CPU_H_
