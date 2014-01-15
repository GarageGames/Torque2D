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

#ifndef _MMATRIX_H_
#define _MMATRIX_H_


#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

/// 4x4 Matrix Class
///
/// This runs at F32 precision.
class MatrixF
{
private:
   F32 m[16];     ///< Note: this is stored in ROW MAJOR format.  OpenGL is
                  ///  COLUMN MAJOR.  Transpose before sending down.

public:
   /// Create an uninitialized matrix.
   ///
   /// @param   identity    If true, initialize to the identity matrix.
   explicit MatrixF(bool identity=false);

   /// Create a matrix to rotate about origin by e.
   /// @see set
   explicit MatrixF( const EulerF &e);

   /// Create a matrix to rotate about p by e.
   /// @see set
   MatrixF( const EulerF &e, const Point3F& p);

   /// Get the index in m to element in column i, row j
   ///
   /// This is necessary as we have m as a one dimensional array.
   ///
   /// @param   i   Column desired.
   /// @param   j   Row desired.
   static U32 idx(U32 i, U32 j) { return (i + j*4); }

   /// Initialize matrix to rotate about origin by e.
   MatrixF& set( const EulerF &e);

   /// Initialize matrix to rotate about p by e.
   MatrixF& set( const EulerF &e, const Point3F& p);

   /// Initialize matrix with a cross product of p.
   MatrixF& setCrossProduct( const Point3F &p);

   /// Initialize matrix with a tensor product of p.
   MatrixF& setTensorProduct( const Point3F &p, const Point3F& q);

   operator F32*() { return (m); }              ///< Allow people to get at m.
   operator F32*() const { return (F32*)(m); }  ///< Allow people to get at m.

   bool isAffine() const;                       ///< Check to see if this is an affine matrix.
   bool isIdentity() const;                     ///< Checks for identity matrix.

   /// Make this an identity matrix.
   MatrixF& identity();

   /// Invert m.
   MatrixF& inverse();

   /// Take inverse without disturbing position data.
   ///
   /// Ie, take inverse of 3x3 submatrix.
   MatrixF& affineInverse();
   MatrixF& transpose();                        ///< Swap rows and columns.
   MatrixF& scale(const Point3F& p);            ///< M * Matrix(p) -> M

   EulerF toEuler() const;

   /// Compute the inverse of the matrix.
   ///
   /// Computes inverse of full 4x4 matrix. Returns false and performs no inverse if
   /// the determinant is 0.
   ///
   /// Note: In most cases you want to use the normal inverse function.  This method should
   ///       be used if the matrix has something other than (0,0,0,1) in the bottom row.
   bool fullInverse();

   /// Swaps rows and columns into matrix.
   void transposeTo(F32 *matrix) const;

   /// Normalize the matrix.
   void normalize();

   /// Copy the requested column into a Point4F.
   void getColumn(S32 col, Point4F *cptr) const;

   /// Copy the requested column into a Point3F.
   ///
   /// This drops the bottom-most row.
   void getColumn(S32 col, Point3F *cptr) const;

   /// Set the specified column from a Point4F.
   void setColumn(S32 col, const Point4F& cptr);

   /// Set the specified column from a Point3F.
   ///
   /// The bottom-most row is not set.
   void setColumn(S32 col, const Point3F& cptr);

   /// Copy the specified row into a Point4F.
   void getRow(S32 row, Point4F *cptr) const;

   /// Copy the specified row into a Point3F.
   ///
   /// Right-most item is dropped.
   void getRow(S32 row, Point3F *cptr) const;

   /// Set the specified row from a Point4F.
   void setRow(S32 row, const Point4F& cptr);

   /// Set the specified row from a Point3F.
   ///
   /// The right-most item is not set.
   void setRow(S32 row, const Point3F& cptr);

   /// Get the position of the matrix.
   ///
   /// This is the 4th column of the matrix.
   Point3F   getPosition() const;

   /// Set the position of the matrix.
   ///
   /// This is the 4th column of the matrix.
   void      setPosition( const Point3F &pos ){ setColumn( 3, pos ); }

   /// Get the z axis of the matrix.
   ///
   /// This is the 3rd column of the matrix and is
   /// normally considered the up vector.   
   VectorF getUpVector() const;

