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


#ifdef TORQUE_OS_IOS
#import <GLKit/GLKMath.h>
#endif

/// 4x4 Matrix Class
///
/// This runs at F32 precision.
class MatrixF
{
public:
    union
    {
   F32 m[16];     ///< Note: this is stored in ROW MAJOR format.  OpenGL is
                  ///  COLUMN MAJOR.  Transpose before sending down.
        
#ifdef __GLK_MATRIX_4_H
        GLKMatrix4 mGM;
#endif        
    };

public:
   /// Create an uninitialized matrix.
   ///
   /// @param   identity    If true, initialize to the identity matrix.
   explicit MatrixF(bool identity=false);

   /// Create a matrix to rotate about origin by e.
   /// @see set
   explicit MatrixF( const EulerF &e);
    
   explicit MatrixF( F32* f);

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
   /// Swap rows and columns.
   MatrixF& transpose();   /// M * Matrix(p) -> M
   MatrixF& scale( const Point3F &s );            
   MatrixF& scale( F32 s ) { return scale( Point3F( s, s, s ) ); }

    /// Return scale assuming scale was applied via mat.scale(s).
    Point3F getScale() const;
    
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
   Point4F getColumn4F(S32 col) const { Point4F ret; getColumn(col,&ret); return ret; }

   /// Copy the requested column into a Point3F.
   ///
   /// This drops the bottom-most row.
   void getColumn(S32 col, Point3F *cptr) const;
   Point3F getColumn3F(S32 col) const { Point3F ret; getColumn(col,&ret); return ret; }

   /// Set the specified column from a Point4F.
   void setColumn(S32 col, const Point4F& cptr);

   /// Set the specified column from a Point3F.
   ///
   /// The bottom-most row is not set.
   void setColumn(S32 col, const Point3F& cptr);

   /// Copy the specified row into a Point4F.
   void getRow(S32 row, Point4F *cptr) const;
   Point4F getRow4F(S32 row) const { Point4F ret; getRow(row,&ret); return ret; }

   /// Copy the specified row into a Point3F.
   ///
   /// Right-most item is dropped.
   void getRow(S32 row, Point3F *cptr) const;
   Point3F getRow3F(S32 row) const { Point3F ret; getRow(row,&ret); return ret; }

   /// Set the specified row from a Point4F.
   void setRow(S32 row, const Point4F& cptr);

   /// Set the specified row from a Point3F.
   ///
   /// The right-most item is not set.
   void setRow(S32 row, const Point3F& cptr);

   /// Get the position of the matrix.
   ///
   /// This is the 4th column of the matrix.
   Point3F getPosition() const;
   Point2F getXYPosition() const;

   /// Set the position of the matrix.
   ///
   /// This is the 4th column of the matrix.
   void setPosition( const Point3F &pos ) { setColumn( 3, pos ); }

   /// Add the passed delta to the matrix position.
   void displace( const Point3F &delta );

   /// Get the x axis of the matrix.
   ///
   /// This is the 1st column of the matrix and is
   /// normally considered the right vector.
   VectorF getRightVector() const;

   /// Get the y axis of the matrix.
   ///
   /// This is the 2nd column of the matrix and is
   /// normally considered the forward vector.   
   VectorF getForwardVector() const;   

   /// Get the z axis of the matrix.
   ///
   /// This is the 3rd column of the matrix and is
   /// normally considered the up vector.   
   VectorF getUpVector() const;

    inline void makeXRotation(float radians);
    inline void rotateX(float radians);
    inline void makeYRotation(float radians);
    inline void rotateY(float radians);
    inline void makeZRotation(float radians);
    inline void rotateZ(float radians);

    inline void translate( float tx, float ty, float tz);
    inline void translate( Point3F vec );
    
    inline void setFustrum(float left, float right,
                           float bottom, float top,
                           float nearZ, float farZ);
    
    inline void setOrtho(float left, float right,
                         float bottom, float top,
                         float nearZ, float farZ);
    inline void setLookAt(float eyeX, float eyeY, float eyeZ,
                          float centerX, float centerY, float centerZ,
                          float upX, float upY, float upZ);



