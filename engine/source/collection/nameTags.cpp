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

#include "collection/nameTags.h"

// Script bindings.
#include "collection/nameTags_ScriptBinding.h"

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT( NameTags );

//-----------------------------------------------------------------------------

NameTags::NameTags() :
    mMasterTagId( 1 )
{
    mNameTagsFieldEntry = StringTable->insert("NameTags");
}

//-----------------------------------------------------------------------------

NameTags::~NameTags()
{
    mHashTagMap.clear();
    mTagNameMap.clear();
}

//-----------------------------------------------------------------------------

bool NameTags::onAdd()
{
    // Call parent.
    if ( !Parent::onAdd() )
        return false;

    // Fetch tag count.
    StringTableEntry tagCountFieldName = StringTable->insert("TagCount");
    const U32 tagCount = dAtoi( getDataField( tagCountFieldName, NULL ) );

    // Finish if not tags to add.
    if ( tagCount == 0 )
        return true;

    // Clear tag count field.
    setDataField( tagCountFieldName, NULL, "" );

    // Read tags.
    StringTableEntry tagFieldName = StringTable->insert("Tag");
    char indexBuffer[16];
    for ( U32 index = 0; index < tagCount; ++index )
    {
        // Fetch field value.
        dSprintf( indexBuffer, 16, "%d", index );
        const char* pFieldValue = getDataField( tagFieldName, indexBuffer );

        // Fetch tag Id.
        const TagId tagId = dAtoi( StringUnit::getUnit( pFieldValue, 0, "\n" ) );

        // Fetch tag name.
        const char* pTagName = StringUnit::getUnit( pFieldValue, 1, "\n" );

        // Fetch hash.
        const HashId hash = StringTable->hashString( pTagName );

        // Insert hash / tag Id.
        mHashTagMap.insert( hash, tagId );

        // Insert tag.
        mTagNameMap.insert( tagId, StringTable->insert( pTagName, true ) );

        // Ensure master tag Id is correct.
        if ( tagId >= mMasterTagId )
            mMasterTagId = tagId+1;

        //dSscanf( pFieldValue, "%d\n%s", &tagId, tagNameBuffer );
        setDataField( tagFieldName, indexBuffer, "" );
    }

    return true;
}

//-----------------------------------------------------------------------------

NameTags::TagId NameTags::createTag( const char* pTagName )
{
    // Sanity!
    AssertFatal( pTagName != NULL, "Tag cannot be null." );
    AssertFatal( mMasterTagId != 0, "Tag system is full." );

    // Fetch hash Id.
    const HashId hashId = StringTable->hashString( pTagName );

    // Find hash Id.
    hashTagMapType::iterator hashItr = mHashTagMap.find( hashId );

    // Does hash Id already exist?
    if ( hashItr != mHashTagMap.end() )
    {
        // Yes, so return tag Id.
        return hashItr->value;
    }

    // Allocate tag Id.
    const NameTags::TagId tagId = mMasterTagId++;

    // Insert hash Id / tag Id.
    mHashTagMap.insert( hashId, tagId );

    // Insert tag.
    mTagNameMap.insert( tagId, StringTable->insert( pTagName, true ) );

    return tagId;
}

//-----------------------------------------------------------------------------

NameTags::TagId NameTags::renameTag( const NameTags::TagId tagId, const char* pNewTagName )
{
    // Sanity!
    AssertFatal( tagId > 0, "Invalid Tag Id." );
    AssertFatal( pNewTagName != NULL, "Tag cannot be null." );

    // Find tag.
    tagNameMapType::iterator itr = mTagNameMap.find( tagId );

    // Did we find the tag?
    if ( itr == mTagNameMap.end() )
    {
        // No, so error.
        return 0;
    }

    // Remove hash tag.
    mHashTagMap.erase( StringTable->hashString( itr->value ) );

    // Fetch new hash.
    const NameTags::HashId newHashId = StringTable->hashString( pNewTagName );

    // Insert hash tag.
    mHashTagMap.insert( newHashId, tagId );

    // Insert new name entry.
    itr->value = StringTable->insert( pNewTagName, true );

    return tagId;
}

//-----------------------------------------------------------------------------

NameTags::TagId NameTags::deleteTag( const NameTags::TagId tagId )
{
    // Sanity!
    AssertFatal( tagId > 0, "Invalid Tag Id." );

    // Find tag.
    tagNameMapType::iterator itr = mTagNameMap.find( tagId );

    // Did we find the tag?
    if ( itr == mTagNameMap.end() )
    {
        // No.
        return 0;
    }

    // Remove tag.
    mHashTagMap.erase( tagId );
    mTagNameMap.erase( tagId );

    return tagId;
}

//-----------------------------------------------------------------------------

StringTableEntry NameTags::getTagName( const NameTags::TagId tagId )
{
    // Sanity!
    AssertFatal( tagId > 0, "Invalid Tag Id." );

    // Find tag.
    tagNameMapType::iterator itr = mTagNameMap.find( tagId );

    // Did we find the tag?
    if ( itr == mTagNameMap.end() )
    {
        // No.
        return StringTable->EmptyString;
    }

    return itr->value;
}

