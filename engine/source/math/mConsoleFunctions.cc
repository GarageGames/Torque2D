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

#include "platform/platform.h"
#include "console/console.h"
#include "math/mMathFn.h"
#include "math/mRandom.h"
#include "string/stringUnit.h"

ConsoleFunctionGroupBegin( GeneralMath, "General math functions. Use these whenever possible, as they'll run much faster than script equivalents.");

ConsoleFunction( mSolveQuadratic, const char *, 4, 4, "(float a, float b, float c)"
              "Solve a quadratic equation of form a*x^2 + b*x + c = 0.\n\n"
              "@returns A triple, contanining: sol x0 x1. sol is the number of"
              " solutions (being 0, 1, or 2), and x0 and x1 are the solutions, if any."
              " Unused x's are undefined.")
{
   char * retBuffer = Con::getReturnBuffer(256);
   F32 x[2];
   U32 sol = mSolveQuadratic(dAtof(argv[1]), dAtof(argv[2]), dAtof(argv[3]), x);
   dSprintf(retBuffer, 256, "%d %g %g", sol, x[0], x[1]);
   return retBuffer;
}

ConsoleFunction( mSolveCubic, const char *, 5, 5, "(float a, float b, float c, float d)"
              "Solve a cubic equation of form a*x^3 + b*x^2 + c*x + d = 0.\n\n"
              "@returns A 4-tuple, contanining: sol x0 x1 x2. sol is the number of"
              " solutions (being 0, 1, 2, or 3), and x0, x1, x2 are the solutions, if any."
              " Unused x's are undefined.")
{
   char * retBuffer = Con::getReturnBuffer(256);
   F32 x[3];
   U32 sol = mSolveCubic(dAtof(argv[1]), dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), x);
   dSprintf(retBuffer, 256, "%d %g %g %g", sol, x[0], x[1], x[2]);
   return retBuffer;
}

ConsoleFunction( mSolveQuartic, const char *, 6, 6, "(float a, float b, float c, float d, float e)"
              "Solve a quartic equation of form a*x^4 + b*x^3 + c*x^2 + d*x + e = 0.\n\n"
              "@returns A 5-tuple, contanining: sol x0 x1 x2 x3. sol is the number of"
              " solutions (ranging from 0-4), and x0, x1, x2 and x3 are the solutions, if any."
              " Unused x's are undefined.")
{
   char * retBuffer = Con::getReturnBuffer(256);
   F32 x[4];
   U32 sol = mSolveQuartic(dAtof(argv[1]), dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), dAtof(argv[5]), x);
   dSprintf(retBuffer, 256, "%d %g %g %g %g", sol, x[0], x[1], x[2], x[3]);
   return retBuffer;
}

ConsoleFunction( mFloor, S32, 2, 2, "( val ) Use the mFloor function to calculate the next lowest integer value from val.\n"
                                                                "@param val A floating-point value.\n"
                                                                "@return Returns an integer representing the next lowest integer from val.\n"
                                                                "@sa mCeil")
{
   return (S32)mFloor(dAtof(argv[1]));
}
ConsoleFunction( mRound, F32, 2, 2, "(float v) Rounds a number. 0.5 is rounded up.\n"
                "@param val A floating-point value\n"
                "@return Returns the integer value closest to the given float")

{
   return mRound( dAtof(argv[1]) );
}

ConsoleFunction( mCeil, S32, 2, 2, "( val ) Use the mCeil function to calculate the next highest integer value from val.\n"
                                                                "@param val A floating-point value.\n"
                                                                "@return Returns an integer representing the next highest integer from val.\n"
                                                                "@sa mFloor")
{
   return (S32)mCeil(dAtof(argv[1]));
}


