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

#ifndef _BEHAVIORCOMPONENT_RAISEEVENT_H_
#include "behaviorComponentRaiseEvent.h"
#endif

//-----------------------------------------------------------------------------

/*! Copies a behaviors values to a component
*/
ConsoleFunctionWithDocs( copyBehaviorToComponent, ConsoleBool, 3, 3, (behavior, component))
{
    // Fetch behavior/component Ids.
    const S32 behaviorId  = dAtoi( argv[1] );
    const S32 componentId = dAtoi( argv[2] );

    // Fetch behavior.
    BehaviorInstance* pBehavior = dynamic_cast<BehaviorInstance*>( Sim::findObject( behaviorId ) );

    // Sanity!
    if ( !pBehavior )
    {
        Con::warnf( "copyBehaviorToComponent() - Could not find behavior '%d'.", behaviorId );
        return false;
    }

    // Fetch component.
    SimComponent* pComponent = dynamic_cast<SimComponent*>( Sim::findObject( componentId ) );

    // Sanity!
    if( !pComponent )
    {
        Con::errorf( "copyBehaviorToComponent() -  cannot find component '%d'.", componentId );
        return false;
    }

    // Fetch template.
    BehaviorTemplate* pTemplate = pBehavior->getTemplate();

    // Fetch template field count.
    const U32 fieldCount = pTemplate->getBehaviorFieldCount();
    
    const char* pFieldValue = NULL;
    BehaviorTemplate::BehaviorField* pField = NULL;

    // Copy behavior fields.
    for( U32 index = 0; index < fieldCount; ++index )
    {
        // Fetch behavior field.
        pField = pTemplate->getBehaviorField( index );

        // Fetch field value from behavior (if any).
        pFieldValue = pBehavior->getDataField( pField->mName, NULL );

        // Set field value on component.
        pComponent->setDataField( pField->mName, NULL, pFieldValue );
    }

    return true;
}

//-----------------------------------------------------------------------------

ConsoleMethodGroupBeginWithDocs(BehaviorComponent, DynamicConsoleMethodComponent)

/*! Add a behavior to the object
    @param bi The behavior instance to add
    @return (bool success) Whether or not the behavior was successfully added
*/
ConsoleMethodWithDocs( BehaviorComponent, addBehavior, ConsoleBool, 3, 3, (BehaviorInstance bi))
{
   return object->addBehavior( dynamic_cast<BehaviorInstance *>( Sim::findObject( argv[2] ) ) );
}

//-----------------------------------------------------------------------------

/*! 
    @param bi The behavior instance to remove
    @param deleteBehavior Whether or not to delete the behavior
    @return (bool success) Whether the behavior was successfully removed
*/
ConsoleMethodWithDocs( BehaviorComponent, removeBehavior, ConsoleBool, 3, 4, (BehaviorInstance bi, [bool deleteBehavior = true]))
{
   bool deleteBehavior = true;
   if (argc > 3)
      deleteBehavior = dAtob(argv[3]);

   return object->removeBehavior( dynamic_cast<BehaviorInstance *>( Sim::findObject( argv[2] ) ), deleteBehavior );
}

//-----------------------------------------------------------------------------

/*! Clear all behavior instances
    @return No return value
*/
ConsoleMethodWithDocs( BehaviorComponent, clearBehaviors, ConsoleVoid, 2, 2, ())
{
   object->clearBehaviors();
}

//-----------------------------------------------------------------------------

/*! Get the count of behaviors on an object
    @return (int count) The number of behaviors on an object
*/
ConsoleMethodWithDocs( BehaviorComponent, getBehaviorCount, ConsoleInt, 2, 2, ())
{
   return object->getBehaviorCount();
}

//-----------------------------------------------------------------------------

/*! gets a behavior
    @param BehaviorTemplateName The name of the template of the behavior instance you want
    @return (BehaviorInstance bi) The behavior instance you requested
*/
ConsoleMethodWithDocs( BehaviorComponent, getBehavior, ConsoleInt, 3, 3, (string BehaviorTemplateName))
{
   BehaviorInstance* pBehaviorInstance = object->getBehavior( StringTable->insert( argv[2] ) );

   return pBehaviorInstance ? pBehaviorInstance->getId() : 0;
}

//-----------------------------------------------------------------------------

