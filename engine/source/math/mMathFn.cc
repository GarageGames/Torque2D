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

#include "math/mMathFn.h"
#include "math/mPlane.h"
#include "math/mMatrix.h"


void mTransformPlane(const MatrixF& mat,
                     const Point3F& scale,
                     const PlaneF&  plane,
                     PlaneF*        result)
{
   m_matF_x_scale_x_planeF(mat, &scale.x, &plane.x, &result->x);
}

//--------------------------------------

U32 getNextPow2(U32 io_num)
{
   S32 oneCount   = 0;
   S32 shiftCount = -1;
   while (io_num) {
      if(io_num & 1)
         oneCount++;
      shiftCount++;
      io_num >>= 1;
   }
   if(oneCount > 1)
      shiftCount++;

   return U32(1 << shiftCount);
}

//--------------------------------------

U32 getBinLog2(U32 io_num)
{
   AssertFatal(io_num != 0 && isPow2(io_num) == true,
               "Error, this only works on powers of 2 > 0");

   S32 shiftCount = 0;
   while (io_num) {
      shiftCount++;
      io_num >>= 1;
   }

   return U32(shiftCount - 1);
}


