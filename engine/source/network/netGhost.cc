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
#include "network/connectionProtocol.h"
#include "sim/simBase.h"
#include "network/netConnection.h"
#include "io/bitStream.h"
#include "network/netObject.h"
#include "io/resource/resourceManager.h"
#include "console/console.h"
#include "console/consoleTypes.h"

#define DebugChecksum 0xF00DBAAD

extern U32 gGhostUpdates;

class GhostAlwaysObjectEvent : public NetEvent
{
   SimObjectId objectId;
   U32 ghostIndex;
   NetObject *object;
   bool validObject;
public:
   GhostAlwaysObjectEvent(NetObject *obj = NULL, U32 index = 0)
   {
      if(obj)
      {
         objectId = obj->getId();
         ghostIndex = index;
      }
      object = NULL;
   }
   ~GhostAlwaysObjectEvent()
      { delete object; }

   void pack(NetConnection *ps, BitStream *bstream)
   {
      bstream->writeInt(ghostIndex, NetConnection::GhostIdBitSize);

      NetObject *obj = (NetObject *) Sim::findObject(objectId);
      if(bstream->writeFlag(obj != NULL))
      {
         S32 classId = obj->getClassId(ps->getNetClassGroup());
         bstream->writeClassId(classId, NetClassTypeObject, ps->getNetClassGroup());
         obj->packUpdate(ps, 0xFFFFFFFF, bstream);
      }
   }
   void write(NetConnection *ps, BitStream *bstream)
   {
      bstream->writeInt(ghostIndex, NetConnection::GhostIdBitSize);
      if(bstream->writeFlag(validObject))
      {
         S32 classId = object->getClassId(ps->getNetClassGroup());
         bstream->writeClassId(classId, NetClassTypeObject, ps->getNetClassGroup());
         object->packUpdate(ps, 0xFFFFFFFF, bstream);
      }
   }
   void unpack(NetConnection *ps, BitStream *bstream)
   {
      ghostIndex = bstream->readInt(NetConnection::GhostIdBitSize);

      if(bstream->readFlag())
      {
         S32 classId = bstream->readClassId(NetClassTypeObject, ps->getNetClassGroup());
         if(classId == -1)
         {
            ps->setLastError("Invalid packet.");
            return;
         }
         object = (NetObject *) ConsoleObject::create(ps->getNetClassGroup(), NetClassTypeObject, classId);
         if(!object)
         {
            ps->setLastError("Invalid packet.");
            return;
         }
         object->mNetFlags = NetObject::IsGhost;
         object->mNetIndex = ghostIndex;
         object->unpackUpdate(ps, bstream);
         validObject = true;
      }
      else
      {
         object = new NetObject;
         validObject = false;
      }
   }
   void process(NetConnection *ps)
   {
      Con::executef(1, "onGhostAlwaysObjectReceived");

      ps->setGhostAlwaysObject(object, ghostIndex);
      object = NULL;
   }
   DECLARE_CONOBJECT(GhostAlwaysObjectEvent);
};

IMPLEMENT_CO_NETEVENT_V1(GhostAlwaysObjectEvent);

void NetConnection::setGhostTo(bool ghostTo)
{
   if(mLocalGhosts) // if ghosting to this is already enabled, silently return
      return;

   if(ghostTo)
   {
      mLocalGhosts = new NetObject *[MaxGhostCount];
      for(S32 i = 0; i < MaxGhostCount; i++)
         mLocalGhosts[i] = NULL;
   }
}

void NetConnection::setGhostFrom(bool ghostFrom)
{
   if(mGhostArray)
      return;

   if(ghostFrom)
   {
      mGhostFreeIndex = mGhostZeroUpdateIndex = 0;
      mGhostArray = new GhostInfo *[MaxGhostCount];
      mGhostRefs = new GhostInfo[MaxGhostCount];
      S32 i;
      for(i = 0; i < MaxGhostCount; i++)
      {
         mGhostRefs[i].obj = NULL;
         mGhostRefs[i].index = i;
         mGhostRefs[i].updateMask = 0;
      }
      mGhostLookupTable = new GhostInfo *[GhostLookupTableSize];
      for(i = 0; i < GhostLookupTableSize; i++)
         mGhostLookupTable[i] = 0;
   }
}

void NetConnection::ghostOnRemove()
{
   if(mGhostArray)
      clearGhostInfo();
}

