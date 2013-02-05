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
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "io/resource/resourceManager.h"
#include "io/fileStream.h"
#include "console/compiler.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platform/platformInput.h"

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif


// Buffer for expanding script filenames.
static char pathBuffer[1024];

static U32 execDepth = 0;

#ifdef TORQUE_ALLOW_JOURNALING
   static U32 journalDepth = 1;
#endif

//----------------------------------------------------------------

ConsoleFunctionGroupBegin(StringFunctions, "General string manipulation functions.");

ConsoleFunction(strcmp, S32, 3, 3, "( string1 , string2 ) Use the strcmp function to do a lexicographic case sensitive string comparison between string1 and string2.\n"
                                                                "@param string1 String to be compared to string2.\n"
                                                                "@param string2 String to be compared to string1.\n"
                                                                "@return Returns a numeric value: <-1 (string1 is less than string2, including case), 0 (string1 is equal to string2, including case), 1 (string1 is greater than string2, including case)>.\n"
                                                                "@sa see stricmp, strstr")
{
   TORQUE_UNUSED( argc );
   return dStrcmp(argv[1], argv[2]);
}

ConsoleFunction(stricmp, S32, 3, 3, "( string1 , string2 ) Use the stricmp function to do a lexicographic case in-sensitive string comparison between string1 and string2.\n"
                                                                "@param string1 String to be compared to string2.\n"
                                                                "@param string2 String to be compared to string1.\n"
                                                                "@return Returns a numeric value: <-1 (string1 is less than string2, ignoring case), 0 (string1 is equal to string2, ignoring case), 1 (string1 is greater than string2, ignoring case)>.\n"
                                                                "@sa see strcmp, strstr")
{
   TORQUE_UNUSED( argc );
   return dStricmp(argv[1], argv[2]);
}

ConsoleFunction(strlen, S32, 2, 2, "( string ) Use the strlen function to determine how many characters there are in string.\n"
                                                                "@param string The string to count characters for.\n"
                                                                "@return Returns the number of characters in string, or 0 if string is invalid or a NULL string")
{
   TORQUE_UNUSED( argc );
   return dStrlen(argv[1]);
}

ConsoleFunction(strstr, S32 , 3, 3, "( sourceString , searchString ) Use the strstr function to locate the first instance of searchString in sourceString.\n"
                                                                "@param sourceString The string in which to search for searchString.\n"
                                                                "@param searchString The string for which to search for in sourceString.\n"
                                                                "@return Returns a numeric character index representing the position in sourceString at which searchString was found, or -1 to indicate that no instance of searchString was found.\n"
                                                                "@sa strpos")
{
   TORQUE_UNUSED( argc );
   // returns the start of the sub string argv[2] in argv[1]
   // or -1 if not found.

   const char *retpos = dStrstr(argv[1], argv[2]);
   if(!retpos)
      return -1;
   return retpos - argv[1];
}

ConsoleFunction(strpos, S32, 3, 4, "( sourceString , searchString [ , offset ] ) Use the strPos function to locate the first instance of searchString in sourceString, starting at character 0, or at an optional offset.\n"
                                                                "@param sourceString The string in which to search for searchString.\n"
                                                                "@param searchString The string for which to search for in sourceString.\n"
                                                                "@param offset An optional non-negative integer value representing the character offset within sourceString at which to begin the search.\n"
                                                                "@return Returns a numeric character index representing the postion in sourceString at which searchString was found, or -1 to indicate that no instance of searchString was found.\n"
                                                                "@sa strstr")
{
   S32 start = 0;
   if(argc == 4)
      start = dAtoi(argv[3]);
   U32 sublen = dStrlen(argv[2]);
   U32 strlen = dStrlen(argv[1]);
   if(start < 0)
      return -1;
   if(sublen + start > strlen)
      return -1;
   for(; start + sublen <= strlen; start++)
      if(!dStrncmp(argv[1] + start, argv[2], sublen))
         return start;
   return -1;
}

ConsoleFunction(ltrim, const char *,2,2,"( sourceString ) Use the ltrim function to strip the leading white space from sourceString.\n"
                                                                "White space is any character in this set: spaces, TABs, and NULL strings.\n"
                                                                "@param sourceString The string to be trimmed.\n"
                                                                "@return Returns sourceString with all the leading white spaces removed.\n"
                                                                "@sa stripChars, stripMLControlChars, stripTrailingSpaces, rtrim, trim")
{
   TORQUE_UNUSED( argc );
   const char *ret = argv[1];
   while(*ret == ' ' || *ret == '\n' || *ret == '\t')
      ret++;
   return ret;
}

ConsoleFunction(rtrim, const char *,2,2, "( sourceString ) Use the rtrim function to strip the trailing white space from sourceString.\n"
                                                                "White space is any character in this set: spaces, TABs, and NULL strings.\n"
                                                                "@param sourceString The string to be trimmed.\n"
                                                                "@return Returns sourceString with all the trailing white spaces removed.\n"
                                                                "@sa stripChars, stripMLControlChars, stripTrailingSpaces, ltrim, trim")
{
   TORQUE_UNUSED( argc );
   S32 firstWhitespace = 0;
   S32 pos = 0;
   const char *str = argv[1];
   while(str[pos])
   {
      if(str[pos] != ' ' && str[pos] != '\n' && str[pos] != '\t')
         firstWhitespace = pos + 1;
      pos++;
   }
   char *ret = Con::getReturnBuffer(firstWhitespace + 1);
   dStrncpy(ret, argv[1], firstWhitespace);
   ret[firstWhitespace] = 0;
   return ret;
}

ConsoleFunction(trim, const char *,2,2,"( sourceString ) Use the trim function to strip the leading and trailing white space from sourceString.\n"
                                                                "White space is any character in this set: spaces, TABs, and NULL strings.\n"
                                                                "@param sourceString The string to be trimmed.\n"
                                                                "@return Returns sourceString with all the leading and trailing white spaces removed.\n"
                                                                "@sa stripChars, stripMLControlChars, stripTrailingSpaces, ltrim, rtrim")
{
   TORQUE_UNUSED( argc );
   const char *ptr = argv[1];
   while(*ptr == ' ' || *ptr == '\n' || *ptr == '\t')
      ptr++;
   S32 firstWhitespace = 0;
   S32 pos = 0;
   const char *str = ptr;
   while(str[pos])
   {
      if(str[pos] != ' ' && str[pos] != '\n' && str[pos] != '\t')
         firstWhitespace = pos + 1;
      pos++;
   }
   char *ret = Con::getReturnBuffer(firstWhitespace + 1);
   dStrncpy(ret, ptr, firstWhitespace);
   ret[firstWhitespace] = 0;
   return ret;
}

ConsoleFunction(stripChars, const char*, 3, 3, "( sourceString , chars ) Use the stripChars function to remove chars from sourceString.\n"
                                                                "@param sourceString The string to be modified.\n"
                                                                "@param chars The character or characters to search for and remove.\n"
                                                                "@return Returns a copy of sourceString, from which all instances of chars have been removed. This may be the original sourceString, if chars was not found.\n"
                                                                "@sa stripMLControlChars, stripTrailingSpaces")
{
   TORQUE_UNUSED( argc );
   char* ret = Con::getReturnBuffer( dStrlen( argv[1] ) + 1 );
   dStrcpy( ret, argv[1] );
   U32 pos = dStrcspn( ret, argv[2] );
   while ( pos < dStrlen( ret ) )
   {
      dStrcpy( ret + pos, ret + pos + 1 );
      pos = dStrcspn( ret, argv[2] );
   }
   return( ret );
}

