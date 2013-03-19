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

#include "component/behaviors/behaviorInstance.h"
#include "component/behaviors/behaviorTemplate.h"
#include "console/consoleTypes.h"
#include "console/consoleInternal.h"
#include "io/stream.h"

// Script bindings.
#include "behaviorInstance_ScriptBinding.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(BehaviorInstance);

//-----------------------------------------------------------------------------

BehaviorInstance::BehaviorInstance( BehaviorTemplate* pTemplate ) :
    mTemplate( pTemplate ),
    mBehaviorOwner( NULL ),
    mBehaviorId( 0 )
{
    if ( pTemplate != NULL )
    {
        // Fetch field prototype count.
        const U32 fieldCount = pTemplate->getBehaviorFieldCount();

        // Set field prototypes.
        for( U32 index = 0; index < fieldCount; ++index )
        {        
            // Fetch fields.
            BehaviorTemplate::BehaviorField* pField = pTemplate->getBehaviorField( index );

            // Set cloned field.
            setDataField( pField->mName, NULL, pField->mDefaultValue );
        }
    }
}

//-----------------------------------------------------------------------------

bool BehaviorInstance::onAdd()
{
   if(! Parent::onAdd())
      return false;

   // Store this object's namespace
   mNameSpace = Namespace::global()->find( getTemplateName() );

   return true;
}

//-----------------------------------------------------------------------------

void BehaviorInstance::onRemove()
{
   Parent::onRemove();
}

//-----------------------------------------------------------------------------

void BehaviorInstance::initPersistFields()
{
   addGroup("Behavior");
      addField("template", TypeSimObjectName, Offset(mTemplate, BehaviorInstance), "Template this instance was created from.");
      addProtectedField( "Owner", TypeSimObjectPtr, Offset(mBehaviorOwner, BehaviorInstance), &setOwner, &defaultProtectedGetFn, "Behavior component owner." );
   endGroup("Behavior");

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

const char* BehaviorInstance::getTemplateName( void )
{
    return mTemplate ? mTemplate->getName() : NULL;
}

// Get template.
const char* BehaviorInstance::getTemplate(void* obj, const char* data)
{
    return static_cast<BehaviorInstance*>(obj)->getTemplate()->getIdString();
}
