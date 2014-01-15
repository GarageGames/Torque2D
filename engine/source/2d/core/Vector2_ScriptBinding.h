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

ConsoleFunctionGroupBegin( Vector2Math, "Vector2 math functions.");

//-----------------------------------------------------------------------------

ConsoleFunction( Vector2Add, const char*, 3, 3, "(Vector2 v1, Vector2 v2) - Returns v1+v2.")
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
// Subtract two 2D Vectors.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Sub, const char*, 3, 3, "(Vector2 v1, Vector2 v2) - Returns v1-v2.")
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
// The absolute difference between two 2D Vectors.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Abs, const char*, 3, 3, "(Vector2 v1, Vector2 v2) - Returns abs(v1-v2).")
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
// Multiply two 2D Vectors (Not Dot-Product!)
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Mult, const char*, 3, 3, "(Vector2 v1, Vector2 v2) - Returns v1 mult v2.")
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
// Scale a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Scale, const char*, 3, 3, "(Vector2 v1, scale) - Returns v1 scaled by scale.")
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
// Normalize a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Normalize, const char*, 2, 2, "(Vector2 v1) - Returns Normalized v1.")
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
// Dot-Product of two 2D Vectors.
//-----------------------------------------------------------------------------
ConsoleFunction(Vector2Dot, F32, 3, 3, "(Vector2 v1, Vector2 v2) - Returns dot-product of v1 and v2.")
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
// Equality of two 2D Points.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Compare, bool, 3, 4, "(Vector2 p1, Vector2 p2, [epsilon]) - Compares points p1 and p2 with optional difference (epsilon).")
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
// Distance between two 2D Points.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Distance, F32, 3, 3, "(Vector2 p1, Vector2 p2) - Returns the distance between points p1 and p2.")
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
// Angle between two 2D Vectors.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2AngleBetween, F32, 3, 3, "(Vector2 v1, Vector2 v2) - Returns the angle between v1 and v2.")
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
// Angle from one point to another.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2AngleToPoint, F32, 3, 3, "(Vector2 p1, Vector2 p1) - Returns the angle from p1 to p2.")
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
    return mRadToDeg( mAtan((p2.x - p1.x), (p1.y - p2.y)) );
}

//-----------------------------------------------------------------------------
// Vector from angle and magnitude.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Direction, const char*, 3, 3,   "(F32 angle, F32 magnitude) - Calculates a direction from an angle and magnitude.\n"
                                                        "@param angle The angle of the direction.\n"
                                                        "@param magnitude The magnitude of the direction.\n"
                                                        "@return No return value.")
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
// Length of a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Length, F32, 2, 2, "(Vector2 v1) - Returns the length of v1.")
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
// Inverse of a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2Inverse, const char*, 2, 2, "(Vector2 v1) - Returns the inverse of v1.")
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
// Inverse X component of a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2InverseX, const char*, 2, 2, "(Vector2 v1) - Returns the inverse of the v1 X component.")
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
// Inverse Y component of a 2D Vector.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2InverseY, const char*, 2, 2, "(Vector2 v1) - Returns the inverse of the v1 Y component.")
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
// Normalize Rectangle (two 2D Vectors) with relation to each other.
//-----------------------------------------------------------------------------
ConsoleFunction( Vector2AreaNormalize, const char*, 3, 3, "(Vector2 v1, Vector2 v2) - Returns Normalize rectangle of v1 and v2.")
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

ConsoleFunctionGroupEnd( Vector2Math );
