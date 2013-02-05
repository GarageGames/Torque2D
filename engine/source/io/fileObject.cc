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

#include "fileObject.h"

IMPLEMENT_CONOBJECT(FileObject);

bool FileObject::isEOF()
{
   return mCurPos == mBufferSize;
}

FileObject::FileObject()
{
   mFileBuffer = NULL;
   mBufferSize = 0;
   mCurPos = 0;
}

FileObject::~FileObject()
{
   dFree(mFileBuffer);
}

void FileObject::close()
{
   stream.close();
   dFree(mFileBuffer);
   mFileBuffer = NULL;
   mBufferSize = mCurPos = 0;
}

bool FileObject::openForWrite(const char *fileName, const bool append)
{
   char buffer[1024];
   Con::expandPath(buffer, sizeof(buffer), fileName);

   close();

   if(buffer == NULL || *buffer == 0)
      return false;

   if ( !append )
      return( ResourceManager->openFileForWrite(stream, buffer) );

   // Use the WriteAppend flag so it doesn't clobber the existing file:
   if ( !ResourceManager->openFileForWrite(stream, buffer, File::WriteAppend) )
      return( false );

   stream.setPosition( stream.getStreamSize() );
   return( true );
}

bool FileObject::openForRead(const char* /*fileName*/)
{
   AssertFatal(false, "Error, not yet implemented!");
   return false;
}

bool FileObject::readMemory(const char *fileName)
{
   StringTableEntry fileToOpen = NULL;

   char buffer[1024];
    
   Con::expandPath( buffer, sizeof( buffer ), fileName );

   fileToOpen = StringTable->insert(buffer);

   close();
   Stream *s = ResourceManager->openStream(fileToOpen);
   if(!s)
      return false;
   mBufferSize = ResourceManager->getSize(fileToOpen);
   mFileBuffer = (U8 *) dMalloc(mBufferSize + 1);
   mFileBuffer[mBufferSize] = 0;
   s->read(mBufferSize, mFileBuffer);
   ResourceManager->closeStream(s);
   mCurPos = 0;

   return true;
}

const U8 *FileObject::readLine()
{
   if(!mFileBuffer)
      return (U8 *) "";

   U32 tokPos = mCurPos;

   for(;;)
   {
      if(mCurPos == mBufferSize)
         break;

      if(mFileBuffer[mCurPos] == '\r')
      {
         mFileBuffer[mCurPos++] = 0;
         if(mFileBuffer[mCurPos] == '\n')
            mCurPos++;
         break;
      }

      if(mFileBuffer[mCurPos] == '\n')
      {
         mFileBuffer[mCurPos++] = 0;
         break;
      }

      mCurPos++;
   }

   return mFileBuffer + tokPos;
}

void FileObject::peekLine( U8* line, S32 length )
{
   if(!mFileBuffer)
   {
      line[0] = '\0';
      return;
   }

   // Copy the line into the buffer. We can't do this like readLine because
   // we can't modify the file buffer.
   S32 i = 0;
   U32 tokPos = mCurPos;
   while( ( tokPos != mBufferSize ) && ( mFileBuffer[tokPos] != '\r' ) && ( mFileBuffer[tokPos] != '\n' ) && ( i < ( length - 1 ) ) )
      line[i++] = mFileBuffer[tokPos++];

   line[i++] = '\0';

   //if( i == length )
      //Con::warnf( "FileObject::peekLine - The line contents could not fit in the buffer (size %d). Truncating.", length );
}

void FileObject::writeLine(const U8 *line)
{
   stream.write(dStrlen((const char *) line), line);
   stream.write(2, "\r\n");
}

void FileObject::writeObject( SimObject* object, const U8* objectPrepend )
{
   if( objectPrepend == NULL )
      stream.write(2, "\r\n");
   else
      stream.write(dStrlen((const char *) objectPrepend), objectPrepend );
   object->write( stream, 0 );
}

ConsoleMethod( FileObject, openForRead, bool, 3, 3, "( filename ) Use the openForRead method to open a previously created file for reading.\n"
                                                                "@param filename The path and filename of the file to open for reading.\n"
                                                                "@return Returns true if the file was successfully opened for reading, false otherwise.\n"
                                                                "@sa close, OpenForAppend, OpenForWrite")
{
   return object->readMemory(argv[2]);
}

ConsoleMethod( FileObject, openForWrite, bool, 3, 3, "( filename ) Use the openForWrite method to previously created or a new file for writing. In either case, the file will be overwritten.\n"
                                                                "@param filename The path and filename of the file to open for writing.\n"
                                                                "@return Returns true if the file was successfully opened for writing, false otherwise.\n"
                                                                "@sa close, OpenForAppend, openForRead")
{
   return object->openForWrite(argv[2]);
}

ConsoleMethod( FileObject, openForAppend, bool, 3, 3, "( filename ) Use the openForAppend method to open a previously created file for appending. If the file specified by filename does not exist, the file is created first.\n"
                                                                "@param filename The path and filename of the file to open for appending.\n"
                                                                "@return Returns true if the file was successfully opened for appending, false otherwise.\n"
                                                                "@sa close, openForRead, openForWrite")
{
   return object->openForWrite(argv[2], true);
}

ConsoleMethod( FileObject, isEOF, bool, 2, 2, "() Use the isEOF method to check to see if the end of the current file (opened for read) has been reached.\n"
                                                                "@return Returns true if the end of file has been reached, false otherwise.\n"
                                                                "@sa openForRead")
{
   return object->isEOF();
}

ConsoleMethod( FileObject, readLine, const char*, 2, 2, "() Use the readLine method to read a single line from a file previously opened for reading.\n"
                                                                "Use isEOF to check for end of file while reading.\n"
                                                                "@return Returns the next line in the file, or a NULL string if the end-of-file has been reached.\n"
                                                                "@sa isEOF, openForRead")
{
   return (const char *) object->readLine();
}

ConsoleMethod( FileObject, peekLine, const char*, 2, 2, "Read a line from the file without moving the stream position.")
{
   char *line = Con::getReturnBuffer( 512 );
   object->peekLine( (U8*)line, 512 );
   return line;
}

ConsoleMethod( FileObject, writeLine, void, 3, 3, "( text ) Use the writeLine method to write a value ( text ) into a file that was previously opened for appending or over-writing.\n"
                                                                "@param text The value to write to the file.\n"
                                                                "@return No return value.\n"
                                                                "@sa openForAppend, openForWrite")
{
   object->writeLine((const U8 *) argv[2]);
}

ConsoleMethod( FileObject, close, void, 2, 2, "() Use the close method to close the current file handle. If the file was opened for writing, this flushes the contents of the last write to disk.\n"
                                                                "@return No return value.\n"
                                                                "@sa openForAppend, openForRead, openForWrite")
{
   object->close();
}

ConsoleMethod( FileObject, writeObject, void, 3, 4, "FileObject.writeObject(SimObject, object prepend)" )
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
