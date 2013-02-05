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

#include "2d/assets/ParticleAssetField.h"

#ifndef _CORE_MATH_H_
#include "2d/core/coreMath.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

//-----------------------------------------------------------------------------

static bool particleAssetFieldPropertiesInitialized = false;

static StringTableEntry particleAssetFieldRepeatTimeName;
static StringTableEntry particleAssetFieldMaxTimeName;
static StringTableEntry particleAssetFieldMinValueName;
static StringTableEntry particleAssetFieldMaxValueName;
static StringTableEntry particleAssetFieldDefaultValueName;
static StringTableEntry particleAssetFieldValueScaleName;
static StringTableEntry particleAssetFieldDataKeysName;

ParticleAssetField::DataKey ParticleAssetField::BadDataKey( -1.0f, 0.0f );

//-----------------------------------------------------------------------------

ParticleAssetField::ParticleAssetField() :
                        mFieldName( StringTable->EmptyString ),
                        mRepeatTime( 1.0f ),
                        mMaxTime( 1.0f ),
                        mMinValue( 0.0f ),
                        mMaxValue( 0.0f ),
                        mDefaultValue( 1.0f ),
                        mValueScale( 1.0f ),
                        mValueBoundsDirty( true )
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mDataKeys );

    // Initialize names.
    if ( !particleAssetFieldPropertiesInitialized )
    {
        particleAssetFieldRepeatTimeName   = StringTable->insert( "RepeatTime" );
        particleAssetFieldMaxTimeName      = StringTable->insert( "MaxTime" );
        particleAssetFieldMinValueName     = StringTable->insert( "MinValue" );
        particleAssetFieldMaxValueName     = StringTable->insert( "MaxValue" );
        particleAssetFieldDefaultValueName = StringTable->insert( "DefaultValue" );
        particleAssetFieldValueScaleName   = StringTable->insert( "ValueScale" );
        particleAssetFieldDataKeysName     = StringTable->insert( "Keys" );

        // Flag as initialized.
        particleAssetFieldPropertiesInitialized = true;
    }
}

//-----------------------------------------------------------------------------

ParticleAssetField::~ParticleAssetField()
{

}

//-----------------------------------------------------------------------------

void ParticleAssetField::copyTo( ParticleAssetField& field )
{
    field.mFieldName = mFieldName;
    field.mRepeatTime = mRepeatTime;
    field.mMaxTime = mMaxTime;
    field.mMinValue = mMinValue;
    field.mMaxValue = mMaxValue;
    field.mDefaultValue = mDefaultValue;
    field.mValueScale = mValueScale;

    // Copy data keys.    
    field.clearDataKeys();
    for ( S32 i = 0; i < mDataKeys.size(); i++ )
    {
        DataKey key = mDataKeys[i];
        field.addDataKey(key.mTime, key.mValue);
    }
}

//-----------------------------------------------------------------------------

