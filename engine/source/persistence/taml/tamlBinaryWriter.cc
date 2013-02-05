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

#include "persistence/taml/tamlBinaryWriter.h"

#ifndef _ZIPSUBSTREAM_H_
#include "io/zip/zipSubStream.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

bool TamlBinaryWriter::write( FileStream& stream, const TamlWriteNode* pTamlWriteNode, const bool compressed )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryWriter_Write);
 
    // Write Taml signature.
    stream.writeString( StringTable->insert( TAML_SIGNATURE ) );

    // Write version Id.
    stream.write( mVersionId );

    // Write compressed flag.
    stream.write( compressed );

    // Are we compressed?
    if ( compressed )
    {
        // yes, so attach zip stream.
        ZipSubWStream zipStream;
        zipStream.attachStream( &stream );

        // Write element.
        writeElement( zipStream, pTamlWriteNode );

        // Detach zip stream.
        zipStream.detachStream();
    }
    else
    {
        // No, so write element.
        writeElement( stream, pTamlWriteNode );
    }

    return true;
}

//-----------------------------------------------------------------------------

void TamlBinaryWriter::writeElement( Stream& stream, const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryWriter_WriteElement);

    // Fetch object.
    SimObject* pSimObject = pTamlWriteNode->mpSimObject;

    // Fetch element name.
    const char* pElementName = pSimObject->getClassName();

    // Write element name.
    stream.writeString( pElementName );

    // Fetch object name.
    const char* pObjectName = pTamlWriteNode->mpObjectName;

    // Write object name.
    stream.writeString( pObjectName != NULL ? pObjectName : StringTable->EmptyString );

    // Fetch reference Id.
    const U32 tamlRefId = pTamlWriteNode->mRefId;

    // Write reference Id.
    stream.write( tamlRefId );

    // Do we have a reference to node?
    if ( pTamlWriteNode->mRefToNode != NULL )
    {
        // Yes, so fetch reference to Id.
        const U32 tamlRefToId = pTamlWriteNode->mRefToNode->mRefId;

        // Sanity!
        AssertFatal( tamlRefToId != 0, "Taml: Invalid reference to Id." );

        // Write reference to Id.
        stream.write( tamlRefToId );

        // Finished.
        return;
    }

    // No, so write no reference to Id.
    stream.write( 0 );

    // Write attributes.
    writeAttributes( stream, pTamlWriteNode );

    // Write children.
    writeChildren( stream, pTamlWriteNode );

    // Write custom elements.
    writeCustomElements( stream, pTamlWriteNode );
}

//-----------------------------------------------------------------------------

void TamlBinaryWriter::writeCustomElements( Stream& stream, const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryWriter_WriteCustomElements);

    // Fetch custom properties.
    const TamlCustomProperties& customProperties = pTamlWriteNode->mCustomProperties;

    // Write custom element count.
    stream.write( (U32)customProperties.size() );

    // Iterate custom properties.
    for( TamlCustomProperties::const_iterator customPropertyItr = customProperties.begin(); customPropertyItr != customProperties.end(); ++customPropertyItr )
    {
        // Fetch custom property.
        TamlCustomProperty* pCustomProperty = *customPropertyItr;

        // Write custom element name.
        stream.writeString( pCustomProperty->mPropertyName );

        // Fetch property alias count.
        U32 propertyAliasCount = (U32)pCustomProperty->size();

        // Write property count.
        stream.write( propertyAliasCount );

        // Skip if no property alias.
        if (propertyAliasCount == 0 )
            continue;

        // Iterate property alias.
        for( TamlCustomProperty::const_iterator propertyAliasItr = pCustomProperty->begin(); propertyAliasItr != pCustomProperty->end(); ++propertyAliasItr )
        {
            // Fetch property alias.
            TamlPropertyAlias* pPropertyAlias = *propertyAliasItr;

            // Write property alias name.
            stream.writeString( pPropertyAlias->mAliasName );

            // Write property field count.
            stream.write( (U32)pPropertyAlias->size() );

            // Skip if no property fields.
            if ( pPropertyAlias->size() == 0 )
                continue;

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Fetch object field flag,
                const bool isObjectField = pPropertyField->isObjectField();

                // Write flag.
                stream.write( isObjectField );

                // Is it an object field?
                if ( isObjectField )
                {
                    // Yes, so fetch write node.
                    const TamlWriteNode* pObjectWriteField = pPropertyField->getWriteNode();

                    // Write reference field.
                    stream.writeString( pObjectWriteField->mRefField );

                    // Write field object.
                    writeElement( stream, pObjectWriteField );
                }
                else
                {
                    // No, so write property attribute.
                    stream.writeString( pPropertyField->getFieldName() );
                    stream.writeLongString( MAX_TAML_PROPERTY_FIELDVALUE_LENGTH, pPropertyField->getFieldValue() );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void TamlBinaryWriter::writeAttributes( Stream& stream, const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryWriter_WriteAttributes);

    // Fetch fields.
    const Vector<TamlWriteNode::FieldValuePair*>& fields = pTamlWriteNode->mFields;

    // Write placeholder attribute count.
    stream.write( (U32)fields.size() );

    // Finish if no fields.
    if ( fields.size() == 0 )
        return;

    // Iterate fields.
    for( Vector<TamlWriteNode::FieldValuePair*>::const_iterator itr = fields.begin(); itr != fields.end(); ++itr )
    {
        // Fetch field/value pair.
        TamlWriteNode::FieldValuePair* pFieldValue = (*itr);

        // Write attribute.
        stream.writeString( pFieldValue->mName );
        stream.writeLongString( 4096, pFieldValue->mpValue );
    }
}

void TamlBinaryWriter::writeChildren( Stream& stream, const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlBinaryWriter_WriteChildren);

    // Fetch children.
    Vector<TamlWriteNode*>* pChildren = pTamlWriteNode->mChildren;

    // Do we have any children?
    if ( pChildren == NULL )
    {
        // No, so write no children.
        stream.write( (U32)0 );
        return;
    }

    // Write children count.
    stream.write( (U32)pChildren->size() );

    // Iterate children.
    for( Vector<TamlWriteNode*>::iterator itr = pChildren->begin(); itr != pChildren->end(); ++itr )
    {
        // Write child.
        writeElement( stream, (*itr) );
    }
}