   MatrixF&  mul(const MatrixF &a);                    ///< M * a -> M
   MatrixF&  mulL(const MatrixF &a);                   ///< a * M -> M
   MatrixF&  mul(const MatrixF &a, const MatrixF &b);  ///< a * b -> M

   // Scalar multiplies
   MatrixF&  mul(const F32 a);                         ///< M * a -> M
   MatrixF&  mul(const MatrixF &a, const F32 b);       ///< a * b -> M


   void mul( Point4F& p ) const;                       ///< M * p -> p (full [4x4] * [1x4])
   void mulP( Point2F& p ) const;                      ///< M * p -> p (assume z = 1.0f, w = 1.0f)
   void mulP( Point3F& p ) const;                      ///< M * p -> p (assume w = 1.0f)
   void mulP( const Point3F &p, Point3F *d) const;     ///< M * p -> d (assume w = 1.0f)
   void mulV( VectorF& p ) const;                      ///< M * v -> v (assume w = 0.0f)
   void mulV( const VectorF &p, Point3F *d) const;     ///< M * v -> d (assume w = 0.0f)

   void mul(Box3F& b) const;                           ///< Axial box -> Axial Box
   
   MatrixF& add( const MatrixF& m );

   /// Convenience function to allow people to treat this like an array.
   F32& operator ()(S32 row, S32 col) { return m[idx(col,row)]; }
   F32 operator ()(S32 row, S32 col) const { return m[idx(col,row)]; }

   void dumpMatrix(const char *caption=NULL) const;
   // Math operator overloads
   //------------------------------------
   friend MatrixF operator * ( const MatrixF &m1, const MatrixF &m2 );
   MatrixF& operator *= ( const MatrixF &m );

   // Static identity matrix
   const static MatrixF Identity;
}
#if defined(__VEC__)
__attribute__ ((aligned (16)))
#endif
;

//--------------------------------------
// Inline Functions

inline MatrixF& MatrixF::identity()
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4Identity;
#else
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
#endif
    return (*this);
}


inline MatrixF::MatrixF(bool _identity)
{
   if (_identity)
      identity();
}

inline MatrixF::MatrixF( const EulerF &e )
{
   set(e);
}

inline MatrixF::MatrixF( float values[16])
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4MakeWithArray(values);
#else
    for (int i=0; i<16; i++)
        m[i] = values[i];
#endif
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
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4Transpose( mGM );
#else
    m_matF_transpose(m);
#endif
    return (*this);
}

inline MatrixF& MatrixF::scale(const Point3F& p)
{
#ifdef __GLK_MATRIX_4_H
    transpose();
    mGM = GLKMatrix4ScaleWithVector3( mGM, GLKVector3Make( p.x, p.y, p.z ));
    transpose();
#else
    m_matF_scale(m,p);
#endif
    return *this;
}

inline void MatrixF::normalize()
{
    m_matF_normalize(m);
}

inline MatrixF& MatrixF::mul( const MatrixF &a )
{  // M * a -> M
//    MatrixF tempThis(*this);
//    m_matF_x_matF(tempThis, a, *this);
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4Multiply( mGM, a.mGM);
#else
    MatrixF tempThis(*this);
    m_matF_x_matF(tempThis, a, *this);
#endif
    return (*this);
}


inline MatrixF& MatrixF::mul( const MatrixF &a, const MatrixF &b )
{  // a * b -> M
//    m_matF_x_matF(a, b, *this);
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4Multiply( a.mGM, b.mGM);
#else
    m_matF_x_matF(a, b, *this);
#endif
    return (*this);
}


inline void MatrixF::mul( Point4F& p ) const
{
#ifdef __GLK_MATRIX_4_H
    p.mGV = GLKMatrix4MultiplyVector4( mGM, p.mGV);
#else
    Point4F temp;
    m_matF_x_point4F(*this, &p.x, &temp.x);
    p = temp;
#endif
}


