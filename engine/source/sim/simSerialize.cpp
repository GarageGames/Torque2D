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

#include "console/console.h"
#include "sim/simBase.h"
#include "io/bitStream.h"
#include "io/fileStream.h"
#include "io/resource/resourceManager.h"

#include "simSerialize_ScriptBinding.h"

//////////////////////////////////////////////////////////////////////////
// SimObject Methods
//////////////////////////////////////////////////////////////////////////

bool SimObject::writeObject(Stream *stream)
{
   clearFieldFilters();
   buildFilterList();

   stream->writeString(getName() ? getName() : "");

   // Static fields
   AbstractClassRep *rep = getClassRep();
   AbstractClassRep::FieldList &fieldList = rep->mFieldList;
   AbstractClassRep::FieldList::iterator itr;
   
   U32 savePos = stream->getPosition();
   U32 numFields = fieldList.size();
   stream->write(numFields);

   for(itr = fieldList.begin();itr != fieldList.end();itr++)
   {
      if(itr->type >= AbstractClassRep::StartGroupFieldType || isFiltered(itr->pFieldname))
      {
         numFields--;
         continue;
      }

      const char *field = getDataField(itr->pFieldname, NULL);
      if(field == NULL)
         field = "";

      stream->writeString(itr->pFieldname);
      stream->writeString(field);
   }

   // Dynamic Fields
   if(mCanSaveFieldDictionary)
   {
      SimFieldDictionary * fieldDictionary = getFieldDictionary();
      for(SimFieldDictionaryIterator ditr(fieldDictionary); *ditr; ++ditr)
      {
         SimFieldDictionary::Entry * entry = (*ditr);

         if(isFiltered(entry->slotName))
            continue;

         stream->writeString(entry->slotName);
         stream->writeString(entry->value);
         numFields++;
      }
   }

   // Overwrite the number of fields with the correct value
   U32 savePos2 = stream->getPosition();
   stream->setPosition(savePos);
   stream->write(numFields);
   stream->setPosition(savePos2);

   return true;
}

bool SimObject::readObject(Stream *stream)
{
   const char *name = stream->readSTString(true);
   if(name && *name)
      assignName(name);

   U32 numFields;
   stream->read(&numFields);

   for(U32 i = 0;i < numFields;i++)
   {
      const char *fieldName = stream->readSTString();
      const char *data = stream->readSTString();

      setDataField(fieldName, NULL, data);
   }
   return true;
}

//////////////////////////////////////////////////////////////////////////

void SimObject::buildFilterList()
{
   Con::executef(this, 1, "buildFilterList");
}

//////////////////////////////////////////////////////////////////////////

void SimObject::addFieldFilter( const char *fieldName )
{
   StringTableEntry st = StringTable->insert(fieldName);
   for(S32 i = 0;i < mFieldFilter.size();i++)
   {
      if(mFieldFilter[i] == st)
         return;
   }

   mFieldFilter.push_back(st);
}

void SimObject::removeFieldFilter( const char *fieldName )
{
   StringTableEntry st = StringTable->insert(fieldName);
   for(S32 i = 0;i < mFieldFilter.size();i++)
   {
      if(mFieldFilter[i] == st)
      {
         mFieldFilter.erase(i);
         return;
      }
   }
}

void SimObject::clearFieldFilters()
{
   mFieldFilter.clear();
}

//////////////////////////////////////////////////////////////////////////

bool SimObject::isFiltered( const char *fieldName )
{
   StringTableEntry st = StringTable->insert(fieldName);
   for(S32 i = 0;i < mFieldFilter.size();i++)
   {
      if(mFieldFilter[i] == st)
         return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
// SimSet Methods
//////////////////////////////////////////////////////////////////////////

bool SimSet::writeObject( Stream *stream )
{
   if(! Parent::writeObject(stream))
      return false;

   stream->write(size());
   for(SimSet::iterator i = begin();i < end();++i)
   {
      if(! Sim::saveObject((*i), stream))
         return false;
   }
   return true;
}

bool SimSet::readObject( Stream *stream )
{
   if(! Parent::readObject(stream))
      return false;

   U32 numObj;
   stream->read(&numObj);

   for(U32 i = 0;i < numObj;i++)
   {
      SimObject *obj = Sim::loadObjectStream(stream);
      if(obj == NULL)
         return false;

      addObject(obj);
   }

   return true;
}

//////////////////////////////////////////////////////////////////////////
// Sim Functions
//////////////////////////////////////////////////////////////////////////

namespace Sim
{

bool saveObject(SimObject *obj, const char *filename)
{
   FileStream fs;
   if(ResourceManager->openFileForWrite(fs, filename, FileStream::Write))
   {
      bool ret = saveObject(obj, &fs);
      fs.close();

      return ret;
   }
   return false;
}

bool saveObject(SimObject *obj, Stream *stream)
{
   stream->writeString(obj->getClassName());
   return obj->writeObject(stream);
}

//////////////////////////////////////////////////////////////////////////

SimObject *loadObjectStream(const char *filename)
{
   Stream * stream = ResourceManager->openStream(filename);
   
   if (stream)
   {
      SimObject *ret = loadObjectStream(stream);
      ResourceManager->closeStream(stream);
      return ret;
   }

   return NULL;
}

SimObject *loadObjectStream(Stream *stream)
{
   const char *className = stream->readSTString(true);
   ConsoleObject *conObj = ConsoleObject::create(className);
   if(conObj == NULL)
   {
      Con::errorf("Sim::restoreObjectStream - Could not create object of class \"%s\"", className);
      return NULL;
   }

   SimObject *simObj = dynamic_cast<SimObject *>(conObj);
   if(simObj == NULL)
   {
      Con::errorf("Sim::restoreObjectStream - Object of class \"%s\" is not a SimObject", className);
      delete simObj;
      return NULL;
   }

   if(simObj->readObject(stream))
   {
      simObj->registerObject();
      return simObj;
   }

   delete simObj;
   return NULL;
}

} // end namespace Sim
