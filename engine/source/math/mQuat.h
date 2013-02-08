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

#ifndef _MQUAT_H_
#define _MQUAT_H_

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

class MatrixF;
class QuatF;

inline F32 QuatIsEqual(F32 a,F32 b,F32 epsilon = 0.0001f)
{
   return mFabs(a-b) < epsilon;
}

inline F32 QuatIsZero(F32 a,F32 epsilon = 0.0001f)
{
   return mFabs(a) < epsilon;
}

//----------------------------------------------------------------------------
// rotation about an arbitrary axis through the origin:

class AngAxisF
{
  public:
   Point3F axis;
   F32  angle;

   AngAxisF();
   AngAxisF( const Point3F & _axis, F32 _angle );
   explicit AngAxisF( const MatrixF &m );
   explicit AngAxisF( const QuatF &q );

   AngAxisF& set( const Point3F & _axis, F32 _angle );
   AngAxisF& set( const MatrixF & m );
   AngAxisF& set( const QuatF & q );

   int operator ==( const AngAxisF & c ) const;
   int operator !=( const AngAxisF & c ) const;

   MatrixF * setMatrix( MatrixF * mat ) const;
};

//----------------------------------------------------------------------------
// unit quaternion class:

class QuatF
{
  public:
   F32  x,y,z,w;

   QuatF();
   QuatF( F32 _x, F32 _y, F32 _z, F32 w );
   QuatF( const MatrixF & m );
   QuatF( const AngAxisF & a );
   QuatF( const EulerF & e );

   QuatF& set( F32 _x, F32 _y, F32 _z, F32 _w );
   QuatF& set( const MatrixF & m );
   QuatF& set( const AngAxisF & m );
   QuatF& set( const EulerF & e );

   int operator ==( const QuatF & c ) const;
   int operator !=( const QuatF & c ) const;
   QuatF& operator *=( const QuatF & c );
   QuatF& operator /=( const QuatF & c );
   QuatF& operator +=( const QuatF & c );
   QuatF& operator -=( const QuatF & c );
   QuatF& operator *=( F32 a );
   QuatF& operator /=( F32 a );

   QuatF& square();
   QuatF& neg();
   F32  dot( const QuatF &q ) const;

   MatrixF* setMatrix( MatrixF * mat ) const;
   QuatF& normalize();
   QuatF& inverse();
   QuatF& identity();
   int    isIdentity() const;
   QuatF& slerp( const QuatF & q, F32 t );
   QuatF& extrapolate( const QuatF & q1, const QuatF & q2, F32 t );
   QuatF& interpolate( const QuatF & q1, const QuatF & q2, F32 t );
   F32  angleBetween( const QuatF & q );

   Point3F& mulP(const Point3F& a, Point3F* b);   // r = p * this
   QuatF& mul(const QuatF& a, const QuatF& b);    // This = a * b
};


//----------------------------------------------------------------------------
// AngAxisF implementation:

inline AngAxisF::AngAxisF()
{
}

inline AngAxisF::AngAxisF( const Point3F & _axis, F32 _angle )
{
   set(_axis,_angle);
}

inline AngAxisF::AngAxisF( const MatrixF & mat )
{
   set(mat);
}

inline AngAxisF::AngAxisF( const QuatF & quat )
{
   set(quat);
}

inline AngAxisF& AngAxisF::set( const Point3F & _axis, F32 _angle )
{
   axis = _axis;
   angle = _angle;
   return *this;
}

inline int AngAxisF::operator ==( const AngAxisF & c ) const
{
   return QuatIsEqual(angle, c.angle) && (axis == c.axis);
}

inline int AngAxisF::operator !=( const AngAxisF & c ) const
{
   return !QuatIsEqual(angle, c.angle) || (axis != c.axis);
}

//----------------------------------------------------------------------------
// quaternion implementation:

inline QuatF::QuatF()
{
}

inline QuatF::QuatF( F32 _x, F32 _y, F32 _z, F32 _w )
{
   set( _x, _y, _z, _w );
}

