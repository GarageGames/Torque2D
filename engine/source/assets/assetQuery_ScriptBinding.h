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

ConsoleMethod(AssetQuery, clear, void, 2, 2,    "() - Clears all asset Id results.\n"
                                                "@return () No return value.")
{
    object->clear();
}

//-----------------------------------------------------------------------------

ConsoleMethod(AssetQuery, set, bool, 3, 3,  "(assetQuery) Sets the asset query to a copy of the specified asset query.\n"
                                            "@param assetQuery The asset query to copy.\n"
                                            "@return Whether the operation succeeded or not." )
{
    // Find asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetQuery::set() - Could not find asset query '%s'.", argv[2] );
        return false;
    }

    // Set asset query.
    object->set( *pAssetQuery );

    return true;
}

//-----------------------------------------------------------------------------

ConsoleMethod(AssetQuery, getCount, S32, 2, 2,  "() - Gets the count of asset Id results.\n"
                                                "@return (int) The count of asset Id results.")
{
    return object->size();
}

//-----------------------------------------------------------------------------

ConsoleMethod(AssetQuery, getAsset, const char*, 3, 3,  "(int resultIndex) - Gets the asset Id at the specified query result index.\n"
                                                        "@param resultIndex The query result index to use.\n"
                                                        "@return (assetId) The asset Id at the specified index or NULL if not valid.")
{
    // Fetch result index.
    const S32 resultIndex = dAtoi(argv[2]);

    // Is index within bounds?
    if ( resultIndex >= object->size() )
    {
        // No, so warn.
        Con::warnf( "AssetQuery::getAsset() - Result index '%s' is out of bounds.", argv[2] );
        return StringTable->EmptyString;
    }

    return object->at(resultIndex);
}
