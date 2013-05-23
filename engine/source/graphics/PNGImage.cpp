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
#include "PNGImage.h"
#include "math/mPoint.h"
#include "graphics/gBitmap.h"
#include "platform/platformGL.h"
#include "platform/platform.h"

#include "PNGImage_ScriptBinding.h"

#define min(a,b) (a <= b ? a : b)

IMPLEMENT_CONOBJECT(PNGImage);

#define PNGSIGSIZE 8

PNGImage::PNGImage() : mPNGImageType(PNGTYPE_UNKNOWN)
{
    mWidth = 0;
    mHeight = 0;

    mColorType = 0;
    mBitDepth = 0;

    mPng = 0;
    mInfo = 0;
    mRowPointers = 0;

    mRead = false;
    mWrite = false;
}

PNGImage::~PNGImage()
{

}

bool PNGImage::Read(const char* filePath)
{
    if(mRead == true || mWrite == true)
        CleanMemoryUsage();

    /* open file and test for it being a png */
    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL)
    {
        Con::printf("PNGImage::Read File %s could not be opened for reading.", filePath);
        return false;
    }

    dStrcpy(mReadFilePath, filePath);

    png_byte pngsig[PNGSIGSIZE];
    fread((char*)pngsig, 1, PNGSIGSIZE, fp);

    if (png_sig_cmp(pngsig, 0, PNGSIGSIZE) != 0)
    {
        Con::printf("PNGImage::Read File %s is not recognized as a PNG file.", filePath);
        return false;
    }

    /* initialize stuff */
    mPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (mPng == NULL)
    {
        Con::printf("PNGImage::Read png_create_read_struct failed.");
        return false;
    }

    mInfo = png_create_info_struct(mPng);
    if (mInfo == NULL)
    {
        png_destroy_read_struct(&mPng, NULL, NULL);
        Con::printf("PNGImage::Read png_create_info_struct failed.");
        return false;
    }

    if (setjmp(png_jmpbuf(mPng)))
    {
        png_destroy_read_struct(&mPng, NULL, NULL);
        Con::printf("PNGImage::Read Error during init_io.");
        return false;
    }

    png_init_io(mPng, fp);
    png_set_sig_bytes(mPng, 8);

    png_read_info(mPng, mInfo);

    // Obtain image information
    mWidth = png_get_image_width(mPng, mInfo);
    mHeight = png_get_image_height(mPng, mInfo);
    mColorType = png_get_color_type(mPng, mInfo);
    mBitDepth = png_get_bit_depth(mPng, mInfo);

    mRowPointers = (png_bytep*) dMalloc(sizeof(png_bytep) * mHeight);

    unsigned int y = 0;
    for (y = 0; y < mHeight; y++)
        mRowPointers[y] = (png_byte*) dMalloc(png_get_rowbytes(mPng,mInfo));

    // Will read the actual png data into mRowPointers
    png_read_image(mPng, mRowPointers);

    fclose(fp);

    mRead = true;

    if(png_get_color_type(mPng, mInfo) == PNG_COLOR_TYPE_RGBA)
        mPNGImageType = PNGTYPE_RGBA;
    else if(png_get_color_type(mPng, mInfo) == PNG_COLOR_TYPE_RGB)
        mPNGImageType = PNGTYPE_RGB;
    else
        mPNGImageType = PNGTYPE_UNKNOWN;

    return true;
}

bool PNGImage::Create(U32 width, U32 height, PNGImageType imageType)
{
    mRead = true;

    mHeight = height;
    mWidth = width;

    mRowPointers = (png_bytep*) dMalloc(sizeof(png_bytep) * mHeight);

    unsigned int y = 0;
    for (y = 0; y < height; y++)
        mRowPointers[y] = (png_byte*) dMalloc(sizeof(png_bytep) * mWidth);

    if(imageType == PNGTYPE_RGBA)
    {
        mColorType = PNG_COLOR_TYPE_RGBA;
        mPNGImageType = PNGTYPE_RGBA;
    }
    else if(imageType == PNGTYPE_RGB)
    {
        mColorType = PNG_COLOR_TYPE_RGB;
        mPNGImageType = PNGTYPE_RGB;
    }
    else
    {
        mPNGImageType = PNGTYPE_UNKNOWN;
    }

    mBitDepth = 8;

    ClearImageData();

    return true;
}

