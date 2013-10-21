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
#include "game/version.h"
#include "console/console.h"
#include "string/stringTable.h"
#include "io/resource/resourceManager.h"

#include "version_ScriptBinding.h"

//-----------------------------------------------------------------------------

static const U32 csgVersionNumber = TORQUE_GAME_ENGINE;
static StringTableEntry sgCompanyName = NULL;
static StringTableEntry sgProductName = NULL;

//-----------------------------------------------------------------------------

U32 getVersionNumber()
{
   return csgVersionNumber;
}

//-----------------------------------------------------------------------------

const char* getVersionString()
{
   return TORQUE_GAME_VERSION_STRING;
}

//-----------------------------------------------------------------------------

const char* getCompileTimeString()
{
   return __DATE__ " at " __TIME__;
}

//-----------------------------------------------------------------------------

const char* getCompanyName()
{
	return sgCompanyName;
}

//-----------------------------------------------------------------------------

const char* getProductName()
{
	return sgProductName;
}

//-----------------------------------------------------------------------------

void setCompanyName(StringTableEntry companyName)
{
	sgCompanyName = companyName;
}

//-----------------------------------------------------------------------------

void setProductName(StringTableEntry productName)
{
	sgProductName = productName;
}

//-----------------------------------------------------------------------------
