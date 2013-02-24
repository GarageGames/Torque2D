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

#ifndef _TAML_CUSTOM_H_
#define _TAML_CUSTOM_H_

#ifndef _FACTORY_CACHE_H_
#include "memory/factoryCache.h"
#endif

#ifndef _STRINGTABLE_H_
#include "string/stringTable.h"
#endif

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef B2_MATH_H
#include "box2d/Common/b2Math.h"
#endif

#ifndef _COLOR_H_
#include "graphics/color.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#include "memory/safeDelete.h"

//-----------------------------------------------------------------------------

#define MAX_TAML_NODE_FIELDVALUE_LENGTH 2048

//-----------------------------------------------------------------------------

class TamlWriteNode;

//-----------------------------------------------------------------------------

class TamlCustomNodeField : public IFactoryObjectReset
{
public:
    TamlCustomNodeField()
    {
        resetState();
    }

    virtual ~TamlCustomNodeField()
    {
        // Everything should already be cleared in a state reset.
        // Touching any memory here is dangerous as this type is typically
        // held in a static factory cache until shutdown at which point
        // pretty much anything or everything could be invalid!
    }

    virtual void resetState( void )
    {
        mFieldName = StringTable->EmptyString;
        *mFieldValue = 0;
    }

    void set( const char* pFieldName, const char* pFieldValue );

    inline void getFieldValue( ColorF& fieldValue ) const
    {
        fieldValue.set( 1.0f, 1.0f, 1.0f, 1.0f );

        // Set color.
        const char* argv = (char*)mFieldValue;
        Con::setData( TypeColorF, &fieldValue, 0, 1, &argv );
    }

    inline void getFieldValue( ColorI& fieldValue ) const
    {
        fieldValue.set( 255, 255, 255, 255 );

        // Set color.
        const char* argv = (char*)mFieldValue;
        Con::setData( TypeColorI, &fieldValue, 0, 1, &argv );
    }

    inline void getFieldValue( Point2I& fieldValue ) const
    {
        if ( dSscanf( mFieldValue, "%d %d", &fieldValue.x, &fieldValue.y ) != 2 )
        {
            // Warn.
            Con::warnf( "TamlCustomNodeField - Reading point2I but it has an incorrect format: '%s'.", mFieldValue );
        }
    }

    inline void getFieldValue( Point2F& fieldValue ) const
    {
        if ( dSscanf( mFieldValue, "%g %g", &fieldValue.x, &fieldValue.y ) != 2 )
        {
            // Warn.
            Con::warnf( "TamlCustomNodeField - Reading point2F but it has an incorrect format: '%s'.", mFieldValue );
        }
    }

    inline void getFieldValue( b2Vec2& fieldValue ) const
    {
        if ( dSscanf( mFieldValue, "%g %g", &fieldValue.x, &fieldValue.y ) != 2 )
        {
            // Warn.
            Con::warnf( "TamlCustomNodeField - Reading vector but it has an incorrect format: '%s'.", mFieldValue );
        }
    }

    inline void getFieldValue( bool& fieldValue ) const
    {
        fieldValue = dAtob( mFieldValue );
    }

    inline void getFieldValue( S32& fieldValue ) const
    {
        fieldValue = dAtoi( mFieldValue );
    }

    inline void getFieldValue( U32& fieldValue ) const
    {
        fieldValue = (U32)dAtoi( mFieldValue );
    }

    inline void getFieldValue( F32& fieldValue ) const
    {
        fieldValue = dAtof( mFieldValue );
    }

    inline const char* getFieldValue( void ) const
    {
        return mFieldValue;
    }

    inline StringTableEntry getFieldName( void ) const { return mFieldName; }

    bool fieldNameBeginsWith( const char* pComparison )
    {
        const U32 comparisonLength = dStrlen( pComparison );
        const U32 fieldNameLength = dStrlen( mFieldName );

        if ( comparisonLength == 0 || fieldNameLength == 0 || comparisonLength > fieldNameLength )
            return false;

        StringTableEntry comparison = StringTable->insert( pComparison );

        char fieldNameBuffer[1024];

        // Sanity!
        AssertFatal( fieldNameLength < sizeof(fieldNameBuffer), "TamlCustomNodeField: Field name is too long." );

        dStrcpy( fieldNameBuffer, mFieldName );
        fieldNameBuffer[fieldNameLength-1] = 0;
        StringTableEntry fieldName = StringTable->insert( fieldNameBuffer );

        return ( fieldName == comparison );
    }

private:
    StringTableEntry    mFieldName;
    char                mFieldValue[MAX_TAML_NODE_FIELDVALUE_LENGTH];
};

