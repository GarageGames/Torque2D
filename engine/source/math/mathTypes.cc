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

#include "vector_ScriptBinding.h"
#include "matrix_ScriptBinding.h"
#include "random_ScriptBinding.h"
#include "box_ScriptBinding.h"

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

//------------------------------------------------------------------------------