ConsoleFunction( mFloatLength, const char *, 3, 3, "( val , numDecimals ) Use the mFloatLength function to limit the number of decimal places in val to numDecimals.\n"
                                                                "@param val A floating-point value.\n"
                                                                "@param numDecimals An integer between 0 and inf representing the number of decimal places to allow val to have.\n"
                                                                "@return Returns a floating-point value equivalent to a truncated version of val, where the new version has numDecimals decimal places")
{
   char * outBuffer = Con::getReturnBuffer(256);
   char fmtString[8] = "%.0f";
   U32 precision = dAtoi(argv[2]);
   if (precision > 9)
      precision = 9;
   fmtString[2] = '0' + precision;

   dSprintf(outBuffer, 255, fmtString, dAtof(argv[1]));
   return outBuffer;
}

//------------------------------------------------------------------------------
ConsoleFunction( mAbs, F32, 2, 2, "( val ) Use the mAbs function to get the magnitude of val.\n"
                                                                "@param val An integer or a floating-point value.\n"
                                                                "@return Returns the magnitude of val")
{
   return(mFabs(dAtof(argv[1])));
}

ConsoleFunction( mSqrt, F32, 2, 2, "( val ) Use the mSqrt function to calculated the square root of val.\n"
                                                                "@param val A numeric value.\n"
                                                                "@return Returns the the squareroot of val")
{
   return(mSqrt(dAtof(argv[1])));
}

ConsoleFunction( mPow, F32, 3, 3, "( val , power ) Use the mPow function to calculated val raised to the power of power.\n"
                                                                "@param val A numeric (integer or floating-point) value to be raised to a power.\n"
                                                                "@param power A numeric (integer or floating-point) power to raise val to.\n"
                                                                "@return Returns val^power")
{
   return(mPow(dAtof(argv[1]), dAtof(argv[2])));
}

ConsoleFunction( mLog, F32, 2, 2, "( val ) Use the mLog function to calculate the natural logarithm of val.\n"
                                                                "@param val A numeric value.\n"
                                                                "@return Returns the natural logarithm of val")
{
   return(mLog(dAtof(argv[1])));
}

ConsoleFunction( mSin, F32, 2, 2, "( val ) Use the mSin function to get the sine of the radian angle val.\n"
                                                                "@param val A value between -3.14159 and 3.14159.\n"
                                                                "@return Returns the sine of val. This value will be in the range [ -1.0 , 1.0 ].\n"
                                                                "@sa mAsin")
{
   return(mSin(dAtof(argv[1])));
}

ConsoleFunction( mCos, F32, 2, 2, "( val ) Use the mCos function to get the cosine of the radian angle val.\n"
                                                                "@param val A value between -3.14159 and 3.14159.\n"
                                                                "@return Returns the cosine of val. This value will be in the range [ -1.0 , 1.0 ].\n"
                                                                "@sa mAcos")
{
   return(mCos(dAtof(argv[1])));
}

ConsoleFunction( mTan, F32, 2, 2, "( val ) Use the mTan function to get the tangent of the radian angle val.\n"
                                                                "@param val A value between -3.14159/2 and 3.14159/2.\n"
                                                                "@return Returns the tangent of val. This value will be in the range [ -inf.0 , inf.0 ].\n"
                                                                "@sa mAtan")
{
   return(mTan(dAtof(argv[1])));
}

ConsoleFunction( mAsin, F32, 2, 2, "( val ) Use the mAsin function to get the inverse sine of val in radians.\n"
                                                                "@param val A value between -1.0 and 1.0 equal to the sine of some angle theta.\n"
                                                                "@return Returns the inverse sine of val in radians. This value will be in the range [ - 3.14159/2 , 3.14159/2 ].\n"
                                                                "@sa mSin")
{
   return(mAsin(dAtof(argv[1])));
}

ConsoleFunction( mAcos, F32, 2, 2, "( val ) Use the mAcos function to get the inverse cosine of val in radians.\n"
                                                                "@param val A value between -1.0 and 1.0 equal to the cosine of some angle theta.\n"
                                                                "@return Returns the inverse cosine of val in radians. This value will be in the range [ 0 , 3.14159 ].\n"
                                                                "@sa mCos")
{
   return(mAcos(dAtof(argv[1])));
}

