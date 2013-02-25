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

        // Finish because we're a reference to another object.
        return pElement;
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

//-----------------------------------------------------------------------------

void TamlXmlWriter::compileCustomElements( TiXmlElement* pXmlElement, const TamlWriteNode* pTamlWriteNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(TamlXmlWriter_CompileCustomElements);

    // Fetch custom nodes.
    const TamlCustomNodes& customNodes = pTamlWriteNode->mCustomNodes;

    // Fetch custom nodes.
    const TamlCustomNodeVector& nodes = customNodes.getNodes();

    // Finish if no custom nodes to process.
    if ( nodes.size() == 0 )
        return;

    // Iterate custom properties.
    for( TamlCustomNodeVector::const_iterator customNodesItr = nodes.begin(); customNodesItr != nodes.end(); ++customNodesItr )
    {
        // Fetch the custom node.
        TamlCustomNode* pCustomNode = *customNodesItr;

        // Format extended element name.
        char extendedElementNameBuffer[256];
        dSprintf( extendedElementNameBuffer, sizeof(extendedElementNameBuffer), "%s.%s", pXmlElement->Value(), pCustomNode->mNodeName );
        StringTableEntry extendedElementName = StringTable->insert( extendedElementNameBuffer );

        // Create element.
        TiXmlElement* pExtendedPropertyElement = new TiXmlElement( extendedElementName );

        // Fetch node children.
        const TamlCustomNodeVector& nodeChildren = pCustomNode->getChildren();

        // Iterate children nodes.
        for( TamlCustomNodeVector::const_iterator childNodeItr = nodeChildren.begin(); childNodeItr != nodeChildren.end(); ++childNodeItr )
        {
            // Fetch child node.
            const TamlCustomNode* pChildNode = *childNodeItr;

            // Compile the custom nodes.
            compileCustomNodes( pExtendedPropertyElement, pChildNode );
        }

        // Finish if the node is set to ignore if empty and it is empty.
        if ( pCustomNode->mIgnoreEmpty && pExtendedPropertyElement->NoChildren() )
        {
            // Yes, so delete the extended element.
            delete pExtendedPropertyElement;
            pExtendedPropertyElement = NULL;
        }
        else
        {
            // No, so add elementt as child.
            pXmlElement->LinkEndChild( pExtendedPropertyElement );
        }
    }
}

//-----------------------------------------------------------------------------

void TamlXmlWriter::compileCustomNodes( TiXmlElement* pXmlElement, const TamlCustomNode* pCustomNode )
{
    // Finish if the node is set to ignore if empty and it is empty.
    if ( pCustomNode->mIgnoreEmpty && pCustomNode->isEmpty() )
        return;

    // Create element.
    TiXmlElement* pNodeElement = new TiXmlElement( pCustomNode->mNodeName );

    // Fetch node children.
    const TamlCustomNodeVector& nodeChildren = pCustomNode->getChildren();

    // Iterate children nodes.
    for( TamlCustomNodeVector::const_iterator childNodeItr = nodeChildren.begin(); childNodeItr != nodeChildren.end(); ++childNodeItr )
    {
        // Fetch child node.
        const TamlCustomNode* pChildNode = *childNodeItr;

        // Is the node a proxy object?
        if ( pChildNode->isProxyObject() )
        {
            // Yes, so write the proxy object.
            pNodeElement->LinkEndChild( compileElement( pChildNode->getProxyWriteNode() ) );
        }
        else
        {
            // No, so compile the child nodes.
            compileCustomNodes( pNodeElement, pChildNode );
        }
    }

    // Fetch node fields.
    const TamlCustomFieldVector& nodeFields = pCustomNode->getFields();

    // Iterate property fields.
    for ( TamlCustomFieldVector::const_iterator nodeFieldItr = nodeFields.begin(); nodeFieldItr != nodeFields.end(); ++nodeFieldItr )
    {
        // Fetch node field.
        const TamlCustomNodeField* pNodeField = *nodeFieldItr;

        // Set field.
        pNodeElement->SetAttribute( pNodeField->getFieldName(), pNodeField->getFieldValue() );
    }

    // Finish if the node is set to ignore if empty and it is empty (including fields).
    if ( pCustomNode->mIgnoreEmpty && nodeFields.size() == 0 && pNodeElement->NoChildren() )
    {
        // Yes, so delete the extended element.
        delete pNodeElement;
        pNodeElement = NULL;
    }
    else
    {
        // Add node element as child.
        pXmlElement->LinkEndChild( pNodeElement );
    }
}