inline void MatrixF::mulP( Point2F& p) const
{
    Point3F pn = Point3F(p.x, p.y, 1.0);
    
#ifdef __GLK_MATRIX_4_H
    pn.mGV = GLKMatrix4MultiplyVector3( mGM, pn.mGV);
#else
    Point3F d;
    m_matF_x_point3F(*this, &pn.x, &d.x);
    pn = d;
#endif
    p.x = pn.x;
    p.y = pn.y;
}


inline void MatrixF::mulP( Point3F& p) const
{
#ifdef __GLK_MATRIX_4_H
    p.mGV = GLKMatrix4MultiplyVector3( mGM, p.mGV);
#else
    Point3F d;
    m_matF_x_point3F(*this, &p.x, &d.x);
    p = d;
#endif
}

inline void MatrixF::mulP( const Point3F &p, Point3F *d) const
{
#ifdef __GLK_MATRIX_4_H
    d->mGV = GLKMatrix4MultiplyVector3( mGM, p.mGV);
#else
    m_matF_x_point3F(*this, &p.x, &d->x);
#endif
}

inline void MatrixF::mulV( VectorF& v) const
{
    // M * v -> v
#ifdef __GLK_MATRIX_4_H
    v.mGV = GLKMatrix4MultiplyVector3( mGM, v.mGV);
#else
    VectorF temp;
    m_matF_x_vectorF(*this, &v.x, &temp.x);
    v = temp;
#endif
}

inline void MatrixF::mulV( const VectorF &v, Point3F *d) const
{
    // M * v -> d
#ifdef __GLK_MATRIX_4_H
    d->mGV = GLKMatrix4MultiplyVector3( mGM, v.mGV);
#else
    m_matF_x_vectorF(*this, &v.x, &d->x);
#endif
}

inline void MatrixF::mul(Box3F& b) const
{
   m_matF_x_box3F(*this, &b.mMin.x, &b.mMax.x);
}

inline void MatrixF::getColumn(S32 col, Point4F *cptr) const
{
#ifdef __GLK_MATRIX_4_H
    cptr->mGV = GLKMatrix4GetRow( mGM, col );
#else
    cptr->x = m[col];
    cptr->y = m[col+4];
    cptr->z = m[col+8];
    cptr->w = m[col+12];
#endif
}

inline void MatrixF::getColumn(S32 col, Point3F *cptr) const
{
   cptr->x = m[col];
   cptr->y = m[col+4];
   cptr->z = m[col+8];
}

inline void MatrixF::setColumn(S32 col, const Point4F &cptr)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4SetRow( mGM, col, cptr.mGV);
#else
    m[col]   = cptr.x;
    m[col+4] = cptr.y;
    m[col+8] = cptr.z;
    m[col+12]= cptr.w;
#endif
}

inline void MatrixF::setColumn(S32 col, const Point3F &cptr)
{
    m[col]   = cptr.x;
    m[col+4] = cptr.y;
    m[col+8] = cptr.z;
}


inline void MatrixF::getRow(S32 col, Point4F *cptr) const
{
#ifdef __GLK_MATRIX_4_H
    cptr->mGV = GLKMatrix4GetColumn( mGM, col);
#else
    col *= 4;
    cptr->x = m[col++];
    cptr->y = m[col++];
    cptr->z = m[col++];
    cptr->w = m[col];
#endif
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
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4SetColumn( mGM, col, cptr.mGV);
#else
    col *= 4;
    m[col++] = cptr.x;
    m[col++] = cptr.y;
    m[col++] = cptr.z;
    m[col]   = cptr.w;
#endif
}

inline void MatrixF::setRow(S32 col, const Point3F &cptr)
{
   col *= 4;
   m[col++] = cptr.x;
   m[col++] = cptr.y;
   m[col]   = cptr.z;
}

// 2d conversion
inline Point2F MatrixF::getXYPosition() const
{
    Point3F pos;
    getColumn(3, &pos );
    return Point2F(pos.x, pos.y);
}

// not too speedy, but convienient
inline Point3F MatrixF::getPosition() const
{
   Point3F pos;
   getColumn( 3, &pos );
   return pos;
}

