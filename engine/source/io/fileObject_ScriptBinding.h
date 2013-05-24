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

ConsoleMethodGroupBeginWithDocs(FileObject, SimObject)

/*! Use the openForRead method to open a previously created file for reading.
    @param filename The path and filename of the file to open for reading.
    @return Returns true if the file was successfully opened for reading, false otherwise.
    @sa close, OpenForAppend, OpenForWrite
*/
ConsoleMethodWithDocs( FileObject, openForRead, ConsoleBool, 3, 3, ( filename ))
{
   return object->readMemory(argv[2]);
}

/*! Use the openForWrite method to previously created or a new file for writing. In either case, the file will be overwritten.
    @param filename The path and filename of the file to open for writing.
    @return Returns true if the file was successfully opened for writing, false otherwise.
    @sa close, OpenForAppend, openForRead
*/
ConsoleMethodWithDocs( FileObject, openForWrite, ConsoleBool, 3, 3, ( filename ))
{
   return object->openForWrite(argv[2]);
}

/*! Use the openForAppend method to open a previously created file for appending. If the file specified by filename does not exist, the file is created first.
    @param filename The path and filename of the file to open for appending.
    @return Returns true if the file was successfully opened for appending, false otherwise.
    @sa close, openForRead, openForWrite
*/
ConsoleMethodWithDocs( FileObject, openForAppend, ConsoleBool, 3, 3, ( filename ))
{
   return object->openForWrite(argv[2], true);
}

/*! Use the isEOF method to check to see if the end of the current file (opened for read) has been reached.
    @return Returns true if the end of file has been reached, false otherwise.
    @sa openForRead
*/
ConsoleMethodWithDocs( FileObject, isEOF, ConsoleBool, 2, 2, ())
{
   return object->isEOF();
}

/*! Use the readLine method to read a single line from a file previously opened for reading.
    Use isEOF to check for end of file while reading.
    @return Returns the next line in the file, or a NULL string if the end-of-file has been reached.
    @sa isEOF, openForRead
*/
ConsoleMethodWithDocs( FileObject, readLine, ConsoleString, 2, 2, ())
{
   return (const char *) object->readLine();
}

/*! Read a line from the file without moving the stream position.
*/
ConsoleMethodWithDocs( FileObject, peekLine, ConsoleString, 2, 2, ())
{
   char *line = Con::getReturnBuffer( 512 );
   object->peekLine( (U8*)line, 512 );
   return line;
}

/*! Use the writeLine method to write a value ( text ) into a file that was previously opened for appending or over-writing.
    @param text The value to write to the file.
    @return No return value.
    @sa openForAppend, openForWrite
*/
ConsoleMethodWithDocs( FileObject, writeLine, ConsoleVoid, 3, 3, ( text ))
{
   object->writeLine((const U8 *) argv[2]);
}

/*! Use the close method to close the current file handle. If the file was opened for writing, this flushes the contents of the last write to disk.
    @return No return value.
    @sa openForAppend, openForRead, openForWrite
*/
ConsoleMethodWithDocs( FileObject, close, ConsoleVoid, 2, 2, ())
{
   object->close();
}

/*! 
*/
ConsoleMethodWithDocs( FileObject, writeObject, ConsoleVoid, 3, 4, (SimObject, object prepend))
{
   SimObject* obj = Sim::findObject( argv[2] );
   if( !obj )
   {
      Con::printf("FileObject::writeObject - Invalid Object!");
      return;
   }

   char *objName = NULL;
   if( argc == 4 )
      objName = (char*)argv[3];

   object->writeObject( obj, (const U8*)objName );
}

ConsoleMethodGroupEndWithDocs(FileObject)
