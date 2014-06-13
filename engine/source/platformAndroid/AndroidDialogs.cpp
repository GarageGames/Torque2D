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
#include "sim/simBase.h"
#include "platform/nativeDialogs/fileDialog.h"
#include "platform/threads/mutex.h"
#include "memory/safeDelete.h"
#include "math/mMath.h"
#include "string/unicode.h"
#include "console/consoleTypes.h"
#include "platform/threads/thread.h"

bool FileDialog::Execute()
{
    return true;
}

//-----------------------------------------------------------------------------
// Default Path Property - String Validated on Write
//-----------------------------------------------------------------------------
bool FileDialog::setDefaultPath(void* obj, const char* data)
{

   if( !data )
      return true;

   return true;

};

//-----------------------------------------------------------------------------
// Default File Property - String Validated on Write
//-----------------------------------------------------------------------------
bool FileDialog::setDefaultFile(void* obj, const char* data)
{
   if( !data )
      return true;

   return true;
};
