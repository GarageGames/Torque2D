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

#include "gui/messageVector.h"
#include "io/fileObject.h"

IMPLEMENT_CONOBJECT(MessageVector);

//-------------------------------------- Console functions
ConsoleMethod( MessageVector, clear, void, 2, 2, "() Clear the message vector.\n"
                                                "@return No return value")
{
   object->clear();
}

ConsoleMethod( MessageVector, pushBackLine, void, 4, 4, "( msg [ , tag ] ) Use the pushBackLine method to push a line onto the back of the list.\n"
                                                                "@param msg The text to add to this control.\n"
                                                                "@param tag An optional tag to tag this line with. If not tag is supplied, a tag of 0 is used.\n"
                                                                "@return No return value.\n"
                                                                "@sa popBackLine, popFrontLine, insertLine, pushFrontLine")
{
   U32 tag = 0;
   if (argc == 4)
      tag = dAtoi(argv[3]);

   object->pushBackLine(argv[2], tag);
}

ConsoleMethod( MessageVector, popBackLine, bool, 2, 2, "() Use the popBackLine method to pop a line from the back of the list; destroys the line.\n"
                                                                "@return No return value.\n"
                                                                "@sa insertLine, pushBackLine, pushFrontLine")
{
   if (object->getNumLines() == 0) {
      Con::errorf(ConsoleLogEntry::General, "MessageVector::popBackLine(): underflow");
      return false;
   }

   object->popBackLine();
   return true;
}

ConsoleMethod( MessageVector, pushFrontLine, void, 3, 4, "( msg [ , tag ] ) Use the pushFrontLine method to push a line onto the front of the vector.\n"
                                                                "@param msg The text to add to this control.\n"
                                                                "@param tag An optional tag to tag this line with. If not tag is supplied, a tag of 0 is used.\n"
                                                                "@return No return value.\n"
                                                                "@sa popBackLine, popFrontLine, insertLine, pushBackLine")
{
   U32 tag = 0;
   if (argc == 4)
      tag = dAtoi(argv[3]);

   object->pushFrontLine(argv[2], tag);
}

ConsoleMethod( MessageVector, popFrontLine, bool, 2, 2,"() Use the popFrontLine method to pop a line from the front of the vector, destroying the line.\n"
                                                                "@return No return value.\n"
                                                                "@sa insertLine, pushBackLine, pushFrontLine")
{
   if (object->getNumLines() == 0) {
      Con::errorf(ConsoleLogEntry::General, "MessageVector::popFrontLine(): underflow");
      return false;
   }

   object->popFrontLine();
   return true;
}


ConsoleMethod( MessageVector, insertLine, bool, 4, 5, "( pos , msg [ , tag ] ) Use the insertLine method to insert a new line into the vector at the specified position. An optional tag may also be applied.\n"
                                                                "@param pos The line at which to insert the new text.\n"
                                                                "@param msg The text to add to this control.\n"
                                                                "@param tag An optional tag to tag this line with. If not tag is supplied, a tag of 0 is used.\n"
                                                                "@return No return value.\n"
                                                                "@sa pushBackLine, pushFrontLine")
{
   U32 pos = U32(dAtoi(argv[2]));
   if (pos > object->getNumLines())
      return false;

   S32 tag = 0;
   if (argc == 5)
      tag = dAtoi(argv[4]);

   object->insertLine(pos, argv[3], tag);
   return true;
}

ConsoleMethod( MessageVector, deleteLine, bool, 3, 3, "( lineIndex ) Delete the line at the specified position.\n"
                                                                "@param lineIndex The line to delete in this vector.\n"
                                                                "@return No return value.\n"
                                                                "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 pos = U32(dAtoi(argv[2]));
   if (pos >= object->getNumLines())
      return false;

   object->deleteLine(pos);
   return true;
}

ConsoleMethod( MessageVector, dump, void, 3, 4, "( filename [ , header ] ) Dump the message vector to a file, optionally prefixing the file with a header.\n"
                                                                "@param filename The file to dump this vector to.\n"
                                                                "@param header An optional string containing data to dump to the new file prior to dumping the vector.\n"
                                                                "@return No return value")
{

   if ( argc == 4 )
      object->dump( argv[2], argv[3] );
   else
      object->dump( argv[2] );
}

ConsoleMethod( MessageVector, getNumLines, S32, 2, 2, "() Use the getNumLines method to get the number of lines in the vector.\n"
                                                                "@return Returns an integer value equal to the line count for this vector.\n"
                                                                "@sa insertLine, pushBackLine, pushFrontLine")
{
   return object->getNumLines();
}

