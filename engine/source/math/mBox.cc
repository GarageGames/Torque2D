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

#include "math/mBox.h"
#include "math/mMatrix.h"

bool Box3F::collideLine(const Point3F& start, const Point3F& end, F32* t, Point3F* n) const
{
   // Collide against bounding box. Need at least this for the editor.
   F32 st,et;
   F32 fst = 0;
   F32 fet = 1;
   const F32* bmin = &mMin.x;
   const F32* bmax = &mMax.x;
   const F32* si   = &start.x;
   const F32* ei   = &end.x;

   Point3F na[3] = { Point3F(1, 0, 0), Point3F(0, 1, 0), Point3F(0, 0, 1) };
   Point3F finalNormal;

   for (int i = 0; i < 3; i++) {
      Point3F normal = na[i];
      if (si[i] < ei[i]) {
         if (si[i] > bmax[i] || ei[i] < bmin[i])
            return false;
         F32 di = ei[i] - si[i];
         st = (si[i] < bmin[i]) ? (bmin[i] - si[i]) / di : 0;
         et = (ei[i] > bmax[i]) ? (bmax[i] - si[i]) / di : 1;
         normal.neg();
      }
      else {
         if (ei[i] > bmax[i] || si[i] < bmin[i])
            return false;
         F32 di = ei[i] - si[i];
         st = (si[i] > bmax[i]) ? (bmax[i] - si[i]) / di : 0;
         et = (ei[i] < bmin[i]) ? (bmin[i] - si[i]) / di : 1;
      }
      if (st > fst) {
         fst = st;
         finalNormal = normal;
      }
      if (et < fet)
         fet = et;

      if (fet < fst)
         return false;
   }

   *t = fst;
   *n = finalNormal;
   return true;
}


bool Box3F::collideLine(const Point3F& start, const Point3F& end) const
{
   F32 t;
   Point3F normal;
   return collideLine(start, end, &t, &normal);
}

// returns true if "oriented" box collides with us
// radiiB is dimension of incoming box (half x,y,z length
// toA is transform that takes incoming box into our space
// assumes incoming box is centered at origin of source space
bool Box3F::collideOrientedBox(const Point3F & bRadii, const MatrixF & toA) const
{
   Point3F p;
   toA.getColumn(3,&p);
   Point3F aCenter = mMin + mMax;
   aCenter *= 0.5f;
   p -= aCenter; // this essentially puts origin of toA target space on the center of the current box
   Point3F aRadii = mMax - mMin;
   aRadii *= 0.5f;

    F32 absXX,absXY,absXZ;
    F32 absYX,absYY,absYZ;
    F32 absZX,absZY,absZZ;

   const F32 * f = toA;

   absXX = mFabs(f[0]);
   absYX = mFabs(f[1]);
   absZX = mFabs(f[2]);

    if (aRadii.x + bRadii.x * absXX + bRadii.y * absYX + bRadii.z * absZX - mFabs(p.x)<0.0f)
        return false;

   absXY = mFabs(f[4]);
   absYY = mFabs(f[5]);
   absZY = mFabs(f[6]);
    if (aRadii.y + bRadii.x * absXY +	bRadii.y * absYY +	bRadii.z * absZY - mFabs(p.y)<0.0f)
        return false;
    
   absXZ = mFabs(f[8]);
   absYZ = mFabs(f[9]);
   absZZ = mFabs(f[10]);
    if (aRadii.z + bRadii.x * absXZ + bRadii.y * absYZ +	bRadii.z * absZZ - mFabs(p.z)<0.0f)
        return false;

    if (aRadii.x*absXX + aRadii.y*absXY + aRadii.z*absXZ + bRadii.x - mFabs(p.x*f[0] + p.y*f[4] + p.z*f[8])<0.0f)
        return false;

    if (aRadii.x*absYX + aRadii.y*absYY + aRadii.z*absYZ + bRadii.y - mFabs(p.x*f[1] + p.y*f[5] + p.z*f[9])<0.0f)
        return false;		
    
    if (aRadii.x*absZX + aRadii.y*absZY + aRadii.z*absZZ + bRadii.z - mFabs(p.x*f[2] + p.y*f[6] + p.z*f[10])<0.0f)
        return false;		
    
    if (mFabs(p.z*f[4] - p.y*f[8]) >
                aRadii.y * absXZ + aRadii.z * absXY +
                bRadii.y * absZX + bRadii.z * absYX)
        return false;
    
    if (mFabs(p.z*f[5] - p.y*f[9]) >
                aRadii.y * absYZ + aRadii.z * absYY +
                bRadii.x * absZX + bRadii.z * absXX)
        return false;
    
    if (mFabs(p.z*f[6] - p.y*f[10]) >
                aRadii.y * absZZ + aRadii.z * absZY +
                bRadii.x * absYX + bRadii.y * absXX)
        return false;
    
    if (mFabs(p.x*f[8] - p.z*f[0]) >
                aRadii.x * absXZ + aRadii.z * absXX +
                bRadii.y * absZY + bRadii.z * absYY)
        return false;
    
    if (mFabs(p.x*f[9] - p.z*f[1]) >
                aRadii.x * absYZ + aRadii.z * absYX +
                bRadii.x * absZY + bRadii.z * absXY)
        return false;
    
    if (mFabs(p.x*f[10] - p.z*f[2]) >
                aRadii.x * absZZ + aRadii.z * absZX +
                bRadii.x * absYY + bRadii.y * absXY)
        return false;
    
    if (mFabs(p.y*f[0] - p.x*f[4]) >
                aRadii.x * absXY + aRadii.y * absXX +
                bRadii.y * absZZ + bRadii.z * absYZ)
        return false;
    
    if (mFabs(p.y*f[1] - p.x*f[5]) >
                aRadii.x * absYY + aRadii.y * absYX +
                bRadii.x * absZZ + bRadii.z * absXZ)
        return false;
    
    if (mFabs(p.y*f[2] - p.x*f[6]) >
                aRadii.x * absZY + aRadii.y * absZX +
                bRadii.x * absYZ + bRadii.y * absXZ)
        return false;
    
    return true;
}



