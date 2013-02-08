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

////////////////////////////////////////////////////////////////////////////////
// XML Document Object
////////////////////////////////////////////////////////////////////////////////

#include "persistence/SimXMLDocument.h"
#include "memory/frameAllocator.h"
#include "sim/simBase.h"
#include "console/consoleInternal.h"
#include "io/resource/resourceManager.h"
#include "io/fileStream.h"

IMPLEMENT_CONOBJECT(SimXMLDocument);

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
SimXMLDocument::SimXMLDocument():
m_qDocument(0),
m_CurrentAttribute(0)
{
}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
SimXMLDocument::~SimXMLDocument()
{
}

// -----------------------------------------------------------------------------
// Included for completeness.
// -----------------------------------------------------------------------------
bool SimXMLDocument::processArguments(S32 argc, const char** argv)
{
   if(0 == argc)
   {
      return true;
   }
   else 
   {
      return true;
   }
   return false;
}

// -----------------------------------------------------------------------------
// Script constructor.
// -----------------------------------------------------------------------------
bool SimXMLDocument::onAdd()
{
   if(!Parent::onAdd())
   {
      return false;
   }

   if(!m_qDocument)
   {
      m_qDocument = new TiXmlDocument();
   }
   return true;
}

// -----------------------------------------------------------------------------
// Script destructor.
// -----------------------------------------------------------------------------
void SimXMLDocument::onRemove()
{
   Parent::onRemove();
   if(m_qDocument)
   {
      m_qDocument->Clear();
      delete(m_qDocument);
   }
}

// -----------------------------------------------------------------------------
// Initialize peristent fields (datablocks).
// -----------------------------------------------------------------------------
void SimXMLDocument::initPersistFields()
{
   Parent::initPersistFields();
}

// -----------------------------------------------------------------------------
// Set this to default state at construction.
// -----------------------------------------------------------------------------
void SimXMLDocument::reset(void)
{
   m_qDocument->Clear();
   m_paNode.clear();
   m_CurrentAttribute = 0;
}
ConsoleMethod(SimXMLDocument, reset, void, 2, 2,
              "() Set this to default state at construction.\n"
              "@return No return value")
{
   object->reset();
}

// -----------------------------------------------------------------------------
// Get true if file loads successfully.
// -----------------------------------------------------------------------------
S32 SimXMLDocument::loadFile(const char* rFileName)
{
   reset();

   return m_qDocument->LoadFile(rFileName);
}
ConsoleMethod(SimXMLDocument, loadFile, S32, 3, 3, "(string fileName) Load file from given filename.\n"
              "@param fileName The name of the desired file\n"
              "@return Returns 1 on success and 0 otherwise")
{
   return object->loadFile( argv[2] );
}

// -----------------------------------------------------------------------------
// Get true if file saves successfully.
// -----------------------------------------------------------------------------
S32 SimXMLDocument::saveFile(const char* rFileName)
{
   char buffer[1024];
   Con::expandPath(buffer, sizeof(buffer), rFileName);

   if(buffer == NULL || *buffer == 0)
      return false;

   FileStream strm;
   if(!ResourceManager->openFileForWrite(strm, buffer, FileStream::Write))
   {
      Con::errorf(ConsoleLogEntry::General, "Unable to open file '%s for writing.", buffer);
      return false;
   }

   bool retVal = m_qDocument->SaveFile(strm);//m_qDocument->SaveFile( buffer );

   // close the stream
   strm.close();

   return retVal;
}
ConsoleMethod(SimXMLDocument, saveFile, S32, 3, 3, "(string fileName) Save file to given filename.\n"
              "@param fileName A string presenting the filename to save the XML document as\n"
              "@return Returns 1 on success, and 0 otherwise")
{
   return object->saveFile( argv[2] );
}

