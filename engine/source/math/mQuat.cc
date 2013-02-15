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

#include "math/mQuat.h"
#include "math/mMatrix.h"

QuatF& QuatF::set( const EulerF & e )
{
   F32 cx, sx;
   F32 cy, sy;
   F32 cz, sz;
   mSinCos( -e.x * F32(0.5), sx, cx );
   mSinCos( -e.y * F32(0.5), sy, cy );
   mSinCos( -e.z * F32(0.5), sz, cz );

   // Qyaw(z)   = [ (0, 0, sin z/2), cos z/2 ]
   // Qpitch(x) = [ (sin x/2, 0, 0), cos x/2 ]
   // Qroll(y)  = [ (0, sin y/2, 0), cos y/2 ]
   // this = Qresult = Qyaw*Qpitch*Qroll  ZXY
   //
   // The code that folows is a simplification of:
   //    roll*=pitch;
   //    roll*=yaw;
   //    *this = roll;
   F32 cycz, sysz, sycz, cysz;
   cycz = cy*cz;
   sysz = sy*sz;
   sycz = sy*cz;
   cysz = cy*sz;
   w = cycz*cx + sysz*sx;
   x = cycz*sx + sysz*cx;
   y = sycz*cx - cysz*sx;
   z = cysz*cx - sycz*sx;

   return *this;
}

AngAxisF & AngAxisF::set( const QuatF & q )
{
#ifdef __GLK_QUATERNION_H
    axis.mGV = GLKQuaternionAxis(q.mGQ);
    angle = GLKQuaternionAngle(q.mGQ);
#else
   angle = mAcos( q.w ) * 2;
   F32 sinHalfAngle = mSqrt(1 - q.w * q.w);
   if (sinHalfAngle != 0)
   	axis.set( q.x / sinHalfAngle, q.y / sinHalfAngle, q.z / sinHalfAngle );
   else
      axis.set(1,0,0);
#endif
   return *this;
}

AngAxisF & AngAxisF::set( const MatrixF & mat )
{
   QuatF q( mat );
   set( q );
   return *this;
}

MatrixF * AngAxisF::setMatrix( MatrixF * mat ) const
{
   QuatF q( *this );
   return q.setMatrix( mat );
}

QuatF& QuatF::operator *=( const QuatF & b )
{
   QuatF prod;
   prod.w = w * b.w - x * b.x - y * b.y - z * b.z;
   prod.x = w * b.x + x * b.w + y * b.z - z * b.y;
   prod.y = w * b.y + y * b.w + z * b.x - x * b.z;
   prod.z = w * b.z + z * b.w + x * b.y - y * b.x;
   *this = prod;
   return (*this);
}

QuatF& QuatF::operator /=( const QuatF & c )
{
   QuatF temp = c;
   return ( (*this) *= temp.inverse() );
}

QuatF& QuatF::operator +=( const QuatF & c )
{
#ifdef __GLK_QUATERNION_H
    mGQ = GLKQuaternionAdd(mGQ, c.mGQ);
#else
   x += c.x;
   y += c.y;
   z += c.z;
   w += c.w;
#endif
   return *this;
}

QuatF& QuatF::operator -=( const QuatF & c )
{
#ifdef __GLK_QUATERNION_H
    mGQ = GLKQuaternionSubtract(mGQ, c.mGQ);
#else
   x -= c.x;
   y -= c.y;
   z -= c.z;
   w -= c.w;
#endif
   return *this;
}

QuatF& QuatF::operator *=( F32 a )
{
   x *= a;
   y *= a;
   z *= a;
   w *= a;
   return *this;
}

QuatF& QuatF::operator /=( F32 a )
{
   x /= a;
   y /= a;
   z /= a;
   w /= a;
   return *this;
}

QuatF& QuatF::square()
{
   F32 t = w*2.0f;
   w = (w*w) - (x*x + y*y + z*z);
   x *= t;
   y *= t;
   z *= t;
   return *this;
}

QuatF& QuatF::inverse()
{
#ifdef __GLK_QUATERNION_H
    mGQ = GLKQuaternionInvert(mGQ);
#else
   F32 magnitude = w*w + x*x + y*y + z*z;
   F32 invMagnitude;
   if( magnitude == 1.0f )    // special case unit quaternion
   {
      x = -x;
      y = -y;
      z = -z;
   }
   else                       // else scale
   {
      if( magnitude == 0.0f )
         invMagnitude = 1.0f;
      else
         invMagnitude = 1.0f / magnitude;
      w *= invMagnitude;
      x *= -invMagnitude;
      y *= -invMagnitude;
      z *= -invMagnitude;
   }
#endif
	return *this;
}

QuatF& QuatF::set( const AngAxisF & a )
{
#ifdef __GLK_QUATERNION_H
    mGQ = GLKQuaternionMakeWithAngleAndVector3Axis(a.angle, a.axis.mGV);
#else
   F32 sinHalfAngle, cosHalfAngle;
   mSinCos( a.angle * F32(0.5), sinHalfAngle, cosHalfAngle );
   x = a.axis.x * sinHalfAngle;
   y = a.axis.y * sinHalfAngle;
   z = a.axis.z * sinHalfAngle;
   w = cosHalfAngle;
#endif
   return *this;
}

QuatF & QuatF::normalize()
{
#ifdef __GLK_QUATERNION_H
    mGQ = GLKQuaternionNormalize(mGQ);
#else
   F32 l = mSqrt( x*x + y*y + z*z + w*w );
   if( l == F32(0.0) )
      identity();
   else
   {
      x /= l;
      y /= l;
      z /= l;
      w /= l;
   }
#endif
   return *this;
}

