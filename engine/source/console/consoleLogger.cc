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
#include "console/consoleLogger.h"
#include "console/consoleTypes.h"

#include "consoleLogger_ScriptBinding.h"

Vector<ConsoleLogger *> ConsoleLogger::mActiveLoggers;
bool ConsoleLogger::smInitialized = false;

IMPLEMENT_CONOBJECT( ConsoleLogger );

//-----------------------------------------------------------------------------

ConsoleLogger::ConsoleLogger()
{
   mFilename = NULL;
   mLogging = false;
   mAppend = false;

   mLevel = ConsoleLogEntry::Normal;
}

//-----------------------------------------------------------------------------

ConsoleLogger::ConsoleLogger( const char *fileName, bool append )
{
   mLogging = false;

   mLevel = ConsoleLogEntry::Normal;
   mFilename = StringTable->insert( fileName );
   mAppend = append;

   init();
}

//-----------------------------------------------------------------------------

static EnumTable::Enums logLevelEnums[] =
{
	{ ConsoleLogEntry::Normal,     "normal"  },
   { ConsoleLogEntry::Warning,    "warning" },
	{ ConsoleLogEntry::Error,      "error"   },
};

static EnumTable gLogLevelTable( 3, &logLevelEnums[0] );

void ConsoleLogger::initPersistFields()
{
   Parent::initPersistFields();

   addGroup( "Logging" );
   addField( "level",   TypeEnum,     Offset( mLevel,    ConsoleLogger ), 1, &gLogLevelTable );
   endGroup( "Logging" );
}

//-----------------------------------------------------------------------------

bool ConsoleLogger::processArguments( S32 argc, const char **argv )
{
   if( argc == 0 )
      return false;

   bool append = false;

   if( argc == 2 )
      append = dAtob( argv[1] );

   mAppend = append;
   mFilename = StringTable->insert( argv[0] );

   if( init() )
   {
      attach();
      return true;
   }

   return false;
}

//-----------------------------------------------------------------------------

ConsoleLogger::~ConsoleLogger()
{
   detach();
}

//-----------------------------------------------------------------------------

bool ConsoleLogger::init()
{
   if( smInitialized )
      return true;

   Con::addConsumer( ConsoleLogger::logCallback );
   smInitialized = true;

   return true;
}

//-----------------------------------------------------------------------------

bool ConsoleLogger::attach()
{
   if( mFilename == NULL )
   {
      Con::errorf( "ConsoleLogger failed to attach: no filename supplied." );
      return false;
   }

   // Check to see if this is initialized before using it
   if( !smInitialized )
   {
      if( !init() )
      {
         Con::errorf( "ConsoleLogger failed to initalize." );
         return false;
      }
   }

   if( mLogging )
      return false;

   // Open the filestream
   mStream.open( mFilename, ( mAppend ? FileStream::WriteAppend : FileStream::Write ) );

   // Add this to list of active loggers
   mActiveLoggers.push_back( this );
   mLogging = true;

   return true;
}

//-----------------------------------------------------------------------------

bool ConsoleLogger::detach()
{

   // Make sure this is valid before messing with it
   if( !smInitialized ) 
   {
      if( !init() ) 
      {
         return false;
      }
   }

   if( !mLogging )
      return false;

   // Close filestream
   mStream.close();

   // Remove this object from the list of active loggers
   for( int i = 0; i < mActiveLoggers.size(); i++ ) 
   {
      if( mActiveLoggers[i] == this ) 
      {
         mActiveLoggers.erase( i );
         mLogging = false;
         return true;
      }
   }

   return false; // If this happens, it's bad...
}

//-----------------------------------------------------------------------------

void ConsoleLogger::logCallback( ConsoleLogEntry::Level level, const char *consoleLine )
{

   ConsoleLogger *curr;

   // Loop through active consumers and send them the message
   for( int i = 0; i < mActiveLoggers.size(); i++ ) 
   {
      curr = mActiveLoggers[i];

      // If the log level is within the log threshhold, log it
      if( curr->mLevel <= level )
         curr->log( consoleLine );
   }
}

//-----------------------------------------------------------------------------

void ConsoleLogger::log( const char *consoleLine )
{
   // Check to see if this is intalized before using it
   if( !smInitialized ) 
   {
      if( !init() ) 
      {
         Con::errorf( "I don't know how this happened, but log called on this without it being initialized" );
         return;
      }
   }

   mStream.writeLine( (U8 *)consoleLine );
}

//-----------------------------------------------------------------------------
