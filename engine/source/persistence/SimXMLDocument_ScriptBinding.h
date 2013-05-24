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

ConsoleMethodGroupBeginWithDocs(SimXMLDocument, SimObject)

/*! Set this to default state at construction.
    @return No return value
*/
ConsoleMethodWithDocs(SimXMLDocument, reset, ConsoleVoid, 2, 2, ())
{
   object->reset();
}

/*! Load file from given filename.
    @param fileName The name of the desired file
    @return Returns 1 on success and 0 otherwise
*/
ConsoleMethodWithDocs(SimXMLDocument, loadFile, ConsoleInt, 3, 3, (string fileName))
{
   return object->loadFile( argv[2] );
}

/*! Save file to given filename.
    @param fileName A string presenting the filename to save the XML document as
    @return Returns 1 on success, and 0 otherwise
*/
ConsoleMethodWithDocs(SimXMLDocument, saveFile, ConsoleInt, 3, 3, (string fileName))
{
   return object->saveFile( argv[2] );
}

/*! Create document from XML string.
    @param txtXML The text of the XML document
    @return Returns 1 on success
*/
ConsoleMethodWithDocs(SimXMLDocument, parse, ConsoleInt, 3, 3, (string txtXML))
{
   return object->parse( argv[2] );
}

/*! Clear contents of XML document.
    @return No return value
*/
ConsoleMethodWithDocs(SimXMLDocument, clear, ConsoleVoid, 2, 2, ())
{
   object->clear();
}

/*! Get current error description.
    @return Returns a string with the error description
*/
ConsoleMethodWithDocs(SimXMLDocument, getErrorDesc, ConsoleString, 2, 2, ())
{
   // Create Returnable Buffer (duration of error description is unknown).
   char* pBuffer = Con::getReturnBuffer(256);
   // Format Buffer.
   dSprintf(pBuffer, 256, "%s", object->getErrorDesc());
   // Return Velocity.
   return pBuffer;
}

/*! Clear error description.
    @return No return value
*/
ConsoleMethodWithDocs(SimXMLDocument, clearError, ConsoleVoid, 2, 2, ())
{
   object->clearError();
}

/*! Push first child element with given name onto stack.
    @param name The name of the child element
    @return returns true on success, false otherwise.
*/
ConsoleMethodWithDocs(SimXMLDocument, pushFirstChildElement, ConsoleBool, 3, 3, (string name))
{
   return object->pushFirstChildElement( argv[2] );
}

/*! Push the child element at the given index onto stack.
    @param index A nonnegative integer representing the index of the child element
    @return Returns true on success, and false otherwise
*/
ConsoleMethodWithDocs(SimXMLDocument, pushChildElement, ConsoleBool, 3, 3, (int index))
{
   return object->pushChildElement( dAtoi( argv[2] ) );
}

/*! Set top element on stack to next element with given name.
    @param name The name of the element.
    @return Returns true on success, false otherwise
*/
ConsoleMethodWithDocs(SimXMLDocument, nextSiblingElement, ConsoleBool, 3, 3, (string name))
{
   return object->nextSiblingElement( argv[2] );
}

/*! Get element value if it exists.
    @return A string with the desired value, or empty if not found.
*/
ConsoleMethodWithDocs(SimXMLDocument, elementValue, ConsoleString, 2, 2, ())
{
   // Create Returnable Buffer (because duration of value is unknown).
   char* pBuffer = Con::getReturnBuffer(256);
   dSprintf(pBuffer, 256, "%s", object->elementValue());
   return pBuffer;
}

/*! Pop last element off of stack.
    @return No return value
*/
ConsoleMethodWithDocs(SimXMLDocument, popElement, ConsoleVoid, 2, 2, ())
{
   object->popElement();
}

/*! Get attribute value if it exists.
    @param attribute The desired SimXMLDocument attribute
    @return Returns the value of the attribute as a string, or an empty string on failure
*/
ConsoleMethodWithDocs(SimXMLDocument, attribute, ConsoleString, 3, 3, (string attribute))
{
   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(256);
   // Format Buffer.
   dSprintf(pBuffer, 256, "%s", object->attribute( argv[2] ));
   // Return Velocity.
   return pBuffer;
}

/*! Get attribute value if it exists.
    @param attribute The desired SimXMLDocument attribute
    @return Returns the value of the attribute converted to 32-bit floating-point value from string
*/
ConsoleMethodWithDocs(SimXMLDocument, attributeF32, ConsoleFloat, 3, 3, (string attribute))
{
   return dAtof( object->attribute( argv[2] ) );
}

/*! Get attribute value if it exists.
    @param attribute The desired SimXMLDocument attribute
    @return Returns the value of the attribute converted to 32-bit integer value from string
*/
ConsoleMethodWithDocs(SimXMLDocument, attributeS32, ConsoleInt, 3, 3, (string attribute))
{
   return dAtoi( object->attribute( argv[2] ) );
}

/*! Get true if named attribute exists.
    @param attribute The desired attribute's name
    @return Returns true if attribute exists and false otherwise
*/
ConsoleMethodWithDocs(SimXMLDocument, attributeExists, ConsoleBool, 3, 3, (string attribute))
{
   return object->attributeExists( argv[2] );
}

