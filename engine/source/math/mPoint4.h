//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

#ifndef _MPOINT4_H_
#define _MPOINT4_H_

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif

#ifndef _MPOINT3_H_
#include "math/mPoint3.h"
#endif

#ifdef TORQUE_OS_IOS
#import <GLKit/GLKMath.h>
#endif


class Point4F;

//------------------------------------------------------------------------------
/// 4D integer point
///
/// Uses S32 internally. Currently storage only.
class Point4I
{
public:
    union
    {
        struct { U8 red, green, blue, alpha; };
        U8 v[4];
        struct { U8 x, y, z, w; };
    };

public:
    Point4I() { }
    Point4I(const Point4I& in_rCopy);
    Point4I(const U8 in_r,
            const U8 in_g,
            const U8 in_b,
            const U8 in_a = U8(255));
    
    void set(const U8 in_r,
             const U8 in_g,
             const U8 in_b,
             const U8 in_a = U8(255));
    
    void set(const Point4F in_p);
    
    Point4I& operator*=(const F32 in_mul);
    Point4I  operator*(const F32 in_mul) const;
    
    bool operator==(const Point4I&) const;
    bool operator!=(const Point4I&) const;
    
    void interpolate(const Point4I& in_rC1,
                     const Point4I& in_rC2,
                     const F32  in_factor);
    
    U32 getARGBPack() const;
    U32 getRGBAPack() const;
    U32 getABGRPack() const;
    
    U32 getBGRPack() const;
    U32 getRGBPack() const;
    
    U32 getRGBEndian() const;
    U32 getARGBEndian() const;
    
    U16 get565()  const;
    U16 get4444() const;
    
    operator Point4F() const;
    
    operator const U8*() const { return &red; }
	//-------------------------------------- Public static constants
public:
	const static Point4I One;
	const static Point4I Zero;

    static const Point4I ZERO;
    static const Point4I ONE;
    static const Point4I WHITE;
    static const Point4I BLACK;
    static const Point4I RED;
    static const Point4I GREEN;
    static const Point4I BLUE;

};

typedef Point4I Color4I;
//typedef Point4I ColorI;

//------------------------------------------------------------------------------
/// 4D floating-point point.
///
/// Uses F32 internally.
///
/// Useful for representing quaternions and other 4d beasties.
class Point4F
{
   //-------------------------------------- Public data
  public:
    union 
    {
        struct { F32 red, green, blue, alpha; };
        float v[4];
        struct { F32 x, y, z, w; };
#ifdef __GLK_VECTOR_4_H
        GLKVector4 mGV;
#endif        
    };

    
  public:
   Point4F();               ///< Create an unitialized point.
   Point4F(const Point4F&); ///< Copy constructor.

   /// Create point from coordinates.
   Point4F(const F32 _x, const F32 _y, const F32 _z, const F32 _w = 1.0f);

#ifdef __GLK_VECTOR_4_H
    Point4F(const GLKVector4 in_vector);
#endif
   /// Set point's coordinates.
   void set(const F32 _x, const F32 _y, const F32 _z, const F32 _w = 1.0f);
   void set (const Point4F input);

   /// Interpolate from _pt1 to _pt2, based on _factor.
   ///
   /// @param   _pt1    Starting point.
   /// @param   _pt2    Ending point.
   /// @param   _factor Interpolation factor (0.0 .. 1.0).
   void interpolate(const Point4F& _pt1, const Point4F& _pt2, F32 _factor);

   operator F32*() { return (&x); }
   operator const F32*() const { return &x; }
   
   F32 len() const;

   Point4F operator/(F32) const;

   Point4F operator*(F32) const;
   Point4F  operator+(const Point4F&) const;
   Point4F& operator+=(const Point4F&);
   Point4F  operator-(const Point4F&) const;      
   Point4F operator*(const Point4F&) const;
   Point4F& operator*=(const Point4F&);
   Point4F& operator=(const Point3F&);
   Point4F& operator=(const Point4F&);
   
   Point3F asPoint3F() const { return Point3F(x,y,z); }

    Point4F operator-() const;
    bool operator==(const Point4F&) const;
    bool operator!=(const Point4F&) const;

    U32 getARGBPack() const;
    U32 getRGBAPack() const;
    U32 getBGRAPack() const;
    
    operator Point4I() const;
    
    bool isValidColor() const { return (red   >= 0.0f && red   <= 1.0f) &&
        (green >= 0.0f && green <= 1.0f) &&
        (blue  >= 0.0f && blue  <= 1.0f) &&
        (alpha >= 0.0f && alpha <= 1.0f); }

    void clamp();

	//-------------------------------------- Public static constants
  public:
	const static Point4F One;
	const static Point4F Zero;

    static const Point4F ZERO;
    static const Point4F ONE;
    static const Point4F WHITE;
    static const Point4F BLACK;
    static const Point4F RED;
    static const Point4F GREEN;
    static const Point4F BLUE;
};