// -----------------------------------------------------------------------------
// Get true if XML text parses correctly.
// -----------------------------------------------------------------------------
S32 SimXMLDocument::parse(const char* rText)
{
   reset();
   m_qDocument->Parse( rText );
   return 1;
}
ConsoleMethod(SimXMLDocument, parse, S32, 3, 3, "(string txtXML) Create document from XML string.\n"
              "@param txtXML The text of the XML document\n"
              "@return Returns 1 on success")
{
   return object->parse( argv[2] );
}

// -----------------------------------------------------------------------------
// Clear contents of XML document.
// -----------------------------------------------------------------------------
void SimXMLDocument::clear(void)
{
   reset();
}
ConsoleMethod(SimXMLDocument, clear, void, 2, 2, "() Clear contents of XML document.\n"
              "@return No return value")
{
   object->clear();
}

// -----------------------------------------------------------------------------
// Get error description of XML document.
// -----------------------------------------------------------------------------
const char* SimXMLDocument::getErrorDesc(void) const
{
   if(!m_qDocument)
   {
      return StringTable->insert("No document");
   }
   return m_qDocument->ErrorDesc();
}
ConsoleMethod(SimXMLDocument, getErrorDesc, const char*, 2, 2,
              "() Get current error description.\n"
              "@return Returns a string with the error description")
{
   // Create Returnable Buffer (duration of error description is unknown).
   char* pBuffer = Con::getReturnBuffer(256);
   // Format Buffer.
   dSprintf(pBuffer, 256, "%s", object->getErrorDesc());
   // Return Velocity.
   return pBuffer;
}

// -----------------------------------------------------------------------------
// Clear error description of this.
// -----------------------------------------------------------------------------
void SimXMLDocument::clearError(void)
{
   m_qDocument->ClearError();
}
ConsoleMethod(SimXMLDocument, clearError, void, 2, 2,
              "() Clear error description.\n"
              "@return No return value")
{
   object->clearError();
}

// -----------------------------------------------------------------------------
// Get true if first child element was successfully pushed onto stack.
// -----------------------------------------------------------------------------
bool SimXMLDocument::pushFirstChildElement(const char* rName)
{
   // Clear the current attribute pointer
   m_CurrentAttribute = 0;

   // Push the first element found under the current element of the given name
   TiXmlElement* pElement;
   if(!m_paNode.empty())
   {
      const int iLastElement = m_paNode.size() - 1;
      TiXmlElement* pNode = m_paNode[iLastElement];
      if(!pNode)
      {
         return false;
      }
      pElement = pNode->FirstChildElement(rName);
   }
   else
   {
      if(!m_qDocument)
      {
         return false;
      }
      pElement = m_qDocument->FirstChildElement(rName);
   }

   if(!pElement)
   {
      return false;
   }
   m_paNode.push_back(pElement);
   return true;
}
ConsoleMethod(SimXMLDocument, pushFirstChildElement, bool, 3, 3,
              "(string name) Push first child element with given name onto stack.\n"
              "@param name The name of the child element"
              "@return returns true on success, false otherwise.")
{
   return object->pushFirstChildElement( argv[2] );
}

// -----------------------------------------------------------------------------
// Get true if first child element was successfully pushed onto stack.
// -----------------------------------------------------------------------------
bool SimXMLDocument::pushChildElement(S32 index)
{
   // Clear the current attribute pointer
   m_CurrentAttribute = 0;

   // Push the first element found under the current element of the given name
   TiXmlElement* pElement;
   if(!m_paNode.empty())
   {
      const int iLastElement = m_paNode.size() - 1;
      TiXmlElement* pNode = m_paNode[iLastElement];
      if(!pNode)
      {
         return false;
      }
      pElement = pNode->FirstChildElement();
      for( S32 i = 0; i < index; i++ )
      {
         if( !pElement )
            return false;

         pElement = pElement->NextSiblingElement();
      }
   }
   else
   {
      if(!m_qDocument)
      {
         return false;
      }
      pElement = m_qDocument->FirstChildElement();
      for( S32 i = 0; i < index; i++ )
      {
         if( !pElement )
            return false;

         pElement = pElement->NextSiblingElement();
      }
   }

   if(!pElement)
   {
      return false;
   }
   m_paNode.push_back(pElement);
   return true;
}
ConsoleMethod(SimXMLDocument, pushChildElement, bool, 3, 3,
              "(int index) Push the child element at the given index onto stack.\n"
              "@param index A nonnegative integer representing the index of the child element\n"
              "@return Returns true on success, and false otherwise")
{
   return object->pushChildElement( dAtoi( argv[2] ) );
}

