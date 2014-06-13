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
#include "io/resource/resourceManager.h"
#include "collection/findIterator.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

ResDictionary::ResDictionary()
{
   entryCount = 0;
   hashTableSize = 1023; //DefaultTableSize;
   hashTable = new ResourceObject *[hashTableSize];
   S32 i;
   for(i = 0; i < hashTableSize; i++)
      hashTable[i] = NULL;
}

ResDictionary::~ResDictionary()
{
   // we assume the resources are purged before we destroy
   // the dictionary

   delete[] hashTable;
}

S32 ResDictionary::hash(StringTableEntry path, StringTableEntry file)
{
   return ((U32)((((dsize_t)path) >> 2) + (((dsize_t)file) >> 2) )) % hashTableSize;
}

void ResDictionary::insert(ResourceObject *obj, StringTableEntry path, StringTableEntry file)
{
   if(path)
   {
      char fullPath[1024];
      Platform::makeFullPathName(path, fullPath, sizeof(fullPath));
      path = StringTable->insert(fullPath);
   }

   obj->name = file;
   obj->path = path;

   S32 idx = hash(path, file);
   obj->nextEntry = hashTable[idx];
   hashTable[idx] = obj;
   entryCount++;

   if(entryCount > hashTableSize) {
      ResourceObject *head = NULL, *temp, *walk;
      for(idx = 0; idx < hashTableSize;idx++) {
         walk = hashTable[idx];
         while(walk)
         {
            temp = walk->nextEntry;
            walk->nextEntry = head;
            head = walk;
            walk = temp;
         }
      }
      delete[] hashTable;
      hashTableSize = 2 * hashTableSize - 1;
      hashTable = new ResourceObject *[hashTableSize];
      for(idx = 0; idx < hashTableSize; idx++)
         hashTable[idx] = NULL;
      walk = head;
      while(walk)
      {
         temp = walk->nextEntry;
         idx = hash(walk);
         walk->nextEntry = hashTable[idx];
         hashTable[idx] = walk;
         walk = temp;
      }
   }
}

ResourceObject* ResDictionary::find(StringTableEntry path, StringTableEntry name)
{
   if(path)
   {
      char fullPath[1024];
      Platform::makeFullPathName(path, fullPath, sizeof(fullPath));
      path = StringTable->insert(fullPath);
   }

   for(ResourceObject *walk = hashTable[hash(path, name)]; walk; walk = walk->nextEntry)
      if(walk->name == name && walk->path == path)
         return walk;
   return NULL;
}

ResourceObject* ResDictionary::find(StringTableEntry path, StringTableEntry name, StringTableEntry zipPath, StringTableEntry zipName)
{
   if(path)
   {
      char fullPath[1024];
      Platform::makeFullPathName(path, fullPath, sizeof(fullPath));
      path = StringTable->insert(fullPath);
   }

   for(ResourceObject *walk = hashTable[hash(path, name)]; walk; walk = walk->nextEntry)
      if(walk->name == name && walk->path == path && walk->zipName == zipName && walk->zipPath == zipPath)
         return walk;
   return NULL;
}

ResourceObject* ResDictionary::find(StringTableEntry path, StringTableEntry name, U32 flags)
{
   if(path)
   {
      char fullPath[1024];
      Platform::makeFullPathName(path, fullPath, sizeof(fullPath));
      path = StringTable->insert(fullPath);
   }

   for(ResourceObject *walk = hashTable[hash(path, name)]; walk; walk = walk->nextEntry)
      if(walk->name == name && walk->path == path && U32(walk->flags) == flags)
         return walk;
   return NULL;
}

void ResDictionary::pushBehind(ResourceObject *resObj, S32 flagMask)
{
   remove(resObj);
   entryCount++;
   ResourceObject **walk = &hashTable[hash(resObj)];
   for(; *walk; walk = &(*walk)->nextEntry)
   {
      if(!((*walk)->flags & flagMask))
      {
         resObj->nextEntry = *walk;
         *walk = resObj;
         return;
      }
   }
   resObj->nextEntry = NULL;
   *walk = resObj;
}

void ResDictionary::remove(ResourceObject *resObj)
{
   for(ResourceObject **walk = &hashTable[hash(resObj)]; *walk; walk = &(*walk)->nextEntry)
   {
      if(*walk == resObj)
      {
         entryCount--;
         *walk = resObj->nextEntry;
         return;
      }
   }
}
