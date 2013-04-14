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

#include "sim/simObject.h"
#include "sim/simObjectTimerEvent.h"
#include "console/consoleInternal.h"
#include "console/codeBlock.h"
#include "console/consoleInternal.h"
#include "memory/frameAllocator.h"
#include "io/fileStream.h"
#include "io/fileObject.h"
#include "console/consoleTypeValidators.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SimObject);

namespace Sim
{
    extern U32 gNextObjectId;
    extern SimIdDictionary *gIdDictionary;
    extern SimManagerNameDictionary *gNameDictionary;
    extern void cancelPendingEvents(SimObject *obj);
}

//-----------------------------------------------------------------------------

SimObject::SimObject( const U8 namespaceLinkMask ) : mNSLinkMask( namespaceLinkMask )
{
    mFlags.set( ModStaticFields | ModDynamicFields );
    objectName               = NULL;
    mInternalName            = NULL;
    nextNameObject           = (SimObject*)-1;
    nextManagerNameObject    = (SimObject*)-1;
    nextIdObject             = NULL;  
    mId                      = 0;
    mIdString                = StringTable->EmptyString;
    mGroup                   = 0;
    mNameSpace               = NULL;
    mNotifyList              = NULL;
    mTypeMask                = 0;
    mScriptCallbackGuard     = 0;
    mFieldDictionary         = NULL;
    mCanSaveFieldDictionary	 = true;
    mClassName               = NULL;
    mSuperClassName          = NULL;
    mProgenitorFile          = CodeBlock::getCurrentCodeBlockFullPath();
    mPeriodicTimerID         = 0;
}

//---------------------------------------------------------------------------

bool SimObject::registerObject()
{
    AssertFatal( !mFlags.test( Added ), "reigsterObject - Object already registered!");
    mFlags.clear(Deleted | Removed);

    if( mId == 0 )
    {
        mId = Sim::gNextObjectId++;

        char idBuffer[64];
        dSprintf(idBuffer, sizeof(idBuffer), "%d", mId);
        mIdString = StringTable->insert( idBuffer );
    }

   AssertFatal(Sim::gIdDictionary && Sim::gNameDictionary, 
      "SimObject::registerObject - tried to register an object before Sim::init()!");

   Sim::gIdDictionary->insert(this);	

   Sim::gNameDictionary->insert(this);

    // Notify object
   bool ret = onAdd();

   if(!ret)
      unregisterObject();

   AssertFatal(!ret || isProperlyAdded(), "Object did not call SimObject::onAdd()");

    if ( isMethod( "onAdd" ) )
        Con::executef( this, 1, "onAdd" );

   return ret;
}

//---------------------------------------------------------------------------

void SimObject::unregisterObject()
{
    // Sanity!
    AssertISV( getScriptCallbackGuard() == 0, "SimObject::unregisterObject: Object is being unregistered whilst performing a script callback!" );

    if ( isMethod( "onRemove" ) )
        Con::executef( this, 1, "onRemove" );

   mFlags.set(Removed);

   // Notify object first
   onRemove();

   // Clear out any pending notifications before
   // we call our own, just in case they delete
   // something that we have referenced.
   clearAllNotifications();

   // Notify all objects that are waiting for delete
   // messages
   if (getGroup())
      getGroup()->removeObject(this);

   processDeleteNotifies();

   // Do removals from the Sim.
   Sim::gNameDictionary->remove(this);
   Sim::gIdDictionary->remove(this);
   Sim::cancelPendingEvents(this);
}

//---------------------------------------------------------------------------

void SimObject::deleteObject()
{
    // Sanity!
    AssertISV( getScriptCallbackGuard() == 0, "SimObject::deleteObject: Object is being deleted whilst performing a script callback!" );

    AssertFatal(mFlags.test(Added),
        "SimObject::deleteObject: Object not registered.");
    AssertFatal(!isDeleted(),"SimManager::deleteObject: "
        "Object has already been deleted");
    AssertFatal(!isRemoved(),"SimManager::deleteObject: "
        "Object in the process of being removed");
    mFlags.set(Deleted);

   unregisterObject();
   delete this;
}

//---------------------------------------------------------------------------


void SimObject::setId(SimObjectId newId)
{
    if(!mFlags.test(Added))
    {
        mId = newId;
    }
    else
    {
       // get this object out of the id dictionary if it's in it
       Sim::gIdDictionary->remove(this);

        // Free current Id.
        // Assign new one.
        mId = newId ? newId : Sim::gNextObjectId++;
        Sim::gIdDictionary->insert(this);
    }

    char idBuffer[64];
    dSprintf(idBuffer, sizeof(idBuffer), "%d", mId);
    mIdString = StringTable->insert( idBuffer );
}

void SimObject::assignName(const char *name)
{
   // Added this assert 3/30/2007 because it is dumb to try to name
   // a SimObject the same thing as it's class name -patw
   //AssertFatal( dStricmp( getClassName(), name ), "Attempted to assign a name to a SimObject which matches it's type name." );
   if( dStricmp( getClassName(), name ) == 0 )
      Con::errorf( "SimObject::assignName - Assigning name '%s' to instance of object with type '%s'."
      " This can cause namespace linking issues.", getClassName(), name  );

#if 0
    Con::printf( "SimObject::assignName(%s)", name );
#endif

   // Is this name already registered?
   if ( Sim::gNameDictionary->find(name) != NULL )
   {
       // Yes, so error,
       Con::errorf( "SimObject::assignName() - Attempted to set object to name '%s' but it is already assigned to another object.", name );
       return;
   }

   StringTableEntry newName = NULL;
   if(name[0])
      newName = StringTable->insert(name);

   if(mGroup)
      mGroup->nameDictionary.remove(this);
   if(mFlags.test(Added))
      Sim::gNameDictionary->remove(this);

   objectName = newName;

   if(mGroup)
      mGroup->nameDictionary.insert(this);
   if(mFlags.test(Added))
      Sim::gNameDictionary->insert(this);
}

//---------------------------------------------------------------------------

bool SimObject::registerObject(U32 id)
{
   setId(id);
   return registerObject();
}

bool SimObject::registerObject(const char *name)
{
   assignName(name);
   return registerObject();
}

bool SimObject::registerObject(const char *name, U32 id)
{
   setId(id);
   assignName(name);
   return registerObject();
}

void SimObject::assignDynamicFieldsFrom(SimObject* parent)
{
   if(parent->mFieldDictionary)
   {
      if( mFieldDictionary == NULL )
         mFieldDictionary = new SimFieldDictionary;
      mFieldDictionary->assignFrom(parent->mFieldDictionary);
   }
}

