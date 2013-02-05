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

#include "math/mMathFn.h"
#include "math/mPlane.h"
#include "math/mMatrix.h"


// if we set our flag, we always try to build the inlined asm.
// EXCEPT if we're in an old version of Codewarrior that can't handle SSE code.
#if defined(TORQUE_SUPPORTS_NASM)
#define ADD_SSE_FN
extern "C"
{
   void SSE_MatrixF_x_MatrixF(const F32 *matA, const F32 *matB, F32 *result);
}

#elif defined(TORQUE_SUPPORTS_VC_INLINE_X86_ASM)
#define ADD_SSE_FN
// inlined version here.
void SSE_MatrixF_x_MatrixF(const F32 *matA, const F32 *matB, F32 *result)
{
   __asm
   {
      mov         ecx, matA
      mov         edx, matB
      mov         eax, result

      movss       xmm0, [edx]
      movups      xmm1, [ecx]
      shufps      xmm0, xmm0, 0
      movss       xmm2, [edx+4]
      mulps       xmm0, xmm1
      shufps      xmm2, xmm2, 0
      movups      xmm3, [ecx+10h]
      movss       xmm7, [edx+8]
      mulps       xmm2, xmm3
      shufps      xmm7, xmm7, 0
      addps       xmm0, xmm2
      movups      xmm4, [ecx+20h]
      movss       xmm2, [edx+0Ch]
      mulps       xmm7, xmm4
      shufps      xmm2, xmm2, 0
      addps       xmm0, xmm7
      movups      xmm5, [ecx+30h]
      movss       xmm6, [edx+10h]
      mulps       xmm2, xmm5
      movss       xmm7, [edx+14h]
      shufps      xmm6, xmm6, 0
      addps       xmm0, xmm2
      shufps      xmm7, xmm7, 0
      movlps      [eax], xmm0
      movhps      [eax+8], xmm0
      mulps       xmm7, xmm3
      movss       xmm0, [edx+18h]
      mulps       xmm6, xmm1
      shufps      xmm0, xmm0, 0
      addps       xmm6, xmm7
      mulps       xmm0, xmm4
      movss       xmm2, [edx+24h]
      addps       xmm6, xmm0
      movss       xmm0, [edx+1Ch]
      movss       xmm7, [edx+20h]
      shufps      xmm0, xmm0, 0
      shufps      xmm7, xmm7, 0
      mulps       xmm0, xmm5
      mulps       xmm7, xmm1
      addps       xmm6, xmm0
      shufps      xmm2, xmm2, 0
      movlps      [eax+10h], xmm6
      movhps      [eax+18h], xmm6
      mulps       xmm2, xmm3
      movss       xmm6, [edx+28h]
      addps       xmm7, xmm2
      shufps      xmm6, xmm6, 0
      movss       xmm2, [edx+2Ch]
      mulps       xmm6, xmm4
      shufps      xmm2, xmm2, 0
      addps       xmm7, xmm6
      mulps       xmm2, xmm5
      movss       xmm0, [edx+34h]
      addps       xmm7, xmm2
      shufps      xmm0, xmm0, 0
      movlps      [eax+20h], xmm7
      movss       xmm2, [edx+30h]
      movhps      [eax+28h], xmm7
      mulps       xmm0, xmm3
      shufps      xmm2, xmm2, 0
      movss       xmm6, [edx+38h]
      mulps       xmm2, xmm1
      shufps      xmm6, xmm6, 0
      addps       xmm2, xmm0
      mulps       xmm6, xmm4
      movss       xmm7, [edx+3Ch]
      shufps      xmm7, xmm7, 0
      addps       xmm2, xmm6
      mulps       xmm7, xmm5
      addps       xmm2, xmm7
      movups      [eax+30h], xmm2
   }
}
#endif


void mInstall_Library_SSE()
{
#if defined(ADD_SSE_FN)
   m_matF_x_matF           = SSE_MatrixF_x_MatrixF;
   // m_matF_x_point3F = Athlon_MatrixF_x_Point3F;
   // m_matF_x_vectorF = Athlon_MatrixF_x_VectorF;
#endif
}
