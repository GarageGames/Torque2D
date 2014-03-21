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
#include "dataChunker.h"


//----------------------------------------------------------------------------

DataChunker::DataChunker(S32 size)
{
   chunkSize          = size;
   curBlock           = new DataBlock(size);
   curBlock->next     = NULL;
   curBlock->curIndex = 0;
}

DataChunker::~DataChunker()
{
   freeBlocks();
}

void *DataChunker::alloc(S32 size)
{
   AssertFatal(size <= chunkSize, "Data chunk too large.");
   if(!curBlock || size + curBlock->curIndex > chunkSize)
   {
      DataBlock *temp = new DataBlock(chunkSize);
      temp->next = curBlock;
      temp->curIndex = 0;
      curBlock = temp;
   }
   void *ret = curBlock->data + curBlock->curIndex;
   
   S32 index;
   index = curBlock->curIndex;
   curBlock->curIndex += ((U32)size + 3) & ~3; // dword align
   
#ifdef EMSCRIPTEN
   if (curBlock->curIndex < index || ((curBlock->data + curBlock->curIndex) - (U8*)ret) < size) {
      AssertFatal(false, "Something weird is going on here");
   }
#endif

   return ret;
}

DataChunker::DataBlock::DataBlock(S32 size)
{
   data = new U8[size];
}

DataChunker::DataBlock::~DataBlock()
{
   delete[] data;
}

void DataChunker::freeBlocks()
{
   while(curBlock)
   {
      DataBlock *temp = curBlock->next;
      delete curBlock;
      curBlock = temp;
   }
}