// -----------------------------------------------------------------------------
// Convert top stack element into its next sibling element.
// -----------------------------------------------------------------------------
bool SimXMLDocument::nextSiblingElement(const char* rName)
{
   // Clear the current attribute pointer
   m_CurrentAttribute = 0;

   // Attempt to find the next sibling element
   if(m_paNode.empty())
   {
      return false;
   }
   const int iLastElement = m_paNode.size() - 1;
   TiXmlElement*& pElement = m_paNode[iLastElement];
   if(!pElement)
   {
      return false;
   }

   pElement = pElement->NextSiblingElement(rName);
   if(!pElement)
   {
      return false;
   }

   return true;
}
ConsoleMethod(SimXMLDocument, nextSiblingElement, bool, 3, 3,
              "(string name) Set top element on stack to next element with given name.\n"
              "@param name The name of the element.\n"
              "@return Returns true on success, false otherwise")
{
   return object->nextSiblingElement( argv[2] );
}

// -----------------------------------------------------------------------------
// Get element value if it exists.  Used to extract a text node from the element.
// for example.
// -----------------------------------------------------------------------------
const char* SimXMLDocument::elementValue()
{
   if(m_paNode.empty())
   {
      return StringTable->EmptyString;
   }
   const int iLastElement = m_paNode.size() - 1;
   TiXmlElement* pNode = m_paNode[iLastElement];
   if(!pNode)
   {
      return StringTable->EmptyString;
   }

   return pNode->Value();
}
ConsoleMethod(SimXMLDocument, elementValue, const char*, 2, 2,
              "() Get element value if it exists.\n"
              "@return A string with the desired value, or empty if not found.")
{
   // Create Returnable Buffer (because duration of value is unknown).
   char* pBuffer = Con::getReturnBuffer(256);
   dSprintf(pBuffer, 256, "%s", object->elementValue());
   return pBuffer;
}

// -----------------------------------------------------------------------------
// Pop last element off of stack.
// -----------------------------------------------------------------------------
void SimXMLDocument::popElement(void)
{
   m_paNode.pop_back();
}
ConsoleMethod(SimXMLDocument, popElement, void, 2, 2,
              "() Pop last element off of stack.\n"
              "@return No return value")
{
   object->popElement();
}

// -----------------------------------------------------------------------------
// Get attribute value if it exists.
// -----------------------------------------------------------------------------
const char* SimXMLDocument::attribute(const char* rAttribute)
{
   if(m_paNode.empty())
   {
      return StringTable->EmptyString;
   }
   const int iLastElement = m_paNode.size() - 1;
   TiXmlElement* pNode = m_paNode[iLastElement];
   if(!pNode)
   {
      return StringTable->EmptyString;
   }

   if(!pNode->Attribute(rAttribute))
   {
      return StringTable->EmptyString;
   }

   return pNode->Attribute(rAttribute);
}
ConsoleMethod(SimXMLDocument, attribute, const char*, 3, 3,
              "(string attribute) Get attribute value if it exists.\n"
              "@param attribute The desired SimXMLDocument attribute\n"
              "@return Returns the value of the attribute as a string, or an empty string on failure")
{
   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(256);
   // Format Buffer.
   dSprintf(pBuffer, 256, "%s", object->attribute( argv[2] ));
   // Return Velocity.
   return pBuffer;
}
ConsoleMethod(SimXMLDocument, attributeF32, F32, 3, 3,
              "(string attribute) Get attribute value if it exists.\n"
              "@param attribute The desired SimXMLDocument attribute\n"
              "@return Returns the value of the attribute converted to 32-bit floating-point value from string")
{
   return dAtof( object->attribute( argv[2] ) );
}
ConsoleMethod(SimXMLDocument, attributeS32, S32, 3, 3,
              "(string attribute) Get attribute value if it exists.\n"
              "@param attribute The desired SimXMLDocument attribute\n"
              "@return Returns the value of the attribute converted to 32-bit integer value from string")
{
   return dAtoi( object->attribute( argv[2] ) );
}

