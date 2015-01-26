//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
// Portions Copyright (c) 2014 James S Urquhart
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

#include "platformEmscripten/platformEmscripten.h"
#include "platformEmscripten/EmscriptenConsole.h"
#include "platform/event.h"
#include "game/gameInterface.h"

#include <stdio.h>

extern "C"
{
extern void js_ConsoleEnabled(const char *prompt);
extern void js_ConsoleDisabled();
extern void js_ConsoleLine(const char *message);
}

EmscriptenConsole *gConsole = NULL;

ConsoleFunction(enableWinConsole, void, 2, 2, "(bool enable)")
{
   if (gConsole)
      gConsole->enable(dAtob(argv[1]));
}

ConsoleFunction(enableDebugOutput, void, 2, 2, "(bool enable)")
{
	if (gConsole)
		gConsole->enableDebugOutput(dAtob(argv[1]));
}

static void EmscriptenConsoleConsumer(ConsoleLogEntry::Level, const char *line)
{
   if (gConsole)
      gConsole->processConsoleLine(line);
}

void EmscriptenConsole::create()
{
   gConsole = new EmscriptenConsole();
}

void EmscriptenConsole::destroy()
{
   if (gConsole)
      delete gConsole;
   gConsole = NULL;
}

void EmscriptenConsole::enable(bool enabled)
{
   if (gConsole == NULL)
      return;
   
   consoleEnabled = enabled;
   if(consoleEnabled)
   {
      printf("Initializing Console...\n");
      printf("Console Initialized.\n");

      js_ConsoleEnabled(Con::getVariable("Con::Prompt"));
   }
   else
   {
      printf("Deactivating Console.");
      js_ConsoleDisabled();
   }
}

void EmscriptenConsole::enableDebugOutput(bool enabled)
{
	if (gConsole != NULL)
	  debugOutputEnabled = enabled;
}


bool EmscriptenConsole::isEnabled()
{
   return gConsole ? gConsole->consoleEnabled : false;
}

EmscriptenConsole::EmscriptenConsole()
{
   consoleEnabled = false;

   clearInBuf();
   
   Con::addConsumer(EmscriptenConsoleConsumer);
}

EmscriptenConsole::~EmscriptenConsole()
{
   Con::removeConsumer(EmscriptenConsoleConsumer);
}

void EmscriptenConsole::processConsoleLine(const char *consoleLine)
{
   if(consoleEnabled)
   {
      printf("%s\n", consoleLine);
   }
}

void EmscriptenConsole::clearInBuf()
{
   dMemset(inBuf, 0, MaxConsoleLineSize);
   inBufPos=0;
}

void Torque2D_postConsole(const char *buffer)
{
   if (!gConsole)
      return;

   dStrncpy(gConsole->postEvent.data, buffer, MaxConsoleLineSize-1);
   gConsole->postEvent.data[MaxConsoleLineSize] = '\0';
   gConsole->postEvent.size = ConsoleEventHeaderSize + dStrlen(gConsole->postEvent.data) + 1;
   Con::printf("=> %s",gConsole->postEvent.data);
   
   Game->postEvent(gConsole->postEvent);
}

