<<<<<<< HEAD
﻿//-----------------------------------------------------------------------------
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

ConsoleMethodGroupBeginWithDocs(ParticleAsset, AssetBase)

//-----------------------------------------------------------------------------
/// Particle asset accessors.
//-----------------------------------------------------------------------------

/*! Sets the life-mode of the particle effect.
    @param lifeMode The life-mode of the particle effect (either INFINITE, CYCLE, KILL or STOP.
    A life-mode of INFINITE causes the particle effect to last forever.
    A life-mode of CYCLE causes the particle effect to restart playing when its specified 'lifetime' has been reached.
    A life-mode of KILL causes the particle effect to be deleted when its specified 'lifetime' has been reached.
    A life-mode of STOP causes the particle effect to stop playing (but not be deleted) when its specified lifetime has been reached.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAsset, setLifeMode, ConsoleVoid, 3, 3, (lifeMode))
{
    object->setLifeMode( ParticleAsset::getParticleAssetLifeModeEnum( argv[2] ) );
}

//-----------------------------------------------------------------------------

/*! Gets the life-mode of the particle effect.
    @return The life-mode of the particle effect.
*/
ConsoleMethodWithDocs( ParticleAsset, getLifeMode, ConsoleString, 2, 2, ())
{
    return ParticleAsset::getParticleAssetLifeModeDescription( object->getLifeMode() );
}

//-----------------------------------------------------------------------------

/*! Sets the lifetime of the particle effect.
    @param lifeTime The lifetime of the particle effect.  This is used according to the 'lifeMode' setting.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAsset, setLifetime, ConsoleVoid, 3, 3, (lifeTime))
{
    object->setLifetime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the lifetime of the particle effect.
    @return The lifetime of the particle effect.
*/
ConsoleMethodWithDocs(ParticleAsset, getLifetime, ConsoleFloat, 2, 2, ())
{
    return object->getLifetime();
}

//-----------------------------------------------------------------------------
/// Particle asset fields.
//-----------------------------------------------------------------------------

/*! Gets the number of available selectable fields.
    @return The number of available selectable fields.
*/
ConsoleMethodWithDocs(ParticleAsset, getSelectableFieldCount, ConsoleInt, 2, 2, ())
{
    return object->getParticleFields().getFields().size();
}

//-----------------------------------------------------------------------------

