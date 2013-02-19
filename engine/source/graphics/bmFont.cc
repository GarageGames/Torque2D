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
#include "graphics/bmFont.h"

#include "debug/profiler.h"
#include "platform/threads/mutex.h"
#include "console/console.h"
#include "io/stream.h"
#include "graphics/gBitmap.h"
#include "io/fileStream.h"
#include "string/findMatch.h"
#include "graphics/TextureManager.h"
#include "graphics/gFont.h"
#include "memory/safeDelete.h"
#include "memory/frameAllocator.h"
#include "string/unicode.h"
#include "zlib.h"
#include "ctype.h"  // Needed for isupper and tolower
#include "string/stringUnit.h"

static PlatformFont* createSafePlatformFont(const char *name, U32 size, U32 charset = TGE_ANSI_CHARSET)
{
    PlatformFont *platFont = createPlatformFont(name, size, charset);
    
    if (platFont == NULL)
    {
        Con::errorf("Loading platform font failed, trying font fallbacks...");
        // Couldn't load the requested font.  This probably will be common
        // since many unix boxes don't have arial or lucida console installed.
        // Attempt to map the font name into a font we're pretty sure exist
        // Lucida Console is a common code & console font on windows, and
        // Monaco is the recommended code & console font on mac.
        
        // this is the name of the final fallback font.
        const char* fallback = "Helvetica";
        
        if(dStricmp(name, fallback) == 0)
        {
            Con::errorf("Font fallback utterly failed.");
            return NULL;
        }
        else if (dStricmp(name, "arial") == 0)
            fallback = "Helvetica";
        else if (dStricmp(name, "lucida console") == 0)
            fallback = "Monaco";
        else if (dStricmp(name, "monaco") == 0)
            fallback = "Courier";
        
        platFont = createSafePlatformFont(fallback, size, charset);
    }
    
    return platFont;
}


/// Used for repacking in bmFont::importStrip.
struct GlyphMap
{
   U32 charId;
   GBitmap *bitmap;
};

static S32 QSORT_CALLBACK GlyphMapCompare(const void *a, const void *b)
{
   S32 ha = ((GlyphMap *) a)->bitmap->getHeight();
   S32 hb = ((GlyphMap *) b)->bitmap->getHeight();

   return hb - ha;
}


const U32 bmFont::csm_fileVersion = 3;


Resource<bmFont> bmFont::create(const StringTableEntry path )
{
    char buf[512];
    Platform::makeFullPathName( path, buf, 512 );
    
    Resource<bmFont> ret = ResourceManager->load(buf);
    if(bool(ret))
    {
        ret->mGFTFile = StringTable->insert(buf);
        return ret;
    }
    return NULL;
}

//-------------------------------------------------------------------------

bmFont::bmFont()
{
   VECTOR_SET_ASSOCIATION(mCharInfoList);
   VECTOR_SET_ASSOCIATION(mTextureSheets);

   for (U32 i = 0; i < (sizeof(mRemapTable) / sizeof(S32)); i++)
      mRemapTable[i] = -1;

   mCurX = mCurY = mCurSheet = -1;

   mPlatformFont = NULL;
   mGFTFile = NULL;
   mFaceName = NULL;
   mSize = 0;
   mCharSet = 0;
   mNeedSave = false;
   
   mMutex = Mutex::createMutex();
}

bmFont::~bmFont()
{
   
   // Need to stop this for now!
   mNeedSave = false; 
   if(mNeedSave)
   {
      FileStream stream;
      if(ResourceManager->openFileForWrite(stream, mGFTFile)) 
      {
         write(stream);
         stream.close();
      }
   }
   
   S32 i;

   for(i = 0;i < mCharInfoList.size();i++)
   {
       SAFE_DELETE_ARRAY(mCharInfoList[i].bitmapData);
   }

   //Luma:	decrement reference of the texture handles too
   for(i=0;i<mTextureSheets.size();i++)
   {
       mTextureSheets[i] = 0;
   }
   
   SAFE_DELETE(mPlatformFont);
   
   Mutex::destroyMutex(mMutex);
}