void SimObject::assignFieldsFrom(SimObject *parent)
{
   // only allow field assigns from objects of the same class:
   if(getClassRep() == parent->getClassRep())
   {
      const AbstractClassRep::FieldList &list = getFieldList();

      // copy out all the fields:
      for(U32 i = 0; i < (U32)list.size(); i++)
      {
         const AbstractClassRep::Field* f = &list[i];
         S32 lastField = f->elementCount - 1;
         for(S32 j = 0; j <= lastField; j++)
         {
             const char* fieldVal = (*f->getDataFn)( this,  Con::getData(f->type, (void *) (((const char *)parent) + f->offset), j, f->table, f->flag));
            //if(fieldVal)
            //   Con::setData(f->type, (void *) (((const char *)this) + f->offset), j, 1, &fieldVal, f->table);
            if(fieldVal)
            {
               // code copied from SimObject::setDataField().
               // TODO: paxorr: abstract this into a better setData / getData that considers prot fields.
               FrameTemp<char> buffer(2048);
               FrameTemp<char> bufferSecure(2048); // This buffer is used to make a copy of the data 
               ConsoleBaseType *cbt = ConsoleBaseType::getType( f->type );
               const char* szBuffer = cbt->prepData( fieldVal, buffer, 2048 );
               dMemset( bufferSecure, 0, 2048 );
               dMemcpy( bufferSecure, szBuffer, dStrlen( szBuffer ) );

               if((*f->setDataFn)( this, bufferSecure ) )
                  Con::setData(f->type, (void *) (((const char *)this) + f->offset), j, 1, &fieldVal, f->table);
            }
         }
      }
   }

   assignDynamicFieldsFrom(parent);
}

bool SimObject::writeField(StringTableEntry fieldname, const char* value)
{
   // Don't write empty fields.
   if (!value || !*value)
      return false;

   // Don't write ParentGroup
   if( fieldname == StringTable->insert("parentGroup") )
      return false;


   return true;
}

void SimObject::writeFields(Stream &stream, U32 tabStop)
{
   const AbstractClassRep::FieldList &list = getFieldList();

   for(U32 i = 0; i < (U32)list.size(); i++)
   {
      const AbstractClassRep::Field* f = &list[i];

      if( f->type == AbstractClassRep::DepricatedFieldType ||
          f->type == AbstractClassRep::StartGroupFieldType ||
          f->type == AbstractClassRep::EndGroupFieldType) continue;

      // Fetch fieldname.
      StringTableEntry fieldName = StringTable->insert( f->pFieldname );

      // Fetch element count.
      const S32 elementCount = f->elementCount;

      // Skip if the field should not be written.
      // For now, we only deal with non-array fields.
      if (  elementCount == 1 &&
            f->writeDataFn != NULL &&
            f->writeDataFn( this, fieldName ) == false )
            continue;

      for(U32 j = 0; S32(j) < elementCount; j++)
      {
         char array[8];
         dSprintf( array, 8, "%d", j );
         const char *val = getDataField(fieldName, array );

         // Make a copy for the field check.
         if (!val)
            continue;

         U32 nBufferSize = dStrlen( val ) + 1;
         FrameTemp<char> valCopy( nBufferSize );
         dStrcpy( (char *)valCopy, val );

         if (!writeField(fieldName, valCopy))
            continue;

         val = valCopy;

         U32 expandedBufferSize = ( nBufferSize  * 2 ) + 32;
         FrameTemp<char> expandedBuffer( expandedBufferSize );
         if(f->elementCount == 1)
            dSprintf(expandedBuffer, expandedBufferSize, "%s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, expandedBufferSize, "%s[%d] = \"", f->pFieldname, j);

         // detect and collapse relative path information
         char fnBuf[1024];
         if (f->type == TypeFilename)
         {
            Con::collapsePath(fnBuf, 1024, val);
            val = fnBuf;
         }

         expandEscape((char*)expandedBuffer + dStrlen(expandedBuffer), val);
         dStrcat(expandedBuffer, "\";\r\n");

         stream.writeTabs(tabStop);
         stream.write(dStrlen(expandedBuffer),expandedBuffer);
      }
   }    
   if(mFieldDictionary && mCanSaveFieldDictionary)
      mFieldDictionary->writeFields(this, stream, tabStop);
}

void SimObject::write(Stream &stream, U32 tabStop, U32 flags)
{
   // Only output selected objects if they want that.
   if((flags & SelectedOnly) && !isSelected())
      return;

   stream.writeTabs(tabStop);
   char buffer[1024];
   dSprintf(buffer, sizeof(buffer), "new %s(%s) {\r\n", getClassName(), getName() ? getName() : "");
   stream.write(dStrlen(buffer), buffer);
   writeFields(stream, tabStop + 1);
   stream.writeTabs(tabStop);
   stream.write(4, "};\r\n");
}

bool SimObject::save(const char* pcFileName, bool bOnlySelected)
{
   static const char *beginMessage = "//--- OBJECT WRITE BEGIN ---";
   static const char *endMessage = "//--- OBJECT WRITE END ---";
   FileStream stream;
   FileObject f;
   f.readMemory(pcFileName);

   // check for flags <selected, ...>
   U32 writeFlags = 0;
   if(bOnlySelected)
      writeFlags |= SimObject::SelectedOnly;

   if(!ResourceManager->openFileForWrite(stream, pcFileName)) 
      return false;

   char docRoot[256];
   char modRoot[256];

   dStrcpy(docRoot, pcFileName);
   char *p = dStrrchr(docRoot, '/');
   if (p) *++p = '\0';
   else  docRoot[0] = '\0';

   dStrcpy(modRoot, pcFileName);
   p = dStrchr(modRoot, '/');
   if (p) *++p = '\0';
   else  modRoot[0] = '\0';

   Con::setVariable("$DocRoot", docRoot);
   Con::setVariable("$ModRoot", modRoot);

   const char *buffer;
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      if(!dStrcmp(buffer, beginMessage))
         break;
      stream.write(dStrlen(buffer), buffer);
      stream.write(2, "\r\n");
   }
   stream.write(dStrlen(beginMessage), beginMessage);
   stream.write(2, "\r\n");
   write(stream, 0, writeFlags);
   stream.write(dStrlen(endMessage), endMessage);
   stream.write(2, "\r\n");
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      if(!dStrcmp(buffer, endMessage))
         break;
   }
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      stream.write(dStrlen(buffer), buffer);
      stream.write(2, "\r\n");
   }

   Con::setVariable("$DocRoot", NULL);
   Con::setVariable("$ModRoot", NULL);

   return true;

}

void SimObject::setInternalName(const char* newname)
{
   if(newname)
      mInternalName = StringTable->insert(newname);
}

ConsoleMethod( SimObject, getInternalName, const char*, 2, 2, "getInternalName returns the objects internal name")
{
   return object->getInternalName();
}

StringTableEntry SimObject::getInternalName()
{
   return mInternalName;
}