ConsoleFunction(stripColorCodes, const char*, 2,2,  "(stringtoStrip) - "
                "remove TorqueML color codes from the string.\n"
                "@param stringtoString The string from which to remove TorqueML color codes\n"
                "@return A string consisting of the original string minus color codes")
{
   char* ret = Con::getReturnBuffer( dStrlen( argv[1] ) + 1 );
   dStrcpy(ret, argv[1]);
   Con::stripColorChars(ret);
   return ret;
}

ConsoleFunction(strlwr,const char *,2,2, "( sourceString ) Use the strlwr function to convert all alpha characters in sourceString to lower-case equivalents.\n"
                                                                "@param sourceString The string to be modified.\n"
                                                                "@return Returns a copy of sourceString in which all upper-case characters have been converted to lower-case letters.\n"
                                                                "@sa strupr")
{
   TORQUE_UNUSED( argc );
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1);
   dStrcpy(ret, argv[1]);
   return dStrlwr(ret);
}

ConsoleFunction(strupr,const char *,2,2, "( sourceString ) Use the strupr function to convert all alpha characters in sourceString to upper-case equivalents.\n"
                                                                "@param sourceString The string to be modified.\n"
                                                                "@return Returns a copy of sourceString in which all lower-case characters have been converted to upper-case letters.\n"
                                                                "@sa strlwr")
{
   TORQUE_UNUSED( argc );
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1);
   dStrcpy(ret, argv[1]);
   return dStrupr(ret);
}

ConsoleFunction(strchr,const char *,3,3,"( sourceString , char ) Use the strchr function to extract a sub-string of sourceString, where the sub-string is equal to the first occurence of char in sourceString followed by the remainder of sourceString.\n"
                                                                "@param sourceString The string from which to extract a sub-string.\n"
                                                                "@param char The character to search for in sourceString.\n"
                                                                "@return Returns a string composed of first instance of char in sourceString, and all of the characters after it. If char is not found, a NULL string is returned.\n"
                                                                "@sa getSubStr")
{
   TORQUE_UNUSED( argc );
   const char *ret = dStrchr(argv[1], argv[2][0]);
   return ret ? ret : "";
}

ConsoleFunction(strrchr,const char *,3,3,"( sourceString , char ) strrchr searches the sourceString for the last occurance of the giuven char\n"
                "@param sourceString The string to search\n"
                "@return Either a string consisting of the given string from the last occurance of the given char on or an empty string if not found")
{
   TORQUE_UNUSED( argc );
   const char *ret = dStrrchr(argv[1], argv[2][0]);
   return ret ? ret : "";
}

ConsoleFunction(strreplace, const char *, 4, 4, "( sourceString , from , to ) Use the strreplace function to replace every instance of from in sourceString with to.\n"
                                                                "This function is case-sensitive and only does exact matching\n"
                                                                "@param sourceString The string to do replacement operations on.\n"
                                                                "@param from The old value to be replaced.\n"
                                                                "@param to The new value to replace old values with.\n"
                                                                "@return Returns a new version of sourceString in which every instance of the value in from was replaced with the value in to.")
{
   TORQUE_UNUSED( argc );
   S32 fromLen = dStrlen(argv[2]);
   if(!fromLen)
      return argv[1];

   S32 toLen = dStrlen(argv[3]);
   S32 count = 0;
   const char *scan = argv[1];
   while(scan)
   {
      scan = dStrstr(scan, argv[2]);
      if(scan)
      {
         scan += fromLen;
         count++;
      }
   }
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1 + (toLen - fromLen) * count);
   U32 scanp = 0;
   U32 dstp = 0;
   for(;;)
   {
      const char *scan = dStrstr(argv[1] + scanp, argv[2]);
      if(!scan)
      {
         dStrcpy(ret + dstp, argv[1] + scanp);
         return ret;
      }
      U32 len = scan - (argv[1] + scanp);
      dStrncpy(ret + dstp, argv[1] + scanp, len);
      dstp += len;
      dStrcpy(ret + dstp, argv[3]);
      dstp += toLen;
      scanp += len + fromLen;
   }
   return ret;
}

ConsoleFunction(getSubStr, const char *, 4, 4, "( sourceString , start , count ) Use the getSubStr function to get a sub-string of sourceString, starting at character index start and ending at character index start + count, or the end-of-string, which ever comes first.\n"
                                                                "If start + count is greater than the length of sourceString, the extraction will return a string shorter than count.\n"
                                                                "@param sourceString The string from which to extract a sub-string.\n"
                                                                "@param start The character index at which the extraction starts.\n"
                                                                "@param count The length of the sub-string to extract.\n"
                                                                "@return Returns a string made up of the character at start in sourceString and ending at the end of the original sourceString, or start + count, whichever comes first.\n"
                                                                "@sa strchr")
{
   TORQUE_UNUSED( argc );
   // Returns the substring of argv[1], starting at argv[2], and continuing
   //  to either the end of the string, or argv[3] characters, whichever
   //  comes first.
   //
   S32 startPos   = dAtoi(argv[2]);
   S32 desiredLen = dAtoi(argv[3]);
   if (startPos < 0 || desiredLen < 0) {
      Con::errorf(ConsoleLogEntry::Script, "getSubStr(...): error, starting position and desired length must be >= 0: (%d, %d)", startPos, desiredLen);

      return "";
   }

   S32 baseLen = dStrlen(argv[1]);
   if (baseLen < startPos)
      return "";

   U32 actualLen = desiredLen;
   if (startPos + desiredLen > baseLen)
      actualLen = baseLen - startPos;

   char *ret = Con::getReturnBuffer(actualLen + 1);
   dStrncpy(ret, argv[1] + startPos, actualLen);
   ret[actualLen] = '\0';

   return ret;
}

// Used?
ConsoleFunction( stripTrailingSpaces, const char*, 2, 2, "( string )Removes all spaces after the final \n"
                "@param string from which to remove trailing spaces\n"
                "@return the source string minus trailing spaces")
{
   TORQUE_UNUSED( argc );
   S32 temp = S32(dStrlen( argv[1] ));
   if ( temp )
   {
      while ( ( argv[1][temp - 1] == ' ' || argv[1][temp - 1] == '_' ) && temp >= 1 )
         temp--;

      if ( temp )
      {
         char* returnString = Con::getReturnBuffer( temp + 1 );
         dStrncpy( returnString, argv[1], U32(temp) );
         returnString[temp] = '\0';
         return( returnString );			
      }
   }

   return( "" );	
}

ConsoleFunctionGroupEnd(StringFunctions);

//--------------------------------------

#include "string/stringUnit.h"

//--------------------------------------
ConsoleFunctionGroupBegin( FieldManipulators, "Functions to manipulate data returned in the form of \"x y z\".");

//-----------------------------------------------------------------------------

ConsoleFunction(getUnit, const char*, 4, 4, "( sourceString ,index, separator )" )
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), argv[3] ) );
}

//-----------------------------------------------------------------------------

ConsoleFunction(getUnitCount, S32, 3, 3, "( sourceString, separator )")
{
   TORQUE_UNUSED( argc );
   return StringUnit::getUnitCount(argv[1], argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleFunction(setUnit, const char *, 5, 5, "( sourceString ,index, replace, separator)")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], argv[4]) );
}

//-----------------------------------------------------------------------------

ConsoleFunction(getWord, const char *, 3, 3, "( sourceString ,index ) Use the getWord function to get the word at index in sourceString.\n"
                                                                "@param sourceString A string containing one or more words.\n"
                                                                "@return Returns word at index in sourceString, or null string if no word exists at that index.\n"
                                                                "@sa getWords, setWord")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), " \t\n") );
}