void bmFont::dumpInfo()
{
   // Number and extent of mapped characters?
   U32 mapCount = 0, mapBegin=0xFFFF, mapEnd=0;
   for(U32 i=0; i<0x10000; i++)
   {
      if(mRemapTable[i] != -1)
      {
         mapCount++;
         if(i<mapBegin) mapBegin = i;
         if(i>mapEnd)   mapEnd   = i;
      }
   }


   // Let's write out all the info we can on this font.
   Con::printf("   '%s' %dpt", mFaceName, mSize);
   Con::printf("      - %d texture sheets, %d mapped characters.", mTextureSheets.size(), mapCount);

   if(mapCount)
      Con::printf("      - Codepoints range from 0x%x to 0x%x.", mapBegin, mapEnd);
   else
      Con::printf("      - No mapped codepoints.", mapBegin, mapEnd);
   Con::printf("      - Platform font is %s.", (mPlatformFont ? "present" : "not present") );
}

//////////////////////////////////////////////////////////////////////////

bool bmFont::loadCharInfo(const UTF16 ch)
{
    if(mRemapTable[ch] != -1)
        return true;    // Not really an error

    if(mPlatformFont && mPlatformFont->isValidChar(ch))
    {
        Mutex::lockMutex(mMutex); // the CharInfo returned by mPlatformFont is static data, must protect from changes.
        PlatformFont::CharInfo &ci = mPlatformFont->getCharInfo(ch);
        if(ci.bitmapData)
            addBitmap(ci);

        mCharInfoList.push_back(ci);
        mRemapTable[ch] = mCharInfoList.size() - 1;
        Mutex::unlockMutex(mMutex);
        return true;
    }

    return false;
}

void bmFont::addBitmap(PlatformFont::CharInfo &charInfo)
{
}

void bmFont::addSheet()
{
}

//-----------------------------------------------------------------------------

const PlatformFont::CharInfo &bmFont::getCharInfo(const UTF16 in_charIndex)
{
    PROFILE_SCOPE(bmFont_getCharInfo);

    AssertFatal(in_charIndex, "bmFont::getCharInfo - can't get info for char 0!");

//    if(mRemapTable[in_charIndex] == -1)
//        loadCharInfo(in_charIndex);
//
    AssertFatal(mRemapTable[in_charIndex] != -1, "No remap info for this character");
    
    return mCharInfoList[mRemapTable[in_charIndex]];
}

const PlatformFont::CharInfo &bmFont::getDefaultCharInfo()
{
   static PlatformFont::CharInfo c;
   // c is initialized by the CharInfo default constructor.
   return c;
}

//-----------------------------------------------------------------------------

U32 bmFont::getStrWidth(const UTF8* in_pString)
{
   AssertFatal(in_pString != NULL, "bmFont::getStrWidth: String is NULL, width is undefined");
   // If we ain't running debug...
   if (in_pString == NULL || *in_pString == '\0')
      return 0;

   return getStrNWidth(in_pString, dStrlen(in_pString));
}

U32 bmFont::getStrWidthPrecise(const UTF8* in_pString)
{
   AssertFatal(in_pString != NULL, "bmFont::getStrWidth: String is NULL, height is undefined");
   // If we ain't running debug...
   if (in_pString == NULL)
      return 0;

   return getStrNWidthPrecise(in_pString, dStrlen(in_pString));
}

//-----------------------------------------------------------------------------
U32 bmFont::getStrNWidth(const UTF8 *str, U32 n)
{
   // UTF8 conversion is expensive. Avoid converting in a tight loop.
   FrameTemp<UTF16> str16(n + 1);
   convertUTF8toUTF16(str, str16, n + 1);
   return getStrNWidth(str16, dStrlen(str16));
}