/*! Obtain the name of the current element's first attribute.
    @return A string with the name of the first attribute
*/
ConsoleMethodWithDocs(SimXMLDocument, firstAttribute, ConsoleString, 2, 2, ())
{
   const char* name = object->firstAttribute();

   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(dStrlen(name)+1);
   dStrcpy(pBuffer, name);
   return pBuffer;
}

/*! Obtain the name of the current element's last attribute.
    @return A string with the name of the last attribute
*/
ConsoleMethodWithDocs(SimXMLDocument, lastAttribute, ConsoleString, 2, 2, ())
{
   const char* name = object->lastAttribute();

   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(dStrlen(name)+1);
   dStrcpy(pBuffer, name);
   return pBuffer;
}

/*! Get the name of the next attribute for the current element after a call to firstAttribute().
    @return A string with the name of the next attribute
*/
ConsoleMethodWithDocs(SimXMLDocument, nextAttribute, ConsoleString, 2, 2, ())
{
   const char* name = object->nextAttribute();

   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(dStrlen(name)+1);
   dStrcpy(pBuffer, name);
   return pBuffer;
}

/*! Get the name of the previous attribute for the current element after a call to lastAttribute().
    @return A string with the name of the previous attribute
*/
ConsoleMethodWithDocs(SimXMLDocument, prevAttribute, ConsoleString, 2, 2, ())
{
   const char* name = object->prevAttribute();

   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(dStrlen(name)+1);
   dStrcpy(pBuffer, name);
   return pBuffer;
}

/*! Set attribute of top stack element to given value.
    @param attributeName The name of the attribute of the element you wish to set
    @param attributeValue The value you wish to set the given attribute to
    @return No return value.
*/
ConsoleMethodWithDocs(SimXMLDocument, setAttribute, ConsoleVoid, 4, 4, (string attributeName, string attributeValue))
{
   object->setAttribute(argv[2], argv[3]);
}

/*! Set attribute of top stack element to given value.
    @param attributeValue The value you wish to set the given attribute to
    @return No return value.
*/
ConsoleMethodWithDocs(SimXMLDocument, setObjectAttributes, ConsoleVoid, 3, 3, (string attributeValue))
{
   object->setObjectAttributes(argv[2]);
}

/*! Create new element as child of current stack element 
    and push new element on to stack.
    @param name The anme of the new element
    @return No return value
*/
ConsoleMethodWithDocs(SimXMLDocument, pushNewElement, ConsoleVoid, 3, 3, (string name))
{
   object->pushNewElement( argv[2] );
}

/*! Create new element as child of current stack element 
    and push new element on to stack.
    @param name The anme of the new element
    @return No return value
*/
ConsoleMethodWithDocs(SimXMLDocument, addNewElement, ConsoleVoid, 3, 3, (string name))
{
   object->addNewElement( argv[2] );
}

/*! Add XML header to document.
    @return No return value.
*/
ConsoleMethodWithDocs(SimXMLDocument, addHeader, ConsoleVoid, 2, 2, ())
{
   object->addHeader();
}

/*! Add the given comment as a child of current stack element.
    @param comment The desired comment to add
    @return No return value.
*/
ConsoleMethodWithDocs(SimXMLDocument, addComment, ConsoleVoid, 3, 3, (string comment))
{
   object->addComment(argv[2]);
}

/*! Returns the comment at the specified index.
    @param index The index of the desired comment as a nonnegative integer value
    @return The comment as a string
*/
ConsoleMethodWithDocs(SimXMLDocument, readComment, ConsoleString, 3, 3, (S32 index))
{
   return object->readComment( dAtoi( argv[2] ) );
}

/*! Add the given text as a child of current stack element.
    @param text The desired text to add
    @return No return value.
*/
ConsoleMethodWithDocs(SimXMLDocument, addText, ConsoleVoid, 3, 3, (string text))
{
   object->addText(argv[2]);
}

/*! Gets the text from the current stack element.
    @return Returns the desired text, or empty string on failure
*/
ConsoleMethodWithDocs(SimXMLDocument, getText, ConsoleString, 2, 2, ())
{
   const char* text = object->getText();
   if( !text )
      return "";

   char* pBuffer = Con::getReturnBuffer(dStrlen( text ) + 1);
   dStrcpy( pBuffer, text );
   return pBuffer;
}

/*! Remove any text on the current stack element.
    @return No return value
*/
ConsoleMethodWithDocs(SimXMLDocument, removeText, ConsoleVoid, 2, 2, ())
{
   object->removeText();
}

/*! Add the given text as a child of current stack element.
    @param The desired text to add
    @return No return value
*/
ConsoleMethodWithDocs(SimXMLDocument, addData, ConsoleVoid, 3, 3, (string text))
{
   object->addData(argv[2]);
}

/*! Gets the text from the current stack element.
    @return Returns the desired text or empty string if failed (no text)
*/
ConsoleMethodWithDocs(SimXMLDocument, getData, ConsoleString, 2, 2, ())
{
   const char* text = object->getData();
   if( !text )
      return "";

   char* pBuffer = Con::getReturnBuffer(dStrlen( text ) + 1);
   dStrcpy( pBuffer, text );
   return pBuffer;
}

ConsoleMethodGroupEndWithDocs(SimXMLDocument)