void NetConnection::ghostPacketDropped(PacketNotify *notify)
{
   GhostRef *packRef = notify->ghostList;
   // loop through all the packRefs in the packet

   while(packRef)
   {
      GhostRef *temp = packRef->nextRef;

      U32 orFlags = 0;
      AssertFatal(packRef->nextUpdateChain == NULL, "Out of order notify!!");

      // clear out the ref for this object, plus or together all
      // flags from updates after this

      GhostRef **walk = &(packRef->ghost->updateChain);
      while(*walk != packRef)
      {
         orFlags |= (*walk)->mask;
         walk = &((*walk)->nextUpdateChain);
      }
      *walk = 0;

      // for any flags we haven't updated since this (dropped) packet
      // or them into the mask so they'll get updated soon

      orFlags = packRef->mask & ~orFlags;

      if(orFlags)
      {
         if(!packRef->ghost->updateMask)
         {
            packRef->ghost->updateMask = orFlags;
            ghostPushNonZero(packRef->ghost);
         }
         else
            packRef->ghost->updateMask |= orFlags;
      }

      // if this packet was ghosting an object, set it
      // to re ghost at it's earliest convenience

      if(packRef->ghostInfoFlags & GhostInfo::Ghosting)
      {
         packRef->ghost->flags |= GhostInfo::NotYetGhosted;
         packRef->ghost->flags &= ~GhostInfo::Ghosting;
      }

      // otherwise, if it was being deleted,
      // set it to re-delete

      else if(packRef->ghostInfoFlags & GhostInfo::KillingGhost)
      {
         packRef->ghost->flags |= GhostInfo::KillGhost;
         packRef->ghost->flags &= ~GhostInfo::KillingGhost;
      }

      delete packRef;
      packRef = temp;
   }
}

void NetConnection::ghostPacketReceived(PacketNotify *notify)
{
   GhostRef *packRef = notify->ghostList;

   // loop through all the notifies in this packet

   while(packRef)
   {
      GhostRef *temp = packRef->nextRef;

      AssertFatal(packRef->nextUpdateChain == NULL, "Out of order notify!!");

      // clear this notify from the end of the object's notify
      // chain

      GhostRef **walk = &(packRef->ghost->updateChain);
      while(*walk != packRef)
         walk = &((*walk)->nextUpdateChain);

      *walk = 0;

      // if this object was ghosting , it is now ghosted

      if(packRef->ghostInfoFlags & GhostInfo::Ghosting)
         packRef->ghost->flags &= ~GhostInfo::Ghosting;

      // otherwise, if it was dieing, free the ghost

      else if(packRef->ghostInfoFlags & GhostInfo::KillingGhost)
         freeGhostInfo(packRef->ghost);

      delete packRef;
      packRef = temp;
   }
}

struct UpdateQueueEntry
{
   F32 priority;
   GhostInfo *obj;

   UpdateQueueEntry(F32 in_priority, GhostInfo *in_obj)
      { priority = in_priority; obj = in_obj; }
};

static S32 QSORT_CALLBACK UQECompare(const void *a,const void *b)
{
   GhostInfo *ga = *((GhostInfo **) a);
   GhostInfo *gb = *((GhostInfo **) b);

   F32 ret = ga->priority - gb->priority;
   return (ret < 0) ? -1 : ((ret > 0) ? 1 : 0);
}

