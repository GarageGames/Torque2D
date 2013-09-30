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

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

//-----------------------------------------------------------------------------

/*!
   @defgroup Vector2Math Vector2 Math
      @ingroup TorqueScriptFunctions
      @{
*/

//-----------------------------------------------------------------------------

/*! Add two 2-vectors.
	@return v1+v2

	@boundto
	Vector2::operator+
*/
ConsoleFunctionWithDocs( Vector2Add, ConsoleString, 3, 3, (Vector2 v1, Vector2 v2) )
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("Vector2Add() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 v1( argv[1] );
    Vector2 v2( argv[2] );

    return ( v1 + v2 ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Subtract two 2D vectors.
    @return v1-v2

	@boundto
	Vector2::operator-(Vector2)
*/
ConsoleFunctionWithDocs( Vector2Sub, ConsoleString, 3, 3,(Vector2 v1, Vector2 v2))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("Vector2Sub() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 v1( argv[1] );
    Vector2 v2( argv[2] );

    return ( v1 - v2 ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Absolute difference of two 2-vectors
	@return abs(v1-v2)

	@boundto
	Vector2::absolute
*/
ConsoleFunctionWithDocs( Vector2Abs, ConsoleString, 3, 3, (Vector2 v1, Vector2 v2) )
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("Vector2Sub() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 v1( argv[1] );
    Vector2 v2( argv[2] );

    return ( v1 - v2 ).absolute().scriptThis();
}

//-----------------------------------------------------------------------------
/*! Multiply two 2D vectors (Not Dot-Product!)
	@return v1 mult v2.
*/
ConsoleFunctionWithDocs( Vector2Mult, ConsoleString, 3, 3, (Vector2 v1, Vector2 v2))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("Vector2Mult() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 v1( argv[1] );
    Vector2 v2( argv[2] );

    return Vector2( v1.x * v2.x, v1.y * v2.y ).scriptThis();
}

//-----------------------------------------------------------------------------
/*! Scale a 2D vector.
	@return v1 scaled by scale.

	@boundto
	Vector2::operator*
*/

ConsoleFunctionWithDocs( Vector2Scale, ConsoleString, 3, 3, (Vector2 v1, scale))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("Vector2Scale() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 v1( argv[1] );

    v1 *= dAtof(argv[2]);

    return v1.scriptThis();
}

//-----------------------------------------------------------------------------
/*! Normalize a 2D vector.
	@return Normalized v1.

	@boundto
	Vector2::Normalize
*/
ConsoleFunctionWithDocs( Vector2Normalize, ConsoleString, 2, 2, (Vector2 v1))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("Vector2Normalize() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 v1( argv[1] );
    v1.Normalize();
    return v1.scriptThis();
}

//-----------------------------------------------------------------------------
/*! Dot-Product of two 2D vectors.
	@return dot-product of v1 and v2.

	@boundto
	Vector2::dot
*/
ConsoleFunctionWithDocs(Vector2Dot, ConsoleFloat, 3, 3, (Vector2 v1, Vector2 v2))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("Vector2Dot() - Invalid number of parameters!");
        return 0.0f;
    }

    Vector2 v1( argv[1] );
    Vector2 v2( argv[2] );

    return v1.dot( v2 );
}

//-----------------------------------------------------------------------------
/*! Equality of two 2D Points.
	Compares points p1 and p2 with optional difference (epsilon).
	@return true if equal
*/
ConsoleFunctionWithDocs( Vector2Compare, ConsoleBool, 3, 4, (Vector2 p1, Vector2 p2, [Vector2 epsilon=0.0001]?))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("Vector2Compare() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 p1( argv[1] );
    Vector2 p2( argv[2] );

    // Do Vector Operation.
    const F32 delta = (p2 - p1).Length();

    // Calculate Epsilon.
    const F32 epsilon = (argc >= 4) ? dAtof(argv[3]) : FLT_EPSILON;

    // Return  epsilon delta.
    return mIsEqualRange( delta, 0.0f, epsilon );
}

//-----------------------------------------------------------------------------
/*! Distance between two 2D Points.
	@return the distance between points p1 and p2
*/
ConsoleFunctionWithDocs( Vector2Distance, ConsoleFloat, 3, 3, (Vector2 p1, Vector2 p2))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("Vector2Distance() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 p1( argv[1] );
    Vector2 p2( argv[2] );

    return (p2 - p1).Length();
}

//-----------------------------------------------------------------------------
/*! Angle between two 2D vectors.
	@return the angle between v1 and v2.
*/
ConsoleFunctionWithDocs( Vector2AngleBetween, ConsoleFloat, 3, 3, (Vector2 v1, Vector2 v2))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dAngleBetween() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 v1( argv[1] );
    Vector2 v2( argv[2] );

    v1.Normalize();
    v2.Normalize();

    return mRadToDeg( mAcos( v1.dot(v2) ) );
}

//-----------------------------------------------------------------------------
/*! Angle from one point to another.
	@return the angle from p1 to p2.
*/
ConsoleFunctionWithDocs( Vector2AngleToPoint, ConsoleFloat, 3, 3, (Vector2 p1, Vector2 p2))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dAngleToPoint() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 p1( argv[1] );
    Vector2 p2( argv[2] );

    // Do Operation.
    return mRadToDeg( mAtan((p2.x - p1.x), (p2.y - p1.y)) );
}

