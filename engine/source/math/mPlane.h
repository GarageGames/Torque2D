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

#ifndef _MPLANE_H_
#define _MPLANE_H_

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif
#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif

//---------------------------------------------------------------------------

class PlaneF: public Point3F
{
public:
   F32 d;

   PlaneF();
   PlaneF( const Point3F& p, const Point3F& n );
   PlaneF( F32 _x, F32 _y, F32 _z, F32 _d);
   PlaneF( const Point3F& j, const Point3F& k, const Point3F& l );

   // Comparison operators
   bool operator==(const PlaneF&a) const
   {
      return (a.x == x && a.y == y && a.z == z && a.d == d);
   }

   bool operator!=(const PlaneF&a) const
   {
      return (a.x != x || a.y != y || a.z != z || a.d == d);
   }

   // Methods
   void set(const F32 _x, const F32 _y, const F32 _z);

   void     set( const Point3F& p, const Point3F& n);
   void     set( const Point3F& k, const Point3F& j, const Point3F& l );
   void     setPoint(const Point3F &p); // assumes the x,y,z fields are already set
                                        // creates an un-normalized plane

   void     setXY(F32 zz);
   void     setYZ(F32 xx);
   void     setXZ(F32 yy);
   void     setXY(const Point3F& P, F32 dir);
   void     setYZ(const Point3F& P, F32 dir);
   void     setXZ(const Point3F& P, F32 dir);
   void     shiftX(F32 xx);
   void     shiftY(F32 yy);
   void     shiftZ(F32 zz);
   void     invert();
   void     neg();
   Point3F  project(Point3F pt); // project's the point onto the plane.

   F32      distToPlane( const Point3F& cp ) const;

   enum Side
   {
      Front = 1,
      On    = 0,
      Back  = -1
   };

   Side whichSide(const Point3F& cp) const;
   F32  intersect(const Point3F &start, const Point3F &end) const;

   bool     isHorizontal() const;
   bool     isVertical() const;

   Side whichSideBox(const Point3F& center,
                     const Point3F& axisx,
                     const Point3F& axisy,
                     const Point3F& axisz,
                     const Point3F& offset) const;
};
#define PARALLEL_PLANE  1e20f

#define PlaneSwitchCode(s, e) (s * 3 + e)


//---------------------------------------------------------------------------

inline PlaneF::PlaneF()
{
}

inline PlaneF::
   PlaneF( F32 _x, F32 _y, F32 _z, F32 _d )
{
   x = _x; y = _y; z = _z; d = _d;
}

inline PlaneF::PlaneF( const Point3F& p, const Point3F& n )
{
   set(p,n);
}

inline PlaneF::PlaneF( const Point3F& j, const Point3F& k, const Point3F& l )
{
   set(j,k,l);
}

inline void PlaneF::setXY( F32 zz )
{
   x = y = 0; z = 1; d = -zz;
}

inline void PlaneF::setYZ( F32 xx )
{
   x = 1; z = y = 0; d = -xx;
}

inline void PlaneF::setXZ( F32 yy )
{
   x = z = 0; y = 1; d = -yy;
}

inline void PlaneF::setXY(const Point3F& point, F32 dir)       // Normal = (0, 0, -1|1)
{
   x = y = 0;
   d = -((z = dir) * point.z);
}

inline void PlaneF::setYZ(const Point3F& point, F32 dir)       // Normal = (-1|1, 0, 0)
{
   z = y = 0;
   d = -((x = dir) * point.x);
}

inline void PlaneF::setXZ(const Point3F& point, F32 dir)       // Normal = (0, -1|1, 0)
{
   x = z = 0;
   d = -((y = dir) * point.y);
}

inline void PlaneF::shiftX( F32 xx )
{
   d -= xx * x;
}

inline void PlaneF::shiftY( F32 yy )
{
   d -= yy * y;
}

inline void PlaneF::shiftZ( F32 zz )
{
   d -= zz * z;
}

inline bool PlaneF::isHorizontal() const
{
   return (x == 0 && y == 0) ? true : false;
}