// -----------------------------------------------------------------------------
// Get true if given attribute exists.
// -----------------------------------------------------------------------------
bool SimXMLDocument::attributeExists(const char* rAttribute)
{
   if(m_paNode.empty())
   {
      return false;
   }
   const int iLastElement = m_paNode.size() - 1;
   TiXmlElement* pNode = m_paNode[iLastElement];
   if(!pNode)
   {
      return false;
   }

   if(!pNode->Attribute(rAttribute))
   {
      return false;
   }

   return true;
}
ConsoleMethod(SimXMLDocument, attributeExists, bool, 3, 3,
              "(string attribute) Get true if named attribute exists.\n"
              "@param attribute The desired attribute's name\n"
              "@return Returns true if attribute exists and false otherwise")
{
   return object->attributeExists( argv[2] );
}

// -----------------------------------------------------------------------------
// Obtain the name of the current element's first attribute
// -----------------------------------------------------------------------------
const char* SimXMLDocument::firstAttribute()
{
   // Get the current element
   if(m_paNode.empty())
   {
      return StringTable->EmptyString;
   }
   const int iLastElement = m_paNode.size() - 1;
   TiXmlElement* pNode = m_paNode[iLastElement];
   if(!pNode)
   {
      return StringTable->EmptyString;
   }

   // Gets its first attribute, if any
   m_CurrentAttribute = pNode->FirstAttribute();
   if(!m_CurrentAttribute)
   {
      return StringTable->EmptyString;
   }

   return m_CurrentAttribute->Name();
}
ConsoleMethod(SimXMLDocument, firstAttribute, const char*, 2, 2,
              "() Obtain the name of the current element's first attribute.\n"
              "@return A string with the name of the first attribute")
{
   const char* name = object->firstAttribute();

   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(dStrlen(name)+1);
   dStrcpy(pBuffer, name);
   return pBuffer;
}

// -----------------------------------------------------------------------------
// Obtain the name of the current element's last attribute
// -----------------------------------------------------------------------------
const char* SimXMLDocument::lastAttribute()
{
   // Get the current element
   if(m_paNode.empty())
   {
      return StringTable->EmptyString;
   }
   const int iLastElement = m_paNode.size() - 1;
   TiXmlElement* pNode = m_paNode[iLastElement];
   if(!pNode)
   {
      return StringTable->EmptyString;
   }

   // Gets its last attribute, if any
   m_CurrentAttribute = pNode->LastAttribute();
   if(!m_CurrentAttribute)
   {
      return StringTable->EmptyString;
   }

   return m_CurrentAttribute->Name();
}
ConsoleMethod(SimXMLDocument, lastAttribute, const char*, 2, 2,
              "() Obtain the name of the current element's last attribute.\n"
              "@return A string with the name of the last attribute")
{
   const char* name = object->lastAttribute();

   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(dStrlen(name)+1);
   dStrcpy(pBuffer, name);
   return pBuffer;
}

