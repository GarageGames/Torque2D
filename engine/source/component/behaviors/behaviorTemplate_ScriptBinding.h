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

ConsoleMethod(BehaviorTemplate, createInstance, S32, 2, 2, "() - Create an instance of this behavior.\n"
              "@return (BehaviorInstance inst) The behavior instance created")
{
   BehaviorInstance *inst = object->createInstance();
   return inst ? inst->getId() : 0;
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, addBehaviorField, bool, 5, 7, "(fieldName, desc, type, [defaultValue, userData])\n"
              "Adds a named BehaviorField to a Behavior Template\n"
              "@param fieldName The name of this field\n"
              "@param desc The Description of this field\n"
              "@param type The DataType for this field (default, int, float, Point2F, bool, enum, Object, keybind, color)\n"
              "@param defaultValue The Default value for this field\n"
              "@param userData An extra data field that can be used for custom data on a per-field basis<br>Usage for default types<br>"
              "-enum: a TAB separated list of possible values<br>"
              "-object: the scene-object type that are valid choices for the field.  The object types observe inheritance, so if you have a SceneObject field you will be able to choose t2dStaticSrpites, t2dAnimatedSprites, etc.\n"
              "@return Whether the field was added or not.\n")
{
    const char *defValue = argc > 5 ? argv[5] : NULL;
    const char *typeInfo = argc > 6 ? argv[6] : NULL;
   
    return object->addBehaviorField(argv[2], argv[3], argv[4], defValue, typeInfo);
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, getBehaviorFieldCount, S32, 2, 2,   "() - Get the number of behavior fields.\n"
                                                                    "@return Returns the number of behavior fields.\n")
{
    return object->getBehaviorFieldCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, getBehaviorField, const char *, 3, 3,   "(int fieldIndex) - Gets a tab-delimited description of the behavior field.\n"
                                                                        "@param fieldIndex The index of the behavior field.\n"
                                                                        "@return A tab-delimited description of the behavior field of the format <Name><Type><DefaultValue> or an empty string if behavior field is not found.\n")
{

    // Fetch behavior field.
    BehaviorTemplate::BehaviorField* pField = object->getBehaviorField( dAtoi(argv[2]) );

    // Was the field found?
    if( !pField )
    {
        // No, so warn.
        Con::warnf("getBehaviorField() - Could not find the behavior field '%s' on behavior '%s'", argv[2], object->getFriendlyName() );
        return StringTable->EmptyString;
    }

    // Format and return behavior field.
    char* pBuffer = Con::getReturnBuffer(1024);
    dSprintf(pBuffer, 1024, "%s\t%s\t%s", pField->mName, pField->mType, pField->mDefaultValue);
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, getBehaviorFieldUserData, const char *, 3, 3,   "(int fieldIndex) - Gets the UserData associated with a field by index in the field list\n"
                                                                                "@param fieldIndex The index of the behavior field.\n"
                                                                                "@return Returns a string representing the user data of this field\n")
{
    // Fetch behavior field.
    BehaviorTemplate::BehaviorField* pField = object->getBehaviorField( dAtoi(argv[2]) );

    // Was the field found?
    if( !pField )
    {
        // No, so warn.
        Con::warnf("getBehaviorFieldUserData() - Could not find the behavior field '%s' on behavior '%s'", argv[2], object->getFriendlyName() );
        return StringTable->EmptyString;
    }

    return pField->mUserData;
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, getBehaviorFieldDescription, const char *, 3, 3,    "(int fieldIndex) - Gets the description of the field.\n"
                                                                                    "@param fieldIndex The index of the behavior field.\n"
                                                                                    "@return Returns the field description.\n")
{
    // Fetch behavior field.
    BehaviorTemplate::BehaviorField* pField = object->getBehaviorField( dAtoi(argv[2]) );

    // Was the field found?
    if( !pField )
    {
        // No, so warn.
        Con::warnf("getBehaviorFieldDescription() - Could not find the behavior field '%s' on behavior '%s'", argv[2], object->getFriendlyName() );
        return StringTable->EmptyString;
    }
    return pField->mDescription ? pField->mDescription : StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, addBehaviorOutput, bool, 5, 5,  "(outputName, label, description) - Adds a behavior output.\n"
                                                                "@param outputName The output name to use.\n"
                                                                "@param label The short label name to show in the editor.\n"
                                                                "@param description The long description to show in the editor.\n"
                                                                "@return Whether the output was added or not.\n")
{
    // Add behavior output.
    return object->addBehaviorOutput( argv[2], argv[3], argv[4] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, getBehaviorOutputCount, S32, 2, 2,  "() - Get the number of behavior outputs.\n"
                                                                    "@return Returns the number of behavior outputs.\n")
{
    return object->getBehaviorOutputCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, getBehaviorOutput, const char *, 3, 3,  "(int outputIndex) - Gets a comma-delimited description of the behavior output.\n"
                                                                        "@param outputIndex The index of the behavior output.\n"
                                                                        "@return A comma-delimited description of the behavior output of the format <Name>,<Label><,Description> or an empty string if behavior output is not found.\n")
{

    // Fetch behavior output.
    BehaviorTemplate::BehaviorPortOutput* pPortOutput = object->getBehaviourOutput( dAtoi(argv[2]) );

    // Was the output found?
    if( !pPortOutput )
    {
        // No, so warn.
        Con::warnf("getBehaviorOutput() - Could not find the behavior output '%s' on behavior '%s'", argv[2], object->getFriendlyName() );
        return StringTable->EmptyString;
    }

    // Format and return behavior output.
    char* pBuffer = Con::getReturnBuffer(1024);
    dSprintf(pBuffer, 1024, "%s,%s,%s", pPortOutput->mName, pPortOutput->mLabel, pPortOutput->mDescription );
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, hasBehaviorOutput, bool, 3, 3,  "(outputName) - Gets whether the behavior has the behavior output or not.\n"
                                                                "@param outputName The output name to check.\n"
                                                                "@return Whether the behavior has the behavior output or not.\n")
{
    return object->hasBehaviorOutput( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, addBehaviorInput, bool, 5, 5,   "(inputName, label, description) - Adds a behavior input.\n"
                                                                "@param inputName The input name to use.\n"
                                                                "@param label The short label name to show in the editor.\n"
                                                                "@param description The long description to show in the editor.\n"
                                                                "@return Whether the input was added or not.\n")
{
    // Add behavior input.
    return object->addBehaviorInput( argv[2], argv[3], argv[4] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, getBehaviorInputCount, S32, 2, 2,   "() - Get the number of behavior inputs.\n"
                                                                    "@return Returns the number of behavior inputs.\n")
{
    return object->getBehaviorInputCount();
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, getBehaviorInput, const char *, 3, 3,   "(int inputIndex) - Gets a comma-delimited description of the behavior input.\n"
                                                                        "@param inputIndex The index of the behavior input.\n"
                                                                        "@return A comma-delimited description of the behavior input of the format <Name>,<Label><,Description> or an empty string if behavior input is not found.\n")
{

    // Fetch behavior input.
    BehaviorTemplate::BehaviorPortInput* pPortInput = object->getBehaviourInput( dAtoi(argv[2]) );

    // Was the input found?
    if( !pPortInput )
    {
        // No, so warn.
        Con::warnf("getBehaviorOutput() - Could not find the behavior input '%s' on behavior '%s'", argv[2], object->getFriendlyName() );
        return StringTable->EmptyString;
    }

    // Format and return behavior input.
    char* pBuffer = Con::getReturnBuffer(1024);
    dSprintf(pBuffer, 1024, "%s,%s,%s", pPortInput->mName, pPortInput->mLabel, pPortInput->mDescription );
    return pBuffer;
}

//-----------------------------------------------------------------------------

ConsoleMethod(BehaviorTemplate, hasBehaviorInput, bool, 3, 3,  "(inputName) - Gets whether the behavior has the behavior input or not.\n"
                                                                "@param inputName The input name to check.\n"
                                                                "@return Whether the behavior has the behavior input or not.\n")
{
    return object->hasBehaviorInput( argv[2] );
}