   MatrixF&  mul(const MatrixF &a);                    ///< M * a -> M
   MatrixF&  mul(const MatrixF &a, const MatrixF &b);  ///< a * b -> M

   // Scalar multiplies
   MatrixF&  mul(const F32 a);                         ///< M * a -> M
   MatrixF&  mul(const MatrixF &a, const F32 b);       ///< a * b -> M


   void mul( Point4F& p ) const;                       ///< M * p -> p (full [4x4] * [1x4])
   void mulP( Point3F& p ) const;                      ///< M * p -> p (assume w = 1.0f)
   void mulP( const Point3F &p, Point3F *d) const;     ///< M * p -> d (assume w = 1.0f)
   void mulV( VectorF& p ) const;                      ///< M * v -> v (assume w = 0.0f)
   void mulV( const VectorF &p, Point3F *d) const;     ///< M * v -> d (assume w = 0.0f)

   void mul(Box3F& b) const;                           ///< Axial box -> Axial Box

   /// Convenience function to allow people to treat this like an array.
   F32& operator ()(S32 row, S32 col) { return m[idx(col,row)]; }

   void dumpMatrix(const char *caption=NULL) const;
} 
#if defined(__VEC__)
__attribute__ ((aligned (16)))
#endif
;

//--------------------------------------
// Inline Functions

inline MatrixF::MatrixF(bool _identity)
{
   if (_identity)
      identity();
}

inline MatrixF::MatrixF( const EulerF &e )
{
   set(e);
}

inline MatrixF::MatrixF( const EulerF &e, const Point3F& p )
{
   set(e,p);
}

inline MatrixF& MatrixF::set( const EulerF &e)
{
   m_matF_set_euler( e, *this );
   return (*this);
}


inline MatrixF& MatrixF::set( const EulerF &e, const Point3F& p)
{
   m_matF_set_euler_point( e, p, *this );
   return (*this);
}

inline MatrixF& MatrixF::setCrossProduct( const Point3F &p)
{
   m[1] = -(m[4] = p.z);
   m[8] = -(m[2] = p.y);
   m[6] = -(m[9] = p.x);
   m[0] = m[3] = m[5] = m[7] = m[10] = m[11] =
      m[12] = m[13] = m[14] = 0;
   m[15] = 1;
   return (*this);
}

inline MatrixF& MatrixF::setTensorProduct( const Point3F &p, const Point3F &q)
{
   m[0] = p.x * q.x;
   m[1] = p.x * q.y;
   m[2] = p.x * q.z;
   m[4] = p.y * q.x;
   m[5] = p.y * q.y;
   m[6] = p.y * q.z;
   m[8] = p.z * q.x;
   m[9] = p.z * q.y;
   m[10] = p.z * q.z;
   m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = 0;
   m[15] = 1;
   return (*this);
}

inline bool MatrixF::isIdentity() const
{
   return
   m[0]  == 1.0f &&
   m[1]  == 0.0f &&
   m[2]  == 0.0f &&
   m[3]  == 0.0f &&
   m[4]  == 0.0f &&
   m[5]  == 1.0f &&
   m[6]  == 0.0f &&
   m[7]  == 0.0f &&
   m[8]  == 0.0f &&
   m[9]  == 0.0f &&
   m[10] == 1.0f &&
   m[11] == 0.0f &&
   m[12] == 0.0f &&
   m[13] == 0.0f &&
   m[14] == 0.0f &&
   m[15] == 1.0f;
}

inline MatrixF& MatrixF::identity()
{
   m[0]  = 1.0f;
   m[1]  = 0.0f;
   m[2]  = 0.0f;
   m[3]  = 0.0f;
   m[4]  = 0.0f;
   m[5]  = 1.0f;
   m[6]  = 0.0f;
   m[7]  = 0.0f;
   m[8]  = 0.0f;
   m[9]  = 0.0f;
   m[10] = 1.0f;
   m[11] = 0.0f;
   m[12] = 0.0f;
   m[13] = 0.0f;
   m[14] = 0.0f;
   m[15] = 1.0f;
   return (*this);
}


inline MatrixF& MatrixF::inverse()
{
   m_matF_inverse(m);
   return (*this);
}