ConsoleMethod(SimObject, dumpClassHierarchy, void, 2, 2, "obj.dumpClassHierarchy()")
{
   object->dumpClassHierarchy();
}

void  SimObject::dumpClassHierarchy()
{
   AbstractClassRep* pRep = getClassRep();
   while(pRep)
   {
      Con::warnf("%s ->", pRep->getClassName());
      pRep	=	pRep->getParentClass();
   }
}

ConsoleMethod(SimObject, isMemberOfClass, bool, 3, 3, " isMemberOfClass(string classname) -- returns true if this object is a member of the specified class")
{

   AbstractClassRep* pRep = object->getClassRep();
   while(pRep)
   {
      if(!dStricmp(pRep->getClassName(), argv[2]))
      {
         //matches
         return true;
      }

      pRep	=	pRep->getParentClass();
   }

   return false;
}

ConsoleMethod(SimObject, save, bool, 3, 4, "obj.save(fileName, [selectedOnly])")
{
   bool bSelectedOnly	=	false;
   if(argc > 3)
      bSelectedOnly	= dAtob(argv[3]);

   const char* filename = NULL;

   filename = argv[2];

   if(filename == NULL || *filename == 0)
      return false;

   return object->save(filename, bSelectedOnly);

}

ConsoleMethod(SimObject, setName, void, 3, 3, "obj.setName(newName)")
{
   object->assignName(argv[2]);
}

ConsoleMethod(SimObject, getName, const char *, 2, 2, "obj.getName()")
{
   const char *ret = object->getName();
   return ret ? ret : "";
}

ConsoleMethod(SimObject, getClassName, const char *, 2, 2, "obj.getClassName()")
{
   const char *ret = object->getClassName();
   return ret ? ret : "";
}

ConsoleMethod(SimObject, getFieldValue, const char *, 3, 3, "obj.getFieldValue(fieldName);")
{
   const char *fieldName = StringTable->insert( argv[2] );
   return object->getDataField( fieldName, NULL );
}


ConsoleMethod(SimObject, setFieldValue, bool, 4, 4, "obj.setFieldValue(fieldName,value);")
{
   const char *fieldName = StringTable->insert(argv[2]);
   const char *value = argv[3];

   object->setDataField( fieldName, NULL, value );

   return true;

}

ConsoleMethod(SimObject, getFieldType, const char *, 3, 3, "obj.getFieldType(fieldName);")
{
   const char *fieldName = StringTable->insert( argv[2] );
   U32 typeID = object->getDataFieldType( fieldName, NULL );
   ConsoleBaseType* type = ConsoleBaseType::getType( typeID );
   
   if( type )
      return type->getTypeClassName();

   return "";
}

ConsoleMethod( SimObject, call, const char*, 2, 0, "( %args ) - Dynamically call a method on an object." )
{
   argv[1] = argv[2];
   return Con::execute( object, argc - 1, argv + 1 );
}

//-----------------------------------------------------------------------------
//	Set the internal name, can be used to find child objects
//	in a meaningful way, usually from script, while keeping
//	common script functionality together using the controls "Name" field.
//-----------------------------------------------------------------------------
ConsoleMethod( SimObject, setInternalName, void, 3, 3, "string InternalName")
{
   object->setInternalName(argv[2]);
}

ConsoleMethod(SimObject, getId, S32, 2, 2, "() Use the getId method to get the numeric ID of this shape.\n"
                                                                "@return Returns the unique numeric ID of this shape.\n"
                                                                "@sa getName, setName")
{
   return object->getId();
}

ConsoleMethod(SimObject, getGroup, S32, 2, 2, "() Use the getGroup method to determine if this object is contained in a SimGroup and if so, which one.\n"
                                                                "@return Returns the ID of the SimGroup this shape is in or zero if the shape is not contained in a SimGroup")
{
   SimGroup *grp = object->getGroup();
   if(!grp)
      return -1;
   return grp->getId();
}

ConsoleMethod(SimObject, delete, void, 2, 2,"() Use the delete method to delete this object.\n"
                                                                "When an object is deleted, it automatically: Unregisters its ID and name (if it has one) with the engine. Removes itself from any SimGroup or SimSet it may be a member of. (eventually) returns the memory associated with itself and its non-dynamic members. Cancels all pending %obj.schedule() events. For objects in the GameBase, ScriptObject, or GUIControl hierarchies, an object will first: Call the onRemove() method for the object's namespace\n"
                                                                "@return No return value.")
{
   object->deleteObject();
}

ConsoleMethod(SimObject,schedule, S32, 4, 0, "(time , command , <arg1 ... argN> ) Use the schedule method to schedule an action to be executed upon this object time milliseconds in the future.\n"
                                                                "The major difference between this and the schedule console function is that if this object is deleted prior to the scheduled event, the event is automatically canceled. times should not be treated as exact since some 'simulation delay' is to be expected. The minimum resolution for a scheduled event is \"32 ms, or one tick. schedule does not validate the existence of command. i.e. If you pass an invalid console method name, the schedule() method will still return a schedule ID, but the subsequent event will fail silently.\n"
                                                                "@param time Time in milliseconds till action is scheduled to occur.\n"
                                                                "@param command Name of the command to execute. This command must be scoped to this object (i.e. It must exist in the namespace of the object), otherwise the schedule call will fail.\n"
                                                                "@param arg1...argN These are optional arguments which will be passed to command. This version of schedule automatically passes the ID of %obj as arg0 to command.\n"
                                                                "@return Returns an integer schedule ID.\n"
                                                                "@sa See the schedule console function and its corresponding helper functions.")
{
   U32 timeDelta = U32(dAtof(argv[2]));
   argv[2] = argv[3];
   argv[3] = argv[1];
   SimConsoleEvent *evt = new SimConsoleEvent(argc - 2, argv + 2, true);
   S32 ret = Sim::postEvent(object, evt, Sim::getCurrentTime() + timeDelta);
// #ifdef DEBUG
//    Con::printf("obj %s schedule(%s) = %d", argv[3], argv[2], ret);
//    Con::executef(1, "backtrace");
// #endif
   return ret;
}

static S32 QSORT_CALLBACK compareFields(const void* a,const void* b)
{
   const AbstractClassRep::Field* fa = *((const AbstractClassRep::Field**)a);
   const AbstractClassRep::Field* fb = *((const AbstractClassRep::Field**)b);

   return dStricmp(fa->pFieldname, fb->pFieldname);
}

ConsoleMethod(SimObject, getDynamicFieldCount, S32, 2, 2, "obj.getDynamicFieldCount()")
{
   S32 count = 0;
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   for (SimFieldDictionaryIterator itr(fieldDictionary); *itr; ++itr)
      count++;

   return count;
}