ConsoleFunction(getWords, const char *, 3, 4, "( sourceString ,index [,endindex]) Use the getWords function to retrieve a set of words from a sourceString.\n"
                                                                "@param sourceString A string containing one or more words.\n"
                                                                "@param index The index of the first word to retrieve.\n"
                                                                "@param endindex The index of the final word to retrieve.\n"
                                                                "@return Returns all words (separated by current delimiter) from sourceString, starting at index and ending at endIndex or end of string, whichever comes first. If no endIndex is specified, all remaining words are returned.\n"
                                                                "@sa getWord, setWord")
{
   U32 endIndex;
   if(argc==3)
      endIndex = 1000000;
   else
      endIndex = dAtoi(argv[3]);
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, " \t\n") );
}

ConsoleFunction(setWord, const char *, 4, 4, "( sourceString ,index,replace) Use the setWord function to replace an existing word with a new word(s), or to add word(s) to a string..\n"
                                                                "@param sourceString A string containing one or more words.\n"
                                                                "@param index The index of the word to remove.\n"
                                                                "@param replace The new word(s) to replace the word at index with.\n"
                                                                "@return There are multiple return cases:\n- In the first case, a simple one-to-one replacement, the word at index in sourceString will be replaced with the value in replace, and the new string will be returned.\n- In the first case, a multi-to-one replacement, the word at index in sourceString will be replaced with the value in replace, which can be two or more words, and the new string will be returned.\n- In the third and final case, new records, empty or filled, can be appended to the end of sourceString. If index is beyond the end of the sourceString, that is, the index is greater than the total count of words in sourceString, the requisite number of empty (null-string) words will be appended to the end of sourceString and the value in replace will be appended to the end of this new string. This entire resultant string will be returned.\n"
                                                                "@sa getWord, getWords, removeWord")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], " \t\n") );
}

ConsoleFunction(removeWord, const char *, 3, 3, "( sourceString ,index) Use the removeWord function to remove a single indexed word from a sourceString.\n"
                                                                "@param sourceString A string containing one or more words.\n"
                                                                "@param index The index of the word to remove.\n"
                                                                "@return Returns sourceString minus the removed word. If the index is greater than the number of words in sourceString, the original string is returned.\n"
                                                                "@sa setWord")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), " \t\n") );
}

ConsoleFunction(getWordCount, S32, 2, 2, "( sourceString ) Use the getWordCount function to get the number of words in sourceString.\n"
                                                                "@param sourceString A string containing one or more words.\n"
                                                                "@return Returns number of words in sourceString or 0 if no words are present")
{
   TORQUE_UNUSED( argc );
   return StringUnit::getUnitCount(argv[1], " \t\n");
}

//--------------------------------------
ConsoleFunction(getField, const char *, 3, 3, "( sourceString , index ) Use the getField function to get the field at index in sourceString.\n"
                                                                "@param sourceString A string containing one or more fields.\n"
                                                                "@return Returns field at index in sourceString, or null string if no field exists at that index.\n"
                                                                "@sa getFields, setField")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), "\t\n") );
}

ConsoleFunction(getFields, const char *, 3, 4, "( sourceString , index [ , endindex ] ) Use the getFields function to retrieve a set of fields from a sourceString.\n"
                                                                "@param sourceString A string containing one or more fields.\n"
                                                                "@param index The index of the first field to retrieve.\n"
                                                                "@param endindex The index of the final field to retrieve.\n"
                                                                "@return Returns all fields (separated by current delimiter) from sourceString, starting at index and ending at endIndex or end of string, whichever comes first. If no endIndex is specified, all remaining fields are returned.\n"
                                                                "@sa getField, setField")
{
   U32 endIndex;
   if(argc==3)
      endIndex = 1000000;
   else
      endIndex = dAtoi(argv[3]);
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, "\t\n") );
}

ConsoleFunction(setField, const char *, 4, 4, "( sourceString , index , replace ) Use the setField function to replace an existing field with a new field(s), or to add field(s) to a string..\n"
                                                                "@param sourceString A string containing one or more fields.\n"
                                                                "@param index The index of the field to remove.\n"
                                                                "@param replace The new field(s) to replace the field at index with.\n"
                                                                "@return There are multiple return cases:\n- In the first case, a simple one-to-one replacement, the field at index in sourceString will be replaced with the value in replace, and the new string will be returned.\n- In the first case, a multi-to-one replacement, the field at index in sourceString will be replaced with the value in replace, which can be two or more fields, and the new string will be returned.\n- In the thrid and final case, new records, empty or filled, can be appended to the end of sourceString. If index is beyond the end of the sourceString, that is, the index is greater than the total count of fields in sourceString, the requisite number of empty (null-string) fields will be appended to the end of sourceString and the value in replace will be appended to the end of this new string. This entire resultant string will be returned.\n"
                                                                "@sa getField, getFields, removeField")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], "\t\n") );
}

ConsoleFunction(removeField, const char *, 3, 3, "( sourceString , index ) Use the removeField function to remove a single indexed field from a sourceString.\n"
                                                                "@param sourceString A string containing one or more fields.\n"
                                                                "@param index The index of the field to remove.\n"
                                                                "@return Returns sourceString minus the removed field. If the index is greater than the number of fields in sourceString, the original string is returned.\n"
                                                                "@sa setField")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), "\t\n") );
}

ConsoleFunction(getFieldCount, S32, 2, 2, "( sourceString ) Use the getFieldCount function to get the number of fields in sourceString.\n"
                                                                "@param sourceString A string containing one or more fields.\n"
                                                                "@return Returns number of fields in sourceString or 0 if no fields are present")
{
   TORQUE_UNUSED( argc );
   return StringUnit::getUnitCount(argv[1], "\t\n");
}

//--------------------------------------
ConsoleFunction(getRecord, const char *, 3, 3, "( sourceString , index ) Use the getRecord function to get the record at index in sourceString.\n"
                                                                "@param sourceString A string containing one or more records.\n"
                                                                "@return Returns record at index in sourceString, or NULL string if no record exists at that index.\n"
                                                                "@sa getRecords, setRecord")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), "\n") );
}

ConsoleFunction(getRecords, const char *, 3, 4, "( sourceString , index [ , endindex ] ) Use the getRecords function to retrieve a set of records from a sourceString.\n"
                                                                "@param sourceString A string containing one or more records.\n"
                                                                "@param index The index of the first record to retrieve.\n"
                                                                "@param endindex The index of the final record to retrieve.\n"
                                                                "@return Returns all records (separated by current delimiter) from sourceString, starting at index and ending at endIndex or end of string, whichever comes first. If no endIndex is specified, all remaining records are returned.\n"
                                                                "@sa getRecord, setRecord")
{
   U32 endIndex;
   if(argc==3)
      endIndex = 1000000;
   else
      endIndex = dAtoi(argv[3]);
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, "\n") );
}

ConsoleFunction(setRecord, const char *, 4, 4, "( sourceString , index , replace ) Use the setRecord function to replace an existing record with a new record(s), or to add record(s) to a string..\n"
                                                                "@param sourceString A string containing one or more records.\n"
                                                                "@param index The index of the record to remove.\n"
                                                                "@param replace The new record(s) to replace the record at index with.\n"
                                                                "@return There are multiple return cases:\n- In the first case, a simple one-to-one replacement, the record at index in sourceString will be replaced with the value in replace, and the new string will be returned.\n- In the first case, a multi-to-one replacement, the record at index in sourceString will be replaced with the value in replace, which can be two or more records, and the new string will be returned.\n- In the thrid and final case, new records, empty or filled, can be appended to the end of sourceString. If index is beyond the end of the sourceString, that is, the index is greater than the total count of records in sourceString, the requisite number of empty (null-string) records will be appended to the end of sourceString and the value in replace will be appended to the end of this new string. This entire resultant string will be returned.\n"
                                                                "@sa getRecord, getRecords, removeRecord")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], "\n") );
}

