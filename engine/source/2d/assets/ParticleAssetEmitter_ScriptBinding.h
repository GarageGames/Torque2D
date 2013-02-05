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
/// Particle emitter accessors.
//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getOwner, const char*, 2, 2,    "() Gets the asset owner of the emitter.\n"
                                                                    "@return The asset owner of the emitter or nothing if no owner assigned." )
{
    // Fetch the owner.
    ParticleAsset* pParticleAsset = object->getOwner();

    return pParticleAsset == NULL ? StringTable->EmptyString : pParticleAsset->getIdString();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setEmitterName, void, 3, 3,     "(emitterName) Sets the name of the emitter.\n"
                                                                    "@param emitterName The name to set the emitter to.\n"
                                                                    "@return No return value." )
{
    object->setEmitterName( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getEmitterName, const char*, 2, 2,  "() Gets the name of the emitter.\n"
                                                                        "@return The name of the emitter." )
{
    return object->getEmitterName();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setEmitterType, void, 3, 3,     "(emitterType) Sets the type of the emitter.\n"
                                                                    "@param emitterType The type to set the emitter.  Either 'POINT', 'LINE', 'BOX' or 'DISK', 'ELLIPSE' or 'TORUS'.\n"
                                                                    "An emitter-type of 'POINT' creates the particles at the position of the particle asset.\n"
                                                                    "An emitter-type of 'LINE' creates the particles along a line defined by the particle width.\n"
                                                                    "An emitter-type of 'BOX' creates the particles within the dimensions defined by the particle size.\n"
                                                                    "An emitter-type of 'DISK' creates the particles within a disk with radii defined by the particle size.\n"
                                                                    "An emitter-type of 'ELLIPSE' creates the particles on an ellipse with the radii defined by the particle size.\n"
                                                                    "An emitter-type of 'TORUS' creates the particles within a torus with a maximum and minimum radii defined by the particle width and height respectively.\n"
                                                                    "@return No return value." )
{
    object->setEmitterType( ParticleAssetEmitter::getEmitterTypeEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getEmitterType, const char*, 2, 2,  "() Gets the type of the emitter.\n"
                                                                        "@return The type of the emitter." )
{
    return ParticleAssetEmitter::getEmitterTypeDescription( object->getEmitterType() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setFixedAspect, void, 3, 3,     "(fixedAspect) Sets the emitter to used a fixed-aspect for particles.\n"
                                                                    "@param fixedAspect Whether to use a fixed-aspect or not.\n"
                                                                    "@return No return value." )
{
    object->setFixedAspect( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getFixedAspect, bool, 2, 2,     "() Gets whether the emitter uses a fixed-aspect for particles or not.\n"
                                                                    "@return Whether the emitter uses a fixed-aspect for particles or not." )
{
    return object->getFixedAspect();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setFixedForceAngle, void, 3, 3, "(fixedForceAngle) Sets the emitter fixed-force angle for particles.\n"
                                                                    "@param fixedForceAngle The fixed-force angle for particles.\n"
                                                                    "@return No return value." )
{
    object->setFixedForceAngle( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getFixedForceAngle, F32, 2, 2,  "() Gets the emitter fixed-force angle for particles.\n"
                                                                    "@return The fixed-force angle for particles." )
{
    return object->getFixedForceAngle();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setOrientationType, void, 3, 3,     "(orientationType) Sets the orientation-type of the emitter.\n"
                                                                        "@param orientationType The orientation-type to set the emitter to.  Either 'FIXED', 'ALIGNED' or 'RANDOM'.\n"
                                                                        "An orientation-type of 'FIXED' causes the particles to be orientation at a fixed angle.\n"
                                                                        "An orientation-type of 'ALIGNED' causes the particles to be orientation at the current emission angle.\n"
                                                                        "An orientation-type of 'RANDOM' causes the particles to be orientation at a fixed angle.\n"
                                                                        "@return No return value." )
{
    object->setOrientationType( ParticleAssetEmitter::getOrientationTypeEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getOrientationType, const char*, 2, 2,  "() Gets the orientation-type of the emitter.\n"
                                                                            "@return The orientation-type of the emitter." )
{
    return ParticleAssetEmitter::getOrientationTypeDescription( object->getOrientationType() );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setKeepAligned, void, 3, 3,     "(keepAligned) Sets whether to keep emitted particles aligned or not."
                                                                    "@keepAligned Whether to keep emitted particles aligned or not.\n"
                                                                    "@return No return value." )
{
    object->setKeepAligned( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getKeepAligned, bool, 2, 2, "() Gets whether to keep emitted particles aligned or not.\n"
                                                                "@return Whether to keep emitted particles aligned or not." )
{
    return object->getKeepAligned();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setAlignedAngleOffset, void, 3, 3,  "(alignAngleOffset) Sets the aligned angle offset.\n"
                                                                        "@param alignAngleOffset The aligned angle offset.\n"
                                                                        "@return No return value." )
{
    object->setAlignedAngleOffset( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getAlignedAngleOffset, F32, 2, 2,  "() Gets the aligned angle offset.\n"
                                                                        "@return The aligned angle offset." )
{
    return object->getAlignedAngleOffset();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setRandomAngleOffset, void, 3, 3, "(randomAngle) - Set Random-Orientation Angle-Offset.")
{
    object->setRandomAngleOffset( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getRandomAngleOffset, F32, 2, 2, "Get Random-Orientation Angle-Offset.")
{
    return object->getRandomAngleOffset();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setRandomArc, void, 3, 3, "(randomArc) - Set Random-Orientation Arc.")
{
    object->setRandomArc( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getRandomArc, F32, 2, 2, "Get Random-Orientation Arc.")
{
    return object->getRandomArc();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setFixedAngleOffset, void, 3, 3, "(randomAngle) - Set Fixed-Orientation Angle-Offset.")
{
    object->setFixedAngleOffset( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getFixedAngleOffset, F32, 2, 2, "Get Fixed-Orientation Angle-Offset.")
{
    return object->getFixedAngleOffset();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setPivotPoint, void, 3, 4, "(pivotX / pivotY) - Set the Pivot-Point.")
{
    // Grab the element count.
    U32 elementCount =Utility::mGetStringElementCount(argv[2]);

    // ("pivotX pivotY")
    if ( (elementCount == 2) && (argc < 4) )
    {
        object->setPivotPoint( Vector2( argv[2] ) );
        return;
    }

    // (pivotX, pivotY)
    if ( (elementCount == 1) && (argc > 3) )
    {
        object->setPivotPoint( Vector2( dAtof(argv[2]), dAtof(argv[3]) ) );
        return;
    }

    // Warn.
    Con::warnf( "ParticleAssetEmitter::setPivotPoint() - Invalid number of parameters!" );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getPivotPoint, const char*, 2, 2, "Get Pivot-Point.")
{
    return object->getPivotPoint().scriptThis();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setLinkEmissionRotation, void, 3, 3, "(linkEmissionRotation) - Set Link-Emission-Rotation Flag.")
{
    object->setLinkEmissionRotation( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getLinkEmissionRotation, bool, 2, 2, "Get Link-Emission-Rotation Flag.")
{
    return object->getLinkEmissionRotation();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setIntenseParticles, void, 3, 3, "(intenseParticles) - Set Intense-Particles Flag.")
{
    object->setIntenseParticles( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getIntenseParticles, bool, 2, 2, "Get Intense-Particles Flag.")
{
    return object->getIntenseParticles();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setSingleParticle, void, 3, 3, "(singleParticle) - Set Single-Particle Flag.")
{
    // Set Single Particle.
    object->setSingleParticle( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getSingleParticle, bool, 2, 2, "Get Single-Particle Flag.")
{
    return object->getSingleParticle();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setAttachPositionToEmitter, void, 3, 3, "(attachPositionToEmitter) - Set Attach-Position-To-Emitter Flag.")
{
    object->setAttachPositionToEmitter( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getAttachPositionToEmitter, bool, 2, 2, "Get Attach-Position-To-Emitter Flag.")
{
    return object->getAttachPositionToEmitter();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setAttachRotationToEmitter, void, 3, 3, "(attachRotationToEmitter) - Set Attach-Rotation-To-Emitter Flag.")
{
    object->setAttachRotationToEmitter( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getAttachRotationToEmitter, bool, 2, 2, "Get Attach-Rotation-To-Emitter Flag.")
{
    return object->getAttachRotationToEmitter();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setOldestInFront, void, 3, 3,   "(oldestInFront) Sets whether to render particles as oldest on front or not.\n"
                                                                    "@param oldestInFront Whether to render particles as oldest on front or not.\n"
                                                                    "@return No return value." )
{
    object->setOldestInFront( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getOldestInFront, bool, 2, 2,   "() Gets whether to render particles as oldest on front or not.\n"
                                                                    "@return Whether to render particles as oldest on front or not." )
{
    return object->getOldestInFront();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setImage, bool, 3, 4,   "(imageAssetId, [frame]) Sets the emitter to use the specified image asset Id and optional frame.\n"
                                                            "@param imageAssetId The image asset Id to use.\n"
                                                            "@param frame The frame of the image asset Id to use.  Optional.\n"
                                                            "@return Whether the operation was successful or not." )
{
    // Fetch the frame.
    const U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;

    return object->setImage( argv[2], frame );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getImage, const char*, 2, 2,    "() Gets the asset Id of the image asset assigned to the emitter.\n"
                                                                    "@return The asset Id of the image asset assigned to the emitter or nothing if no image is assigned." )
{
    return object->getImage();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setImageFrame, bool, 3, 3,   "(frame) Sets the emitter to use the specified image frame.\n"
                                                                "@param frame The frame of the image to use..\n"
                                                                "@return Whether the operation was successful or not." )
{
    return object->setImageFrame( dAtoi(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getImageFrame, S32, 2, 2,    "() Gets the asset Id of the image asset assigned to the emitter.\n"
                                                                    "@return The asset Id of the image asset assigned to the emitter or nothing if no image is assigned." )
{
    return object->getImageFrame();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setAnimation, bool, 3, 3,   "(animationAssetId) Sets the emitter to use the specified animation asset Id.\n"
                                                                "@param animationAssetId The animation asset Id to use.\n"
                                                                "@return Whether the operation was successful or not." )
{
    return object->setAnimation( argv[2] );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getAnimation, const char*, 2, 2,    "() Gets the asset Id of the animation asset assigned to the emitter.\n"
                                                                        "@return The asset Id of the animation asset assigned to the emitter or nothing if no animation is assigned." )
{
    return object->getAnimation();
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setBlendMode, void, 3, 3,   "(blendMode) Sets whether to use render blending or not.\n"
                                                                "@param blendMode Whether to use render blending or not.\n"
                                                                "@return No return value." )
{
    object->setBlendMode( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getBlendMode, bool, 2, 2,   "() Gets whether to use render blending or not.\n"
                                                                "@return Whether to use render blending or not." )
{
    return object->getBlendMode();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setSrcBlendFactor, void, 3, 3,  "(srcBlend) - Sets the source blend factory.\n"
                                                                    "@param srcBlend The source blend factor.\n"
                                                                    "@return No return Value.")
{
    // Fetch source blend factor.
    const  GLenum blendFactor = SceneObject::getSrcBlendFactorEnum(argv[2]);

    object->setSrcBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getSrcBlendFactor, const char*, 2, 2,   "() - Gets the source render blend factor.\n"
                                                                            "@return (srcBlend) The source render blend factor.")
{
   return SceneObject::getSrcBlendFactorDescription(object->getSrcBlendFactor());
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setDstBlendFactor, void, 3, 3,  "(dstBlend) - Sets the destination render blend factor.\n"
                                                                    "@param dstBlend The destination render blend factor.\n"
                                                                    "@return No return Value.")
{
    // Fetch destination blend factor.
    const GLenum blendFactor = SceneObject::getDstBlendFactorEnum(argv[2]);

    object->setDstBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getDstBlendFactor, const char*, 2, 2,   "() - Gets the destination render blend factor.\n"
                                                                            "@return (dstBlend) The destination render blend factor.")
{
   return SceneObject::getDstBlendFactorDescription(object->getDstBlendFactor());
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setAlphaTest, void, 3, 3,   "(float alpha) - Set the render alpha test threshold.\n"
                                                                "@param alpha The alpha test threshold in the range of 0.0 to 1.0.  Less than zero to disable alpha testing.\n"
                                                                "@return No return value." )

{
    object->setAlphaTest(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getAlphaTest, F32, 2, 2,    "() - Gets the render alpha test threshold.\n"
                                                                "@return The render alpha test threshold in the range of 0.0f to 1.0.  Less than zero represents disabled alpha testing.")
{
    return object->getAlphaTest();
}


//-----------------------------------------------------------------------------
/// Particle emitter fields.
//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getSelectableFieldCount, S32, 2, 2, "() Gets the number of available selectable fields.\n"
                                                                        "@return The number of available selectable fields." )
{
    return object->getParticleFields().getFields().size();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getSelectableFieldName, const char*, 3, 3, "(fieldIndex) Gets the selectable field at the specified index.\n"
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
    Con::warnf( "ParticleAssetEmitter::getSelectableFieldName() - Index '%d' is out of range.", fieldIndex );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, selectField, bool, 3, 3,   "(fieldName) Select the specified field by its name.\n"
                                                                "@param fieldName The field name to use for the selection.  Use an empty name to deselect to stop accidental changes.\n"
                                                            "@return Whether the field was successfully selected or not.")
{
    return object->getParticleFields().selectField( argv[2] ) != NULL;
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, deselectField, void, 2, 2,  "() Deselect any selected field.  If no field is selected then nothing happens.\n"
                                                                "@return No return value.")
{
    object->getParticleFields().deselectField();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getSelectedField, bool, 2, 2,   "() Gets the selected field name or nothing if no field is selected.\n"
                                                                    "@return The selected field name or nothing if no fields is selected.")
{
    // Get the selected field.
    const ParticleAssetField* pParticleAssetField = object->getParticleFields().getSelectedField();

    return pParticleAssetField == NULL ? StringTable->EmptyString : pParticleAssetField->getFieldName();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setSingleDataKey, S32, 3, 3,    "(value) Sets a single data-key at time-zero with the specified value.  All existing keys are cleared.\n"
                                                                    "@param value The value to set the key to.\n"
                                                                    "@return Returns the index of the new data-key (always zero) or -1 on failure.")
{
    return object->getParticleFields().setSingleDataKey( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, addDataKey, S32, 4, 4,  "(time, value) Add Data-Key to Graph.\n"
                                                            "@param time The key time.\n"
                                                            "@param value The value at specified time\n"
                                                            "@return Returns the index of the new data-key or -1 on failure.")
{
    return object->getParticleFields().addDataKey( dAtof(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, removeDataKey, bool, 3, 3,  "(keyIndex) Remove the data-key from the field.\n"
                                                                "@param keyIndex The index of the data-key you want to remove.\n"
                                                                "@return Whether the operation was successful or not.")
{
   return object->getParticleFields().removeDataKey( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, clearDataKeys, bool, 2, 2,  "() Clears all data-key(s) from the field.\n"
                                                                "@return Whether the operation was successful or not.")
{
   return object->getParticleFields().clearDataKeys();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setDataKeyValue, bool, 4, 4,    "(keyIndex, value) Set data-key value for the field.\n"
                                                                    "@param keyIndex The index of the key to be modified.\n"
                                                                    "@param value The value to change the key to.\n"
                                                                    "@return Whether the operation was successful or not.")
{
   // Set Data Key.
   return object->getParticleFields().setDataKey( dAtoi(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getDataKeyCount, S32, 2, 2,     "() Gets the data-key count.\n"
                                                                    "@return The number of data-keys in the currently selected field or -1 if no field is selected.")
{
   // Get Data Key Count.
   return object->getParticleFields().getDataKeyCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getDataKey, const char*, 3, 3,  "(keyIndex) Gets the data-key at the specified index from the field.\n"
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

ConsoleMethod(ParticleAssetEmitter, getMinValue, F32, 2, 2,     "() Get the minimum value for the field.\n"
                                                                "@return The minimum value for the field or always 0.0 if no field is selected." )
{
   return object->getParticleFields().getMinValue();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getMaxValue, F32, 2, 2,     "() Get the maximum value for the field.\n"
                                                                "@return The maximum value for the field or always 0.0 if no field is selected." )
{
   return object->getParticleFields().getMaxValue();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getMinTime, F32, 2, 2,      "() Get the minimum time for the field.\n"
                                                                "@return The minimum time for the field or always 0.0 if no field is selected." )
{
   return object->getParticleFields().getMinTime();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getMaxTime, F32, 2, 2,      "() Get the maximum time for the field.\n"
                                                                "@return The maximum time for the field or always 0.0 if no field is selected." )
{
   return object->getParticleFields().getMaxTime();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getFieldValue, F32, 3, 3,   "(time) Get the fields' value at the specified time.\n"
                                                                "@param time The time to sample the field value at.\n"
                                                                "@return The fields' value at the specified time or always 0.0 if no field is selected.")
{
   return object->getParticleFields().getFieldValue( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setRepeatTime, bool, 3, 3,  "(repeatTime) Sets the time period to repeat (cycle) the fields' values at.\n"
                                                                "@return Whether the operation was successful or not.")
{
   return object->getParticleFields().setRepeatTime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getRepeatTime, F32, 2, 2,   "() Gets the time period that the fields' value repeat (cycle) at.\n"
                                                                "@return The time period that the fields' value repeat (cycle) at.\n" )
{
   return object->getParticleFields().getRepeatTime();
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, setValueScale, bool, 3, 3,  "(valueScale) Set the scaling of field values retrieved from the field.  This does not alter the actual data-key values.\n"
                                                                "@param valueScale The scale for field values retrieved from the field.\n"
                                                                "@return Whether the operation was successful or not.")
{
   return object->getParticleFields().setValueScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(ParticleAssetEmitter, getValueScale, F32, 2, 2,   "() Gets the scaling of field values' retrieved from the field.\n"
                                                                "@return The scaling of field values' retrieved from the field." )
{
   return object->getParticleFields().getValueScale();
}
