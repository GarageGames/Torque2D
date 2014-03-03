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

#include "console/consoleInternal.h"
#include "console/ast.h"

ConsoleFunctionGroupBegin( FieldManipulators, "Functions to manipulate data returned in the form of \"x y z\".");

/*! @defgroup FieldManipulatorFunctions String Fields
      @ingroup TorqueScriptFunctions
      @{
*/

//-----------------------------------------------------------------------------

/*! 
*/
ConsoleFunctionWithDocs(getUnit, ConsoleString, 4, 4, ( sourceString ,index, separator ))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), argv[3] ) );
}

//-----------------------------------------------------------------------------

/*! 
*/
ConsoleFunctionWithDocs(getUnitCount, ConsoleInt, 3, 3, ( sourceString, separator ))
{
   TORQUE_UNUSED( argc );
   return StringUnit::getUnitCount(argv[1], argv[2] );
}

//-----------------------------------------------------------------------------

/*! 
*/
ConsoleFunctionWithDocs(setUnit, ConsoleString, 5, 5, ( sourceString ,index, replace, separator))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], argv[4]) );
}

//-----------------------------------------------------------------------------

/*! Use the getWord function to get the word at index in sourceString.
    @param sourceString A string containing one or more words.
    @return Returns word at index in sourceString, or null string if no word exists at that index.
    @sa getWords, setWord
*/
ConsoleFunctionWithDocs(getWord, ConsoleString, 3, 3, ( sourceString ,index ))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), " \t\n") );
}

/*! Use the getWords function to retrieve a set of words from a sourceString.
    @param sourceString A string containing one or more words.
    @param index The index of the first word to retrieve.
    @param endindex The index of the final word to retrieve.
    @return Returns all words (separated by current delimiter) from sourceString, starting at index and ending at endIndex or end of string, whichever comes first. If no endIndex is specified, all remaining words are returned.
    @sa getWord, setWord
*/
ConsoleFunctionWithDocs(getWords, ConsoleString, 3, 4, ( sourceString ,index [,endindex]))
{
   U32 endIndex;
   if(argc==3)
      endIndex = 1000000;
   else
      endIndex = dAtoi(argv[3]);
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, " \t\n") );
}

/*! Use the setWord function to replace an existing word with a new word(s), or to add word(s) to a string..
    @param sourceString A string containing one or more words.
    @param index The index of the word to remove.
    @param replace The new word(s) to replace the word at index with.
    @return There are multiple return cases:- In the first case, a simple one-to-one replacement, the word at index in sourceString will be replaced with the value in replace, and the new string will be returned.- In the first case, a multi-to-one replacement, the word at index in sourceString will be replaced with the value in replace, which can be two or more words, and the new string will be returned.- In the third and final case, new records, empty or filled, can be appended to the end of sourceString. If index is beyond the end of the sourceString, that is, the index is greater than the total count of words in sourceString, the requisite number of empty (null-string) words will be appended to the end of sourceString and the value in replace will be appended to the end of this new string. This entire resultant string will be returned.
    @sa getWord, getWords, removeWord
*/
ConsoleFunctionWithDocs(setWord, ConsoleString, 4, 4, ( sourceString ,index,replace))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], " \t\n") );
}

/*! Use the removeWord function to remove a single indexed word from a sourceString.
    @param sourceString A string containing one or more words.
    @param index The index of the word to remove.
    @return Returns sourceString minus the removed word. If the index is greater than the number of words in sourceString, the original string is returned.
    @sa setWord
*/
ConsoleFunctionWithDocs(removeWord, ConsoleString, 3, 3, ( sourceString ,index))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), " \t\n") );
}

/*! Use the getWordCount function to get the number of words in sourceString.
    @param sourceString A string containing one or more words.
    @return Returns number of words in sourceString or 0 if no words are present
*/
ConsoleFunctionWithDocs(getWordCount, ConsoleInt, 2, 2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   return StringUnit::getUnitCount(argv[1], " \t\n");
}

//--------------------------------------
/*! Use the getField function to get the field at index in sourceString.
    @param sourceString A string containing one or more fields.
    @return Returns field at index in sourceString, or null string if no field exists at that index.
    @sa getFields, setField
*/
ConsoleFunctionWithDocs(getField, ConsoleString, 3, 3, ( sourceString , index ))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), "\t\n") );
}