typedef Point4F Vector4F;   ///< Points can be vectors!
//typedef Point4F ColorF;


//------------------------------------------------------------------------------
//-------------------------------------- INLINES (Point4I)
//
inline void Point4I::set(const U8 in_r,
                         const U8 in_g,
                         const U8 in_b,
                         const U8 in_a)
{
    red   = in_r;
    green = in_g;
    blue  = in_b;
    alpha = in_a;
}

inline void Point4I::set(const Point4F in_p)
{
    red   = U8(in_p.red   * 255.0f + 0.5);
    green = U8(in_p.green * 255.0f + 0.5);
    blue  = U8(in_p.blue  * 255.0f + 0.5);
    alpha = U8(in_p.alpha * 255.0f + 0.5);
}

inline Point4I::Point4I(const Point4I& in_rCopy)
{
    red   = in_rCopy.red;
    green = in_rCopy.green;
    blue  = in_rCopy.blue;
    alpha = in_rCopy.alpha;
}

inline Point4I::Point4I(const U8 in_r,
                        const U8 in_g,
                        const U8 in_b,
                        const U8 in_a)
{
    set(in_r, in_g, in_b, in_a);
}

inline Point4I& Point4I::operator*=(const F32 in_mul)
{
    red   = U8((F32(red)   * in_mul) + 0.5f);
    green = U8((F32(green) * in_mul) + 0.5f);
    blue  = U8((F32(blue)  * in_mul) + 0.5f);
    alpha = U8((F32(alpha) * in_mul) + 0.5f);
    
    return *this;
}

inline Point4I Point4I::operator*(const F32 in_mul) const
{
    Point4I temp(*this);
    temp *= in_mul;
    return temp;
}

inline bool Point4I::operator==(const Point4I& in_Cmp) const
{
    return (red == in_Cmp.red && green == in_Cmp.green && blue == in_Cmp.blue && alpha == in_Cmp.alpha);
}

inline bool Point4I::operator!=(const Point4I& in_Cmp) const
{
    return (red != in_Cmp.red || green != in_Cmp.green || blue != in_Cmp.blue || alpha != in_Cmp.alpha);
}

inline void Point4I::interpolate(const Point4I& in_rC1,
                                 const Point4I& in_rC2,
                                 const F32  in_factor)
{
    F32 f2= 1.0f - in_factor;
    red   = U8(((F32(in_rC1.red)   * f2) + (F32(in_rC2.red)   * in_factor)) + 0.5f);
    green = U8(((F32(in_rC1.green) * f2) + (F32(in_rC2.green) * in_factor)) + 0.5f);
    blue  = U8(((F32(in_rC1.blue)  * f2) + (F32(in_rC2.blue)  * in_factor)) + 0.5f);
    alpha = U8(((F32(in_rC1.alpha) * f2) + (F32(in_rC2.alpha) * in_factor)) + 0.5f);
}

inline U32 Point4I::getARGBPack() const
{
    return (U32(alpha) << 24) |
    (U32(red)   << 16) |
    (U32(green) <<  8) |
    (U32(blue)  <<  0);
}

inline U32 Point4I::getRGBAPack() const
{
    return (U32(alpha) <<  0) |
    (U32(red)   << 24) |
    (U32(green) << 16) |
    (U32(blue)  <<  8);
}

inline U32 Point4I::getABGRPack() const
{
    return (U32(alpha) << 24) |
    (U32(red)   << 16) |
    (U32(green) <<  8) |
    (U32(blue)  <<  0);
}


inline U32 Point4I::getBGRPack() const
{
    return (U32(blue)  << 16) |
    (U32(green) <<  8) |
    (U32(red)   <<  0);
}

inline U32 Point4I::getRGBPack() const
{
    return (U32(red)   << 16) |
    (U32(green) <<  8) |
    (U32(blue)  <<  0);
}

inline U32 Point4I::getRGBEndian() const
{
#if defined(TORQUE_BIG_ENDIAN)
    return(getRGBPack());
#else
    return(getBGRPack());
#endif
}

inline U32 Point4I::getARGBEndian() const
{
#if defined(TORQUE_BIG_ENDIAN)
    return(getABGRPack());
#else
    return(getARGBPack());
#endif
}

inline U16 Point4I::get565() const
{
    return U16((U16(red   >> 3) << 11) |
               (U16(green >> 2) <<  5) |
               (U16(blue  >> 3) <<  0));
}

inline U16 Point4I::get4444() const
{
    return U16(U16(U16(alpha >> 4) << 12) |
               U16(U16(red   >> 4) <<  8) |
               U16(U16(green >> 4) <<  4) |
               U16(U16(blue  >> 4) <<  0));
}

//------------------------------------------------------------------------------
//-------------------------------------- Point4F
//
inline Point4F::Point4F()
{
}