inline void MatrixF::makeXRotation(float radians)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4MakeXRotation( radians);
#else
    float cos = cosf(radians);
    float sin = sinf(radians);
    m[0]  = 1.0f;
    m[1]  = 0.0f;
    m[2]  = 0.0f;
    m[3]  = 0.0f;
    m[4]  = 0.0f;
    m[5]  = cos;
    m[6]  = sin;
    m[7]  = 0.0f;
    m[8]  = 0.0f;
    m[9]  = -sin;
    m[10] = cos;
    m[11] = 0.0f;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
#endif
}

inline void MatrixF::rotateX(float radians)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4RotateX( mGM , radians);
#else
    MatrixF rm;
    rm.makeXRotation(radians);
    mul(rm);
#endif
}

inline void MatrixF::makeYRotation(float radians)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4MakeYRotation( radians);
#else
    float cos = cosf(radians);
    float sin = sinf(radians);
    
    m[0]  = cos;
    m[1]  = 0.0f;
    m[2]  = -sin;
    m[3]  = 0.0f;
    m[4]  = 0.0f;
    m[5]  = 1.0f;
    m[6]  = 0.0f;
    m[7]  = 0.0f;
    m[8]  = sin;
    m[9]  = 0.0f;
    m[10] = cos;
    m[11] = 0.0f;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
#endif
}

inline void MatrixF::rotateY(float radians)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4RotateY( mGM , radians);
#else
    MatrixF rm;
    rm.makeYRotation(radians);
    mul(rm);
#endif
}

inline void MatrixF::makeZRotation(float radians)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4MakeZRotation( radians);
#else
    float cos = cosf(radians);
    float sin = sinf(radians);
    
    m[0]  = cos;
    m[1]  = sin;
    m[2]  = 0.0f;
    m[3]  = 0.0f;
    m[4]  = -sin;
    m[5]  = cos;
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
#endif
}

inline void MatrixF::rotateZ(float radians)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4RotateZ( mGM , radians);
#else
    MatrixF rm;
    rm.makeZRotation(radians);
    mul(rm);
#endif
}


inline MatrixF& MatrixF::mulL( const MatrixF &a )
{  // a * M -> M
   AssertFatal(&a != this, "MatrixF::mulL - a.mul(a) is invalid!");

   MatrixF tempThis(*this);
   m_matF_x_matF(a, tempThis, *this);
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



inline MatrixF& MatrixF::add( const MatrixF& a )
{
   for( U32 i = 0; i < 16; ++ i )
      m[ i ] += a.m[ i ];
      
   return *this;
}

//------------------------------------
// Math operator overloads
//------------------------------------
inline MatrixF operator * ( const MatrixF &m1, const MatrixF &m2 )
{
   // temp = m1 * m2
   MatrixF temp;
   m_matF_x_matF(m1, m2, temp);
   return temp;
}

inline MatrixF& MatrixF::operator *= ( const MatrixF &m )
{
   MatrixF tempThis(*this);
   m_matF_x_matF(tempThis, m, *this);
   return (*this);
}

//------------------------------------
// Non-member methods
//------------------------------------

inline void mTransformPlane(const MatrixF& mat, const Point3F& scale, const PlaneF&  plane, PlaneF * result)
{
   m_matF_x_scale_x_planeF(mat, &scale.x, &plane.x, &result->x);
}


inline Point3F MatrixF::getScale() const
{
    Point3F scale;
    scale.x = mSqrt(m[0]*m[0] + m[4] * m[4] + m[8] * m[8]);
    scale.y = mSqrt(m[1]*m[1] + m[5] * m[5] + m[9] * m[9]);
    scale.z = mSqrt(m[2]*m[2] + m[6] * m[6] + m[10] * m[10]);
    return scale;
}

inline void MatrixF::displace( const Point3F &delta )
{
    m[3]   += delta.x;
    m[3+4] += delta.y;
    m[3+8] += delta.z;
}

inline VectorF MatrixF::getForwardVector() const
{
    VectorF vec;
    getColumn( 1, &vec );
    return vec;
}

inline VectorF MatrixF::getRightVector() const
{
    VectorF vec;
    getColumn( 0, &vec );
    return vec;
}

inline VectorF MatrixF::getUpVector() const
{
    VectorF vec;
    getColumn( 2, &vec );
    return vec;
}

inline void MatrixF::translate( float tx, float ty, float tz)
{
    m[3] = m[0] * tx + m[1] * ty + m[2] * tz + m[3];
    m[7] = m[4] * tx + m[5] * ty + m[6] * tz + m[7];
    m[11] = m[8] * tx + m[9] * ty + m[10] * tz + m[11];
}

inline void MatrixF::translate( Point3F vec )
{
    translate(vec.m[0], vec.m[1], vec.m[2]);
}

inline void MatrixF::setFustrum(float left, float right,
                                float bottom, float top,
                                float nearZ, float farZ)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4MakeFrustum(left, right, bottom, top, nearZ, farZ);
    this->transpose();
#else
    float ral = right + left;
    float rsl = right - left;
    float tsb = top - bottom;
    float tab = top + bottom;
    float fan = farZ + nearZ;
    float fsn = farZ - nearZ;
    
    m[0] = 2.0f * nearZ / rsl;
    m[4] = 0.0f;
    m[8] = 0.0f;
    m[12] = 0.0f;
    m[1] = 0.0f;
    m[5] = 2.0f * nearZ / tsb;
    m[9] = 0.0f;
    m[13] = 0.0f;
    m[2] = ral / rsl;
    m[6] = tab / tsb;
    m[10] = -fan / fsn;
    m[14] = -1.0f;
    m[3] = 0.0f;
    m[7] = 0.0f;
    m[11] = (-2.0f * farZ * nearZ) / fsn;
    m[15] = 0.0f;
#endif
}


