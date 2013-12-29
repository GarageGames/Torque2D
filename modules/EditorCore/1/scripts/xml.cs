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

/// @class XML
/// Provides saving and loading support for the XML format.
/// 
/// @field fileObject The xmlDoc used for reading and writing.
/// @field filename The filename currently being written to or read from.
/// @field index Used when reading to store the element number that should
/// be read next when reading in arbitrary element names.
/// 
/// @see Persistence
/// @see xmlDoc

//package XMLPersistencePackage
//{

function XML::beginWrite( %this, %filename )
{
   // Validate the file.
   if( !isWriteableFileName( %filename ) )
   {
      error( "XML::beginWrite - Failed to write to file" @ %filename @ "." );
      return false;
   }
   
   // Initialize the xmlDoc
   %this.fileObject = new SimXMLDocument();
   %this.fileObject.addHeader();
   
   // Save the filename.
   %this.filename = %filename;
   
   // Success.
   return true;
}

function XML::beginRead( %this, %filename )
{
   // Initialize the xmlDoc.
   %this.fileObject = new SimXMLDocument();
   if( !%this.fileObject.loadFile( %filename ) )
      return false;
   
   // Start at element index 0.
   %this.index = 0;
   
   // This is a mock stack that holds the parent index.
   %this.parentIndex = "";
   
   return true;
}

function XML::endWrite( %this )
{
   %this.fileObject.saveFile( %this.filename );
   %this.fileObject.delete();
}

function XML::endRead( %this )
{
   %this.fileObject.delete();
}

function XML::writeHeader( %this, %documentType, %target, %version, %creator )
{
   %this.fileObject.addComment( "3 Step Studio - http://www.3stepstudio.com" );
   %this.fileObject.addComment( "Type: " @ %documentType );
   %this.fileObject.addComment( "Target: " @ %target );
   %this.fileObject.addComment( "Version: " @ %version );
   %this.fileObject.addComment( "Creator: 3 Step Studio" );
}

function XML::readHeader( %this )
{
   %this.fileObject.readComment( 1 );
   %documentType = %this.fileObject.readComment( 2 );
   %target = %this.fileObject.readComment( 3 );
   %version = %this.fileObject.readComment( 4 );
   %creator = %this.fileObject.readComment( 5 );
   
   %documentType = getSubStr( %documentType, 6, strlen( %documentType ) );
   %target = getSubStr( %target, 8, strlen( %target ) );
   %version = getSubStr( %version, 9, strlen( %version ) );
   %creator = getSubStr( %creator, 9, strlen( %creator ) );
   
   return %documentType TAB %target TAB %version TAB %creator;
}

function XML::writeDocument( %this, %document )
{
   %document.save( %this );
   
   // Write the file.
   return %this.fileObject.saveFile( %this.filename );
}

function XML::readDocument( %this, %document )
{
   // Make sure the document can be loaded.
   if( !%document.isMethod( "load" ) )
      return "";
   
   %document.load( %this );
   return %document;
}

function XML::writeClassBegin( %this, %class, %name )
{
   %this.fileObject.pushNewElement( %class );
   if( %name !$= "" )
      %this.fileObject.setAttribute( "name", %name );
}

function XML::writeClassEnd( %this )
{
   %this.fileObject.popElement();
}

function XML::readNextClass( %this )
{
   %class = "";
   %name = "";
   if( %this.fileObject.pushChildElement( %this.index ) )
   {
      // Reset the index to 0 since we are stepping down a level in the
      // heirarchy.
      %this.parentIndex = %this.index SPC %this.parentIndex;
      %this.index = 0;
      %class = %this.fileObject.elementValue();
      %name = %this.fileObject.attribute( "name" );
   }
   
   if( %class $= "" )
      return "";
   
   return trim( %class SPC %name );
}

function XML::readClassBegin( %this, %class, %index )
{
   if( %this.fileObject.pushFirstChildElement( %class ) )
   {
      if( %index $= "" ) %index = 0;
      for( %i = 0; %i < %index; %i++ )
      {
         if( !%this.fileObject.nextSiblingElement( %class ) )
         {
            %this.fileObject.popElement();
            return false;
         }
      }
      
      // Reset the index to 0 since we are stepping down a level in the
      // heirarchy.
      %this.parentIndex = %this.index SPC %this.parentIndex;
      %this.index = 0;
      return true;
   }
   
   return false;
}