//-----------------------------------------------------------------------------

NameTags::TagId NameTags::getTagId( const char* pTagName )
{
    // Sanity!
    AssertFatal( pTagName != NULL, "Tag cannot be null." );

    // Fetch hash Id.
    const NameTags::HashId hashId = StringTable->hashString( pTagName );

    // Find hash Id.
    hashTagMapType::iterator hashItr = mHashTagMap.find( hashId );

    // Does hash exist?
    if ( hashItr == mHashTagMap.end() )
    {
        // No.
        return 0;
    }

    return hashItr->value;
}

//-----------------------------------------------------------------------------

bool NameTags::tag( const SimObjectId objId, const TagId tagId )
{
    // Sanity!
    AssertFatal( tagId > 0, "Invalid Tag Id." );

    // Find object.
    SimObject* pSimObject = Sim::findObject( objId );

    // Sanity!
    if ( pSimObject == NULL )
    {
        // Warn.
        Con::warnf( "Could not find object '%d' to tag Id '%d'.", objId, tagId );
        return false;
    }

    char newTagsBuffer[4096];

    // Fetch tags.
    const char* pOldTags = pSimObject->getDataField( mNameTagsFieldEntry, NULL );

    // Check if already tagged.
    if ( dStrlen( pOldTags ) != 0 )
    {
        // Fetch element count.
        const U32 elementCount = StringUnit::getUnitCount( pOldTags, " \t\n" );

        // Iterate elements.
        for ( U32 index = 0; index < elementCount; ++index )
        {
            // Fetch element.
            const char* pElement = StringUnit::getUnit( pOldTags, index, " \t\n" );

            // if already tagged then finish.
            if ( dAtoi( pElement ) == tagId )
                return true;
        }

        // Format new tag Id.
        dSprintf( newTagsBuffer, 4096, "%s %d", pOldTags, tagId );
    }
    else
    {
        // Format new tag Id.
        dSprintf( newTagsBuffer, 4096, "%d", tagId );
    }

    // Update field.
    pSimObject->setDataField( mNameTagsFieldEntry, NULL, newTagsBuffer );

    return true;
}

//-----------------------------------------------------------------------------

