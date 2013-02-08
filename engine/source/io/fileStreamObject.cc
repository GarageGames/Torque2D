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

//////////////////////////////////////////////////////////////////////////
// Local Globals
//////////////////////////////////////////////////////////////////////////

static const struct
{
   const char *strMode;
   FileStream::AccessMode mode;
} gModeMap[]=
{
   { "read", FileStream::Read },
   { "write", FileStream::Write },
   { "readwrite", FileStream::ReadWrite },
   { "writeappend", FileStream::WriteAppend },
   { NULL, (FileStream::AccessMode)0 }
};

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

//////////////////////////////////////////////////////////////////////////
// Console Methods
//////////////////////////////////////////////////////////////////////////

ConsoleMethod(FileStreamObject, open, bool, 4, 4, "(filename, mode) Open a file. Mode can be one of Read, Write, ReadWrite or WriteAppend.")
{
   FileStream::AccessMode mode;
   bool found = false;
   for(S32 i = 0;gModeMap[i].strMode;++i)
   {
      if(dStricmp(gModeMap[i].strMode, argv[3]) == 0)
      {
         mode = gModeMap[i].mode;
         found = true;
         break;
      }
   }

   if(! found)
   {
      Con::errorf("FileStreamObject::open - Mode must be one of Read, Write, ReadWrite or WriteAppend.");
      return false;
   }

   char buffer[1024];
   Con::expandPath(buffer, sizeof(buffer), argv[2]);
   return object->open(buffer, mode);
}

ConsoleMethod(FileStreamObject, close, void, 2, 2, "() Close the file.")
{
   object->close();
}