/*! Gets a particular behavior
    @param index The index of the behavior to get
    @return (BehaviorInstance bi) The behavior instance you requested
*/
ConsoleMethodWithDocs( BehaviorComponent, getBehaviorByIndex, ConsoleInt, 3, 3, (int index))
{
   BehaviorInstance *bInstance = object->getBehavior( dAtoi(argv[2]) );

   return ( bInstance != NULL ) ? bInstance->getId() : 0;
}

//-----------------------------------------------------------------------------

/*! 
    @param inst The behavior instance you want to reorder
    @param desiredIndex The index you want the behavior instance to be reordered to
    @return (bool success) Whether or not the behavior instance was successfully reordered
*/
ConsoleMethodWithDocs( BehaviorComponent, reOrder, ConsoleBool, 3, 3, (BehaviorInstance inst, [int desiredIndex = 0]))
{
   BehaviorInstance *inst = dynamic_cast<BehaviorInstance *>( Sim::findObject( argv[1] ) );

   if( inst == NULL )
      return false;

   U32 idx = 0;
   if( argc > 2 )
      idx = dAtoi( argv[2] );

   return object->reOrder( inst, idx );
}

//-----------------------------------------------------------------------------

/*! Connects a behavior output to a behavior input.
    @param outputBehavior The behavior that owns the output.
    @param inputBehavior The behavior that owns the input.
    @param outputName The output name owned by the output behavior.
    @param inputName The input name owned by the input behavior.
    @return (bool success) Whether the connection was successful or not.
*/
ConsoleMethodWithDocs( BehaviorComponent, connect, ConsoleBool, 6, 6, (outputBehavior, inputBehavior, outputName, inputName))
{
    // Find output behavior.
    BehaviorInstance* pOutputBehavior = dynamic_cast<BehaviorInstance*>( Sim::findObject( argv[2] ) );

    // Did we find the behavior?
    if ( !pOutputBehavior )
    {
        // No, so warn.
        Con::warnf("BehaviorComponent::connect() - Could not find output behavior '%s'.", argv[2] );
        return false;
    }

    // Find input behavior.
    BehaviorInstance* pInputBehavior = dynamic_cast<BehaviorInstance*>( Sim::findObject( argv[3] ) );

    // Did we find the behavior?
    if ( !pInputBehavior )
    {
        // No, so warn.
        Con::warnf("BehaviorComponent::connect() - Could not find input behavior '%s'.", argv[3] );
        return false;
    }

    // Fetch port names.
    StringTableEntry pOutputName = StringTable->insert( argv[4] );
    StringTableEntry pInputName = StringTable->insert( argv[5] );

    // Perform the connection.
    return object->connect( pOutputBehavior, pInputBehavior, pOutputName, pInputName );
}

//-----------------------------------------------------------------------------

/*! Connects a behavior output to a behavior input.
    @param outputBehavior The behavior that owns the output.
    @param inputBehavior The behavior that owns the input.
    @param outputName The output name owned by the output behavior.
    @param inputName The input name owned by the input behavior.
    @return (bool success) Whether the disconnection was successful or not.
*/
ConsoleMethodWithDocs( BehaviorComponent, disconnect, ConsoleBool, 6, 6, (outputBehavior, inputBehavior, outputName, inputName))
{
    // Find output behavior.
    BehaviorInstance* pOutputBehavior = dynamic_cast<BehaviorInstance*>( Sim::findObject( argv[2] ) );

    // Did we find the behavior?
    if ( !pOutputBehavior )
    {
        // No, so warn.
        Con::warnf("BehaviorComponent::disconnect() - Could not find output behavior '%s'.", argv[2] );
        return false;
    }

    // Find input behavior.
    BehaviorInstance* pInputBehavior = dynamic_cast<BehaviorInstance*>( Sim::findObject( argv[3] ) );

    // Did we find the behavior?
    if ( !pInputBehavior )
    {
        // No, so warn.
        Con::warnf("BehaviorComponent::disconnect() - Could not find input behavior '%s'.", argv[3] );
        return false;
    }

    // Fetch port names.
    StringTableEntry pOutputName = StringTable->insert( argv[4] );
    StringTableEntry pInputName = StringTable->insert( argv[5] );

    // Perform the disconnection.
    return object->disconnect( pOutputBehavior, pInputBehavior, pOutputName, pInputName );
}

//-----------------------------------------------------------------------------