void NetConnection::ghostWritePacket(BitStream *bstream, PacketNotify *notify)
{
#ifdef    TORQUE_DEBUG_NET
   bstream->writeInt(DebugChecksum, 32);
#endif

   notify->ghostList = NULL;

   if(!isGhostingFrom())
      return;

   if(!bstream->writeFlag(mGhosting))
      return;

   // fill a packet (or two) with ghosting data

   // first step is to check all our polled ghosts:

   // 1. Scope query - find if any new objects have come into
   //    scope and if any have gone out.
   // 2. call scoped objects' priority functions if the flag set is nonzero
   //    A removed ghost is assumed to have a high priority
   // 3. call updates based on sorted priority until the packet is
   //    full.  set flags to zero for all updated objects

   CameraScopeQuery camInfo;

   camInfo.camera = NULL;
   camInfo.pos.set(0,0,0);
   camInfo.orientation.set(0,1,0);
   camInfo.visibleDistance = 1;
   camInfo.fov = (F32)(3.1415f / 4.0f);
   camInfo.sinFov = 0.7071f;
   camInfo.cosFov = 0.7071f;

   GhostInfo *walk;

   // only need to worry about the ghosts that have update masks set...
   S32 maxIndex = 0;
   S32 i;
   for(i = 0; i < (S32)mGhostZeroUpdateIndex; i++)
   {
      // increment the updateSkip for everyone... it's all good
      walk = mGhostArray[i];
      walk->updateSkipCount++;
      if(!(walk->flags & (GhostInfo::ScopeAlways | GhostInfo::ScopeLocalAlways)))
         walk->flags &= ~GhostInfo::InScope;
   }

   if(mScopeObject)
      mScopeObject->onCameraScopeQuery(this, &camInfo);

   for(i = mGhostZeroUpdateIndex - 1; i >= 0; i--)
   {
      if(!(mGhostArray[i]->flags & GhostInfo::InScope))
         detachObject(mGhostArray[i]);
   }

   for(i = mGhostZeroUpdateIndex - 1; i >= 0; i--)
   {
      walk = mGhostArray[i];
      if(walk->index > (U32)maxIndex)
         maxIndex = walk->index;

      // clear out any kill objects that haven't been ghosted yet
      if((walk->flags & GhostInfo::KillGhost) && (walk->flags & GhostInfo::NotYetGhosted))
      {
         freeGhostInfo(walk);
         continue;
      }
      // don't do any ghost processing on objects that are being killed
      // or in the process of ghosting
      else if(!(walk->flags & (GhostInfo::KillingGhost | GhostInfo::Ghosting)))
      {
         if(walk->flags & GhostInfo::KillGhost)
            walk->priority = 10000;
         else
            walk->priority = walk->obj->getUpdatePriority(&camInfo, walk->updateMask, walk->updateSkipCount);
      }
      else
         walk->priority = 0;
   }
   GhostRef *updateList = NULL;
   dQsort(mGhostArray, mGhostZeroUpdateIndex, sizeof(GhostInfo *), UQECompare);

   // reset the array indices...
   for(i = mGhostZeroUpdateIndex - 1; i >= 0; i--)
      mGhostArray[i]->arrayIndex = i;

   S32 sendSize = 1;
   while(maxIndex >>= 1)
      sendSize++;

   if(sendSize < 3)
      sendSize = 3;

   bstream->writeInt(sendSize - 3, GhostIndexBitSize);

   U32 count = 0;
   //
   for(i = mGhostZeroUpdateIndex - 1; i >= 0 && !bstream->isFull(); i--)
   {
      GhostInfo *walk = mGhostArray[i];
        if(walk->flags & (GhostInfo::KillingGhost | GhostInfo::Ghosting))
           continue;
        
      bstream->writeFlag(true);

      bstream->writeInt(walk->index, sendSize);
      U32 updateMask = walk->updateMask;

      GhostRef *upd = new GhostRef;

      upd->nextRef = updateList;
      updateList = upd;
      upd->nextUpdateChain = walk->updateChain;
      walk->updateChain = upd;

      upd->ghost = walk;
      upd->ghostInfoFlags = 0;

      if(walk->flags & GhostInfo::KillGhost)
      {
         walk->flags &= ~GhostInfo::KillGhost;
         walk->flags |= GhostInfo::KillingGhost;
         walk->updateMask = 0;
         upd->mask = updateMask;
         ghostPushToZero(walk);
         upd->ghostInfoFlags = GhostInfo::KillingGhost;
         bstream->writeFlag(true); // killing ghost
      }
      else
      {
         bstream->writeFlag(false);
         if(walk->flags & GhostInfo::NotYetGhosted)
         {
            S32 classId = walk->obj->getClassId(getNetClassGroup());
            bstream->writeClassId(classId, NetClassTypeObject, getNetClassGroup());
#ifdef TORQUE_DEBUG_NET
            bstream->writeInt(classId ^ DebugChecksum, 32);
#endif

            walk->flags &= ~GhostInfo::NotYetGhosted;
            walk->flags |= GhostInfo::Ghosting;
            upd->ghostInfoFlags = GhostInfo::Ghosting;
         }
#ifdef TORQUE_DEBUG_NET
         else {
            S32 classId = walk->obj->getClassId(getNetClassGroup());
            bstream->writeClassId(classId, NetClassTypeObject, getNetClassGroup());
            bstream->writeInt(classId ^ DebugChecksum, 32);
         }
#endif
         // update the object
         U32 retMask = walk->obj->packUpdate(this, updateMask, bstream);
         DEBUG_LOG(("PKLOG %d GHOST %d: %s", getId(), bstream->getCurPos() - 16 - startPos, walk->obj->getClassName()));

         AssertFatal((retMask & (~updateMask)) == 0, "Cannot set new bits in packUpdate return");

         walk->updateMask = retMask;
         if(!retMask)
            ghostPushToZero(walk);

         upd->mask = updateMask & ~retMask;

         //PacketStream::getStats()->addBits(PacketStats::Send, bstream->getCurPos() - startPos, walk->obj->getPersistTag());
#ifdef TORQUE_DEBUG_NET
         bstream->writeInt(walk->index ^ DebugChecksum, 32);
#endif
      }
      walk->updateSkipCount = 0;
      count++;
   }
   //Con::printf("Ghosts updated: %d (%d remain)", count, mGhostZeroUpdateIndex);
   // no more objects...
   bstream->writeFlag(false);
   notify->ghostList = updateList;
}

