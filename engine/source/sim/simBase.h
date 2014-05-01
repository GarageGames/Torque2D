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

#ifndef _SIMBASE_H_
#define _SIMBASE_H_

#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif

#ifndef _FIND_ITERATOR_H_
#include "collection/findIterator.h"
#endif

#ifndef _BITSET_H_
#include "collection/bitSet.h"
#endif

#ifndef _CONSOLEOBJECT_H_
#include "console/consoleObject.h"
#endif

#ifndef _SIMDICTIONARY_H_
#include "sim/simDictionary.h"
#endif

#ifndef _PLATFORMMUTEX_H_
#include "platform/threads/mutex.h"
#endif

#ifndef _PLATFORMSEMAPHORE_H_
#include "platform/platformSemaphore.h"
#endif

#ifndef _TAML_CALLBACKS_H_
#include "persistence/taml/tamlCallbacks.h"
#endif

#ifndef _SIM_OBJECT_LIST_H_
#include "sim/SimObjectList.h"
#endif

#ifndef _SIM_OBJECT_H_
#include "sim/simObject.h"
#endif

#ifndef _SIM_EVENT_H_
#include "simEvent.h"
#endif

#ifndef _SIM_CONSOLE_EVENT_H_
#include "simConsoleEvent.h"
#endif

#ifndef _SIM_CONSOLE_THREAD_EXEC_EVENT_H_
#include "simConsoleThreadExecEvent.h"
#endif

#ifndef _SIM_OBJECT_PTR_H_
#include "simObjectPtr.h"
#endif

#ifndef _SIM_DATABLOCK_H_
#include "simDatablock.h"
#endif

#ifndef _SIM_DATABLOCK_GROUP_H_
#include "simDatablockGroup.h"
#endif

//---------------------------------------------------------------------------

/// Definition of some basic Sim system constants.
///
/// These constants define the range of ids assigned to datablocks
/// (DataBlockObjectIdFirst - DataBlockObjectIdLast), and the number
/// of bits used to store datablock IDs.
///
/// Normal Sim objects are given the range of IDs starting at
/// DynamicObjectIdFirst and going to infinity. Sim objects use
/// a SimObjectId to represent their ID; this is currently a U32.
///
/// The RootGroupId is assigned to gRootGroup, in which most SimObjects
/// are addded as child members. See simManager.cc for details, particularly
/// Sim::initRoot() and following.
enum SimObjectsConstants
{
   DataBlockObjectIdFirst = 3,
   DataBlockObjectIdBitSize = 10,
   DataBlockObjectIdLast = DataBlockObjectIdFirst + (1 << DataBlockObjectIdBitSize) - 1,

   MessageObjectIdFirst = DataBlockObjectIdLast + 1,
   MessageObjectIdBitSize = 6,
   MessageObjectIdLast = MessageObjectIdFirst + (1 << MessageObjectIdBitSize) - 1,

   DynamicObjectIdFirst = MessageObjectIdLast + 1,
   InvalidEventId = 0,
   RootGroupId = 0xFFFFFFFF,
};

class SimEvent;
class SimObject;
class SimGroup;
class SimManager;
class Namespace;
class BitStream;
class Stream;

typedef U32 SimTime;

//---------------------------------------------------------------------------

/// @defgroup simbase_helpermacros Helper Macros
///
/// These are used for named sets and groups in the manager.
/// @{
#define DeclareNamedSet(set) extern SimSet *g##set;inline SimSet *get##set() { return g##set; }
#define DeclareNamedGroup(set) extern SimGroup *g##set;inline SimGroup *get##set() { return g##set; }
#define ImplementNamedSet(set) SimSet *g##set;
#define ImplementNamedGroup(set) SimGroup *g##set;
/// @}
//---------------------------------------------------------------------------

namespace Sim
{
   DeclareNamedSet(ActiveActionMapSet)
   DeclareNamedSet(GhostAlwaysSet)
   DeclareNamedSet(BehaviorSet)
   DeclareNamedSet(AchievementSet)
   DeclareNamedGroup(ActionMapGroup)
   DeclareNamedGroup(ClientGroup)
   DeclareNamedGroup(GuiGroup)
   DeclareNamedGroup(GuiDataGroup)
   DeclareNamedGroup(TCPGroup)
   DeclareNamedGroup(ClientConnectionGroup)
   DeclareNamedGroup(ChunkFileGroup);

