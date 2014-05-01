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

ConsoleMethodGroupBeginWithDocs(Taml, SimObject)

/*! Sets the format that Taml should use to read/write.
    @param format The format to use: 'xml' or 'binary'.
    @return No return value.
*/
ConsoleMethodWithDocs(Taml, setFormat, ConsoleVoid, 3, 3, (format))
{
    // Fetch format mode.
    const Taml::TamlFormatMode formatMode = Taml::getFormatModeEnum(argv[2]);

    // Was the format valid?
    if ( formatMode == Taml::InvalidFormat )
    {
        // No, so warn.
        Con::warnf( "Taml::setFormat() - Invalid format mode used: '%s'.", argv[2] );
        return;
    }

    // Set format mode.
    object->setFormatMode( formatMode );
}

//-----------------------------------------------------------------------------

/*! Gets the format that Taml should use to read/write.
    @return The format that Taml should use to read/write.
*/
ConsoleMethodWithDocs(Taml, getFormat, ConsoleString, 2, 2, ())
{
    // Fetch format mode.
    return Taml::getFormatModeDescription( object->getFormatMode() );
}

//-----------------------------------------------------------------------------

/*! Sets whether the format type is automatically determined by the filename extension or not.
    @param autoFormat Whether the format type is automatically determined by the filename extension or not.
    @return No return value.
*/
ConsoleMethodWithDocs(Taml, setAutoFormat, ConsoleVoid, 3, 3, (autoFormat))
{
    object->setAutoFormat( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the format type is automatically determined by the filename extension or not.
    @return Whether the format type is automatically determined by the filename extension or not.
*/
ConsoleMethodWithDocs(Taml, getAutoFormat, ConsoleBool, 2, 2, ())
{
    return object->getAutoFormat();
}

//-----------------------------------------------------------------------------

/*! Sets whether to write static fields that are at their default or not.
    @param writeDefaults Whether to write static fields that are at their default or not.
    @return No return value.
*/
ConsoleMethodWithDocs(Taml, setWriteDefaults, ConsoleVoid, 3, 3, (writeDefaults))
{
    object->setWriteDefaults( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether to write static fields that are at their default or not.
    @return Whether to write static fields that are at their default or not.
*/
ConsoleMethodWithDocs(Taml, getWriteDefaults, ConsoleBool, 2, 2, ())
{
    return object->getWriteDefaults();
}

//-----------------------------------------------------------------------------

/*! Sets whether to update each type instances file-progenitor or not.
    If not updating then the progenitor stay as the script that executed the call to Taml.
    @param progenitorUpdate Whether to update each type instances file-progenitor or not.
    @return No return value.
*/
ConsoleMethodWithDocs(Taml, setProgenitorUpdate, ConsoleVoid, 3, 3, (progenitorUpdate))
{
    object->setProgenitorUpdate( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether to update each type instances file-progenitor or not.
    @return Whether to update each type instances file-progenitor or not.
*/
ConsoleMethodWithDocs(Taml, getProgenitorUpdate, ConsoleBool, 2, 2, ())
{
    return object->getProgenitorUpdate();
}

//-----------------------------------------------------------------------------

/*! Sets the extension (end of filename) used to detect the XML format.
    @param extension The extension (end of filename) used to detect the XML format.
    @return No return value.
*/
ConsoleMethodWithDocs(Taml, setAutoFormatXmlExtension, ConsoleVoid, 3, 3, (extension))
{
    object->setAutoFormatXmlExtension( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the extension (end of filename) used to detect the XML format.
    @return The extension (end of filename) used to detect the XML format.
*/
ConsoleMethodWithDocs(Taml, getAutoFormatXmlExtension, ConsoleString, 3, 3, ())
{
    return object->getAutoFormatXmlExtension();
}

//-----------------------------------------------------------------------------

/*! Sets the extension (end of filename) used to detect the Binary format.
    @param extension The extension (end of filename) used to detect the Binary format.
    @return No return value.
*/
ConsoleMethodWithDocs(Taml, setAutoFormatBinaryExtension, ConsoleVoid, 3, 3, (extension))
{
    object->setAutoFormatBinaryExtension( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the extension (end of filename) used to detect the Binary format.
    @return The extension (end of filename) used to detect the Binary format.
*/
ConsoleMethodWithDocs(Taml, getAutoFormatBinaryExtension, ConsoleString, 3, 3, ())
{
    return object->getAutoFormatBinaryExtension();
}

//-----------------------------------------------------------------------------

/*! Sets whether ZIP compression is used on binary formatting or not.
    @param compressed Whether compression is on or off.
    @return No return value.
*/
ConsoleMethodWithDocs(Taml, setBinaryCompression, ConsoleVoid, 3, 3, (compressed))
{
    // Set compression.
    object->setBinaryCompression( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether ZIP compression is used on binary formatting or not.
    @return Whether ZIP compression is used on binary formatting or not.
*/
ConsoleMethodWithDocs(Taml, getBinaryCompression, ConsoleBool, 2, 2, ())
{
    // Fetch compression.
    return object->getBinaryCompression();
}

//-----------------------------------------------------------------------------

/*! Sets whether to write JSON that is strictly compatible with RFC4627 or not.
    @param jsonStrict Whether to write JSON that is strictly compatible with RFC4627 or not.
    @return No return value.
*/
ConsoleMethodWithDocs(Taml, setJSONStrict, ConsoleVoid, 3, 3, (jsonStrict))
{
    // Set JSON Strict.
    object->setJSONStrict( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether to write JSON that is strictly compatible with RFC4627 or not.
    @return whether to write JSON that is strictly compatible with RFC4627 or not.
*/
ConsoleMethodWithDocs(Taml, getJSONStrict, ConsoleBool, 2, 2, ())
{
    // Get RFC strict.
    return object->getJSONStrict();
}
//-----------------------------------------------------------------------------

/*! Writes an object to a file using Taml.
    @param object The object to write.
    @param filename The filename to write to.
    @return Whether the write was successful or not.
*/
ConsoleMethodWithDocs(Taml, write, ConsoleBool, 4, 4, (object, filename))
{
    // Fetch filename.
    const char* pFilename = argv[3];

    // Find object.
    SimObject* pSimObject = Sim::findObject( argv[2] );

    // Did we find the object?
    if ( pSimObject == NULL )
    {
        // No, so warn.
        Con::warnf( "Taml::write() - Could not find object '%s' to write to file '%s'.", argv[2], pFilename );
        return false;
    }

    return object->write( pSimObject, pFilename );
}

//-----------------------------------------------------------------------------

/*! Read an object from a file using Taml.
    @param filename The filename to read from.
    @return (Object) The object read from the file or an empty string if read failed.
*/
ConsoleMethodWithDocs(Taml, read, ConsoleString, 3, 3, (filename))
{
    // Fetch filename.
    const char* pFilename = argv[2];

    // Read object.
    SimObject* pSimObject = object->read( pFilename );

    // Did we find the object?
    if ( pSimObject == NULL )
    {
        // No, so warn.
        Con::warnf( "Taml::read() - Could not read object from file '%s'.", pFilename );
        return StringTable->EmptyString;
    }

    return pSimObject->getIdString();
}

//-----------------------------------------------------------------------------

ConsoleMethodGroupEndWithDocs(Taml)


/*! Writes an object to a file using Taml.
    @param object The object to write.
    @param filename The filename to write to.
    @param format The file format to use.  Optional: Defaults to 'xml'.  Can be set to 'binary'.
    @param compressed Whether ZIP compression is used on binary formatting or not.  Optional: Defaults to 'true'.
    @return Whether the write was successful or not.
*/
ConsoleFunctionWithDocs(TamlWrite, ConsoleBool, 3, 5, (object, filename, [format]?, [compressed]?))
{
    // Fetch filename.
    const char* pFilename = argv[2];

    // Find object.
    SimObject* pSimObject = Sim::findObject( argv[1] );

    // Did we find the object?
    if ( pSimObject == NULL )
    {
        // No, so warn.
        Con::warnf( "TamlWrite() - Could not find object '%s' to write to file '%s'.", argv[2], pFilename );
        return false;
    }

    Taml taml;

    // Was the format mode specified?
    if ( argc > 3 )
    {
        // Yes, so set it.
        taml.setFormatMode( Taml::getFormatModeEnum( argv[3] ) );  

        // Was binary compression specified?
        if ( argc > 4 )
        {
            // Yes, so is the format mode binary?
            if ( taml.getFormatMode() == Taml::BinaryFormat )
            {
                // Yes, so set binary compression.
                taml.setBinaryCompression( dAtob(argv[4]) );
            }
            else
            {
                // No, so warn.
                Con::warnf( "TamlWrite() - Setting binary compression is only valid for XML formatting." );
            }
        }

		// Turn-off auto-formatting.
		taml.setAutoFormat( false );
    }

    // Write.
    return taml.write( pSimObject, pFilename );
}

//-----------------------------------------------------------------------------

/*! Read an object from a file using Taml.
    @param filename The filename to read from.
    @param format The file format to use.  Optional: Defaults to 'xml'.  Can be set to 'binary'.
    @return (Object) The object read from the file or an empty string if read failed.
*/
ConsoleFunctionWithDocs(TamlRead, ConsoleString, 2, 4, (filename, [format]?))
{
    // Fetch filename.
    const char* pFilename = argv[1];

    // Set the format mode.
    Taml taml;

	// Was a format mode specified?
	if ( argc > 2 )
	{
		// Yes, so set it.
		taml.setFormatMode( Taml::getFormatModeEnum( argv[2] ) );  

		// Turn-off auto-formatting.
		taml.setAutoFormat( false );
	}

    // Read object.
    SimObject* pSimObject = taml.read( pFilename );

    // Did we find the object?
    if ( pSimObject == NULL )
    {
        // No, so warn.
        Con::warnf( "TamlRead() - Could not read object from file '%s'.", pFilename );
        return StringTable->EmptyString;
    }

    return pSimObject->getIdString();
}

//-----------------------------------------------------------------------------

/*! Generate a TAML schema file of all engine types.
    The schema file is specified using the console variable ' TAML_SCHEMA_VARIABLE '.
    @return Whether the schema file was writtent or not.
*/
ConsoleFunctionWithDocs(GenerateTamlSchema, ConsoleBool, 1, 1, ())
{
    // Generate the schema.
    return Taml::generateTamlSchema();
}