inline Point4F::Point4F(const Point4F& _copy)
 : x(_copy.x), y(_copy.y), z(_copy.z), w(_copy.w)
{
}

inline Point4F::Point4F(F32 _x, F32 _y, F32 _z, F32 _w)
 : x(_x), y(_y), z(_z), w(_w)
{
}

inline void Point4F::set(F32 _x, F32 _y, F32 _z, F32 _w)
{
   x = _x;
   y = _y;
   z = _z;
   w = _w;
}

inline void Point4F::set(Point4F input)
{
    x = input.x;
    y = input.y;
    z = input.z;
    w = input.w;
}

inline F32 Point4F::len() const
{
   return mSqrt(x*x + y*y + z*z + w*w);
}

inline void Point4F::interpolate(const Point4F& _from, const Point4F& _to, F32 _factor)
{
   x = (_from.x * (1.0f - _factor)) + (_to.x * _factor);
   y = (_from.y * (1.0f - _factor)) + (_to.y * _factor);
   z = (_from.z * (1.0f - _factor)) + (_to.z * _factor);
   w = (_from.w * (1.0f - _factor)) + (_to.w * _factor);
}

inline Point4F& Point4F::operator=(const Point3F &_vec)
{
   x = _vec.x;
   y = _vec.y;
   z = _vec.z;
   w = 1.0f;
   return *this;
}

inline Point4F& Point4F::operator=(const Point4F &_vec)
{
   x = _vec.x;
   y = _vec.y;
   z = _vec.z;
   w = _vec.w;

   return *this;
}

inline Point4F Point4F::operator+(const Point4F& _add) const
{
   return Point4F( x + _add.x, y + _add.y, z + _add.z, w + _add.w );
}

inline Point4F& Point4F::operator+=(const Point4F& _add)
{
   x += _add.x;
   y += _add.y;
   z += _add.z;
   w += _add.w;

   return *this;
}

inline Point4F Point4F::operator-(const Point4F& _rSub) const
{
   return Point4F( x - _rSub.x, y - _rSub.y, z - _rSub.z, w - _rSub.w );
}

inline Point4F Point4F::operator*(const Point4F &_vec) const
{
   return Point4F(x * _vec.x, y * _vec.y, z * _vec.z, w * _vec.w);
}

inline Point4F Point4F::operator*(F32 _mul) const
{
   return Point4F(x * _mul, y * _mul, z * _mul, w * _mul);
}

inline Point4F Point4F::operator/(const F32 in_div) const
{
    AssertFatal(in_div != 0.0f, "Error, div by zero...");
#ifdef __GLK_VECTOR_4_H
    return Point4F(GLKVector4DivideScalar( mGV, in_div));
#else
    F32 inv = 1.0f / in_div;
    
    return Point4F(red * inv,
                   green * inv,
                   blue  * inv,
                   alpha * inv);
#endif
}

inline Point4F Point4F::operator-() const
{
#ifdef __GLK_VECTOR_4_H
    return Point4F( GLKVector4Negate( mGV));
#else
    return Point4F(-red, -green, -blue, -alpha);
#endif
}

inline bool Point4F::operator==(const Point4F& in_Cmp) const
{
#ifdef __GLK_VECTOR_4_H
    return GLKVector4AllEqualToVector4( mGV, in_Cmp.mGV);
#else
    return (red == in_Cmp.red && green == in_Cmp.green && blue == in_Cmp.blue && alpha == in_Cmp.alpha);
#endif
}

inline bool Point4F::operator!=(const Point4F& in_Cmp) const
{
#ifdef __GLK_VECTOR_4_H
    return !GLKVector4AllEqualToVector4( mGV, in_Cmp.mGV);
#else
    return (red != in_Cmp.red || green != in_Cmp.green || blue != in_Cmp.blue || alpha != in_Cmp.alpha);
#endif
}


//-------------------------------------------------------------------
// Non-Member Operators
//-------------------------------------------------------------------

inline Point4F operator*(F32 mul, const Point4F& multiplicand)
{
   return multiplicand * mul;
}

inline bool mIsNaN( const Point4F &p )
{
   return mIsNaN_F( p.x ) || mIsNaN_F( p.y ) || mIsNaN_F( p.z ) || mIsNaN_F( p.w );
}

//-------------------------------------- INLINE CONVERSION OPERATORS
inline Point4F::operator Point4I() const
{
   return Point4I(U8(red   * 255.0f + 0.5),
                  U8(green * 255.0f + 0.5),
                  U8(blue  * 255.0f + 0.5),
                  U8(alpha * 255.0f + 0.5));
}

inline Point4I::operator Point4F() const
{
   const F32 inv255 = 1.0f / 255.0f;

   return Point4I(U8(F32(red)   * inv255),
                 U8(F32(green) * inv255),
                 U8(F32(blue)  * inv255),
                 U8(F32(alpha) * inv255));
}


#endif // _MPOINT4_H_
