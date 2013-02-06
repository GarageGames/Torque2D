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

#include "persistence/taml/tamlBinaryReader.h"

#ifndef _ZIPSUBSTREAM_H_
#include "io/zip/zipSubStream.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

SimObject* TamlBinaryReader::read( FileStream& stream )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryReader_Read);

    // Read Taml signature.
    StringTableEntry tamlSignature = stream.readSTString();

    // Is the signature correct?
    if ( tamlSignature != StringTable->insert( TAML_SIGNATURE ) )
    {
        // Warn.
        Con::warnf("Taml: Cannot read binary file as signature is incorrect '%s'.", tamlSignature );
        return NULL;
    }

    // Read version Id.
    U32 versionId;
    stream.read( &versionId );

    // Read compressed flag.
    bool compressed;
    stream.read( &compressed );

    SimObject* pSimObject = NULL;

    // Is the stream compressed?
    if ( compressed )
    {
        // Yes, so attach zip stream.
        ZipSubRStream zipStream;
        zipStream.attachStream( &stream );

        // Parse element.
        pSimObject = parseElement( zipStream, versionId );

        // Detach zip stream.
        zipStream.detachStream();
    }
    else
    {
        // No, so parse element.
        pSimObject = parseElement( stream, versionId );
    }

    return pSimObject;
}

//-----------------------------------------------------------------------------

void TamlBinaryReader::resetParse( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryReader_ResetParse);

    // Clear object reference map.
    mObjectReferenceMap.clear();
}

//-----------------------------------------------------------------------------

SimObject* TamlBinaryReader::parseElement( Stream& stream, const U32 versionId )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryReader_ParseElement);

    SimObject* pSimObject = NULL;

#ifdef TORQUE_DEBUG
    // Format the type location.
    char typeLocationBuffer[64];
    dSprintf( typeLocationBuffer, sizeof(typeLocationBuffer), "Taml [format='binary' offset=%u]", stream.getPosition() );
#endif

    // Fetch element name.    
    StringTableEntry typeName = stream.readSTString();

    // Fetch object name.
    StringTableEntry objectName = stream.readSTString();

    // Read references.
    U32 tamlRefId;
    U32 tamlRefToId;
    stream.read( &tamlRefId );
    stream.read( &tamlRefToId );

    // Do we have a reference to Id?
    if ( tamlRefToId != 0 )
    {
        // Yes, so fetch reference.
        typeObjectReferenceHash::iterator referenceItr = mObjectReferenceMap.find( tamlRefToId );

        // Did we find the reference?
        if ( referenceItr == mObjectReferenceMap.end() )
        {
            // No, so warn.
            Con::warnf( "Taml: Could not find a reference Id of '%d'", tamlRefToId );
            return NULL;
        }

        // Return object.
        return referenceItr->value;
    }

#ifdef TORQUE_DEBUG
    // Create type.
    pSimObject = Taml::createType( typeName, mpTaml, typeLocationBuffer );
#else
    // Create type.
    pSimObject = Taml::createType( typeName, mpTaml );
#endif

    // Finish if we couldn't create the type.
    if ( pSimObject == NULL )
        return NULL;

    // Find Taml callbacks.
    TamlCallbacks* pCallbacks = dynamic_cast<TamlCallbacks*>( pSimObject );

    // Are there any Taml callbacks?
    if ( pCallbacks != NULL )
    {
        // Yes, so call it.
        mpTaml->tamlPreRead( pCallbacks );
    }

    // Parse attributes.
    parseAttributes( stream, pSimObject, versionId );

    // Does the object require a name?
    if ( objectName == StringTable->EmptyString )
    {
        // No, so just register anonymously.
        pSimObject->registerObject();
    }
    else
    {
        // Yes, so register a named object.
        pSimObject->registerObject( objectName );

        // Was the name assigned?
        if ( pSimObject->getName() != objectName )
        {
            // No, so warn that the name was rejected.
#ifdef TORQUE_DEBUG
            Con::warnf( "Taml::parseElement() - Registered an instance of type '%s' but a request to name it '%s' was rejected.  This is typically because an object of that name already exists.  '%s'", typeName, objectName, typeLocationBuffer );
#else
            Con::warnf( "Taml::parseElement() - Registered an instance of type '%s' but a request to name it '%s' was rejected.  This is typically because an object of that name already exists.", typeName, objectName );
#endif
        }
    }

    // Do we have a reference Id?
    if ( tamlRefId != 0 )
    {
        // Yes, so insert reference.
        mObjectReferenceMap.insert( tamlRefId, pSimObject );
    }

    // Parse custom elements.
    TamlCustomProperties customProperties;

    // Parse children.
    parseChildren( stream, pCallbacks, pSimObject, versionId );

    // Parse custom elements.
    parseCustomElements( stream, pCallbacks, customProperties, versionId );

    // Are there any Taml callbacks?
    if ( pCallbacks != NULL )
    {
        // Yes, so call it.
        mpTaml->tamlPostRead( pCallbacks, customProperties );
    }

    // Return object.
    return pSimObject;
}

//-----------------------------------------------------------------------------

