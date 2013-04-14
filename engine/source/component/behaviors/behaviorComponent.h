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

#ifndef _BEHAVIOR_COMPONENT_H_
#define _BEHAVIOR_COMPONENT_H_

#ifndef _DYNAMIC_CONSOLEMETHOD_COMPONENT_H_
#include "component/dynamicConsoleMethodComponent.h"
#endif

#ifndef _BEHAVIORINSTANCE_H_
#include "behaviorInstance.h"
#endif

//-----------------------------------------------------------------------------

class BehaviorComponent : public DynamicConsoleMethodComponent
{
    friend class BehaviorInterface;
    typedef DynamicConsoleMethodComponent Parent;

private:
    /// Component Behaviors
    SimSet  mBehaviors;

    /// Master behavior Id.
    U32 mMasterBehaviorId;

    Vector<StringTableEntry>* mpBehaviorFieldNames;


public:
    /// A behavior port connection.
    struct BehaviorPortConnection
    {
        BehaviorPortConnection(
            BehaviorInstance* pOutputBehavior,
            BehaviorInstance* pInputBehavior,
            StringTableEntry pOutputName,
            StringTableEntry pInputName )
        {
            mOutputInstance = pOutputBehavior;
            mInputInstance  = pInputBehavior;
            mOutputName     = pOutputName;
            mInputName      = pInputName;
        }

        BehaviorInstance*   mOutputInstance;
        BehaviorInstance*   mInputInstance;
        StringTableEntry    mOutputName;
        StringTableEntry    mInputName;
    };

    /// Behavior connection map.
    /// NOTE: This configuration provides more efficient raising of outputs as opposed to general administration.
    typedef Vector<BehaviorPortConnection> typePortConnectionVector;
    typedef HashMap<StringTableEntry, typePortConnectionVector*> typeOutputNameConnectionHash;
    typedef HashMap<SimObjectId, typeOutputNameConnectionHash*> typeInstanceConnectionHash;
    typeInstanceConnectionHash mBehaviorConnections;

protected:
    virtual const char* _callMethod( U32 argc, const char *argv[], bool callThis = true );

    /// Taml callbacks.
    virtual void onTamlCustomWrite( TamlCustomNodes& customNodes );
    virtual void onTamlCustomRead( const TamlCustomNodes& customNodes );

private:
    void destroyBehaviorOutputConnections( BehaviorInstance* pOutputBehavior );
    void destroyBehaviorInputConnections( BehaviorInstance* pInputBehavior );
    
  
public:
    BehaviorComponent();
    virtual ~BehaviorComponent() {}

    /// SimObject overrides
    virtual bool onAdd();
    virtual void onRemove();
    virtual void onDeleteNotify( SimObject *object );
    virtual void copyTo(SimObject* object);

    /// Behavior interface.
    BehaviorInstance* getBehaviorByInstanceId( const U32 behaviorId );
    virtual bool addBehavior( BehaviorInstance *bi);
    virtual bool removeBehavior( BehaviorInstance *bi, bool deleteBehavior = true );
    virtual void clearBehaviors();
    virtual U32 getBehaviorCount() const { return mBehaviors.size(); }
    virtual const SimSet &getBehaviors() const { return mBehaviors; }
    virtual BehaviorInstance *getBehavior( StringTableEntry behaviorTemplateName );
    virtual BehaviorInstance *getBehavior( const U32 index ) { return index < (U32)mBehaviors.size() ? reinterpret_cast<BehaviorInstance *>(mBehaviors[index]) : NULL; }
    virtual bool reOrder( BehaviorInstance *obj, U32 desiredIndex );

    /// Behavior connectivity.
    bool connect( BehaviorInstance* pOutputBehavior, BehaviorInstance* pInputBehavior, StringTableEntry pOutputName, StringTableEntry pInputName );
    bool disconnect( BehaviorInstance* pOutputBehavior, BehaviorInstance* pInputBehavior, StringTableEntry pOutputName, StringTableEntry pInputName );
    bool raise( BehaviorInstance* pOutputBehavior, StringTableEntry pOutputName );
    U32 getBehaviorConnectionCount( BehaviorInstance* pOutputBehavior, StringTableEntry pOutputName );
    const BehaviorPortConnection* getBehaviorConnection( BehaviorInstance* pOutputBehavior, StringTableEntry pOutputName, const U32 connectionIndex );
    const typePortConnectionVector* getBehaviorConnections( BehaviorInstance* pOutputBehavior, StringTableEntry pOutputName );

    /// DynamicConsoleMethodComponent Overrides
    virtual bool handlesConsoleMethod( const char *fname, S32 *routingId );
    virtual const char* callOnBehaviors( U32 argc, const char *argv[] );

    /// SimComponent overrides
    virtual void write( Stream &stream, U32 tabStop, U32 flags = 0 );

    DECLARE_CONOBJECT( BehaviorComponent );
};

#endif