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

#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "io/bitStream.h"
#include "Trigger.h"

// Script bindings.
#include "Trigger_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(Trigger);

//-----------------------------------------------------------------------------

Trigger::Trigger()
{
    // Setup some debug vector associations.
    VECTOR_SET_ASSOCIATION(mEnterColliders);
    VECTOR_SET_ASSOCIATION(mLeaveColliders);

    // Set default callbacks.
    mEnterCallback = true;
    mStayCallback = false;
    mLeaveCallback = true;

    // Use a static body by default.
    mBodyDefinition.type = b2_staticBody;

    // Gather contacts.
    mGatherContacts = true;
}

//-----------------------------------------------------------------------------

bool Trigger::onAdd()
{
    // Eventually, we'll need to deal with Server/Client functionality!

    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void Trigger::initPersistFields()
{
   addProtectedField("EnterCallback", TypeBool, Offset(mEnterCallback, Trigger), &setEnterCallback, &defaultProtectedGetFn, &writeEnterCallback,"");
   addProtectedField("StayCallback", TypeBool, Offset(mStayCallback, Trigger), &setStayCallback, &defaultProtectedGetFn, &writeStayCallback, "");
   addProtectedField("LeaveCallback", TypeBool, Offset(mLeaveCallback, Trigger), &setLeaveCallback, &defaultProtectedGetFn, &writeLeaveCallback, "");

   Parent::initPersistFields();
}

//-----------------------------------------------------------------------------

void Trigger::preIntegrate(const F32 totalTime, const F32 elapsedTime, DebugStats *pDebugStats)
{
    // Call Parent.
    Parent::preIntegrate(totalTime, elapsedTime, pDebugStats);

    // Clear Collider Callback Lists.
    mEnterColliders.clear();
    mLeaveColliders.clear();
}

//-----------------------------------------------------------------------------

void Trigger::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats *pDebugStats )
{
    // Call Parent.
    Parent::integrateObject(totalTime, elapsedTime, pDebugStats);

    // Debug Profiling.
    PROFILE_SCOPE(Trigger_IntegrateObject);

    // Perform "OnEnter" callback.
    if ( mEnterCallback && mEnterColliders.size() > 0 )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Trigger_OnEnterCallback);

        for ( collideCallbackType::iterator contactItr = mEnterColliders.begin(); contactItr != mEnterColliders.end(); ++contactItr )
        {
            Con::executef(this, 2, "onEnter", (*contactItr)->getIdString());
        }
    }

    // Fetch current contacts.
    const Scene::typeContactVector* pCurrentContacts = getCurrentContacts();

    // Sanity!
    AssertFatal( pCurrentContacts != NULL, "Trigger::integrateObject() - Contacts not initialized correctly." );

    // Perform "OnStay" callback.
    if ( mStayCallback && pCurrentContacts->size() > 0 )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Trigger_OnStayCallback);

        for ( Scene::typeContactVector::const_iterator contactItr = pCurrentContacts->begin(); contactItr != pCurrentContacts->end(); ++contactItr )
        {
            // Fetch colliding object.
            SceneObject* pCollideWidth = contactItr->getCollideWith( this );

            Con::executef(this, 2, "onStay", pCollideWidth->getIdString());
        }
    }

    // Perform "OnLeave" callback.
    if ( mLeaveCallback && mLeaveColliders.size() > 0 )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Trigger_OnLeaveCallback);

        for ( collideCallbackType::iterator contactItr = mLeaveColliders.begin(); contactItr != mLeaveColliders.end(); ++contactItr )
        {
            Con::executef(this, 2, "onLeave", (*contactItr)->getIdString());
        }
    }
}

//-----------------------------------------------------------------------------

void Trigger::onBeginCollision( const TickContact& tickContact )
{
    // Call parent.
    Parent::onBeginCollision( tickContact );

    // Add to enter colliders.
    mEnterColliders.push_back( tickContact.getCollideWith( this ) );
}

//-----------------------------------------------------------------------------

void Trigger::onEndCollision( const TickContact& tickContact )
{
    // Call parent.
    Parent::onEndCollision( tickContact );

    // Add to leave colliders.
    mLeaveColliders.push_back( tickContact.getCollideWith( this ) );
}

//-----------------------------------------------------------------------------

void Trigger::copyTo(SimObject* object)
{
   Parent::copyTo(object);

   AssertFatal(dynamic_cast<Trigger*>(object), "Trigger::copyTo() - Object is not the correct type.");
   Trigger* trigger = static_cast<Trigger*>(object);

   trigger->mEnterCallback = mEnterCallback;
   trigger->mStayCallback = mStayCallback;
   trigger->mLeaveCallback = mLeaveCallback;
}
