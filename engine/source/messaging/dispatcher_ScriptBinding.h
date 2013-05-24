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

/*! @defgroup MessageQueueFunctions Message Queue (Dispatcher)
	@ingroup TorqueScriptFunctions
	@{
*/

using namespace Dispatcher;

/*! Checks whether message queue is registered
    @param queueName The name of the queue to check
    @return Returns true if registered and false if not
*/
ConsoleFunctionWithDocs(isQueueRegistered, ConsoleBool, 2, 2, (queueName))
{
   return isQueueRegistered(argv[1]);
}

/*! Registers given message queue
    @param queueName The name of the message queue
    @return No Return Value
*/
ConsoleFunctionWithDocs(registerMessageQueue, ConsoleVoid, 2, 2, (queueName))
{
   return registerMessageQueue(argv[1]);
}

/*! Unregisters given message queue
    @param The name of the message queue
    @return No Return Value
*/
ConsoleFunctionWithDocs(unregisterMessageQueue, ConsoleVoid, 2, 2, (queueName))
{
   return unregisterMessageQueue(argv[1]);
}

//////////////////////////////////////////////////////////////////////////

/*! Registers a message listener on a message queue
    @param queueName The name of the message queue
    @param listener The name of the listener to register
    @return Returns true on success, and false otherwise (probably not found)
*/
ConsoleFunctionWithDocs(registerMessageListener, ConsoleBool, 3, 3, (queueName, listener))
{
   IMessageListener *listener = dynamic_cast<IMessageListener *>(Sim::findObject(argv[2]));
   if(listener == NULL)
   {
      Con::errorf("registerMessageListener - Unable to find listener object, not an IMessageListener ?!");
      return false;
   }

   return registerMessageListener(argv[1], listener);
}

/*! Unregisters the message listener on given message queue
    @param queueName The name of the message queue
    @param listener The name of the listener to unregister
    @return No Return Value
*/
ConsoleFunctionWithDocs(unregisterMessageListener, ConsoleVoid, 3, 3, (queueName, listener))
{
   IMessageListener *listener = dynamic_cast<IMessageListener *>(Sim::findObject(argv[2]));
   if(listener == NULL)
   {
      Con::errorf("unregisterMessageListener - Unable to find listener object, not an IMessageListener ?!");
      return;
   }

   unregisterMessageListener(argv[1], listener);
}

//////////////////////////////////////////////////////////////////////////

/*! Dispatches a message to given message queue
    @param queueName The queue to dispatch to
    @param event The message you are passing
    @param data Data
    @return Returns true on success and false otherwise
*/
ConsoleFunctionWithDocs(dispatchMessage, ConsoleBool, 3, 4, (queueName, event, data))
{
   return dispatchMessage(argv[1], argv[2], argc > 3 ? argv[3] : "" );
}

/*! Dispatches a message object to the given queue
    @param queueName The name of the queue to dispatch object to
    @param message The message object
*/
ConsoleFunctionWithDocs(dispatchMessageObject, ConsoleBool, 3, 3, (queueName, message))
{
   Message *msg = dynamic_cast<Message *>(Sim::findObject(argv[2]));
   if(msg == NULL)
   {
      Con::errorf("dispatchMessageObject - Unable to find message object");
      return false;
   }

   return dispatchMessageObject(argv[1], msg);
}

/*! @} */ // group MessageQueueFunctions
