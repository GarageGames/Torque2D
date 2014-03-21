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

#include "platformEmscripten/platformEmscripten.h"
#include "platform/platform.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "console/console.h"

char *dStrdup_r(const char *src, const char *file, dsize_t line)
{
   char *buffer = (char *) dMalloc_r(dStrlen(src) + 1, file, line);
   dStrcpy(buffer, src);
   return buffer;
}

char *dStrnew(const char *src)
{
   char *buffer = new char[dStrlen(src) + 1];
   dStrcpy(buffer, src);
   return buffer;
}

char* dStrcat(char *dst, const char *src)
{
   return strcat(dst,src);
}   

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

int dStrcmp( const UTF16 *str1, const UTF16 *str2)
{
    int ret;
    const UTF16 *a, *b;
    a = str1;
    b = str2;
    
    while(*a && *b && (ret = *a - *b) == 0)
    {
        a++, b++;
    }
    
    if ( *a == 0 && *b != 0 )
        return -1;
    
    if ( *b == 0 && *a != 0 )
        return 1;
    
    return ret;
}

int dStricmp(const char *str1, const char *str2)
{
   char c1, c2;
   while (1)
   {
      c1 = tolower(*str1++);
      c2 = tolower(*str2++);
      if (c1 < c2) return -1;
      if (c1 > c2) return 1;
      if (c1 == 0) return 0;
   }
}  

int dStrncmp(const char *str1, const char *str2, dsize_t len)
{
   return strncmp(str1, str2, len);   
}  
 
int dStrnicmp(const char *str1, const char *str2, dsize_t len)
{
   S32 i;
   char c1, c2;
   for (i=0; i<len; i++)
   {
      c1 = tolower(*str1++);
      c2 = tolower(*str2++);
      if (c1 < c2) return -1;
      if (c1 > c2) return 1;
      if (!c1) return 0;
     }
   return 0;
}   

char* dStrcpy(char *dst, const char *src)
{
   AssertFatal(dst && src, "bad strings passed to dStrcpy()");
   return strcpy(dst,src);
}   

char* dStrncpy(char *dst, const char *src, dsize_t len)
{
   return strncpy(dst,src,len);
}   

dsize_t dStrlen(const char *str)
{
   return str ? strlen(str) : 0;
}   

char* dStrupr(char *str)
{
   char* saveStr = str;
   while (*str)
   {
      *str = toupper(*str);
      str++;
   }
   return saveStr;
}   

char* dStrlwr(char *str)
{
   char* saveStr = str;
   while (*str)
   {
      *str = tolower(*str);
      str++;
   }
   return saveStr;
}   

char* dStrchr(char *str, int c)
{
   return strchr(str,c);
}   

const char* dStrchr(const char *str, int c)
{
   return strchr(str,c);
}   

const char* dStrrchr(const char *str, int c)
{
   return strrchr(str,c);
}   

char* dStrrchr(char *str, int c)
{
   return strrchr(str,c);
}   

dsize_t dStrspn(const char *str, const char *set)
{
   return(strspn(str, set));
}

dsize_t dStrcspn(const char *str, const char *set)
{
   return strcspn(str, set);
}   


char* dStrstr(char *str1, char *str2)
{
   return strstr(str1,str2);
}

char* dStrstr(const char *str1, const char *str2)
{
   return strstr(str1,str2);
}   

char* dStrtok(char *str, const char *sep)
{
   return strtok(str, sep);
}

int dAtoi(const char *str)
{
   if(!str)
      return 0;
   return atoi(str);
}  

float dAtof(const char *str)
{
   if(!str)
      return 0;
   return atof(str);   
}   

bool dAtob(const char *str)
{
   return !dStricmp(str, "true") || !dStricmp(str, "1") || (0!=dAtoi(str));
}   

bool dIsalnum(const char c)
{
   return isalnum(c);
}

bool dIsalpha(const char c)
{
   return(isalpha(c));
}

bool dIsspace(const char c)
{
   return(isspace(c));
}

bool dIsdigit(const char c)
{
   return(isdigit(c));
}

void dPrintf(const char *format, ...)
{
   va_list args;
   va_start(args, format);
   vprintf(format, args);
   va_end(args);
}

int dVprintf(const char *format, va_list arglist)
{
   S32 len = vprintf(format, arglist);

   return (len);
}   

int dSprintf(char *buffer, dsize_t bufferSize, const char *format, ...)
{
   va_list args;
   va_start(args, format);
   S32 len = vsnprintf(buffer, bufferSize, format, args);
   va_end(args);

   return (len);
}

int dVsprintf(char *buffer, dsize_t bufferSize, const char *format, va_list arglist)
{
   S32 len = vsnprintf(buffer, bufferSize, format, arglist);

   return (len);
}

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

//-Mat hack
int dItoa(int n, char s[])
{
  int i, sign;
   
   if ((sign = n) < 0)  /* record sign */
      n = -n;          /* make n positivNSStringe */
   i = 0;
   do {       /* generate digits in reverse order */
      s[i++] = n % 10 + '0';   /* get next digit */
   } while ((n /= 10) > 0);     /* delete it */
   if (sign < 0)
      s[i++] = '-';
   s[i] = '\0';
    
    // Reverse string.
   dStrrev(s);
    
  // Return length.
  return dStrlen(s);
} 

int dSscanf(const char *buffer, const char *format, ...)
{
   va_list args;
   va_start(args, format);
   int ret = vsscanf(buffer, format, args);
   va_end(args);
   return ret;
}

int dFflushStdout()
{
   return fflush(stdout);
}

int dFflushStderr()
{
   return fflush(stderr);
}

void dQsort(void *base, U32 nelem, U32 width, int (QSORT_CALLBACK *fcmp)(const void *, const void *))
{
   qsort(base, nelem, width, fcmp);
}   

StringTableEntry Platform::createUUID( void )
{
  Con::errorf("createUUID not supported on Emscripten");
  return StringTable->insert("");
}
