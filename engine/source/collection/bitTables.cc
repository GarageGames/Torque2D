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

#include "bitTables.h"

bool     BitTables::mTablesBuilt = false;
S8       BitTables::mHighBit[256];
S8       BitTables::mWhichOn[256][8];
S8       BitTables::mNumOn[256];
static   BitTables  sBuildTheTables;     // invoke ctor first-time work

BitTables::BitTables()
{
   if(! mTablesBuilt){
      // This code only happens once - it relies on the tables being clear.
      for( U32 byte = 0; byte < 256; byte++ )
         for( U32 bit = 0; bit < 8; bit++ )
            if( byte & (1 << bit) )
               mHighBit[byte] = (mWhichOn[byte][mNumOn[byte]++] = bit) + 1;

      mTablesBuilt = true;
   }
}