ConsoleMethod(SimObject, getDynamicField, const char*, 3, 3, "obj.getDynamicField(index)")
{
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   SimFieldDictionaryIterator itr(fieldDictionary);
   S32 index = dAtoi(argv[2]);
   for (S32 i = 0; i < index; i++)
   {
      if (!(*itr))
      {
         Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
         return NULL;
      }
      ++itr;
   }

   char* buffer = Con::getReturnBuffer(256);
   if (*itr)
   {
      SimFieldDictionary::Entry* entry = *itr;
      dSprintf(buffer, 256, "%s", entry->slotName);
      return buffer;
   }

   Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
   return NULL;
}

ConsoleMethod( SimObject, getFieldCount, S32, 2, 2, "() - Gets the number of persistent fields on the object." )
{
   const AbstractClassRep::FieldList &list = object->getFieldList();
   const AbstractClassRep::Field* f;
   U32 numDummyEntries = 0;
   for(int i = 0; i < list.size(); i++)
   {
      f = &list[i];

      if( f->type == AbstractClassRep::DepricatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType )
      {
         numDummyEntries++;
      }
   }

   return list.size() - numDummyEntries;
}

ConsoleMethod( SimObject, getField, const char*, 3, 3, "(int index) - Gets the name of the field at the given index." )
{
   S32 index = dAtoi( argv[2] );
   const AbstractClassRep::FieldList &list = object->getFieldList();
   if( ( index < 0 ) || ( index >= list.size() ) )
      return "";

   const AbstractClassRep::Field* f;
   S32 currentField = 0;
   for(int i = 0; i < list.size() && currentField <= index; i++)
   {
      f = &list[i];
      
      // skip any dummy fields   
      if(f->type == AbstractClassRep::DepricatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType)
      {
         continue;
      }

      if(currentField == index)
         return f->pFieldname;
      
      currentField++;
   }  

   // if we found nada, return nada.
   return "";
}

ConsoleMethod(SimObject,dump, void, 2, 2, "() Use the dump method to display the following information about this object: All engine and script registered console methods (including parent methods) for this object, all Non-Dynamic Fields AND all Dynamic Fields\n"
                                                                "@return No return value")
{
   const AbstractClassRep::FieldList &list = object->getFieldList();
   char expandedBuffer[4096];

   Con::printf("Member Fields:");
   Vector<const AbstractClassRep::Field *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < (U32)list.size(); i++)
      flist.push_back(&list[i]);

   dQsort(flist.address(),flist.size(),sizeof(AbstractClassRep::Field *),compareFields);

   for(Vector<const AbstractClassRep::Field *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      const AbstractClassRep::Field* f = *itr;
      if( f->type == AbstractClassRep::DepricatedFieldType ||
          f->type == AbstractClassRep::StartGroupFieldType ||
          f->type == AbstractClassRep::EndGroupFieldType) continue;

      for(U32 j = 0; S32(j) < f->elementCount; j++)
      {
         // [neo, 07/05/2007 - #3000]
         // Some objects use dummy vars and projected fields so make sure we call the get functions 
         //const char *val = Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag);                          
         const char *val = (*f->getDataFn)( object, Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag) );// + typeSizes[fld.type] * array1));

         if(!val /*|| !*val*/)
            continue;
         if(f->elementCount == 1)
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s[%d] = \"", f->pFieldname, j);
         expandEscape(expandedBuffer + dStrlen(expandedBuffer), val);
         Con::printf("%s\"", expandedBuffer);
      }
   }

   Con::printf("Tagged Fields:");
   if(object->getFieldDictionary())
      object->getFieldDictionary()->printFields(object);

   Con::printf("Methods:");
   Namespace *ns = object->getNamespace();
   Vector<Namespace::Entry *> vec(__FILE__, __LINE__);

   if(ns)
      ns->getEntryList(&vec);

   for(Vector<Namespace::Entry *>::iterator j = vec.begin(); j != vec.end(); j++)
      Con::printf("  %s() - %s", (*j)->mFunctionName, (*j)->mUsage ? (*j)->mUsage : "");

}



ConsoleMethod(SimObject, getType, S32, 2, 2, "() Use the getType method to get the type for this object. This type is an integer value composed of bitmasks. For simplicity, these bitmasks are defined in the engine and exposed for our use as global variables.\n"
                                                                "To simplify the writing of scripts, a set of globals has been provided containing the bit setting for each class corresponding to a particular type.\n"
              "@return Returns a bit mask containing one or more set bits.\n"
                                                                "@sa getClassName")
{
   return((S32)object->getType());
}

bool SimObject::isMethod( const char* methodName )
{
   if( !methodName || !methodName[0] )
      return false;

   StringTableEntry stname = StringTable->insert( methodName );

   if( getNamespace() )
      return ( getNamespace()->lookup( stname ) != NULL );

   return false;
}

ConsoleMethod(SimObject, isMethod, bool, 3, 3, "obj.isMethod(string method name)")
{
   return object->isMethod( argv[2] );
}


const char *SimObject::tabComplete(const char *prevText, S32 baseLen, bool fForward)
{
   return mNameSpace->tabComplete(prevText, baseLen, fForward);
}

//-----------------------------------------------------------------------------

void SimObject::setDataField(StringTableEntry slotName, const char *array, const char *value)
{
   // first search the static fields if enabled
   if(mFlags.test(ModStaticFields))
   {
      const AbstractClassRep::Field *fld = findField(slotName);
      if(fld)
      {
         if( fld->type == AbstractClassRep::DepricatedFieldType ||
            fld->type == AbstractClassRep::StartGroupFieldType ||
            fld->type == AbstractClassRep::EndGroupFieldType) 
            return;

         S32 array1 = array ? dAtoi(array) : 0;

         if(array1 >= 0 && array1 < fld->elementCount && fld->elementCount >= 1)
         {
            // If the set data notify callback returns true, then go ahead and
            // set the data, otherwise, assume the set notify callback has either
            // already set the data, or has deemed that the data should not
            // be set at all.
            FrameTemp<char> buffer(2048);
            FrameTemp<char> bufferSecure(2048); // This buffer is used to make a copy of the data 
            // so that if the prep functions or any other functions use the string stack, the data
            // is not corrupted.

            ConsoleBaseType *cbt = ConsoleBaseType::getType( fld->type );
            AssertFatal( cbt != NULL, "Could not resolve Type Id." );

            const char* szBuffer = cbt->prepData( value, buffer, 2048 );
            dMemset( bufferSecure, 0, 2048 );
            dMemcpy( bufferSecure, szBuffer, dStrlen( szBuffer ) );

            if( (*fld->setDataFn)( this, bufferSecure ) )
               Con::setData(fld->type, (void *) (((const char *)this) + fld->offset), array1, 1, &value, fld->table);

            onStaticModified( slotName, value );

            return;
         }

         if(fld->validator)
            fld->validator->validateType(this, (void *) (((const char *)this) + fld->offset));

         onStaticModified( slotName, value );
         return;
      }
   }

   if(mFlags.test(ModDynamicFields))
   {
      if(!mFieldDictionary)
         mFieldDictionary = new SimFieldDictionary;

      if(!array)
         mFieldDictionary->setFieldValue(slotName, value);
      else
      {
         char buf[256];
         dStrcpy(buf, slotName);
         dStrcat(buf, array);
         mFieldDictionary->setFieldValue(StringTable->insert(buf), value);
      }
   }
}