ConsoleFunction(removeRecord, const char *, 3, 3, "( sourceString , index ) Use the removeRecord function to remove a single indexed record from a sourceString.\n"
                                                                "@param sourceString A string containing one or more records.\n"
                                                                "@param index The index of the record to remove.\n"
                                                                "@return Returns sourceString minus the removed record. If the index is greater than the number of records in sourceString, the original string is returned.\n"
                                                                "@sa setRecord")
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), "\n") );
}

ConsoleFunction(getRecordCount, S32, 2, 2, "( sourceString ) Use the getRecordCount function to get the number of records in sourceString.\n"
                                                                "@param sourceString A string containing one or more records.\n"
                                                                "@return Returns number of records in sourceString or 0 if no records are present")
{
   TORQUE_UNUSED( argc );
   return StringUnit::getUnitCount(argv[1], "\n");
}
//--------------------------------------
ConsoleFunction(firstWord, const char *, 2, 2, "( sourceString ) Use the firstWord function to retrieve the first word found in sourceString.\n"
                                                                "@param sourceString A string containing one or more words.\n"
                                                                "@return Returns the first word found in sourceString, or a NULL string, if no words are found.\n"
                                                                "@sa restWords")
{
   TORQUE_UNUSED( argc );
   const char *word = dStrchr(argv[1], ' ');
   U32 len;
   if(word == NULL)
      len = dStrlen(argv[1]);
   else
      len = word - argv[1];
   char *ret = Con::getReturnBuffer(len + 1);
   dStrncpy(ret, argv[1], len);
   ret[len - 1] = 0;
   return ret;
}

ConsoleFunction(restWords, const char *, 2, 2, "( sourceString ) Use the restWords function to retrieve all words after the first word in sourceString.\n"
                                                                "@param sourceString A string containing one or more words.\n"
                                                                "@return Returns a string containing all the words after the first word found in sourceString, or a NULL string if no words remain after the first word (or if no words at all remain).\n"
                                                                "@sa firstWord")
{
   TORQUE_UNUSED( argc );
   const char *word = dStrchr(argv[1], ' ');
   if(word == NULL)
      return "";
   char *ret = Con::getReturnBuffer(dStrlen(word + 1) + 1);
   dStrcpy(ret, word + 1);
   return ret;
}

static bool isInSet(char c, const char *set)
{
   if (set)
      while (*set)
         if (c == *set++)
            return true;

   return false;
}

ConsoleFunction(NextToken,const char *,4,4,"( tokenList , tokenVar , delimeter ) Use the nextToken function to get the first token found in tokenList, where tokens are separated by the character(s) specified in delimeter. The token itself is stored in a variable whose name is specified in tokenVar. This function provides complex power in a simple package. Please read the notes below, they are very important.\n"
                                                                "This function is scope-smart. That is, when we specify the name of the variable to store a token in by passing a value in tokenVar, we do not include either a local symbol (%), or a global symbol ($). We just pass in an un-adorned name, let's say \"George\". Then, depending on where this function is called, \"George\" will become a local (%George), or a global ($George) variable. If this function is called within a function or method definition, \"George\" will be local (%George). If this function is called from the file-scope (executed as part of a file and not within the scope of a function or method), \"George\" will become a global ($George). There is one additional special case. If you attempt to use this from the console command line, the token will vaporize and no variable will be created\n"
                                                                "@param tokenList The string containing token(s).\n"
                                                                "@param tokenVar The 'name' of the variable to store the token in.\n"
                                                                "@param delimeter The character(s) to use as a delimeter. A delimeter may be a single character, or a sequence of characters.\n"
                                                                "@return Returns a copy of tokenList, less the first token and the first delimiter. If there are no more tokens, a NULL string is returned.")
{
   TORQUE_UNUSED( argc );

   char *str = (char *) argv[1];
   const char *token = argv[2];
   const char *delim = argv[3];

   if (str)
   {
      // skip over any characters that are a member of delim
      // no need for special '\0' check since it can never be in delim
      while (isInSet(*str, delim))
         str++;

      // skip over any characters that are NOT a member of delim
      const char *tmp = str;

      while (*str && !isInSet(*str, delim))
         str++;

      // terminate the token
      if (*str)
         *str++ = 0;

      // set local variable if inside a function
      if (gEvalState.stack.size() && 
         gEvalState.stack.last()->scopeName)
         Con::setLocalVariable(token,tmp);
      else
         Con::setVariable(token,tmp);

      // advance str past the 'delim space'
      while (isInSet(*str, delim))
         str++;
   }

   return str;
}

ConsoleFunctionGroupEnd( FieldManipulators )
//----------------------------------------------------------------

ConsoleFunctionGroupBegin( TaggedStrings, "Functions dealing with tagging/detagging strings.");

ConsoleFunction(detag, const char *, 2, 2, "( tagID ) Use the detag function to convert a tag to a string. This can only be used in the proper context, i.e. to parse values passed to a client command or to a server command. See 'Remote Procedure Call Samples' below.\n"
                                                                "@param tagID A numeric tag ID corresponding to a previously tagged string.\n"
                                                                "@return Returns the string associated with the tag ID.\n"
                                                                "@sa commandToClient, commandToServer")
{
   TORQUE_UNUSED( argc );
   if(argv[1][0] == StringTagPrefixByte)
   {
      const char *word = dStrchr(argv[1], ' ');
      if(word == NULL)
         return "";
      char *ret = Con::getReturnBuffer(dStrlen(word + 1) + 1);
      dStrcpy(ret, word + 1);
      return ret;
   }
   else
      return argv[1];
}

ConsoleFunction(getTag, const char *, 2, 2, "( taggedString ) Use the getTag function to retrieve the tag ID associated with a previously tagged string.\n"
                                                                "@param taggedString A previously tagged string.\n"
                                                                "@return Returns the tag ID of the string. If the string was not previously tagged, it gets tagged and the new tag ID is returned")
{
   TORQUE_UNUSED( argc );
   if(argv[1][0] == StringTagPrefixByte)
   {
      const char * space = dStrchr(argv[1], ' ');

      U32 len;
      if(space)
         len = space - argv[1];
      else
         len = dStrlen(argv[1]) + 1;

      char * ret = Con::getReturnBuffer(len);
      dStrncpy(ret, argv[1] + 1, len - 1);
      ret[len - 1] = 0;

      return(ret);
   }
   else
      return(argv[1]);
}

ConsoleFunctionGroupEnd( TaggedStrings );

//----------------------------------------------------------------

ConsoleFunctionGroupBegin( Output, "Functions to output to the console." );

ConsoleFunction(echo, void, 2, 0, "( text [ , ... ] ) Use the echo function to print messages to the console.\n"
                                                                "@param text Any valid text string.\n"
                                                                "@param ... Any additional valid text string(s).\n"
                                                                "@return No return value.\n"
                                                                "@sa error, warn")
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, argv[i]);

   Con::printf("%s", ret);
   ret[0] = 0;
}

ConsoleFunction(echoSeparator, void, 0, 0, "() - Prints a separator to the console.\n" )
{
    Con::printSeparator();
}

ConsoleFunction(warn, void, 2, 0, "( text [ , ... ] ) Use the warn function to print warning messages to the console. These messages usually yellow or orange.\n"
                                                                "@param text Any valid text string.\n"
                                                                "@param ... Any additional valid text string(s).\n"
                                                                "@return No return value.\n"
                                                                "@sa warn, error")
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, argv[i]);

   Con::warnf(ConsoleLogEntry::General, "%s", ret);
   ret[0] = 0;
}

ConsoleFunction(error, void, 2, 0, "( text [ , ... ] ) Use the error function to print error messages to the console. These messages usually print in red.\n"
                                                                "@param text Any valid text string.\n"
                                                                "@param ... Any additional valid text string(s).\n"
                                                                "@return No return value.\n"
                                                                "@sa echo, warn")
{
   U32 len = 0;
   S32 i;
   for(i = 1; i < argc; i++)
      len += dStrlen(argv[i]);

   char *ret = Con::getReturnBuffer(len + 1);
   ret[0] = 0;
   for(i = 1; i < argc; i++)
      dStrcat(ret, argv[i]);

   Con::errorf(ConsoleLogEntry::General, "%s", ret);
   ret[0] = 0;
}

