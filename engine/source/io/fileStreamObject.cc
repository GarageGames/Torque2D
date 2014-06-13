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

#include "console/console.h"
#include "fileStreamObject.h"

#include "fileStreamObject_ScriptBinding.h"

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

FileStreamObject::FileStreamObject()
{
}

FileStreamObject::~FileStreamObject()
{
   close();
}

IMPLEMENT_CONOBJECT(FileStreamObject);

//////////////////////////////////////////////////////////////////////////

bool FileStreamObject::onAdd()
{
   // [tom, 2/12/2007] Skip over StreamObject's onAdd() so that we can
   // be instantiated from script.
   return SimObject::onAdd();
}

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

bool FileStreamObject::open(const char *filename, FileStream::AccessMode mode)
{
   close();

   if(! mFileStream.open(filename, mode))
      return false;

   mStream = &mFileStream;
   return true;
}

void FileStreamObject::close()
{
   mFileStream.close();
   mStream = NULL;
}