ConsoleMethod( MessageVector, getLineTextByTag, const char*, 3, 3, "( tag ) Use the getLineTextByTag method to scan through the lines in the vector, returning the first line that has a matching tag.\n"
                                                                "@param tag An special marker that may have been used when creating lines in the vector.\n"
                                                                "@return Returns the contents of the first line found with a matching tag or NULL indicating no match.\n"
                                                                "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 tag = dAtoi(argv[2]);

   for(U32 i = 0; i < object->getNumLines(); i++)
      if(object->getLine(i).messageTag == tag)
         return object->getLine(i).message;
   return "";
}

ConsoleMethod( MessageVector, getLineIndexByTag, S32, 3, 3, "( tag ) Scan through the vector, returning the line number of the first line that matches the specified tag; else returns -1 if no match was found.\n"
                                                                "@param tag A special marker, possibly embedded in one or more lines in the vector.\n"
                                                                "@return Returns the line number of the first line found with the specified tag, otherwise returns -1.\n"
                                                                "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 tag = dAtoi(argv[2]);

   for(U32 i = 0; i < object->getNumLines(); i++)
      if(object->getLine(i).messageTag == tag)
         return i;
   return -1;
}

ConsoleMethod( MessageVector, getLineText, const char*, 3, 3, "( index ) Use the getLineIndex method to get the text at a specified line.\n"
                                                                "@param index The index in the vector from which to retrieve a line of text.\n"
                                                                "@return Returns the text at the specified line, or NULL indicating a bad index.\n"
                                                                "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 pos = U32(dAtoi(argv[2]));
   if (pos >= object->getNumLines()) {
      Con::errorf(ConsoleLogEntry::General, "MessageVector::getLineText(con): out of bounds line");
      return "";
   }

   return object->getLine(pos).message;
}

ConsoleMethod( MessageVector, getLineTag, S32, 3, 3, "(line) Use the getLineTag method to retrieve the tag for the specified line.\n"
                                                                "@param line Line to search for tag in.\n"
                                                                "@return Returns a tag value or 0 indicating no tag found.\n"
                                                                "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 pos = U32(dAtoi(argv[2]));
   if (pos >= object->getNumLines()) {
      Con::errorf(ConsoleLogEntry::General, "MessageVector::getLineTag(con): out of bounds line");
      return 0;
   }

   return object->getLine(pos).messageTag;
}

//--------------------------------------------------------------------------
MessageVector::MessageVector()
{
   VECTOR_SET_ASSOCIATION(mMessageLines);
   VECTOR_SET_ASSOCIATION(mSpectators);
}


//--------------------------------------------------------------------------
MessageVector::~MessageVector()
{
   for (U32 i = 0; i < (U32)mMessageLines.size(); i++) {
      char* pDelete = const_cast<char*>(mMessageLines[i].message);
      delete [] pDelete;
      mMessageLines[i].message = 0;
      mMessageLines[i].messageTag = 0xFFFFFFFF;
   }
   mMessageLines.clear();
}


//--------------------------------------------------------------------------
void MessageVector::initPersistFields()
{
   Parent::initPersistFields();
}



//--------------------------------------------------------------------------
bool MessageVector::onAdd()
{
   return Parent::onAdd();
}


//--------------------------------------------------------------------------
void MessageVector::onRemove()
{
   // Delete all the lines from the observers, and then forcibly detatch ourselves
   //
   for (S32 i = mMessageLines.size() - 1; i >= 0; i--)
      spectatorMessage(LineDeleted, i);
   spectatorMessage(VectorDeletion, 0);
   mSpectators.clear();

   Parent::onRemove();
}


//--------------------------------------------------------------------------
void MessageVector::pushBackLine(const char* newMessage, const S32 newMessageTag)
{
   insertLine(mMessageLines.size(), newMessage, newMessageTag);
}


void MessageVector::popBackLine()
{
   AssertFatal(mMessageLines.size() != 0, "MessageVector::popBackLine: nothing to pop!");
   if (mMessageLines.size() == 0)
      return;

   deleteLine(mMessageLines.size() - 1);
}

void MessageVector::clear()
{
   while(mMessageLines.size())
      deleteLine(mMessageLines.size() - 1);
}

//--------------------------------------------------------------------------
void MessageVector::pushFrontLine(const char* newMessage, const S32 newMessageTag)
{
   insertLine(0, newMessage, newMessageTag);
}