//-----------------------------------------------------------------------------

const char *SimObject::getDataField(StringTableEntry slotName, const char *array)
{
   if(mFlags.test(ModStaticFields))
   {
      S32 array1 = array ? dAtoi(array) : -1;
      const AbstractClassRep::Field *fld = findField(slotName);
   
      if(fld)
      {
         if(array1 == -1 && fld->elementCount == 1)
            return (*fld->getDataFn)( this, Con::getData(fld->type, (void *) (((const char *)this) + fld->offset), 0, fld->table, fld->flag) );
         if(array1 >= 0 && array1 < fld->elementCount)
            return (*fld->getDataFn)( this, Con::getData(fld->type, (void *) (((const char *)this) + fld->offset), array1, fld->table, fld->flag) );// + typeSizes[fld.type] * array1));
         return "";
      }
   }

   if(mFlags.test(ModDynamicFields))
   {
      if(!mFieldDictionary)
         return "";

      if(!array) 
      {
         if (const char* val = mFieldDictionary->getFieldValue(slotName))
            return val;
      }
      else
      {
         static char buf[256];
         dStrcpy(buf, slotName);
         dStrcat(buf, array);
         if (const char* val = mFieldDictionary->getFieldValue(StringTable->insert(buf)))
            return val;
      }
   }

   return "";
}

//-----------------------------------------------------------------------------

const char *SimObject::getPrefixedDataField(StringTableEntry fieldName, const char *array)
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot get field value with NULL field name." );

    // Fetch field value.
    const char* pFieldValue = getDataField( fieldName, array );

    // Sanity.
    AssertFatal( pFieldValue != NULL, "Field value cannot be NULL." );

    // Return without the prefix if there's no value.
    if ( *pFieldValue == 0 )
        return StringTable->EmptyString;

    // Fetch the field prefix.
    StringTableEntry fieldPrefix = getDataFieldPrefix( fieldName );

    // Sanity!
    AssertFatal( fieldPrefix != NULL, "Field prefix cannot be NULL." );

    // Calculate a buffer size including prefix.
    const U32 valueBufferSize = dStrlen(fieldPrefix) + dStrlen(pFieldValue) + 1;

    // Fetch a buffer.
    char* pValueBuffer = Con::getReturnBuffer( valueBufferSize );

    // Format the value buffer.
    dSprintf( pValueBuffer, valueBufferSize, "%s%s", fieldPrefix, pFieldValue );

    return pValueBuffer;
}

//-----------------------------------------------------------------------------

void SimObject::setPrefixedDataField(StringTableEntry fieldName, const char *array, const char *value)
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot set object field value with NULL field name." );
    AssertFatal( value != NULL, "Field value cannot be NULL." );

    // Set value without prefix if there's no value.
    if ( *value == 0 )
    {
        setDataField( fieldName, NULL, value );
        return;
    }

    // Fetch the field prefix.
    StringTableEntry fieldPrefix = getDataFieldPrefix( fieldName );

    // Sanity.
    AssertFatal( fieldPrefix != NULL, "Field prefix cannot be NULL." );

    // Do we have a field prefix?
    if ( fieldPrefix == StringTable->EmptyString )
    {
        // No, so set the data field in the usual way.
        setDataField( fieldName, NULL, value );
        return;
    }

    // Yes, so fetch the length of the field prefix.
    const U32 fieldPrefixLength = dStrlen(fieldPrefix);

    // Yes, so does it start with the object field prefix?
    if ( dStrnicmp( value, fieldPrefix, fieldPrefixLength ) != 0 )
    {
        // No, so set the data field in the usual way.
        setDataField( fieldName, NULL, value );
        return;
    }

    // Yes, so set the data excluding the prefix.
    setDataField( fieldName, NULL, value + fieldPrefixLength );
}

//-----------------------------------------------------------------------------

const char *SimObject::getPrefixedDynamicDataField(StringTableEntry fieldName, const char *array, const S32 fieldType )
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot get field value with NULL field name." );

    // Fetch field value.
    const char* pFieldValue = getDataField( fieldName, array );

    // Sanity.
    AssertFatal( pFieldValue != NULL, "Field value cannot be NULL." );

    // Return the field if no field type is specified.
    if ( fieldType == -1 )
        return pFieldValue;

    // Return without the prefix if there's no value.
    if ( *pFieldValue == 0 )
        return StringTable->EmptyString;

    // Fetch the console base type.
    ConsoleBaseType* pConsoleBaseType = ConsoleBaseType::getType( fieldType );

    // Did we find the console base type?
    if ( pConsoleBaseType == NULL )
    {
        // No, so warn.
        Con::warnf("getPrefixedDynamicDataField() - Invalid field type '%d' specified for field '%s' with value '%s'.",
            fieldType, fieldName, pFieldValue );
    }

    // Fetch the field prefix.
    StringTableEntry fieldPrefix = pConsoleBaseType->getTypePrefix();

    // Sanity!
    AssertFatal( fieldPrefix != NULL, "Field prefix cannot be NULL." );

    // Calculate a buffer size including prefix.
    const U32 valueBufferSize = dStrlen(fieldPrefix) + dStrlen(pFieldValue) + 1;

    // Fetch a buffer.
    char* pValueBuffer = Con::getReturnBuffer( valueBufferSize );

    // Format the value buffer.
    dSprintf( pValueBuffer, valueBufferSize, "%s%s", fieldPrefix, pFieldValue );

    return pValueBuffer;
}

//-----------------------------------------------------------------------------