ConsoleFunction( mAtan, F32, 3, 3, "( val ) Use the mAtan function to get the inverse tangent of rise/run in radians.\n"
                                                                "@param rise Vertical component of a line.\n"
                                                                "@param run Horizontal component of a line.\n"
                                                                "@return Returns the slope in radians (the arc-tangent) of a line with the given rise and run.\n"
                                                                "@sa mTan")
{
   return(mAtan(dAtof(argv[1]), dAtof(argv[2])));
}

ConsoleFunction( mRadToDeg, F32, 2, 2, "( val ) Use the mRadToDeg function to convert radians to degrees.\n"
                                                                "@param val A floating-point number representing some number of radians.\n"
                                                                "@return Returns the equivalent of the radian value val in degrees.\n"
                                                                "@sa mDegToRad")
{
   return(mRadToDeg(dAtof(argv[1])));
}

ConsoleFunction( mDegToRad, F32, 2, 2, "( val ) Use the mDegToRad function to convert degrees to radians.\n"
                                                                "@param val A floating-point number representing some number of degrees.\n"
                                                                "@return Returns the equivalent of the degree value val in radians.\n"
                                                                "@sa mRadToDeg")
{
   return(mDegToRad(dAtof(argv[1])));
}

ConsoleFunction( mClamp, F32, 4, 4, "(float number, float min, float max) Clamp a value between two other values.\n"
                "@param number A float value representing the number to clamp\n"
                "@param min The lower bound\n"
                "@param max The upper bound\n"
                "@return A float value the is within the given range")
{
   F32 value = dAtof( argv[1] );
   F32 min = dAtof( argv[2] );
   F32 max = dAtof( argv[3] );
   return mClampF( value, min, max );
}

//-----------------------------------------------------------------------------

ConsoleFunction( mGetMin, F32, 3, 3, "(a, b) - Returns the Minimum of two values.")
{
   return getMin(dAtof(argv[1]), dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleFunction( mGetMax, F32, 3, 3, "(a, b) - Returns the Maximum of two values.")
{
   return getMax(dAtof(argv[1]), dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleFunction( bits, const char*, 2, 2, "Converts a list of bit-positions into a value." )
{
    // Calculate Element Count.
    U32 elementCount = StringUnit::getUnitCount( argv[1], " \t\n" );

    // Return nothing if there's no elements.
    if ( elementCount < 1 )
    {
        // Error!
        Con::printf("bits() - Invalid number of parameters!");
        return "0";
    }

    // Reset Bit Value.
    U32 bitValue = 0;

    // Parse Bits.
    for ( U32 n = 0; n < elementCount; n++ )
    {
        // Merge Bit Value.
        bitValue |= (U32)BIT(dAtoi(StringUnit::getUnit(argv[1], n, " \t\n")));
    }

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);
    // Format Output.
    dSprintf( pBuffer, 16, "%u", bitValue );
    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleFunction( bit, const char*, 2, 2, "Converts a bit-position into a value." )
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);

    // Format Output.
    dSprintf( pBuffer, 16, "%u", U32(BIT(dAtoi(argv[1]))) );

    // Return Buffer.
    return pBuffer;
}

ConsoleFunction( bitInverse, const char*, 2, 2, "Returns the ones complement of a bit." )
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);

    // Format Output.
    dSprintf( pBuffer, 16, "%u", U32(~BIT(dAtoi(argv[1]))) );

    // Return Buffer.
    return pBuffer;
}

ConsoleFunction( addBitToMask, S32, 3, 3, "( mask, bit ) - Returns the mask with a bit added to it" )
{
   U32 mask;
   dSscanf( argv[1], "%u", &mask );
   U32 bit = BIT( dAtoi( argv[2] ) );
   
   return mask | bit;
}

ConsoleFunction( removeBitFromMask, S32, 3, 3, "( mask, bit ) - Returns the mask with a bit removed from it" )
{
   U32 mask;
   dSscanf( argv[1], "%u", &mask );
   U32 bit = BIT( dAtoi( argv[2] ) );
   
   return mask & ~bit;
}

ConsoleFunctionGroupEnd( GeneralMath );
