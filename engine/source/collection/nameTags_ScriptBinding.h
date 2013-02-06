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

ConsoleMethod(NameTags, createTag, S32, 3, 3,           "(tagName) Creates a tag.\n"
                                                        "@param tagName The tag name to create.\n"
                                                        "@return The tag Id created." )
{
    return object->createTag( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(NameTags, renameTag, S32, 4, 4,           "(tagId, newTagName) Renames an existing tag.\n"
                                                        "@param tagId The tag Id to use.\n"
                                                        "@param newTagName The new tag name to assign.\n"
                                                        "@return The tag Id confirms the rename else 0 indicates an error." )
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

ConsoleMethod(NameTags, deleteTag, S32, 3, 3,           "(tagId) Deletes a tag.\n"
                                                        "@param tagId The tag Id to use.\n"
                                                        "@return The tag Id confirms the delete else 0 indicates an error." )
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

ConsoleMethod(NameTags, getTagCount, S32, 2, 2,         "() Gets the total tag count.\n"
                                                        "@return No return value.")
{
    return object->getTagCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(NameTags, getTagName, const char*, 3, 3,  "(tagId) Gets the tag name associated with the tag Id.\n"
                                                        "@param tagId The tag Id to use.\n"
                                                        "@return The tag name associated with the tag Id or empty string if the tag Id was not found." )
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

ConsoleMethod(NameTags, getTagId, S32, 3, 3,            "(tagName) Gets the tag Id associated with the tag name.\n"
                                                        "@param tagName The tag name to associated with the tag Id.\n"
                                                        "@return The tag Id associated with the tag name or 0 if the tag name was not found." )
{
    return object->getTagId( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(NameTags, getAllTags, const char*, 2, 2,  "() Gets all the tag Id / tag name pairs.\n"
                                                        "@return [tag Id / tag Name] pairs")
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

ConsoleMethod(NameTags, tag, bool, 4, 4,            "(object, tagIds) Adds the tag Id(s) to the object.\n"
                                                    "@param object The object to use.\n"
                                                    "@param tagIds The tag Id(s) to use.\n"
                                                    "@return Whether the tag operation was successful or not." )
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

ConsoleMethod(NameTags, untag, bool, 4, 4,          "(object, tagIds) Removes the tag Id(s) from the object.\n"
                                                    "@param object The object to use.\n"
                                                    "@param tagIds The tag Id(s) to use.\n"
                                                    "@return Whether the un-tag operation was successful or not." )
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

ConsoleMethod(NameTags, hasTag, bool, 4, 4,         "(object, tagId) Checks whether the object is tagged with any of the tag Ids or not.\n"
                                                    "@param object The object to use.\n"
                                                    "@param tagId The tag Id(s) to use.\n"
                                                    "@return Whether the object is tagged with any of the tag Ids or not." )
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

ConsoleMethod(NameTags, queryTags, const char*, 3, 4,   "(tagIds, [excluded]) Finds any objects added to this instance that are tagged with any of the specified tag(s).\n"                                            
                                                        "@param tagId The tag Id to use.\n"
                                                        "@param excluded Optionally inverts the query resulting in objects that are excluded i.e. objects that are NOT tagged with any of the specified tag(s).\n"
                                                        "@return Any objects added to this instance that are tagged with any of the specified tag(s)." )
{
    // Fetch tag Id.
    const NameTags::TagId tagId = dAtoi(argv[2]);

    // Sanity!
    if ( tagId == 0 )
    {
        Con::warnf("Invalid tag Id.\n");
        return false;
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
    dSprintf(pReturnBuffer, sizeof(pReturnBuffer), "%s", "");
    char* pBuffer = pReturnBuffer;

    for( NameTags::queryType::iterator itr = results.begin(); itr != results.end(); ++itr )
    {
        const U32 offset = dSprintf( pBuffer, bufferSize, "%d ", itr->key );
        pBuffer += offset;
        bufferSize -= offset;
    }

    return pReturnBuffer;
}


#endif // _NAMETAGS_SCRIPTBINDING_H_