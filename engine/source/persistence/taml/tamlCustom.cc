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

#include "persistence/taml/tamlCustom.h"

#ifndef _TAML_WRITE_NODE_H_
#include "persistence/taml/tamlWriteNode.h"
#endif

//-----------------------------------------------------------------------------

void TamlPropertyField::resetState( void )
{
    mFieldName = StringTable->EmptyString;
    *mFieldValue = 0;

    // We don't need to delete the write node as it'll get destroyed when the compilation is reset!
    mpFieldWriteNode = NULL;
    mpFieldObject = NULL;
}

//-----------------------------------------------------------------------------

void TamlPropertyField::set( const char* pFieldName, const char* pFieldValue )
{
    // Sanity!
    AssertFatal( pFieldName != NULL, "Field name cannot be NULL." );
    AssertFatal( pFieldValue != NULL, "Field value cannot be NULL." );

    // Set field name.
    mFieldName = StringTable->insert( pFieldName );

#if TORQUE_DEBUG
    // Is the field value too big?
    if ( dStrlen(pFieldValue) >= sizeof(mFieldValue) )
    {
        // Yes, so warn!
        Con::warnf( "Taml property field '%s' has a value that exceeds then maximum length: '%s'", pFieldName, pFieldValue );
        AssertFatal( false, "Field value is too big!" );
        return;
    }
#endif
    // Copy field value.
    dStrcpy( mFieldValue, pFieldValue );

    // Reset field object.
    mpFieldObject = NULL;
    SAFE_DELETE( mpFieldWriteNode );
}

//-----------------------------------------------------------------------------

void TamlPropertyField::set( const char* pFieldName, SimObject* pFieldObject )
{
    // Sanity!
    AssertFatal( pFieldName != NULL, "Field name cannot be NULL." );
    AssertFatal( pFieldObject != NULL, "Field object cannot be NULL." );
    AssertFatal( mpFieldWriteNode == NULL, "Field write node must be NULL." );

    // Set field name.
    mFieldName = StringTable->insert( pFieldName );

    mpFieldObject = pFieldObject;
    SAFE_DELETE( mpFieldWriteNode );

    // Reset field value.
    *mFieldValue = 0;
}

//-----------------------------------------------------------------------------

void TamlPropertyField::setWriteNode( TamlWriteNode* pWriteNode )
{
    // Sanity!
    AssertFatal( mFieldName != StringTable->EmptyString, "Cannot set write node with an empty field name." );
    AssertFatal( pWriteNode != NULL, "Write node cannot be NULL." );
    AssertFatal( pWriteNode->mpSimObject == mpFieldObject, "Write node does not match existing field object." );
    AssertFatal( mpFieldWriteNode == NULL, "Field write node must be NULL." );

    // Set field object.
    mpFieldWriteNode = pWriteNode;

    // Reset field value.
    *mFieldValue = 0;
}

//-----------------------------------------------------------------------------

SimObject* TamlPropertyField::getFieldObject( void ) const
{
    return mpFieldObject != NULL ? mpFieldObject : NULL;
}

//-----------------------------------------------------------------------------

bool TamlPropertyField::isObjectField( void ) const
{
    return mpFieldObject != NULL;
}