// -----------------------------------------------------------------------------
// Get the name of the next attribute for the current element after a call
// to firstAttribute().
// -----------------------------------------------------------------------------
const char* SimXMLDocument::nextAttribute()
{
   if(!m_CurrentAttribute)
   {
      return StringTable->EmptyString;
   }

   // Gets its next attribute, if any
   m_CurrentAttribute = m_CurrentAttribute->Next();
   if(!m_CurrentAttribute)
   {
      return StringTable->EmptyString;
   }

   return m_CurrentAttribute->Name();
}
ConsoleMethod(SimXMLDocument, nextAttribute, const char*, 2, 2,
              "() Get the name of the next attribute for the current element after a call to firstAttribute().\n"
              "@return A string with the name of the next attribute")
{
   const char* name = object->nextAttribute();

   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(dStrlen(name)+1);
   dStrcpy(pBuffer, name);
   return pBuffer;
}

// -----------------------------------------------------------------------------
// Get the name of the previous attribute for the current element after a call
// to lastAttribute().
// -----------------------------------------------------------------------------
const char* SimXMLDocument::prevAttribute()
{
   if(!m_CurrentAttribute)
   {
      return StringTable->EmptyString;
   }

   // Gets its next attribute, if any
   m_CurrentAttribute = m_CurrentAttribute->Previous();
   if(!m_CurrentAttribute)
   {
      return StringTable->EmptyString;
   }

   return m_CurrentAttribute->Name();
}
ConsoleMethod(SimXMLDocument, prevAttribute, const char*, 2, 2,
              "() Get the name of the previous attribute for the current element after a call to lastAttribute().\n"
              "@return A string with the name of the previous attribute")
{
   const char* name = object->prevAttribute();

   // Create Returnable Buffer (because duration of attribute is unknown).
   char* pBuffer = Con::getReturnBuffer(dStrlen(name)+1);
   dStrcpy(pBuffer, name);
   return pBuffer;
}

// -----------------------------------------------------------------------------
// Set attribute of top stack element to given value.
// -----------------------------------------------------------------------------
void SimXMLDocument::setAttribute(const char* rAttribute, const char* rVal)
{
   if(m_paNode.empty())
   {
      return;
   }

   const int iLastElement = m_paNode.size() - 1;
   TiXmlElement* pElement = m_paNode[iLastElement];
   if(!pElement)
   {
      return;
   }
   pElement->SetAttribute(rAttribute, rVal);
}
ConsoleMethod(SimXMLDocument, setAttribute, void, 4, 4,
              "(string attributeName, string attributeValue) Set attribute of top stack element to given value.\n"
              "@param attributeName The name of the attribute of the element you wish to set\n"
              "@param attributeValue The value you wish to set the given attribute to\n"
              "@return No return value.")
{
   object->setAttribute(argv[2], argv[3]);
}