inline void MatrixF::setOrtho(float left, float right,
                              float bottom, float top,
                              float nearZ, float farZ)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4MakeOrtho(left, right, bottom, top, nearZ, farZ);
    this->transpose();
#else
    float ral = right + left;
    float rsl = right - left;
    float tab = top + bottom;
    float tsb = top - bottom;
    float fan = farZ + nearZ;
    float fsn = farZ - nearZ;
    
    m[0] = 2.0f / rsl;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = -ral / rsl;
    m[4] = 0.0f;
    m[5] = 2.0f / tsb;
    m[6] = 0.0f;
    m[7] = -tab / tsb;
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = -2.0f / fsn;
    m[11] = -fan / fsn;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
#endif
}

inline void MatrixF::setLookAt(float eyeX, float eyeY, float eyeZ,
                               float centerX, float centerY, float centerZ,
                               float upX, float upY, float upZ)
{
#ifdef __GLK_MATRIX_4_H
    mGM = GLKMatrix4MakeLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
    this->transpose();
#else
    Point3F ev = Point3F( eyeX, eyeY, eyeZ);
    Point3F cv = Point3F( centerX, centerY, centerZ);
    Point3F uv = Point3F( upX, upY, upZ);
    Point3F n = cv;
    n.neg();
    n += ev;
    n.normalize();
    Point3F u = uv;
    u *= n;
    u.normalize();
    Point3F v = n;
    v *= u;
    
    m[0] = u[0];
    m[4] = v[0];
    m[8] = n[0];
    m[12] = 0.0f;
    m[1] = u[1];
    m[5] = v[1];
    m[9] = n[1];
    m[13] = 0.0f;
    m[2] = u[2];
    m[6] = v[2];
    m[10] = n[2];
    m[14] = 0.0f;
    
    u.neg();
    v.neg();
    n.neg();
    
    m[3] = u[0] * ev[0] + u[1] * ev[1] + u[2] * ev[2];
    m[7] = v[0] * ev[0] + v[1] * ev[1] + v[2] * ev[2];
    m[11] = n[0] * ev[0] + n[1] * ev[1] + n[2] * ev[2];
    m[15] = 1.0f;
#endif
}

#endif //_MMATRIX_H_