void SimObject::setPrefixedDynamicDataField(StringTableEntry fieldName, const char *array, const char *value, const S32 fieldType )
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot set object field value with NULL field name." );
    AssertFatal( value != NULL, "Field value cannot be NULL." );

    // Set value without prefix if no field type was specified.
    if ( fieldType == -1 )
    {
        setDataField( fieldName, NULL, value );
        return;
    }

    // Fetch the console base type.
    ConsoleBaseType* pConsoleBaseType = ConsoleBaseType::getType( fieldType );

    // Did we find the console base type?
    if ( pConsoleBaseType == NULL )
    {
        // No, so warn.
        Con::warnf("setPrefixedDynamicDataField() - Invalid field type '%d' specified for field '%s' with value '%s'.",
            fieldType, fieldName, value );
    }

    // Set value without prefix if there's no value or we didn't find the console base type.
    if ( *value == 0 || pConsoleBaseType == NULL )
    {
        setDataField( fieldName, NULL, value );
        return;
    }

    // Fetch the field prefix.
    StringTableEntry fieldPrefix = pConsoleBaseType->getTypePrefix();

    // Sanity.
    AssertFatal( fieldPrefix != NULL, "Field prefix cannot be NULL." );

    // Do we have a field prefix?
    if ( fieldPrefix == StringTable->EmptyString )
    {
        // No, so set the data field in the usual way.
        setDataField( fieldName, NULL, value );
        return;
    }

    // Yes, so fetch the length of the field prefix.
    const U32 fieldPrefixLength = dStrlen(fieldPrefix);

    // Yes, so does it start with the object field prefix?
    if ( dStrnicmp( value, fieldPrefix, fieldPrefixLength ) != 0 )
    {
        // No, so set the data field in the usual way.
        setDataField( fieldName, NULL, value );
        return;
    }

    // Yes, so set the data excluding the prefix.
    setDataField( fieldName, NULL, value + fieldPrefixLength );
}

//-----------------------------------------------------------------------------

StringTableEntry SimObject::getDataFieldPrefix( StringTableEntry fieldName )
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot get field prefix with NULL field name." );

    // Find the field.
    const AbstractClassRep::Field* pField = findField( fieldName );

    // Return nothing if field was not found.
    if ( pField == NULL )
        return StringTable->EmptyString;

    // Yes, so fetch the console base type.
    ConsoleBaseType* pConsoleBaseType = ConsoleBaseType::getType( pField->type );

    // Fetch the type prefix.
    return pConsoleBaseType->getTypePrefix();
}

//-----------------------------------------------------------------------------

U32 SimObject::getDataFieldType( StringTableEntry slotName, const char* array )
{
   const AbstractClassRep::Field* field = findField( slotName );
   if( field )
      return field->type;

   return 0;
}

SimObject::~SimObject()
{
   delete mFieldDictionary;

   AssertFatal(nextNameObject == (SimObject*)-1,avar(
                  "SimObject::~SimObject:  Not removed from dictionary: name %s, id %i",
                  objectName, mId));
   AssertFatal(nextManagerNameObject == (SimObject*)-1,avar(
                  "SimObject::~SimObject:  Not removed from manager dictionary: name %s, id %i",
                  objectName,mId));
   AssertFatal(mFlags.test(Added) == 0, "SimObject::object "
               "missing call to SimObject::onRemove");
}

//---------------------------------------------------------------------------

bool SimObject::isLocked()
{
   if(!mFieldDictionary)
      return false;

   const char * val = mFieldDictionary->getFieldValue( StringTable->insert( "locked", false ) );

   return( val ? dAtob(val) : false );
}

void SimObject::setLocked( bool b = true )
{
   setDataField(StringTable->insert("locked", false), NULL, b ? "true" : "false" );
}

bool SimObject::isHidden()
{
   if(!mFieldDictionary)
      return false;

   const char * val = mFieldDictionary->getFieldValue( StringTable->insert( "hidden", false ) );
   return( val ? dAtob(val) : false );
}

void SimObject::setHidden(bool b = true)
{
   setDataField(StringTable->insert("hidden", false), NULL, b ? "true" : "false" );
}

//---------------------------------------------------------------------------

bool SimObject::onAdd()
{
   mFlags.set(Added);

   if(getClassRep())
      mNameSpace = getClassRep()->getNameSpace();

   linkNamespaces();

   // onAdd() should return FALSE if there was an error
   return true;
}

void SimObject::onRemove()
{
   mFlags.clear(Added);

   unlinkNamespaces();
}

void SimObject::onGroupAdd()
{
}

void SimObject::onGroupRemove()
{
}

void SimObject::onDeleteNotify(SimObject*)
{
}

void SimObject::onNameChange(const char*)
{
}

void SimObject::onStaticModified(const char* slotName, const char* newValue)
{
}

bool SimObject::processArguments(S32 argc, const char**)
{
   return argc == 0;
}

bool SimObject::isChildOfGroup(SimGroup* pGroup)
{
   if(!pGroup)
      return false;

   //if we *are* the group in question,
   //return true:
   if(pGroup == dynamic_cast<SimGroup*>(this))
      return true;

   SimGroup* temp	=	mGroup;
   while(temp)
   {
      if(temp == pGroup)
         return true;
      temp = temp->mGroup;
   }

   return false;
}


ConsoleMethod(SimObject, isChildOfGroup, bool, 3,3," returns true, if we are in the specified simgroup - or a subgroup thereof")
{
   SimGroup* pGroup = dynamic_cast<SimGroup*>(Sim::findObject(dAtoi(argv[2])));
   if(pGroup)
   {
      return object->isChildOfGroup(pGroup);
   }

   return false;
}
//---------------------------------------------------------------------------

static Chunker<SimObject::Notify> notifyChunker(128000);
SimObject::Notify *SimObject::mNotifyFreeList = NULL;

SimObject::Notify *SimObject::allocNotify()
{
   if(mNotifyFreeList)
   {
      SimObject::Notify *ret = mNotifyFreeList;
      mNotifyFreeList = ret->next;
      return ret;
   }
   return notifyChunker.alloc();
}

void SimObject::freeNotify(SimObject::Notify* note)
{
   AssertFatal(note->type != SimObject::Notify::Invalid, "Invalid notify");
   note->type = SimObject::Notify::Invalid;
   note->next = mNotifyFreeList;
   mNotifyFreeList = note;
}

//------------------------------------------------------------------------------

SimObject::Notify* SimObject::removeNotify(void *ptr, SimObject::Notify::Type type)
{
   Notify **list = &mNotifyList;
   while(*list)
   {
      if((*list)->ptr == ptr && (*list)->type == type)
      {
         SimObject::Notify *ret = *list;
         *list = ret->next;
         return ret;
      }
      list = &((*list)->next);
   }
   return NULL;
}

void SimObject::deleteNotify(SimObject* obj)
{
   AssertFatal(!obj->isDeleted(),
               "SimManager::deleteNotify: Object is being deleted");
   Notify *note = allocNotify();
   note->ptr = (void *) this;
   note->next = obj->mNotifyList;
   note->type = Notify::DeleteNotify;
   obj->mNotifyList = note;

   note = allocNotify();
   note->ptr = (void *) obj;
   note->next = mNotifyList;
   note->type = Notify::ClearNotify;
   mNotifyList = note;

   //obj->deleteNotifyList.pushBack(this);
   //clearNotifyList.pushBack(obj);
}

