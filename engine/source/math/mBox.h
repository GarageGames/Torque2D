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

#ifndef _MBOX_H_
#define _MBOX_H_

#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif

//------------------------------------------------------------------------------
/// Bounding Box
///
/// A helper class for working with boxes. It runs at F32 precision.
///
/// @see Box3D
class Box3F
{
  public:
   Point3F mMin; ///< Minimum extents of box
   Point3F mMax; ///< Maximum extents of box

  public:
   Box3F() { }

   /// Create a box from two points.
   ///
   /// Normally, this function will compensate for mismatched
   /// min/max values. If you know your values are valid, you
   /// can set in_overrideCheck to true and skip this.
   ///
   /// @param   in_rMin          Minimum extents of box.
   /// @param   in_rMax          Maximum extents of box.
   /// @param   in_overrideCheck  Pass true to skip check of extents.
   Box3F(const Point3F& in_rMin, const Point3F& in_rMax, const bool in_overrideCheck = false);

   /// Create a box from six extent values.
   ///
   /// No checking is performed as to the validity of these
   /// extents, unlike the other constructor.
   Box3F(F32 xmin, F32 ymin, F32 zmin, F32 max, F32 ymax, F32 zmax);

   /// Check to see if a point is contained in this box.
   bool isContained(const Point3F& in_rContained) const;

   /// Check to see if another box overlaps this box.
   bool isOverlapped(const Box3F&  in_rOverlap) const;

   /// Check to see if another box is contained in this box.
   bool isContained(const Box3F& in_rContained) const;

   F32 len_x() const;
   F32 len_y() const;
   F32 len_z() const;

   /// Perform an intersection operation with another box
   /// and store the results in this box.
   void intersect(const Box3F& in_rIntersect);
   void intersect(const Point3F& in_rIntersect);

   /// Get the center of this box.
   ///
   /// This is the average of min and mMax.
   void getCenter(Point3F* center) const;

   /// Collide a line against the box.
   ///
   /// @param   start   Start of line.
   /// @param   end     End of line.
   /// @param   t       Value from 0.0-1.0, indicating position
   ///                  along line of collision.
   /// @param   n       Normal of collision.
   bool collideLine(const Point3F& start, const Point3F& end, F32*t, Point3F*n) const;

   /// Collide a line against the box.
   ///
   /// Returns true on collision.
   bool collideLine(const Point3F& start, const Point3F& end) const;

   /// Collide an oriented box against the box.
   ///
   /// Returns true if "oriented" box collides with us.
   /// Assumes incoming box is centered at origin of source space.
   ///
   /// @param   radii   The dimension of incoming box (half x,y,z length).
   /// @param   toUs    A transform that takes incoming box into our space.
   bool collideOrientedBox(const Point3F & radii, const MatrixF & toUs) const;

   /// Check that the box is valid.
   ///
   /// Currently, this just means that min < mMax.
   bool isValidBox() const { return (mMin.x <= mMax.x) &&
                                    (mMin.y <= mMax.y) &&
                                    (mMin.z <= mMax.z); }

   /// Return the closest point of the box, relative to the passed point.
   Point3F getClosestPoint(const Point3F& refPt) const;
};

inline Box3F::Box3F(const Point3F& in_rMin, const Point3F& in_rMax, const bool in_overrideCheck)
 : mMin(in_rMin),
   mMax(in_rMax)
{
   if (in_overrideCheck == false) {
      mMin.setMin(in_rMax);
      mMax.setMax(in_rMin);
   }
}

inline Box3F::Box3F(F32 xMin, F32 yMin, F32 zMin, F32 xMax, F32 yMax, F32 zMax)
   : mMin(xMin,yMin,zMin),
     mMax(xMax,yMax,zMax)
{
}

inline bool Box3F::isContained(const Point3F& in_rContained) const
{
   return (in_rContained.x >= mMin.x && in_rContained.x < mMax.x) &&
          (in_rContained.y >= mMin.y && in_rContained.y < mMax.y) &&
          (in_rContained.z >= mMin.z && in_rContained.z < mMax.z);
}

inline bool Box3F::isOverlapped(const Box3F& in_rOverlap) const
{
   if (in_rOverlap.mMin.x > mMax.x ||
       in_rOverlap.mMin.y > mMax.y ||
       in_rOverlap.mMin.z > mMax.z)
      return false;
   if (in_rOverlap.mMax.x < mMin.x ||
       in_rOverlap.mMax.y < mMin.y ||
       in_rOverlap.mMax.z < mMin.z)
      return false;
   return true;
}

