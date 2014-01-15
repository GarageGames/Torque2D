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

#include "platform/platform.h"
#include "console/console.h"
#include "math/mMath.h"


extern void mInstallLibrary_C();
extern void mInstallLibrary_ASM();


extern void mInstall_AMD_Math();
extern void mInstall_Library_SSE();


//--------------------------------------
ConsoleFunction( mathInit, void, 1, 10, "( extension ) Use the MathInit function to install a specified math extensions, or to detect and enable all extensions.\n"
                                                                "Generally speaking, the best extension choice is to used detect. This will automatically detected and enable all extensions supported by the current processor. It will also print out a list of the extension that were enabled to the console\n"
                                                                "@param extension Can be any of these:\ndetect – Detect all supported extensions and enable.\nC - Enable standard C extensions.\nFPU - Enable floating-point-unit extensions.\nMMX - Enable Intel MMX extensions.\n3DNOW - Enable AMD 3DNOW extensions.\nSSE - Enable Intel SSE extensions.\n"
                                                                "@return No return value.")


{
   U32 properties = CPU_PROP_C;  // C entensions are always used

   if (argc == 1)
   {
         Math::init(0);
         return;
   }
   for (argc--, argv++; argc; argc--, argv++)
   {
      if (dStricmp(*argv, "DETECT") == 0) {
         Math::init(0);
         return;
      }
      if (dStricmp(*argv, "C") == 0) {
         properties |= CPU_PROP_C;
         continue;
      }
      if (dStricmp(*argv, "FPU") == 0) {
         properties |= CPU_PROP_FPU;
         continue;
      }
      if (dStricmp(*argv, "MMX") == 0) {
         properties |= CPU_PROP_MMX;
         continue;
      }
      if (dStricmp(*argv, "3DNOW") == 0) {
         properties |= CPU_PROP_3DNOW;
         continue;
      }
      if (dStricmp(*argv, "SSE") == 0) {
         properties |= CPU_PROP_SSE;
         continue;
      }
      Con::printf("Error: MathInit(): ignoring unknown math extension '%s'", *argv);
   }
   Math::init(properties);
}



//------------------------------------------------------------------------------
void Math::init(U32 properties)
{
   Con::printSeparator();
   Con::printf("Math Initialization:");

   if (!properties)
      // detect what's available
      properties = PlatformSystemInfo.processor.properties;
   else
      // Make sure we're not asking for anything that's not supported
      properties &= PlatformSystemInfo.processor.properties;

   Con::printf("   Installing Standard C extensions");
   mInstallLibrary_C();

   Con::printf("   Installing Assembly extensions");
   mInstallLibrary_ASM();

   if (properties & CPU_PROP_FPU)
   {
      Con::printf("   Installing FPU extensions");
   }

   if (properties & CPU_PROP_MMX)
   {
      Con::printf("   Installing MMX extensions");
      if (properties & CPU_PROP_3DNOW)
      {
         Con::printf("   Installing 3DNow extensions");
         mInstall_AMD_Math();
      }
   }

   if (properties & CPU_PROP_SSE)
   {
      Con::printf("   Installing SSE extensions");
      mInstall_Library_SSE();
   }

   Con::printf(" ");
}