inline bool PlaneF::isVertical() const
{
    return ((x != 0 || y != 0) && z == 0) ? true : false;
}

inline Point3F PlaneF::project(Point3F pt)
{
   F32 dist = distToPlane(pt);
   return Point3F(pt.x - x * dist, pt.y - y * dist, pt.z - z * dist);
}

inline F32 PlaneF::distToPlane( const Point3F& cp ) const
{
   // return mDot(*this,cp) + d;
   return (x * cp.x + y * cp.y + z * cp.z) + d;
}

inline PlaneF::Side PlaneF::whichSide(const Point3F& cp) const
{
   F32 dist = distToPlane(cp);
   if (dist >= 0.005f)                 // if (mFabs(dist) < 0.005f)
      return Front;                    //    return On;
   else if (dist <= -0.005f)           // else if (dist > 0.0f)
      return Back;                     //    return Front;
   else                                // else
      return On;                       //    return Back;
}

inline void PlaneF::set(const F32 _x, const F32 _y, const F32 _z)
{
    Point3F::set(_x,_y,_z);
}

//---------------------------------------------------------------------------
/// Calculate the coefficients of the plane passing through
/// a point with the given normal.
inline void PlaneF::setPoint(const Point3F &p)
{
   d = -(p.x * x + p.y * y + p.z * z);
}

inline void PlaneF::set( const Point3F& p, const Point3F& n )
{
   x = n.x; y = n.y; z = n.z;
   normalize();

   // Calculate the last plane coefficient.
   d = -(p.x * x + p.y * y + p.z * z);
}

//---------------------------------------------------------------------------
/// Calculate the coefficients of the plane passing through
/// three points.  Basically it calculates the normal to the three
/// points then calculates a plane through the middle point with that
/// normal.
inline void PlaneF::set( const Point3F& k, const Point3F& j, const Point3F& l )
{
//   Point3F kj,lj,pv;
//   kj = k;
//   kj -= j;
//   lj = l;
//   lj -= j;
//   mCross( kj, lj, &pv );
//   set(j,pv);

// Above ends up making function calls up the %*#...
// This is called often enough to be a little more direct
// about it (sqrt should become intrinsic in the future)...
   F32 ax = k.x-j.x;
   F32 ay = k.y-j.y;
   F32 az = k.z-j.z;
   F32 bx = l.x-j.x;
   F32 by = l.y-j.y;
   F32 bz = l.z-j.z;
   x = ay*bz - az*by;
   y = az*bx - ax*bz;
   z = ax*by - ay*bx;
   F32 squared = x*x + y*y + z*z;
   AssertFatal(squared != 0.0, "Error, no plane possible!");

   // In non-debug mode
   if (squared != 0) {
      F32 invSqrt = 1.0f / mSqrt(x*x + y*y + z*z);
      x *= invSqrt;
      y *= invSqrt;
      z *= invSqrt;
      d = -(j.x * x + j.y * y + j.z * z);
   } else {
      x = 0;
      y = 0;
      z = 1;
      d = -(j.x * x + j.y * y + j.z * z);
   }
}

inline void PlaneF::invert()
{
   x = -x;
   y = -y;
   z = -z;
   d = -d;
}

inline void PlaneF::neg()
{
   invert();
}

inline F32 PlaneF::intersect(const Point3F &p1, const Point3F &p2) const
{
   F32 den = mDot(p2 - p1, *this);
   if(den == 0)
      return PARALLEL_PLANE;
   return -distToPlane(p1) / den;
}

inline PlaneF::Side PlaneF::whichSideBox(const Point3F& center,
                                         const Point3F& axisx,
                                         const Point3F& axisy,
                                         const Point3F& axisz,
                                         const Point3F& /*offset*/) const
{
   F32 baseDist = distToPlane(center);

   F32 compDist = mFabs(mDot(axisx, *this)) +
                  mFabs(mDot(axisy, *this)) +
                  mFabs(mDot(axisz, *this));

   if (baseDist >= compDist)
      return Front;
   else if (baseDist <= -compDist)
      return Back;
   else
      return On;
}

#endif
