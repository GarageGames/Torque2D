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

// Borrowed from: http://burtleburtle.net/bob/hash/doobs.html
//
// Original code by:
//
// By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
// code any way you wish, private, educational, or commercial.  It's free.

#include "platform/platform.h"

// Debug Profiling.
#include "debug/profiler.h"

#include "algorithm/hashFunction.h"

#define hashsize(n) ((U32)1 << (n))
#define hashmask(n) (hashsize(n) - 1)

/*
--------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.
For every delta with one or two bits set, and the deltas of all three
high bits or all three low bits, whether the original value of a,b,c
is almost all zero or is uniformly distributed,
* If mix() is run forward or backward, at least 32 bits in a,b,c
have at least 1/4 probability of changing.
* If mix() is run forward, every bit of c will change between 1/3 and
2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
mix() was built out of 36 single-cycle latency instructions in a
structure that could supported 2x parallelism, like so:
a -= b;
a -= c; x = (c>>13);
b -= c; a ^= x;
b -= a; x = (a<<8);
c -= a; b ^= x;
c -= b; x = (b>>13);
...
Unfortunately, superscalar Pentiums and Sparcs can't take advantage
of that parallelism.  They've also turned some of those single-cycle
latency instructions into multi-cycle latency instructions.  Still,
this is the fastest good hash I could find.  There were about 2^^68
to choose from.  I only looked at a billion or so.
--------------------------------------------------------------------
*/
#define mix(a,b,c) \
{ \
   a -= b; a -= c; a ^= (c >> 13); \
   b -= c; b -= a; b ^= (a << 8); \
   c -= a; c -= b; c ^= (b >> 13); \
   a -= b; a -= c; a ^= (c >> 12);  \
   b -= c; b -= a; b ^= (a << 16); \
   c -= a; c -= b; c ^= (b >> 5); \
   a -= b; a -= c; a ^= (c >> 3);  \
   b -= c; b -= a; b ^= (a << 10); \
   c -= a; c -= b; c ^= (b >> 15); \
}

/*
--------------------------------------------------------------------
hash() -- hash a variable-length key into a 32-bit value
k       : the key (the unaligned variable-length array of bytes)
len     : the length of the key, counting by bytes
initval : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  Every 1-bit and 2-bit delta achieves avalanche.
About 6*len+35 instructions.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 32 bits,
use a bitmask.  For example, if you need only 10 bits, do
h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (U8 **)k, do it like this:
for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free.

See http://burtleburtle.net/bob/hash/evahash.html
Use for hash table lookup, or anything where one collision in 2^^32 is
acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
*/

U32 hash(register U8 *k, register U32 length, register U32 initval)
{
    // Debug Profiling.
    PROFILE_SCOPE(HashFunction);

   U32 a, b, c, len;

   /* Set up the internal state */
   len = length;
   a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
   c = initval;         /* the previous hash value */

   /*---------------------------------------- handle most of the key */
   while (len >= 12)
   {
      a += (k[0] + ((U32)k[1] << 8) + ((U32)k[ 2] << 16) + ((U32)k[ 3] << 24));
      b += (k[4] + ((U32)k[5] << 8) + ((U32)k[ 6] << 16) + ((U32)k[ 7] << 24));
      c += (k[8] + ((U32)k[9] << 8) + ((U32)k[10] << 16) + ((U32)k[11] << 24));
      mix(a, b, c);
      k += 12;
      len -= 12;
   }

   /*------------------------------------- handle the last 11 bytes */
   c += length;
   
   switch (len)              /* all the case statements fall through */
   {
      case 11: c += ((U32)k[10] << 24);
      case 10: c += ((U32)k[9] << 16);
      case 9 : c += ((U32)k[8] << 8);
      /* the first byte of c is reserved for the length */
      case 8 : b += ((U32)k[7] << 24);
      case 7 : b += ((U32)k[6] << 16);
      case 6 : b += ((U32)k[5] << 8);
      case 5 : b += k[4];
      case 4 : a += ((U32)k[3] << 24);
      case 3 : a += ((U32)k[2] << 16);
      case 2 : a += ((U32)k[1] << 8);
      case 1 : a += k[0];
      /* case 0: nothing left to add */
   }
   
   mix(a, b, c);
   /*-------------------------------------------- report the result */
   return c;
}
