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

#include "platform/platform.h"
#include "sim/simBase.h"
#include "console/consoleTypes.h"
#include "component/simComponent.h"
#include "component/behaviors/behaviorTemplate.h"
#include "memory/safeDelete.h"
#include "io/resource/resourceManager.h"

// Script bindings.
#include "behaviorTemplate_ScriptBinding.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(BehaviorTemplate);

//-----------------------------------------------------------------------------

BehaviorTemplate::BehaviorTemplate() :
    mFriendlyName( StringTable->EmptyString ),
    mDescription( StringTable->EmptyString ),
    mBehaviorType( StringTable->EmptyString )
{
}

//-----------------------------------------------------------------------------

bool BehaviorTemplate::onAdd()
{
    if(! Parent::onAdd())
        return false;

    Sim::gBehaviorSet->addObject(this);

    return true;
}

//-----------------------------------------------------------------------------

void BehaviorTemplate::onRemove()
{
    Sim::gBehaviorSet->removeObject(this);

    Parent::onRemove();
}

//-----------------------------------------------------------------------------

void BehaviorTemplate::initPersistFields()
{
    addGroup("Behavior");
        addField("friendlyName", TypeCaseString, Offset(mFriendlyName, BehaviorTemplate), "Human friendly name of this behavior");
        addProtectedField("description", TypeCaseString, Offset(mDescription, BehaviorTemplate), &setDescription, &getDescription, "The description of this behavior.\n");
        addField("behaviorType", TypeString, Offset(mBehaviorType, BehaviorTemplate), "?? Organizational keyword ??");
    endGroup("Behavior");

    Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

BehaviorInstance* BehaviorTemplate::createInstance( void )
{
    // Create behavior instance.
    BehaviorInstance* pBehavior = new BehaviorInstance( this );

    // Register object.
    if( pBehavior->registerObject() )
        return pBehavior;

    // Registration failed so delete behavior.
    delete pBehavior;
    return NULL;
}

//-----------------------------------------------------------------------------

bool BehaviorTemplate::addBehaviorField( const char* name, const char* description, const char* type, const char* defaultValue, const char* userData )
{
    // Does the behavior already have the field?
    if ( hasBehaviorField( name ) )
    {
        // Yes, so warn.
        Con::warnf("Behavior field named '%s' on template '%s' already exists.", name, mFriendlyName );
        return false;
    }

    // Create field.
    BehaviorField field( name, description, type, defaultValue, userData );

    // Use field.
    mFields.push_back( field );

    return true;
}

//-----------------------------------------------------------------------------

bool BehaviorTemplate::addBehaviorOutput( const char* name, const char* label, const char* description )
{
    // Does the behavior already have the output?
    if ( hasBehaviorOutput( name ) )
    {
        // Yes, so warn.
        Con::warnf("Behavior output named '%s' on template '%s' already exists.", name, mFriendlyName );
        return false;
    }

    // Create output.
    BehaviorPortOutput output( name, label, description );

    // Use port.
    mPortOutputs.push_back( output );

    return true;
}

//-----------------------------------------------------------------------------

bool BehaviorTemplate::addBehaviorInput( const char* name, const char* label, const char* description )
{
    // Does the behavior already have the input?
    if ( hasBehaviorInput( name ) )
    {
        // Yes, so warn.
        Con::warnf("Behavior input named '%s' on template '%s' already exists.", name, mFriendlyName );
        return false;
    }

    // Create input.
    BehaviorPortInput input( name, label, description );

    // Use port.
    mPortInputs.push_back( input );

    return true;
}