/*! Use the getFields function to retrieve a set of fields from a sourceString.
    @param sourceString A string containing one or more fields.
    @param index The index of the first field to retrieve.
    @param endindex The index of the final field to retrieve.
    @return Returns all fields (separated by current delimiter) from sourceString, starting at index and ending at endIndex or end of string, whichever comes first. If no endIndex is specified, all remaining fields are returned.
    @sa getField, setField
*/
ConsoleFunctionWithDocs(getFields, ConsoleString, 3, 4, ( sourceString , index [ , endindex ] ))
{
   U32 endIndex;
   if(argc==3)
      endIndex = 1000000;
   else
      endIndex = dAtoi(argv[3]);
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, "\t\n") );
}

/*! Use the setField function to replace an existing field with a new field(s), or to add field(s) to a string..
    @param sourceString A string containing one or more fields.
    @param index The index of the field to remove.
    @param replace The new field(s) to replace the field at index with.
    @return There are multiple return cases:- In the first case, a simple one-to-one replacement, the field at index in sourceString will be replaced with the value in replace, and the new string will be returned.- In the first case, a multi-to-one replacement, the field at index in sourceString will be replaced with the value in replace, which can be two or more fields, and the new string will be returned.- In the thrid and final case, new records, empty or filled, can be appended to the end of sourceString. If index is beyond the end of the sourceString, that is, the index is greater than the total count of fields in sourceString, the requisite number of empty (null-string) fields will be appended to the end of sourceString and the value in replace will be appended to the end of this new string. This entire resultant string will be returned.
    @sa getField, getFields, removeField
*/
ConsoleFunctionWithDocs(setField, ConsoleString, 4, 4, ( sourceString , index , replace ))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], "\t\n") );
}

/*! Use the removeField function to remove a single indexed field from a sourceString.
    @param sourceString A string containing one or more fields.
    @param index The index of the field to remove.
    @return Returns sourceString minus the removed field. If the index is greater than the number of fields in sourceString, the original string is returned.
    @sa setField
*/
ConsoleFunctionWithDocs(removeField, ConsoleString, 3, 3, ( sourceString , index ))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), "\t\n") );
}

/*! Use the getFieldCount function to get the number of fields in sourceString.
    @param sourceString A string containing one or more fields.
    @return Returns number of fields in sourceString or 0 if no fields are present
*/
ConsoleFunctionWithDocs(getFieldCount, ConsoleInt, 2, 2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   return StringUnit::getUnitCount(argv[1], "\t\n");
}

//--------------------------------------
/*! Use the getRecord function to get the record at index in sourceString.
    @param sourceString A string containing one or more records.
    @return Returns record at index in sourceString, or NULL string if no record exists at that index.
    @sa getRecords, setRecord
*/
ConsoleFunctionWithDocs(getRecord, ConsoleString, 3, 3, ( sourceString , index ))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::getUnit(argv[1], dAtoi(argv[2]), "\n") );
}

/*! Use the getRecords function to retrieve a set of records from a sourceString.
    @param sourceString A string containing one or more records.
    @param index The index of the first record to retrieve.
    @param endindex The index of the final record to retrieve.
    @return Returns all records (separated by current delimiter) from sourceString, starting at index and ending at endIndex or end of string, whichever comes first. If no endIndex is specified, all remaining records are returned.
    @sa getRecord, setRecord
*/
ConsoleFunctionWithDocs(getRecords, ConsoleString, 3, 4, ( sourceString , index [ , endindex ] ))
{
   U32 endIndex;
   if(argc==3)
      endIndex = 1000000;
   else
      endIndex = dAtoi(argv[3]);
   return Con::getReturnBuffer( StringUnit::getUnits(argv[1], dAtoi(argv[2]), endIndex, "\n") );
}