/*! Gets the selectable field at the specified index.
    @return The selectable field name at the specified index.
*/
ConsoleMethodWithDocs(ParticleAsset, getSelectableFieldName, ConsoleString, 3, 3, (fieldIndex))
{
    // Fetch the field hash.
    const ParticleAssetFieldCollection::typeFieldHash& fieldHash = object->getParticleFields().getFields();

    // Fetch the index.
    S32 fieldIndex = dAtoi( argv[2] );

    // Is the field index valid?
    if ( fieldIndex >= 0 && fieldIndex < (S32)fieldHash.size() )
    {
        // Yes, but because the fields are in a hash-table, we'll have to iterate and get O(index).
        for( ParticleAssetFieldCollection::typeFieldHash::const_iterator fieldItr = fieldHash.begin(); fieldItr != fieldHash.end(); ++fieldItr, --fieldIndex )
        {
            // Skip if this is not the field index we're looking for?
            if ( fieldIndex != 0 )
                continue;

            // Found it so return the field name.
            return fieldItr->value->getFieldName();
        }
    }


    // Warn.
    Con::warnf( "ParticleAsset::getSelectableFieldName() - Index '%d' is out of range.", fieldIndex );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

/*! Select the specified field by its name.
    @param fieldName The field name to use for the selection.  Use an empty name to deselect to stop accidental changes.
    @return Whether the field was successfully selected or not.
*/
ConsoleMethodWithDocs(ParticleAsset, selectField, ConsoleBool, 3, 3, (fieldName))
{
    return object->getParticleFields().selectField( argv[2] ) != NULL;
}

//-----------------------------------------------------------------------------

/*! Deselect any selected field.  If no field is selected then nothing happens.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAsset, deselectField, ConsoleVoid, 2, 2, ())
{
    object->getParticleFields().deselectField();
}

//-----------------------------------------------------------------------------

/*! Gets the selected field name or nothing if no field is selected.
    @return The selected field name or nothing if no fields is selected.
*/
ConsoleMethodWithDocs(ParticleAsset, getSelectedField, ConsoleBool, 2, 2, ())
{
    // Get the selected field.
    const ParticleAssetField* pParticleAssetField = object->getParticleFields().getSelectedField();

    return pParticleAssetField == NULL ? StringTable->EmptyString : pParticleAssetField->getFieldName();
}

//-----------------------------------------------------------------------------

/*! Sets a single data-key at time-zero with the specified value.  All existing keys are cleared.
    @param value The value to set the key to.
    @return Returns the index of the new data-key (always zero) or -1 on failure.
*/
ConsoleMethodWithDocs(ParticleAsset, setSingleDataKey, ConsoleInt, 3, 3, (value))
{
    return object->getParticleFields().setSingleDataKey( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Add Data-Key to Graph.
    @param time The key time.
    @param value The value at specified time
    @return Returns the index of the new data-key or -1 on failure.
*/
ConsoleMethodWithDocs(ParticleAsset, addDataKey, ConsoleInt, 4, 4, (time, value))
{
    return object->getParticleFields().addDataKey( dAtof(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Remove the data-key from the field.
    @param keyIndex The index of the data-key you want to remove.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAsset, removeDataKey, ConsoleBool, 3, 3, (keyIndex))
{
   return object->getParticleFields().removeDataKey( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Clears all data-key(s) from the field.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAsset, clearDataKeys, ConsoleBool, 2, 2, ())
{
   return object->getParticleFields().clearDataKeys();
}

//-----------------------------------------------------------------------------

/*! Set data-key value for the field.
    @param keyIndex The index of the key to be modified.
    @param value The value to change the key to.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAsset, setDataKeyValue, ConsoleBool, 4, 4, (keyIndex, value))
{
   // Set Data Key.
   return object->getParticleFields().setDataKey( dAtoi(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Gets the data-key count.
    @return The number of data-keys in the currently selected field or -1 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAsset, getDataKeyCount, ConsoleInt, 2, 2, ())
{
   // Get Data Key Count.
   return object->getParticleFields().getDataKeyCount();
}

//-----------------------------------------------------------------------------

/*! Gets the data-key at the specified index from the field.
    @param keyIndex The index of the data-key to be retrieved.
    @return The data-key comprising both the time and value or nothing if the key is invalid.
*/
ConsoleMethodWithDocs(ParticleAsset, getDataKey, ConsoleString, 3, 3, (keyIndex))
{
   // Fetch the key index.
   const S32 keyIndex = dAtoi(argv[2]);

   // Fetch the data-key.
   const ParticleAssetField::DataKey dataKey = object->getParticleFields().getDataKey( keyIndex );

   // Finish if the data-key is bad.
   if ( dataKey == ParticleAssetField::BadDataKey )
       return StringTable->EmptyString;

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);

   // Format Buffer.
   dSprintf(pBuffer, 32, "%f %f", dataKey.mTime, dataKey.mValue );

   // Return buffer.
   return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Get the minimum value for the field.
    @return The minimum value for the field or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAsset, getMinValue, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getMinValue();
}

//-----------------------------------------------------------------------------

/*! Get the maximum value for the field.
    @return The maximum value for the field or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAsset, getMaxValue, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getMaxValue();
}

//-----------------------------------------------------------------------------

/*! Get the minimum time for the field.
    @return The minimum time for the field or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAsset, getMinTime, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getMinTime();
}

//-----------------------------------------------------------------------------

/*! Get the maximum time for the field.
    @return The maximum time for the field or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAsset, getMaxTime, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getMaxTime();
}

//-----------------------------------------------------------------------------

/*! Get the fields' value at the specified time.
    @param time The time to sample the field value at.
    @return The fields' value at the specified time or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAsset, getFieldValue, ConsoleFloat, 3, 3, (time))
{
   return object->getParticleFields().getFieldValue( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Sets the time period to repeat (cycle) the fields' values at.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAsset, setRepeatTime, ConsoleBool, 3, 3, (repeatTime))
{
   return object->getParticleFields().setRepeatTime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the time period that the fields' value repeat (cycle) at.
    @return The time period that the fields' value repeat (cycle) at.
*/
ConsoleMethodWithDocs(ParticleAsset, getRepeatTime, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getRepeatTime();
}

//-----------------------------------------------------------------------------

/*! Set the scaling of field values retrieved from the field.  This does not alter the actual data-key values.
    @param valueScale The scale for field values retrieved from the field.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAsset, setValueScale, ConsoleBool, 3, 3, (valueScale))
{
   return object->getParticleFields().setValueScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the scaling of field values' retrieved from the field.
    @return The scaling of field values' retrieved from the field.
*/
ConsoleMethodWithDocs(ParticleAsset, getValueScale, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getValueScale();
}

//-----------------------------------------------------------------------------
/// Emitter asset methods.
//-----------------------------------------------------------------------------

/*! Creates and add a new emitter.
    @return The new emitter that was added or 0 if failed.
*/
ConsoleMethodWithDocs(ParticleAsset, createEmitter, ConsoleString, 2, 2, ())
{
    // Find the emitter.
    ParticleAssetEmitter* pEmitter = object->createEmitter();

    return pEmitter == NULL ? StringTable->EmptyString : pEmitter->getIdString();
}

//-----------------------------------------------------------------------------

/*! Adds an existing emitter.
    @param emitterId The emitter to add.
    @return On success it returns the ID of the emitter, or 0 if failed.
*/
ConsoleMethodWithDocs(ParticleAsset, addEmitter, ConsoleBool, 3, 3, (emitterId))
{
    // Find the emitter.
    ParticleAssetEmitter* pEmitter = Sim::findObject<ParticleAssetEmitter>( argv[2] );

    // Did we find the emitter?
    if ( pEmitter == NULL )
    {
        // No, so warn.
        Con::warnf( "ParticleAsset::addEmitter() - Could not find the emitter '%s'.", argv[2] );
        return false;
    }

    return object->addEmitter(pEmitter);
}

//-----------------------------------------------------------------------------

/*! Removes an emitter.
    @param emitterId The emitter to remove.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAsset, removeEmitter, ConsoleBool, 3, 4, (emitterId, [bool deleteEmitter]))
{
    // Find the emitter.
    ParticleAssetEmitter* pEmitter = Sim::findObject<ParticleAssetEmitter>( argv[2] );

    // Did we find the emitter?
    if ( pEmitter == NULL )
    {
        // No, so warn.
        Con::warnf( "ParticleAsset::removeEmitter() - Could not find the emitter '%s'.", argv[2] );
        return false;
    }

    bool deleteEmitter = true;
    if (argc > 3)
        deleteEmitter = dAtob(argv[3]);

    // Remove the emitter.
    object->removeEmitter( pEmitter, deleteEmitter );

    return true;
}

//-----------------------------------------------------------------------------

/*! Clear all the emitters.
    @return No return Value.
*/
ConsoleMethodWithDocs(ParticleAsset, clearEmitters, ConsoleVoid, 2, 2, ())
{
   // Clear Emitters.
   object->clearEmitters();
}

//-----------------------------------------------------------------------------

/*! Gets the emitter count.
    @return Returns the number of emitters as an integer.
*/
ConsoleMethodWithDocs(ParticleAsset, getEmitterCount, ConsoleInt, 2, 2, ())
{
   // Get Emitter Count.
   return object->getEmitterCount();
}

//-----------------------------------------------------------------------------

/*! Gets the emitter at the specified index.
    @param emitterIndex The index for the desired emitter
    @return The emitter or 0 if not found.
*/
ConsoleMethodWithDocs(ParticleAsset, getEmitter, ConsoleInt, 3, 3, (emitterIndex))
{
   // Get the emitter.
   ParticleAssetEmitter* pEmitter = object->getEmitter( dAtoi(argv[2]) );

   return pEmitter == NULL ? 0 : pEmitter->getId();
}

//-----------------------------------------------------------------------------

/*! Finds the emitter by its name.
    @param emitterName The name of the desired emitter.
    @return The emitter or 0 if not found.
*/
ConsoleMethodWithDocs(ParticleAsset, findEmitter, ConsoleInt, 3, 3, (emitterName))
{
   // Find the emitter.
   ParticleAssetEmitter* pEmitter = object->findEmitter( argv[2] );

   return pEmitter == NULL ? 0 : pEmitter->getId();
}

//-----------------------------------------------------------------------------

/*! Moves the emitter order.
    @param fromEmitterIndex The source index of the emitter to move.
    @param toEmitterIndex The destination index to move the emitter to.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAsset, moveEmitter, ConsoleVoid, 4, 4, (fromEmitterIndex, toEmitterIndex))
{
   // Move Emitter Object.
   object->moveEmitter( dAtoi(argv[2]), dAtoi(argv[3]) );
}

ConsoleMethodGroupEndWithDocs(ParticleAsset)
=======
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

//-----------------------------------------------------------------------------
/// Particle asset accessors.
//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, setLifeMode, void, 3, 3,   "(lifeMode) Sets the life-mode of the particle effect.\n"
                                                        "@param lifeMode The life-mode of the particle effect (either INFINITE, CYCLE, KILL or STOP.\n"
                                                        "A life-mode of INFINITE causes the particle effect to last forever.\n"
                                                        "A life-mode of CYCLE causes the particle effect to restart playing when its specified 'lifetime' has been reached.\n"
                                                        "A life-mode of KILL causes the particle effect to be deleted when its specified 'lifetime' has been reached.\n"
                                                        "A life-mode of STOP causes the particle effect to stop playing (but not be deleted) when its specified lifetime has been reached.\n"
                                                        "@return No return value.")
{
    object->setLifeMode( ParticleAsset::getParticleAssetLifeModeEnum( argv[2] ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod( ParticleAsset, getLifeMode, const char*, 2, 2,   "() - Gets the life-mode of the particle effect.\n"
                                                                "@return The life-mode of the particle effect." )
{
    return ParticleAsset::getParticleAssetLifeModeDescription( object->getLifeMode() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, setLifetime, void, 3, 3,   "(lifeTime) Sets the lifetime of the particle effect.\n"
                                                        "@param lifeTime The lifetime of the particle effect.  This is used according to the 'lifeMode' setting.\n"
                                                        "@return No return value." )
{
    object->setLifetime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getLifetime, F32, 2, 2,    "() Gets the lifetime of the particle effect.\n"
                                                        "@return The lifetime of the particle effect." )
{
    return object->getLifetime();
}

//-----------------------------------------------------------------------------
/// Particle asset fields.
//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getSelectableFieldCount, S32, 2, 2,    "() Gets the number of available selectable fields.\n"
                                                                    "@return The number of available selectable fields." )
{
    return object->getParticleFields().getFields().size();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getSelectableFieldName, const char*, 3, 3, "(fieldIndex) Gets the selectable field at the specified index.\n"
                                                                        "@return The selectable field name at the specified index." )
{
    // Fetch the field hash.
    const ParticleAssetFieldCollection::typeFieldHash& fieldHash = object->getParticleFields().getFields();

    // Fetch the index.
    S32 fieldIndex = dAtoi( argv[2] );

    // Is the field index valid?
    if ( fieldIndex >= 0 && fieldIndex < (S32)fieldHash.size() )
    {
        // Yes, but because the fields are in a hash-table, we'll have to iterate and get O(index).
        for( ParticleAssetFieldCollection::typeFieldHash::const_iterator fieldItr = fieldHash.begin(); fieldItr != fieldHash.end(); ++fieldItr, --fieldIndex )
        {
            // Skip if this is not the field index we're looking for?
            if ( fieldIndex != 0 )
                continue;

            // Found it so return the field name.
            return fieldItr->value->getFieldName();
        }
    }


    // Warn.
    Con::warnf( "ParticleAsset::getSelectableFieldName() - Index '%d' is out of range.", fieldIndex );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, selectField, bool, 3, 3,   "(fieldName) Select the specified field by its name.\n"
                                                        "@param fieldName The field name to use for the selection.  Use an empty name to deselect to stop accidental changes.\n"
                                                        "@return Whether the field was successfully selected or not.")
{
    return object->getParticleFields().selectField( argv[2] ) != NULL;
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, deselectField, void, 2, 2, "() Deselect any selected field.  If no field is selected then nothing happens.\n"
                                                        "@return No return value.")
{
    object->getParticleFields().deselectField();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getSelectedField, const char*, 2, 2,  "() Gets the selected field name or nothing if no field is selected.\n"
                                                            "@return The selected field name or nothing if no fields is selected.")
{
    // Get the selected field.
    const ParticleAssetField* pParticleAssetField = object->getParticleFields().getSelectedField();

    return pParticleAssetField == NULL ? StringTable->EmptyString : pParticleAssetField->getFieldName();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, setSingleDataKey, S32, 3, 3,   "(value) Sets a single data-key at time-zero with the specified value.  All existing keys are cleared.\n"
                                                            "@param value The value to set the key to.\n"
                                                            "@return Returns the index of the new data-key (always zero) or -1 on failure.")
{
    return object->getParticleFields().setSingleDataKey( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, addDataKey, S32, 4, 4, "(time, value) Add Data-Key to Graph.\n"
                                                    "@param time The key time.\n"
                                                    "@param value The value at specified time\n"
                                                    "@return Returns the index of the new data-key or -1 on failure.")
{
    return object->getParticleFields().addDataKey( dAtof(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, removeDataKey, bool, 3, 3, "(keyIndex) Remove the data-key from the field.\n"
                                                        "@param keyIndex The index of the data-key you want to remove.\n"
                                                        "@return Whether the operation was successful or not.")
{
   return object->getParticleFields().removeDataKey( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, clearDataKeys, bool, 2, 2,     "() Clears all data-key(s) from the field.\n"
                                                            "@return Whether the operation was successful or not.")
{
   return object->getParticleFields().clearDataKeys();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, setDataKeyValue, bool, 4, 4, "(keyIndex, value) Set data-key value for the field.\n"
                                                          "@param keyIndex The index of the key to be modified.\n"
                                                          "@param value The value to change the key to.\n"
                                                          "@return Whether the operation was successful or not.")
{
   // Set Data Key.
   return object->getParticleFields().setDataKey( dAtoi(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getDataKeyCount, S32, 2, 2,    "() Gets the data-key count.\n"
                                                            "@return The number of data-keys in the currently selected field or -1 if no field is selected.")
{
   // Get Data Key Count.
   return object->getParticleFields().getDataKeyCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getDataKey, const char*, 3, 3, "(keyIndex) Gets the data-key at the specified index from the field.\n"
                                                            "@param keyIndex The index of the data-key to be retrieved.\n"
                                                            "@return The data-key comprising both the time and value or nothing if the key is invalid.")
{
   // Fetch the key index.
   const S32 keyIndex = dAtoi(argv[2]);

   // Fetch the data-key.
   const ParticleAssetField::DataKey dataKey = object->getParticleFields().getDataKey( keyIndex );

   // Finish if the data-key is bad.
   if ( dataKey == ParticleAssetField::BadDataKey )
       return StringTable->EmptyString;

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);

   // Format Buffer.
   dSprintf(pBuffer, 32, "%f %f", dataKey.mTime, dataKey.mValue );

   // Return buffer.
   return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getMinValue, F32, 2, 2,    "() Get the minimum value for the field.\n"
                                                        "@return The minimum value for the field or always 0.0 if no field is selected." )
{
   return object->getParticleFields().getMinValue();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getMaxValue, F32, 2, 2,    "() Get the maximum value for the field.\n"
                                                        "@return The maximum value for the field or always 0.0 if no field is selected." )
{
   return object->getParticleFields().getMaxValue();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getMinTime, F32, 2, 2,     "() Get the minimum time for the field.\n"
                                                        "@return The minimum time for the field or always 0.0 if no field is selected." )
{
   return object->getParticleFields().getMinTime();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getMaxTime, F32, 2, 2,     "() Get the maximum time for the field.\n"
                                                        "@return The maximum time for the field or always 0.0 if no field is selected." )
{
   return object->getParticleFields().getMaxTime();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getFieldValue, F32, 3, 3,  "(time) Get the fields' value at the specified time.\n"
                                                        "@param time The time to sample the field value at.\n"
                                                        "@return The fields' value at the specified time or always 0.0 if no field is selected.")
{
   return object->getParticleFields().getFieldValue( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, setRepeatTime, bool, 3, 3, "(repeatTime) Sets the time period to repeat (cycle) the fields' values at.\n"
                                                        "@return Whether the operation was successful or not.")
{
   return object->getParticleFields().setRepeatTime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getRepeatTime, F32, 2, 2,  "() Gets the time period that the fields' value repeat (cycle) at.\n"
                                                        "@return The time period that the fields' value repeat (cycle) at.\n" )
{
   return object->getParticleFields().getRepeatTime();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, setValueScale, bool, 3, 3, "(valueScale) Set the scaling of field values retrieved from the field.  This does not alter the actual data-key values.\n"
                                                        "@param valueScale The scale for field values retrieved from the field.\n"
                                                        "@return Whether the operation was successful or not.")
{
   return object->getParticleFields().setValueScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getValueScale, F32, 2, 2,  "() Gets the scaling of field values' retrieved from the field.\n"
                                                        "@return The scaling of field values' retrieved from the field." )
{
   return object->getParticleFields().getValueScale();
}

//-----------------------------------------------------------------------------
/// Emitter asset methods.
//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, createEmitter, const char*, 2, 2,  "() - Creates and add a new emitter.\n"
                                                                "@return The new emitter that was added or 0 if failed.")
{
    // Find the emitter.
    ParticleAssetEmitter* pEmitter = object->createEmitter();

    return pEmitter == NULL ? StringTable->EmptyString : pEmitter->getIdString();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, addEmitter, bool, 3, 3,    "(emitterId) - Adds an existing emitter.\n"
                                                        "@param emitterId The emitter to add.\n"
                                                        "@return On success it returns the ID of the emitter, or 0 if failed.")
{
    // Find the emitter.
    ParticleAssetEmitter* pEmitter = Sim::findObject<ParticleAssetEmitter>( argv[2] );

    // Did we find the emitter?
    if ( pEmitter == NULL )
    {
        // No, so warn.
        Con::warnf( "ParticleAsset::addEmitter() - Could not find the emitter '%s'.", argv[2] );
        return false;
    }

    return object->addEmitter(pEmitter);
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, removeEmitter, bool, 3, 4, "(emitterId, [bool deleteEmitter]) - Removes an emitter.\n"
                                                        "@param emitterId The emitter to remove.\n"
                                                        "@return No return value.")
{
    // Find the emitter.
    ParticleAssetEmitter* pEmitter = Sim::findObject<ParticleAssetEmitter>( argv[2] );

    // Did we find the emitter?
    if ( pEmitter == NULL )
    {
        // No, so warn.
        Con::warnf( "ParticleAsset::removeEmitter() - Could not find the emitter '%s'.", argv[2] );
        return false;
    }

    bool deleteEmitter = true;
    if (argc > 3)
        deleteEmitter = dAtob(argv[3]);

    // Remove the emitter.
    object->removeEmitter( pEmitter, deleteEmitter );

    return true;
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, clearEmitters, void, 2, 2, "() Clear all the emitters.\n"
                                                        "@return No return Value.")
{
   // Clear Emitters.
   object->clearEmitters();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getEmitterCount, S32, 2, 2,    "() Gets the emitter count.\n"
                                                            "@return Returns the number of emitters as an integer.")
{
   // Get Emitter Count.
   return object->getEmitterCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, getEmitter, S32, 3, 3,     "(emitterIndex) Gets the emitter at the specified index.\n"
                                                        "@param emitterIndex The index for the desired emitter\n"
                                                        "@return The emitter or 0 if not found.")
{
   // Get the emitter.
   ParticleAssetEmitter* pEmitter = object->getEmitter( dAtoi(argv[2]) );

   return pEmitter == NULL ? 0 : pEmitter->getId();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, findEmitter, S32, 3, 3,    "(emitterName) Finds the emitter by its name.\n"
                                                        "@param emitterName The name of the desired emitter.\n"
                                                        "@return The emitter or 0 if not found.")
{
   // Find the emitter.
   ParticleAssetEmitter* pEmitter = object->findEmitter( argv[2] );

   return pEmitter == NULL ? 0 : pEmitter->getId();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAsset, moveEmitter, void, 4, 4, "(fromEmitterIndex, toEmitterIndex) Moves the emitter order.\n"
              "@param fromEmitterIndex The source index of the emitter to move.\n"
              "@param toEmitterIndex The destination index to move the emitter to.\n"
              "@return No return value.")
{
   // Move Emitter Object.
   object->moveEmitter( dAtoi(argv[2]), dAtoi(argv[3]) );
}
>>>>>>> 6e2964681666532c99f49535de98f93c3b6dfb24
