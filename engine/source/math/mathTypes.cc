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

#include "console/consoleTypes.h"
#include "console/console.h"
#include "math/mPoint.h"
#include "math/mMatrix.h"
#include "math/mRect.h"
#include "math/mBox.h"
#include "math/mathTypes.h"
#include "math/mRandom.h"

//////////////////////////////////////////////////////////////////////////
// TypePoint2I
//////////////////////////////////////////////////////////////////////////
ConsoleType( Point2I, TypePoint2I, sizeof(Point2I), "" )

ConsoleGetType( TypePoint2I )
{
   Point2I *pt = (Point2I *) dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d %d", pt->x, pt->y);
   return returnBuffer;
}

ConsoleSetType( TypePoint2I )
{
   if(argc == 1)
      dSscanf(argv[0], "%d %d", &((Point2I *) dptr)->x, &((Point2I *) dptr)->y);
   else if(argc == 2)
      *((Point2I *) dptr) = Point2I(dAtoi(argv[0]), dAtoi(argv[1]));
   else
      Con::printf("Point2I must be set as { x, y } or \"x y\"");
}

//////////////////////////////////////////////////////////////////////////
// TypePoint2F
//////////////////////////////////////////////////////////////////////////
ConsoleType( Point2F, TypePoint2F, sizeof(Point2F), "" )

ConsoleGetType( TypePoint2F )
{
   Point2F *pt = (Point2F *) dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.3f %.3f", pt->x, pt->y);
   return returnBuffer;
}

ConsoleSetType( TypePoint2F )
{
   if(argc == 1)
      dSscanf(argv[0], "%g %g", &((Point2F *) dptr)->x, &((Point2F *) dptr)->y);
   else if(argc == 2)
      *((Point2F *) dptr) = Point2F(dAtof(argv[0]), dAtof(argv[1]));
   else
      Con::printf("Point2F must be set as { x, y } or \"x y\"");
}

//////////////////////////////////////////////////////////////////////////
// TypePoint2FVector
//////////////////////////////////////////////////////////////////////////
ConsoleType( point2FList, TypePoint2FVector, sizeof(Vector<Point2F>), "" )

ConsoleGetType( TypePoint2FVector )
{
   Vector<Point2F> *vec = (Vector<Point2F> *)dptr;
   char* returnBuffer = Con::getReturnBuffer( vec->size() * 64 );
   S32 maxReturn = 1024;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<Point2F>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%.3f %.3f ", (*itr).x, (*itr).y);
      // update the length of the return string (so far)
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return returnBuffer;
}

ConsoleSetType( TypePoint2FVector )
{
   Vector<Point2F> *vec = (Vector<Point2F> *)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if(argc == 1)
   {
      const char *values = argv[0];
      const char *endValues = values + dStrlen(values);
      Point2F value;
      // advance through the string, pulling off S32's and advancing the pointer
      while (values < endValues && dSscanf(values, "%g %g", &value.x, &value.y) != 0)
      {
         vec->push_back(value);
         const char *nextSeperator = dStrchr(values, ' ');
         if( !nextSeperator )
            break;
         const char *nextValues = dStrchr(nextSeperator + 1, ' ');
         if (nextValues != 0 && nextValues < endValues)
            values = nextValues + 1;
         else
            break;
      }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < (argc - 1); i += 2)
         vec->push_back(Point2F(dAtof(argv[i]), dAtof(argv[i + 1])));
   }
   else
      Con::printf("Vector<Point2F> must be set as { a, b, c, ... } or \"a b c ...\"");
}

//////////////////////////////////////////////////////////////////////////
// TypePoint3F
//////////////////////////////////////////////////////////////////////////
ConsoleType( Point3F, TypePoint3F, sizeof(Point3F), "" )

ConsoleGetType( TypePoint3F )
{
   Point3F *pt = (Point3F *) dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%g %g %g", pt->x, pt->y, pt->z);
   return returnBuffer;
}

