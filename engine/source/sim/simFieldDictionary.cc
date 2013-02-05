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

#include "sim/simFieldDictionary.h"
#include "memory/dataChunker.h"
#include "console/consoleInternal.h"
#include "memory/frameAllocator.h"

//-----------------------------------------------------------------------------

SimFieldDictionary::Entry *SimFieldDictionary::mFreeList = NULL;

static Chunker<SimFieldDictionary::Entry> fieldChunker;

SimFieldDictionary::Entry *SimFieldDictionary::allocEntry()
{
   if(mFreeList)
   {
      Entry *ret = mFreeList;
      mFreeList = ret->next;
      return ret;
   }
   else
      return fieldChunker.alloc();
}

void SimFieldDictionary::freeEntry(SimFieldDictionary::Entry *ent)
{
   ent->next = mFreeList;
   mFreeList = ent;
}

SimFieldDictionary::SimFieldDictionary()
{
   for(U32 i = 0; i < HashTableSize; i++)
      mHashTable[i] = 0;

   mVersion = 0;
}

SimFieldDictionary::~SimFieldDictionary()
{
   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i]; walk;)
      {
         Entry *temp = walk;
         walk = temp->next;

         dFree(temp->value);
         freeEntry(temp);
      }
   }
}

void SimFieldDictionary::setFieldValue(StringTableEntry slotName, const char *value)
{
   U32 bucket = HashPointer(slotName) % HashTableSize;
   Entry **walk = &mHashTable[bucket];
   while(*walk && (*walk)->slotName != slotName)
      walk = &((*walk)->next);

   Entry *field = *walk;
   if(!*value)
   {
      if(field)
      {
         mVersion++;

         dFree(field->value);
         *walk = field->next;
         freeEntry(field);
      }
   }
   else
   {
      if(field)
      {
         dFree(field->value);
         field->value = dStrdup(value);
      }
      else
      {
         mVersion++;

         field = allocEntry();
         field->value = dStrdup(value);
         field->slotName = slotName;
         field->next = NULL;
         *walk = field;
      }
   }
}

const char *SimFieldDictionary::getFieldValue(StringTableEntry slotName)
{
   U32 bucket = HashPointer(slotName) % HashTableSize;

   for(Entry *walk = mHashTable[bucket];walk;walk = walk->next)
      if(walk->slotName == slotName)
         return walk->value;

   return NULL;
}


void SimFieldDictionary::assignFrom(SimFieldDictionary *dict)
{
   mVersion++;

   for(U32 i = 0; i < HashTableSize; i++)
      for(Entry *walk = dict->mHashTable[i];walk; walk = walk->next)
         setFieldValue(walk->slotName, walk->value);
}

static S32 QSORT_CALLBACK compareEntries(const void* a,const void* b)
{
   SimFieldDictionary::Entry *fa = *((SimFieldDictionary::Entry **)a);
   SimFieldDictionary::Entry *fb = *((SimFieldDictionary::Entry **)b);
   return dStricmp(fa->slotName, fb->slotName);
}

void SimFieldDictionary::writeFields(SimObject *obj, Stream &stream, U32 tabStop)
{

   const AbstractClassRep::FieldList &list = obj->getFieldList();
   Vector<Entry *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i];walk; walk = walk->next)
      {
         // make sure we haven't written this out yet:
         U32 i;
         for(i = 0; i < (U32)list.size(); i++)
            if(list[i].pFieldname == walk->slotName)
               break;

         if(i != list.size())
            continue;


         if (!obj->writeField(walk->slotName, walk->value))
            continue;

         flist.push_back(walk);
      }
   }

   // Sort Entries to prevent version control conflicts
   dQsort(flist.address(),flist.size(),sizeof(Entry *),compareEntries);

   // Save them out
   for(Vector<Entry *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      U32 nBufferSize = (dStrlen( (*itr)->value ) * 2) + dStrlen( (*itr)->slotName ) + 16;
      FrameTemp<char> expandedBuffer( nBufferSize );

      stream.writeTabs(tabStop+1);

      dSprintf(expandedBuffer, nBufferSize, "%s = \"", (*itr)->slotName);
      expandEscape((char*)expandedBuffer + dStrlen(expandedBuffer), (*itr)->value);
      dStrcat(expandedBuffer, "\";\r\n");

      stream.write(dStrlen(expandedBuffer),expandedBuffer);
   }

}
void SimFieldDictionary::printFields(SimObject *obj)
{
   const AbstractClassRep::FieldList &list = obj->getFieldList();
   char expandedBuffer[4096];
   Vector<Entry *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < HashTableSize; i++)
   {
      for(Entry *walk = mHashTable[i];walk; walk = walk->next)
      {
         // make sure we haven't written this out yet:
         U32 i;
         for(i = 0; i < (U32)list.size(); i++)
            if(list[i].pFieldname == walk->slotName)
               break;

         if(i != list.size())
            continue;

         flist.push_back(walk);
      }
   }
   dQsort(flist.address(),flist.size(),sizeof(Entry *),compareEntries);

   for(Vector<Entry *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s = \"", (*itr)->slotName);
      expandEscape(expandedBuffer + dStrlen(expandedBuffer), (*itr)->value);
      Con::printf("%s\"", expandedBuffer);
   }
}

//------------------------------------------------------------------------------

SimFieldDictionaryIterator::SimFieldDictionaryIterator(SimFieldDictionary * dictionary)
{
   mDictionary = dictionary;
   mHashIndex = -1;
   mEntry = 0;
   operator++();
}

SimFieldDictionary::Entry* SimFieldDictionaryIterator::operator++()
{
   if(!mDictionary)
      return(mEntry);

   if(mEntry)
      mEntry = mEntry->next;

   while(!mEntry && (mHashIndex < (SimFieldDictionary::HashTableSize-1)))
      mEntry = mDictionary->mHashTable[++mHashIndex];

   return(mEntry);
}

SimFieldDictionary::Entry* SimFieldDictionaryIterator::operator*()
{
   return(mEntry);
}
