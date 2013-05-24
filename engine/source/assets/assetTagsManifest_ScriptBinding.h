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

ConsoleMethodGroupBeginWithDocs(AssetTagsManifest, SimObject)

/*! Creates an asset tag.
    @param tagName The tag name to create.
    @return No return value.
*/
ConsoleMethodWithDocs( AssetTagsManifest, createTag, ConsoleVoid, 3, 3, (tagName))
{
    object->createTag( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Renames an existing asset tag.
    @param tagName The tag name to rename.
    @param newTagName The new tag name to assign.
    @return Whether the asset tag was renamed or not.
*/
ConsoleMethodWithDocs( AssetTagsManifest, renameTag, ConsoleBool, 4, 4, (oldTagName, newTagName))
{
    return object->renameTag( argv[2], argv[3] );
}

//-----------------------------------------------------------------------------

/*! Deletes an asset tag.
    @param tagName The tag name to delete.
    @return Whether the asset tag was deleted or not.
*/
ConsoleMethodWithDocs( AssetTagsManifest, deleteTag, ConsoleBool, 3, 3, (tagName))
{
    return object->deleteTag( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Checks whether the specified asset tag exists or not.
    @param tagName The tag name to check.
    @return Whether the specified asset tag exists or not.
*/
ConsoleMethodWithDocs( AssetTagsManifest, isTag, ConsoleBool, 3, 3, (tagName))
{
    return object->isTag( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the total asset tag count.
    @return The total asset tag count.
*/
ConsoleMethodWithDocs( AssetTagsManifest, getTagCount, ConsoleInt, 2, 2, ())
{
    return object->getTagCount();
}

//-----------------------------------------------------------------------------

/*! Gets the asset tag at the specified index.
    @param tagIndex The asset tag index.  This must be 0 to the asset tag count less one.
    @return The asset tag at the specified index or NULL if invalid.
*/
ConsoleMethodWithDocs( AssetTagsManifest, getTag, ConsoleString, 3, 3, (int tagIndex))
{
    // Fetch tag index.
    const U32 tagIndex = dAtoi(argv[2]);

    // Is the tag index out-of-bounds?
    if ( tagIndex >= object->getTagCount() )
    {
        // Yes, so warn.
        Con::warnf( "AssetTagsManifest: Asset tag index '%d' is out of bounds.  Asset tag count is '%d'", tagIndex, object->getTagCount() );
        return StringTable->EmptyString;
    }

    return object->getTag( tagIndex );
}

//-----------------------------------------------------------------------------

/*! Gets the asset tag count on the specified asset Id.
    @param assetId The asset Id to count tags on.
    @return The asset tag count on the specified asset Id.
*/
ConsoleMethodWithDocs( AssetTagsManifest, getAssetTagCount, ConsoleInt, 3, 3, (assetId))
{
    return object->getAssetTagCount( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the asset tag on the specified asset Id at the specified index.
    @param assetId The asset Id to count tags on.
    @param tagIndex The asset tag index.  This must be 0 to the asset tag count less one.
    @return The asset tag on the specified asset Id at the specified index or NULL if invalid.
*/
ConsoleMethodWithDocs( AssetTagsManifest, getAssetTag, ConsoleString, 4, 4, (assetId, int tagIndex))
{
    // Fetch asset Id.
    const char* pAssetId = argv[2];

    // Fetch tag index.
    const U32 tagIndex = dAtoi(argv[3]);

    // Is the tag index out-of-bounds?
    if ( tagIndex >= object->getAssetTagCount( pAssetId ) )
    {
        // Yes, so warn.
        Con::warnf( "AssetTagsManifest: Asset tag index '%d' is out of bounds.  Asset tag count is '%d'", tagIndex, object->getAssetTagCount( pAssetId ) );
        return StringTable->EmptyString;
    }

    return object->getAssetTag( pAssetId, tagIndex );
}

//-----------------------------------------------------------------------------

/*! Tags the asset Id with the specified asset tag.
    @param assetId The asset Id to tag.
    @param tagName The tag name to assign.
    @return Whether the tag operation was successful or not.
*/
ConsoleMethodWithDocs( AssetTagsManifest, tag, ConsoleBool, 4, 4, (assetId, tagName))
{
    return object->tag( argv[2], argv[3] );
}

//-----------------------------------------------------------------------------

/*! Un-tags the asset Id from the specified asset tag.
    @param assetId The asset Id to un-tag.
    @param tagName The tag name to un-assign.
    @return Whether the un-tag operation was successful or not.
*/
ConsoleMethodWithDocs( AssetTagsManifest, untag, ConsoleBool, 4, 4, (assetId, tagName))
{
    return object->untag( argv[2], argv[3] );
}

//-----------------------------------------------------------------------------

/*! Checks whether the asset Id is tagged with the specified asset tag.
    @param assetId The asset Id to check.
    @param tagName The tag name to check.
    @return Whether the asset Id is tagged with the specified asset tag or not.
*/
ConsoleMethodWithDocs( AssetTagsManifest, hasTag, ConsoleBool, 4, 4, (assetId, tagName))
{
    return object->hasTag( argv[2], argv[3] );
}

ConsoleMethodGroupEndWithDocs(AssetTagsManifest)