ConsoleSetType( TypePoint3F )
{
   if(argc == 1)
      dSscanf(argv[0], "%g %g %g", &((Point3F *) dptr)->x, &((Point3F *) dptr)->y, &((Point3F *) dptr)->z);
   else if(argc == 3)
      *((Point3F *) dptr) = Point3F(dAtof(argv[0]), dAtof(argv[1]), dAtof(argv[2]));
   else
      Con::printf("Point3F must be set as { x, y, z } or \"x y z\"");
}

//////////////////////////////////////////////////////////////////////////
// TypePoint4F
//////////////////////////////////////////////////////////////////////////
ConsoleType( Point4F, TypePoint4F, sizeof(Point4F), "" )

ConsoleGetType( TypePoint4F )
{
   Point4F *pt = (Point4F *) dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%g %g %g %g", pt->x, pt->y, pt->z, pt->w);
   return returnBuffer;
}

ConsoleSetType( TypePoint4F )
{
   if(argc == 1)
      dSscanf(argv[0], "%g %g %g %g", &((Point4F *) dptr)->x, &((Point4F *) dptr)->y, &((Point4F *) dptr)->z, &((Point4F *) dptr)->w);
   else if(argc == 4)
      *((Point4F *) dptr) = Point4F(dAtof(argv[0]), dAtof(argv[1]), dAtof(argv[2]), dAtof(argv[3]));
   else
      Con::printf("Point4F must be set as { x, y, z, w } or \"x y z w\"");
}

//////////////////////////////////////////////////////////////////////////
// TypeRectI
//////////////////////////////////////////////////////////////////////////
ConsoleType( RectI, TypeRectI, sizeof(RectI), "" )

ConsoleGetType( TypeRectI )
{
   RectI *rect = (RectI *) dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d %d %d %d", rect->point.x, rect->point.y,
            rect->extent.x, rect->extent.y);
   return returnBuffer;
}

ConsoleSetType( TypeRectI )
{
   if(argc == 1)
      dSscanf(argv[0], "%d %d %d %d", &((RectI *) dptr)->point.x, &((RectI *) dptr)->point.y,
              &((RectI *) dptr)->extent.x, &((RectI *) dptr)->extent.y);
   else if(argc == 4)
      *((RectI *) dptr) = RectI(dAtoi(argv[0]), dAtoi(argv[1]), dAtoi(argv[2]), dAtoi(argv[3]));
   else
      Con::printf("RectI must be set as { x, y, w, h } or \"x y w h\"");
}

//////////////////////////////////////////////////////////////////////////
// TypeRectF
//////////////////////////////////////////////////////////////////////////
ConsoleType( RectF, TypeRectF, sizeof(RectF), "" )

ConsoleGetType( TypeRectF )
{
   RectF *rect = (RectF *) dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%g %g %g %g", rect->point.x, rect->point.y,
            rect->extent.x, rect->extent.y);
   return returnBuffer;
}

ConsoleSetType( TypeRectF )
{
   if(argc == 1)
      dSscanf(argv[0], "%g %g %g %g", &((RectF *) dptr)->point.x, &((RectF *) dptr)->point.y,
              &((RectF *) dptr)->extent.x, &((RectF *) dptr)->extent.y);
   else if(argc == 4)
      *((RectF *) dptr) = RectF(dAtof(argv[0]), dAtof(argv[1]), dAtof(argv[2]), dAtof(argv[3]));
   else
      Con::printf("RectF must be set as { x, y, w, h } or \"x y w h\"");
}

//////////////////////////////////////////////////////////////////////////
// TypeMatrixPosition
//////////////////////////////////////////////////////////////////////////
ConsoleType( MatrixPosition, TypeMatrixPosition, sizeof(4*sizeof(F32)), "" )

ConsoleGetType( TypeMatrixPosition )
{
   F32 *col = (F32 *) dptr + 3;
   char* returnBuffer = Con::getReturnBuffer(256);
   if(col[12] == 1.f)
      dSprintf(returnBuffer, 256, "%g %g %g", col[0], col[4], col[8]);
   else
      dSprintf(returnBuffer, 256, "%g %g %g %g", col[0], col[4], col[8], col[12]);
   return returnBuffer;
}