// -----------------------------------------------------------------------------
// Set attribute of top stack element to given value.
// -----------------------------------------------------------------------------
void SimXMLDocument::setObjectAttributes(const char* objectID)
{
   if( !objectID || !objectID[0] )
      return;

   if(m_paNode.empty())
      return;

   SimObject *pObject = Sim::findObject( objectID );

   if( pObject == NULL )
      return;

   const int iLastElement = m_paNode.size() - 1;
   TiXmlElement* pElement = m_paNode[iLastElement];
   if(!pElement)
      return;

   char textbuf[1024];
   TiXmlElement field( "Field" );
   TiXmlElement group( "FieldGroup" );
   pElement->SetAttribute( "Name", pObject->getName() );


      // Iterate over our filed list and add them to the XML document...
      AbstractClassRep::FieldList fieldList = pObject->getFieldList();
      AbstractClassRep::FieldList::iterator itr;
      for(itr = fieldList.begin(); itr != fieldList.end(); itr++)
      {

         if( itr->type == AbstractClassRep::DepricatedFieldType ||
            itr->type == AbstractClassRep::StartGroupFieldType ||
            itr->type == AbstractClassRep::EndGroupFieldType) continue;

         // Not an Array
         if(itr->elementCount == 1)
         {
            // get the value of the field as a string.
            ConsoleBaseType *cbt = ConsoleBaseType::getType(itr->type);

            const char *val = Con::getData(itr->type, (void *) (((const char *)pObject) + itr->offset), 0, itr->table, itr->flag);

            // Make a copy for the field check.
            if (!val)
               continue;

            FrameTemp<char> valCopy( dStrlen( val ) + 1 );
            dStrcpy( (char *)valCopy, val );

            if (!pObject->writeField(itr->pFieldname, valCopy))
               continue;

            val = valCopy;


            expandEscape(textbuf, val);

            if( !pObject->writeField( itr->pFieldname, textbuf ) )
               continue;

            field.SetValue( "Property" );
            field.SetAttribute( "name",  itr->pFieldname );
            if( cbt != NULL )
               field.SetAttribute( "type", cbt->getTypeName() );
            else
               field.SetAttribute( "type", "TypeString" );
            field.SetAttribute( "data", textbuf );

            pElement->InsertEndChild( field );

            continue;
         }
      }

      //// IS An Array
      //for(U32 j = 0; S32(j) < f->elementCount; j++)
      //{

      //   // If the start of a group create an element for the group and
      //   // the our chache to it
      //   const char *val = Con::getData(itr->type, (void *) (((const char *)pObject) + itr->offset), j, itr->table, itr->flag);

      //   // Make a copy for the field check.
      //   if (!val)
      //      continue;

      //   FrameTemp<char> valCopy( dStrlen( val ) + 1 );
      //   dStrcpy( (char *)valCopy, val );

      //   if (!pObject->writeField(itr->pFieldname, valCopy))
      //      continue;

      //   val = valCopy;

      //      // get the value of the field as a string.
      //      ConsoleBaseType *cbt = ConsoleBaseType::getType(itr->type);
      //      const char * dstr = Con::getData(itr->type, (void *)(((const char *)pObject) + itr->offset), 0, itr->table, itr->flag);
      //      if(!dstr)
      //         dstr = "";
      //      expandEscape(textbuf, dstr);


      //      if( !pObject->writeField( itr->pFieldname, dstr ) )
      //         continue;

      //      field.SetValue( "Property" );
      //      field.SetAttribute( "name",  itr->pFieldname );
      //      if( cbt != NULL )
      //         field.SetAttribute( "type", cbt->getTypeName() );
      //      else
      //         field.SetAttribute( "type", "TypeString" );
      //      field.SetAttribute( "data", textbuf );

      //      pElement->InsertEndChild( field );
      //}

}
ConsoleMethod(SimXMLDocument, setObjectAttributes, void, 3, 3,
              "(string attributeValue) Set attribute of top stack element to given value.\n"
              "@param attributeValue The value you wish to set the given attribute to\n"
              "@return No return value.")
{
   object->setObjectAttributes(argv[2]);
}

// -----------------------------------------------------------------------------
// Create a new element and set to child of current stack element.
// New element is placed on top of element stack.
// -----------------------------------------------------------------------------
void SimXMLDocument::pushNewElement(const char* rName)
{    
   TiXmlElement cElement( rName );
   TiXmlElement* pStackTop = 0;
   if(m_paNode.empty())
   {
      pStackTop = dynamic_cast<TiXmlElement*>
         (m_qDocument->InsertEndChild( cElement ) );
   }
   else
   {
      const int iFinalElement = m_paNode.size() - 1;
      TiXmlElement* pNode = m_paNode[iFinalElement];
      if(!pNode)
      {
         return;
      }
      pStackTop = dynamic_cast<TiXmlElement*>
         (pNode->InsertEndChild( cElement ));
   }
   if(!pStackTop)
   {
      return;
   }
   m_paNode.push_back(pStackTop);
}
ConsoleMethod(SimXMLDocument, pushNewElement, void, 3, 3,
              "(string name) Create new element as child of current stack element "
              "and push new element on to stack.\n"
              "@param name The anme of the new element\n"
              "@return No return value")
{
   object->pushNewElement( argv[2] );
}