bool NameTags::untag( const SimObjectId objId, const TagId tagId )
{
    // Sanity!
    AssertFatal( tagId > 0, "Invalid Tag Id." );

    // Find object.
    SimObject* pSimObject = Sim::findObject( objId );

    // Sanity!
    if ( pSimObject == NULL )
    {
        // Warn.
        Con::warnf( "Could not find object '%d' to un-tag Id '%d'.", objId, tagId );
        return false;
    }

    // Fetch tags.
    const char* pOldTags = pSimObject->getDataField( mNameTagsFieldEntry, NULL );

    // Finish if no tags.
    if ( dStrlen( pOldTags ) == 0 )
        return true;

    // Fetch element count.
    const U32 elementCount = StringUnit::getUnitCount( pOldTags, " \t\n" );

    // Iterate elements.
    for ( U32 index = 0; index < elementCount; ++index )
    {
        // Fetch element.
        const char* pElement = StringUnit::getUnit( pOldTags, index, " \t\n" );

        // Found tag?
        if ( dAtoi( pElement ) == tagId )
        {
            // Yes, so remove element.
            const char* pNewTags = StringUnit::removeUnit( pOldTags, index, " \t\n" );

            // Update field.
            pSimObject->setDataField( mNameTagsFieldEntry, NULL, pNewTags );

            // Done.
            return true;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

bool NameTags::hasTag( const SimObjectId objId, const TagId tagId ) const
{
    // Sanity!
    AssertFatal( tagId > 0, "Invalid Tag Id." );

    // Find object.
    SimObject* pSimObject = Sim::findObject( objId );

    // Sanity!
    if ( pSimObject == NULL )
    {
        // Warn.
        Con::warnf( "Could not find object '%d' to check tag Id '%d'.", objId, tagId );
        return false;
    }

    // Fetch tags.
    const char* pTags = pSimObject->getDataField( mNameTagsFieldEntry, NULL );

    // Finish if no tags.
    if ( dStrlen( pTags ) == 0 )
        return false;

    // Fetch element count.
    const U32 elementCount = StringUnit::getUnitCount( pTags, " \t\n" );

    // Iterate elements.
    for ( U32 index = 0; index < elementCount; ++index )
    {
        // Fetch element.
        const char* pElement = StringUnit::getUnit( pTags, index, " \t\n" );

        // Finish if found tag.
        if ( dAtoi( pElement ) == tagId )
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

void NameTags::queryTags( const char* pTags )
{
    Vector<NameTags::TagId> tags;
    Vector<SimObjectId> transfers;

    // Clear queries.
    mIncludedQueryMap.clear();
    mExcludedQueryMap.clear();

    // Reset excluded.
    for( Parent::iterator itr = begin(); itr != end(); ++itr )
    {
        SimObject* pSimObject = (*itr);
        mExcludedQueryMap.insert( pSimObject->getId(), pSimObject );
    }

    // Fetch tag count.
    const U32 tagCount = StringUnit::getUnitCount( pTags, " \t\n" );

    // Finish if no tags specified.
    if ( tagCount == 0 )
        return;

    // Get tags.
    for ( U32 index = 0; index < tagCount; ++index )
    {   
        // Fetch tag Id.
        const NameTags::TagId tagId = dAtoi( StringUnit::getUnit( pTags, index, " \t\n" ) );

        // Sanity!
        if ( tagId == 0 )
        {
            Con::warnf("Invalid tag Id used in query.\n");
            continue;
        }

        // Fetch tag Id.
        tags.push_back( tagId );
    }

    // Iterate tag Ids.
    for ( U32 tagIndex = 0; tagIndex < tagCount; ++tagIndex )
    {   
        // Fetch tag Id.
        const NameTags::TagId tagId = tags[tagIndex];

        // Iterate excluded objects.
        for( queryType::iterator itr = mExcludedQueryMap.begin(); itr != mExcludedQueryMap.end(); ++itr )
        {
            // Fetch object.
            const SimObjectId objectId = itr->key;
            SimObject* pSimObject = itr->value;

            // Fetch tags.
            const char* pFieldTags = pSimObject->getDataField( mNameTagsFieldEntry, NULL );

            // Any field tags?
            if ( dStrlen( pFieldTags ) > 0 )
            {
                // Yes, so fetch element count.
                const U32 elementCount = StringUnit::getUnitCount( pFieldTags, " \t\n" );

                // Iterate elements.
                for ( U32 index = 0; index < elementCount; ++index )
                {
                    // Fetch element.
                    const char* pElement = StringUnit::getUnit( pFieldTags, index, " \t\n" );

                    // Found tag?
                    if ( dAtoi( pElement ) == tagId )                        
                    {
                        // Yes, so add to transfers.
                        transfers.push_back( objectId );
                        mIncludedQueryMap.insert( objectId, pSimObject );
                        break;
                    }
                }
            }
        }

        // Adjust excluded objects.
        for( U32 transferIndex = 0; transferIndex < (U32)transfers.size(); ++transferIndex )
        {
            mExcludedQueryMap.erase( transfers[transferIndex] );
        }

        // Clear transfers.
        transfers.clear();
    }
}

//-----------------------------------------------------------------------------

S32 NameTags::formatTags( char* pBuffer, U32 bufferLength )
{
    // Sanity!
    AssertFatal( pBuffer != NULL, "Invalid buffer" );
    AssertFatal( bufferLength > 0, "Invalid buffer length" );

    S32 totalBufferUsed = 0;

    // Iterate tags.
    for( tagNameMapType::iterator itr = mTagNameMap.begin(); itr != mTagNameMap.end(); ++itr )
    {
        // Format tag (use newline as separator as tag names can have spaces).
        const U32 offset = dSprintf( pBuffer, bufferLength, "%d\n%s\n", itr->key, itr->value );
        
        // Finish if no formatting occurred.
        if ( offset == 0 )
            return -totalBufferUsed;

        totalBufferUsed += offset;
        pBuffer += offset;
        bufferLength -= offset;        
    }

    return totalBufferUsed;
}

//-----------------------------------------------------------------------------

void NameTags::write( Stream &stream, U32 tabStop, U32 flags )
{
    // Skipping the immediate parent (SimSet) stops the children from being written
    // which is exactly what we want.
    SimObject::write( stream, tabStop, flags );
}

//-----------------------------------------------------------------------------

void NameTags::writeFields(Stream& stream, U32 tabStop)
{
    // Fetch tag count.
    const U32 tagCount = getTagCount();

    if ( tagCount > 0 )
    {
        char tempBuffer[16];
        char valueBuffer[1024];

        // Tag count.
        dSprintf( tempBuffer, 16, "%d", tagCount );
        setDataField( StringTable->insert("TagCount"), NULL, tempBuffer );

        // Write tags.
        StringTableEntry tagFieldName = StringTable->insert("Tag");
        U32 fieldIndex = 0;
        for( tagNameMapType::iterator itr = mTagNameMap.begin(); itr != mTagNameMap.end(); ++itr )
        {
            dSprintf( tempBuffer, 16, "%d", fieldIndex++ );
            dSprintf( valueBuffer, 1024, "%d\n%s", itr->key, itr->value );
            setDataField( tagFieldName, tempBuffer, valueBuffer ); 
        }

        // Call parent.
        Parent::writeFields(stream, tabStop);

        // Now we've written these temporary fields, clear them.
        for( tagNameMapType::iterator itr = mTagNameMap.begin(); itr != mTagNameMap.end(); ++itr )
        {
            dSprintf( tempBuffer, 16, "%d", itr->key );
            setDataField( tagFieldName, tempBuffer, "" ); 
        }
    }
    else
    {
        // Call parent.
        Parent::writeFields(stream, tabStop);
    }
}