inline QuatF::QuatF( const AngAxisF & a )
{
   set( a );
}

inline QuatF::QuatF( const EulerF & e )
{
   set(e);
}

inline QuatF& QuatF::set( F32 _x, F32 _y, F32 _z, F32 _w )
{
   x = _x;
   y = _y;
   z = _z;
   w = _w;
   return *this;
}

inline int QuatF::operator ==( const QuatF & c ) const
{
   QuatF a = *this;
   QuatF b = c;
   a.normalize();
   b.normalize();
   b.inverse();
   a *= b;
   return a.isIdentity();
}

inline int QuatF::isIdentity() const
{
   return QuatIsZero( x ) && QuatIsZero( y ) && QuatIsZero( z );
}

inline QuatF& QuatF::identity()
{
   x = 0.0f;
   y = 0.0f;
   z = 0.0f;
   w = 1.0f;
   return *this;
}

inline int QuatF::operator !=( const QuatF & c ) const
{
   return ! operator==( c );
}

inline QuatF::QuatF( const MatrixF & m )
{
   set( m );
}


inline QuatF& QuatF::neg()
{
   x = -x;
   y = -y;
   z = -z;
   w = -w;
   return *this;
}

inline F32 QuatF::dot( const QuatF &q ) const
{
   return (w*q.w + x*q.x + y*q.y + z*q.z);
}


inline F32 QuatF::angleBetween( const QuatF & q )
{
   // angle between to quaternions
   return mAcos(x * q.x + y * q.y + z * q.z + w * q.w);
}


//----------------------------------------------------------------------------
// TQuatF classes:

class TQuatF : public QuatF
{
  public:
   enum
   {
      Matrix_HasRotation = 1,
      Matrix_HasTranslation = 2,
      Matrix_HasScale = 4
   };

   Point3F p;
   U32 flags;

   TQuatF();
   TQuatF( bool ident );
   TQuatF( const EulerF & e, const Point3F & p );
   TQuatF( const AngAxisF & aa, const Point3F & p );
   TQuatF( const QuatF & q, const Point3F & p );

   TQuatF& set( const EulerF & euler, const Point3F & p );
   TQuatF& set( const AngAxisF & aa, const Point3F & p );
   TQuatF& set( const QuatF & quat, const Point3F & p );

   TQuatF& inverse( void );
   TQuatF& identity( void );

   Point3F& mulP(const Point3F& p, Point3F* r);   // r = p * this
};

//---------------------------------------------------------------------------

inline TQuatF::TQuatF()
{
   // Beware: no initialization is done!
}

inline TQuatF::TQuatF( bool ident )
{
   if( ident )
      identity();
}

inline TQuatF::TQuatF( const EulerF & euler, const Point3F & p )
{
   set( euler, p );
}

inline TQuatF::TQuatF( const AngAxisF & aa, const Point3F & p )
{
   set( aa, p );
}

inline TQuatF::TQuatF( const QuatF & quat, const Point3F & p )
{
   set( quat, p );
}

inline TQuatF& TQuatF::set( const EulerF & e, const Point3F & t )
{
   p = t;
   QuatF::set( e );
   flags |= Matrix_HasTranslation;
   return *this;
}

inline TQuatF& TQuatF::set( const AngAxisF & aa, const Point3F & t )
{
   p = t;
   QuatF::set( aa );
   flags |= Matrix_HasTranslation;
   return *this;
}

inline TQuatF& TQuatF::set( const QuatF & q, const Point3F & t )
{
   p = t;
   QuatF::set( q.x, q.y, q.z, q.w );
   flags |= Matrix_HasTranslation;
   return *this;
}

inline TQuatF& TQuatF::inverse( void )
{
   QuatF::inverse();
   if( flags & Matrix_HasTranslation )
   {
      Point3F p2 = p;
      p2.neg();
      QuatF::mulP(p2,&p);
   }
   return *this;
}

inline TQuatF& TQuatF::identity( void )
{
   QuatF::identity();
   p.set(0,0,0);
   flags &= ~U32(Matrix_HasTranslation);
   return *this;
}

#endif