ConsoleSetType( TypeMatrixPosition )
{
   F32 *col = ((F32 *) dptr) + 3;
   if (argc == 1)
   {
      col[0] = col[4] = col[8] = 0.f;
      col[12] = 1.f;
      dSscanf(argv[0], "%g %g %g %g", &col[0], &col[4], &col[8], &col[12]);
   }
   else if (argc <= 4) 
   {
         for (S32 i = 0; i < argc; i++)
            col[i << 2] = dAtof(argv[i]);
   }
   else
      Con::printf("Matrix position must be set as { x, y, z, w } or \"x y z w\"");
}

//////////////////////////////////////////////////////////////////////////
// TypeMatrixRotation
//////////////////////////////////////////////////////////////////////////
ConsoleType( MatrixRotation, TypeMatrixRotation, sizeof(MatrixF), "" )

ConsoleGetType( TypeMatrixRotation )
{
   AngAxisF aa(*(MatrixF *) dptr);
   aa.axis.normalize();
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g %g",aa.axis.x,aa.axis.y,aa.axis.z,mRadToDeg(aa.angle));
   return returnBuffer;
}

ConsoleSetType( TypeMatrixRotation )
{
   // DMM: Note that this will ONLY SET the ULeft 3x3 submatrix.
   //
   AngAxisF aa(Point3F(0,0,0),0);
   if (argc == 1)
   {
      dSscanf(argv[0], "%g %g %g %g", &aa.axis.x, &aa.axis.y, &aa.axis.z, &aa.angle);
      aa.angle = mDegToRad(aa.angle);
   }
   else if (argc == 4) 
   {
         for (S32 i = 0; i < argc; i++)
            ((F32*)&aa)[i] = dAtof(argv[i]);
         aa.angle = mDegToRad(aa.angle);
   }
   else
      Con::printf("Matrix rotation must be set as { x, y, z, angle } or \"x y z angle\"");

   //
   MatrixF temp;
   aa.setMatrix(&temp);

   F32* pDst = *(MatrixF *)dptr;
   const F32* pSrc = temp;
   for (U32 i = 0; i < 3; i++)
      for (U32 j = 0; j < 3; j++)
         pDst[i*4 + j] = pSrc[i*4 + j];
}



//////////////////////////////////////////////////////////////////////////
// TypeBox3F
//////////////////////////////////////////////////////////////////////////
ConsoleType( Box3F, TypeBox3F, sizeof(Box3F), "" )

ConsoleGetType( TypeBox3F )
{
   const Box3F* pBox = (const Box3F*)dptr;

   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%g %g %g %g %g %g",
            pBox->mMin.x, pBox->mMin.y, pBox->mMin.z,
            pBox->mMax.x, pBox->mMax.y, pBox->mMax.z);

   return returnBuffer;
}

ConsoleSetType( TypeBox3F )
{
   Box3F* pDst = (Box3F*)dptr;

   if (argc == 1) 
   {
       U32 args;
       args = dSscanf(argv[0], "%g %g %g %g %g %g",
                         &pDst->mMin.x, &pDst->mMin.y, &pDst->mMin.z,
                         &pDst->mMax.x, &pDst->mMax.y, &pDst->mMax.z);
      AssertWarn(args == 6, "Warning, box probably not read properly");
   } 
   else 
   {
      Con::printf("Box3F must be set as \"xMin yMin zMin xMax yMax zMax\"");
   }
}


//----------------------------------------------------------------------------

ConsoleFunctionGroupBegin( VectorMath, "Vector manipulation functions.");

ConsoleFunction( VectorAdd, const char*, 3, 3, "( vecA , vecB ) Use the VectorAdd function to add two vectors of up to three elements each to each other\n"
                                                                "@param vecA A vector of up to three elements.\n"
                                                                "@param vecB A vector of up to three elements.\n"
                                                                "@return Returns the result of vecA + vecB.\n"
                                                                "@sa vectorSub")
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

