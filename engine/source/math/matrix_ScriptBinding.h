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

ConsoleFunctionGroupBegin(MatrixMath, "Matrix manipulation functions.");

/*! @defgroup MatrixMathFunctions Matrix Math
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the matrixCreate function to create a transform matrix from a three-element floating-point translation vector and a four-element floating-point rotation vector.
    @param posVec A three-element floating-point translation vector: \PosX PosY PosZ\.
    @param rotVec A four-element floating-point rotation vector: \RotX RotY RotZ\.
    @param These re rotations about the specified axes.
    @return Returns a transform matrix of the form \PosX PosY PosZ RotX RotY RotZ theta\.
    @sa MatrixCreateFromEuler
*/
ConsoleFunctionWithDocs( MatrixCreate, ConsoleString, 3, 3, ( posVec , rotVec ))
{
   Point3F pos;
   dSscanf(argv[1], "%g %g %g", &pos.x, &pos.y, &pos.z);

   AngAxisF aa(Point3F(0,0,0),0);
   dSscanf(argv[2], "%g %g %g %g", &aa.axis.x, &aa.axis.y, &aa.axis.z, &aa.angle);
   aa.angle = mDegToRad(aa.angle);

   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 255, "%g %g %g %g %g %g %g",
            pos.x, pos.y, pos.z,
            aa.axis.x, aa.axis.y, aa.axis.z,
            aa.angle);
   return returnBuffer;
}

/*! Use the MatrixCreateFromEuler function to calculate a transform matrix from a three-element floating-point rotation vector.
    @param rotVec A three-element floating-point rotation vector: \RotX RotY RotZ\. These are rotations about the specified axes.
    @return Returns a transform matrix of the form \0 0 0 X Y Z theta\.
    @sa MatrixCreate
*/
ConsoleFunctionWithDocs( MatrixCreateFromEuler, ConsoleString, 2, 2,  ( rotVec ))
{
   EulerF rot;
   dSscanf(argv[1], "%g %g %g", &rot.x, &rot.y, &rot.z);

   QuatF rotQ(rot);
   AngAxisF aa;
   aa.set(rotQ);

   char* ret = Con::getReturnBuffer(256);
   dSprintf(ret, 255, "0 0 0 %g %g %g %g",aa.axis.x,aa.axis.y,aa.axis.z,aa.angle);
   return ret;
}


/*! Use the MatrixMultiply function to multiply two seven-element transform matrices to produce a new seven element matrix.
    @param transformA A seven-element transform matrix of the form \PosX PosY PosZ RotX RotY RotZ theta\.
    @param transformB A seven-element transform matrix of the form \PosX PosY PosZ RotX RotY RotZ theta\.
    @return Returns a seven-element matrix resulting from transiformA x transformB.
    @sa MatrixMulPoint, MatrixMulVector
*/
ConsoleFunctionWithDocs( MatrixMultiply, ConsoleString, 3, 3, ( transformA , transformB ))
{
   Point3F pos1;
   AngAxisF aa1(Point3F(0,0,0),0);
   dSscanf(argv[1], "%g %g %g %g %g %g %g", &pos1.x, &pos1.y, &pos1.z, &aa1.axis.x, &aa1.axis.y, &aa1.axis.z, &aa1.angle);

   MatrixF temp1(true);
   aa1.setMatrix(&temp1);
   temp1.setColumn(3, pos1);

   Point3F pos2;
   AngAxisF aa2(Point3F(0,0,0),0);
   dSscanf(argv[2], "%g %g %g %g %g %g %g", &pos2.x, &pos2.y, &pos2.z, &aa2.axis.x, &aa2.axis.y, &aa2.axis.z, &aa2.angle);

   MatrixF temp2(true);
   aa2.setMatrix(&temp2);
   temp2.setColumn(3, pos2);

   temp1.mul(temp2);


   Point3F pos;
   AngAxisF aa(temp1);

   aa.axis.normalize();
   temp1.getColumn(3, &pos);

   char* ret = Con::getReturnBuffer(256);
   dSprintf(ret, 255, "%g %g %g %g %g %g %g",
            pos.x, pos.y, pos.z,
            aa.axis.x, aa.axis.y, aa.axis.z,
            aa.angle);
   return ret;
}


/*! Use the MatrixMulVector function to multiply a seven-element transform matrix with a three-element matrix.
    @param transform A seven-element transform matrix of the form \PosX PosY PosZ RotX RotY RotZ theta\.
    @param vector A three-element vector.
    @return Returns three-element resulting from vector * transform.
    @sa MatrixMulPoint, MatrixMultiply
*/
ConsoleFunctionWithDocs( MatrixMulVector, ConsoleString, 3, 3, ( transform , vector ))
{
   Point3F pos1;
   AngAxisF aa1(Point3F(0,0,0),0);
   dSscanf(argv[1], "%g %g %g %g %g %g %g", &pos1.x, &pos1.y, &pos1.z, &aa1.axis.x, &aa1.axis.y, &aa1.axis.z, &aa1.angle);

   MatrixF temp1(true);
   aa1.setMatrix(&temp1);
   temp1.setColumn(3, pos1);

   Point3F vec1;
   dSscanf(argv[2], "%g %g %g", &vec1.x, &vec1.y, &vec1.z);

   Point3F result;
   temp1.mulV(vec1, &result);

   char* ret = Con::getReturnBuffer(256);
   dSprintf(ret, 255, "%g %g %g", result.x, result.y, result.z);
   return ret;
}

/*! Use the MatrixMulPoint function to multiply a seven element transform matrix by a three element point vector, producing a three element position vector.
    @param transform A seven-element transform matrix.
    @param point A three-element point/position vector.
    @return Returns a three-element position vector.
    @sa MatrixMultiply, MatrixMulVector
*/
ConsoleFunctionWithDocs( MatrixMulPoint, ConsoleString, 3, 3, ( transform , point ))
{
   Point3F pos1;
   AngAxisF aa1(Point3F(0,0,0),0);
   dSscanf(argv[1], "%g %g %g %g %g %g %g", &pos1.x, &pos1.y, &pos1.z, &aa1.axis.x, &aa1.axis.y, &aa1.axis.z, &aa1.angle);

   MatrixF temp1(true);
   aa1.setMatrix(&temp1);
   temp1.setColumn(3, pos1);

   Point3F vec1;
   dSscanf(argv[2], "%g %g %g", &vec1.x, &vec1.y, &vec1.z);

   Point3F result;
   temp1.mulP(vec1, &result);

   char* ret = Con::getReturnBuffer(256);
   dSprintf(ret, 255, "%g %g %g", result.x, result.y, result.z);
   return ret;
}

ConsoleFunctionGroupEnd(MatrixMath);

/*! @} */ // group MatrixMathFunctions