ConsoleFunction(expandEscape, const char *, 2, 2, "( text ) Use the collapseEscape function to replace all escape sequences ('xx') with an expanded version ('xx').\n"
                                                                "@param text A string, possibly containing escape sequences.\n"
                                                                "@return Returns a copy of text with all escape sequences expanded.\n"
                                                                "@sa collapseEscape")
{
   TORQUE_UNUSED( argc );
   char *ret = Con::getReturnBuffer(dStrlen(argv[1])*2 + 1);  // worst case situation
   expandEscape(ret, argv[1]);
   return ret;
}

ConsoleFunction(collapseEscape, const char *, 2, 2, "( text ) Use the collapseEscape function to replace all escape sequences ('xx') with a collapsed version ('xx').\n"
                                                                "@param text A string, possibly containing escape sequences.\n"
                                                                "@return Returns a copy of text with all escape sequences converted to an encoding.\n"
                                                                "@sa expandEscape")
{
   TORQUE_UNUSED( argc );
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1);  // worst case situation
   dStrcpy( ret, argv[1] );
   collapseEscape( ret );
   return ret;
}

ConsoleFunction(setLogMode, void, 2, 2, "( mode ) Use the setLogMode function to set the logging level based on bits that are set in the mode argument.\n"
                                                                "This is a general debug method and should be used in all but release cases and perhaps even then.\n"
                                                                "@param mode A bitmask enabling various types of logging. See 'Logging Modes' table below.\n"
                                                                "@return No return value.\n"
                                                                "@sa intputLog")
{
   TORQUE_UNUSED( argc );
   Con::setLogMode(dAtoi(argv[1]));
}

ConsoleFunction(setEchoFileLoads, void, 2, 2, "( enable ) Use the setEchoFileLoads function to enable/disable echoing of file loads (to console).\n"
                                                                "This does not completely disable message, but rather adds additional methods when echoing is set to true. File loads will always echo a compile statement if compiling is required, and an exec statement at all times\n"
                                                                "@param enable A boolean value. If this value is true, extra information will be dumped to the console when files are loaded.\n"
                                                                "@return No return value.")
{
   TORQUE_UNUSED( argc );
   ResourceManager->setFileNameEcho(dAtob(argv[1]));
}

ConsoleFunctionGroupEnd( Output );

//----------------------------------------------------------------

ConsoleFunction(quit, void, 1, 1, "() Use the quit function to stop the engine and quit to the command line.\n"
                                                                "@return No return value")
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   Platform::postQuitMessage(0);
}

ConsoleFunction(quitWithErrorMessage, void, 2, 2, "(msg string) quitWithErrorMessage(msg)"
                " - Quit, showing the provided error message. This is equivalent"
                " to an AssertISV.\n"
                "@param Error Message\n"
                "@return No return value")
{
   AssertISV(false, argv[1]);
}

//----------------------------------------------------------------

ConsoleFunction( gotoWebPage, void, 2, 2, "( address ) - Open a URL in the user's favorite web browser." )
{
   TORQUE_UNUSED( argc );
   char* protocolSep = dStrstr(argv[1],"://");

   if( protocolSep != NULL )
   {
      Platform::openWebBrowser(argv[1]);
      return;
   }

   // if we don't see a protocol seperator, then we know that some bullethead
   // sent us a bad url. We'll first check to see if a file inside the sandbox
   // with that name exists, then we'll just glom "http://" onto the front of 
   // the bogus url, and hope for the best.
   
   char urlBuf[2048];
   if(Platform::isFile(argv[1]) || Platform::isDirectory(argv[1]))
   {
      dSprintf(urlBuf, sizeof(urlBuf), "file://%s",argv[1]);
   }
   else
      dSprintf(urlBuf, sizeof(urlBuf), "http://%s",argv[1]);
   
   Platform::openWebBrowser(urlBuf);
   return;
}


//----------------------------------------------------------------

ConsoleFunctionGroupBegin(MetaScripting, "Functions that let you manipulate the scripting engine programmatically.");

ConsoleFunction(call, const char *, 2, 0, "( funcName [ , args ... ] ) Use the call function to dynamically build and call a function.\n"
                                                                "@param funcName A string containing the unadorned name of a function to be executed.\n"
                                                                "@param args .. - Any arguments that should be passed to the function.\n"
                                                                "@return Returns a string containing the results from the function that is built and called.\n"
                                                                "@sa eval")
{
   return Con::execute(argc - 1, argv + 1);
}

static StringTableEntry getDSOPath(const char *scriptPath)
{
   const char *slash = dStrrchr(scriptPath, '/');
   return StringTable->insertn(scriptPath, slash - scriptPath, true);
}

ConsoleFunction(getDSOPath, const char *, 2, 2, "(scriptFileName) Returns the DSO path of the given filename\n"
                "@param scriptFileName A string contains the file to search for\n"
                "@return Returns either the requested pathname as a string, or an empty string if not found.")
{
   Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]);  

   const char *filename = getDSOPath(pathBuffer);
   if(filename == NULL || *filename == 0)
      return "";

   return filename;
}

ConsoleFunction(compile, bool, 2, 2, "( fileName ) Use the compile function to pre-compile a script file without executing the contents.\n"
                                                                "@param fileName A path to the script to compile.\n"
                                                                "@return Returns 1 if the script compiled without errors and 0 if the file did not compile correctly or if the path is wrong. Also, ff the path is invalid, an error will print to the console.\n"
                                                                "@sa exec")
{
   TORQUE_UNUSED( argc );
   char nameBuffer[512];
   char* script = NULL;
   U32 scriptSize = 0;

   FileTime comModifyTime, scrModifyTime;

   Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]);

   // Figure out where to put DSOs
   StringTableEntry dsoPath = getDSOPath(pathBuffer);

   // If the script file extention is '.ed.cs' then compile it to a different compiled extention
   bool isEditorScript = false;
   const char *ext = dStrrchr( pathBuffer, '.' );
   if( ext && ( dStricmp( ext, ".cs" ) == 0 ) )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.cs" ) == 0 )
         isEditorScript = true;
   }
   else if( ext && ( dStricmp( ext, ".gui" ) == 0 ) )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.gui" ) == 0 )
         isEditorScript = true;
   }

   const char *filenameOnly = dStrrchr(pathBuffer, '/');
   if(filenameOnly)
      ++filenameOnly;
   else
      filenameOnly = pathBuffer;

   if( isEditorScript )
      dStrcpyl(nameBuffer, sizeof(nameBuffer), dsoPath, "/", filenameOnly, ".edso", NULL);
   else
      dStrcpyl(nameBuffer, sizeof(nameBuffer), dsoPath, "/", filenameOnly, ".dso", NULL);
   
   ResourceObject *rScr = ResourceManager->find(pathBuffer);
   ResourceObject *rCom = ResourceManager->find(nameBuffer);

   if(rCom)
      rCom->getFileTimes(NULL, &comModifyTime);
   if(rScr)
      rScr->getFileTimes(NULL, &scrModifyTime);

   Stream *s = ResourceManager->openStream(pathBuffer);
   if(s)
   {
      scriptSize = ResourceManager->getSize(pathBuffer);
      script = new char [scriptSize+1];
      s->read(scriptSize, script);
      ResourceManager->closeStream(s);
      script[scriptSize] = 0;
   }

   if (!scriptSize || !script)
   {
      delete [] script;
      Con::errorf(ConsoleLogEntry::Script, "compile: invalid script file %s.", pathBuffer);
      return false;
   }
   // compile this baddie.