void ParticleAssetField::initialize( const F32 maxTime, const F32 minValue, const F32 maxValue, const F32 defaultValue )
{
    // Set the value bounds.
    setValueBounds( maxTime, minValue, maxValue, defaultValue );

    // Reset the value bounds dirty flag.
    mValueBoundsDirty = false;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::setValueBounds( F32 maxTime, F32 minValue, F32 maxValue, F32 defaultValue )
{
    // Check Max Time.
    if ( maxTime <= 0.0f )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueBounds() - Max-time '%f' is invalid", maxTime );

        // Set Default Max Time.
        maxTime = 1.0f;
    }

    // Set Max Time.
    mMaxTime = maxTime;

    // Check Value Range Normalisation.
    if ( minValue > maxValue )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueBounds() - Value Range is not normalised! (minValue:%f / maxValue:%f)", minValue, maxValue );

        // Normalise Y-Axis.
        F32 temp = minValue;
        minValue = maxValue;
        maxValue = temp;
    }
    // Check Value Range Scale.
    else if ( minValue == maxValue )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueBounds() - Value Range has no scale! (minValue:%f / maxValue:%f)", minValue, maxValue );

        // Insert some Y-Axis Scale.
        maxValue = minValue + 0.001f;
    }

    // Set Bounds.
    mMinValue = minValue;
    mMaxValue = maxValue;

    // Check Default Value.
    if ( defaultValue < minValue || defaultValue > maxValue )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueBounds() - Default Value is out of range! (minValue:%f / maxValue:%f / defaultValue:%f)", minValue, maxValue, defaultValue );

        // Clamp at lower value.
        defaultValue = minValue;
    }

    // Set Default Value.
    mDefaultValue = defaultValue;

    // Reset the data keys if none are present.
    if ( mDataKeys.size() == 0 )
        resetDataKeys();

    // Flag the value bounds as dirty.
    mValueBoundsDirty = true;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::setFieldName( const char* pFieldName )
{
    // Sanity!
    AssertFatal( mFieldName == StringTable->EmptyString, "ParticleAssetField::setFieldName() - Cannot set particle asset field name once it has been set." );

    mFieldName = StringTable->insert( pFieldName );

    // Sanity!
    AssertFatal( mFieldName != StringTable->EmptyString, "ParticleAssetField::setFieldName() - Field name cannot be empty." );
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::setRepeatTime( const F32 repeatTime )
{
    // Check repeat time.
    if ( repeatTime < 0.0f )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setRepeatTime() - Repeat time''%f'' is invalid.", repeatTime );

        // Return Error.
        return false;
    }

    // Set repeat time.
    mRepeatTime = repeatTime;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::setValueScale( const F32 valueScale )
{
    // Check Value Scale.
    if ( valueScale < 0.0f )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueScale() - Invalid Value Scale! (%f)", valueScale );

        // Return Error.
        return false;
    }

    // Set Value Scale/
    mValueScale = valueScale;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::resetDataKeys(void)
{
    // Clear Data Keys.
    mDataKeys.clear();

    // Add default value Data-Key.
    addDataKey( 0.0f, mDefaultValue );
}

//-----------------------------------------------------------------------------

S32 ParticleAssetField::setSingleDataKey( const F32 value )
{
    // Clear Data Keys.
    mDataKeys.clear();

    // Add a single key with the specified value.
    return addDataKey( 0.0f, value );
}

//-----------------------------------------------------------------------------

S32 ParticleAssetField::addDataKey( const F32 time, const F32 value )
{
    // Check Max Time.
    if ( time > mMaxTime )
    {
        // Warn.
        Con::warnf("ParticleAssetField::addDataKey() - Time is out of bounds! (time:%f)", time );

        // Return Error.
        return -1;
    }

    // If data key exists already then set it and return the key index.
    U32 index = 0;
    for ( index = 0; index < getDataKeyCount(); index++ )
    {
        // Found Time?
        if ( mDataKeys[index].mTime == time )
        {
            // Yes, so set time.
            mDataKeys[index].mValue = value;

            // Return Index.
            return index;
        }
        // Past Time?
        else if ( mDataKeys[index].mTime > time )
            // Finish search.
            break;
    }

    // Insert Data-Key.
    mDataKeys.insert( index );

    // Set Data-Key.
    mDataKeys[index].mTime = time;
    mDataKeys[index].mValue = value;

    // Return Index.
    return index;
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::removeDataKey( const U32 index )
{
    // Cannot Remove First Node!
    if ( index == 0 )
    {
        // Warn.
        Con::warnf("rParticleAssetField::emoveDataKey() - Cannot remove first Data-Key!");
        return false;
    }

    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("rParticleAssetField::emoveDataKey() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return false;
    }

    // Remove Index.
    mDataKeys.erase(index);

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::clearDataKeys( void )
{
    // Reset Data Keys.
    resetDataKeys();
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::setDataKeyValue( const U32 index, const F32 value )
{
    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setDataKeyValue() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return false;
    }

    // Set Data Key Value.
    mDataKeys[index].mValue = value;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::getDataKeyValue( const U32 index ) const
{
    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("ParticleAssetField::getDataKeyValue() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return 0.0f;
    }

    // Return Data Key Value.
    return mDataKeys[index].mValue;
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::getDataKeyTime( const U32 index ) const
{
    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("ParticleAssetField::getDataKeyTime() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return 0.0f;
    }

    // Return Data Key Time.
    return mDataKeys[index].mTime;
}

//-----------------------------------------------------------------------------

const ParticleAssetField::DataKey& ParticleAssetField::getDataKey( const U32 index ) const
{
    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("ParticleAssetField::getDataKey() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return BadDataKey;
    }

    // Return Data-Key.
    return mDataKeys[index];
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::getFieldValue( F32 time ) const
{
    // Return First Entry if it's the only one or we're using zero time.
    if ( mIsZero(time) || getDataKeyCount() < 2)
        return mDataKeys[0].mValue * mValueScale;

    // Clamp Key-Time.
    time = getMin(getMax( 0.0f, time ), mMaxTime);

    // Repeat Time.
    time = mFmod( time * mRepeatTime, mMaxTime + FLT_EPSILON );

    // Fetch Max Key Index.
    const U32 maxKeyIndex = getDataKeyCount()-1;

    // Return Last Value if we're on/past the last time.
    if ( time >= mDataKeys[maxKeyIndex].mTime )
        return mDataKeys[maxKeyIndex].mValue * mValueScale;

    // Find Data-Key Indexes.
    U32 index1;
    U32 index2;
    for ( index1 = 0; index1 < getDataKeyCount(); index1++ )
        if ( mDataKeys[index1].mTime >= time )
            break;

    // If we're exactly on a Data-Key then return that key.
    if ( mIsEqual( mDataKeys[index1].mTime, time) )
        return mDataKeys[index1].mValue * mValueScale;

    // Set Adjacent Indexes.
    index2 = index1--;

    // Fetch Index Times.
    const F32 time1 = mDataKeys[index1].mTime;
    const F32 time2 = mDataKeys[index2].mTime;
    // Calculate Time Differential.
    const F32 dTime = (time-time1)/(time2-time1);

    // Return lerped Value.
    return ((mDataKeys[index1].mValue * (1.0f-dTime)) + (mDataKeys[index2].mValue * dTime)) * mValueScale;
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::calculateFieldBV( const ParticleAssetField& base, const ParticleAssetField& variation, const F32 effectAge, const bool modulate, const F32 modulo )
{
    // Fetch Graph Components.
    const F32 baseValue   = base.getFieldValue( effectAge );
    const F32 varValue    = variation.getFieldValue( effectAge ) * 0.5f;

    // Modulate?
    if ( modulate )
        // Return Modulo Calculation.
        return mFmod( baseValue + CoreMath::mGetRandomF(-varValue, varValue), modulo );
    else
        // Return Clamped Calculation.
        return mClampF( baseValue + CoreMath::mGetRandomF(-varValue, varValue), base.getMinValue(), base.getMaxValue() );
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::calculateFieldBVE( const ParticleAssetField& base, const ParticleAssetField& variation, const ParticleAssetField& effect, const F32 effectAge, const bool modulate, const F32 modulo )
{
    // Fetch Graph Components.
    const F32 baseValue   = base.getFieldValue( effectAge );
    const F32 varValue    = variation.getFieldValue( effectAge ) * 0.5f;
    const F32 effectValue = effect.getFieldValue( effectAge );

    // Modulate?
    if ( modulate )
        // Return Modulo Calculation.
        return mFmod( (baseValue + CoreMath::mGetRandomF(-varValue, varValue)) * effectValue, modulo );
    else
        // Return Clamped Calculation.
        return mClampF( (baseValue + CoreMath::mGetRandomF(-varValue, varValue)) * effectValue, base.getMinValue(), base.getMaxValue() );
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::calculateFieldBVLE( const ParticleAssetField& base, const ParticleAssetField& variation, const ParticleAssetField& overlife, const ParticleAssetField& effect, const F32 effectAge, const F32 particleAge, const bool modulate, const F32 modulo )
{
    // Fetch Graph Components.
    const F32 baseValue   = base.getFieldValue( effectAge );
    const F32 varValue    = variation.getFieldValue( effectAge ) * 0.5f;
    const F32 effectValue = effect.getFieldValue( effectAge );
    const F32 lifeValue   = overlife.getFieldValue( particleAge );

    // Modulate?
    if ( modulate )
        // Return Modulo Calculation.
        return mFmod( (baseValue + CoreMath::mGetRandomF(-varValue, varValue)) * effectValue * lifeValue, modulo );
    else
        // Return Clamped Calculation.
        return mClampF( (baseValue + CoreMath::mGetRandomF(-varValue, varValue)) * effectValue * lifeValue, base.getMinValue(), base.getMaxValue() );
}

//------------------------------------------------------------------------------

void ParticleAssetField::onTamlCustomWrite( TamlCustomProperty* pCustomProperty )
{
    // Debug Profiling.
    PROFILE_SCOPE(ParticleAssetField_OnTamlCustomWrite);

    // Add a alias (ignore it if there ends up being no properties).
    TamlPropertyAlias* pPropertyAlias = pCustomProperty->addAlias( getFieldName(), true );

    // Sanity!
    AssertFatal( pPropertyAlias != NULL, "ParticleAssetField::onTamlCustomWrite() - Could not create field alias." );

    if ( mValueBoundsDirty && (mNotEqual( getMinValue(), 0.0f ) || mNotEqual( getMaxValue(), 0.0f )) )
    {
        pPropertyAlias->addField( particleAssetFieldMinValueName, getMinValue() );
        pPropertyAlias->addField( particleAssetFieldMaxValueName, getMaxValue() );
    }
    
    if ( mValueBoundsDirty && mNotEqual( getMaxTime(), 1.0f ) )
        pPropertyAlias->addField( particleAssetFieldMaxTimeName, getMaxTime() );

    if ( mValueBoundsDirty && mNotEqual( getDefaultValue(), 1.0f ) )
        pPropertyAlias->addField( particleAssetFieldDefaultValueName, getDefaultValue() );

    if ( mNotEqual( getValueScale(), 1.0f ) )
        pPropertyAlias->addField( particleAssetFieldValueScaleName, getValueScale() );

    if ( mNotEqual( getRepeatTime(), 1.0f ) )
        pPropertyAlias->addField( particleAssetFieldRepeatTimeName, getRepeatTime() );

    // Fetch key count.
    const U32 keyCount = getDataKeyCount();

    // Finish if no data keys.
    if ( keyCount == 0 )
        return;

    // Finish if there's only one key and it's the default one.
    if ( keyCount == 1 && mIsEqual(mDataKeys[0].mTime, 0.0f) && mIsEqual(mDataKeys[0].mValue, mDefaultValue) )
        return;

    // Format the keys,
    char keysBuffer[MAX_TAML_PROPERTY_FIELDVALUE_LENGTH];
    char* pKeysBuffer = keysBuffer;
    S32 bufferSize = sizeof(keysBuffer);

    // Iterate the keys.
    for( U32 index = 0; index < keyCount; ++index )
    {
        // Fetch the data key.
        const DataKey& dataKey = mDataKeys[index];

        // Format the key.
        S32 written = dSprintf( pKeysBuffer, bufferSize, index == 0 ? "%.5g %.5g" : " %.5g %.5g", dataKey.mTime, dataKey.mValue );
        pKeysBuffer += written;
        bufferSize -= written;
    }

    pPropertyAlias->addField( particleAssetFieldDataKeysName, keysBuffer );
}

//-----------------------------------------------------------------------------

void ParticleAssetField::onTamlCustomRead( const TamlPropertyAlias* pPropertyAlias )
{
    // Debug Profiling.
    PROFILE_SCOPE(ParticleAssetField_OnTamlCustomRead);

    // Fetch existing values.
    F32 repeatTime = getRepeatTime();
    F32 maxTime = getMaxTime();
    F32 minValue = getMinValue();
    F32 maxValue = getMaxValue();
    F32 defaultValue = getDefaultValue();
    F32 valueScale = getValueScale();

    // Set-up a temporary set of keys.
    Vector<DataKey> keys;

    // Clear the existing keys.
    mDataKeys.clear();

    // Iterate property fields.
    for ( TamlPropertyAlias::const_iterator propertyFieldItr = pPropertyAlias->begin(); propertyFieldItr != pPropertyAlias->end(); ++propertyFieldItr )
    {
        // Fetch property field.
        TamlPropertyField* pPropertyField = *propertyFieldItr;

        // Fetch property field name.
        StringTableEntry fieldName = pPropertyField->getFieldName();

        if ( fieldName == particleAssetFieldRepeatTimeName )
        {
            pPropertyField->getFieldValue( repeatTime );
        }
        else if ( fieldName == particleAssetFieldMaxTimeName )
        {
            pPropertyField->getFieldValue( maxTime );
            mValueBoundsDirty = true;
        }
        else if ( fieldName == particleAssetFieldMinValueName )
        {
            pPropertyField->getFieldValue( minValue );
            mValueBoundsDirty = true;
        }
        else if ( fieldName == particleAssetFieldMaxValueName )
        {
            pPropertyField->getFieldValue( maxValue );
            mValueBoundsDirty = true;
        }
        else if ( fieldName == particleAssetFieldDefaultValueName )
        {
            pPropertyField->getFieldValue( defaultValue );
            mValueBoundsDirty = true;
        }
        else if ( fieldName == particleAssetFieldValueScaleName )
        {
            pPropertyField->getFieldValue( valueScale );
        }
        else if ( fieldName == particleAssetFieldDataKeysName )
        {
            const char* pDataKeys = pPropertyField->getFieldValue();
            const S32 elementCount = StringUnit::getUnitCount( pDataKeys, " ,\t" );

            // Are there a valid number of elements?
            if ( elementCount < 2 || (elementCount % 2 ) != 0 )
            {
                // No, so warn.
                Con::warnf( "ParticleAssetField::onTamlCustomRead() - An invalid set of data keys was found." );
            }
            else
            {
                // Iterate the elements.
                for( S32 elementIndex = 0; elementIndex <= (elementCount-2); elementIndex += 2 )
                {
                    DataKey key;
                    key.mTime = dAtof( StringUnit::getUnit( pDataKeys, elementIndex, " ,\t" ) );
                    key.mValue = dAtof( StringUnit::getUnit( pDataKeys, elementIndex+1, " ,\t" ) );
                    keys.push_back( key );
                }
            }            
        }
    }

    // Set the value bounds.
    setValueBounds( maxTime, minValue, maxValue, defaultValue );

    // Set the value scale.
    setValueScale( valueScale );

    // Set the repeat time.
    setRepeatTime( repeatTime );

    // Set the data keys.
    for ( S32 index = 0; index < keys.size(); ++index )
    {
        const DataKey& key = keys[index];
        addDataKey( key.mTime, key.mValue );
    }
}
