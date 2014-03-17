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

#ifndef _NAMETAGS_SCRIPTBINDING_H_
#define _NAMETAGS_SCRIPTBINDING_H_
 
#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

//-----------------------------------------------------------------------------

ConsoleMethodGroupBeginWithDocs(NameTags, SimSet)

/*! Creates a tag.
    @param tagName The tag name to create.
    @return The tag Id created.
*/
ConsoleMethodWithDocs(NameTags, createTag, ConsoleInt, 3, 3, (tagName))
{
    return object->createTag( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Renames an existing tag.
    @param tagId The tag Id to use.
    @param newTagName The new tag name to assign.
    @return The tag Id confirms the rename else 0 indicates an error.
*/
ConsoleMethodWithDocs(NameTags, renameTag, ConsoleInt, 4, 4, (tagId, newTagName))
{
    // Fetch tag Id.
    const NameTags::TagId tagId = dAtoi(argv[2]);

    // Sanity!
    if ( tagId == 0 )
    {
        Con::warnf("Invalid tag Id.\n");
        return 0;
    }

    // Fetch new tag name.
    const char* newTagName = argv[3];

    return object->renameTag( tagId, newTagName );
}

//-----------------------------------------------------------------------------

/*! Deletes a tag.
    @param tagId The tag Id to use.
    @return The tag Id confirms the delete else 0 indicates an error.
*/
ConsoleMethodWithDocs(NameTags, deleteTag, ConsoleInt, 3, 3, (tagId))
{
    // Fetch tag Id.
    const NameTags::TagId tagId = dAtoi(argv[2]);

    // Sanity!
    if ( tagId == 0 )
    {
        Con::warnf("Invalid tag Id.\n");
        return NULL;
    }

    return object->deleteTag( tagId );
}

//-----------------------------------------------------------------------------

/*! Gets the total tag count.
    @return No return value.
*/
ConsoleMethodWithDocs(NameTags, getTagCount, ConsoleInt, 2, 2, ())
{
    return object->getTagCount();
}

//-----------------------------------------------------------------------------

/*! Gets the tag name associated with the tag Id.
    @param tagId The tag Id to use.
    @return The tag name associated with the tag Id or empty string if the tag Id was not found.
*/
ConsoleMethodWithDocs(NameTags, getTagName, ConsoleString, 3, 3, (tagId))
{
    // Fetch tag Id.
    const NameTags::TagId tagId = dAtoi(argv[2]);

    // Sanity!
    if ( tagId == 0 )
    {
        Con::warnf("Invalid tag Id.\n");
        return NULL;
    }

    return object->getTagName( tagId );
}

//-----------------------------------------------------------------------------

/*! Gets the tag Id associated with the tag name.
    @param tagName The tag name to associated with the tag Id.
    @return The tag Id associated with the tag name or 0 if the tag name was not found.
*/
ConsoleMethodWithDocs(NameTags, getTagId, ConsoleInt, 3, 3, (tagName))
{
    return object->getTagId( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets all the tag Id / tag name pairs.
    @return [tag Id / tag Name] pairs
*/
ConsoleMethodWithDocs(NameTags, getAllTags, ConsoleString, 2, 2, ())
{
    // Get buffer.
    const U32 bufferLength = 4096;
    char* pBuffer = Con::getReturnBuffer( bufferLength );

    // Format tags.
    const S32 bufferUsed = object->formatTags( pBuffer, bufferLength );

    // Sanity!
    if ( bufferUsed < 0 )
    {
        Con::warnf("Buffer overflow when formatting all tags.  All tags will not be returned.\n");
    }

    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Adds the tag Id(s) to the object.
    @param object The object to use.
    @param tagIds The tag Id(s) to use.
    @return Whether the tag operation was successful or not.
*/
ConsoleMethodWithDocs(NameTags, tag, ConsoleBool, 4, 4, (object, tagIds))
{
    // Fetch object Id.
    const SimObjectId objectId = (SimObjectId)dAtoi(argv[2]);

    // Fetch element count.
    const U32 elementCount = StringUnit::getUnitCount( argv[3], " \t\n" );

    // Iterate tags.
    for ( U32 index = 0; index < elementCount; ++index )
    {
        // Fetch tag Id.
        const NameTags::TagId tagId = dAtoi( StringUnit::getUnit( argv[3], index, " \t\n" ) );

        // Sanity!
        if ( tagId == 0 )
        {
            Con::warnf("Invalid tag Id.\n");
            return false;
        }
   
        // Tag.
        if ( !object->tag( objectId, tagId ) )
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

/*! Removes the tag Id(s) from the object.
    @param object The object to use.
    @param tagIds The tag Id(s) to use.
    @return Whether the un-tag operation was successful or not.
*/
ConsoleMethodWithDocs(NameTags, untag, ConsoleBool, 4, 4, (object, tagIds))
{
    // Fetch object Id.
    const SimObjectId objectId = (SimObjectId)dAtoi(argv[2]);

    // Fetch element count.
    const U32 elementCount = StringUnit::getUnitCount( argv[3], " \t\n" );

    // Iterate tags.
    for ( U32 index = 0; index < elementCount; ++index )
    {
        // Fetch tag Id.
        const NameTags::TagId tagId = dAtoi( StringUnit::getUnit( argv[3], index, " \t\n" ) );

        // Sanity!
        if ( tagId == 0 )
        {
            Con::warnf("Invalid tag Id.\n");
            return false;
        }
  
        // Un-tag.
        if ( !object->untag( objectId, tagId ) )
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

/*! Checks whether the object is tagged with any of the tag Ids or not.
    @param object The object to use.
    @param tagId The tag Id(s) to use.
    @return Whether the object is tagged with any of the tag Ids or not.
*/
ConsoleMethodWithDocs(NameTags, hasTag, ConsoleBool, 4, 4, (object, tagId))
{
    // Fetch object Id.
    const SimObjectId objectId = (SimObjectId)dAtoi(argv[2]);

    // Fetch element count.
    const U32 elementCount = StringUnit::getUnitCount( argv[3], " \t\n" );

    // Iterate tags.
    for ( U32 index = 0; index < elementCount; ++index )
    {
        // Fetch tag Id.
        const NameTags::TagId tagId = dAtoi( StringUnit::getUnit( argv[3], index, " \t\n" ) );

        // Sanity!
        if ( tagId == 0 )
        {
            Con::warnf("Invalid tag Id.\n");
            return false;
        }
   
        // Has tag?
        if ( object->hasTag( objectId, tagId ) )
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

/*! Finds any objects added to this instance that are tagged with any of the specified tag(s).    
    @param tagId The tag Id to use.
    @param excluded Optionally inverts the query resulting in objects that are excluded i.e. objects that are NOT tagged with any of the specified tag(s).
    @return Any objects added to this instance that are tagged with any of the specified tag(s).
*/
ConsoleMethodWithDocs(NameTags, queryTags, ConsoleString, 3, 4, (tagIds, [excluded]))
{
    // Fetch tag Id.
    const NameTags::TagId tagId = dAtoi(argv[2]);

    // Sanity!
    if ( tagId == 0 )
    {
        Con::warnf("Invalid tag Id.\n");
        return NULL;
    }

    // Query tags.
    object->queryTags( argv[2] );

    // Fetch appropriate results.
    NameTags::queryType results;
    if ( argc < 4 )
    {
        results = object->mIncludedQueryMap;
    }
    else
    {
        results = object->mExcludedQueryMap;
    }    

    // Format results.
    U32 bufferSize = 8192;
    char* pReturnBuffer = Con::getReturnBuffer( bufferSize );
    dSprintf(pReturnBuffer, bufferSize * sizeof(char), "%s", "");
    char* pBuffer = pReturnBuffer;

    for( NameTags::queryType::iterator itr = results.begin(); itr != results.end(); ++itr )
    {
        const U32 offset = dSprintf( pBuffer, bufferSize, "%d ", itr->key );
        pBuffer += offset;
        bufferSize -= offset;
    }

    return pReturnBuffer;
}

ConsoleMethodGroupEndWithDocs(NameTags)

#endif // _NAMETAGS_SCRIPTBINDING_H_
