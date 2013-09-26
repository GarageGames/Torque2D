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

#include "streamObject.h"

#include "streamObject_ScriptBinding.h"

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

StreamObject::StreamObject()
{
   mStream = NULL;
}

StreamObject::StreamObject(Stream *stream)
{
   mStream = stream;
}

StreamObject::~StreamObject()
{
}

IMPLEMENT_CONOBJECT(StreamObject);

//////////////////////////////////////////////////////////////////////////

bool StreamObject::onAdd()
{
   if(mStream == NULL)
   {
      Con::errorf("StreamObject::onAdd - StreamObject can not be instantiated from script.");
      return false;
   }
   return Parent::onAdd();
}

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

const char * StreamObject::getStatus()
{
   if(mStream == NULL)
      return "";

   switch(mStream->getStatus())
   {
      case Stream::Ok:
         return "Ok";
      case Stream::IOError:
         return "IOError";
      case Stream::EOS:
         return "EOS";
      case Stream::IllegalCall:
         return "IllegalCall";
      case Stream::Closed:
         return "Closed";
      case Stream::UnknownError:
         return "UnknownError";

      default:
         return "Invalid";
   }
}

//////////////////////////////////////////////////////////////////////////

const char * StreamObject::readLine()
{
   if(mStream == NULL)
      return NULL;

   char *buffer = Con::getReturnBuffer(256);
   mStream->readLine((U8 *)buffer, 256);
   return buffer;
}

const char * StreamObject::readString()
{
   if(mStream == NULL)
      return NULL;

   char *buffer = Con::getReturnBuffer(256);
   mStream->readString(buffer);
   return buffer;
}

const char *StreamObject::readLongString(U32 maxStringLen)
{
   if(mStream == NULL)
      return NULL;

   char *buffer = Con::getReturnBuffer(maxStringLen + 1);
   mStream->readLongString(maxStringLen, buffer);
   return buffer;
}