inline bool Box3F::isContained(const Box3F& in_rContained) const
{
   return (mMin.x <= in_rContained.mMin.x) &&
          (mMin.y <= in_rContained.mMin.y) &&
          (mMin.z <= in_rContained.mMin.z) &&
          (mMax.x >= in_rContained.mMax.x) &&
          (mMax.y >= in_rContained.mMax.y) &&
          (mMax.z >= in_rContained.mMax.z);
}

inline F32 Box3F::len_x() const
{
   return mMax.x - mMin.x;
}

inline F32 Box3F::len_y() const
{
   return mMax.y - mMin.y;
}

inline F32 Box3F::len_z() const
{
   return mMax.z - mMin.z;
}

inline void Box3F::intersect(const Box3F& in_rIntersect)
{
   mMin.setMin(in_rIntersect.mMin);
   mMax.setMax(in_rIntersect.mMax);
}

inline void Box3F::intersect(const Point3F& in_rIntersect)
{
   mMin.setMin(in_rIntersect);
   mMax.setMax(in_rIntersect);
}

inline void Box3F::getCenter(Point3F* center) const
{
   center->x = F32((mMin.x + mMax.x) * 0.5);
   center->y = F32((mMin.y + mMax.y) * 0.5);
   center->z = F32((mMin.z + mMax.z) * 0.5);
}

inline Point3F Box3F::getClosestPoint(const Point3F& refPt) const
{
   Point3F closest;
   if      (refPt.x <= mMin.x) closest.x = mMin.x;
   else if (refPt.x >  mMax.x) closest.x = mMax.x;
   else                       closest.x = refPt.x;

   if      (refPt.y <= mMin.y) closest.y = mMin.y;
   else if (refPt.y >  mMax.y) closest.y = mMax.y;
   else                       closest.y = refPt.y;

   if      (refPt.z <= mMin.z) closest.z = mMin.z;
   else if (refPt.z >  mMax.z) closest.z = mMax.z;
   else                       closest.z = refPt.z;

   return closest;
}


//------------------------------------------------------------------------------
/// Clone of Box3F, using 3D types.
///
/// 3D types use F64.
///
/// @see Box3F
class Box3D
{
  public:
   Point3D mMin;
   Point3D mMax;

  public:
   Box3D() { }
   Box3D(const Point3D& in_rMin, const Point3D& in_rMax, const bool in_overrideCheck = false);

   bool isContained(const Point3D& in_rContained) const;
   bool isOverlapped(const Box3D&  in_rOverlap) const;

   F64 len_x() const;
   F64 len_y() const;
   F64 len_z() const;

   void intersect(const Box3D& in_rIntersect);
   void getCenter(Point3D* center) const;
};

inline Box3D::Box3D(const Point3D& in_rMin, const Point3D& in_rMax, const bool in_overrideCheck)
 : mMin(in_rMin),
   mMax(in_rMax)
{
   if (in_overrideCheck == false) {
      mMin.setMin(in_rMax);
      mMax.setMax(in_rMin);
   }
}

inline bool Box3D::isContained(const Point3D& in_rContained) const
{
   return (in_rContained.x >= mMin.x && in_rContained.x < mMax.x) &&
          (in_rContained.y >= mMin.y && in_rContained.y < mMax.y) &&
          (in_rContained.z >= mMin.z && in_rContained.z < mMax.z);
}

inline bool Box3D::isOverlapped(const Box3D& in_rOverlap) const
{
   if (in_rOverlap.mMin.x > mMax.x ||
       in_rOverlap.mMin.y > mMax.y ||
       in_rOverlap.mMin.z > mMax.z)
      return false;
   if (in_rOverlap.mMax.x < mMin.x ||
       in_rOverlap.mMax.y < mMin.y ||
       in_rOverlap.mMax.z < mMin.z)
      return false;
   return true;
}

inline F64 Box3D::len_x() const
{
   return mMax.x - mMin.x;
}

inline F64 Box3D::len_y() const
{
   return mMax.y - mMin.y;
}

inline F64 Box3D::len_z() const
{
   return mMax.z - mMin.z;
}

inline void Box3D::intersect(const Box3D& in_rIntersect)
{
   mMin.setMin(in_rIntersect.mMin);
   mMax.setMax(in_rIntersect.mMax);
}

inline void Box3D::getCenter(Point3D* center) const
{
   center->x = (mMin.x + mMax.x) * 0.5;
   center->y = (mMin.y + mMax.y) * 0.5;
   center->z = (mMin.z + mMax.z) * 0.5;
}


#endif // _DBOX_H_
