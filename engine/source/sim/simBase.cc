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
#include "sim/simBase.h"
#include "string/stringTable.h"
#include "console/console.h"
#include "io/fileStream.h"
#include "input/actionMap.h"
#include "io/resource/resourceManager.h"
#include "io/fileObject.h"
#include "console/consoleInternal.h"
#include "debug/profiler.h"
#include "console/ConsoleTypeValidators.h"
#include "memory/frameAllocator.h"

// Script bindings.
#include "simBase_ScriptBinding.h"

namespace Sim
{
   // Don't forget to InstantiateNamed* in simManager.cc - DMM
   ImplementNamedSet(ActiveActionMapSet)
   ImplementNamedSet(GhostAlwaysSet)
   ImplementNamedSet(BehaviorSet)
   ImplementNamedSet(AchievementSet);
   ImplementNamedGroup(ActionMapGroup)
   ImplementNamedGroup(ClientGroup)
   ImplementNamedGroup(GuiGroup)
   ImplementNamedGroup(GuiDataGroup)
   ImplementNamedGroup(TCPGroup)

   //groups created on the client
   ImplementNamedGroup(ClientConnectionGroup)
   ImplementNamedGroup(ChunkFileGroup)
}   

//-----------------------------------------------------------------------------
