//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
// Portions Copyright (c) 2014 James S Urquhart
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
#include "platformEmscripten/platformEmscripten.h"
#include "platformEmscripten/EmscriptenFont.h"
#include "string/unicode.h"

//------------------------------------------------------------------------------
// New Unicode capable font class.
PlatformFont *createPlatformFont(const char *name, U32 size, U32 charset /* = TGE_ANSI_CHARSET */)
{
    return NULL;
}

//------------------------------------------------------------------------------

void PlatformFont::enumeratePlatformFonts( Vector<StringTableEntry>& fonts )
{
}

//------------------------------------------------------------------------------

EmscriptenFont::EmscriptenFont()
{
}

//------------------------------------------------------------------------------

EmscriptenFont::~EmscriptenFont()
{
}

//------------------------------------------------------------------------------

bool EmscriptenFont::create( const char* name, U32 size, U32 charset )
{
    return false;
}

//------------------------------------------------------------------------------

bool EmscriptenFont::isValidChar( const UTF8* str ) const
{
    // since only low order characters are invalid, and since those characters
    // are single codeunits in UTF8, we can safely cast here.
    return isValidChar((UTF16)*str);
}

//------------------------------------------------------------------------------

bool EmscriptenFont::isValidChar( const UTF16 character) const
{
    // We cut out the ASCII control chars here. Only printable characters are valid.
    // 0x20 == 32 == space
    if( character < 0x20 )
        return false;
    
    return true;
}

//------------------------------------------------------------------------------

PlatformFont::CharInfo& EmscriptenFont::getCharInfo(const UTF8 *str) const
{
    return getCharInfo( oneUTF32toUTF16(oneUTF8toUTF32(str,NULL)) );
}

//------------------------------------------------------------------------------

PlatformFont::CharInfo& EmscriptenFont::getCharInfo(const UTF16 character) const
{
    // Declare and clear out the CharInfo that will be returned.
    static PlatformFont::CharInfo characterInfo;
    dMemset(&characterInfo, 0, sizeof(characterInfo));

    // Return character information.
    return characterInfo;
}