void MessageVector::popFrontLine()
{
   AssertFatal(mMessageLines.size() != 0, "MessageVector::popBackLine: nothing to pop!");
   if (mMessageLines.size() == 0)
      return;

   deleteLine(0);
}


//--------------------------------------------------------------------------
void MessageVector::insertLine(const U32   position,
                               const char* newMessage,
                               const S32   newMessageTag)
{
   AssertFatal(position >= 0 && position <= (U32)mMessageLines.size(), "MessageVector::insertLine: out of range position!");
   AssertFatal(newMessage != NULL, "Error, no message to insert!");

   U32 len = dStrlen(newMessage) + 1;
   char* copy = new char[len];
   dStrcpy(copy, newMessage);

   mMessageLines.insert(position);
   mMessageLines[position].message    = copy;
   mMessageLines[position].messageTag = newMessageTag;

   // Notify of insert
   spectatorMessage(LineInserted, position);
}


//--------------------------------------------------------------------------
void MessageVector::deleteLine(const U32 position)
{
   AssertFatal(position >= 0 && position < (U32)mMessageLines.size(), "MessageVector::deleteLine: out of range position!");

   char* pDelete = const_cast<char*>(mMessageLines[position].message);
   delete [] pDelete;
   mMessageLines[position].message    = NULL;
   mMessageLines[position].messageTag = 0xFFFFFFFF;

   mMessageLines.erase(position);

   // Notify of delete
   spectatorMessage(LineDeleted, position);
}


//--------------------------------------------------------------------------
bool MessageVector::dump( const char* filename, const char* header )
{
   Con::printf( "Dumping message vector %s to %s...", getName(), filename );
   FileObject file;
   if ( !file.openForWrite( filename ) )
      return( false );

   // If passed a header line, write it out first:
   if ( header )
      file.writeLine( (const U8*) header );

   // First write out the record count:
   char* lineBuf = (char*) dMalloc( 10 );
   dSprintf( lineBuf, 10, "%d", mMessageLines.size() );
   file.writeLine( (const U8*) lineBuf );

   // Write all of the lines of the message vector:
   U32 len;
   for ( U32 i = 0; i < (U32)mMessageLines.size(); i++ )
   {
      len = ( dStrlen( mMessageLines[i].message ) * 2 ) + 10;
      lineBuf = (char*) dRealloc( lineBuf, len );
      dSprintf( lineBuf, len, "%d ", mMessageLines[i].messageTag );
      expandEscape( lineBuf + dStrlen( lineBuf ), mMessageLines[i].message );
      file.writeLine( (const U8*) lineBuf );
   }

   file.close();
   return( true );
}


//--------------------------------------------------------------------------
void MessageVector::registerSpectator(SpectatorCallback callBack, void *spectatorKey)
{
   AssertFatal(callBack != NULL, "Error, must have a callback!");

   // First, make sure that this hasn't been registered already...
   U32 i;
   for (i = 0; i < (U32)mSpectators.size(); i++) {
      AssertFatal(mSpectators[i].key != spectatorKey, "Error, spectator key already registered!");
      if (mSpectators[i].key == spectatorKey)
         return;
   }

   mSpectators.increment();
   mSpectators.last().callback = callBack;
   mSpectators.last().key      = spectatorKey;

   // Need to message this spectator of all the lines currently inserted...
   for (i = 0; i < (U32)mMessageLines.size(); i++) {
      (*mSpectators.last().callback)(mSpectators.last().key,
                                     LineInserted, i);
   }
}

void MessageVector::unregisterSpectator(void * spectatorKey)
{
   for (U32 i = 0; i < (U32)mSpectators.size(); i++) {
      if (mSpectators[i].key == spectatorKey) {
         // Need to message this spectator of all the lines currently inserted...
         for (S32 j = mMessageLines.size() - 1; j >= 0 ; j--) {
            (*mSpectators[i].callback)(mSpectators[i].key,
                                       LineDeleted, j);
         }

         mSpectators.erase(i);
         return;
      }
   }

   AssertFatal(false, "MessageVector::unregisterSpectator: tried to unregister a spectator that isn't subscribed!");
   Con::errorf(ConsoleLogEntry::General,
               "MessageVector::unregisterSpectator: tried to unregister a spectator that isn't subscribed!");
}

void MessageVector::spectatorMessage(MessageCode code, const U32 arg)
{
   for (U32 i = 0; i < (U32)mSpectators.size(); i++) {
      (*mSpectators[i].callback)(mSpectators[i].key,
                                 code, arg);
   }
}