inline MatrixF& MatrixF::affineInverse()
{
//   AssertFatal(isAffine() == true, "Error, this matrix is not an affine transform");
   m_matF_affineInverse(m);
   return (*this);
}

inline MatrixF& MatrixF::transpose()
{
   m_matF_transpose(m);
   return (*this);
}

inline MatrixF& MatrixF::scale(const Point3F& p)
{
   m_matF_scale(m,p);
   return *this;
}

inline void MatrixF::normalize()
{
   m_matF_normalize(m);
}

inline MatrixF& MatrixF::mul( const MatrixF &a )
{  // M * a -> M
   MatrixF tempThis(*this);
   m_matF_x_matF(tempThis, a, *this);
   return (*this);
}


inline MatrixF& MatrixF::mul( const MatrixF &a, const MatrixF &b )
{  // a * b -> M
   m_matF_x_matF(a, b, *this);
   return (*this);
}


inline MatrixF& MatrixF::mul(const F32 a)
{
   for (U32 i = 0; i < 16; i++)
      m[i] *= a;

   return *this;
}


inline MatrixF& MatrixF::mul(const MatrixF &a, const F32 b)
{
   *this = a;
   mul(b);

   return *this;
}

inline void MatrixF::mul( Point4F& p ) const
{
   Point4F temp;
   m_matF_x_point4F(*this, &p.x, &temp.x);
   p = temp;
}

inline void MatrixF::mulP( Point3F& p) const
{
   // M * p -> d
   Point3F d;
   m_matF_x_point3F(*this, &p.x, &d.x);
   p = d;
}

inline void MatrixF::mulP( const Point3F &p, Point3F *d) const
{
   // M * p -> d
   m_matF_x_point3F(*this, &p.x, &d->x);
}

inline void MatrixF::mulV( VectorF& v) const
{
   // M * v -> v
   VectorF temp;
   m_matF_x_vectorF(*this, &v.x, &temp.x);
   v = temp;
}

inline void MatrixF::mulV( const VectorF &v, Point3F *d) const
{
   // M * v -> d
   m_matF_x_vectorF(*this, &v.x, &d->x);
}

inline void MatrixF::mul(Box3F& b) const
{
   m_matF_x_box3F(*this, &b.mMin.x, &b.mMax.x);
}

inline void MatrixF::getColumn(S32 col, Point4F *cptr) const
{
   cptr->x = m[col];
   cptr->y = m[col+4];
   cptr->z = m[col+8];
   cptr->w = m[col+12];
}

inline void MatrixF::getColumn(S32 col, Point3F *cptr) const
{
   cptr->x = m[col];
   cptr->y = m[col+4];
   cptr->z = m[col+8];
}

inline void MatrixF::setColumn(S32 col, const Point4F &cptr)
{
   m[col]   = cptr.x;
   m[col+4] = cptr.y;
   m[col+8] = cptr.z;
   m[col+12]= cptr.w;
}

inline void MatrixF::setColumn(S32 col, const Point3F &cptr)
{
   m[col]   = cptr.x;
   m[col+4] = cptr.y;
   m[col+8] = cptr.z;
}


inline void MatrixF::getRow(S32 col, Point4F *cptr) const
{
   col *= 4;
   cptr->x = m[col++];
   cptr->y = m[col++];
   cptr->z = m[col++];
   cptr->w = m[col];
}

inline void MatrixF::getRow(S32 col, Point3F *cptr) const
{
   col *= 4;
   cptr->x = m[col++];
   cptr->y = m[col++];
   cptr->z = m[col];
}

inline void MatrixF::setRow(S32 col, const Point4F &cptr)
{
   col *= 4;
   m[col++] = cptr.x;
   m[col++] = cptr.y;
   m[col++] = cptr.z;
   m[col]   = cptr.w;
}

inline void MatrixF::setRow(S32 col, const Point3F &cptr)
{
   col *= 4;
   m[col++] = cptr.x;
   m[col++] = cptr.y;
   m[col]   = cptr.z;
}

// not too speedy, but convienient
inline Point3F MatrixF::getPosition() const
{
   Point3F pos;
   getColumn( 3, &pos );
   return pos;
}

inline VectorF MatrixF::getUpVector() const
{
   VectorF vec;
   getColumn( 2, &vec );
   return vec;
}

#endif //_MMATRIX_H_
