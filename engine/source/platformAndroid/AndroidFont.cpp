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

#include "platform/platform.h"
#include "platformAndroid/platformAndroid.h"
#include "platformAndroid/AndroidFont.h"
#include "string/unicode.h"
#include <sstream>

//------------------------------------------------------------------------------
// New Unicode capable font class.
PlatformFont *createPlatformFont(const char *name, U32 size, U32 charset /* = TGE_ANSI_CHARSET */)
{
    PlatformFont *retFont = new AndroidFont;

    if(retFont->create(name, size, charset))
        return retFont;

    delete retFont;
    return NULL;
}

//------------------------------------------------------------------------------

void PlatformFont::enumeratePlatformFonts( Vector<StringTableEntry>& fonts )
{
}

//------------------------------------------------------------------------------

AndroidFont::AndroidFont()
{
	fontFileBuffer = NULL;
	fontFileBufferSize = 0;
	fontFaceCreated = false;
	int error = FT_Init_FreeType( &library );
	if ( error )
	{
		Con::errorf("Failed to initialize the freetype2 library");
	}
}

//------------------------------------------------------------------------------

AndroidFont::~AndroidFont()
{
	FT_Done_Face( face );
	if (fontFileBuffer != NULL)
	{
		delete[] fontFileBuffer;
		fontFileBufferSize = 0;
	}
}

//------------------------------------------------------------------------------

bool AndroidFont::create( const char* name, U32 size, U32 charset )
{
    // Sanity!
    AssertFatal( name != NULL, "Cannot create a NULL font name." );

    // Sanity!
    if ( !name )
    {
        Con::errorf("Could not handle font name of '%s'.", name );
        return false;
    }
    
	char fontPath[255];
	activity.getFontPath(name, fontPath);

    int error = FT_New_Face( library, fontPath, 0, &face );

    if ( error == FT_Err_Unknown_File_Format )
    {
    	fontFaceCreated = false;
    	Con::errorf("freetype2: Font was found but format is unsupported");
    }
    else if ( error )
    {
    	fontFaceCreated = false;
    	Con::errorf("freetype2: Font file was not found.");
    }
    else
    {
    	fontFaceCreated = true;
    }
    
    if (fontFaceCreated == true)
    {
    	error = FT_Set_Pixel_Sizes(face, 0, size);
    	mBaseline = (face->size->metrics.ascender + 32) >> 6;
    	mHeight = ((face->size->metrics.ascender + -face->size->metrics.descender) + 32) >> 6;
    }
    else
    {
    	mHeight = 0;
    	mBaseline = 0;
    }

    return true;
}

//------------------------------------------------------------------------------

bool AndroidFont::isValidChar( const UTF8* str ) const
{
    // since only low order characters are invalid, and since those characters
    // are single codeunits in UTF8, we can safely cast here.
    return isValidChar((UTF16)*str);
}

//------------------------------------------------------------------------------

bool AndroidFont::isValidChar( const UTF16 character) const
{
    // We cut out the ASCII control chars here. Only printable characters are valid.
    // 0x20 == 32 == space
    if( character < 0x20 )
        return false;
    
    return true;
}

//------------------------------------------------------------------------------

PlatformFont::CharInfo& AndroidFont::getCharInfo(const UTF8 *str) const
{
    return getCharInfo( oneUTF32toUTF16(oneUTF8toUTF32(str,NULL)) );
}

//------------------------------------------------------------------------------

PlatformFont::CharInfo& AndroidFont::getCharInfo(const UTF16 character) const
{
    // Declare and clear out the CharInfo that will be returned.
    static PlatformFont::CharInfo characterInfo;
    dMemset(&characterInfo, 0, sizeof(characterInfo));
    
    // prep values for GFont::addBitmap()
    characterInfo.bitmapIndex = 0;
    characterInfo.xOffset = 0;
    characterInfo.yOffset = 0;
    FT_GlyphSlot slot = face->glyph;

    int error = FT_Load_Char( face, character, FT_LOAD_RENDER );
    
    if ( error ) {
    	return characterInfo;
    }
    
    FT_Glyph_Metrics metrics = face->glyph->metrics;
    
    // Set character metrics,
    characterInfo.xOrigin = slot->bitmap_left;
    characterInfo.yOrigin = slot->bitmap_top;
    characterInfo.width = (metrics.width / 64);
    characterInfo.height = (metrics.height / 64);
    characterInfo.xIncrement = slot->advance.x / 64;
    
   // characterInfo.yOrigin = characterInfo.height - characterInfo.yOrigin;

    // Finish if character is undrawable.
    if ( characterInfo.width == 0 && characterInfo.height == 0 )
        return characterInfo;
    
    // Clamp character minimum width.
    if ( characterInfo.width == 0 )
    	return characterInfo;
    
    if ( characterInfo.height == 0 )
    	return characterInfo;
    
    // Allocate a bitmap surface.
    const U32 bitmapSize = characterInfo.width * characterInfo.height;
    characterInfo.bitmapData = new U8[bitmapSize];
    dMemset(characterInfo.bitmapData, 0x00, bitmapSize);
    
    //copy glyph
    if (slot->bitmap.buffer != NULL)
    {
    	for (int i = 0; i < slot->bitmap.width; i++)
		{
			for (int j = 0; j < slot->bitmap.rows; j++)
			{
				characterInfo.bitmapData[i + (j*slot->bitmap.width)] = slot->bitmap.buffer[i + (j*slot->bitmap.width)];
			}
		}
    }

    // Return character information.
    return characterInfo;
}
