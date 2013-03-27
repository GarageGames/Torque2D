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

ConsoleMethod(Taml, setFormat, void, 3, 3,  "(format) - Sets the format that Taml should use to read/write.\n"
                                            "@param format The format to use: 'xml' or 'binary'.\n"
                                            "@return No return value.")
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

ConsoleMethod(Taml, getFormat, const char*, 2, 2,   "() - Gets the format that Taml should use to read/write.\n"
                                                    "@return The format that Taml should use to read/write.")
{
    // Fetch format mode.
    return Taml::getFormatModeDescription( object->getFormatMode() );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, setAutoFormat, void, 3, 3,  "(autoFormat) Sets whether the format type is automatically determined by the filename extension or not.\n"
                                                "@param autoFormat Whether the format type is automatically determined by the filename extension or not.\n"
                                                "@return No return value." )
{
    object->setAutoFormat( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, getAutoFormat, bool, 2, 2,  "() Gets whether the format type is automatically determined by the filename extension or not.\n"
                                                "@return Whether the format type is automatically determined by the filename extension or not." )
{
    return object->getAutoFormat();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, setWriteDefaults, void, 3, 3,   "(writeDefaults) Sets whether to write static fields that are at their default or not.\n"
                                                    "@param writeDefaults Whether to write static fields that are at their default or not.\n"
                                                    "@return No return value." )
{
    object->setWriteDefaults( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, getWriteDefaults, bool, 2, 2,   "() Gets whether to write static fields that are at their default or not.\n"
                                                    "@return Whether to write static fields that are at their default or not." )
{
    return object->getWriteDefaults();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, setProgenitorUpdate, void, 3, 3,    "(progenitorUpdate) Sets whether to update each type instances file-progenitor or not.\n"
                                                        "If not updating then the progenitor stay as the script that executed the call to Taml.\n"
                                                        "@param progenitorUpdate Whether to update each type instances file-progenitor or not.\n"
                                                        "@return No return value." )
{
    object->setProgenitorUpdate( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, getProgenitorUpdate, bool, 2, 2,    "() Gets whether to update each type instances file-progenitor or not.\n"
                                                        "@return Whether to update each type instances file-progenitor or not." )
{
    return object->getProgenitorUpdate();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, setAutoFormatXmlExtension, void, 3, 3,  "(extension) Sets the extension (end of filename) used to detect the XML format.\n"
                                                            "@param extension The extension (end of filename) used to detect the XML format.\n"
                                                            "@return No return value." )
{
    object->setAutoFormatXmlExtension( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, getAutoFormatXmlExtension, const char*, 3, 3,   "() Gets the extension (end of filename) used to detect the XML format.\n"
                                                                    "@return The extension (end of filename) used to detect the XML format." )
{
    return object->getAutoFormatXmlExtension();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, setAutoFormatBinaryExtension, void, 3, 3,   "(extension) Sets the extension (end of filename) used to detect the Binary format.\n"
                                                                "@param extension The extension (end of filename) used to detect the Binary format.\n"
                                                                "@return No return value." )
{
    object->setAutoFormatBinaryExtension( argv[2] );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, getAutoFormatBinaryExtension, const char*, 3, 3,    "() Gets the extension (end of filename) used to detect the Binary format.\n"
                                                                        "@return The extension (end of filename) used to detect the Binary format." )
{
    return object->getAutoFormatBinaryExtension();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, setBinaryCompression, void, 3, 3,   "(compressed) - Sets whether ZIP compression is used on binary formatting or not.\n"
                                                        "@param compressed Whether compression is on or off.\n"
                                                        "@return No return value.")
{
    // Set compression.
    object->setBinaryCompression( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, getBinaryCompression, bool, 2, 2,  "() - Gets whether ZIP compression is used on binary formatting or not.\n"
                                                        "@return Whether ZIP compression is used on binary formatting or not.")
{
    // Fetch compression.
    return object->getBinaryCompression();
}

//-----------------------------------------------------------------------------

ConsoleMethod(Taml, write, bool, 4, 4,  "(object, filename) - Writes an object to a file using Taml.\n"
                                        "@param object The object to write.\n"
                                        "@param filename The filename to write to.\n"
                                        "@return Whether the write was successful or not.")
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

ConsoleMethod(Taml, read, const char*, 3, 3,    "(filename) - Read an object from a file using Taml.\n"
                                                "@param filename The filename to read from.\n"
                                                "@return (Object) The object read from the file or an empty string if read failed.")
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

ConsoleFunction(TamlWrite, bool, 3, 5,  "(object, filename, [format], [compressed]) - Writes an object to a file using Taml.\n"
                                        "@param object The object to write.\n"
                                        "@param filename The filename to write to.\n"
                                        "@param format The file format to use.  Optional: Defaults to 'xml'.  Can be set to 'binary'.\n"
                                        "@param compressed Whether ZIP compression is used on binary formatting or not.  Optional: Defaults to 'true'.\n"
                                        "@return Whether the write was successful or not.")
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

ConsoleFunction(TamlRead, const char*, 2, 4,    "(filename, [format]) - Read an object from a file using Taml.\n"
                                                "@param filename The filename to read from.\n"
                                                "@param format The file format to use.  Optional: Defaults to 'xml'.  Can be set to 'binary'.\n"
                                                "@return (Object) The object read from the file or an empty string if read failed.")
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

ConsoleFunction(GenerateTamlSchema, bool, 1, 1, "() - Generate a TAML schema file of all engine types.\n"
                                                "The schema file is specified using the console variable '" TAML_SCHEMA_VARIABLE "'.\n"
                                                "@return Whether the schema file was writtent or not." )
{
    // Generate the schema.
    return Taml::generateTamlSchema();
}