// -Mat reducing console noise
#if defined(TORQUE_DEBUG)
   Con::printf("Compiling %s...", pathBuffer);
#endif
   CodeBlock *code = new CodeBlock();
   code->compile(nameBuffer, pathBuffer, script);
   delete code;
   code = NULL;

   delete[] script;
   return true;
}

ConsoleFunction(compilePath, const char*, 2, 2, "compilePath( path )")
{
    if ( !Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]) )
        return "-1 0";
    
    const char *compileArgs[2] = { "compile", NULL };
    
    S32 failedScripts = 0;
    S32 totalScripts = 0;
    ResourceObject *match = NULL;
    
    while ( (match = ResourceManager->findMatch( pathBuffer, &compileArgs[1], match )) )
    {
        if ( !ccompile( NULL, 1, compileArgs ) )
            failedScripts++;
        
        totalScripts++;
    }
    
    char* result = Con::getReturnBuffer(32);
    dSprintf( result, 32, "%d %d", failedScripts, totalScripts );
   return result;
}

static bool scriptExecutionEcho = false;
ConsoleFunction(setScriptExecEcho, void, 2, 2, "(echo?) - Whether to echo script file execution or not." )
{
    scriptExecutionEcho = dAtob(argv[1]);
}

ConsoleFunction(exec, bool, 2, 4, "( fileName [ , nocalls [ , journalScript ] ] ) Use the exec function to compile and execute a normal script, or a special journal script.\n"
                                                                "If $Pref::ignoreDSOs is set to true, the system will use .cs before a .dso file if both are found.\n"
                                                                "@param fileName A string containing a path to the script to be compiled and executed.\n"
                                                                "@param nocalls A boolean value. If this value is set to true, then all function calls encountered while executing the script file will be skipped and not called. This allows us to re-define function definitions found in a script file, without re-executing other worker scripts in the same file.\n"
                                                                "@param journalScript A boolean value. If this value is set tot true, and if a journal is being played, the engine will attempt to read this script from the journal stream. If no journal is playing, this field is ignored.\n"
                                                                "@return Returns true if the file compiled and executed w/o errors, false otherwise.\n"
                                                                "@sa compile")
{
   execDepth++;

#ifdef TORQUE_ALLOW_JOURNALING
   bool journal = false;

   if(journalDepth >= execDepth)
      journalDepth = execDepth + 1;
   else
      journal = true;
#endif //TORQUE_ALLOW_JOURNALING

   bool noCalls = false;
   bool ret = false;

   if(argc >= 3 && dAtoi(argv[2]))
      noCalls = true;

#ifdef TORQUE_ALLOW_JOURNALING
   if(argc >= 4 && dAtoi(argv[3]) && !journal)
   {
      journal = true;
      journalDepth = execDepth;
   }
#endif //TORQUE_ALLOW_JOURNALING

   // Determine the filename we actually want...
   Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]);

   // Figure out where to put DSOs
   StringTableEntry dsoPath = getDSOPath(pathBuffer);

   const char *ext = dStrrchr(pathBuffer, '.');

   if(!ext)
   {
      // We need an extension!
      Con::errorf(ConsoleLogEntry::Script, "exec: invalid script file name %s.", pathBuffer);
      execDepth--;
      return false;
   }

   // Check Editor Extensions
   bool isEditorScript = false;

#ifdef TORQUE_ALLOW_DSO_GENERATION
   // If the script file extension is '.ed.cs' then compile it to a different compiled extension
   if( dStricmp( ext, ".cs" ) == 0 )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.cs" ) == 0 )
         isEditorScript = true;
   }
   else if( dStricmp( ext, ".gui" ) == 0 )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.gui" ) == 0 )
         isEditorScript = true;
   }
#endif //TORQUE_ALLOW_DSO_GENERATION

   // rdbhack: if we can't find the script file in the game directory, look for it
   //   in the Application Data directory. This makes it possible to keep the user
   //   ignorant of where the files are actually saving to, thus eliminating the need
   //   for the script functions: execPrefs, getUserDataDirectory, etc.
   //
   //   This works because we know that script files located in the prefs path will 
   //   not have compiled versions (it checks for this further down). Otherwise this
   //   would be a big problem!
   
   StringTableEntry scriptFileName = StringTable->EmptyString;


//Luma : This is redundant, we wont be building dso's on the device - 
//plus saving dso to the user directory when attempting build for the
//release tests on iPhone is irrelevant.
#ifdef TORQUE_ALLOW_DSO_GENERATION

   if(!ResourceManager->find(pathBuffer))
   {
      // NOTE: this code is pretty much a duplication of code much further down in this
      //       function...

      // our work just got a little harder.. if we couldn't find the .cs, then we need to
      // also look for the .dso BEFORE we can try the prefs path.. UGH
      const char *filenameOnly = dStrrchr(pathBuffer, '/');
      if(filenameOnly)
         ++filenameOnly;
      else
         filenameOnly = pathBuffer;

      // we could skip this step and rid ourselves of a bunch of nonsense but we can't be
      // certain the dso path is the same as the path given to use in scriptFileNameBuffer
      char pathAndFilename[1024]; 
      Platform::makeFullPathName(filenameOnly, pathAndFilename, sizeof(pathAndFilename), dsoPath);

      char nameBuffer[1024];
      if( isEditorScript ) // this should never be the case since we are a PLAYER not a TOOL, but you never know
         dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, ".edso", NULL);
      else
         dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, ".dso", NULL);

      if(!ResourceManager->find(nameBuffer))
         scriptFileName = Platform::getPrefsPath(Platform::stripBasePath(pathBuffer));
      else
         scriptFileName = StringTable->insert(pathBuffer);
   }
   else
      scriptFileName = StringTable->insert(pathBuffer);
#else //TORQUE_ALLOW_DSO_GENERATION

    //Luma : Just insert the file name.
    scriptFileName = StringTable->insert(pathBuffer);

#endif //TORQUE_ALLOW_DSO_GENERATION

    //Luma : Something screwed up so get out early
   if(scriptFileName == NULL || *scriptFileName == 0)
   {
      execDepth--;
      return false;
   }

#ifdef TORQUE_ALLOW_JOURNALING
   
   bool compiled = dStricmp(ext, ".mis") && !journal && !Con::getBoolVariable("Scripts::ignoreDSOs");
#else
   bool compiled = dStricmp(ext, ".mis")  && !Con::getBoolVariable("Scripts::ignoreDSOs");
#endif //TORQUE_ALLOW_JOURNALING

   // [tom, 12/5/2006] stripBasePath() messes up if the filename is not in the exe
   // path, current directory or prefs path. Thus, getDSOFilename() will also mess
   // up and so this allows the scripts to still load but without a DSO.
   if(Platform::isFullPath(Platform::stripBasePath(pathBuffer)))
      compiled = false;

   // [tom, 11/17/2006] It seems to make sense to not compile scripts that are in the
   // prefs directory. However, getDSOPath() can handle this situation and will put
   // the dso along with the script to avoid name clashes with tools/game dsos.
#ifdef TORQUE_ALLOW_DSO_GENERATION
    // Is this a file we should compile? (anything in the prefs path should not be compiled)
    StringTableEntry prefsPath = Platform::getPrefsPath();

   if( dStrlen(prefsPath) > 0 && dStrnicmp(scriptFileName, prefsPath, dStrlen(prefsPath)) == 0)
      compiled = false;
#endif //TORQUE_ALLOW_DSO_GENERATION

   // If we're in a journaling mode, then we will read the script
   // from the journal file.
