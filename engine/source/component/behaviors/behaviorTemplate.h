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

#ifndef _BEHAVIORTEMPLATE_H_
#define _BEHAVIORTEMPLATE_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _BEHAVIORINSTANCE_H_
#include "behaviorInstance.h"
#endif

//-----------------------------------------------------------------------------

class BehaviorTemplate : public SimObject
{
   typedef SimObject Parent;

public:
    struct BehaviorField
    {
        BehaviorField( const char* name, const char* description, const char* type, const char* defaultValue, const char* userData )
        {
            mName             = name ? StringTable->insert(name) : StringTable->EmptyString;;
            mDescription      = description ? StringTable->insert(description) : StringTable->EmptyString;
            mType             = type ? StringTable->insert(type) : StringTable->EmptyString;
            mDefaultValue     = defaultValue ? StringTable->insert(defaultValue) : StringTable->EmptyString;
            mUserData         = userData ? StringTable->insert(userData) : StringTable->EmptyString;
        }

        StringTableEntry mName;
        StringTableEntry mDescription;
        StringTableEntry mType;
        StringTableEntry mUserData;
        StringTableEntry mDefaultValue;
    };

    /// Behavior port common functionality.
    struct BehaviorPort
    {
        BehaviorPort( const char* name, const char* label, const char* description )
        {
            mName        = name ? StringTable->insert(name) : StringTable->EmptyString;
            mLabel       = label ? StringTable->insert(label) : StringTable->EmptyString;
            mDescription = description ? StringTable->insert(description) : StringTable->EmptyString;
        }

        StringTableEntry mName;
        StringTableEntry mLabel;
        StringTableEntry mDescription;
    };

    /// A behavior port that accepts input.
    struct BehaviorPortInput : public BehaviorPort
    {
        BehaviorPortInput( const char* name, const char* label, const char* description ) :
            BehaviorPort( name, label, description )
            {
            }
    };

    /// A behavior port that raises an output.
    struct BehaviorPortOutput : public BehaviorPort
    {
        BehaviorPortOutput( const char* name, const char* label, const char* description ) :
            BehaviorPort( name, label, description )
            {
            }
    };

public:
    BehaviorTemplate();
    virtual ~BehaviorTemplate() {}

    virtual bool onAdd();
    virtual void onRemove();
    static void initPersistFields();

    /// Create a BehaviorInstance from this template
    BehaviorInstance* createInstance( void );

    /// Template.
    inline StringTableEntry getFriendlyName( void ) const { return mFriendlyName; }
    inline StringTableEntry getDescription( void ) const { return mDescription; }
    inline StringTableEntry getBehaviorType( void ) const { return mBehaviorType; }

    /// Fields.
    bool addBehaviorField( const char* fieldName, const char* description, const char* type, const char* defaultValue = NULL, const char* userData = NULL );
    inline U32 getBehaviorFieldCount( void ) const { return mFields.size(); };
    inline BehaviorField* getBehaviorField( const U32 index ) { return index < (U32)mFields.size() ? &mFields[index] : NULL; }
    inline BehaviorField* getBehaviorField( const char* fieldName )
    {
        StringTableEntry name = StringTable->insert( fieldName );
        for( Vector<BehaviorField>::iterator itr = mFields.begin(); itr != mFields.end(); ++itr )
        {
            // Check if found.
            if ( name == itr->mName )
                return itr;
        }
        return NULL;
    }
    inline bool hasBehaviorField( const char* fieldName )
    {
        StringTableEntry name = StringTable->insert( fieldName );
        for( Vector<BehaviorField>::iterator itr = mFields.begin(); itr != mFields.end(); ++itr )
        {
            // Check if found.
            if ( name == itr->mName )
                return true;
        }
        return false;
    }
    /// Outputs.
    bool addBehaviorOutput( const char* portName, const char* label, const char* description );
    inline U32 getBehaviorOutputCount( void ) const { return mPortOutputs.size(); }
    inline BehaviorPortOutput* getBehaviourOutput( const U32 index ) { return index < (U32)mPortOutputs.size() ? &mPortOutputs[index] : NULL; }
    inline bool hasBehaviorOutput( const char* portName )
    {
        StringTableEntry name = StringTable->insert( portName );
        for( Vector<BehaviorPortOutput>::iterator itr = mPortOutputs.begin(); itr != mPortOutputs.end(); ++itr )
        {
            // Check if found.
            if ( name == itr->mName )
                return true;
        }
        return false;
    }

    /// Inputs.
    bool addBehaviorInput( const char* portName, const char* label, const char* description );
    inline U32 getBehaviorInputCount( void ) const { return mPortInputs.size(); }
    inline BehaviorPortInput* getBehaviourInput( const U32 index ) { return index < (U32)mPortInputs.size() ? &mPortInputs[index] : NULL; }
    inline bool hasBehaviorInput( const char* portName )
    {
        StringTableEntry name = StringTable->insert( portName );
        for( Vector<BehaviorPortInput>::iterator itr = mPortInputs.begin(); itr != mPortInputs.end(); ++itr )
        {
            // Check if found.
            if ( name == itr->mName )
                return true;
        }
        return false;
    }

    DECLARE_CONOBJECT(BehaviorTemplate);

protected:
    StringTableEntry mFriendlyName;
    StringTableEntry mDescription;   
    StringTableEntry mBehaviorType;

    Vector<BehaviorField> mFields;
    Vector<BehaviorPortInput> mPortInputs;
    Vector<BehaviorPortOutput> mPortOutputs;


    static bool setDescription(void* obj, const char* data) { static_cast<BehaviorTemplate *>(obj)->mDescription = data ? StringTable->insert(data) : StringTable->EmptyString; return false; }
    static const char* getDescription(void* obj, const char* data) { return static_cast<BehaviorTemplate *>(obj)->mDescription; }
};

#endif // _BEHAVIORTEMPLATE_H_