   void init();
   void shutdown();

   SimDataBlockGroup *getDataBlockGroup();
   SimGroup* getRootGroup();

   SimObject* findObject(SimObjectId);
   SimObject* findObject(const char* name);
   template<class T> inline bool findObject(SimObjectId id,T*&t)
   {
      t = dynamic_cast<T*>(findObject(id));
      return t != NULL;
   }
   template<class T> inline bool findObject(const char* pObjectName,T*&t)
   {
      t = dynamic_cast<T*>(findObject(pObjectName));
      return t != NULL;
   }
   template<class T> inline T* findObject(SimObjectId id)
   {
       return dynamic_cast<T*>(findObject(id));
   }
   template<class T> inline T* findObject(const char* pObjectName)
   {
       return dynamic_cast<T*>(findObject(pObjectName));
   }

   void advanceToTime(SimTime time);
   void advanceTime(SimTime delta);
   SimTime getCurrentTime();
   SimTime getTargetTime();

   /// a target time of 0 on an event means current event
   U32 postEvent(SimObject*, SimEvent*, U32 targetTime);

   inline U32 postEvent(SimObjectId iD,SimEvent*evt, U32 targetTime)
   {
      return postEvent(findObject(iD), evt, targetTime);
   }
   inline U32 postEvent(const char *objectName,SimEvent*evt, U32 targetTime)
   {
      return postEvent(findObject(objectName), evt, targetTime);
   }
   inline U32 postCurrentEvent(SimObject*obj, SimEvent*evt)
   {
      return postEvent(obj,evt,getCurrentTime());
   }
   inline U32 postCurrentEvent(SimObjectId obj,SimEvent*evt)
   {
      return postEvent(obj,evt,getCurrentTime());
   }
   inline U32 postCurrentEvent(const char *obj,SimEvent*evt)
   {
      return postEvent(obj,evt,getCurrentTime());
   }

   void cancelEvent(U32 eventId);
   bool isEventPending(U32 eventId);
   U32  getEventTimeLeft(U32 eventId);
   U32  getTimeSinceStart(U32 eventId);
   U32  getScheduleDuration(U32 eventId);

   bool saveObject(SimObject *obj, Stream *stream);
   SimObject *loadObjectStream(Stream *stream);

   bool saveObject(SimObject *obj, const char *filename);
   SimObject *loadObjectStream(const char *filename);
}

//----------------------------------------------------------------------------
#define DECLARE_CONSOLETYPE(T) \
   DefineConsoleType( Type##T##Ptr )

#define IMPLEMENT_CONSOLETYPE(T) \
   DatablockConsoleType( T##Ptr, Type##T##Ptr, sizeof(T*), T )

#define IMPLEMENT_SETDATATYPE(T) \
   ConsoleSetType( Type##T##Ptr ) \
   {                                                                                                 \
      if (argc == 1) {                                                                               \
         *reinterpret_cast<T**>(dptr) = NULL;                                                        \
         if (argv[0] && argv[0][0] && !Sim::findObject(argv[0],*reinterpret_cast<T**>(dptr)))        \
            Con::printf("Object '%s' is not a member of the '%s' data block class", argv[0], #T);    \
      }                                                                                              \
      else                                                                                           \
         Con::printf("Cannot set multiple args to a single pointer.");                               \
   }

#define IMPLEMENT_GETDATATYPE(T) \
   ConsoleGetType( Type##T##Ptr ) \
   {                                                                                   \
      T** obj = reinterpret_cast<T**>(dptr);                                           \
      char* returnBuffer = Con::getReturnBuffer(16);                                   \
      dSprintf(returnBuffer, 16, "%s", *obj ? (*obj)->getIdString() : "");             \
      return returnBuffer;                                                             \
   }

//---------------------------------------------------------------------------

#endif
