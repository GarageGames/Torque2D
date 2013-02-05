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

#include <stdlib.h>
#include "/usr/local/include/png.h"
#include "/usr/local/include/pngconf.h"
//#include "stdafx.h"
//#include "C:\\Program Files\\GnuWin32\\include\\png.h"

#define MAX_ARGS 10
#define TEMP_OUTPUT_FILE "xxtempxx.pvr"
#define TEST_PVR_FILE "compare.pvr"

extern bool readPvr( FILE* fp);
extern bool writePvr( FILE* fp);

bool readPNG( char *filename);

int gBitsPerPixel = -1;
int gChannelWeight = -1;
int gGenerateMipMaps = -1;
char gPNGFileName[256];
char gPVRFileName[256];
unsigned long gWidth;
unsigned long gHeight;
int gBitDepth;
int gColorType;
long gFsize;
bool gIsAlpha;

bool parseArgs(int argc, char * const argv[]);
void usage();

int main (int argc, char * const argv[]) {

#if 0
	FILE *tf;
	tf = fopen(TEST_PVR_FILE, "rb");
	readPvr(tf);
	fclose(tf);
#endif
	
	FILE *wf;
	FILE *rf;
    // insert code here...
    memset(gPNGFileName,0,256);
    memset(gPVRFileName,0,256);
	
	if(parseArgs(argc, argv)){
#if 1		
		char *argv2[MAX_ARGS];
		for(int i=0; i<MAX_ARGS-1; i++){
			argv2[i] = new char[256];
		}
		strcpy(argv2[0],"texturetool");
		strcpy(argv2[1],"-e");
		strcpy(argv2[2],"PVRTC");
		if(gChannelWeight==0){
			strcpy(argv2[3],"--channel-weighting-linear");
		}
		else if(gChannelWeight==1){		
			strcpy(argv2[3],"--channel-weighting-perceptual");
		}
		else{ 
			usage();
			return -1;
		}
		if(gBitsPerPixel==2){
			strcpy(argv2[4],"--bits-per-pixel-2");
		}
		else if(gBitsPerPixel==4){		
			strcpy(argv2[4],"--bits-per-pixel-4");
		}
		else{ 
			usage();
			return -1;
		}

		strcpy(argv2[5],gPNGFileName);

		strcpy(argv2[6],"-o");
		strcpy(argv2[7],TEMP_OUTPUT_FILE);
		
		if(gGenerateMipMaps==1){
			strcpy(argv2[8],"-m");
		}
		else{
			delete argv2[MAX_ARGS-2];
			argv2[MAX_ARGS-2] = NULL;
		}
		argv2[MAX_ARGS-1] = NULL;

		char *newargs = new char[8*1024];
		memset(newargs, 0, 8*1024);
		sprintf(newargs,"./%s",argv2[0]);
		for(int i=1; i<MAX_ARGS-1; i++){
			if(argv2[i]){
				strcat(newargs," ");
				strcat(newargs,argv2[i]);
			}
		}
		
		system(newargs);
		//if(execv("texturetool", argv2)<0){
		//	usage();
		//	return -1;
		//}
		delete newargs;
		//printf("\nHere %d\n",gFsize);

		if(!readPNG(gPNGFileName)){
			usage();
			return -1;
		}
		
		wf = fopen(gPVRFileName, "wb");
		if(!wf){
			printf("\nCannot open file %s\n\n",gPVRFileName);
			return -1;
		}
#endif
		rf = fopen(TEMP_OUTPUT_FILE, "rb");
		if(!rf){
			printf("\nCannot open file %s\n\n",TEMP_OUTPUT_FILE);
			return -1;
		}
		fseek(rf,0,SEEK_END);
		gFsize = ftell(rf);
		//printf("\nHere %d\n",gFsize);
		fseek(rf,0,SEEK_SET);
		char *filedata = new char[gFsize];
		fread(filedata,1,gFsize,rf);
		fclose(rf);

		writePvr(wf);
		fwrite(filedata,1,gFsize,wf);
		fclose(wf);
		delete filedata;

		return 0;

	}
	usage();

    return -1;
}

