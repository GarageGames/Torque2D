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

ConsoleMethod( AssetManager, compileReferencedAssets, bool, 3, 3,  "(moduleDefinition) - Compile the referenced assets determined by the specified module definition.\n"
                                                                    "@param moduleDefinition The module definition specifies the asset manifest.\n"
                                                                    "@return Whether the compilation was successful or not." )
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = Sim::findObject<ModuleDefinition>( argv[2] );

    // Did we find the module definition?
    if ( pModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::compileReferencedAssets() - Could not find the module definition '%s'.", argv[2] );        
        return false;
    }

    // Compile referenced assets.
    return object->compileReferencedAssets( pModuleDefinition );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, addModuleDeclaredAssets, bool, 3, 3,   "(moduleDefinition) - Add any the declared assets specified by the module definition.\n"
                                                                    "@param moduleDefinition The module definition specifies the asset manifest.\n"
                                                                    "@return Whether adding declared assets was successful or not." )
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = Sim::findObject<ModuleDefinition>( argv[2] );

    // Did we find the module definition?
    if ( pModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::addDeclaredAssets() - Could not find the module definition '%s'.", argv[2] );        
        return false;
    }

    // Add module declared assets.
    return object->addModuleDeclaredAssets( pModuleDefinition );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, addDeclaredAsset, bool, 4, 4,  "(moduleDefinition, assetFilePath) - Add the specified asset against the specified module definition.\n"
                                                            "@param moduleDefinition The module definition that may contain declared assets.\n"
                                                            "@return Whether adding declared assets was successful or not." )
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = Sim::findObject<ModuleDefinition>( argv[2] );

    // Did we find the module definition?
    if ( pModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::addDeclaredAsset() - Could not find the module definition '%s'.", argv[2] );        
        return false;
    }

    // Fetch asset file-path.
    const char* pAssetFilePath = argv[3];

    // Add declared asset.
    return object->addDeclaredAsset( pModuleDefinition, pAssetFilePath );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, addPrivateAsset, const char*, 3, 3,    "(assetObject) - Adds a private asset object.\n"
                                                                    "@param assetObject The asset object to add as a private asset.\n"
                                                                    "@return The allocated private asset Id." )
{
    // Fetch asset.
    AssetBase* pAssetBase = Sim::findObject<AssetBase>( argv[2] );

    // Did we find the asset?
    if ( pAssetBase == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::addPrivateAsset() - Could not find the asset '%s'.", argv[2] );        
        return StringTable->EmptyString;
    }

    // Add private asset.
    return object->addPrivateAsset( pAssetBase );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, removeDeclaredAssets, bool, 3, 3,  "(moduleDefinition) - Remove any the declared assets specified by the module definition.\n"
                                                                "@param moduleDefinition The module definition that may contain declared assets.\n"
                                                                "@return Whether removing declared assets was successful or not." )
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = Sim::findObject<ModuleDefinition>( argv[2] );

    // Did we find the module definition?
    if ( pModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::removeDeclaredAssets() - Could not find the module definition '%s'.", argv[2] );        
        return false;
    }

    // Remove declared assets.
    return object->removeDeclaredAssets( pModuleDefinition );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, removeDeclaredAsset, bool, 3, 3,   "(assetId) - Remove the specified declared asset Id.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return Whether removing the declared asset was successful or not." )
{
    // Remove the declared asset Id.
    return object->removeDeclaredAsset( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getAssetName, const char*, 3, 3,   "(assetId) - Gets the asset name from the specified asset Id.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return The asset name from the specified asset Id.")
{
    return object->getAssetName( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getAssetDescription, const char*, 3, 3,    "(assetId) - Gets the asset description from the specified asset Id.\n"
                                                                        "@param assetId The selected asset Id.\n"
                                                                        "@return The asset description from the specified asset Id.")
{
    return object->getAssetDescription( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getAssetCategory, const char*, 3, 3,       "(assetId) - Gets the asset category from the specified asset Id.\n"
                                                                        "@param assetId The selected asset Id.\n"
                                                                        "@return The asset category from the specified asset Id.")
{
    return object->getAssetCategory( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getAssetType, const char*, 3, 3,   "(assetId) - Gets the asset type from the specified asset Id.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return The asset type from the specified asset Id.")
{
    return object->getAssetType( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getAssetFilePath, const char*, 3, 3,   "(assetId) - Gets the asset file-path from the specified asset Id.\n"
                                                                    "@param assetId The selected asset Id.\n"
                                                                    "@return The asset file-path from the specified asset Id.")
{
    return object->getAssetFilePath( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getAssetPath, const char*, 3, 3,       "(assetId) - Gets the asset path (not including the asset file) from the specified asset Id.\n"
                                                                    "@param assetId The selected asset Id.\n"
                                                                    "@return The asset path (not including the asset file) from the specified asset Id.")
{
    return object->getAssetPath( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getAssetModule, const char*, 3, 3, "(assetId) - Gets the module definition where the the specified asset Id is located.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return The module definition where the the specified asset Id is located")
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = object->getAssetModuleDefinition( argv[2] );

    return pModuleDefinition == NULL ? StringTable->EmptyString : pModuleDefinition->getIdString();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, isAssetInternal, bool, 3, 3,       "(assetId) - Check whether the specified asset Id is internal or not.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return Whether the specified asset Id is internal or not.")
{
    return object->isAssetInternal( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, isAssetPrivate, bool, 3, 3,       "(assetId) - Check whether the specified asset Id is private or not.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return Whether the specified asset Id is private or not.")
{
    return object->isAssetPrivate( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, isAssetAutoUnload, bool, 3, 3,    "(assetId) - Check whether the specified asset Id is auto-unload or not.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return Whether the specified asset Id is auto-unload or not.")
{
    return object->isAssetAutoUnload( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, isAssetLoaded, bool, 3, 3,         "(assetId) - Check whether the specified asset Id is loaded or not.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return Whether the specified asset Id is loaded or not.")
{
    return object->isAssetLoaded( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, isDeclaredAsset, bool, 3, 3,       "(assetId) - Check whether the specified asset Id is declared or not.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return Whether the specified asset Id is declared or not.")
{
    return object->isDeclaredAsset( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, isReferencedAsset, bool, 3, 3,     "(assetId) - Check whether the specified asset Id is referenced or not.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return Whether the specified asset Id is referenced or not.")
{
    return object->isReferencedAsset( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, renameDeclaredAsset, bool, 4, 4,   "(assetIdFrom, assetIdTo) - Rename declared asset Id.\n"
                                                                "@param assetIdFrom The selected asset Id to rename from.\n"
                                                                "@param assetIdFrom The selected asset Id to rename to.\n"
                                                                "@return Whether the rename was successful or not.")
{
    return object->renameDeclaredAsset( argv[2], argv[3] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, renameReferencedAsset, bool, 4, 4,     "(assetIdFrom, assetIdTo) - Rename referenced asset Id.\n" 
                                                                    "@param assetIdFrom The selected asset Id to rename from.\n"
                                                                    "@param assetIdFrom The selected asset Id to rename to.\n"
                                                                    "@return Whether the rename was successful or not.")
{
    return object->renameReferencedAsset( argv[2], argv[3] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, acquireAsset, const char*, 3, 4,   "(assetId, [asPrivate?]) - Acquire the specified asset Id.\n"
                                                                "You must release the asset once you're finish with it using 'releaseAsset'.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@param asPrivate Whether to acquire the asset Id as a private asset.\n"
                                                                "@return The acquired asset or NULL if not acquired.")
{
    // Fetch asset Id.
    const char* pAssetId = argv[2];

    // Fetch private asset flag.
    const bool asPrivate = argc >= 4 ? dAtob(argv[3]) : false;

    // Reset asset reference.
    AssetBase* pAssetBase = NULL;

    // Acquire private asset?
    if ( asPrivate )
    {
        // Acquire private asset.
        pAssetBase = object->acquireAsPrivateAsset<AssetBase>( pAssetId );
    }
    else
    {
        // Acquire public asset.
        pAssetBase = object->acquireAsset<AssetBase>( pAssetId );
    }

    return pAssetBase != NULL ? pAssetBase->getIdString() : StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, releaseAsset, bool, 3, 3,          "(assetId) - Release the specified asset Id.\n"
                                                                "The asset should have been acquired using 'acquireAsset'.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return Whether the asset was released or not.")
{
    // Release asset.
    return object->releaseAsset( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, purgeAssets, void, 2, 2,           "() - Purge all assets that are not referenced even if they are set to not auto-unload.\n"
                                                                "Assets can be in this state because they are either set to not auto-unload or the asset manager has/is disabling auto-unload.\n"
                                                                "@return No return value.")
{
    // Purge assets.
    object->purgeAssets();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, deleteAsset, bool, 5, 5,   "(assetId, deleteLooseFiles, deleteDependencies) Deletes the specified asset Id and optionally its loose files and asset dependencies.\n"
                                                        "@param assetId The selected asset Id.\n"
                                                        "@param deleteLooseFiles Whether to delete an assets loose files or not.\n"
                                                        "@param deleteDependencies Whether to delete assets that depend on this asset or not.\n"
                                                        "@return Whether the asset deletion was successful or not.  A failure only indicates that the specified asset was not deleted but dependent assets and their loose files may have being deleted.\n" )
{
    // Fetch asset Id.
    const char* pAssetId = argv[2];

    // Fetch delete loose-files flag.
    const bool deleteLooseFiles = dAtob(argv[3]);

    // Fetch delete dependencies flag.
    const bool deleteDependencies = dAtob(argv[4]);

    // Delete asset.
    return object->deleteAsset( pAssetId, deleteLooseFiles, deleteDependencies );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, refreshAsset, void, 3, 3,          "(assetId) Refresh the specified asset Id.\n"
                                                                "@param assetId The selected asset Id.\n"
                                                                "@return No return value.")
{
    object->refreshAsset( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, refreshAllAssets, void, 2, 3,      "([bool includeUnloaded]) Refresh all declared assets.\n"
                                                                "@param Whether to include currently unloaded assets in the refresh or not.  Optional: Defaults to false.\n"
                                                                "Refreshing all assets can be an expensive (time-consuming) operation to perform.\n"
                                                                "@return No return value.")
{
    // Refresh assets without flag.
    if ( argc == 2 )
        object->refreshAllAssets();

    // Refresh assets with flag.
    object->refreshAllAssets(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, saveAssetTags, bool, 2, 2,         "() - Save the currently loaded asset tags manifest.\n"
                                                                "@return Whether the save was successful or not." )
{
    // Save asset tags.
    return object->saveAssetTags();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, restoreAssetTags, bool, 2, 2,      "() - Restore the currently loaded asset tags manifest from disk (replace anything in memory).\n"
                                                                "@return Whether the restore was successful or not." )
{
    // Restore asset tags.
    return object->restoreAssetTags();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getAssetTags, S32, 2, 2,   "() - Gets the currently loaded asset tags manifest.\n"
                                                        "@return The currently loaded asset tags manifest or zero if not loaded." )
{
    // Fetch the asset tags manifest.
    AssetTagsManifest* pAssetTagsManifest = object->getAssetTags();

    return pAssetTagsManifest == NULL ? 0 : pAssetTagsManifest->getId();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAllAssets, S32, 3, 5,      "(assetQuery, [ignoreInternal?], [ignorePrivate?]) - Performs an asset query searching for all assets optionally ignoring internal assets.\n"
                                                            "@param assetQuery The asset query object that will be populated with the results.\n"
                                                            "@param ignoreInternal Whether to ignore internal assets or not.  Optional: Defaults to true."
                                                            "@param ignorePrivate Whether to ignore private assets or not.  Optional: Defaults to true."
                                                            "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAllAssets() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Perform query if no more arguments.
    if ( argc == 3 )
        return object->findAllAssets( pAssetQuery );

    // Fetch ignore internal flag.
    const bool ignoreInternal = dAtob(argv[3]);

    // Perform query if no more arguments.
    if ( argc == 4 )
        return object->findAllAssets( pAssetQuery, ignoreInternal );

    // Fetch ignore private flag.
    const bool ignorePrivate = dAtob(argv[4]);

    // Perform query.
    return object->findAllAssets( pAssetQuery, ignoreInternal, ignorePrivate );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetName, S32, 4, 5,          "(assetQuery, assetName, [partialName?]) - Performs an asset query searching for the specified asset name.\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetName The asset name to search for.  This may be a partial name if 'partialName' is true.\n"
                                                                "@param partialName Whether the asset name is to be used as a partial name or not.  Optional: Defaults to false.\n"
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetName() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset name.
    const char* pAssetName = argv[3];

    // Perform query if no more arguments.
    if ( argc == 4 )
        return object->findAssetName( pAssetQuery, pAssetName );

    // Yes, so fetch partial name flag.
    const bool partialName = dAtob( argv[4] );

    // Perform query.
    return object->findAssetName( pAssetQuery, pAssetName, partialName );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetCategory, S32, 4, 5,      "(assetQuery, assetCategory, [assetQueryAsSource?]) - Performs an asset query searching for the specified asset category.\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetCategory The asset category to search for."
                                                                "@param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.\n"
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetCategory() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset category.
    const char* pAssetCategory = argv[3];

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetCategory( pAssetQuery, pAssetCategory );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetCategory( pAssetQuery, pAssetCategory, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetAutoUnload, S32, 4, 5,    "(assetQuery, assetAutoUnload, [assetQueryAsSource?]) - Performs an asset query searching for the specified asset auto-unload flag.\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetInternal The asset internal flag to search for."
                                                                "@param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.\n"
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetAutoUnload() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset auto-unload flag.
    const bool assetAutoUnload = dAtob(argv[3]);

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetAutoUnload( pAssetQuery, assetAutoUnload );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetAutoUnload( pAssetQuery, assetAutoUnload, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetInternal, S32, 4, 5,      "(assetQuery, assetInternal, [assetQueryAsSource?]) - Performs an asset query searching for the specified asset internal flag.\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetInternal The asset internal flag to search for."
                                                                "@param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.\n"
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetInternal() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset internal flag.
    const bool assetInternal = dAtob(argv[3]);

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetInternal( pAssetQuery, assetInternal );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetInternal( pAssetQuery, assetInternal, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetPrivate, S32, 4, 5,       "(assetQuery, assetPrivate, [assetQueryAsSource?]) - Performs an asset query searching for the specified asset private flag.\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetPrivate The asset private flag to search for."
                                                                "@param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.\n"
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetPrivate() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset private flag.
    const bool assetPrivate = dAtob(argv[3]);

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetPrivate( pAssetQuery, assetPrivate );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetInternal( pAssetQuery, assetPrivate, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetType, S32, 4, 5,          "(assetQuery, assetType, [assetQueryAsSource?]) - Performs an asset query searching for the specified asset type.\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetType The asset type to search for."
                                                                "@param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.\n"
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetType() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset type.
    const char* pAssetType = argv[3];

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetType( pAssetQuery, pAssetType );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetType( pAssetQuery, pAssetType, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetDependsOn, S32, 4, 4,     "(assetQuery, assetId) - Performs an asset query searching for asset Ids that the specified asset Id depends on.\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetId The asset Id to query for any asset Ids that it depends on."
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetDependsOn() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset Id.
    const char* pAssetId = argv[3];

    // Perform query.
    return object->findAssetDependsOn( pAssetQuery, pAssetId );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetIsDependedOn, S32, 4, 4,  "(assetQuery, assetId) - Performs an asset query searching for asset Ids that depend on the specified asset Id.\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetId The asset Id to query for any asset Ids that may depend on it."
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetIsDependedOn() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset Id.
    const char* pAssetId = argv[3];

    // Perform query.
    return object->findAssetIsDependedOn( pAssetQuery, pAssetId );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findInvalidAssetReferences, S32, 3, 3, "(assetQuery) - Performs an asset query searching for invalid asset references.\n"
                                                                    "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                    "@return The number of asset Ids found that are invalid or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findInvalidAssetReferences() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Perform query.
    return object->findInvalidAssetReferences( pAssetQuery );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findTaggedAssets, S32, 4, 5,       "(assetQuery, assetTagNames, [assetQueryAsSource?]) - Performs an asset query searching for the specified asset tag name(s).\n"
                                                                "@param assetQuery The asset query object that will be populated with the results.\n"
                                                                "@param assetTagNames The asset tag name or names to search for.  Multiple names can be specified using comma, space, tab or newline separation.  Tags use an OR operation i.e. only assets tagged with ANY of the specified tags will be returned.\n"
                                                                "@param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.\n"
                                                                "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findTaggedAssets() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset tag name(s).
    const char* pAssetTagNames = argv[3];

    // Perform query if no more arguments.
    if ( argc == 4 )
        return object->findTaggedAssets( pAssetQuery, pAssetTagNames );

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findTaggedAssets( pAssetQuery, pAssetTagNames, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, findAssetLooseFile, S32, 4, 5, "(assetQuery, assetLooseFile, [assetQueryAsSource?]) - Performs an asset query searching for the specified loose file.\n"
        "@param assetQuery The asset query object that will be populated with the results.\n"
        "@param assetLooseFile The loose-file used by the asset to search for."
        "@param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.\n"
        "@return The number of asset Ids found or (-1) if an error occurred.")
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetLooseFile() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset loose file.
    const char* pAssetLooseFile = argv[3];

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetLooseFile( pAssetQuery, pAssetLooseFile );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetLooseFile( pAssetQuery, pAssetLooseFile, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getDeclaredAssetCount, bool, 2, 2,     "() - Gets the number of declared assets.\n"
                                                                    "@return Returns the number of declared assets.")
{
    return object->getDeclaredAssetCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getReferencedAssetCount, bool, 2, 2,   "() - Gets the number of asset referenced.\n"
                                                                    "@return Returns the number of asset references.")
{
    return object->getReferencedAssetCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getLoadedInternalAssetCount, bool, 2, 2,   "() - Gets the number of loaded internal assets.\n"
                                                                        "@return Returns the number of loaded internal assets.")
{
    return object->getLoadedInternalAssetCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getMaxLoadedInternalAssetCount, bool, 2, 2,    "() - Gets the maximum number of loaded internal assets.\n"
                                                                            "@return Returns the maximum number of loaded internal assets.")
{
    return object->getMaxLoadedInternalAssetCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getLoadedExternalAssetCount, bool, 2, 2,   "() - Gets the number of loaded external assets.\n"
                                                                        "@return Returns the number of loaded external assets.")
{
    return object->getLoadedExternalAssetCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, getMaxLoadedExternalAssetCount, bool, 2, 2,    "() - Gets the maximum number of loaded external assets.\n"
                                                                            "@return Returns the maximum number of loaded external assets.")
{
    return object->getMaxLoadedExternalAssetCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod( AssetManager, dumpDeclaredAssets, void, 2, 2,     "() - Dumps a breakdown of all declared assets.\n"
                                                                "@return No return value.")
{
    return object->dumpDeclaredAssets();
}