void SimObject::registerReference(SimObject **ptr)
{
   Notify *note = allocNotify();
   note->ptr = (void *) ptr;
   note->next = mNotifyList;
   note->type = Notify::ObjectRef;
   mNotifyList = note;
}

void SimObject::unregisterReference(SimObject **ptr)
{
   Notify *note = removeNotify((void *) ptr, Notify::ObjectRef);
   if(note)
      freeNotify(note);
}

void SimObject::clearNotify(SimObject* obj)
{
   Notify *note = obj->removeNotify((void *) this, Notify::DeleteNotify);
   if(note)
      freeNotify(note);

   note = removeNotify((void *) obj, Notify::ClearNotify);
   if(note)
      freeNotify(note);
}

void SimObject::processDeleteNotifies()
{
   // clear out any delete notifies and
   // object refs.

   while(mNotifyList)
   {
      Notify *note = mNotifyList;
      mNotifyList = note->next;

      AssertFatal(note->type != Notify::ClearNotify, "Clear notes should be all gone.");

      if(note->type == Notify::DeleteNotify)
      {
         SimObject *obj = (SimObject *) note->ptr;
         Notify *cnote = obj->removeNotify((void *)this, Notify::ClearNotify);
         obj->onDeleteNotify(this);
         freeNotify(cnote);
      }
      else
      {
         // it must be an object ref - a pointer refs this object
         *((SimObject **) note->ptr) = NULL;
      }
      freeNotify(note);
   }
}

void SimObject::clearAllNotifications()
{
   for(Notify **cnote = &mNotifyList; *cnote; )
   {
      Notify *temp = *cnote;
      if(temp->type == Notify::ClearNotify)
      {
         *cnote = temp->next;
         Notify *note = ((SimObject *) temp->ptr)->removeNotify((void *) this, Notify::DeleteNotify);
         freeNotify(temp);
         freeNotify(note);
      }
      else
         cnote = &(temp->next);
   }
}

//---------------------------------------------------------------------------

void SimObject::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("SimBase");
   addField("canSaveDynamicFields",		TypeBool,		Offset(mCanSaveFieldDictionary, SimObject), &writeCanSaveDynamicFields, "");
   addField("internalName",            TypeString,       Offset(mInternalName, SimObject), &writeInternalName, "");   
   addProtectedField("parentGroup",        TypeSimObjectPtr, Offset(mGroup, SimObject), &setParentGroup, &defaultProtectedGetFn, &writeParentGroup, "Group hierarchy parent of the object." );
   endGroup("SimBase");

   // Namespace Linking.
   addGroup("Namespace Linking");
   addProtectedField("superclass", TypeString, Offset(mSuperClassName, SimObject), &setSuperClass, &defaultProtectedGetFn, &writeSuperclass, "Script Class of object.");
   addProtectedField("class",      TypeString, Offset(mClassName,      SimObject), &setClass,      &defaultProtectedGetFn, &writeClass, "Script SuperClass of object.");
   endGroup("Namespace Linking");
}

//-----------------------------------------------------------------------------

SimObject* SimObject::clone( const bool copyDynamicFields )
{
    // Craete cloned object.
    SimObject* pCloneObject = dynamic_cast<SimObject*>( ConsoleObject::create(getClassName()) );
    if (!pCloneObject)
    {
        Con::errorf("SimObject::clone() - Unable to create cloned object.");
        return NULL;
    }

    // Register object.
    if ( !pCloneObject->registerObject() )
    {
        Con::warnf("SimObject::clone() - Unable to register cloned object.");
        delete pCloneObject;
        return NULL;
    }

    // Copy object.
    copyTo( pCloneObject );

    // Copy over dynamic fields if requested.
    if ( copyDynamicFields )
        pCloneObject->assignDynamicFieldsFrom( this );

    return pCloneObject;
}


//-----------------------------------------------------------------------------

ConsoleMethod(SimObject, clone, S32, 2, 3,      "([bool copyDynamicFields? = false]) - Clones the object.\n"
                                                "@param copyDynamicFields Whether the dynamic fields should be copied to the cloned object or not.  Optional: Defaults to false.\n"
                                                "@return (newObjectID) The newly cloned object's id if successful, otherwise a 0.")
{
    // Fetch copy dynamic fields flag.
    const bool copyDynamicFields = ( argc >= 3 ) ? dAtob( argv[2] ) : false;

    // Clone Object.
    SimObject* pClonedObject = object->clone( copyDynamicFields );

    // Finish if object was not cloned.
    if ( pClonedObject == NULL )
        return 0;

    return pClonedObject->getId();
}

//-----------------------------------------------------------------------------

void SimObject::copyTo(SimObject* object)
{
   object->mClassName = mClassName;
   object->mSuperClassName = mSuperClassName;

   object->linkNamespaces();
}

//-----------------------------------------------------------------------------

bool SimObject::setParentGroup(void* obj, const char* data)
{
   SimGroup *parent = NULL;
   SimObject *object = static_cast<SimObject*>(obj);

   if(Sim::findObject(data, parent))
      parent->addObject(object);
   
   // always return false, because we've set mGroup when we called addObject
   return false;
}


bool SimObject::addToSet(SimObjectId spid)
{
   if (mFlags.test(Added) == false)
      return false;

   SimObject* ptr = Sim::findObject(spid);
   if (ptr) 
   {
      SimSet* sp = dynamic_cast<SimSet*>(ptr);
      AssertFatal(sp != 0,
                  "SimObject::addToSet: "
                  "ObjectId does not refer to a set object");
      if (sp)
      {
         sp->addObject(this);
         return true;
      }
   }
   return false;
}

bool SimObject::addToSet(const char *ObjectName)
{
   if (mFlags.test(Added) == false)
      return false;

   SimObject* ptr = Sim::findObject(ObjectName);
   if (ptr) 
   {
      SimSet* sp = dynamic_cast<SimSet*>(ptr);
      AssertFatal(sp != 0,
                  "SimObject::addToSet: "
                  "ObjectName does not refer to a set object");
      if (sp)
      {
         sp->addObject(this);
         return true;
      }
   }
   return false;
}

bool SimObject::removeFromSet(SimObjectId sid)
{
   if (mFlags.test(Added) == false)
      return false;

   SimSet *set;
   if(Sim::findObject(sid, set))
   {
      set->removeObject(this);
      return true;
   }
   return false;
}

bool SimObject::removeFromSet(const char *objectName)
{
   if (mFlags.test(Added) == false)
      return false;

   SimSet *set;
   if(Sim::findObject(objectName, set))
   {
      set->removeObject(this);
      return true;
   }
   return false;
}

void SimObject::inspectPreApply()
{
}

void SimObject::inspectPostApply()
{
}