void usage(){
	printf("\n");
	printf("Usage: PVRTC_Mac [-m] -b# -wX -i <input_png> -o <output_pvr>");
	printf("\n");
	printf("    -m            Generate a complete mipmap chain from the input image.\n");
	printf("    -b#           Bits per pixel either 2 or 4 is valid.\n");
	printf("    -wX           Encoding either l = linear or p = perceptual.\n");
	printf("    -i <input>    <input> PNG file must be square power of 2.\n");
	printf("    -o <output>   Write processed image to <output>.");
	printf("\n\n");
	printf("Example:\n");
	printf("	PVRTC_Mac -b2 -wp -i my.png -o my.pvr\n\n");
	printf("This Will encode my.png with perceptual coding at 2 bits per pixel and output my.pvr.\n");
	printf("\n\n");
/*
	Usage: texturetool [-hlm] [-e <encoder>] [-p <preview_file>] -o <output> [-f <format>] input_image

    -h            Display this help menu.
    -l            List available encoders, individual encoder options, and file formats.
    -m            Generate a complete mipmap chain from the input image.
    -e <encoder>        Encode texture levels with <encoder>.
    -p <preview_file>    Output a PNG preview of the encoded output to <preview_file>. Requires -e option.
    -o <output>        Write processed image to <output>.
    -f <format>        Set file <format> for <output> image.

	PVRTC
	 --channel-weighting-linear
	 --channel-weighting-perceptual
	 --bits-per-pixel-2
	 --bits-per-pixel-4

	Formats:
	Raw
	PVR
*/
}

bool parseArgs(int argc, char * const argv[]){
	if(argc < 7) return false;
	
	for(int i=1; i<argc; i++){
		if(strcmp(argv[i],"-b2")==0){
			gBitsPerPixel = 2;
		}
		if(strcmp(argv[i],"-b4")==0){
			gBitsPerPixel = 4;
		}
		if(strcmp(argv[i],"-wl")==0){
			gChannelWeight = 0;
		}
		if(strcmp(argv[i],"-wp")==0){
			gChannelWeight = 1;
		}
		if(strcmp(argv[i],"-m")==0){
			gGenerateMipMaps = 1;
		}
		if(strcmp(argv[i],"-i")==0){
			i++;
			if(i<argc){
				strcpy(gPNGFileName,argv[i]);
			}
			else{
				return false;
			}
		}
		if(strcmp(argv[i],"-o")==0){
			i++;
			if(i<argc){
				strcpy(gPVRFileName,argv[i]);
			}
			else{
				return false;
			}
		}
	}
	return true;
}

//--------------------------------------
static void pngFatalErrorFn(png_structp     /*png_ptr*/,
                            png_const_charp pMessage)
{
	printf("Error reading PNG file:\n %s", pMessage);
}


//--------------------------------------
static void pngWarningFn(png_structp, png_const_charp pMessage)
{
	printf("Warning reading PNG file:\n %s", pMessage);
}

png_voidp gPngErrorPtr;

