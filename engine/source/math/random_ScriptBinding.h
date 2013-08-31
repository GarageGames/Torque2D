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

ConsoleFunctionGroupBegin(RandomNumbers, "Functions relating to the generation of random numbers.");


/*! @defgroup RandomNumbersFunctions Random Numbers
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the setRandomSeed function to initialize the random number generator with the initial seed of startSeed.
    @param startSeed The new starting seed value for the random generator.
    @return No return value.
    @sa getRandom, getRandomSeed
*/
ConsoleFunctionWithDocs(setRandomSeed, ConsoleVoid, 1, 2, ( startSeed ))
{
    U32 seed = Platform::getRealMilliseconds();
    if (argc == 2)
        seed = dAtoi(argv[1]);
    RandomLCG::setGlobalRandSeed(seed);
}

/*! Use the getRandomSeed function to get the current seed for the random generator.
    You can re-seed the generator with this value to allow you to recreate a random sequence. Merely save the seed and execute your random sequence. Later, to reproduce this sequence re-seed the generator with setRandomSeed and your saved value. Then, the generator will produce the same random sequence that followed the call to getRandomSeed.
    @return Returns an integer value representing the current seed of the random generator.
    @sa getRandom, setRandomSeed
*/
ConsoleFunctionWithDocs(getRandomSeed, ConsoleInt, 1, 1, ())
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

/*! Use the getRandom function to get a random floating-point or integer value. This function comes in three forms.
    The first getRandom() takes no arguments and will return a random floating-point value in the range of 0.0 to 1.0. The second getRandom( max ) takes one argument representing the max integer value this will return. It will return an integer value between 0 and max. The third getRandom( min , max ) takes two arguments representing the min and max integer values this will return. It will return an integer value between min and max. Only the no-args version will return a floating point.
    @param min Minimum inclusive integer value to return.
    @param max Maximum inclusive integer value to return.
    @return If no arguments are passed, will return a floating-point value between 0.0 and 1.0. If one argument is passed, will return an integer value between 0 and max inclusive. If two arguments are passed, will return an integer value between min and max inclusive.
    @sa getRandomSeed
*/
ConsoleFunctionWithDocs(getRandom, ConsoleFloat, 1, 3, ([ min ]?,[ max ]?))
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

/*! Gets a random floating-point number from min to max.
    @param min The minimum range of the random floating-point number.
    @param max The maximum range of the random floating-point number.
    @return A random floating-point number from min to max.
*/
ConsoleFunctionWithDocs(getRandomF, ConsoleFloat, 3, 3, (min, max))
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

//------------------------------------------------------------------------------

ConsoleFunctionGroupEnd(RandomNumbers)

/*! @} */ // group RandomNumberFunctions
