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

ConsoleMethodGroupBeginWithDocs(StreamObject, SimObject)

/*! Gets the current status of the StreamObject
    @return The current status as a string (Ok, IOError, EOS, IllegalCall, Closed, UnknownError, Invalid)
*/
ConsoleMethodWithDocs(StreamObject, getStatus, const char *, 2, 2, ())
{
   return object->getStatus();
}

/*! Test for end of stream
    @return Returns true if at the end of the stream, false otherwise.
*/
ConsoleMethodWithDocs(StreamObject, isEOS, ConsoleBool, 2, 2, ())
{
   return object->isEOS();
}

/*! Test for end of file stream (identical to isEOS()
    @return Returns true if at the end of the stream, false otherwise.
    @sa isEOS
*/
ConsoleMethodWithDocs(StreamObject, isEOF, ConsoleBool, 2, 2, ())
{
   return object->isEOS();
}

//////////////////////////////////////////////////////////////////////////

/*!  
    @return Returns the current position in the stream as an integer or zero if failed
*/
ConsoleMethodWithDocs(StreamObject, getPosition, ConsoleInt, 2, 2, ())
{
   return object->getPosition();
}

/*! Resets the current stream position
    @param The desired index
    @return Returns true if succeeded, flase otherwise
*/
ConsoleMethodWithDocs(StreamObject, setPosition, ConsoleBool, 3, 3, (newPosition))
{
   return object->setPosition(dAtoi(argv[2]));
}

/*! Get the size of the stream
    @return The size of the stream as an integer
*/
ConsoleMethodWithDocs(StreamObject, getStreamSize, ConsoleInt, 2, 2, ())
{
   return object->getStreamSize();
}

//////////////////////////////////////////////////////////////////////////

/*! Read the stream until '\' or EOS
    @return A string containing the read line or an empty string if failed
*/
ConsoleMethodWithDocs(StreamObject, readLine, const char *, 2, 2, ())
{
   const char *str = object->readLine();
   return str ? str : "";
}

/*! Writes a line of text to the stream buffer
    @param The line to write
    @return No return value.
*/
ConsoleMethodWithDocs(StreamObject, writeLine, ConsoleVoid, 3, 3, (line))
{
   object->writeLine((U8 *)argv[2]);
}

//////////////////////////////////////////////////////////////////////////

/*! Read a String and insert it into a StringTable
    @param caseSensitive A boolean representing whether the parser should ignore case or not (default false)
    @return Returns the string, or empty string if failed
*/
ConsoleMethodWithDocs(StreamObject, readSTString, const char *, 2, 3, ([caseSensitive = false]))
{
   const char *str = object->readSTString(argc > 2 ? dAtob(argv[2]) : false);
   return str ? str : "";
}

/*! Reads a string from a stream buffer
    @return The string or an empty string if failed.
*/
ConsoleMethodWithDocs(StreamObject, readString, const char *, 2, 2, ())
{
   const char *str = object->readString();
   return str ? str : "";
}

/*! Reads a string of provided length from the stream buffer.
    @param The maximum length to read in
    @return The requested string
*/
ConsoleMethodWithDocs(StreamObject, readLongString, const char *, 3, 3, (maxLength))
{
   const char *str = object->readLongString(dAtoi(argv[2]));
   return str ? str : "";
}

/*! Writes a string to buffer or provided length
    @param maxLength The maximum length to write
    @param string The string to write
    @return No return value
*/
ConsoleMethodWithDocs(StreamObject, writeLongString, ConsoleVoid, 4, 4, (maxLength, string))
{
   object->writeLongString(dAtoi(argv[2]), argv[3]);
}

/*! Write a string to the stream buffer
    @param string The string to write
    @param maxLength The maximum length to write (default 255).
    @return No return value.
*/
ConsoleMethodWithDocs(StreamObject, writeString, ConsoleVoid, 3, 4, (string, [maxLength = 255]))
{
   object->writeString(argv[2], argc > 3 ? dAtoi(argv[3]) : 255);
}

//////////////////////////////////////////////////////////////////////////

/*! Copies stream contents from current position
    @param other The StreamObject from which to copy
    @return Returns true on success, and false otherwise.
*/
ConsoleMethodWithDocs(StreamObject, copyFrom, ConsoleBool, 3, 3, (StreamObject other))
{
   StreamObject *other = dynamic_cast<StreamObject *>(Sim::findObject(argv[2]));
   if(other == NULL)
      return false;

   return object->copyFrom(other);
}

ConsoleMethodGroupEndWithDocs(StreamObject)