function XML::readClassEnd( %this )
{
   %this.fileObject.popElement();
   %this.index = firstWord( %this.parentIndex ) + 1;
   %this.parentIndex = removeWord( %this.parentIndex, 0 );
}

function XML::writeField( %this, %field, %value )
{
   %this.fileObject.pushNewElement( %field );
   %this.fileObject.addText( %value );
   %this.fileObject.popElement();
}

function XML::readField( %this, %field )
{
   %value = "";
   if( %this.fileObject.pushFirstChildElement( %field ) )
   {
      %this.index++;
      %value = %this.fileObject.getText();
      %this.fileObject.popElement();
   }
   
   return %value;
}

function XML::writePoint2F( %this, %field, %value )
{
   %this.fileObject.pushNewElement( %field );
   %this.writeField( "X", getWord( %value, 0 ) );
   %this.writeField( "Y", getWord( %value, 1 ) );
   %this.fileObject.popElement();
}

function XML::readPoint2F( %this, %field )
{
   %value = "";
   if( %this.fileObject.pushFirstChildElement( %field ) )
   {
      // Store the index since readField will increment it twice.
      %index = %this.index;
      %value = %this.readField( "X" ) SPC %this.readField( "Y" );
      %this.fileObject.popElement();
      %this.index = %index + 1;
   }
   
   return %value;
}

function XML::writePoint3F( %this, %field, %value )
{
   %this.fileObject.pushNewElement( %field );
   %this.writeField( "X", getWord( %value, 0 ) );
   %this.writeField( "Y", getWord( %value, 1 ) );
   %this.writeField( "Z", getWord( %value, 2 ) );
   %this.fileObject.popElement();
}

function XML::readPoint3F( %this, %field )
{
   %value = "";
   if( %this.fileObject.pushFirstChildElement( %field ) )
   {
      // Store the index since readField will increment it twice.
      %index = %this.index;
      %value = %this.readField( "X" ) SPC %this.readField( "Y" ) SPC %this.readField( "Z" );
      %this.fileObject.popElement();
      %this.index = %index + 1;
   }
   
   return %value;
}

function XML::writePoint4F( %this, %field, %value )
{
   %this.fileObject.pushNewElement( %field );
   %this.writeField( "X", getWord( %value, 0 ) );
   %this.writeField( "Y", getWord( %value, 1 ) );
   %this.writeField( "Z", getWord( %value, 2 ) );
   %this.writeField( "W", getWord( %value, 3 ) );
   %this.fileObject.popElement();
}

function XML::readPoint4F( %this, %field )
{
   %value = "";
   if( %this.fileObject.pushFirstChildElement( %field ) )
   {
      // Store the index since readField will increment it twice.
      %index = %this.index;
      %value = %this.readField( "X" ) SPC %this.readField( "Y" ) SPC %this.readField( "Z" ) SPC %this.readField( "W" );
      %this.fileObject.popElement();
      %this.index = %index + 1;
   }
   
   return %value;
}

function XML::writeAttribute( %this, %field, %value )
{
   %this.fileObject.setAttribute( %field, %value );
}

function XML::readAttribute( %this, %field )
{
   return %this.fileObject.attribute( %field );
}

function XML::writeData( %this, %field, %value )
{
   %this.fileObject.pushNewElement( %field );
   %this.fileObject.addData( %value );
   %this.fileObject.popElement();
}

function XML::readData( %this, %field )
{
   %value = "";
   if( %this.fileObject.pushFirstChildElement( %field ) )
   {
      %this.index++;
      %value = %this.fileObject.getData();
      %this.fileObject.popElement();
   }
   
   return %value;
}

function XML::writeBool( %this, %field, %value )
{
   %write = "false";
   if( %value )
      %write = "true";

   %this.writeField( %field, %write );
}

function XML::readBool( %this, %field )
{
   %val = %this.readField( %field );
   if( %val $= "" )
      return "";
      
   if( %val $= "true" )
      return true;
   
   return false;
}

function XML::writeValue( %this, %value )
{
   %this.fileObject.addText( %value );
}

function XML::readValue( %this )
{
   return %this.fileObject.getText();
}

//};

//Persistence::registerFormat( "XML", XMLPersistencePackage, ".xml" );
