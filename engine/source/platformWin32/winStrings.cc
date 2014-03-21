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

#include "platformWin32/platformWin32.h"

#ifndef _STRINGTABLE_H_
#include "string/stringTable.h"
#endif

#ifdef HAS_VSSCANF
#  undef HAS_VSSCANF
#endif

#if defined(TORQUE_COMPILER_CODEWARRIOR)
#  define HAS_VSSCANF
#  define __vsscanf vsscanf
#endif

#if defined(TORQUE_COMPILER_GCC)
#  include <stdlib.h>
#  include <ctype.h>
#endif

#include <stdarg.h>


char *dStrdup_r(const char *src, const char *fileName, dsize_t lineNumber)
{
   char *buffer = (char *) dMalloc_r(dStrlen(src) + 1, fileName, lineNumber);
   dStrcpy(buffer, src);
   return buffer;
}

char* dStrcat(char *dst, const char *src)
{
   return strcat(dst,src);
}

/*UTF8* dStrcat(UTF8 *dst, const UTF8 *src)
{
   return (UTF8*)strcat((char*)dst,(char*)src);
} */  

char* dStrncat(char *dst, const char *src, dsize_t len)
{
   return strncat(dst,src,len);
}

// concatenates a list of src's onto the end of dst
// the list of src's MUST be terminated by a NULL parameter
// dStrcatl(dst, sizeof(dst), src1, src2, NULL);
char* dStrcatl(char *dst, dsize_t dstSize, ...)
{
   const char* src;
   char *p = dst;

   AssertFatal(dstSize > 0, "dStrcatl: destination size is set zero");
   dstSize--;  // leave room for string termination

   // find end of dst
   while (dstSize && *p)
   {
       p++;
      dstSize--;
   }

   va_list args;
   va_start(args, dstSize);

   // concatenate each src to end of dst
   while ( (src = va_arg(args, const char*)) != NULL )
   {
      while( dstSize && *src )
      {
         *p++ = *src++;
         dstSize--;
      }
   }

   va_end(args);

   // make sure the string is terminated
   *p = 0;

   return dst;
}


// copy a list of src's into dst
// the list of src's MUST be terminated by a NULL parameter
// dStrccpyl(dst, sizeof(dst), src1, src2, NULL);
char* dStrcpyl(char *dst, dsize_t dstSize, ...)
{
   const char* src;
   char *p = dst;

   AssertFatal(dstSize > 0, "dStrcpyl: destination size is set zero");
   dstSize--;  // leave room for string termination

   va_list args;
   va_start(args, dstSize);

   // concatenate each src to end of dst
   while ( (src = va_arg(args, const char*)) != NULL )
      while( dstSize && *src )
      {
         *p++ = *src++;
         dstSize--;
      }

   va_end(args);

   // make sure the string is terminated
   *p = 0;

   return dst;
}


int dStrcmp(const char *str1, const char *str2)
{
   return strcmp(str1, str2);
}

int dStrcmp(const UTF16 *str1, const UTF16 *str2)
{
    return wcscmp(str1, str2);
}
 
int dStricmp(const char *str1, const char *str2)
{
   return stricmp(str1, str2);
}

int dStrncmp(const char *str1, const char *str2, dsize_t len)
{
   return strncmp(str1, str2, len);
}

int dStrnicmp(const char *str1, const char *str2, dsize_t len)
{
   return strnicmp(str1, str2, len);
}

char* dStrcpy(UTF8 *dst, const UTF8 *src)
{
   return strcpy((char*)dst,(char*)src);
}

char* dStrncpy(char *dst, const char *src, dsize_t len)
{
   return strncpy(dst,src,len);
}

/*char* dStrncpy(UTF8 *dst, const UTF8 *src, dsize_t len)
{
   return strncpy((char*)dst,(char*)src,len);
} */  

U32 dStrlen(const char *str)
{
   return (U32)strlen(str);
}   

/*dsize_t dStrlen(const UTF8 *str)
{
    // [tom, 7/12/2005] http://mail.nl.linux.org/linux-utf8/2000-06/msg00002.html
    int c = 0;
    for(; str; str = getNextUTF8Char(str))
        c++;
    
    return c;
}*/

U32 dStrlen(const UTF16 *str)
{
    return (U32)wcslen(str);
}

char* dStrupr(char *str)
{
#if defined(TORQUE_COMPILER_CODEWARRIOR)
   // metrowerks strupr is broken
   _strupr(str);
   return str;
#else
   return strupr(str);
#endif
}


char* dStrlwr(char *str)
{
   return strlwr(str);
}


char* dStrchr(char *str, S32 c)
{
   return strchr(str,c);
}


const char* dStrchr(const char *str, S32 c)
{
   return strchr(str,c);
}


const char* dStrrchr(const char *str, S32 c)
{
   return strrchr(str,c);
}

char* dStrrchr(char *str, S32 c)
{
   return strrchr(str,c);
}

U32 dStrspn(const char *str, const char *set)
{
   return (U32)strspn(str, set);
}

U32 dStrcspn(const char *str, const char *set)
{
   return (U32)strcspn(str, set);
}


char* dStrstr(char *str1, char *str2)
{
   return strstr(str1,str2);
}

char* dStrstr(const char *str1, const char *str2)
{
   return strstr((char *)str1,str2);
}


char* dStrtok(char *str, const char *sep)
{
   return strtok(str, sep);
}


S32 dAtoi(const char *str)
{
   return atoi(str);
}

F32 dAtof(const char *str)
{
   // Warning: metrowerks crashes when strange strings are passed in '0x [enter]' for example!
   return (F32)atof(str);
}

