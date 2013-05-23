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


/*! @defgroup BoxFunctions Box Math
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the getBoxCenter function to find the centroid of a cube (box).
	@param box A vector containing two three-element floating-point position vectors: \"X1 Y1 Z1 X2 Y2 Z2\".
	@return Returns a vector containing a three-element floating-point position vector equal to the centroid of the area defined by box
*/

ConsoleFunctionWithDocs( getBoxCenter, ConsoleString, 2, 2, (box) )
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

/*! @} */ // group BoxFunctions
