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

#include "persistence/taml/tamlXmlWriter.h"

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

bool TamlXmlWriter::write( FileStream& stream, const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlXmlWriter_Write);

    // Create document.
    TiXmlDocument xmlDocument;

    // Compile root element.
    xmlDocument.LinkEndChild( compileElement( pTamlWriteNode ) );

    // Save document to stream.
    return xmlDocument.SaveFile( stream );
}

//-----------------------------------------------------------------------------

TiXmlNode* TamlXmlWriter::compileElement( const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlXmlWriter_CompileElement);

    // Fetch object.
    SimObject* pSimObject = pTamlWriteNode->mpSimObject;

    // Fetch element name.
    const char* pElementName = pSimObject->getClassName();

    // Create element.
    TiXmlElement* pElement = new TiXmlElement( pElementName );

    // Fetch reference Id.
    const U32 referenceId = pTamlWriteNode->mRefId;

    // Do we have a reference Id?
    if ( referenceId != 0 )
    {
        // Yes, so set reference Id attribute.
        pElement->SetAttribute( TAML_ID_ATTRIBUTE_NAME, referenceId );
    }

    // Do we have a reference to node?
    else if ( pTamlWriteNode->mRefToNode != NULL )
    {
        // Yes, so fetch reference to Id.
        const U32 referenceToId = pTamlWriteNode->mRefToNode->mRefId;

        // Sanity!
        AssertFatal( referenceToId != 0, "Taml: Invalid reference to Id." );

        // Set reference to Id attribute.
        pElement->SetAttribute( TAML_REFID_ATTRIBUTE_NAME, referenceToId );

        // Do we have a reference field?
        if ( pTamlWriteNode->mRefField != StringTable->EmptyString )
        {
            // Yes, so set attribute.
            pElement->SetAttribute( TAML_REF_FIELD_NAME, pTamlWriteNode->mRefField );
        }

        // Finish because we're a reference to another object.
        return pElement;
    }

    // Do we have a reference field?
    if ( pTamlWriteNode->mRefField != StringTable->EmptyString )
    {
        // Yes, so set attribute.
        pElement->SetAttribute( TAML_REF_FIELD_NAME, pTamlWriteNode->mRefField );
    }

    // Fetch object name.
    const char* pObjectName = pTamlWriteNode->mpObjectName;

    // Do we have a name?
    if ( pObjectName != NULL )
    {
        // Yes, so set name attribute.
        pElement->SetAttribute( TAML_OBJECTNAME_ATTRIBUTE_NAME, pObjectName );
    }

    // Compile attributes.
    compileAttributes( pElement, pTamlWriteNode );

    // Fetch children.
    Vector<TamlWriteNode*>* pChildren = pTamlWriteNode->mChildren;

    // Do we have any children?
    if ( pChildren )
    {
        // Yes, so iterate children.
        for( Vector<TamlWriteNode*>::iterator itr = pChildren->begin(); itr != pChildren->end(); ++itr )
        {
            // Write child element.
            pElement->LinkEndChild( compileElement( (*itr) ) );
        }
    }

    // Compile custom elements.
    compileCustomElements( pElement, pTamlWriteNode );

    return pElement;
}

//-----------------------------------------------------------------------------

void TamlXmlWriter::compileCustomElements( TiXmlElement* pXmlElement, const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlXmlWriter_CompileCustomElements);

    // Fetch custom properties.
    const TamlCustomProperties& customProperties = pTamlWriteNode->mCustomProperties;

    // Finish if no custom properties exist.
    if ( customProperties.size() == 0 )
        return;

    // Iterate custom properties.
    for( TamlCustomProperties::const_iterator customPropertyItr = customProperties.begin(); customPropertyItr != customProperties.end(); ++customPropertyItr )
    {
        // Fetch custom property.
        TamlCustomProperty* pCustomProperty = *customPropertyItr;

        // Format extended element name.
        char extendedElementNameBuffer[256];
        dSprintf( extendedElementNameBuffer, sizeof(extendedElementNameBuffer), "%s.%s", pXmlElement->Value(), pCustomProperty->mPropertyName );
        StringTableEntry extendedElementName = StringTable->insert( extendedElementNameBuffer );

        // Create element.
        TiXmlElement* pExtendedPropertyElement = new TiXmlElement( extendedElementName );

        // Iterate property alias.
        for( TamlCustomProperty::const_iterator propertyAliasItr = pCustomProperty->begin(); propertyAliasItr != pCustomProperty->end(); ++propertyAliasItr )
        {
            // Fetch property alias.
            TamlPropertyAlias* pPropertyAlias = *propertyAliasItr;

            // Skip if the alias is set to ignore no properties and there are none.
            if ( pPropertyAlias->mIgnoreEmpty && pPropertyAlias->size() == 0 )
                continue;

            // Create element.
            TiXmlElement* pPropertyElement = new TiXmlElement( pPropertyAlias->mAliasName );

            // Iterate property fields.
            for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
            {
                // Fetch property field.
                TamlPropertyField* pPropertyField = *propertyFieldItr;

                // Is it an object field?
                if ( pPropertyField->isObjectField() )
                {
                    // Yes, so write child element.
                    pPropertyElement->LinkEndChild( compileElement( pPropertyField->getWriteNode() ) );
                }
                else
                {
                    // No, so set property attribute.
                    pPropertyElement->SetAttribute( pPropertyField->getFieldName(), pPropertyField->getFieldValue() );
                }
            }

            // Write property element as child.
            pExtendedPropertyElement->LinkEndChild( pPropertyElement );
        }

        // Is the custom property set to ignore no alias' and there are none.
        if ( pCustomProperty->mIgnoreEmpty && pExtendedPropertyElement->NoChildren() )
        {
            // Yes, so delete the extended element.
            delete pExtendedPropertyElement;
            pExtendedPropertyElement = NULL;
        }
        else
        {
            // No, so write the extended property element as child.
            pXmlElement->LinkEndChild( pExtendedPropertyElement );
        }
    }
}

//-----------------------------------------------------------------------------

void TamlXmlWriter::compileAttributes( TiXmlElement* pXmlElement, const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlXmlWriter_CompileAttributes);

    // Fetch fields.
    const Vector<TamlWriteNode::FieldValuePair*>& fields = pTamlWriteNode->mFields;

    // Ignore if no fields.
    if ( fields.size() == 0 )
        return;

    // Iterate fields.
    for( Vector<TamlWriteNode::FieldValuePair*>::const_iterator itr = fields.begin(); itr != fields.end(); ++itr )
    {
        // Fetch field/value pair.
        TamlWriteNode::FieldValuePair* pFieldValue = (*itr);

        // Set field attribute.
        pXmlElement->SetAttribute( pFieldValue->mName, pFieldValue->mpValue );
    }
}