#ifdef TORQUE_ALLOW_JOURNALING
   if(journal && Game->isJournalReading())
   {
      char fileNameBuf[256];
      bool fileRead;
      U32 fileSize;

      Game->getJournalStream()->readString(fileNameBuf);
      Game->getJournalStream()->read(&fileRead);
      if(!fileRead)
      {
         Con::errorf(ConsoleLogEntry::Script, "Journal script read (failed) for %s", fileNameBuf);
         execDepth--;
         return false;
      }
      Game->journalRead(&fileSize);
      char *script = new char[fileSize + 1];
      Game->journalRead(fileSize, script);
      script[fileSize] = 0;
      Con::printf("Executing (journal-read) %s.", scriptFileName);
      CodeBlock *newCodeBlock = new CodeBlock();
      newCodeBlock->compileExec(scriptFileName, script, noCalls, 0);
      delete [] script;

      execDepth--;
      return true;
   }
#endif //TORQUE_ALLOW_JOURNALING

   // Ok, we let's try to load and compile the script.
   ResourceObject *rScr = ResourceManager->find(scriptFileName);
   ResourceObject *rCom = NULL;

   char nameBuffer[512];
   char* script = NULL;
   U32 scriptSize = 0;
   U32 version;

   Stream *compiledStream = NULL;
   FileTime comModifyTime, scrModifyTime;

   // Check here for .edso
   //bool edso = false;
   //if( dStricmp( ext, ".edso" ) == 0  && rScr )
   //{
   //   edso = true;
   //   rCom = rScr;
   //   rScr = NULL;

   //   rCom->getFileTimes( NULL, &comModifyTime );
   //   dStrcpy( nameBuffer, scriptFileName );
   //}

   // If we're supposed to be compiling this file, check to see if there's a DSO
   if(compiled /*&& !edso*/)
   {
      const char *filenameOnly = dStrrchr(scriptFileName, '/');
      if(filenameOnly)
         ++filenameOnly; //remove the / at the front
      else
         filenameOnly = scriptFileName;

      char pathAndFilename[1024];
      Platform::makeFullPathName(filenameOnly, pathAndFilename, sizeof(pathAndFilename), dsoPath);

      if( isEditorScript )
         dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, ".edso", NULL);
      else
         dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, ".dso", NULL);

      rCom = ResourceManager->find(nameBuffer);

      if(rCom)
         rCom->getFileTimes(NULL, &comModifyTime);
      if(rScr)
         rScr->getFileTimes(NULL, &scrModifyTime);
   }

   // Let's do a sanity check to complain about DSOs in the future.
   //
   // MM:	This doesn't seem to be working correctly for now so let's just not issue
   //		the warning until someone knows how to resolve it.
   //
   //if(compiled && rCom && rScr && Platform::compareFileTimes(comModifyTime, scrModifyTime) < 0)
   //{
   //Con::warnf("exec: Warning! Found a DSO from the future! (%s)", nameBuffer);
   //}

    // If we had a DSO, let's check to see if we should be reading from it.
    if((compiled && rCom) && (!rScr || Platform::compareFileTimes(comModifyTime, scrModifyTime) >= 0))
    {
      compiledStream = ResourceManager->openStream(nameBuffer);
      if (compiledStream)
      {
         // Check the version!
         compiledStream->read(&version);
         if(version != DSO_VERSION)
         {
            Con::warnf("exec: Found an old DSO (%s, ver %d < %d), ignoring.", nameBuffer, version, DSO_VERSION);
            ResourceManager->closeStream(compiledStream);
            compiledStream = NULL;
         }
      }
    }

#ifdef TORQUE_ALLOW_JOURNALING
   // If we're journalling, let's write some info out.
   if(journal && Game->isJournalWriting())
      Game->getJournalStream()->writeString(scriptFileName);
#endif //TORQUE_ALLOW_JOURNALING

   if(rScr && !compiledStream)
   {
      // If we have source but no compiled version, then we need to compile
      // (and journal as we do so, if that's required).

       //Con::errorf( "No DSO found! : %s", scriptFileName );
       
      Stream *s = ResourceManager->openStream(scriptFileName);
       
#ifdef	TORQUE_ALLOW_JOURNALING
      if(journal && Game->isJournalWriting())
         Game->getJournalStream()->write(bool(s != NULL));
#endif	//TORQUE_ALLOW_JOURNALING

      if(s)
      {
         scriptSize = ResourceManager->getSize(scriptFileName);
         script = new char [scriptSize+1];
         s->read(scriptSize, script);

#ifdef	TORQUE_ALLOW_JOURNALING
         if(journal && Game->isJournalWriting())
         {
            Game->journalWrite(scriptSize);
            Game->journalWrite(scriptSize, script);
         }
#endif	//TORQUE_ALLOW_JOURNALING
         ResourceManager->closeStream(s);
         script[scriptSize] = 0;
      }

      if (!scriptSize || !script)
      {
         delete [] script;
         Con::errorf(ConsoleLogEntry::Script, "exec: invalid script file %s.", scriptFileName);
         execDepth--;
         return false;
      }

//Luma: Sven -
// no dsos in the editor, seems to fail with so many console changes and version crap. Leaving it to
// work with cs files as is, as they are included with the source either way.

//Also, no DSO generation on iPhone
#ifdef TORQUE_OS_IOS
      if(false) 
#else
      if(compiled)
#endif
      {
         // compile this baddie.
         #if defined(TORQUE_DEBUG)
         Con::printf("Compiling %s...", scriptFileName);
         #endif
         CodeBlock *code = new CodeBlock();
         code->compile(nameBuffer, scriptFileName, script);
         delete code;
         code = NULL;

         compiledStream = ResourceManager->openStream(nameBuffer);
         if(compiledStream)
         {
            compiledStream->read(&version);
         }
         else
         {
            // We have to exit out here, as otherwise we get double error reports.
            delete [] script;
            execDepth--;
            return false;
         }
      }
   }
   else
   {
#ifdef	TORQUE_ALLOW_JOURNALING
      if(journal && Game->isJournalWriting())
         Game->getJournalStream()->write(bool(false));
#endif	//TORQUE_ALLOW_JOURNALING
   }
    
    //Luma : Load compiled script here
   if(compiledStream)
   {
      // Delete the script object first to limit memory used
      // during recursive execs.
      delete [] script;
      script = 0;

      // We're all compiled, so let's run it.
      
      //Luma: Profile script executions 
      F32 st1 = (F32)Platform::getRealMilliseconds();

      CodeBlock *code = new CodeBlock;
      code->read(scriptFileName, *compiledStream);
      ResourceManager->closeStream(compiledStream);
      code->exec(0, scriptFileName, NULL, 0, NULL, noCalls, NULL, 0);

        F32 et1 = (F32)Platform::getRealMilliseconds();
        
        F32 etf = et1 - st1;

        if ( scriptExecutionEcho )
            Con::printf("Loaded compiled script %s. Took %.0f ms", scriptFileName, etf);

      ret = true;
   }
   else if(rScr) //Luma : Load normal cs file here.
   {
         // No compiled script,  let's just try executing it
         // directly... this is either a mission file, or maybe
         // we're on a readonly volume.
        
         CodeBlock *newCodeBlock = new CodeBlock();
         StringTableEntry name = StringTable->insert(scriptFileName);

      
      //Luma: Profile script executions 
         F32 st1 = (F32)Platform::getRealMilliseconds();
         
         newCodeBlock->compileExec(name, script, noCalls, 0);

         F32 et1 = (F32)Platform::getRealMilliseconds();

         F32 etf = et1 - st1;
         
        if ( scriptExecutionEcho )
            Con::printf("Executed %s. Took %.0f ms", scriptFileName, etf);

         ret = true;
   }
   else
   {
      // Don't have anything.
      Con::warnf(ConsoleLogEntry::Script, "Missing file: %s!", pathBuffer);
      ret = false;
   }

   delete [] script;
   execDepth--;
   return ret;
}

