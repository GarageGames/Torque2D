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

#ifndef _TAML_ASSET_DECLARED_VISITOR_H_
#define _TAML_ASSET_DECLARED_VISITOR_H_

#ifndef _TAML_XMLPARSER_H_
#include "persistence//taml/tamlXmlParser.h"
#endif

#ifndef _ASSET_FIELD_TYPES_H_
#include "assets/assetFieldTypes.h"
#endif

#ifndef _ASSET_DEFINITION_H_
#include "assetDefinition.h"
#endif

#ifndef _ASSET_BASE_H_
#include "assetBase.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

class TamlAssetDeclaredVisitor : public TamlXmlVisitor
{
protected:
    virtual bool visit( TiXmlElement* pXmlElement, TamlXmlParser& xmlParser )
    {
        // Debug Profiling.
        PROFILE_SCOPE(TamlAssetDeclaredVisitor_VisitElement);

        // Finish if this is not the root element.
        if ( pXmlElement != pXmlElement->GetDocument()->RootElement() )
            return true;

        // Fetch asset field names.
        StringTableEntry assetNameField = StringTable->insert( ASSET_BASE_ASSETNAME_FIELD );
        StringTableEntry assetDescriptionField = StringTable->insert( ASSET_BASE_ASSETDESCRIPTION_FIELD );
        StringTableEntry assetCategoryField = StringTable->insert( ASSET_BASE_CATEGORY_FIELD );
        StringTableEntry assetAutoUnloadField = StringTable->insert( ASSET_BASE_AUTOUNLOAD_FIELD );
        StringTableEntry assetInternalField = StringTable->insert( ASSET_BASE_ASSETINTERNAL_FIELD );

        // Iterate attributes.
        for ( TiXmlAttribute* pAttribute = pXmlElement->FirstAttribute(); pAttribute; pAttribute = pAttribute->Next() )
        {
            // Insert attribute name.
            StringTableEntry attributeName = StringTable->insert( pAttribute->Name() );

            // Asset name?
            if ( attributeName == assetNameField )
            {
                // Yes, so assign it.
                mAssetDefinition.mAssetName = StringTable->insert( pAttribute->Value() );
                continue;
            }
            // Asset description?
            else if ( attributeName == assetDescriptionField )
            {
                // Yes, so assign it.
                mAssetDefinition.mAssetDescription = StringTable->insert( pAttribute->Value() );
                continue;
            }
            // Asset description?
            else if ( attributeName == assetCategoryField )
            {
                // Yes, so assign it.
                mAssetDefinition.mAssetCategory = StringTable->insert( pAttribute->Value() );
                continue;
            }
            // Asset auto-unload?
            else if ( attributeName == assetAutoUnloadField )
            {
                // Yes, so assign it.
                mAssetDefinition.mAssetAutoUnload = dAtob( pAttribute->Value() );
                continue;
            }
            // Asset internal?
            else if ( attributeName == assetInternalField )
            {
                // Yes, so assign it.
                mAssetDefinition.mAssetInternal = dAtob( pAttribute->Value() );
                continue;
            }
        }

        // Did we get an asset name?
        if ( mAssetDefinition.mAssetName == StringTable->EmptyString )
        {
            // No, so reset everything.
            clear();

            // Stop processing!
            return false;
        }

        // Set asset file-path.
        mAssetDefinition.mAssetBaseFilePath = StringTable->insert( xmlParser.getParsingFilename() );

        // Set asset type.
        mAssetDefinition.mAssetType = StringTable->insert( pXmlElement->Value() );

        return true;
    }