U32 bmFont::getStrNWidth(const UTF16 *str, U32 n)
{
   AssertFatal(str != NULL, "bmFont::getStrNWidth: String is NULL");

   if (str == NULL || str[0] == '\0' || n == 0)   
      return 0;
      
   U32 totWidth = 0;
   UTF16 curChar;
   U32 charCount;
   
   for(charCount = 0; charCount < n; charCount++)
   {
      curChar = str[charCount];
      if(curChar == '\0')
         break;

      if(isValidChar(curChar))
      {
         const PlatformFont::CharInfo& rChar = getCharInfo(curChar);
         totWidth += rChar.xIncrement +  rChar.xOrigin;
      }
      else if (curChar == dT('\t'))
      {
         const PlatformFont::CharInfo& rChar = getCharInfo(dT(' '));
         totWidth += rChar.xIncrement * TabWidthInSpaces;
      }
   }

   return(totWidth);
}

U32 bmFont::getStrNWidthPrecise(const UTF8 *str, U32 n)
{
   FrameTemp<UTF16> str16(n + 1);
   convertUTF8toUTF16(str, str16, n + 1);
   return getStrNWidthPrecise(str16, dStrlen(str16));
}

U32 bmFont::getStrNWidthPrecise(const UTF16 *str, U32 n)
{
   AssertFatal(str != NULL, "bmFont::getStrNWidth: String is NULL");

   if (str == NULL || str[0] == '\0' || n == 0)   
      return(0);
      
   U32 totWidth = 0;
   UTF16 curChar;
   U32 charCount = 0;
   
   for(charCount = 0; charCount < n; charCount++)
   {
      curChar = str[charCount];
      if(curChar == '\0')
         break;
         
      if(isValidChar(curChar))
      {
         const PlatformFont::CharInfo& rChar = getCharInfo(curChar);
         totWidth += rChar.xIncrement;
      }
      else if (curChar == dT('\t'))
      {
         const PlatformFont::CharInfo& rChar = getCharInfo(dT(' '));
         totWidth += rChar.xIncrement * TabWidthInSpaces;
      }
   }

   UTF16 endChar = str[getMin(charCount,n-1)];

   if (isValidChar(endChar))
   {
      const PlatformFont::CharInfo& rChar = getCharInfo(endChar);
      if (rChar.width != rChar.xIncrement)
         totWidth += (rChar.width - rChar.xIncrement);
   }

   return(totWidth);
}

U32 bmFont::getBreakPos(const UTF16 *str16, U32 slen, U32 width, bool breakOnWhitespace)
{
   // Some early out cases.
   if(slen==0)
      return 0;

   U32 ret = 0;
   U32 lastws = 0;
   UTF16 c;
   U32 charCount = 0;

   for( charCount=0; charCount < slen; charCount++)
   {
      c = str16[charCount];
      if(c == '\0')
         break;
         
      if(c == dT('\t'))
         c = dT(' ');
      
      if(!isValidChar(c))
      {
         ret++;
         continue;
      }
      
      if(c == dT(' '))
         lastws = ret+1;

      const PlatformFont::CharInfo& rChar = getCharInfo(c);
      if(rChar.width > width || rChar.xIncrement > width)
      {
         if(lastws && breakOnWhitespace)
            return lastws;
         return ret;
      }

      width -= rChar.xIncrement;
      
      ret++;
   }
   return ret;
}

void bmFont::wrapString(const UTF8 *txt, U32 lineWidth, Vector<U32> &startLineOffset, Vector<U32> &lineLen)
{
   // TODO: Is this error still true?
   //Con::errorf("bmFont::wrapString(): Not yet converted to be UTF-8 safe");

   startLineOffset.clear();
   lineLen.clear();

   if (!txt || !txt[0] || lineWidth < getCharWidth('W')) //make sure the line width is greater then a single character
      return;

   U32 len = dStrlen(txt);

   U32 startLine; 

   for (U32 i = 0; i < len;)
   {
      startLine = i;
      startLineOffset.push_back(startLine);

      // loop until the string is too large
      bool needsNewLine = false;
      U32 lineStrWidth = 0;
      for (; i < len; i++)
      {
         if( txt[ i ] == '\n' )
         {
            needsNewLine = true;
            break;
         }
         else if(isValidChar(txt[i]))
         {
            lineStrWidth += getCharInfo(txt[i]).xIncrement;
            if( lineStrWidth > lineWidth )
            {
               needsNewLine = true;
               break;      
            }
         }
      }

      if (!needsNewLine)
      {
         // we are done!
         lineLen.push_back(i - startLine);
         return;
      }

      S32 j; 

      // Did we hit a hardwrap (newline character) in the string.
      bool hardwrap = ( txt[i] == '\n' );
      
      if ( hardwrap )
      {
         j = i;
      }
      // determine where to put the newline
      // we need to backtrack until we find a space character 
      // we don't do this for hardwrap(s)
      else
      {        
         for (j = i - 1; j >= startLine; j--)
         {
            if ( dIsspace(txt[j]) || txt[i] == '\n' )
               break;
         }

         if (j < startLine)
         {
            // the line consists of a single word!              
            // So, just break up the word

            j = i - 1;
         }
      }

      lineLen.push_back(j - startLine);
      i = j;

      // Now we need to increment through any space characters at the
      // beginning of the next line.
      // We don't skip spaces after a hardwrap because they were obviously intended.
      for (i++; i < len; i++)
      {         
         if ( txt[i] == '\n' )
            continue;

         if ( dIsspace( txt[i] ) && !hardwrap )
            continue;

         break;
      }
   }
}