/*! Use the setRecord function to replace an existing record with a new record(s), or to add record(s) to a string..
    @param sourceString A string containing one or more records.
    @param index The index of the record to remove.
    @param replace The new record(s) to replace the record at index with.
    @return There are multiple return cases:- In the first case, a simple one-to-one replacement, the record at index in sourceString will be replaced with the value in replace, and the new string will be returned.- In the first case, a multi-to-one replacement, the record at index in sourceString will be replaced with the value in replace, which can be two or more records, and the new string will be returned.- In the thrid and final case, new records, empty or filled, can be appended to the end of sourceString. If index is beyond the end of the sourceString, that is, the index is greater than the total count of records in sourceString, the requisite number of empty (null-string) records will be appended to the end of sourceString and the value in replace will be appended to the end of this new string. This entire resultant string will be returned.
    @sa getRecord, getRecords, removeRecord
*/
ConsoleFunctionWithDocs(setRecord, ConsoleString, 4, 4, ( sourceString , index , replace ))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::setUnit(argv[1], dAtoi(argv[2]), argv[3], "\n") );
}

/*! Use the removeRecord function to remove a single indexed record from a sourceString.
    @param sourceString A string containing one or more records.
    @param index The index of the record to remove.
    @return Returns sourceString minus the removed record. If the index is greater than the number of records in sourceString, the original string is returned.
    @sa setRecord
*/
ConsoleFunctionWithDocs(removeRecord, ConsoleString, 3, 3, ( sourceString , index ))
{
   TORQUE_UNUSED( argc );
   return Con::getReturnBuffer( StringUnit::removeUnit(argv[1], dAtoi(argv[2]), "\n") );
}

/*! Use the getRecordCount function to get the number of records in sourceString.
    @param sourceString A string containing one or more records.
    @return Returns number of records in sourceString or 0 if no records are present
*/
ConsoleFunctionWithDocs(getRecordCount, ConsoleInt, 2, 2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   return StringUnit::getUnitCount(argv[1], "\n");
}
//--------------------------------------
/*! Use the firstWord function to retrieve the first word found in sourceString.
    @param sourceString A string containing one or more words.
    @return Returns the first word found in sourceString, or a NULL string, if no words are found.
    @sa restWords
*/
ConsoleFunctionWithDocs(firstWord, ConsoleString, 2, 2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   const char *word = dStrchr(argv[1], ' ');
   U32 len;
   if(word == NULL)
      len = dStrlen(argv[1]);
   else
      len = (U32)(word - argv[1]);
   char *ret = Con::getReturnBuffer(len + 1);
   dStrncpy(ret, argv[1], len);
   ret[len - 1] = 0;
   return ret;
}

/*! Use the restWords function to retrieve all words after the first word in sourceString.
    @param sourceString A string containing one or more words.
    @return Returns a string containing all the words after the first word found in sourceString, or a NULL string if no words remain after the first word (or if no words at all remain).
    @sa firstWord
*/
ConsoleFunctionWithDocs(restWords, ConsoleString, 2, 2, ( sourceString ))
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

/*! Use the nextToken function to get the first token found in tokenList, where tokens are separated by the character(s) specified in delimeter.
	The token itself is stored in a variable whose name is specified in tokenVar. This function provides complex power in a simple package.
	Please read the notes below, they are very important.
    This function is scope-smart. That is, when we specify the name of the variable to store a token in by passing a value in tokenVar,
	we do not include either a local symbol (%), or a global symbol ($). We just pass in an un-adorned name, let's say \George\.
	Then, depending on where this function is called, \George\ will become a local (%George), or a global ($George) variable.
	If this function is called within a function or method definition, \George\ will be local (%George).
	If this function is called from the file-scope (executed as part of a file and not within the scope of a function or method),
	\George\ will become a global ($George). There is one additional special case. If you attempt to use this from the console
	command line, the token will vaporize and no variable will be created
    @param tokenList The string containing token(s).
    @param tokenVar The 'name' of the variable to store the token in.
    @param delimeter The character(s) to use as a delimeter. A delimeter may be a single character, or a sequence of characters.
    @return Returns a copy of tokenList, less the first token and the first delimiter. If there are no more tokens, a NULL string is returned.
*/
ConsoleFunctionWithDocs(NextToken, ConsoleString,4,4, ( tokenList , tokenVar , delimeter ))
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

ConsoleFunctionGroupEnd(FieldManipulators)

/*! @} */ // group FieldManipulatorFunctions
