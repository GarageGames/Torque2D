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
#include "io/resource/resourceManager.h"
#include "platform/platformInput.h"

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif

#include "taggedStrings_ScriptBinding.h"
#include "inputManagement_ScriptBinding.h"

#ifdef TORQUE_ALLOW_JOURNALING
   static U32 journalDepth = 1;
#endif

//Luma:	Console function to tell if this is a TORQUE_OS_IOS build
ConsoleFunction(isiPhoneBuild, bool, 1, 1, "Returns true if this is a iPhone build, false otherwise")
{
#ifdef	TORQUE_OS_IOS
   return true;
#else
   return false;
#endif	//TORQUE_OS_IOS
}
