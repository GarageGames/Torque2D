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

#include "sim/simBase.h"
#include "console/consoleTypes.h"
#include "sim/scriptObject.h"
#include "sim/simDatablock.h"

#include "simDatablock_ScriptBinding.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(SimDataBlock);
SimObjectId SimDataBlock::sNextObjectId = DataBlockObjectIdFirst;
S32 SimDataBlock::sNextModifiedKey = 0;

//---------------------------------------------------------------------------

SimDataBlock::SimDataBlock()
{
   setModDynamicFields(true);
   setModStaticFields(true);
}

bool SimDataBlock::onAdd()
{
   Parent::onAdd();

   // This initialization is done here, and not in the constructor,
   // because some jokers like to construct and destruct objects
   // (without adding them to the manager) to check what class
   // they are.
   modifiedKey = ++sNextModifiedKey;
   AssertFatal(sNextObjectId <= DataBlockObjectIdLast,
               "Exceeded maximum number of data blocks");

   // add DataBlock to the DataBlockGroup unless it is client side ONLY DataBlock
   if (getId() >= DataBlockObjectIdFirst && getId() <= DataBlockObjectIdLast)
      if (SimGroup* grp = Sim::getDataBlockGroup())
         grp->addObject(this);

   return true;
}

void SimDataBlock::onRemove()
{
    Parent::onRemove();
}

void SimDataBlock::assignId()
{
   // We don't want the id assigned by the manager, but it may have
   // already been assigned a correct data block id.
   if (getId() < DataBlockObjectIdFirst || getId() > DataBlockObjectIdLast)
      setId(sNextObjectId++);
}

void SimDataBlock::onStaticModified(const char* slotName, const char* newValue)
{
   modifiedKey = sNextModifiedKey++;

}

/*void SimDataBlock::setLastError(const char*)
{
} */

void SimDataBlock::packData(BitStream*)
{
}

void SimDataBlock::unpackData(BitStream*)
{
}

bool SimDataBlock::preload(bool, char[256])
{
   return true;
}
