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

#ifndef _GAMEINTERFACE_H_
#define _GAMEINTERFACE_H_

#include "platform/event.h"
#include "collection/vector.h"

class FileStream;

class GameInterface
{
private:
   enum JournalMode {
      JournalOff,
      JournalSave,
      JournalPlay,
   };
   JournalMode mJournalMode;
   bool mRunning;
   bool mJournalBreak;
   bool mRequiresRestart;

   /// Events are stored here by any thread, for processing by the main thread.
   Vector<Event*> eventQueue1, eventQueue2, *eventQueue;
   
public:
   GameInterface();

   /// @name Game Loop
   /// @{
   virtual bool mainInitialize( int argc, const char **argv ) = 0;
   virtual void mainLoop( void ) = 0;
   virtual void mainShutdown( void ) = 0;
   /// @}

   virtual void gameDeactivate( const bool noRender ) = 0;
   virtual void gameReactivate( void ) = 0;

   /// @name Platform Interface
   /// The platform calls these functions to control execution of the game.
   /// @{
   virtual void textureKill( void ) = 0;
   virtual void textureResurrect( void ) = 0;
   virtual void refreshWindow( void ) = 0;

   /// Place an event in Game's event queue.
   virtual void postEvent(Event &event);

   /// Process all the events in Game's event queue. Only the main thread should call this.
   virtual void processEvents();
   /// @}

   /// @name Event Handlers
   /// default event behavior with journaling support
   /// default handler forwards events to appropriate routines
   /// @{
   virtual void processEvent(Event *event);

   virtual void processQuitEvent() = 0;
   virtual void processTimeEvent(TimeEvent *event) = 0;
   virtual void processInputEvent(InputEvent *event) = 0;
   virtual void processMouseMoveEvent(MouseMoveEvent *event) = 0;
   virtual void processScreenTouchEvent(ScreenTouchEvent *event) = 0;
   virtual void processConsoleEvent(ConsoleEvent *event) = 0;
   virtual void processPacketReceiveEvent(PacketReceiveEvent *event) = 0;
   virtual void processConnectedAcceptEvent(ConnectedAcceptEvent *event) = 0;
   virtual void processConnectedReceiveEvent(ConnectedReceiveEvent *event) = 0;
   virtual void processConnectedNotifyEvent(ConnectedNotifyEvent *event) = 0;
   /// @}

   /// @name Running
   /// @{
   inline void setRunning( const bool running ) { mRunning = running; }
   inline bool isRunning( void ) const { return mRunning; }
   inline void setRestart( const bool restart ) { mRequiresRestart = restart; }
   inline bool requiresRestart( void ) const { return mRequiresRestart; }
   /// @}

   /// @name Journaling
   ///
   /// Journaling is used in order to make a "demo" of the actual game.  It logs
   /// all processes that happen throughout code execution (NOT script).  This is
   /// very handy for debugging.  Say an end user finds a crash in the program.
   /// The user can start up the engine with journal recording enabled, reproduce
   /// the crash, then send in the journal file to the development team.  The
   /// development team can then play back the journal file and see exactly what
   /// happened to cause the crash.  This will result in the ability to run the
   /// program through the debugger to easily track what went wrong and easily
   /// create a stack trace.
   ///
   /// Actually enabling journaling may be different in different distributions
   /// if the developers decided to change how it works.  However, by default,
   /// run the program with the "-jSave filename" command argument.  The filename
   /// does not need an extension, and only requires write access.  If the file
   /// does not exist, it will be created.  In order to play back a journal,
   /// use the "-jPlay filename" command argument, and just watch the magic happen.
   /// Examples:
   /// @code
   /// torqueDemo_DEBUG.exe -jSave crash
   /// torqueDemo_DEBUG.exe -jPlay crash
   /// @endcode
   /// @{

   /// If we're doing a journal playback, this function is responsible for reading
   /// events from the journal file and dispatching them.
   void journalProcess();

   /// Start loading journal data from the specified file.
   void loadJournal(const char *fileName);

   /// Start saving journal data to the specified file (must be able to write it).
   void saveJournal(const char *fileName);

   /// Play back the specified journal.
   ///
   /// @param  fileName       Journal file to play back.
   /// @param  journalBreak   Should we break execution after we're done?
   void playJournal(const char *fileName, bool journalBreak = false);

   JournalMode getJournalMode() { return mJournalMode; };

   /// Are we reading back from the journal?
   inline bool isJournalReading( void ) const { return mJournalMode == JournalPlay; }

   /// Are we writing to the journal?
   inline bool isJournalWriting( void ) const { return mJournalMode == JournalSave; }

   void journalRead(U32 *val);                     ///< Read a U32 from the journal.
   void journalWrite(U32 val);                     ///< Write a U32 to the journal.
   void journalRead(U32 size, void *buffer);       ///< Read a block of data from the journal.
   void journalWrite(U32 size, const void *buffer);///< Write a block of data to the journal.

   FileStream *getJournalStream( void );
   /// @}
};

/// Global game instance.
extern GameInterface* Game;

#endif