void SimObject::linkNamespaces()
{
   if( mNameSpace )
      unlinkNamespaces();
   
   StringTableEntry parent = StringTable->insert( getClassName() );
   if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
   {
      if( Con::linkNamespaces( parent, mSuperClassName ) )
         parent = mSuperClassName;
      else
         mSuperClassName = StringTable->EmptyString; // CodeReview Is this behavior that we want?
                                                      // CodeReview This will result in the mSuperClassName variable getting hosed
                                                      // CodeReview if Con::linkNamespaces returns false. Looking at the code for
                                                      // CodeReview Con::linkNamespaces, and the call it makes to classLinkTo, it seems
                                                      // CodeReview like this would only fail if it had bogus data to begin with, but
                                                      // CodeReview I wanted to note this behavior which occurs in some implementations
                                                      // CodeReview but not all. -patw
   }

   // ClassName -> SuperClassName
   if ( ( mNSLinkMask & LinkClassName ) && mClassName && mClassName[0] )
   {
      if( Con::linkNamespaces( parent, mClassName ) )
         parent = mClassName;
      else
         mClassName = StringTable->EmptyString; // CodeReview (See previous note on this code)
   }

   // ObjectName -> ClassName
   StringTableEntry objectName = getName();
   if( objectName && objectName[0] )
   {
      if( Con::linkNamespaces( parent, objectName ) )
         parent = objectName;
   }

   // Store our namespace.
   mNameSpace = Con::lookupNamespace( parent );
}

void SimObject::unlinkNamespaces()
{
   if (!mNameSpace)
      return;

   // Restore NameSpace's
   StringTableEntry child = getName();
   if( child && child[0] )
   {
      if( ( mNSLinkMask & LinkClassName ) && mClassName && mClassName[0])
      {
         if( Con::unlinkNamespaces( mClassName, child ) )
            child = mClassName;
      }

      if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
      {
         if( Con::unlinkNamespaces( mSuperClassName, child ) )
            child = mSuperClassName;
      }

      Con::unlinkNamespaces( getClassName(), child );
   }
   else
   {
      child = mClassName;
      if( child && child[0] )
      {
         if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
         {
            if( Con::unlinkNamespaces( mSuperClassName, child ) )
               child = mSuperClassName;
         }

         Con::unlinkNamespaces( getClassName(), child );
      }
      else
      {
         if( ( mNSLinkMask & LinkSuperClassName ) && mSuperClassName && mSuperClassName[0] )
            Con::unlinkNamespaces( getClassName(), mSuperClassName );
      }
   }

   mNameSpace = NULL;
}

void SimObject::setClassNamespace( const char *classNamespace )
{
   mClassName = StringTable->insert( classNamespace );
   linkNamespaces();
}

void SimObject::setSuperClassNamespace( const char *superClassNamespace )
{  
   mSuperClassName = StringTable->insert( superClassNamespace );
   linkNamespaces();
}

ConsoleMethod(SimObject, getClassNamespace, const char*, 2, 2, "")
{
   return object->getClassNamespace();
}

ConsoleMethod(SimObject, getSuperClassNamespace, const char*, 2, 2, "")
{
   return object->getSuperClassNamespace();
}

ConsoleMethod(SimObject, setClassNamespace, void, 2, 3, "")
{
   object->setClassNamespace(argv[2]);
}

ConsoleMethod(SimObject, setSuperClassNamespace, void, 2, 3, "")
{
   object->setSuperClassNamespace(argv[2]);
}

//-----------------------------------------------------------------------------

ConsoleMethod(SimObject, setProgenitorFile, void, 3, 3, "(file) Sets the progenitor file responsible for this instances creation.\n"
                                                        "@param file The progenitor file responsible for this instances creation.\n"
                                                        "@return No return value." )
{
    object->setProgenitorFile( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SimObject, getProgenitorFile, const char*, 2, 2,  "() Gets the progenitor file responsible for this instances creation.\n"
                                                                "@return The progenitor file responsible for this instances creation." )
{
    return object->getProgenitorFile();
}


//-----------------------------------------------------------------------------

ConsoleMethod(SimObject, startTimer, bool, 4, 5,    "(callbackFunction, float timePeriod, [repeat]) - Starts a periodic timer for this object.\n"
                                                    "Sets a timer on the object that, when it expires, will cause the object to execute the onTimer() callback.\n"
                                                    "The timer event will continue to occur at regular intervals until setTimerOff() is called.\n"
                                                    "@param callbackFunction The name of the callback function to call for each timer repetition.\n"
                                                    "@param timePeriod The period of time (in milliseconds) between each callback.\n"
                                                    "@param repeat The number of times the timer should repeat.  If not specified or zero then it will run infinitely\n"
                                                    "@return No return Value.")
{
    // Is the periodic timer running?
    if ( object->getPeriodicTimerID() != 0 )
    {
        // Yes, so cancel it.
        Sim::cancelEvent( object->getPeriodicTimerID() );

        // Reset Timer ID.
        object->setPeriodicTimerID( 0 );
    }

    // Fetch the callback function.
    StringTableEntry callbackFunction = StringTable->insert( argv[2] );

    // Does the function exist?
    if ( !object->isMethod( callbackFunction ) )
    {
        // No, so warn.
        Con::warnf("SimObject::startTimer() - The callback function of '%s' does not exist.", callbackFunction );
        return false;
    }

    // Fetch the time period.
    const S32 timePeriod = dAtoi(argv[3]);

    // Is the time period valid?
    if ( timePeriod < 1 )
    {
        // No, so warn.
        Con::warnf("SimObject::startTimer() - The time period of '%d' is invalid.", timePeriod );
        return false;
    }        

    // Fetch the repeat count.
    const S32 repeat = argc >= 5 ? dAtoi(argv[4]) : 0;

    // Create Timer Event.
    SimObjectTimerEvent* pEvent = new SimObjectTimerEvent( callbackFunction, (U32)timePeriod, (U32)repeat );

    // Post Event.
    object->setPeriodicTimerID( Sim::postEvent( object, pEvent, Sim::getCurrentTime() + timePeriod ) );

    return true;
}

//-----------------------------------------------------------------------------

ConsoleMethod(SimObject, stopTimer, void, 2, 2, "() - Stops the periodic timer for this object.\n"
                                                "@return No return Value.")
{
    // Finish if the periodic timer isn't running.
    if ( object->getPeriodicTimerID() == 0 )
        return;

    // Cancel It.
    Sim::cancelEvent( object->getPeriodicTimerID() );

    // Reset Timer ID.
    object->setPeriodicTimerID( 0 );
}

//-----------------------------------------------------------------------------

ConsoleMethod(SimObject, isTimerActive, bool, 2, 2, "() - Checks whether the periodic timer is active for this object or not.\n"
                                                    "@return Whether the periodic timer is active for this object or not.")
{
    return object->isPeriodicTimerActive();
}