#define idx(r,c) (r*4 + c)

QuatF& QuatF::set( const MatrixF & mat )
{
#ifdef __GLK_QUATERNION_H
    mGQ = GLKQuaternionMakeWithMatrix4(mat.mGM);
#else
   F32 const *m = mat;

   F32 trace = m[idx(0, 0)] + m[idx(1, 1)] + m[idx(2, 2)];
   if (trace > 0.0) {
      F32 s = mSqrt(trace + F32(1));
      w = s * 0.5f;
      s = 0.5f / s;
      x = (m[idx(1,2)] - m[idx(2,1)]) * s;
      y = (m[idx(2,0)] - m[idx(0,2)]) * s;
      z = (m[idx(0,1)] - m[idx(1,0)]) * s;
   } else {
      F32* q = &x;
      U32 i = 0;
      if (m[idx(1, 1)] > m[idx(0, 0)]) i = 1;
      if (m[idx(2, 2)] > m[idx(i, i)]) i = 2;
      U32 j = (i + 1) % 3;
      U32 k = (j + 1) % 3;

      F32 s = mSqrt((m[idx(i, i)] - (m[idx(j, j)] + m[idx(k, k)])) + 1.0f);
      q[i] = s * 0.5f;
      s = 0.5f / s;
      q[j] = (m[idx(i,j)] + m[idx(j,i)]) * s;
      q[k] = (m[idx(i,k)] + m[idx(k, i)]) * s;
      w = (m[idx(j,k)] - m[idx(k, j)]) * s;
   }
#endif
   return *this;
}

MatrixF * QuatF::setMatrix( MatrixF * mat ) const
{
   if( x*x + y*y + z*z < 10E-20f) // isIdentity() -- substituted code a little more stringent but a lot faster
      mat->identity();
   else
      m_quatF_set_matF( x, y, z, w, *mat );
   return mat;
}

QuatF & QuatF::slerp( const QuatF & q, F32 t )
{
   return interpolate( *this, q, t );
}

QuatF & QuatF::extrapolate( const QuatF & q1, const QuatF & q2, F32 t )
{
   // assert t >= 0 && t <= 1
   // q1 is value at time = 0
   // q2 is value at time = t
   // Computes quaternion at time = 1
   F64 flip,cos = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
   if (cos < 0) {
      cos = -cos;
      flip = -1;
   }
   else
      flip = 1;

   F64 s1,s2;
   if ((1.0 - cos) > 0.00001) {
      F64 om = mAcos(cos) / t;
      F64 sd = 1 / mSin(t * om);
      s1 = flip * mSin(om) * sd;
      s2 = mSin((1 - t) * om) * sd;
   }
   else {
      // If quats are very close, do linear interpolation
      s1 = flip / t;
      s2 = (1 - t) / t;
   }

   x = F32(s1 * q2.x - s2 * q1.x);
   y = F32(s1 * q2.y - s2 * q1.y);
   z = F32(s1 * q2.z - s2 * q1.z);
   w = F32(s1 * q2.w - s2 * q1.w);

   return *this;
}

QuatF & QuatF::interpolate( const QuatF & q1, const QuatF & q2, F32 t )
{
#ifdef __GLK_QUATERNION_H
    mGQ = GLKQuaternionSlerp ( q1.mGQ, q2.mGQ , t);
#else
    //-----------------------------------
   // Calculate the cosine of the angle:

   double cosOmega = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

   //-----------------------------------
   // adjust signs if necessary:

   F32 sign2;
   if ( cosOmega < 0.0 )
   {
      cosOmega = -cosOmega;
      sign2 = -1.0f;
   }
   else
      sign2 = 1.0f;

   //-----------------------------------
   // calculate interpolating coeffs:

   double scale1, scale2;
   if ( (1.0 - cosOmega) > 0.00001 )
   {
      // standard case
      double omega = mAcos(cosOmega);
      double sinOmega = mSin(omega);
      scale1 = mSin((1.0 - t) * omega) / sinOmega;
      scale2 = sign2 * mSin(t * omega) / sinOmega;
   }
   else
   {
      // if quats are very close, just do linear interpolation
      scale1 = 1.0 - t;
      scale2 = sign2 * t;
   }


   //-----------------------------------
   // actually do the interpolation:

   x = F32(scale1 * q1.x + scale2 * q2.x);
   y = F32(scale1 * q1.y + scale2 * q2.y);
   z = F32(scale1 * q1.z + scale2 * q2.z);
   w = F32(scale1 * q1.w + scale2 * q2.w);
#endif
   return *this;
}

Point3F& QuatF::mulP(const Point3F& p, Point3F* r)
{
   QuatF qq;
   QuatF qi = *this;
   QuatF qv( p.x, p.y, p.z, 0);

   qi.inverse();
   qq.mul(qi, qv);
   qv.mul(qq, *this);
   r->set(qv.x, qv.y, qv.z);
   return *r;
}

QuatF& QuatF::mul( const QuatF &a, const QuatF &b)
{
	AssertFatal( &a != this && &b != this, "QuatF::mul: dest should not be same as source" );
   w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
   x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
   y = a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z;
   z = a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x;
   return *this;
}

Point3F& TQuatF::mulP(const Point3F& pt, Point3F* r)
{
   QuatF a;
   QuatF i = *this;
   QuatF v( pt.x, pt.y, pt.z, 0.0f);
   i.inverse();
   a.mul(i, v);
   v.mul(a, *this);
   v.normalize();
   r->set(v.x, v.y, v.z);

   *r += p;
   return ( *r );
}