    virtual bool visit( TiXmlAttribute* pAttribute, TamlXmlParser& xmlParser )
    {
        // Debug Profiling.
        PROFILE_SCOPE(TamlAssetDeclaredVisitor_VisitAttribute);

        // Sanity!
        AssertFatal( mAssetDefinition.mAssetName != StringTable->EmptyString, "Cannot generate asset dependencies without asset name." );

        // Fetch asset reference.
        const char* pAssetReference = pAttribute->Value();

        // Fetch field word count.
        const U32 fieldWordCount = StringUnit::getUnitCount( pAssetReference, ASSET_ASSIGNMENT_TOKEN );

        // Finish if there are not two words.
        if ( fieldWordCount != 2 )
            return true;

        // Fetch the asset signature.
        StringTableEntry assetSignature = StringTable->insert( StringUnit::getUnit( pAssetReference, 0, ASSET_ASSIGNMENT_TOKEN ) );

        // Is this an asset Id signature?
        if ( assetSignature == StringTable->insert(ASSET_ID_SIGNATURE) )
        {
            // Yes, so get asset Id.
            typeAssetId assetId = StringTable->insert( StringUnit::getUnit( pAssetReference, 1, ASSET_ASSIGNMENT_TOKEN ) );

            // Finish if the dependency is itself!
            if ( mAssetDefinition.mAssetId == assetId )
                return true;

            // Iterate existing dependencies.
            for( typeAssetIdVector::iterator dependencyItr = mAssetDependencies.begin(); dependencyItr != mAssetDependencies.end(); ++dependencyItr )
            {
                // Finish if asset Id is already a dependency.
                if ( *dependencyItr == assetId )
                    return true;
            }

            // Insert asset reference.
            mAssetDependencies.push_back( assetId );
        }
        // Is this a loose-file signature?
        else if ( assetSignature == StringTable->insert(ASSET_LOOSEFILE_SIGNATURE) )
        {
            // Yes, so get loose-file reference.
            const char* pAssetLooseFile = StringUnit::getUnit( pAssetReference, 1, ASSET_ASSIGNMENT_TOKEN );

            char assetFilePathBuffer[1024];

            // Is the asset loose-file expando specified?
            if ( *pAssetLooseFile == '#' )
            {
                // Yes, so fetch relative path.
                char parsingFileBuffer[1024];
                dStrcpy( parsingFileBuffer, xmlParser.getParsingFilename() );
                
                // Find the final slash.
                char* pLastSlash = dStrrchr( parsingFileBuffer, '/' );

                // Is this the last slash?
                if ( pLastSlash == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Failed to parse the loose-file path '%s' in asset file '%s'.", pAssetLooseFile, parsingFileBuffer );
                    return true;
                }

                // Remove parsing file.
                *pLastSlash = 0;

                // Format expanded path taking into account any missing slash.
                dSprintf( assetFilePathBuffer, sizeof(assetFilePathBuffer), "%s/%s", parsingFileBuffer, pAssetLooseFile + (pAssetLooseFile[1] == '/' ? 2 : 1 ) );               
            }
            else
            {
                // No, so expand the path in the usual way.
                Con::expandPath( assetFilePathBuffer, sizeof(assetFilePathBuffer), pAssetLooseFile );
            }

            // Insert asset loose-file.
            mAssetLooseFiles.push_back( StringTable->insert( assetFilePathBuffer ) );
        }

        return true;
    }

public:
    TamlAssetDeclaredVisitor() { mAssetDefinition.reset(); }
    virtual ~TamlAssetDeclaredVisitor() {}

    bool parse( const char* pFilename )
    {
        TamlXmlParser parser;
        return parser.parse( pFilename, *this, false );
    }

    typedef StringTableEntry typeAssetId;
    typedef Vector<typeAssetId> typeAssetIdVector;
    typedef Vector<StringTableEntry> typeLooseFileVector;

    inline AssetDefinition& getAssetDefinition( void ) { return mAssetDefinition; }
    inline typeAssetIdVector& getAssetDependencies( void ) { return mAssetDependencies; }
    inline typeLooseFileVector& getAssetLooseFiles( void ) { return mAssetLooseFiles; }

    void clear( void ) { mAssetDefinition.reset(); mAssetDependencies.clear(); mAssetLooseFiles.clear(); }

private:
    AssetDefinition         mAssetDefinition;
    typeAssetIdVector       mAssetDependencies;
    typeLooseFileVector     mAssetLooseFiles;
};

#endif // _TAML_ASSET_DECLARED_VISITOR_H_
