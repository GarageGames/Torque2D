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
#include "console/consoleTypes.h"
#include "platform/threads/mutex.h"

//Added for the cprintf below
#include <stdarg.h>
#include <stdio.h>

S32 sgBackgroundProcessSleepTime = 200;
S32 sgTimeManagerProcessInterval = 0;


void Platform::initConsole()
{
   Con::addVariable("Pref::backgroundSleepTime", TypeS32, &sgBackgroundProcessSleepTime);
   Con::addVariable("Pref::timeManagerProcessInterval", TypeS32, &sgTimeManagerProcessInterval);
}

S32 Platform::getBackgroundSleepTime()
{
   return sgBackgroundProcessSleepTime;
}

void Platform::cprintf( const char* str )
{
    printf( "%s \n", str );
}

bool Platform::hasExtension(const char* pFilename, const char* pExtension)
{
    // Sanity!
    AssertFatal( pFilename != NULL, "Filename cannot be NULL." );
    AssertFatal( pExtension != NULL, "Extension cannot be NULL." );

    // Find filename length.
    const U32 filenameLength = dStrlen( pFilename );

    // Find extension length.
    const U32 extensionLength = dStrlen( pExtension );

    // Skip if extension is longer than filename.
    if ( extensionLength >= filenameLength )
        return false;

    // Check if extension exists.
    return dStricmp( pFilename + filenameLength - extensionLength, pExtension ) == 0;
}
