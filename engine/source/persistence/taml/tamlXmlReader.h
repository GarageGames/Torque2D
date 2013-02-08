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

#ifndef _TAML_XMLREADER_H_
#define _TAML_XMLREADER_H_

#ifndef _HASHTABLE_H
#include "collection/hashTable.h"
#endif

#ifndef _TAML_H_
#include "persistence/taml/taml.h"
#endif

#ifndef TINYXML_INCLUDED
#include "persistence/tinyXML/tinyxml.h"
#endif

//-----------------------------------------------------------------------------

class TamlXmlReader
{
public:
    TamlXmlReader( Taml* pTaml ) :
        mpTaml( pTaml )
    {
        mTamlRefId      = StringTable->insert( TAML_ID_ATTRIBUTE_NAME );
        mTamlRefToId    = StringTable->insert( TAML_REFID_ATTRIBUTE_NAME );
        mTamlRefField   = StringTable->insert( TAML_REF_FIELD_NAME );
        mTamlObjectName = StringTable->insert( TAML_OBJECTNAME_ATTRIBUTE_NAME );
    }

    virtual ~TamlXmlReader() {}

    /// Read.
    SimObject* read( FileStream& stream );

private:
    Taml*               mpTaml;
    StringTableEntry    mTamlRefId;
    StringTableEntry    mTamlRefToId;
    StringTableEntry    mTamlObjectName;
    StringTableEntry    mTamlRefField;

    typedef HashMap<SimObjectId, SimObject*> typeObjectReferenceHash;

    typeObjectReferenceHash mObjectReferenceMap;

private:
    void resetParse( void );

    SimObject* parseElement( TiXmlElement* pXmlElement );
    void parseCustomElement( TiXmlElement* pXmlElement, TamlCustomProperties& customProperties );
    void parseAttributes( TiXmlElement* pXmlElement, SimObject* pSimObject );

    U32 getTamlRefId( TiXmlElement* pXmlElement );
    U32 getTamlRefToId( TiXmlElement* pXmlElement );
    const char* getTamlObjectName( TiXmlElement* pXmlElement );   
    const char* getTamlRefField( TiXmlElement* pXmlElement );
};

#endif // _TAML_XMLREADER_H_