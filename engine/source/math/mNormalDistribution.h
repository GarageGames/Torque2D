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

#ifndef _MNORMALDISTRIBUTION_H_
#define _MNORMALDISTRIBUTION_H_

#include <random>

//-----------------------------------------------------------------------------
// Modified from this Stack Overflow answer: http://stackoverflow.com/a/28619226/289956
//-----------------------------------------------------------------------------

class NormalDistributionGenerator
{
   std::mt19937 generator;
   std::normal_distribution<F64> distribution;
   S32 min;
   S32 max;
public:
   NormalDistributionGenerator(S32 min, S32 max) :
      distribution(std::round((min + max) / 2), std::round((max - min) / 6)), min(min), max(max)
   {
      generator.seed(Platform::getRealMilliseconds());
   }

   NormalDistributionGenerator(S32 min, S32 max, S32 mean) :
      distribution(mean, std::round((max - min) / 6)), min(min), max(max)
   {
      generator.seed(Platform::getRealMilliseconds());
   }

   NormalDistributionGenerator(S32 min, S32 max, S32 mean, S32 stddev) :
      distribution(mean, stddev), min(min), max(max)
   {
      generator.seed(Platform::getRealMilliseconds());
   }

   S32 operator ()() {
      while (true) {
         S32 number = (S32)std::round(this->distribution(generator));
         if (number >= this->min && number <= this->max)
            return number;
      }
   }
};

#endif //_MNORMALDISTRIBUTION_H_