static FactoryCache<TamlCustomNodeField> TamlCustomNodeFieldFactory;
typedef Vector<TamlCustomNodeField*> TamlCustomFieldVector;

//-----------------------------------------------------------------------------

class TamlCustomNode : public IFactoryObjectReset
{
public:
    TamlCustomNode()
    {
        // Reset proxy object.
        // NOTE: This MUST be done before the state is reset otherwise we'll be touching uninitialized stuff.
        mpProxyWriteNode = NULL;
        mpProxyObject = NULL;

        resetState();
    }

    virtual ~TamlCustomNode()
    {
        // Everything should already be cleared in a state reset.
        // Touching any memory here is dangerous as this type is typically
        // held in a static factory cache until shutdown at which point
        // pretty much anything or everything could be invalid!
    }

    virtual void resetState( void )
    {
        // We don't need to delete the write node as it'll get destroyed when the compilation is reset!
        mpProxyWriteNode = NULL;
        mpProxyObject = NULL;

        // Cache the children.
        while ( mChildren.size() > 0 )
        {
            TamlCustomNodeFactory.cacheObject( mChildren.back() );
            mChildren.pop_back();
        }

        // Cache the fields.
        while( mFields.size() > 0 )
        {
            TamlCustomNodeFieldFactory.cacheObject( mFields.back() );
            mFields.pop_back();
        }

        // Reset the node name.
        mNodeName = StringTable->EmptyString;

        // Reset the ignore empty flag.
        mIgnoreEmpty = false;
    }

    void set( const char* pNodeName )
    {
        // Sanity!
        AssertFatal( pNodeName != NULL, "Node name cannot be NULL." );

        mNodeName = StringTable->insert( pNodeName );
    }

    void set( const char* pNodeName, SimObject* pProxyObject );

    void setWriteNode( TamlWriteNode* pWriteNode );

    TamlCustomNode* addNode( const char* pNodeName, const bool ignoreEmpty = true )
    {
        // Create a custom node.
        TamlCustomNode* pCustomNode = TamlCustomNodeFactory.createObject();

        // Set node name.
        pCustomNode->set( pNodeName );

        // Set ignore-empty flag.
        pCustomNode->mIgnoreEmpty = ignoreEmpty;

        // Store node.
        mChildren.push_back( pCustomNode );

        return pCustomNode;
    }

    void removeNode( const U32 index )
    {
        // Sanity!
        AssertFatal( index < (U32)mChildren.size(), "tamlCustomNode::removeNode() - Index is out of bounds." );

        // Cache the custom node.
        TamlCustomNodeFactory.cacheObject( mChildren[index] );

        // Remove it.
        mChildren.erase( index );
    }

    const TamlCustomNode* findNode( const char* pNodeName ) const
    {
        // Sanity!
        AssertFatal( pNodeName != NULL, "Cannot find Taml node name that is NULL." );

        // Fetch node name.
        StringTableEntry nodeName = StringTable->insert( pNodeName );

        // Find node.
        for( Vector<TamlCustomNode*>::const_iterator nodeItr = mChildren.begin(); nodeItr != mChildren.end(); ++nodeItr )
        {
            if ( (*nodeItr)->mNodeName == nodeName )
                return (*nodeItr);
        }

        return NULL;
    }