/*! Raise a signal on the behavior output on the specified behavior.
    @param outputBehavior The behavior that owns the output.
    @param outputName The output name owned by the output behavior.
    @param [deltaTime] Optional time-delta (ms) when the raise should occur.
    @return (bool success) Whether the signal raise was successful or not.
*/
ConsoleMethodWithDocs( BehaviorComponent, raise, ConsoleBool, 4, 5, (outputBehavior, outputName, [deltaTime]))
{
    // Find output behavior.
    BehaviorInstance* pOutputBehavior = dynamic_cast<BehaviorInstance*>( Sim::findObject( argv[2] ) );

    // Did we find the behavior?
    if ( !pOutputBehavior )
    {
        // No, so warn.
        Con::warnf("BehaviorComponent::raise() - Could not find output behavior '%s'.", argv[2] );
        return false;
    }

    // Fetch output name.
    StringTableEntry pOutputName = StringTable->insert( argv[3] );

    // Perform the signal raising immediately if no schedule time specified.
    if ( argc < 5 )
        return object->raise( pOutputBehavior, pOutputName );

    // Fetch time delta.
    const U32 timeDelta = U32( dAtoi(argv[4]) );

    // Schedule raise event.
    BehaviorComponentRaiseEvent* pEvent = new BehaviorComponentRaiseEvent( pOutputBehavior, pOutputName );
    Sim::postEvent( object, pEvent, Sim::getCurrentTime() + timeDelta );

    return true;
}

//-----------------------------------------------------------------------------

/*! Gets the number of connections on the behavior output on the specified behavior.
    @param outputBehavior The behavior that owns the output.
    @param outputName The output name owned by the output behavior.
    @return The number of connections on the behavior output on the specified behavior.
*/
ConsoleMethodWithDocs( BehaviorComponent, getBehaviorConnectionCount, ConsoleInt, 4, 4, (outputBehavior, outputName))
{
    // Find output behavior.
    BehaviorInstance* pOutputBehavior = dynamic_cast<BehaviorInstance*>( Sim::findObject( argv[2] ) );

    // Did we find the behavior?
    if ( !pOutputBehavior )
    {
        // No, so warn.
        Con::warnf("BehaviorComponent::getBehaviorConnectionCount() - Could not find output behavior '%s'.", argv[2] );
        return false;
    }

    // Fetch output name.
    StringTableEntry pOutputName = StringTable->insert( argv[3] );

    // Return the connection count.
    return object->getBehaviorConnectionCount( pOutputBehavior, pOutputName );
}

//-----------------------------------------------------------------------------

/*! Gets a comma-delimited list of connections on the behavior output on the specified behavior.
    @param outputBehavior The behavior that owns the output.
    @param outputName The output name owned by the output behavior.
    @param connectionIndex The connection index.
    @return Returns a comma-delimited list of connections on the behavior output on the specified behavior of the format <OutputBehavior>,<InputBehavior>,<OutputName>,<InputName>.
*/
ConsoleMethodWithDocs( BehaviorComponent, getBehaviorConnection, ConsoleString, 5, 5, (outputBehavior, outputName, connectionIndex))
{
    // Find output behavior.
    BehaviorInstance* pOutputBehavior = dynamic_cast<BehaviorInstance*>( Sim::findObject( argv[2] ) );

    // Did we find the behavior?
    if ( !pOutputBehavior )
    {
        // No, so warn.
        Con::warnf("BehaviorComponent::getBehaviorConnections() - Could not find output behavior '%s'.", argv[2] );
        return NULL;
    }

    // Fetch output name.
    StringTableEntry pOutputName = StringTable->insert( argv[3] );

    // Fetch connection index.
    const U32 connectionIndex = dAtoi( argv[4] );

    // Fetch connection.
    const BehaviorComponent::BehaviorPortConnection* pBehaviorConnection = object->getBehaviorConnection( pOutputBehavior, pOutputName, connectionIndex );

    // Finish if there are on connections.
    if ( pBehaviorConnection == NULL )
        return StringTable->EmptyString;

    // Format and return behavior input.
    char* pBuffer = Con::getReturnBuffer(1024);
    dSprintf(pBuffer, 1024, "%d,%d,%s,%s",
        pBehaviorConnection->mOutputInstance->getId(),
        pBehaviorConnection->mInputInstance->getId(),
        pBehaviorConnection->mOutputName,
        pBehaviorConnection->mInputName );
    return pBuffer;
}

ConsoleMethodGroupEndWithDocs(BehaviorComponent)
