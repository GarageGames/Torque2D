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

#include "math/mMath.h"
#include "graphics/dgl.h"
#include "graphics/gBitmap.h"

#if !defined(__MWERKS__) && defined(_MSC_VER)
#define asm _asm
#endif

//--------------------------------------------------------------------------
//void terrMipBlit_asm(U16 *dest, U32 destStride, U32 squareSize, const U8 *sourcePtr, U32 sourceStep, U32 sourceRowAdd)
//{
//   //for(U32 k = 0; k < squareSize; k++)
//   //{
//   //   for(U32 l = 0; l < squareSize; l++)
//   //   {
//   //      dest[l] = *((U16 *) sourcePtr);
//   //      sourcePtr += sourceStep;
//   //   }
//   //   dest -= destStride;
//   //   sourcePtr += sourceRowAdd;
//   //}
//   if(sourceStep == 2)
//   {
//      destStride <<= 1;
//      sourceRowAdd += squareSize << 1;
//      asm
//      {
//         push  eax
//         push  ebx
//         push  ecx
//         push  edx
//         push  edi
//         push  esi
//
//         mov   edx, squareSize
//         mov   edi, dest
//         mov   esi, sourcePtr
//         shr   edx, 1
//         mov   ecx, 0
//         mov   ebx, 0
//      pixelLoop2:
//         mov   eax, [esi+ebx*4]
//         mov   [edi+ebx*4], eax
//         inc   ebx
//         cmp   ebx, edx
//         jnz   pixelLoop2
//
//         mov   ebx, 0
//         inc   ecx
//         sub   edi, destStride
//         add   esi, sourceRowAdd
//         cmp   ecx, squareSize
//         jl    pixelLoop2
//
//         pop   esi
//         pop   edi
//         pop   edx
//         pop   ecx
//         pop   ebx
//         pop   eax
//      }
//   }
//   else if(sourceStep == -2)
//   {
//      destStride <<= 1;
//      asm
//      {
//         push  eax
//         push  ebx
//         push  ecx
//         push  edx
//         push  edi
//         push  esi
//
//         mov   edx, squareSize
//         mov   edi, dest
//         mov   esi, sourcePtr
//         shr   edx, 1
//         mov   ecx, 0
//         mov   ebx, 0
//      pixelLoopNeg2:
//         mov   eax, [esi-2]
//         sub   esi, 4
//         ror   eax, 16
//         mov   [edi+ebx*4], eax
//         inc   ebx
//         cmp   ebx, edx
//         jnz   pixelLoopNeg2
//
//         mov   ebx, 0
//         inc   ecx
//         sub   edi, destStride
//         add   esi, sourceRowAdd
//         cmp   ecx, squareSize
//         jl    pixelLoopNeg2
//
//         pop   esi
//         pop   edi
//         pop   edx
//         pop   ecx
//         pop   ebx
//         pop   eax
//      }
//   }
//   else
//   {
//      destStride = (destStride + squareSize) << 1;
//      asm
//      {
//         push  eax
//         push  ebx
//         push  ecx
//         push  edx
//         push  edi
//         push  esi
//
//         mov   eax, squareSize
//         mov   edi, dest
//         mov   esi, sourcePtr
//         lea   edx, [edi + eax * 2]
//         mov   ecx, 0  // row index
//         mov   ebx, sourceStep
//   pixelLoop:
//         mov   ax, [esi+ebx]
//         shl   eax, 16
//         add   edi, 4
//         mov   ax, [esi]
//         lea   esi, [esi+ebx*2]
//         mov   [edi-4], eax
//         cmp   edi, edx
//         jnz   pixelLoop
//
//         inc   ecx
//         sub   edi, destStride
//         mov   eax, squareSize
//         add   esi, sourceRowAdd
//         lea   edx, [edi + eax * 2]
//         cmp   ecx, squareSize
//         jl    pixelLoop
//
//         pop   esi
//         pop   edi
//         pop   edx
//         pop   ecx
//         pop   ebx
//         pop   eax
//      }
//   }
//}


//--------------------------------------------------------------------------
void bitmapExtrude5551_asm(const void *srcMip, void *mip, U32 height, U32 width)
{
   const U16 *src = (const U16 *) srcMip;
   U16 *dst = (U16 *) mip;
   U32 stride = width << 1;

   for(U32 y = 0; y < height; y++)
   {
      for(U32 x = 0; x < width; x++)
      {
         U32 a = src[0];
         U32 b = src[1];
         U32 c = src[stride];
         U32 d = src[stride+1];
         dst[x] = ((((a >> 11) + (b >> 11) + (c >> 11) + (d >> 11)) >> 2) << 11) |
                  (((  ((a >> 6) & 0x1f) + ((b >> 6) & 0x1f) + ((c >> 6) & 0x1f) + ((d >> 6) & 0x1F) ) >> 2) << 6) |
                  ((( ((a >> 1) & 0x1F) + ((b >> 1) & 0x1F) + ((c >> 1) & 0x1f) + ((d >> 1) & 0x1f)) >> 2) << 1);
         src += 2;
      }
      src += stride;
      dst += width;
   }
}