ConsoleFunction( VectorSub, const char*, 3, 3, "( vecA , vecB ) Use the VectorSub function to subtract vecB from vecA.\n"
                                                                "@param vecA Left side vector in subtraction equation.\n"
                                                                "@param vecB Right side vector in subtraction equation.\n"
                                                                "@return Returns a new vector equivalent to: \"vecA - vecB\".\n"
                                                                "@sa vectorAdd")
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

ConsoleFunction( VectorScale, const char*, 3, 3, "( vec , scale ) Use the VectorScale function to scale the vector vec by the scalar scale.\n"
                                                                "@param vec A vector of up to three elements.\n"
                                                                "@param scale A numeric value (integer or floating-point) representing the scaling factor.\n"
                                                                "@return Returns a scaled version of the vector vec, equivalent to: ( scale * X ) ( scale * Y ) ( scale * Z )\".\n"
                                                                "@sa VectorNormalize")
{
   VectorF v(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v.x,&v.y,&v.z);
   v *= dAtof(argv[2]);
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g",v.x,v.y,v.z);
   return returnBuffer;
}

ConsoleFunction( VectorNormalize, const char*, 2, 2, "( vec ) Use the VectorNormalize function to calculated the unit vector equivalent of the vector vec.\n"
                                                                "@param vec A vector of up to three elements.\n"
                                                                "@return Returns the unit vector equivalent of the vector vec.\n"
                                                                "@sa VectorScale")
{
   VectorF v(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v.x,&v.y,&v.z);
   if (v.len() != 0)
      v.normalize();
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g",v.x,v.y,v.z);
   return returnBuffer;
}

ConsoleFunction( VectorDot, F32, 3, 3, "( vecA , vecB ) Use the VectorCross function to calculate the dot product of two unit vectors of up to three elements each. Warning! Be sure to always normalize both vecA and vecB before attempting to find the dot product. Calculating a dot product using un-normalized vectors (non- unit vectors) will result in meaningless results.\n"
                                                                "If the return value is < 0, the inner-angle between the vectors is > 90 degrees. If the return value is == 0, the inner-angle between the vectors is == 90 degrees. If the return value is > 0, the inner-angle between the vectors is < 90 degrees.\n"
                                                                "@param vecA A unit-vector of up to three elements.\n"
                                                                "@param vecB A unit-vector of up to three elements.\n"
                                                                "@return Returns a scalar value equal to the result of vecA . vecB. This value which will always be a single floating-point value in the range [ -1 , +1 ].\n"
                                                                "@sa VectorCross")
{
   VectorF v1(0,0,0),v2(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v1.x,&v1.y,&v1.z);
   dSscanf(argv[2],"%g %g %g",&v2.x,&v2.y,&v2.z);
   return mDot(v1,v2);
}

ConsoleFunction(VectorCross, const char*, 3, 3, "( vecA , vecB ) Use the VectorCross function to calculate the cross product of two vectors of up to three elements each.\n"
                                                                "Remember, the resultant vector will always be an right angles (orthogonal) to both input vectors.\n"
                                                                "@param vecA A vector of up to three elements.\n"
                                                                "@param vecB A vector of up to three elements.\n"
                                                                "@return Returns the result of vecA x vecB.\n"
                                                                "@sa VectorDot")
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