bool readPNG( char *filename){
	static const unsigned int cs_headerBytesChecked = 8;
	unsigned char header[cs_headerBytesChecked];
	
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }
    fread(header, 1, cs_headerBytesChecked, fp);
    bool is_png = !png_sig_cmp(header, 0, cs_headerBytesChecked);
    if (!is_png)
    {
		fclose(fp);
        return false;
    }
	
	png_structp png_ptr = png_create_read_struct
	(PNG_LIBPNG_VER_STRING, (png_voidp)gPngErrorPtr,
	 pngFatalErrorFn, pngWarningFn);
    if (!png_ptr){
		fclose(fp);
        return false;
	}
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr,
								(png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
        return false;
    }
	
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
								(png_infopp)NULL);
		fclose(fp);
        return false;
    }
	
	if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
								&end_info);
        fclose(fp);
        return false;
    }
	
	png_init_io(png_ptr, fp);
	
	png_set_sig_bytes(png_ptr, cs_headerBytesChecked);
	
	png_read_info(png_ptr, info_ptr);
	
	png_get_IHDR(png_ptr, info_ptr,
				 &gWidth, &gHeight,             // obv.
				 &gBitDepth, &gColorType,     // obv.
				 NULL,                        // interlace
				 NULL,                        // compression_type
				 NULL);                       // filter_type
	
	bool transAlpha     = false;
	gIsAlpha = false;
	
	if(gWidth != gHeight){
		printf("Not a square input image %dx%d\n",gWidth,gHeight);
		return false;
	}
	if(gWidth > 1024){
		printf("Image %dx%d is too large for iPhone\n",gWidth,gHeight);
		return false;
	}
	int count = 0;
	int temp = gWidth;
	while(temp > 0){
		count += temp & 1;
		temp >>= 1;
	}
	if(count != 1){
		printf("Not a power of 2 image %dx%d\n",gWidth,gHeight);
		return false;
	}
	// Expand a transparency channel into a full alpha channel...
	//
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		transAlpha = true;
	}
	
	if (gColorType == PNG_COLOR_TYPE_PALETTE) 
	{
		gIsAlpha = transAlpha;
	} 
	else if (gColorType == PNG_COLOR_TYPE_GRAY_ALPHA) 
	{
		gIsAlpha = true;
	}
	else if (gColorType == PNG_COLOR_TYPE_RGB_ALPHA) 
	{
		gIsAlpha = true;
	}
	
	fclose(fp);
	return true;
}