    TamlCustomNodeField* addField( const char* pFieldName, const ColorI& fieldValue )
    {
        // Fetch the field value.
        const char* pFieldValue = Con::getData( TypeColorI, &const_cast<ColorI&>(fieldValue), 0 );

        // Did we get a field value?
        if ( pFieldValue == NULL )
        {
            // No, so warn.
            Con::warnf( "Taml: Failed to add node field name '%s' with ColorI value.", pFieldName );
            pFieldValue = StringTable->EmptyString;
        }

        return addField( pFieldName, pFieldValue );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const ColorF& fieldValue )
    {
        // Fetch the field value.
        const char* pFieldValue = Con::getData( TypeColorF, &const_cast<ColorF&>(fieldValue), 0 );

        // Did we get a field value?
        if ( pFieldValue == NULL )
        {
            // No, so warn.
            Con::warnf( "Taml: Failed to add node field name '%s' with ColorF value.", pFieldName );
            pFieldValue = StringTable->EmptyString;
        }

        return addField( pFieldName, pFieldValue );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const Point2I& fieldValue )
    {
        char fieldValueBuffer[32];
        dSprintf( fieldValueBuffer, sizeof(fieldValueBuffer), "%d %d", fieldValue.x, fieldValue.y );
        return addField( pFieldName, fieldValueBuffer );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const Point2F& fieldValue )
    {
        char fieldValueBuffer[32];
        dSprintf( fieldValueBuffer, sizeof(fieldValueBuffer), "%.5g %0.5g", fieldValue.x, fieldValue.y );
        return addField( pFieldName, fieldValueBuffer );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const b2Vec2& fieldValue )
    {
        char fieldValueBuffer[32];
        dSprintf( fieldValueBuffer, sizeof(fieldValueBuffer), "%.5g %.5g", fieldValue.x, fieldValue.y );
        return addField( pFieldName, fieldValueBuffer );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const U32 fieldValue )
    {
        char fieldValueBuffer[16];
        dSprintf( fieldValueBuffer, sizeof(fieldValueBuffer), "%d", fieldValue );
        return addField( pFieldName, fieldValueBuffer );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const bool fieldValue )
    {
        char fieldValueBuffer[16];
        dSprintf( fieldValueBuffer, sizeof(fieldValueBuffer), "%d", fieldValue );
        return addField( pFieldName, fieldValueBuffer );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const S32 fieldValue )
    {
        char fieldValueBuffer[16];
        dSprintf( fieldValueBuffer, sizeof(fieldValueBuffer), "%d", fieldValue );
        return addField( pFieldName, fieldValueBuffer );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const float fieldValue )
    {
        char fieldValueBuffer[16];
        dSprintf( fieldValueBuffer, sizeof(fieldValueBuffer), "%.5g", fieldValue );
        return addField( pFieldName, fieldValueBuffer );
    }

    TamlCustomNodeField* addField( const char* pFieldName, const char* pFieldValue )
    {
        // Create a node field.
        TamlCustomNodeField* pNodeField = TamlCustomNodeFieldFactory.createObject();

        // Set node field.
        pNodeField->set( pFieldName, pFieldValue );

#if TORQUE_DEBUG
        // Ensure a field name conflict does not exist.
        for( Vector<TamlCustomNodeField*>::iterator nodeFieldItr = mFields.begin(); nodeFieldItr != mFields.end(); ++nodeFieldItr )
        {
            // Skip if field name is not the same.
            if ( pNodeField->getFieldName() != (*nodeFieldItr)->getFieldName() )
                continue;

            // Warn!
            Con::warnf("Conflicting Taml node field name of '%s' in node '%s'.", pFieldName, mNodeName );

            // Cache node field.
            TamlCustomNodeFieldFactory.cacheObject( pNodeField );
            return NULL;
        }

        // Ensure the field value is not too long.
        if ( dStrlen( pFieldValue ) >= MAX_TAML_NODE_FIELDVALUE_LENGTH )
        {
            // Warn.
            Con::warnf("Taml field name '%s' has a field value that is too long (Max:%d): '%s'.",
                pFieldName,
                MAX_TAML_NODE_FIELDVALUE_LENGTH,
                pFieldValue );

            // Cache node field.
            TamlCustomNodeFieldFactory.cacheObject( pNodeField );
            return NULL;
        }
#endif
        // Store node field.
        mFields.push_back( pNodeField );

        return pNodeField;
    }

    TamlCustomNodeField* addField( const char* pFieldName, SimObject* pFieldObject )
    {
        // Create a node field.
        TamlCustomNodeField* pNodeField = TamlCustomNodeFieldFactory.createObject();

        // Set node field.
        pNodeField->set( pFieldName, pFieldObject );

#if TORQUE_DEBUG
        // Ensure a field name conflict does not exist.
        for( TamlCustomFieldVector::iterator nodeFieldItr = mFields.begin(); nodeFieldItr != mFields.end(); ++nodeFieldItr )
        {
            // Skip if field name is not the same.
            if ( pNodeField->getFieldName() != (*nodeFieldItr)->getFieldName() )
                continue;

            // Warn!
            Con::warnf("Conflicting Taml node field name of '%s' in property alias of '%s'.", pFieldName, mNodeName );

            // Cache node field.
            TamlCustomNodeFieldFactory.cacheObject( pNodeField );
            return NULL;
        }
#endif
        // Store node field.
        mFields.push_back( pNodeField );

        return pNodeField;
    }