bool dAtob(const char *str)
{
   return !dStricmp(str, "true") || dAtof(str);
}


bool dIsalnum(const char c)
{
   return isalnum(c)!=0;
}

bool dIsalpha(const char c)
{
   return isalpha(c)!=0;
}

bool dIsspace(const char c)
{
   return isspace(c)!=0;
}

bool dIsdigit(const char c)
{
   return isdigit(c)!=0;
}

void dPrintf(const char *format, ...)
{
   va_list args;
   va_start(args, format);
   vprintf(format, args);
}

S32 dVprintf(const char *format, va_list arglist)
{
   S32 len = vprintf(format, arglist);
   return (len);
}

S32 dSprintf(char *buffer, dsize_t bufferSize, const char *format, ...)
{
   va_list args;
   va_start(args, format);

   S32 len = vsnprintf(buffer, bufferSize, format, args);

   AssertFatal( (U32)len < bufferSize, "dSprintf wrote to more memory than the specified buffer size - Stack Corruption Possible" ); //Added

   return (len);
}


S32 dVsprintf(char *buffer, dsize_t bufferSize, const char *format, va_list arglist)
{
   S32 len = vsnprintf(buffer, bufferSize, format, arglist);
    
   AssertFatal( (U32)len < bufferSize, "dVsprintf wrote to more memory than the specified buffer size - Stack Corruption Possible" );

   return (len);
}

//%PUAP% -Mat -----------------------------------------------------------------------------------------------

int dStrrev(char* str)
{
    int l=dStrlen(str)-1; //get the string length
    for(int x=0;x < l;x++,l--)
    {
        str[x]^=str[l];  //triple XOR Trick
        str[l]^=str[x];  //for not using a temp
        str[x]^=str[l];
    }
    return l;
}

int dItoa(int n, char s[])
{
    int i, sign;
    
    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    dStrrev(s);
    return dStrlen(s);
} 

//%PUAP%-----------------------------------------------------------------------------------------------------


S32 dSscanf(const char *buffer, const char *format, ...)
{
   va_list args;
#if defined(HAS_VSSCANF)
   va_start(args, format);
   return __vsscanf(buffer, format, args);
#else
   va_start(args, format);

   // Boy is this lame.  We have to scan through the format string, and find out how many
   //  arguments there are.  We'll store them off as void*, and pass them to the sscanf
   //  function through specialized calls.  We're going to have to put a cap on the number of args that
   //  can be passed, 8 for the moment.  Sigh.
   void* sVarArgs[20];
   U32 numArgs = 0;

   for (const char* search = format; *search != '\0'; search++) {
      if (search[0] == '%' && search[1] != '%')
         numArgs++;
   }
   AssertFatal(numArgs <= 20, "Error, too many arguments to lame implementation of dSscanf.  Fix implmentation");

   // Ok, we have the number of arguments...
   for (U32 i = 0; i < numArgs; i++)
      sVarArgs[i] = va_arg(args, void*);
   va_end(args);

   switch (numArgs) {
     case 0: return 0;
     case 1:  return sscanf(buffer, format, sVarArgs[0]);
     case 2:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1]);
     case 3:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2]);
     case 4:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3]);
     case 5:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4]);
     case 6:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5]);
     case 7:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6]);
     case 8:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7]);
     case 9:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8]);
     case 10: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9]);
     case 11: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10]);
     case 12: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11]);
     case 13: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12]);
     case 14: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13]);
     case 15: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14]);
     case 16: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15]);
     case 17: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15], sVarArgs[16]);
     case 18: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15], sVarArgs[16], sVarArgs[17]);
     case 19: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15], sVarArgs[16], sVarArgs[17], sVarArgs[18]);
     case 20: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15], sVarArgs[16], sVarArgs[17], sVarArgs[18], sVarArgs[19]);
   }
   return 0;
#endif
}

S32 dFflushStdout()
{
   return fflush(stdout);
}

S32 dFflushStderr()
{
   return fflush(stderr);
}

void dQsort(void *base, U32 nelem, U32 width, S32 (QSORT_CALLBACK *fcmp)(const void *, const void *))
{
    if ( nelem == 0 )
        return;

   qsort(base, nelem, width, fcmp);
}   

UTF8 * convertUTF16toUTF8(const UTF16 *string, UTF8 *buffer, U32 bufsize)
{
    int nRet;
    if((nRet = WideCharToMultiByte(CP_UTF8, 0, string, dStrlen(string), (LPSTR)buffer, bufsize, NULL, NULL)) != 0)
    {
        buffer[nRet] = 0;
        return buffer;
    }
    else
        return NULL;
}

UTF16 * convertUTF8toUTF16(const UTF8 *string, UTF16 *buffer, U32 bufsize)
{
    int nRet;
    if((nRet = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)string, dStrlen((const char *)string), buffer, bufsize)) != 0)
    {
        buffer[nRet] = 0;
        return buffer;
    }
    else
        return NULL;
}


//-----------------------------------------------------------------------------

StringTableEntry Platform::createUUID( void )
{
    // Create UUID.
    UUID id; 
    UuidCreate( &id );

    // Format UUID.
    char uuidBuffer[128];
    dSprintf( uuidBuffer, sizeof(uuidBuffer),
        "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        id.Data1,
        id.Data2,
        id.Data3,
        id.Data4[0],
        id.Data4[1],
        id.Data4[2],
        id.Data4[3],
        id.Data4[4],
        id.Data4[5],
        id.Data4[6],
        id.Data4[7]);

    return StringTable->insert(uuidBuffer);
}