bool PNGImage::ClearImageData()
{
    if(mRowPointers == NULL)
        return false;

    if(mPNGImageType == PNGTYPE_RGBA)
    {
        for(U32 h = 0; h < mHeight; ++h)
        {
            png_byte* currentRow = mRowPointers[h];

            for(U32 w = 0; w < mWidth; ++w)
            {
                png_byte* currentPngByte = &(currentRow[w*4]);
                currentPngByte[0] = 255;
                currentPngByte[1] = 255;
                currentPngByte[2] = 255;
                currentPngByte[3] = 255;
            }
        }
    }
    else if(mPNGImageType == PNGTYPE_RGB)
    {
        for(U32 h = 0; h < mHeight; ++h)
        {
            png_byte* currentRow = mRowPointers[h];

            for(U32 w = 0; w < mWidth; ++w)
            {
                png_byte* currentPngByte = &(currentRow[w*3]);
                currentPngByte[0] = 255;
                currentPngByte[1] = 255;
                currentPngByte[2] = 255;
            }
        }
    }

    return true;
}

bool PNGImage::MergeOn(U32 x, U32 y, const PNGImage* inc)
{
    if(x + inc->GetWidth() > mWidth || y + inc->GetHeight() > mHeight)
        return false;

    if(mPNGImageType == PNGTYPE_RGBA && inc->GetPNGImageType() == PNGTYPE_RGBA)
    {
        for(U32 h = 0; h < inc->GetHeight(); ++h)
        {
            png_byte* incRow = inc->GetRowPointers()[h];
            png_byte* currentRow = mRowPointers[y + h];

            for(U32 w = 0; w < inc->GetWidth(); ++w)
            {
                png_byte* incPngByte = &(incRow[w*4]);
                png_byte* currentPngByte = &(currentRow[(x+w)*4]);

                //float incAlpha = (incPngByte[3]/255.0f);
                //float currentAlpha = (currentPngByte[3]/255.0f);

                //float red = ((float)incPngByte[0]/255.0f * incAlpha) + ((float)currentPngByte[0]/255.0f * (currentAlpha - incAlpha));
                //float green = ((float)incPngByte[1]/255.0f * incAlpha) + ((float)currentPngByte[1]/255.0f * (currentAlpha - incAlpha));
                //float blue = ((float)incPngByte[2]/255.0f * incAlpha) + ((float)currentPngByte[2]/255.0f * (currentAlpha - incAlpha));
                ////float alpha = ((float)incPngByte[3]/255.0f * incAlpha) + ((float)currentPngByte[3]/255.0f * (currentAlpha - incAlpha));

                //currentPngByte[0] = (png_byte)min(red * 255.0f, 255.0f);
                //currentPngByte[1] = (png_byte)min(green * 255.0f, 255.0f);
                //currentPngByte[2] = (png_byte)min(blue * 255.0f, 255.0f);
                ////currentPngByte[3] = (png_byte)min(blue * 255.0f, 255.0f);

                currentPngByte[0] = incPngByte[0];
                currentPngByte[1] = incPngByte[1];
                currentPngByte[2] = incPngByte[2];
            }
        }
    }
    else if(mPNGImageType == PNGTYPE_RGB && inc->GetPNGImageType() == PNGTYPE_RGBA)
    {
        for(U32 h = 0; h < inc->GetHeight(); ++h)
        {
            png_byte* incRow = inc->GetRowPointers()[h];
            png_byte* currentRow = mRowPointers[y + h];

            for(U32 w = 0; w < inc->GetWidth(); ++w)
            {
                png_byte* incPngByte = &(incRow[w*4]);
                png_byte* currentPngByte = &(currentRow[(x+w)*3]);

                float incAlpha = (incPngByte[3]/255.0f);

                float red = ((float)incPngByte[0]/255.0f * incAlpha) + ((float)currentPngByte[0]/255.0f * (1.0f - incAlpha));
                float green = ((float)incPngByte[1]/255.0f * incAlpha) + ((float)currentPngByte[1]/255.0f * (1.0f - incAlpha));
                float blue = ((float)incPngByte[2]/255.0f * incAlpha) + ((float)currentPngByte[2]/255.0f * (1.0f - incAlpha));

                currentPngByte[0] = (png_byte)min(red * 255.0f, 255.0f);
                currentPngByte[1] = (png_byte)min(green * 255.0f, 255.0f);
                currentPngByte[2] = (png_byte)min(blue * 255.0f, 255.0f);
            }
        }
    }
    else if(mPNGImageType == PNGTYPE_RGBA && inc->GetPNGImageType() == PNGTYPE_RGB)
    {
        for(U32 h = 0; h < inc->GetHeight(); ++h)
        {
            png_byte* incRow = inc->GetRowPointers()[h];
            png_byte* currentRow = mRowPointers[y + h];

            for(U32 w = 0; w < inc->GetWidth(); ++w)
            {
                png_byte* incPngByte = &(incRow[w*3]);
                png_byte* currentPngByte = &(currentRow[(x+w)*4]);

                currentPngByte[0] = incPngByte[0];
                currentPngByte[1] = incPngByte[1];
                currentPngByte[2] = incPngByte[2];
            }
        }
    }
    else if(mPNGImageType == PNGTYPE_RGB && inc->GetPNGImageType() == PNGTYPE_RGB)
    {
        for(U32 h = 0; h < inc->GetHeight(); ++h)
        {
            png_byte* incRow = inc->GetRowPointers()[h];
            png_byte* currentRow = mRowPointers[y + h];

            for(U32 w = 0; w < inc->GetWidth(); ++w)
            {
                png_byte* incPngByte = &(incRow[w*3]);
                png_byte* currentPngByte = &(currentRow[(x+w)*3]);

                currentPngByte[0] = incPngByte[0];
                currentPngByte[1] = incPngByte[1];
                currentPngByte[2] = incPngByte[2];
            }
        }
    }

    return true;
}