// -----------------------------------------------------------------------------
// Create a new element and set to child of current stack element.
// New element is placed on top of element stack.
// -----------------------------------------------------------------------------
void SimXMLDocument::addNewElement(const char* rName)
{    
   TiXmlElement cElement( rName );
   TiXmlElement* pStackTop = 0;
   if(m_paNode.empty())
   {
      pStackTop = dynamic_cast<TiXmlElement*>
         (m_qDocument->InsertEndChild( cElement ));
      if(!pStackTop)
      {
         return;
      }
      m_paNode.push_back(pStackTop);
      return;
   }

   const int iParentElement = m_paNode.size() - 2;
   if(iParentElement < 0)
   {
      pStackTop = dynamic_cast<TiXmlElement*>
         (m_qDocument->InsertEndChild( cElement ));
      if(!pStackTop)
      {
         return;
      }
      m_paNode.push_back(pStackTop);
      return;
   }
   else
   {
      TiXmlElement* pNode = m_paNode[iParentElement];
      if(!pNode)
      {
         return;
      }   
      pStackTop = dynamic_cast<TiXmlElement*>
         (pNode->InsertEndChild( cElement ));
      if(!pStackTop)
      {
         return;
      }

      // Overwrite top stack position.
      const int iFinalElement = m_paNode.size() - 1;
      m_paNode[iFinalElement] = pStackTop;
      //pNode = pStackTop;
   }
}
ConsoleMethod(SimXMLDocument, addNewElement, void, 3, 3,
              "(string name) Create new element as child of current stack element "
              "and push new element on to stack.\n"
              "@param name The anme of the new element\n"
              "@return No return value")
{
   object->addNewElement( argv[2] );
}

// -----------------------------------------------------------------------------
// Write XML document declaration.
// -----------------------------------------------------------------------------
void SimXMLDocument::addHeader(void)
{
   TiXmlDeclaration cDeclaration("1.0", "utf-8", "yes");
   m_qDocument->InsertEndChild(cDeclaration);
}
ConsoleMethod(SimXMLDocument, addHeader, void, 2, 2, "() Add XML header to document.\n"
              "@return No return value.")
{
   object->addHeader();
}

void SimXMLDocument::addComment(const char* comment)
{
   TiXmlComment cComment;
   cComment.SetValue(comment);

   if(m_paNode.empty()) 
   {
       Con::warnf("Cannot add comment without any elements: '%s'", comment);
       return;
   }

    const int iFinalElement = m_paNode.size() - 1;
    TiXmlElement* pNode = m_paNode[iFinalElement];
    if(!pNode)
    {
        return;
    }
    pNode->InsertEndChild( cComment );
}
ConsoleMethod(SimXMLDocument, addComment, void, 3, 3, "(string comment) Add the given comment as a child of current stack element.\n"
              "@param comment The desired comment to add\n"
              "@return No return value.")
{
   object->addComment(argv[2]);
}

const char* SimXMLDocument::readComment( S32 index )
{
   // Clear the current attribute pointer
   m_CurrentAttribute = 0;

   // Push the first element found under the current element of the given name
   if(!m_paNode.empty())
   {
      const int iLastElement = m_paNode.size() - 1;
      TiXmlElement* pNode = m_paNode[iLastElement];
      if(!pNode)
      {
         return "";
      }
      TiXmlNode* node = pNode->FirstChild();
      for( S32 i = 0; i < index; i++ )
      {
         if( !node )
            return "";

         node = node->NextSiblingElement();
      }

      if( node )
      {
         TiXmlComment* comment = node->ToComment();
         if( comment )
            return comment->Value();
      }
   }
   else
   {
      if(!m_qDocument)
      {
         return "";
      }
      TiXmlNode* node = m_qDocument->FirstChild();
      for( S32 i = 0; i < index; i++ )
      {
         if( !node )
            return "";

         node = node->NextSibling();
      }

      if( node )
      {
         TiXmlComment* comment = node->ToComment();
         if( comment )
            return comment->Value();
      }
   }
   return "";
}
ConsoleMethod(SimXMLDocument, readComment, const char*, 3, 3, "(S32 index) Returns the comment at the specified index.\n"
              "@param index The index of the desired comment as a nonnegative integer value\n"
              "@return The comment as a string")
{
   return object->readComment( dAtoi( argv[2] ) );
}

