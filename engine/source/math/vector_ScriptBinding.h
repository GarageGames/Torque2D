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

ConsoleFunctionGroupBegin( VectorMath, "Vector manipulation functions.");

/*! @defgroup VectorMathFunctions Vector Math
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the VectorAdd function to add two vectors of up to three elements each to each other
    @param vecA A vector of up to three elements.
    @param vecB A vector of up to three elements.
    @return Returns the result of vecA + vecB.
    @sa vectorSub
*/
ConsoleFunctionWithDocs( VectorAdd, ConsoleString, 3, 3, ( vecA , vecB ))
{
   VectorF v1(0,0,0),v2(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v1.x,&v1.y,&v1.z);
   dSscanf(argv[2],"%g %g %g",&v2.x,&v2.y,&v2.z);
   VectorF v;
   v = v1 + v2;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g",v.x,v.y,v.z);
   return returnBuffer;
}

/*! Use the VectorSub function to subtract vecB from vecA.
    @param vecA Left side vector in subtraction equation.
    @param vecB Right side vector in subtraction equation.
    @return Returns a new vector equivalent to: \vecA - vecB\.
    @sa vectorAdd
*/
ConsoleFunctionWithDocs( VectorSub, ConsoleString, 3, 3, ( vecA , vecB ))
{
   VectorF v1(0,0,0),v2(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v1.x,&v1.y,&v1.z);
   dSscanf(argv[2],"%g %g %g",&v2.x,&v2.y,&v2.z);
   VectorF v;
   v = v1 - v2;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g",v.x,v.y,v.z);
   return returnBuffer;
}

/*! Use the VectorScale function to scale the vector vec by the scalar scale.
    @param vec A vector of up to three elements.
    @param scale A numeric value (integer or floating-point) representing the scaling factor.
    @return Returns a scaled version of the vector vec, equivalent to: ( scale * X ) ( scale * Y ) ( scale * Z )\.
    @sa VectorNormalize
*/
ConsoleFunctionWithDocs( VectorScale, ConsoleString, 3, 3, ( vec , scale ))
{
   VectorF v(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v.x,&v.y,&v.z);
   v *= dAtof(argv[2]);
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g",v.x,v.y,v.z);
   return returnBuffer;
}

/*! Use the VectorNormalize function to calculated the unit vector equivalent of the vector vec.
    @param vec A vector of up to three elements.
    @return Returns the unit vector equivalent of the vector vec.
    @sa VectorScale
*/
ConsoleFunctionWithDocs( VectorNormalize, ConsoleString, 2, 2, ( vec ))
{
   VectorF v(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v.x,&v.y,&v.z);
   if (v.len() != 0)
      v.normalize();
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g",v.x,v.y,v.z);
   return returnBuffer;
}

/*! Use the VectorCross function to calculate the dot product of two unit vectors of up to three elements each. Warning! Be sure to always normalize both vecA and vecB before attempting to find the dot product. Calculating a dot product using un-normalized vectors (non- unit vectors) will result in meaningless results.
    If the return value is < 0, the inner-angle between the vectors is > 90 degrees. If the return value is == 0, the inner-angle between the vectors is == 90 degrees. If the return value is > 0, the inner-angle between the vectors is < 90 degrees.
    @param vecA A unit-vector of up to three elements.
    @param vecB A unit-vector of up to three elements.
    @return Returns a scalar value equal to the result of vecA . vecB. This value which will always be a single floating-point value in the range [ -1 , +1 ].
    @sa VectorCross
*/
ConsoleFunctionWithDocs( VectorDot, ConsoleFloat, 3, 3, ( vecA , vecB ))
{
   VectorF v1(0,0,0),v2(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v1.x,&v1.y,&v1.z);
   dSscanf(argv[2],"%g %g %g",&v2.x,&v2.y,&v2.z);
   return mDot(v1,v2);
}

/*! Use the VectorCross function to calculate the cross product of two vectors of up to three elements each.
    Remember, the resultant vector will always be an right angles (orthogonal) to both input vectors.
    @param vecA A vector of up to three elements.
    @param vecB A vector of up to three elements.
    @return Returns the result of vecA x vecB.
    @sa VectorDot
*/
ConsoleFunctionWithDocs(VectorCross, ConsoleString, 3, 3, ( vecA , vecB ))
{
   VectorF v1(0,0,0),v2(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v1.x,&v1.y,&v1.z);
   dSscanf(argv[2],"%g %g %g",&v2.x,&v2.y,&v2.z);
   VectorF v;
   mCross(v1,v2,&v);
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g",v.x,v.y,v.z);
   return returnBuffer;
}

/*! Use the VectorDist function to calculate distance between two vectors of up to three elements each.
    @param vecA A vector of up to three elements.
    @param vecB A vector of up to three elements.
    @return Returns the result of \ |Xa - Xb| |Ya - Yb| |Za - Zb| \.
    @sa VectorLen
*/
ConsoleFunctionWithDocs(VectorDist, ConsoleFloat, 3, 3, ( vecA , vecB ))
{
   VectorF v1(0,0,0),v2(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v1.x,&v1.y,&v1.z);
   dSscanf(argv[2],"%g %g %g",&v2.x,&v2.y,&v2.z);
   VectorF v = v2 - v1;
   return mSqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/*! Use the VectorLen function to calculate the length of vector vec.
    @param vec A vector of up to three elements.
    @return Returns a scalar representing the length of the vector vec.
    @sa VectorDist
*/
ConsoleFunctionWithDocs(VectorLen, ConsoleFloat, 2, 2, ( vec ))
{
   VectorF v(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v.x,&v.y,&v.z);
   return mSqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

/*! Use the VectorOrthoBasis function to calculate a 3x3 Row-Major formated matrix representing the orthogonal basis for the vector vec.
    @param vec A four element vector of the form \AxisX AxisY AxisZ theta\, where theta is the angle of rotation about the vector formed by the prior three values.
    @return Returns a 3x3 Row-Major formated matrix
*/
ConsoleFunctionWithDocs( VectorOrthoBasis, ConsoleString, 2, 2, ( vec ))
{
   AngAxisF aa;
   dSscanf(argv[1],"%g %g %g %g", &aa.axis.x,&aa.axis.y,&aa.axis.z,&aa.angle);
   MatrixF mat;
   aa.setMatrix(&mat);
   Point3F col0, col1, col2;
   mat.getColumn(0, &col0);
   mat.getColumn(1, &col1);
   mat.getColumn(2, &col2);
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g %g %g %g %g %g %g",
            col0.x, col0.y, col0.z, col1.x, col1.y, col1.z, col2.x, col2.y, col2.z);
   return returnBuffer;
}

ConsoleFunctionGroupEnd(VectorMath);

/*! @} */ // group VectorMathFunctions
