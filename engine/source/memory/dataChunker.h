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

#ifndef _DATACHUNKER_H_
#define _DATACHUNKER_H_

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif

//----------------------------------------------------------------------------
/// Implements a chunked data allocater.
///
/// Calling new/malloc all the time is a time consuming operation. Therefore,
/// we provide the DataChunker, which allocates memory in blockss of
/// chunkSize (by default 16k, see ChunkSize, though it can be set in
/// the constructor), then doles it out as requested, in chunks of up to
/// chunkSize in size.
///
/// It will assert if you try to get more than ChunkSize bytes at a time,
/// and it deals with the logic of allocating new blocks and giving out
/// word-aligned chunks.
///
/// Note that new/free/realloc WILL NOT WORK on memory gotten from the
/// DataChunker. This also only grows (you can call freeBlocks to deallocate
/// and reset things).
class DataChunker
{
  public:
   enum {
      ChunkSize = 16376 ///< Default size for chunks.
   };

  private:
   /// Block of allocated memory.
   ///
   /// <b>This has nothing to do with datablocks as used in the rest of Torque.</b>
   struct DataBlock
   {
      DataBlock *next;
      U8 *data;
      S32 curIndex;
      DataBlock(S32 size);
      ~DataBlock();
   };
   DataBlock *curBlock;
   S32 chunkSize;

  public:

   /// Return a pointer to a chunk of memory from a pre-allocated block.
   ///
   /// This memory goes away when you call freeBlocks.
   ///
   /// This memory is word-aligned.
   /// @param   size    Size of chunk to return. This must be less than chunkSize or else
   ///                  an assertion will occur.
   void *alloc(S32 size);

   /// Free all allocated memory blocks.
   ///
   /// This invalidates all pointers returned from alloc().
   void freeBlocks();

   /// Initialize using blocks of a given size.
   ///
   /// One new block is allocated at constructor-time.
   ///
   /// @param   size    Size in bytes of the space to allocate for each block.
   DataChunker(S32 size=ChunkSize);
   ~DataChunker();
};


//----------------------------------------------------------------------------

template<class T>
class Chunker: private DataChunker
{
public:
   Chunker(S32 size = DataChunker::ChunkSize) : DataChunker(size) {};
   T* alloc()  { return reinterpret_cast<T*>(DataChunker::alloc(S32(sizeof(T)))); }
   void clear()  { freeBlocks(); };
};

template<class T>
class FreeListChunker: private DataChunker
{
   S32 numAllocated;
   S32 elementSize;
   T *freeListHead;
public:
   FreeListChunker(S32 size = DataChunker::ChunkSize) : DataChunker(size)
   {
      numAllocated = 0;
      elementSize = getMax(U32(sizeof(T)), U32(sizeof(T *)));
      freeListHead = NULL;
   }

   T *alloc()
   {
      numAllocated++;
      if(freeListHead == NULL)
         return reinterpret_cast<T*>(DataChunker::alloc(elementSize));
      T* ret = freeListHead;
      freeListHead = *(reinterpret_cast<T**>(freeListHead));
      return ret;
   }

   void free(T* elem)
   {
      numAllocated--;
      *(reinterpret_cast<T**>(elem)) = freeListHead;
      freeListHead = elem;

      // If nothing's allocated, clean up!
      if(!numAllocated)
      {
         freeBlocks();
         freeListHead = NULL;
      }
   }

   // Allow people to free all their memory if they want.
   void freeBlocks()
   {
      DataChunker::freeBlocks();

      // We have to terminate the freelist as well or else we'll run
      // into crazy unused memory.
      freeListHead = NULL;
   }
};


#endif