void NetConnection::ghostReadPacket(BitStream *bstream)
{
#ifdef    TORQUE_DEBUG_NET
   U32 sum = bstream->readInt(32);
   AssertISV(sum == DebugChecksum, "Invalid checksum.");
#endif

   if(!isGhostingTo())
      return;
   if(!bstream->readFlag())
      return;

   S32 idSize;
   idSize = bstream->readInt( GhostIndexBitSize);
   idSize += 3;

   // while there's an object waiting...

   while(bstream->readFlag())
   {

      gGhostUpdates++;

      U32 index;
      //S32 startPos = bstream->getCurPos();
      index = (U32) bstream->readInt(idSize);
      if(bstream->readFlag()) // is this ghost being deleted?
      {
         mGhostsActive--;
         AssertFatal(mLocalGhosts[index] != NULL, "Error, NULL ghost encountered.");
         mLocalGhosts[index]->deleteObject();
         mLocalGhosts[index] = NULL;
      }
      else
      {
         if(!mLocalGhosts[index]) // it's a new ghost... cool
         {
            mGhostsActive++;
            S32 classId = bstream->readClassId(NetClassTypeObject, getNetClassGroup());
            if(classId == -1)
            {
               setLastError("Invalid packet.");
               return;
            }

            NetObject *obj = (NetObject *) ConsoleObject::create(getNetClassGroup(), NetClassTypeObject, classId);
            if(!obj)
            {
               setLastError("Invalid packet.");
               return;
            }
            obj->mNetFlags = NetObject::IsGhost;

            // object gets initial update before adding to the manager

            obj->mNetIndex = index;
            mLocalGhosts[index] = obj;
#ifdef TORQUE_DEBUG_NET
            U32 checksum = bstream->readInt(32);
            S32 origId = checksum ^ DebugChecksum;
            AssertISV(mLocalGhosts[index] != NULL, "Invalid dest ghost.");
            AssertISV(origId == mLocalGhosts[index]->getClassId(getNetClassGroup()),
               avar("class id mismatch for dest class %s.",
                  mLocalGhosts[index]->getClassName()) );
#endif
            mLocalGhosts[index]->unpackUpdate(this, bstream);

            if(!obj->registerObject())
            {
               if(!mErrorBuffer[0])
                  setLastError("Invalid packet.");
               return;
            }
            if(mRemoteConnection)
               obj->mServerObject = mRemoteConnection->resolveObjectFromGhostIndex(index);

            addObject(obj);
         }
         else
         {
#ifdef TORQUE_DEBUG_NET
            S32 classId = bstream->readClassId(NetClassTypeObject, getNetClassGroup());
            U32 checksum = bstream->readInt(32);
            S32 origId = checksum ^ DebugChecksum;
            AssertISV(mLocalGhosts[index] != NULL, "Invalid dest ghost.");
            AssertISV(origId == mLocalGhosts[index]->getClassId(getNetClassGroup()),
               avar("class id mismatch for dest class %s.",
                  mLocalGhosts[index]->getClassName()) );
#endif
            mLocalGhosts[index]->unpackUpdate(this, bstream);
         }
         //PacketStream::getStats()->addBits(PacketStats::Receive, bstream->getCurPos() - startPos, ghostRefs[index].localGhost->getPersistTag());
#ifdef TORQUE_DEBUG_NET
         U32 checksum = bstream->readInt(32);
         S32 origIndex = checksum ^ DebugChecksum;
         AssertISV(origIndex == index,
            avar("unpackUpdate did not match packUpdate for object of class %s.",
               mLocalGhosts[index]->getClassName()) );
#endif
         if(mErrorBuffer[0])
            return;
      }
   }
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

void NetConnection::setScopeObject(NetObject *obj)
{
   if(((NetObject *) mScopeObject) == obj)
      return;
   mScopeObject = obj;
}

void NetConnection::detachObject(GhostInfo *info)
{
   // mark it for ghost killin'
   info->flags |= GhostInfo::KillGhost;

   // if the mask is in the zero range, we've got to move it up...
   if(!info->updateMask)
   {
      info->updateMask = 0xFFFFFFFF;
      ghostPushNonZero(info);
   }
   if(info->obj)
   {
      if(info->prevObjectRef)
         info->prevObjectRef->nextObjectRef = info->nextObjectRef;
      else
         info->obj->mFirstObjectRef = info->nextObjectRef;
      if(info->nextObjectRef)
         info->nextObjectRef->prevObjectRef = info->prevObjectRef;

      // remove it from the lookup table
      U32 id = info->obj->getId();
      for(GhostInfo **walk = &mGhostLookupTable[id & (GhostLookupTableSize - 1)]; *walk; walk = &((*walk)->nextLookupInfo))
      {
         GhostInfo *temp = *walk;
         if(temp == info)
         {
            *walk = temp->nextLookupInfo;
            break;
         }
      }
      info->prevObjectRef = info->nextObjectRef = NULL;
      info->obj = NULL;
   }
}

void NetConnection::freeGhostInfo(GhostInfo *ghost)
{
   AssertFatal(ghost->arrayIndex < mGhostFreeIndex, "Ghost already freed.");
   if(ghost->arrayIndex < mGhostZeroUpdateIndex)
   {
      AssertFatal(ghost->updateMask != 0, "Invalid ghost mask.");
      ghost->updateMask = 0;
      ghostPushToZero(ghost);
   }
   ghostPushZeroToFree(ghost);
   AssertFatal(ghost->updateChain == NULL, "Ack!");
}

//-----------------------------------------------------------------------------

void NetConnection::objectLocalScopeAlways(NetObject *obj)
{
   if(!isGhostingFrom())
      return;
   objectInScope(obj);
   for(GhostInfo *walk = mGhostLookupTable[obj->getId() & (GhostLookupTableSize - 1)]; walk; walk = walk->nextLookupInfo)
   {
      if(walk->obj != obj)
         continue;
      walk->flags |= GhostInfo::ScopeLocalAlways;
      return;
   }
}

void NetConnection::objectLocalClearAlways(NetObject *obj)
{
   if(!isGhostingFrom())
      return;
   for(GhostInfo *walk = mGhostLookupTable[obj->getId() & (GhostLookupTableSize - 1)]; walk; walk = walk->nextLookupInfo)
   {
      if(walk->obj != obj)
         continue;
      walk->flags &= ~GhostInfo::ScopeLocalAlways;
      return;
   }
}

bool NetConnection::validateGhostArray()
{
   AssertFatal(mGhostZeroUpdateIndex >= 0 && mGhostZeroUpdateIndex <= mGhostFreeIndex, "Invalid update index range.");
   AssertFatal(mGhostFreeIndex <= MaxGhostCount, "Invalid free index range.");
   U32 i;
   for(i = 0; i < mGhostZeroUpdateIndex; i ++)
   {
      AssertFatal(mGhostArray[i]->arrayIndex == i, "Invalid array index.");
      AssertFatal(mGhostArray[i]->updateMask != 0, "Invalid ghost mask.");
   }
   for(; i < mGhostFreeIndex; i ++)
   {
      AssertFatal(mGhostArray[i]->arrayIndex == i, "Invalid array index.");
      AssertFatal(mGhostArray[i]->updateMask == 0, "Invalid ghost mask.");
   }
   for(; i < MaxGhostCount; i++)
   {
      AssertFatal(mGhostArray[i]->arrayIndex == i, "Invalid array index.");
   }
   return true;
}

void NetConnection::objectInScope(NetObject *obj)
{
   if (!mScoping || !isGhostingFrom())
      return;
    if (obj->isScopeLocal() && !isLocalConnection())
        return;

   S32 index = obj->getId() & (GhostLookupTableSize - 1);

   // check if it's already in scope
   // the object may have been cleared out without the lookupTable being cleared
   // so validate that the object pointers are the same.

   for(GhostInfo *walk = mGhostLookupTable[index ]; walk; walk = walk->nextLookupInfo)
   {
      if(walk->obj != obj)
         continue;
      walk->flags |= GhostInfo::InScope;
      return;
   }

   if (mGhostFreeIndex == MaxGhostCount)
   {
      AssertWarn(0,"NetConnection::objectInScope: too many ghosts");
      return;
   }

   GhostInfo *giptr = mGhostArray[mGhostFreeIndex];
   ghostPushFreeToZero(giptr);
   giptr->updateMask = 0xFFFFFFFF;
   ghostPushNonZero(giptr);

   giptr->flags = GhostInfo::NotYetGhosted | GhostInfo::InScope;

   if(obj->mNetFlags.test(NetObject::ScopeAlways))
      giptr->flags |= GhostInfo::ScopeAlways;

   giptr->obj = obj;
   giptr->updateChain = NULL;
   giptr->updateSkipCount = 0;

   giptr->connection = this;

   giptr->nextObjectRef = obj->mFirstObjectRef;
   if(obj->mFirstObjectRef)
      obj->mFirstObjectRef->prevObjectRef = giptr;
   giptr->prevObjectRef = NULL;
   obj->mFirstObjectRef = giptr;

   giptr->nextLookupInfo = mGhostLookupTable[index];
   mGhostLookupTable[index] = giptr;
   //AssertFatal(validateGhostArray(), "Invalid ghost array!");
}

//-----------------------------------------------------------------------------

void NetConnection::handleConnectionMessage(U32 message, U32 sequence, U32 ghostCount)
{
   if((  message == SendNextDownloadRequest
      || message == FileDownloadSizeMessage
      || message == GhostAlwaysStarting
      || message == GhostAlwaysDone
      || message == EndGhosting) && !isGhostingTo())
   {
      setLastError("Invalid packet.");
      return;
   }

   S32 i;
   GhostSave sv;
   switch(message)
   {
      case GhostAlwaysDone:
         mGhostingSequence = sequence;
         // ok, all the ghost always objects are now on the client... but!
         // it's possible that there were some file load errors...
         // if so, we need to indicate to the server to restart ghosting, after
         // we download all the files...
         sv.ghost = NULL;
         mGhostAlwaysSaveList.push_back(sv);
         if(mGhostAlwaysSaveList.size() == 1)
            loadNextGhostAlwaysObject(true);
         break;
      case ReadyForNormalGhosts:
         if(sequence != mGhostingSequence)
            return;
         Con::executef(this, 1, "onGhostAlwaysObjectsReceived");
         Con::printf("Ghost Always objects received.");
         mGhosting = true;
         for(i = 0; i < (S32)mGhostFreeIndex; i++)
         {
            if(mGhostArray[i]->flags & GhostInfo::ScopedEvent)
               mGhostArray[i]->flags &= ~(GhostInfo::Ghosting | GhostInfo::ScopedEvent);
         }
         break;
      case EndGhosting:
         // just delete all the local ghosts,
         // and delete all the ghosts in the current save list
         for(i = 0; i < MaxGhostCount; i++)
         {
            if(mLocalGhosts[i])
            {
               mLocalGhosts[i]->deleteObject();
               mLocalGhosts[i] = NULL;
            }
         }
         while(mGhostAlwaysSaveList.size())
         {
            delete mGhostAlwaysSaveList[0].ghost;
            mGhostAlwaysSaveList.pop_front();
         }
         break;
      case GhostAlwaysStarting:
         Con::executef(2, "onGhostAlwaysStarted", Con::getIntArg(ghostCount));
         break;
      case SendNextDownloadRequest:
         sendNextFileDownloadRequest();
         break;
      case FileDownloadSizeMessage:
         mCurrentFileBufferSize = sequence;
         mCurrentFileBuffer = dRealloc(mCurrentFileBuffer, mCurrentFileBufferSize);
         mCurrentFileBufferOffset = 0;
         break;
   }
}

void NetConnection::activateGhosting()
{
   if(!isGhostingFrom())
      return;

   mGhostingSequence++;

   // iterate through the ghost always objects and InScope them...
   // also post em all to the other side.

   SimSet* ghostAlwaysSet = Sim::getGhostAlwaysSet();

   SimSet::iterator i;

   AssertFatal((mGhostFreeIndex == 0) && (mGhostZeroUpdateIndex == 0), "Error: ghosts in the ghost list before activate.");

   U32 sz = ghostAlwaysSet->size();
   S32 j;

   for(j = 0; j < (S32)sz; j++)
   {
      U32 idx = MaxGhostCount - sz + j;
      mGhostArray[j] = mGhostRefs + idx;
      mGhostArray[j]->arrayIndex = j;
   }
   for(j = sz; j < MaxGhostCount; j++)
   {
      U32 idx = j - sz;
      mGhostArray[j] = mGhostRefs + idx;
      mGhostArray[j]->arrayIndex = j;
   }
   mScoping = true; // so that objectInScope will work
   for(i = ghostAlwaysSet->begin(); i != ghostAlwaysSet->end(); i++)
   {
      AssertFatal(dynamic_cast<NetObject *>(*i) != NULL, avar("Non NetObject in GhostAlwaysSet: %s", (*i)->getClassName()));
      NetObject *obj = (NetObject *)(*i);
      if(obj->mNetFlags.test(NetObject::Ghostable))
         objectInScope(obj);
   }
   sendConnectionMessage(GhostAlwaysStarting, mGhostingSequence, ghostAlwaysSet->size());
   for(j = mGhostZeroUpdateIndex - 1; j >= 0; j--)
   {
      AssertFatal((mGhostArray[j]->flags & GhostInfo::ScopeAlways) != 0, "Non-scope always in the scope always list.")

      // we may end up resending state here, but at least initial state
      // will not be resent.
      mGhostArray[j]->updateMask = 0;
      ghostPushToZero(mGhostArray[j]);
      mGhostArray[j]->flags &= ~GhostInfo::NotYetGhosted;
      mGhostArray[j]->flags |= GhostInfo::ScopedEvent;

      postNetEvent(new GhostAlwaysObjectEvent(mGhostArray[j]->obj, mGhostArray[j]->index));
   }
   sendConnectionMessage(GhostAlwaysDone, mGhostingSequence);
   //AssertFatal(validateGhostArray(), "Invalid ghost array!");
}

void NetConnection::clearGhostInfo()
{
   // gotta clear out the ghosts...
   for(PacketNotify *walk = mNotifyQueueHead; walk; walk = walk->nextPacket)
   {
      ghostPacketReceived(walk);
      walk->ghostList = NULL;
   }
   for(S32 i = 0; i < MaxGhostCount; i++)
   {
      if(mGhostRefs[i].arrayIndex < mGhostFreeIndex)
      {
         detachObject(&mGhostRefs[i]);
         freeGhostInfo(&mGhostRefs[i]);
      }
   }
   AssertFatal((mGhostFreeIndex == 0) && (mGhostZeroUpdateIndex == 0), "Invalid indices.");
}

void NetConnection::resetGhosting()
{
   if(!isGhostingFrom())
      return;
   // stop all ghosting activity
   // send a message to the other side notifying of this

   mGhosting = false;
   mScoping = false;
   sendConnectionMessage(EndGhosting, mGhostingSequence);
   mGhostingSequence++;
   clearGhostInfo();
   //AssertFatal(validateGhostArray(), "Invalid ghost array!");
}

void NetConnection::setGhostAlwaysObject(NetObject *object, U32 index)
{
   if(!isGhostingTo())
   {
      object->deleteObject();
      setLastError("Invalid packet.");
      return;
   }
   object->mNetFlags = NetObject::IsGhost;
   object->mNetIndex = index;

   // while there's an object waiting...
   if (isLocalConnection()) {
      object->mServerObject = mRemoteConnection->resolveObjectFromGhostIndex(index);
   }

   GhostSave sv;
   sv.ghost = object;
   sv.index = index;
   mGhostAlwaysSaveList.push_back(sv);

   // check if we are already downloading files for a previous object:
   if(mGhostAlwaysSaveList.size() == 1)
      loadNextGhostAlwaysObject(true);  // the initial call always has "new" files

}

void NetConnection::fileDownloadSegmentComplete()
{
   // this is called when a the file list has finished processing...
   // at this point we can try again to add the object
   // subclasses can override this to do, for example, datablock redos.
   if(mGhostAlwaysSaveList.size())
      loadNextGhostAlwaysObject(mNumDownloadedFiles != 0);
}

void NetConnection::loadNextGhostAlwaysObject(bool hadNewFiles)
{
   if(!mGhostAlwaysSaveList.size())
      return;

   while(mGhostAlwaysSaveList.size())
   {
      // only check for new files if this is the first load, or if new
      // files were downloaded from the server.
      if(hadNewFiles)
         ResourceManager->setMissingFileLogging(true);
      ResourceManager->clearMissingFileList();
      NetObject *object = mGhostAlwaysSaveList[0].ghost;
      U32 index = mGhostAlwaysSaveList[0].index;

      if(!object)
      {
         // a null object is used to signify that the last ghost in the list is down
         mGhostAlwaysSaveList.pop_front();
         AssertFatal(mGhostAlwaysSaveList.size() == 0, "Error! Ghost save list should be empty!");
         sendConnectionMessage(ReadyForNormalGhosts, mGhostingSequence);
         ResourceManager->setMissingFileLogging(false);
         return;
      }
      mFilesWereDownloaded = hadNewFiles;

      if(!object->registerObject())
      {
         mFilesWereDownloaded = false;
         // make sure there's an error message if necessary
         if(!mErrorBuffer[0])
            setLastError("Invalid packet.");

         // if there were no new files, make sure the error message
         // is the one from the last time we tried to add this object
         if(!hadNewFiles)
         {
            dStrcpy(mErrorBuffer, mLastFileErrorBuffer);
            ResourceManager->setMissingFileLogging(false);
            return;
         }

         // object failed to load, let's see if it had any missing files
         if(!ResourceManager->getMissingFileList(mMissingFileList))
         {
            // no missing files, must be an error
            // connection will automagically delete the ghost always list
            // when this error is reported.
            ResourceManager->setMissingFileLogging(false);
            return;
         }

         // ok, copy the error buffer out to a scratch pad for now
         dStrcpy(mLastFileErrorBuffer, mErrorBuffer);
         mErrorBuffer[0] = 0;

         // request the missing files...
         mNumDownloadedFiles = 0;
         sendNextFileDownloadRequest();
         break;
      }
      mFilesWereDownloaded = false;
      ResourceManager->setMissingFileLogging(false);
      addObject(object);
      mGhostAlwaysSaveList.pop_front();

      AssertFatal(mLocalGhosts[index] == NULL, "Ghost already in table!");
      mLocalGhosts[index] = object;
      hadNewFiles = true;
   }
}

//-----------------------------------------------------------------------------

NetObject *NetConnection::resolveGhost(S32 id)
{
   return mLocalGhosts[id];
}

NetObject *NetConnection::resolveObjectFromGhostIndex(S32 id)
{
   return mGhostRefs[id].obj;
}

S32 NetConnection::getGhostIndex(NetObject *obj)
{
   if(!isGhostingFrom())
      return obj->mNetIndex;
   S32 index = obj->getId() & (GhostLookupTableSize - 1);

   for(GhostInfo *gptr = mGhostLookupTable[index]; gptr; gptr = gptr->nextLookupInfo)
   {
      if(gptr->obj == obj && (gptr->flags & (GhostInfo::KillingGhost | GhostInfo::Ghosting | GhostInfo::NotYetGhosted | GhostInfo::KillGhost)) == 0)
         return gptr->index;
   }
   return -1;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void NetConnection::ghostWriteStartBlock(ResizeBitStream *stream)
{
   // Ok, writing the start block for the ghosts:
   // here's how it goes.
   //
   // First we record out all the indices and class ids for all the objects
   // This is so when the objects are read in, all the objects are instantiated
   // before they are unpacked.  The unpack code may reference other
   // existing ghosts, so we want to make sure that all the ghosts are in the
   // table with the correct pointers before any of the unpacks are called.

   stream->write(mGhostingSequence);

   // first write out the indices and ids:
   for(U32 i = 0; i < MaxGhostCount; i++)
   {
      if(mLocalGhosts[i])
      {
         stream->writeFlag(true);
         stream->writeInt(i, GhostIdBitSize);
         stream->writeClassId(mLocalGhosts[i]->getClassId(getNetClassGroup()), NetClassTypeObject, getNetClassGroup());
         stream->validate();
      }
   }
   // mark off the end of the ghost list:
   // it would be more space efficient to write out a count of active ghosts followed
   // by index run lengths, but hey, what's a few bits here and there?

   stream->writeFlag(false);

   // then, for each ghost written into the start block, write the full pack update
   // into the start block.  For demos to work properly, packUpdate must
   // be callable from client objects.
   for(U32 i = 0; i < MaxGhostCount; i++)
   {
      if(mLocalGhosts[i])
      {
         mLocalGhosts[i]->packUpdate(this, 0xFFFFFFFF, stream);
         stream->validate();
      }
   }
}

void NetConnection::ghostReadStartBlock(BitStream *stream)
{
   stream->read(&mGhostingSequence);

   // read em back in.
   // first, read in the index/class id, construct the object, and place it in mLocalGhosts[i]

   while(stream->readFlag())
   {
      U32 index = stream->readInt(GhostIdBitSize);
      S32 tag = stream->readClassId(NetClassTypeObject, getNetClassGroup());
      NetObject *obj = (NetObject *) ConsoleObject::create(getNetClassGroup(), NetClassTypeObject, tag);
      if(!obj)
      {
         setLastError("Invalid packet.");
         return;
      }
      obj->mNetFlags = NetObject::IsGhost;
      obj->mNetIndex = index;
      mLocalGhosts[index] = obj;
   }

   // now, all the ghosts are in the mLocalGhosts, so we loop
   // through all non-null mLocalGhosts, unpacking the objects
   // as we go:

   for(U32 i = 0; i < MaxGhostCount; i++)
   {
      if(mLocalGhosts[i])
      {
         mLocalGhosts[i]->unpackUpdate(this, stream);
         if(!mLocalGhosts[i]->registerObject())
         {
            if(mErrorBuffer[0])
               setLastError("Invalid packet.");
            return;
         }
         addObject(mLocalGhosts[i]);
      }
   }
   // MARKF - TODO - looks like we could have memory leaks here
   // if there are errors.
}
