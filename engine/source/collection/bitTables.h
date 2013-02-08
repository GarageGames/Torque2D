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

#ifndef _BITTABLES_H_
#define _BITTABLES_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

class BitTables
{
  private:
   static bool mTablesBuilt;        ///< For first time build
   static S8   mHighBit[256];       ///< I.e. crude logarithm
   static S8   mWhichOn[256][8];    ///< Unroll a bitset collection (note
   static S8   mNumOn[256];         //    ptr table wastes same amt.)

  public:
   BitTables();

   /// How many bits are on?
   static const S32   numOn(U8 b)       {  return mNumOn[b];      }

   /// Return a pointer to 8 S8's, which are set according to which bits are set in the passed value.
   static const S8 *  whichOn(U8 b)     {  return mWhichOn[b];    }

   static const S32   highBit(U8 b)     {  return mHighBit[b];    }


   static S32 getPower16(U16 x)   { return x<256 ? mHighBit[x] : mHighBit[x>>8]+8; }
   static S32 getPower32(U32 x){
                  if( x < (1<<16) )
                     return( x < (1<<8)  ? mHighBit[x] : mHighBit[x>>8]+8 );
                  else
                     return( x < (1<<24) ? mHighBit[x>>16]+16 : mHighBit[x>>24]+24 );
                 }
};

#endif
