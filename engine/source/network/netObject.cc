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
#include "network/connectionProtocol.h"
#include "network/netConnection.h"
#include "network/netObject.h"
#include "console/consoleTypes.h"

#include "netObject_ScriptBinding.h"

IMPLEMENT_CONOBJECT(NetObject);

//----------------------------------------------------------------------------
NetObject *NetObject::mDirtyList = NULL;

NetObject::NetObject()
{
    // netFlags will clear itself to 0
    mNetIndex = U32(-1);
   mFirstObjectRef = NULL;
   mPrevDirtyList = NULL;
   mNextDirtyList = NULL;
   mDirtyMaskBits = 0;
}

NetObject::~NetObject()
{
   if(mDirtyMaskBits)
   {
      if(mPrevDirtyList)
         mPrevDirtyList->mNextDirtyList = mNextDirtyList;
      else
         mDirtyList = mNextDirtyList;
      if(mNextDirtyList)
         mNextDirtyList->mPrevDirtyList = mPrevDirtyList;
   }
}

void NetObject::setMaskBits(U32 orMask)
{
   AssertFatal(orMask != 0, "Invalid net mask bits set.");
   AssertFatal(mDirtyMaskBits == 0 || (mPrevDirtyList != NULL || mNextDirtyList != NULL || mDirtyList == this), "Invalid dirty list state.");
   if(!mDirtyMaskBits)
   {
      AssertFatal(mNextDirtyList == NULL && mPrevDirtyList == NULL, "Object with zero mask already in list.");
      if(mDirtyList)
      {
         mNextDirtyList = mDirtyList;
         mDirtyList->mPrevDirtyList = this;
      }
      mDirtyList = this;
   }
   mDirtyMaskBits |= orMask;
   AssertFatal(mDirtyMaskBits == 0 || (mPrevDirtyList != NULL || mNextDirtyList != NULL || mDirtyList == this), "Invalid dirty list state.");
}

void NetObject::clearMaskBits(U32 orMask)
{
   if(isDeleted())
      return;
   if(mDirtyMaskBits)
   {
      mDirtyMaskBits &= ~orMask;
      if(!mDirtyMaskBits)
      {
         if(mPrevDirtyList)
            mPrevDirtyList->mNextDirtyList = mNextDirtyList;
         else
            mDirtyList = mNextDirtyList;
         if(mNextDirtyList)
            mNextDirtyList->mPrevDirtyList = mPrevDirtyList;
         mNextDirtyList = mPrevDirtyList = NULL;
      }
   }

   for(GhostInfo *walk = mFirstObjectRef; walk; walk = walk->nextObjectRef)
   {
      if(walk->updateMask && walk->updateMask == orMask)
      {
         walk->updateMask = 0;
         walk->connection->ghostPushToZero(walk);
      }
      else
         walk->updateMask &= ~orMask;
   }
}

void NetObject::collapseDirtyList()
{
   Vector<NetObject *> tempV;
   for(NetObject *t = mDirtyList; t; t = t->mNextDirtyList)
      tempV.push_back(t);

   for(NetObject *obj = mDirtyList; obj; )
   {
      NetObject *next = obj->mNextDirtyList;
      U32 orMask = obj->mDirtyMaskBits;

      obj->mNextDirtyList = NULL;
      obj->mPrevDirtyList = NULL;
      obj->mDirtyMaskBits = 0;

      if(!obj->isDeleted() && orMask)
      {
         for(GhostInfo *walk = obj->mFirstObjectRef; walk; walk = walk->nextObjectRef)
         {
            if(!walk->updateMask)
            {
               walk->updateMask = orMask;
               walk->connection->ghostPushNonZero(walk);
            }
            else
               walk->updateMask |= orMask;
         }
      }
      obj = next;
   }
   mDirtyList = NULL;
   for(U32 i = 0; i < (U32)tempV.size(); i++)
   {
      AssertFatal(tempV[i]->mNextDirtyList == NULL && tempV[i]->mPrevDirtyList == NULL && tempV[i]->mDirtyMaskBits == 0, "Error in collapse");
   }
}

//-----------------------------------------------------------------------------

void NetObject::setScopeAlways()
{
   if(mNetFlags.test(Ghostable) && !mNetFlags.test(IsGhost))
   {
      mNetFlags.set(ScopeAlways);

      // if it's a ghost always object, add it to the ghost always set
      // for ClientReps created later.

      Sim::getGhostAlwaysSet()->addObject(this);

      // add it to all Connections that already exist.

      SimGroup *clientGroup = Sim::getClientGroup();
      SimGroup::iterator i;
      for(i = clientGroup->begin(); i != clientGroup->end(); i++)
      {
         NetConnection *con = (NetConnection *) (*i);
         if(con->isGhosting())
            con->objectInScope(this);
      }
   }
}

void NetObject::clearScopeAlways()
{
   if(!mNetFlags.test(IsGhost))
   {
      mNetFlags.clear(ScopeAlways);
      Sim::getGhostAlwaysSet()->removeObject(this);

      // Un ghost this object from all the connections
      while(mFirstObjectRef)
         mFirstObjectRef->connection->detachObject(mFirstObjectRef);
   }
}

bool NetObject::onAdd()
{
   if(mNetFlags.test(ScopeAlways))
      setScopeAlways();

   return Parent::onAdd();
}

void NetObject::onRemove()
{
   while(mFirstObjectRef)
      mFirstObjectRef->connection->detachObject(mFirstObjectRef);

   Parent::onRemove();
}

//-----------------------------------------------------------------------------

F32 NetObject::getUpdatePriority(CameraScopeQuery*, U32, S32 updateSkips)
{
   return F32(updateSkips) * 0.1f;
}

U32 NetObject::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
   return 0;
}

void NetObject::unpackUpdate(NetConnection*, BitStream*)
{
}

void NetObject::onCameraScopeQuery(NetConnection *cr, CameraScopeQuery* /*camInfo*/)
{
   // default behavior -
   // ghost everything that is ghostable

   for (SimSetIterator obj(Sim::getRootGroup()); *obj; ++obj)
   {
        NetObject* nobj = dynamic_cast<NetObject*>(*obj);
        if (nobj)
        {
            AssertFatal(!nobj->mNetFlags.test(NetObject::Ghostable) || !nobj->mNetFlags.test(NetObject::IsGhost),
               "NetObject::onCameraScopeQuery: object marked both ghostable and as ghost");

            // Some objects don't ever want to be ghosted
            if (!nobj->mNetFlags.test(NetObject::Ghostable))
                continue;
         if (!nobj->mNetFlags.test(NetObject::ScopeAlways))
         {
            // it's in scope...
            cr->objectInScope(nobj);
         }
      }
   }
}

//-----------------------------------------------------------------------------

void NetObject::initPersistFields()
{
   Parent::initPersistFields();
}
