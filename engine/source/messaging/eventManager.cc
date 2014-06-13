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

#include "messaging/eventManager.h"
#include "console/consoleTypes.h"
#include "console/consoleInternal.h"

#include "eventManager_ScriptBinding.h"

IMPLEMENT_CONOBJECT( EventManager );

Vector<EventManager*> EventManager::smEventManagers;

//-----------------------------------------------------------------------------
/// Gets a list of all listeners of a specific event type and executes a
/// callback on each one.
/// 
/// @param event The name of the event that was triggered.
/// @param data The data associated with the event.
/// @return true to allow other listeners to receive the event, false otherwise
//-----------------------------------------------------------------------------

// CodeReview [tom, 2/20/2007] There seemed to be a little confusion on the return value here.
// It is not a "successfully dispatched" value, it is used to prevent other
// listeners from receiving the message. Using the event manager this probably
// didn't matter since there was only one listener, however it would cause
// problems if more then one listener is registered with the queue.
bool EventManagerListener::onMessageReceived( StringTableEntry queue, const char* event, const char* data )
{
   Vector<Subscriber>* subscribers = mSubscribers.retrieve( event );
   if( subscribers == NULL )
      return true;

   for( Vector<Subscriber>::const_iterator iter = subscribers->begin(); iter != subscribers->end(); iter++ )
   {
      // If we returned a string that is not "", try to convert it to true/false
      const char* conResult = Con::executef( iter->listener, 2, iter->callback, data );
      if( dStrcmp( conResult, "" ) != 0 && dAtob( conResult ) == false )
         return false;
   }
   return true;
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
EventManager::EventManager() : mQueue( NULL )
{
   addEventManager( this );
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
EventManager::~EventManager()
{
   setMessageQueue( "" );
   unregisterAllEvents();
   removeEventManager( this );
}

//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void EventManager::initPersistFields()
{
   addProtectedField( "queue", TypeString, Offset( mQueue, EventManager ), &setMessageQueue, &defaultProtectedGetFn, "" );
}

//-----------------------------------------------------------------------------
/// Registers the message queue and listener with the messaging system.
/// 
/// @param queue The name of the queue. Set to "" to destroy the queue.
//-----------------------------------------------------------------------------
void EventManager::setMessageQueue( const char* queue )
{
   // If a queue is already registered, unregister it.
   if( mQueue && Dispatcher::isQueueRegistered( mQueue ) )
   {
      unregisterAllEvents();
      Dispatcher::unregisterMessageListener( mQueue, &mListener );
      Dispatcher::unregisterMessageQueue( mQueue );
   }

   // Register the new queue.
   if( queue && *queue )
   {
      Dispatcher::registerMessageQueue( queue );
      Dispatcher::registerMessageListener( queue, &mListener );
      mQueue = StringTable->insert( queue );
   }
}

//-----------------------------------------------------------------------------
/// Determines whether or not an event is registered with the EventManager.
/// 
/// @param event the event to check.
//-----------------------------------------------------------------------------
bool EventManager::isRegisteredEvent( const char* event )
{
   // Iterate over the event list.
   StringTableEntry eventName = StringTable->insert( event );
   for( Vector<StringTableEntry>::const_iterator iter = mEvents.begin(); iter != mEvents.end(); iter++ )
   {
      // Found.
      if( *iter == eventName )
         return true;
   }

   // Not found.
   return false;
}

//-----------------------------------------------------------------------------
/// Register an event with the EventManager.
/// 
/// @param event The event to register.
/// @return Whether or not the event was successfully registered.
//-----------------------------------------------------------------------------
bool EventManager::registerEvent( const char* event )
{
   // Make sure the event has not been registered yet.
   if( isRegisteredEvent( event ) )
   {
      Con::warnf( "EventManager::registerEvent - event %s already registered", event );
      return false;
   }

   // Add to the event list.
   mEvents.push_back( StringTable->insert( event ) );

   // Create a list of subscribers for this event.
   mListener.mSubscribers.insert( new Vector<EventManagerListener::Subscriber>, event );

   return true;
}

//-----------------------------------------------------------------------------
/// Removes all events from the EventManager.
//-----------------------------------------------------------------------------
void EventManager::unregisterAllEvents()
{
   // Iterate over all events.
   for( Vector<StringTableEntry>::const_iterator iter = mEvents.begin(); iter != mEvents.end(); iter++ )
   {
      // Delete the subscriber list.
      Vector<EventManagerListener::Subscriber>* subscribers = mListener.mSubscribers.remove( *iter );
      if( subscribers )
         delete subscribers;
   }

   // Clear the event list.
   mEvents.clear();
}

//-----------------------------------------------------------------------------
/// Removes an event from the EventManager.
/// 
/// @param event The event to remove.
//-----------------------------------------------------------------------------
void EventManager::unregisterEvent( const char* event )
{
   // If the event doesn't exist, we have succeeded in removing it!
   if( !isRegisteredEvent( event ) )
      return;

   // Iterate over all events.
   StringTableEntry eventName = StringTable->insert( event );
   for( Vector<StringTableEntry>::iterator iter = mEvents.begin(); iter != mEvents.end(); iter++ )
   {
      // Erase the event.
      if( *iter == eventName )
      {
         mEvents.erase_fast( iter );
         break;
      }
   }

   // Delete the subscriber list.
   Vector<EventManagerListener::Subscriber>* subscribers = mListener.mSubscribers.remove( event );
   if( subscribers )
      delete subscribers;
}

//-----------------------------------------------------------------------------
/// Post an event to the EventManager's queue.
/// 
/// @param event The event to post.
/// @param data Various data associated with the event.
/// @return Whether or not the message was dispatched successfully.
//-----------------------------------------------------------------------------
bool EventManager::postEvent( const char* event, const char* data )
{
   return Dispatcher::dispatchMessage( mQueue, event, data );
}

//-----------------------------------------------------------------------------
/// Subscribe a listener to an event.
/// 
/// @param listener The listener to subscribe.
/// @param event The event to subscribe to.
/// @param callback Optional callback name to be called when the event is
/// triggered.
/// @return Whether or not the subscription was successful.
//-----------------------------------------------------------------------------

// CodeReview [tom, 2/20/2007] The "listener" argument was an IMessageListener,
// but it was actually used as a SimObject and never a listener. Thus, it is now a SimObject.
bool EventManager::subscribe(SimObject *callbackObj, const char* event, const char* callback /*= NULL */)
{
   // Make sure the event is valid.
   if( !isRegisteredEvent( event ) )
   {
      Con::warnf( "EventManager::subscribe - %s is not a registered event.", event );
      return false;
   }

   // Grab the callback name.
   char* cb = NULL;
   if( !callback || !*callback )
   {
      // Not specified, use default ( "onEvent" ).
      S32 length = dStrlen( event ) + 5;
      cb = new char[length];
      dSprintf( cb, length, "on%s", event );
   }
   else
   {
      cb = new char[dStrlen(callback) + 1];
      dStrcpy(cb, callback);
   }

   // Create the subscriber object.
   EventManagerListener::Subscriber subscriber;
   subscriber.listener = callbackObj;
   subscriber.event = StringTable->insert( event );
   subscriber.callback = StringTable->insert( cb );

   delete [] cb;

   // Grab the subscriber list.
   Vector<EventManagerListener::Subscriber>* subscribers = mListener.mSubscribers.retrieve( event );

   // If the event exists, there should always be a valid subscriber list.
   AssertFatal( subscribers, "Invalid event subscriber list." );

   // Add the subscriber.
   subscribers->push_back( subscriber );

   return true;
}


//-----------------------------------------------------------------------------
/// remove a listener from an event.
/// 
/// @param listener The listener to remove from an event callback list.
/// @param event The event to remove the listener from.
//-----------------------------------------------------------------------------

// CodeReview [tom, 2/20/2007] The "listener" argument was an IMessageListener,
// but it was actually used as a SimObject and never a listener. Thus, it is now a SimObject.
void EventManager::remove(SimObject *cbObj, const char* event)
{
   // If the event doesn't exist, we have succeeded in removing it!
   if( !isRegisteredEvent( event ) )
      return;

   Vector<EventManagerListener::Subscriber>* subscribers = mListener.mSubscribers.retrieve( event );
   if( !subscribers )
      return;

   for( Vector<EventManagerListener::Subscriber>::iterator iter = subscribers->begin(); iter != subscribers->end(); iter++ )
   {
      // Erase the event.
      if( iter->listener == cbObj )
      {
         subscribers->erase_fast( iter );
         break;
      }
   }
}
//-----------------------------------------------------------------------------
/// Print all registered events to the console.
//-----------------------------------------------------------------------------
void EventManager::dumpEvents()
{
   Con::printf( "%s Events", mQueue );
   for( Vector<StringTableEntry>::const_iterator iter = mEvents.begin(); iter != mEvents.end(); iter++ )
      Con::printf( "   %s", *iter );
}

//-----------------------------------------------------------------------------
/// Print the subscribers to an event.
/// 
/// @param event The event whose subscribers are to be printed.
//-----------------------------------------------------------------------------
void EventManager::dumpSubscribers( const char* event )
{
   Vector<EventManagerListener::Subscriber>* subscribers = mListener.mSubscribers.retrieve( event );
   if( !subscribers )
   {
      Con::warnf( "EventManager::dumpSubscriber - %s is not a valid event.", event );
      return;
   }

   Con::printf( "%s Subscribers", event );
   for( Vector<EventManagerListener::Subscriber>::const_iterator iter = subscribers->begin(); iter != subscribers->end(); iter++ )
   {
      // Grab the best fit name. This should be the first found of name, class, superclass, or class type.
      Namespace* ns = iter->listener->getNamespace();
      const char* name = ns ? ns->mName : getClassName() ;
      Con::printf( "   %s -> %s", name, iter->callback );
   }
}

//-----------------------------------------------------------------------------
/// Print all registered events and their subscribers to the console.
//-----------------------------------------------------------------------------
void EventManager::dumpSubscribers()
{
   Con::printf( "%s Events", mQueue );
   for( Vector<StringTableEntry>::const_iterator iter = mEvents.begin(); iter != mEvents.end(); iter++ )
      dumpSubscribers( *iter );
}