bool PNGImage::Write(const char* filePath)
{
    if(mRead == false)
        return false;

    if(mPng != NULL && mInfo != NULL)
    {
        png_destroy_read_struct(&mPng, &mInfo, NULL);
        mPng = NULL;
        mInfo = NULL;
    }
    else
    {
        if(mPng != NULL)
        {
            png_destroy_read_struct(&mPng, NULL, NULL);
            mPng = NULL;
        }
        else if(mInfo != NULL)
        {
            png_read_info(mPng, mInfo);
            mInfo = NULL;
        }
    }

    /* create file */
    FILE *fp = fopen(filePath, "wb");
    if (!fp)
    {
        Con::printf("PNGImage::Write File %s could not be opened for writing.", filePath);
        return false;
    }

    // png_destroy_read_struct(&mPng, NULL, NULL);
    // png_destroy_info_struct(mPng, NULL);

    /* initialize stuff */
    mPng = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!mPng)
    {
        Con::printf("PNGImage::Write png_create_write_struct failed.");
        return false;
    }

    mInfo = png_create_info_struct(mPng);
    if (mInfo == NULL)
    {
        Con::printf("PNGImage::Write png_create_info_struct failed.");
        return false;
    }

    if (setjmp(png_jmpbuf(mPng)))
    {
        Con::printf("PNGImage::Write Error during init_io.");
        return false;
    }

    png_init_io(mPng, fp);

    /* write header */
    if (setjmp(png_jmpbuf(mPng)))
    {
        Con::printf("PNGImage::Write Error during writing header.");
        return false;
    }

    png_set_IHDR(mPng, mInfo, mWidth, mHeight,
        mBitDepth, mColorType, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(mPng, mInfo);

    /* write bytes */
    if (setjmp(png_jmpbuf(mPng)))
    {
        Con::printf("PNGImage::Write Error during writing bytes.");
        return false;
    }

    png_write_image(mPng, mRowPointers);

    /* end write */
    if (setjmp(png_jmpbuf(mPng)))
    {
        Con::printf("PNGImage::Write Error during end of write.");
        return false;
    }

    png_write_end(mPng, NULL);

    fclose(fp);

    mRead = false;
    mWrite = true;

    return true;
}

bool PNGImage::CleanMemoryUsage()
{
    if(mRead == true && mWrite == true)
        return false;

    if(mRead == true)
    {
        if(mPng != NULL && mInfo != NULL)
        {
            png_destroy_read_struct(&mPng, &mInfo, NULL);
            mPng = NULL;
            mInfo = NULL;
        }
        else
        {
            if(mPng != NULL)
            {
                png_destroy_read_struct(&mPng, NULL, NULL);
                mPng = NULL;
            }
            else if(mInfo != NULL)
            {
                png_read_info(mPng, mInfo);
                mInfo = NULL;
            }
        }

        for (unsigned int y = 0; y < mHeight; y++)
            dFree(mRowPointers[y]);

        dFree(mRowPointers);
        mRowPointers= NULL;
    }
    else if(mWrite == true)
    {
        if(mPng != NULL && mInfo != NULL)
        {
            png_destroy_write_struct(&mPng, &mInfo);
            mPng = NULL;
            mInfo = NULL;
        }
        else
        {
            if(mPng != NULL)
            {
                png_destroy_write_struct(&mPng, NULL);
                mPng = NULL;
            }

            if(mInfo != NULL)
            {
                png_read_info(mPng, mInfo);
                mInfo = NULL;
            }
        }
        for (unsigned int y = 0; y < mHeight; y++)
            dFree(mRowPointers[y]);

        dFree(mRowPointers);

        mRowPointers = NULL;
    }

    return true;
}