//-----------------------------------------------------------------------------

bool bmFont::read(Stream& io_rStream)
{
    for (U32 i = 0; i < (sizeof(mRemapTable) / sizeof(S32)); i++)
        mRemapTable[i] = -1;
    
    char buffer[256];
    U32 bmWidth = 0;
    U32 bmHeight = 0;
    U32 numSheets = 0;
    U32 charIndex = 0;
    StringTableEntry fileName = StringTable->insert("");
    
    U32 numBytes = io_rStream.getStreamSize() - io_rStream.getPosition();
    while((io_rStream.getStatus() != Stream::EOS) && numBytes > 0)
    {
        const char *Read, *Key, *Value;
        char Token[256];
        char *buffer = Con::getReturnBuffer(256);
        io_rStream.readLine((U8 *)buffer, 256);
        
        char temp[128];
        U32 tokenCount = StringUnit::getUnitCount(buffer, "\"");
        
        if (tokenCount > 1)
        {
            sprintf(Token, "%s", StringUnit::getUnit(buffer, 1, "\""));
            sprintf(temp, "tok1");
            sprintf(buffer, "%s", (char*)StringUnit::setUnit(buffer, 1, temp, "\""));
        }
        
        U32 wordCount = StringUnit::getUnitCount(buffer, " \t\n");
        
        Read = StringUnit::getUnit(buffer, 0, " \t\n");
        
        if( dStrcmp( Read, "common" ) == 0 )
        {
            U32 currentWordCount = 1;
            //this holds common data
            while( currentWordCount < wordCount )
            {
                Read = StringUnit::getUnit(buffer, currentWordCount, " \t\n");
                char temp[256];
                char Key[256];
                char Value[256];
                
                sprintf(temp, "%s", Read);
                sprintf(Key, "%s", StringUnit::getUnit(temp, 0, "="));
                sprintf(Value, "%s", StringUnit::getUnit(temp, 1, "="));
                
                if (dStrcmp( Value, "\"tok1\"") == 0) {
                    sprintf(Value, "%s", Token);
                }
                
                if( dStrcmp( Key, "lineHeight" ) == 0 )
                    mBaseline = U16(dAtoi(Value));
                else if( dStrcmp( Key, "base" ) == 0 )
                    mHeight = U16(dAtoi(Value));
                else if( dStrcmp( Key, "scaleW" ) == 0 )
                    bmWidth = U16(dAtoi(Value));
                else if( dStrcmp( Key, "scaleH" ) == 0 )
                    bmHeight = U16(dAtoi(Value));
                else if( dStrcmp( Key, "pages" ) == 0 )
                    numSheets = U16(dAtoi(Value));
                currentWordCount++;
            }
        }
        else if( dStrcmp( Read, "page" ) == 0 )
        {
            //this is data for a page
            U16 CharID = 0;
            U32 currentWordCount = 1;
            //this holds common data
            char lineLeft[256];
            sprintf ( lineLeft, "%s", StringUnit::getUnit(buffer, 1, " \t\n"));
            
            while( currentWordCount < wordCount )
            {
                Read = StringUnit::getUnit(buffer, currentWordCount, " \t\n");
                char temp[256];
                char Key[256];
                char Value[256];
                
                sprintf(temp, "%s", Read);
                sprintf(Key, "%s", StringUnit::getUnit(temp, 0, "="));
                sprintf(Value, "%s", StringUnit::getUnit(temp, 1, "="));
                
                if (dStrcmp( Value, "\"tok1\"") == 0) {
                    sprintf(Value, "%s", Token);
                }
                
                //assign the correct value
                if (dStrcmp( Key, "file" ) == 0 )
                {
                    fileName = StringTable->insert(Value);
                }
                currentWordCount++;
            }
        }

        else if( dStrcmp( Read, "char" ) == 0 )
        {
            PlatformFont::CharInfo ci; //  = &mCharInfoList[charIndex];
            ci.bitmapData = NULL;
            //this is data for a character set
            U16 CharID = 0;
            U32 currentWordCount = 1;
            U32 size = 0;
            //this holds common data
            while( currentWordCount < wordCount )
            {
                Read = StringUnit::getUnit(buffer, currentWordCount, " \t\n");
                char temp[256];
                char Key[256];
                char Value[256];
                
                sprintf(temp, "%s", Read);
                sprintf(Key, "%s", StringUnit::getUnit(temp, 0, "="));
                sprintf(Value, "%s", StringUnit::getUnit(temp, 1, "="));
                
                if (dStrcmp( Value, "\"tok1\"") == 0) {
                    sprintf(Value, "%s", Token);
                }
                
                //assign the correct value
                if( dStrcmp( Key, "id" ) == 0 )
                    ci.bitmapIndex = U32(dAtoi(Value));
                else if( dStrcmp( Key, "x" ) == 0 )
                    ci.xOffset = U32(dAtoi(Value));
                else if( dStrcmp( Key, "y" ) == 0 )
                    ci.yOffset = U32(dAtoi(Value));
                else if( dStrcmp( Key, "width" ) == 0 )
                    ci.width = U32(dAtoi(Value));
                else if( dStrcmp( Key, "height" ) == 0 )
                    ci.height = U32(dAtoi(Value));
                else if( dStrcmp( Key, "xoffset" ) == 0 )
                    ci.xOrigin = S32(dAtoi(Value));
                else if( dStrcmp( Key, "yoffset" ) == 0 )
                    ci.yOrigin = -S32(dAtoi(Value));
                else if( dStrcmp( Key, "xadvance" ) == 0 )
                    ci.xIncrement = S32(dAtoi(Value));
                currentWordCount++;
            }
            mCharInfoList.push_back(ci);
        }
    }

   for(U32 i = 0; i < numSheets; i++)
   {
        char buf[1024];
        Platform::makeFullPathName(Con::getVariable("$GUI::fontCacheDirectory"), buf, sizeof(buf));
        
//        path.setPath(fullPath);
//        
//        mTextureSheet[i] = handle;
//       
//       if (mTextureSheet.getPointer() == NULL)
//          return false;
//       
//        mTextureSheet.getPointer()->smEventSignal.notify(this, &bmFont::_handleTextureLoaded);

        for (U32 i = 0; i < mCharInfoList.size(); i++)
        {
            PlatformFont::CharInfo ci  = mCharInfoList[i];
            mRemapTable[ ci.bitmapIndex ] = i;
        }
   }
   return (io_rStream.getStatus() == Stream::EOS);
}


//void bmFont::_handleTextureLoaded(GFXTexNotifyCode code)
//{
//    
//}

bool bmFont::write(Stream& stream)
{
}

void bmFont::exportStrip(const char *fileName, U32 padding, U32 kerning)
{
}


void bmFont::importStrip(const char *fileName, U32 padding, U32 kerning)
{
}


ResourceInstance* constructBMFont(Stream& stream)
{
    bmFont *ret = new bmFont;
    
    if(!ret->read(stream))
    {
        SAFE_DELETE(ret);
        ret = NULL;
    }
    
    return ret;
}