//-----------------------------------------------------------------------------
/*! vector from angle and magnitude.
	Calculates a direction from an angle and magnitude.
    @param angle The angle of the direction.
    @param magnitude The magnitude of the direction.
    @return No return value.
*/
//-----------------------------------------------------------------------------
ConsoleFunctionWithDocs( Vector2Direction, ConsoleString, 3, 3, (F32 angle, F32 magnitude))
{
    // Fetch angle.
    const F32 angle = mDegToRad(dAtof(argv[1]));

    // Fetch magnitude.
    const F32 magnitude = dAtof(argv[2]);

    // Do Operation.
    Vector2 direction;
    direction.setPolar( angle, magnitude );
    return direction.scriptThis();
}

//-----------------------------------------------------------------------------
/*! Length of a 2D vector.
	@return the length of v1.

	@boundto
	Vector2::Length
*/
ConsoleFunctionWithDocs( Vector2Length, ConsoleFloat, 2, 2, (Vector2 v1))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("Vector2Length() - Invalid number of parameters!");
        return 0.0f;
    }

    Vector2 v1( argv[1] );

    return v1.Length();
}

//-----------------------------------------------------------------------------
/*! Inverse of a 2D vector.
	@return the inverse of v1.

	@boundto
	Vector2::operator-()
*/
//-----------------------------------------------------------------------------
ConsoleFunctionWithDocs( Vector2Inverse, ConsoleString, 2, 2, (Vector2 v1))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("Vector2Inverse() - Invalid number of parameters!");
        return StringTable->EmptyString;
    }

    Vector2 v1( argv[1] );

    return (-v1).scriptThis();
}

//-----------------------------------------------------------------------------
/*! Inverse X component of a 2D vector.
	@return the inverse of the v1 X component.
*/
ConsoleFunctionWithDocs( Vector2InverseX, ConsoleString, 2, 2, (Vector2 v1))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("Vector2InverseX() - Invalid number of parameters!");
        return StringTable->EmptyString;
    }

    Vector2 v1( argv[1] );
    v1.x = -v1.x;

    return v1.scriptThis();
}

//-----------------------------------------------------------------------------
/*! Inverse Y component of a 2D vector.
	@return the inverse of the v1 Y component.
*/
ConsoleFunctionWithDocs( Vector2InverseY, ConsoleString, 2, 2, (Vector2 v1))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 )
    {
        Con::warnf("Vector2InverseY() - Invalid number of parameters!");
        return StringTable->EmptyString;
    }

    Vector2 v1( argv[1] );
    v1.y = -v1.y;

    return v1.scriptThis();
}

//-----------------------------------------------------------------------------
/*! Normalize Rectangle (two 2D vectors) with relation to each other.
	@return Normalize rectangle of v1 and v2.
*/
ConsoleFunctionWithDocs( Vector2AreaNormalize, ConsoleString, 3, 3, (Vector2 v1, Vector2 v2))
{
    // Check Parameters.
    if (Utility::mGetStringElementCount(argv[1]) < 2 ||Utility::mGetStringElementCount(argv[2]) < 2 )
    {
        Con::warnf("t2dRectNormalize() - Invalid number of parameters!");
        return NULL;
    }

    Vector2 v1( argv[1] );
    Vector2 v2( argv[2] );

    // Do Vector Operation.
    Vector2 topLeft( (v1.x <= v2.x) ? v1.x : v2.x, (v1.y <= v2.y) ? v1.y : v2.y );
    Vector2 bottomRight( (v1.x > v2.x) ? v1.x : v2.x, (v1.y > v2.y) ? v1.y : v2.y );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer( 64 );

    // Format Buffer.
    dSprintf(pBuffer, 64, "%g %g %g %g", topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);

    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! @} */