ConsoleFunction(VectorDist, F32, 3, 3, "( vecA , vecB ) Use the VectorDist function to calculate distance between two vectors of up to three elements each.\n"
                                                                "@param vecA A vector of up to three elements.\n"
                                                                "@param vecB A vector of up to three elements.\n"
                                                                "@return Returns the result of \" |Xa - Xb| |Ya - Yb| |Za - Zb| \".\n"
                                                                "@sa VectorLen")
{
   VectorF v1(0,0,0),v2(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v1.x,&v1.y,&v1.z);
   dSscanf(argv[2],"%g %g %g",&v2.x,&v2.y,&v2.z);
   VectorF v = v2 - v1;
   return mSqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

ConsoleFunction(VectorLen, F32, 2, 2, "( vec ) Use the VectorLen function to calculate the length of vector vec.\n"
                                                                "@param vec A vector of up to three elements.\n"
                                                                "@return Returns a scalar representing the length of the vector vec.\n"
                                                                "@sa VectorDist")
{
   VectorF v(0,0,0);
   dSscanf(argv[1],"%g %g %g",&v.x,&v.y,&v.z);
   return mSqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

ConsoleFunction( VectorOrthoBasis, const char*, 2, 2, "( vec ) Use the VectorOrthoBasis function to calculate a 3x3 Row-Major formated matrix representing the orthogonal basis for the vector vec.\n"
                                                                "@param vec A four element vector of the form \"AxisX AxisY AxisZ theta\", where theta is the angle of rotation about the vector formed by the prior three values.\n"
                                                                "@return Returns a 3x3 Row-Major formated matrix")
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

ConsoleFunctionGroupBegin(MatrixMath, "Matrix manipulation functions.");

ConsoleFunction( MatrixCreate, const char*, 3, 3, "( posVec , rotVec ) Use the matrixCreate function to create a transform matrix from a three-element floating-point translation vector and a four-element floating-point rotation vector.\n"
                                                                "@param posVec A three-element floating-point translation vector: \"PosX PosY PosZ\".\n"
                                                                "@param rotVec A four-element floating-point rotation vector: \"RotX RotY RotZ\".\n"
                                                                "@param These re rotations about the specified axes.\n"
                                                                "@return Returns a transform matrix of the form \"PosX PosY PosZ RotX RotY RotZ theta\".\n"
                                                                "@sa MatrixCreateFromEuler")
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

ConsoleFunction( MatrixCreateFromEuler, const char*, 2, 2, " ( rotVec ) Use the MatrixCreateFromEuler function to calculate a transform matrix from a three-element floating-point rotation vector.\n"
                                                                "@param rotVec A three-element floating-point rotation vector: \"RotX RotY RotZ\". These are rotations about the specified axes.\n"
                                                                "@return Returns a transform matrix of the form \"0 0 0 X Y Z theta\".\n"
                                                                "@sa MatrixCreate")
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


ConsoleFunction( MatrixMultiply, const char*, 3, 3, "( transformA , transformB ) Use the MatrixMultiply function to multiply two seven-element transform matrices to produce a new seven element matrix.\n"
                                                                "@param transformA A seven-element transform matrix of the form \"PosX PosY PosZ RotX RotY RotZ theta\".\n"
                                                                "@param transformB A seven-element transform matrix of the form \"PosX PosY PosZ RotX RotY RotZ theta\".\n"
                                                                "@return Returns a seven-element matrix resulting from transiformA x transformB.\n"
                                                                "@sa MatrixMulPoint, MatrixMulVector")
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


ConsoleFunction( MatrixMulVector, const char*, 3, 3, "( transform , vector ) Use the MatrixMulVector function to multiply a seven-element transform matrix with a three-element matrix.\n"
                                                                "@param transform A seven-element transform matrix of the form \"PosX PosY PosZ RotX RotY RotZ theta\".\n"
                                                                "@param vector A three-element vector.\n"
                                                                "@return Returns three-element resulting from vector * transform.\n"
                                                                "@sa MatrixMulPoint, MatrixMultiply")
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

ConsoleFunction( MatrixMulPoint, const char*, 3, 3, "( transform , point ) Use the MatrixMulPoint function to multiply a seven element transform matrix by a three element point vector, producing a three element position vector.\n"
                                                                "@param transform A seven-element transform matrix.\n"
                                                                "@param point A three-element point/position vector.\n"
                                                                "@return Returns a three-element position vector.\n"
                                                                "@sa MatrixMultiply, MatrixMulVector")
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

//------------------------------------------------------------------------------

ConsoleFunction( getBoxCenter, const char*, 2, 2, "( box ) Use the getBoxCenter function to find the centroid of a cube (box).\n"
                                                                "@param box A vector containing two three-element floating-point position vectors: \"X1 Y1 Z1 X2 Y2 Z2\".\n"
                                                                "@return Returns a vector containing a three-element floating-point position vector equal to the centroid of the area defined by box")
{
   Box3F box;
   box.mMin.set(0,0,0);
   box.mMax.set(0,0,0);
   dSscanf(argv[1],"%g %g %g %g %g %g",
           &box.mMin.x,&box.mMin.y,&box.mMin.z,
           &box.mMax.x,&box.mMax.y,&box.mMax.z);
   Point3F p;
   box.getCenter(&p);
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer,256,"%g %g %g",p.x,p.y,p.z);
   return returnBuffer;
}


//------------------------------------------------------------------------------
ConsoleFunctionGroupBegin(RandomNumbers, "Functions relating to the generation of random numbers.");

ConsoleFunction(setRandomSeed, void, 1, 2, "( startSeed ) Use the setRandomSeed function to initialize the random number generator with the initial seed of startSeed.\n"
                                                                "@param startSeed The new starting seed value for the random generator.\n"
                                                                "@return No return value.\n"
                                                                "@sa getRandom, getRandomSeed")
{
    U32 seed = Platform::getRealMilliseconds();
    if (argc == 2)
        seed = dAtoi(argv[1]);
    RandomLCG::setGlobalRandSeed(seed);
}

ConsoleFunction(getRandomSeed, S32, 1, 1, "() Use the getRandomSeed function to get the current seed for the random generator.\n"
                                                                "You can re-seed the generator with this value to allow you to recreate a random sequence. Merely save the seed and execute your random sequence. Later, to reproduce this sequence re-seed the generator with setRandomSeed and your saved value. Then, the generator will produce the same random sequence that followed the call to getRandomSeed.\n"
                                                                "@return Returns an integer value representing the current seed of the random generator.\n"
                                                                "@sa getRandom, setRandomSeed")
{
   return gRandGen.getSeed();
}

S32 mRandI( const S32 i1, const S32 i2)
{
   return gRandGen.randRangeI(i1, i2);
}

F32 mRandF(const F32 f1, const F32 f2)
{
   return gRandGen.randRangeF(f1, f2);
}

ConsoleFunction(getRandom, F32, 1, 3, "([ min ],[ max ]) Use the getRandom function to get a random floating-point or integer value. This function comes in three forms.\n"
                                                                "The first getRandom() takes no arguments and will return a random floating-point value in the range of 0.0 to 1.0. The second getRandom( max ) takes one argument representing the max integer value this will return. It will return an integer value between 0 and max. The third getRandom( min , max ) takes two arguments representing the min and max integer values this will return. It will return an integer value between min and max. Only the no-args version will return a floating point.\n"
                                                                "@param min Minimum inclusive integer value to return.\n"
                                                                "@param max Maximum inclusive integer value to return.\n"
                                                                "@return If no arguments are passed, will return a floating-point value between 0.0 and 1.0. If one argument is passed, will return an integer value between 0 and max inclusive. If two arguments are passed, will return an integer value between min and max inclusive.\n"
                                                                "@sa getRandomSeed")
{
   if (argc == 2)
      return F32(gRandGen.randRangeI(0,getMax( dAtoi(argv[1]), 0 )));
   else
   {
      if (argc == 3) 
      {
         S32 min = dAtoi(argv[1]);
         S32 max = dAtoi(argv[2]);
         if (min > max) 
         {
            S32 t = min;
            min = max;
            max = t;
         }
         return F32(gRandGen.randRangeI(min,max));
      }
   }
   return gRandGen.randF();
}

ConsoleFunction(getRandomF, F32, 3, 3,  "(min, max) Gets a random floating-point number from min to max.\n"
                                        "@param min The minimum range of the random floating-point number.\n"
                                        "@param max The maximum range of the random floating-point number.\n"
                                        "@return A random floating-point number from min to max.\n" )
{
    F32 min = dAtof(argv[1]);
    F32 max = dAtof(argv[2]);
    
    if ( min > max )
    {
        const F32 temp = min;
        min = max;
        max = temp;
    }
    
    return min + (gRandGen.randF() * (max-min));
    
}

ConsoleFunctionGroupEnd(RandomNumbers);
//------------------------------------------------------------------------------
