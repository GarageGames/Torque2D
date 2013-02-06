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
#include "messaging/dispatcher.h"
#include "collection/simpleHashTable.h"

#ifndef _EVENTMANAGER_H_
#define _EVENTMANAGER_H_

//-----------------------------------------------------------------------------
/// Listener class used by the EventManager to dispatch messages to specific
/// callbacks.
//-----------------------------------------------------------------------------
class EventManagerListener : public Dispatcher::IMessageListener
{
   friend class EventManager;

   /// Stores subscription information for a subscriber.
   struct Subscriber
   {
      SimObject* listener;       ///< The listener object.
      StringTableEntry callback; ///< The callback to execute when the event is triggered.
      StringTableEntry event;    ///< The event being listened for.
   };

   /// Subscriber table hashed by event name.
   SimpleHashTable< Vector<Subscriber> > mSubscribers;

public:
   /// Called by the EventManager queue when an event is triggered. Calls all listeners subscribed to the triggered event.
   virtual bool onMessageReceived( StringTableEntry queue, const char* event, const char* data );
   virtual bool onMessageObjectReceived( StringTableEntry queue, Message *msg ) { return true; };
};

/// @addtogroup msgsys Message System
// @{

//-----------------------------------------------------------------------------
/// The EventManager class is a wrapper for the standard messaging system. It
/// provides functionality for management of event queues, events, and
/// subscriptions.
/// 
/// Creating an EventManager is as simple as calling <tt>new EventManager</tt>
/// and specifying a queue name.
/// 
/// Example Usage:
/// 
/// @code
/// // Create the EventManager.
/// $MyEventManager = new EventManager() { queue = "MyEventManager"; };
/// 
/// // Create an event.
/// $MyEventManager.registerEvent( "SomeCoolEvent" );
/// 
/// // Create a listener and subscribe.
/// $MyListener = new ScriptMsgListener() { class = MyListener; };
/// $MyEventManager.subscribe( $MyListener, "SomeCoolEvent" );
/// 
/// function MyListener::onSomeCoolEvent( %this, %data )
/// {
///    echo( "onSomeCoolEvent Triggered" );
/// }
/// 
/// // Trigger the event.
/// $MyEventManager.postEvent( "SomeCoolEvent", "Data" );
/// @endcode
//-----------------------------------------------------------------------------
class EventManager : public SimObject
{
   typedef SimObject Parent;

private:
   /// The name of the message queue.
   StringTableEntry mQueue;
   /// Registered events.
   Vector<StringTableEntry> mEvents;

   /// The event listener. Listens for all events and dispatches them to the appropriate subscribers.
   EventManagerListener mListener;

   /// List of all EventManagers.
   static Vector<EventManager*> smEventManagers;

public:
   DECLARE_CONOBJECT( EventManager );

   EventManager();
   virtual ~EventManager();

   static void initPersistFields();

   /// @name Properties
   /// @{

   /// Sets the message queue.
   static bool setMessageQueue( void* obj, const char* data )
   {
      static_cast<EventManager*>( obj )->setMessageQueue( data );
      return false;
   };

   void setMessageQueue( const char* queue );

   /// @}

   /// @name Event Management
   /// @{

   /// Checks if an event is registered.
   bool isRegisteredEvent( const char* eventName );
   /// Registers an event.
   bool registerEvent( const char* eventName );
   /// Removes an event.
   void unregisterEvent( const char* eventName );
   /// Removes all events.
   void unregisterAllEvents();

   /// Triggers an event.
   bool postEvent( const char* eventName, const char* data );
   /// Adds a subscription to an event.
   bool subscribe( SimObject *callbackObj, const char* event, const char* callback = NULL );
   /// Remove a subscriber from an event.
   void remove( SimObject *cbObj, const char* event );

   /// @}

   /// @name Debug Output
   /// @{

   /// Prints all registered events to the console.
   void dumpEvents();
   /// Prints all subscribers to the console.
   void dumpSubscribers();
   /// Prints subscribers to a specific event to the console.
   void dumpSubscribers( const char* event );

   /// @}

   /// @name Event Manager Tracking
   /// @{

   /// Adds an EventManager.
   static void addEventManager( EventManager* em ) { smEventManagers.push_back( em ); };

   /// Removes an EventManager.
   static void removeEventManager( EventManager* em )
   {
      for( Vector<EventManager*>::iterator iter = smEventManagers.begin(); iter != smEventManagers.end(); iter++ )
      {
         if( *iter == em )
         {
            smEventManagers.erase_fast( iter );
            break;
         }
      }
   };

   /// Retrieves an EventManager.
   static EventManager* getEventManager( const char* name )
   {
      StringTableEntry queue = StringTable->insert( name );
      for( Vector<EventManager*>::iterator iter = smEventManagers.begin(); iter != smEventManagers.end(); iter++ )
      {
         if( ( *iter )->mQueue == queue )
            return *iter;
      }
      return NULL;
   };

   /// Prints all the EventManagers to the console.
   static void printEventManagers()
   {
      for( Vector<EventManager*>::iterator iter = smEventManagers.begin(); iter != smEventManagers.end(); iter++ )
         ( *iter )->dumpSubscribers();
   }
  

   /// @}
};

// @}

#endif // _EVENTMANAGER_H_
