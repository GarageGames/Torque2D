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

//////////////////////////////////////////////////////////////////////////
// Console Methods
//////////////////////////////////////////////////////////////////////////

ConsoleMethod(StreamObject, getStatus, const char *, 2, 2, "() Gets the current status of the StreamObject"
              "@return The current status as a string (Ok, IOError, EOS, IllegalCall, Closed, UnknownError, Invalid)")
{
   return object->getStatus();
}

ConsoleMethod(StreamObject, isEOS, bool, 2, 2, "() Test for end of stream"
              "@return Returns true if at the end of the stream, false otherwise.")
{
   return object->isEOS();
}

ConsoleMethod(StreamObject, isEOF, bool, 2, 2, "() Test for end of file stream (identical to isEOS()"
              "@return Returns true if at the end of the stream, false otherwise."
              "@sa isEOS")
{
   return object->isEOS();
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(StreamObject, getPosition, S32, 2, 2, "()\n" 
              "@return Returns the current position in the stream as an integer or zero if failed")
{
   return object->getPosition();
}

ConsoleMethod(StreamObject, setPosition, bool, 3, 3, "(newPosition) Resets the current stream position\n"
              "@param The desired index\n"
              "@return Returns true if succeeded, flase otherwise")
{
   return object->setPosition(dAtoi(argv[2]));
}

ConsoleMethod(StreamObject, getStreamSize, S32, 2, 2, "() Get the size of the stream"
              "@return The size of the stream as an integer")
{
   return object->getStreamSize();
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(StreamObject, readLine, const char *, 2, 2, "() Read the stream until '\\n' or EOS\n"
              "@return A string containing the read line or an empty string if failed\n")
{
   const char *str = object->readLine();
   return str ? str : "";
}

ConsoleMethod(StreamObject, writeLine, void, 3, 3, "(line) Writes a line of text to the stream buffer\n"
              "@param The line to write\n"
              "@return No return value.")
{
   object->writeLine((U8 *)argv[2]);
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(StreamObject, readSTString, const char *, 2, 3, "([caseSensitive = false]) Read a String and insert it into a StringTable\n"
              "@param caseSensitive A boolean representing whether the parser should ignore case or not (default false)\n"
              "@return Returns the string, or empty string if failed\n")
{
   const char *str = object->readSTString(argc > 2 ? dAtob(argv[2]) : false);
   return str ? str : "";
}

ConsoleMethod(StreamObject, readString, const char *, 2, 2, "() Reads a string from a stream buffer\n"
              "@return The string or an empty string if failed.")
{
   const char *str = object->readString();
   return str ? str : "";
}

ConsoleMethod(StreamObject, readLongString, const char *, 3, 3, "(maxLength) Reads a string of provided length from the stream buffer.\n"
              "@param The maximum length to read in\n"
              "@return The requested string")
{
   const char *str = object->readLongString(dAtoi(argv[2]));
   return str ? str : "";
}

ConsoleMethod(StreamObject, writeLongString, void, 4, 4, "(maxLength, string) Writes a string to buffer or provided length\n"
              "@param maxLength The maximum length to write\n"
              "@param string The string to write\n"
              "@return No return value")
{
   object->writeLongString(dAtoi(argv[2]), argv[3]);
}

ConsoleMethod(StreamObject, writeString, void, 3, 4, "(string, [maxLength = 255]) Write a string to the stream buffer\n"
              "@param string The string to write\n"
              "@param maxLength The maximum length to write (default 255).\n"
              "@return No return value.")
{
   object->writeString(argv[2], argc > 3 ? dAtoi(argv[3]) : 255);
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(StreamObject, copyFrom, bool, 3, 3, "(StreamObject other) Copies stream contents from current position\n"
              "@param other The StreamObject from which to copy\n"
              "@return Returns true on success, and false otherwise.")
{
   StreamObject *other = dynamic_cast<StreamObject *>(Sim::findObject(argv[2]));
   if(other == NULL)
      return false;

   return object->copyFrom(other);
}