/*
static void pngReadDataFn(png_structp  png_ptr,
                          png_bytep   data,
                          png_size_t  length)
{
	
	bool success = sg_pStream->read(length, data);
}


//--------------------------------------
static void pngWriteDataFn(png_structp png_ptr,
                           png_bytep   data,
                           png_size_t  length)
{
	AssertFatal(sg_pStream != NULL, "No stream?");
	
	sg_pStream->write(length, data);
}

//--------------------------------------
static void pngFlushDataFn(png_structp png_ptr)
{
	//
}
*/
/*
//--------------------------------------
bool GBitmap::readPNG(Stream& io_rStream)
{
	static const U32 cs_headerBytesChecked = 8;
	
	U8 header[cs_headerBytesChecked];
	io_rStream.read(cs_headerBytesChecked, header);
	
	bool isPng = png_check_sig(header, cs_headerBytesChecked) != 0;
	if (isPng == false) 
	{
		AssertWarn(false, "GBitmap::readPNG: stream doesn't contain a PNG");
		return false;
	}
	
	U32 prevWaterMark = FrameAllocator::getWaterMark();
	
#if defined(PNG_USER_MEM_SUPPORTED)
	png_structp png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
												   NULL,
												   pngFatalErrorFn,
												   pngWarningFn,
												   NULL,
												   pngMallocFn,
												   pngFreeFn);
#else
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
												 NULL,
												 pngFatalErrorFn,
												 pngWarningFn);
#endif
	
	if (png_ptr == NULL) 
	{
		FrameAllocator::setWaterMark(prevWaterMark);
		return false;
	}
	
	// Enable optimizations if appropriate.
#if defined(PNG_LIBPNG_VER) && (PNG_LIBPNG_VER >= 10200)
	png_uint_32 mask, flags;
	
	flags = png_get_asm_flags(png_ptr);
	mask = png_get_asm_flagmask(PNG_SELECT_READ | PNG_SELECT_WRITE);
	png_set_asm_flags(png_ptr, flags | mask);
#endif
	
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr,
								(png_infopp)NULL,
								(png_infopp)NULL);
		FrameAllocator::setWaterMark(prevWaterMark);
		return false;
	}
	
	png_infop end_info = png_create_info_struct(png_ptr);
	if (end_info == NULL) {
		png_destroy_read_struct(&png_ptr,
								&info_ptr,
								(png_infopp)NULL);
		FrameAllocator::setWaterMark(prevWaterMark);
		return false;
	}
	
	sg_pStream = &io_rStream;
	png_set_read_fn(png_ptr, NULL, pngReadDataFn);
	
	// Read off the info on the image.
	png_set_sig_bytes(png_ptr, cs_headerBytesChecked);
	png_read_info(png_ptr, info_ptr);
	
	// OK, at this point, if we have reached it ok, then we can reset the
	//  image to accept the new data...
	//
	deleteImage();
	
	png_uint_32 width;
	png_uint_32 height;
	S32 bit_depth;
	S32 color_type;
	
	png_get_IHDR(png_ptr, info_ptr,
				 &width, &height,             // obv.
				 &bit_depth, &color_type,     // obv.
				 NULL,                        // interlace
				 NULL,                        // compression_type
				 NULL);                       // filter_type
	
	// First, handle the color transformations.  We need this to read in the
	//  data as RGB or RGBA, _always_, with a maximal channel width of 8 bits.
	//
	bool transAlpha     = false;
	BitmapFormat format = RGB;
	
	// Strip off any 16 bit info
	//
	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}
	
	// Expand a transparency channel into a full alpha channel...
	//
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_expand(png_ptr);
		transAlpha = true;
	}
	
	if (color_type == PNG_COLOR_TYPE_PALETTE) 
	{
		png_set_expand(png_ptr);
		format = transAlpha ? RGBA : RGB;
	} 
	else if (color_type == PNG_COLOR_TYPE_GRAY) 
	{
		png_set_expand(png_ptr);
		//png_set_gray_to_rgb(png_ptr);
		format = Alpha; //transAlpha ? RGBA : RGB;
	}
	else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) 
	{
		png_set_expand(png_ptr);
		png_set_gray_to_rgb(png_ptr);
		format = RGBA;
	}
	else if (color_type == PNG_COLOR_TYPE_RGB) 
	{
		format = transAlpha ? RGBA : RGB;
		png_set_expand(png_ptr);
	}
	else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) 
	{
		png_set_expand(png_ptr);
		format = RGBA;
	}
	
	// Update the info pointer with the result of the transformations
	//  above...
	png_read_update_info(png_ptr, info_ptr);
	
	png_uint_32 rowBytes = png_get_rowbytes(png_ptr, info_ptr);
	if (format == RGB) {
		AssertFatal(rowBytes == width * 3,
					"Error, our rowbytes are incorrect for this transform... (3)");
	} 
	else if (format == RGBA) 
	{
		AssertFatal(rowBytes == width * 4,
					"Error, our rowbytes are incorrect for this transform... (4)");
	}
	
	// actually allocate the bitmap space...
	allocateBitmap(width, height,
				   false,            // don't extrude miplevels...
				   format);          // use determined format...
	
	// Set up the row pointers...
	AssertISV(height <= csgMaxRowPointers, "Error, cannot load pngs taller than 2048 pixels!");
	png_bytep* rowPointers = sRowPointers;
	U8* pBase = (U8*)getBits();
	for (U32 i = 0; i < height; i++)
		rowPointers[i] = pBase + (i * rowBytes);
	
	// And actually read the image!
	png_read_image(png_ptr, rowPointers);
	
	// We're outta here, destroy the png structs, and release the lock
	//  as quickly as possible...
	//png_read_end(png_ptr, end_info);
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	
	sg_pStream = NULL;
	
	// Ok, the image is read in, now we need to finish up the initialization,
	//  which means: setting up the detailing members, init'ing the palette
	//  key, etc...
	//
	// actually, all of that was handled by allocateBitmap, so we're outta here
	//
	FrameAllocator::setWaterMark(prevWaterMark);
	
	//
	//-Mat if all palleted images are to be converted, set mForce16bit
	if( color_type == PNG_COLOR_TYPE_PALETTE ) {
		sgForcePalletedPNGsTo16Bit = dAtob( Con::getVariable("$pref::iPhone::ForcePalletedPNGsTo16Bit") );
		if( sgForcePalletedPNGsTo16Bit ) {
			mForce16Bit = true;
		}
	}
	return true;
}
*/