    const TamlCustomNodeField* findField( const char* pFieldName ) const
    {
        // Sanity!
        AssertFatal( pFieldName != NULL, "Cannot find Taml field name that is NULL." );

        // Fetch field name.
        StringTableEntry fieldName = StringTable->insert( pFieldName );

        // Find node field.
        for( TamlCustomFieldVector::const_iterator fieldItr = mFields.begin(); fieldItr != mFields.end(); ++fieldItr )
        {
            if ( (*fieldItr)->getFieldName() == fieldName )
                return (*fieldItr);
        }

        return NULL;
    }

    inline bool isProxyObject( void ) const { return mpProxyObject != NULL; }
    SimObject* getProxyObject( void ) const { return mpProxyObject != NULL ? mpProxyObject : NULL; }
    inline const TamlWriteNode* getProxyWriteNode( void ) const { return mpProxyWriteNode; }



    const TamlCustomNodeVector& getChildren( void ) const { return mChildren; }
    const TamlCustomFieldVector& getFields( void ) const { return mFields; }

    StringTableEntry        mNodeName;
    Vector<TamlCustomNode*> mChildren;
    TamlCustomFieldVector   mFields;
    bool                    mIgnoreEmpty;

    SimObject*              mpProxyObject;
    TamlWriteNode*          mpProxyWriteNode;
};

static FactoryCache<TamlCustomNode> TamlCustomNodeFactory;
typedef Vector<TamlCustomNode*> TamlCustomNodeVector;

//-----------------------------------------------------------------------------

class TamlCustomNodes : public IFactoryObjectReset
{
public:
    TamlCustomNodes()
    {
    }

    virtual ~TamlCustomNodes()
    {
        resetState();
    }

    virtual void resetState( void )
    {
        // Cache the nodes.
        while ( mNodes.size() > 0 )
        {
            TamlCustomNodeFactory.cacheObject( mNodes.back() );
            mNodes.pop_back();
        }
    }

    TamlCustomNode* addNode( const char* pNodeName, const bool ignoreEmpty = true )
    {
        // Create a custom node.
        TamlCustomNode* pCustomNode = TamlCustomNodeFactory.createObject();

        // Set node name.
        pCustomNode->set( pNodeName );

        // Set ignore-empty flag.
        pCustomNode->mIgnoreEmpty = ignoreEmpty;

#if TORQUE_DEBUG
        // Ensure a node name conflict does not exist.
        for( TamlCustomNodeVector::iterator nodeItr = mNodes.begin(); nodeItr != mNodes.end(); ++nodeItr )
        {
            // Skip if node name is not the same.
            if ( pCustomNode->mNodeName != (*nodeItr)->mNodeName )
                continue;

            // Warn!
            Con::warnf("Conflicting Taml custom node name of '%s'.", pNodeName );

            // Cache node.
            TamlCustomNodeFactory.cacheObject( pCustomNode );
            return NULL;
        }
#endif
        // Store node.
        mNodes.push_back( pCustomNode );

        return pCustomNode;
    }

    void removeNode( const U32 index )
    {
        // Sanity!
        AssertFatal( index < (U32)mNodes.size(), "tamlCustomNode::removeNode() - Index is out of bounds." );

        // Cache the custom node.
        TamlCustomNodeFactory.cacheObject( mNodes[index] );

        // Remove it.
        mNodes.erase( index );
    }

    const TamlCustomNode* findNode( const char* pNodeName ) const
    {
        // Sanity!
        AssertFatal( pNodeName != NULL, "Cannot find Taml node name that is NULL." );

        // Fetch node name.
        StringTableEntry nodeName = StringTable->insert( pNodeName );

        // Find node.
        for( Vector<TamlCustomNode*>::const_iterator nodeItr = mNodes.begin(); nodeItr != mNodes.end(); ++nodeItr )
        {
            if ( (*nodeItr)->mNodeName == nodeName )
                return (*nodeItr);
        }

        return NULL;
    }

    const TamlCustomNodeVector& getNodes( void ) const { return mNodes; }

private:
    TamlCustomNodeVector mNodes;
};

#endif // _TAML_CUSTOM_H_