void SimXMLDocument::addText(const char* text)
{
   if(m_paNode.empty())
      return;

   const int iFinalElement = m_paNode.size() - 1;
   TiXmlElement* pNode = m_paNode[iFinalElement];
   if(!pNode)
      return;

   TiXmlText cText(text);
   pNode->InsertEndChild( cText );
}

ConsoleMethod(SimXMLDocument, addText, void, 3, 3, "(string text) Add the given text as a child of current stack element.\n"
              "@param text The desired text to add\n"
              "@return No return value.")
{
   object->addText(argv[2]);
}

const char* SimXMLDocument::getText()
{
   if(m_paNode.empty())
      return "";

   const int iFinalElement = m_paNode.size() - 1;
   TiXmlNode* pNode = m_paNode[iFinalElement];
   if(!pNode)
      return "";

   TiXmlText* text = pNode->FirstChild()->ToText();
   if( !text )
      return "";

   return text->Value();
}

ConsoleMethod(SimXMLDocument, getText, const char*, 2, 2, "() Gets the text from the current stack element.\n"
              "@return Returns the desired text, or empty string on failure")
{
   const char* text = object->getText();
   if( !text )
      return "";

   char* pBuffer = Con::getReturnBuffer(dStrlen( text ) + 1);
   dStrcpy( pBuffer, text );
   return pBuffer;
}

void SimXMLDocument::removeText()
{
   if(m_paNode.empty())
      return;

   const int iFinalElement = m_paNode.size() - 1;
   TiXmlElement* pNode = m_paNode[iFinalElement];
   if(!pNode)
      return;

   TiXmlText* text = pNode->FirstChild()->ToText();
   if( !text )
      return;

   pNode->RemoveChild(text);
}

ConsoleMethod(SimXMLDocument, removeText, void, 2, 2, "() Remove any text on the current stack element.\n"
              "@return No return value\n")
{
   object->removeText();
}

void SimXMLDocument::addData(const char* text)
{
   if(m_paNode.empty())
      return;

   const int iFinalElement = m_paNode.size() - 1;
   TiXmlElement* pNode = m_paNode[iFinalElement];
   if(!pNode)
      return;

   TiXmlText cText(text);
   pNode->InsertEndChild( cText );
}

ConsoleMethod(SimXMLDocument, addData, void, 3, 3, "(string text) Add the given text as a child of current stack element.\n"
              "@param The desired text to add\n"
              "@return No return value")
{
   object->addData(argv[2]);
}

const char* SimXMLDocument::getData()
{
   if(m_paNode.empty())
      return "";

   const int iFinalElement = m_paNode.size() - 1;
   TiXmlNode* pNode = m_paNode[iFinalElement];
   if(!pNode)
      return "";

   TiXmlText* text = pNode->FirstChild()->ToText();
   if( !text )
      return "";

   return text->Value();
}

ConsoleMethod(SimXMLDocument, getData, const char*, 2, 2, "() Gets the text from the current stack element.\n"
              "@return Returns the desired text or empty string if failed (no text)")
{
   const char* text = object->getData();
   if( !text )
      return "";

   char* pBuffer = Con::getReturnBuffer(dStrlen( text ) + 1);
   dStrcpy( pBuffer, text );
   return pBuffer;
}

////EOF/////////////////////////////////////////////////////////////////////////