#if defined(TORQUE_SUPPORTS_VC_INLINE_X86_ASM)

//--------------------------------------------------------------------------
void bitmapExtrudeRGB_mmx(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   if (srcHeight == 1 || srcWidth == 1) {
      bitmapExtrudeRGB_c(srcMip, mip, srcHeight, srcWidth);
      return;
   }

   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;

   if (width <= 1)
   {
      bitmapExtrudeRGB_c(srcMip, mip, srcHeight, srcWidth);
      return;
   }

   U64 ZERO = 0x0000000000000000;
   const U8 *src = (const U8 *) srcMip;
   U8 *dst = (U8 *) mip;
   U32 srcStride = (width << 1) * 3;
   U32 dstStride = width * 3;

   for(U32 y = 0; y < height; y++)
   {
      asm
      {
         mov      eax, src
         mov      ebx, eax
         add      ebx, srcStride
         mov      ecx, dst
         mov      edx, width

         //--------------------------------------
      row_loop:

         punpcklbw   mm0, [eax]
         psrlw       mm0, 8

         punpcklbw   mm1, [eax+3]
         psrlw       mm1, 8
         paddw       mm0, mm1

         punpcklbw   mm1, [ebx]
         psrlw       mm1, 8
         paddw       mm0, mm1

         punpcklbw   mm1, [ebx+3]
         psrlw       mm1, 8
         paddw       mm0, mm1

         psrlw       mm0, 2
         //pxor        mm1, mm1
         packuswb    mm0, ZERO      // mm1

         movd        [ecx], mm0
         add         eax, 6
         add         ebx, 6
         add         ecx, 3
         dec         edx
         jnz         row_loop
      }
      src += srcStride + srcStride;   // advance to next line
      dst += dstStride;
   }
   asm
   {
      emms
   }
}


//--------------------------------------------------------------------------
void bitmapConvertRGB_to_5551_mmx(U8 *src, U32 pixels)
{
   U64 MULFACT      = 0x0008200000082000;   	// RGB quad word multiplier
   U64 REDBLUE      = 0x00f800f800f800f8;    // Red-Blue mask
   U64 GREEN        = 0x0000f8000000f800;    // Green mask
   U64 ALPHA        = 0x0000000000010001;    // 100% Alpha mask
   U64 ZERO         = 0x0000000000000000;

   U32 evenPixels = pixels >> 1;       // the MMX loop can only do an even number
   U32 oddPixels  = pixels & 1;        // of pixels since it processes 2 at a time

   U16 *dst = (U16*)src;

   if (evenPixels)
   {
      asm
      {
         mov         eax, src          // YES, src = dst at start
         mov         ebx, dst          // convert image in place
         mov         edx, evenPixels

      pixel_loop2:
         movd        mm0, [eax]        // get first 24-bit pixel
         movd        mm1, [eax+3]      // get second 24-bit pixel
         punpckldq   mm0, mm1          // put second in high dword
         movq        mm1, mm0          // save the original data
         pand        mm0, REDBLUE      // mask out all but the 5MSBits of red and blue
         pmaddwd     mm0, MULFACT      // multiply each word by
                                       //   2**13, 2**3, 2**13, 2**3 and add results
         pand        mm1, GREEN        // mask out all but the 5MSBits of green
         por         mm0, mm1          // combine the red, green, and blue bits
         psrld       mm0, 6            // shift into position
         packssdw    mm0, ZERO         // pack into single dword
         pslld       mm0, 1            // shift into final position
         por         mm0, ALPHA        // add the alpha bit
         movd        [ebx], mm0

         add         eax, 6
         add         ebx, 4
         dec         edx
         jnz         pixel_loop2

         mov         src, eax
         mov         dst, ebx
         emms
      }
   }

   if (oddPixels)
   {
      U32 r = src[0] >> 3;
      U32 g = src[1] >> 3;
      U32 b = src[2] >> 3;

      *dst = (b << 1) | (g << 6) | (r << 11) | 1;
   }
}

#endif



//--------------------------------------------------------------------------
void PlatformBlitInit()
{
   bitmapExtrude5551 = bitmapExtrude5551_asm;
   bitmapExtrudeRGB  = bitmapExtrudeRGB_c;

   if (PlatformSystemInfo.processor.properties & CPU_PROP_MMX)
   {
#if defined(TORQUE_SUPPORTS_VC_INLINE_X86_ASM)
      bitmapExtrudeRGB  = bitmapExtrudeRGB_mmx;
      bitmapConvertRGB_to_5551 = bitmapConvertRGB_to_5551_mmx;
#endif
   }
//   terrMipBlit = terrMipBlit_asm;
}