ConsoleFunction(eval, const char *, 2, 2, "( script ) Use the eval function to execute any valid script statement.\n"
                                                                "If you choose to eval a multi-line statement, be sure that there are no comments or comment blocks embedded in the script string.\n"
                                                                "@param script A string containing a valid script statement. This may be a single line statement or multiple lines concatenated together with new-line characters.\n"
                                                                "@return Returns the result of executing the script statement.\n"
                                                                "@sa call")
{
   TORQUE_UNUSED( argc );
   return Con::evaluate(argv[1], false, NULL);
}

ConsoleFunction(getVariable, const char *, 2, 2, "(string varName) Grabs the relevant data for the variable represented by the given string\n"
                "@param varName A String representing the variable to check\n"
                "@return Returns a string containing component data for the requested variable or an empty string if not found.")
{
   return Con::getVariable(argv[1]);
}

ConsoleFunction(isFunction, bool, 2, 2, "(string funcName) Checks whether given name represents a current valid function.\n"
                "@param funcName The name of the function to check.\n"
                "@return Returns either true if the string represents a valid function or false if not.")
{
   return Con::isFunction(argv[1]);
}

ConsoleFunction(isMethod, bool, 3, 3, "(string namespace, string method) Checks whether the given method name represents a valid method within the given namespace.\n"
                "@param namespace A string representing the namespace in which the method should reside.\n"
                "@param method The name of the method in question.\n"
                "@return Returns a boolean value which is true if the given mathod name represents a valid method in the namespace and false otherwise.")
{
   Namespace* ns = Namespace::find( StringTable->insert( argv[1] ) );
   Namespace::Entry* nse = ns->lookup( StringTable->insert( argv[2] ) );
   if( !nse )
      return false;

   return true;
}

ConsoleFunction(getModNameFromPath, const char *, 2, 2, "(string path) Attempts to extract a mod directory from path. Returns empty string on failure.")
{
   StringTableEntry modPath = Con::getModNameFromPath(argv[1]);
   return modPath ? modPath : "";
}

//----------------------------------------------------------------

ConsoleFunction(getPrefsPath, const char *, 1, 2, "([fileName])")
{
   const char *filename = Platform::getPrefsPath(argc > 1 ? argv[1] : NULL);
   if(filename == NULL || *filename == 0)
      return "";
     
   return filename;
}

ConsoleFunction(execPrefs, bool, 2, 4, "execPrefs(fileName [, nocalls [,journalScript]])")
{
   const char *filename = Platform::getPrefsPath(argv[1]);
   if (filename == NULL || *filename == 0)
      return false;

   if ( !Platform::isFile(filename) )
    return false;

   argv[0] = "exec";
   argv[1] = filename;
   return dAtob(Con::execute(argc, argv));
}

ConsoleFunction(export, void, 2, 4, "( wildCard [ ,fileName [ , append  ] ]) Use the export function to save all global variables matching the specified name pattern in wildCard to a file, either appending to that file or over-writing it.\n"
                                                                "@param wildCard A string identifying what variable(s) to export. All characters used to create a global are allowed and the special symbol \"*\", meaning 0 or more instances of any character.\n"
                                                                "@param fileName A string containing a path to a file in which to save the globals and their definitions.\n"
                                                                "@param append A boolean value. If this value is true, the file will be appended to if it exists, otherwise it will be created/over-written.\n"
                                                                "@return No return value")
{
    // Fetch the wildcard.
    const char* pWildcard = argv[1];

    // Fetch the filename.
    const char* pFilename = NULL;
    if ( argc >= 3 )
    {
        Con::expandPath( pathBuffer, sizeof(pathBuffer), argv[2] );
        pFilename = pathBuffer;
    }

    // Fetch append flag.
    const bool append = argc >= 4 ? dAtob(argv[3] ) : false;

    // Export the variables.
    gEvalState.globalVars.exportVariables( pWildcard, pFilename, append );
}

ConsoleFunction(deleteVariables, void, 2, 2, "( wildCard ) Use the deleteVariables function to delete any global variable matching the wildCard statement.\n"
                                                                "@param wildCard A string identifying what variable(s) to delete. All characters used to create a global are allowed and the special symbol \"*\", meaning 0 or more instances of any character.\n"
                                                                "@return No return value")
{
   TORQUE_UNUSED( argc );
   gEvalState.globalVars.deleteVariables(argv[1]);
}

//----------------------------------------------------------------

ConsoleFunction(trace, void, 2, 2, "( enable ) Use the trace function to enable (or disable) function call tracing. If enabled, tracing will print a message every time a function is entered, showing what arguments it received, and it will print a message every time a function is exited, showing the return value (or last value of last statement) for that function.\n"
                                                                "@param enable A boolean value. If set to true, tracing is enabled, otherwise it is disabled.\n"
                                                                "@return No return value")
{
   TORQUE_UNUSED( argc );
   gEvalState.traceOn = dAtob(argv[1]);
   Con::printf("Console trace is %s", gEvalState.traceOn ? "on." : "off.");
}

//----------------------------------------------------------------

#if defined(TORQUE_DEBUG) || defined(INTERNAL_RELEASE)
ConsoleFunction(debug, void, 1, 1, "() Use the debug function to cause the engine to issue a debug break and to break into an active debugger.\n"
                                                                "For this to work, the engine must have been compiled with either TORQUE_DEBUG, or INTERNAL_RELEASE defined\n"
                                                                "@return No return value.")
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   Platform::debugBreak();
}
#endif

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
//Some code for allowing torsion to connect, this code returns the ipads local ip address
//code was obtained from http://blog.zachwaugh.com/post/309927273/programmatically-retrieving-ip-address-of-iphone
//adapted to iT2D by me
ConsoleFunction(getAppleDeviceIPAddress, const char*, 1, 1, "Gets the Apple hardware local IP on wifi. Should work on OS X and iOS")
{
    char *address = Con::getReturnBuffer(32);
    dStrcpy(address, "error");
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;

    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0)
    {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL)
        {
            if(temp_addr->ifa_addr->sa_family == AF_INET)
            {
                // Check if interface is en0 which is the wifi connection on the iPhone
                // Note: Could be different on MacOSX and simulator and may need modifying
                if(dStrcmp(temp_addr->ifa_name, "en0") == 0)
                {
                    dStrcpy(address, inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr));
                }
            }

            temp_addr = temp_addr->ifa_next;
        }
    }

    // Free memory
    freeifaddrs(interfaces);

    return address;
}
#endif

ConsoleFunctionGroupEnd( MetaScripting );

//----------------------------------------------------------------

ConsoleFunctionGroupBegin( InputManagement, "Functions that let you deal with input from scripts" );

ConsoleFunction( deactivateDirectInput, void, 1, 1, "() Use the deactivateDirectInput function to de-activate polling of direct input devices (keyboard, mouse, joystick, et cetera).\n"
                                                                "@return No return value.\n"
                                                                "@sa activateDirectInput")
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   if ( Input::isActive() )
      Input::deactivate();
}

//--------------------------------------------------------------------------
ConsoleFunction( activateDirectInput, void, 1, 1, "() Use the activateDirectInput function to activate polling of direct input devices (keyboard, mouse, joystick, et cetera).\n"
                                                                "@return No return value.\n"
                                                                "@sa deactivateDirectInput")
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   if ( !Input::isActive() )
      Input::activate();
}

ConsoleFunctionGroupEnd( InputManagement );

ConsoleFunction(openFolder, void, 2, 2, "(folder) Opens a folder of the platform")
{
    Platform::openFolder(argv[1]);
}

//Luma:	Console function to tell if this is a TORQUE_OS_IOS build
ConsoleFunction(isiPhoneBuild, bool, 1, 1, "Returns true if this is a iPhone build, false otherwise")
{
#ifdef	TORQUE_OS_IOS
   return true;
#else
   return false;
#endif	//TORQUE_OS_IOS
}