void TamlBinaryReader::parseCustomElements( Stream& stream, TamlCallbacks* pCallbacks, TamlCustomProperties& customProperties, const U32 versionId )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryReader_ParseCustomElement);

    // Read custom element count.
    U32 customPropertyCount;
    stream.read( &customPropertyCount );

    // Finish if no custom properties.
    if ( customPropertyCount == 0 )
        return;

    // Iterate custom properties.
    for ( U32 propertyIndex = 0; propertyIndex < customPropertyCount; ++propertyIndex )
    {
        // Read custom element name.
        StringTableEntry propertyName = stream.readSTString();

        // Add custom property.
        TamlCustomProperty* pCustomProperty = customProperties.addProperty( propertyName );

        // Read property alias count.
        U32 propertyAliasCount;
        stream.read( &propertyAliasCount );

        // Skip if no property alias.
        if ( propertyAliasCount == 0 )
            continue;

        // Iterate property alias.
        for( U32 propertyAliasIndex = 0; propertyAliasIndex < propertyAliasCount; ++propertyAliasIndex )
        {
            // Read property alias name.
            StringTableEntry propertyAliasName = stream.readSTString();

            // Add property alias.
            TamlPropertyAlias* pPropertyAlias = pCustomProperty->addAlias( propertyAliasName );

            // Read property field count.
            U32 propertyFieldCount;
            stream.read( &propertyFieldCount );

            // Skip if no property fields.
            if ( propertyFieldCount == 0 )
                continue;

            // Iterate property fields.
            for( U32 propertyFieldIndex = 0; propertyFieldIndex < propertyFieldCount; ++propertyFieldIndex )
            {
                // Read is object field flag.
                bool isObjectField;
                stream.read( &isObjectField );

                // Is it an object field?
                if ( isObjectField )
                {
                    // Yes, so read reference field.
                    StringTableEntry fieldName = stream.readSTString();

                    // Read field object.
                    SimObject* pFieldObject = parseElement( stream, versionId );

                    // Add property field.
                    pPropertyAlias->addField( fieldName, pFieldObject );
                }
                else
                {
                    // No, so read field name.
                    StringTableEntry propertyFieldName = stream.readSTString();

                    // Read field value.
                    char valueBuffer[MAX_TAML_PROPERTY_FIELDVALUE_LENGTH];
                    stream.readLongString( MAX_TAML_PROPERTY_FIELDVALUE_LENGTH, valueBuffer );

                    // Add property field.
                    pPropertyAlias->addField( propertyFieldName, valueBuffer );
                }
            }
        }
    }

    // Do we have callbacks?
    if ( pCallbacks == NULL )
    {
        // No, so warn.
        Con::warnf( "Taml: Encountered custom data but object does not support custom data." );
        return;
    }

    // Custom read callback.
    mpTaml->tamlCustomRead( pCallbacks, customProperties );
}

//-----------------------------------------------------------------------------

void TamlBinaryReader::parseAttributes( Stream& stream, SimObject* pSimObject, const U32 versionId )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryReader_ParseAttributes);

    // Sanity!
    AssertFatal( pSimObject != NULL, "Taml: Cannot parse attributes on a NULL object." );

    // Fetch attribute count.
    U32 attributeCount;
    stream.read( &attributeCount );

    // Finish if no attributes.
    if ( attributeCount == 0 )
        return;

    char valueBuffer[4096];

    // Iterate attributes.
    for ( U32 index = 0; index < attributeCount; ++index )
    {
        // Fetch attribute.
        StringTableEntry attributeName = stream.readSTString();
        stream.readLongString( 4096, valueBuffer );

        // We can assume this is a field for now.
        pSimObject->setPrefixedDataField( attributeName, NULL, valueBuffer );
    }
}

//-----------------------------------------------------------------------------

void TamlBinaryReader::parseChildren( Stream& stream, TamlCallbacks* pCallbacks, SimObject* pSimObject, const U32 versionId )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryReader_ParseChildren);

    // Sanity!
    AssertFatal( pSimObject != NULL, "Taml: Cannot parse children on a NULL object." );

    // Fetch children count.
    U32 childrenCount;
    stream.read( &childrenCount );

    // Finish if no children.
    if ( childrenCount == 0 )
        return;

    // Fetch the Taml children.
    TamlChildren* pChildren = dynamic_cast<TamlChildren*>( pSimObject );

    // Is this a sim set?
    if ( pChildren == NULL )
    {
        // No, so warn.
        Con::warnf("Taml: Child element found under parent but object cannot have children." );
        return;
    }

    // Iterate children.
    for ( U32 index = 0; index < childrenCount; ++ index )
    {
        // Parse child element.
        SimObject* pChildSimObject = parseElement( stream, versionId );

        // Finish if child failed.
        if ( pChildSimObject == NULL )
            return;

        // Add child.
        pChildren->addTamlChild( pChildSimObject );

        // Find Taml callbacks for child.
        TamlCallbacks* pChildCallbacks = dynamic_cast<TamlCallbacks*>( pChildSimObject );

        // Do we have callbacks on the child?
        if ( pChildCallbacks != NULL )
        {
            // Yes, so perform callback.
            mpTaml->tamlAddParent( pChildCallbacks, pSimObject );
        }
    }
}
