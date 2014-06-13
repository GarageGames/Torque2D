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

ConsoleMethodGroupBeginWithDocs(ZipObject, SimObject)

static const struct
{
   const char *strMode;
   Zip::ZipArchive::AccessMode mode;
} gModeMap[]=
{
   { "read", Zip::ZipArchive::Read },
   { "write", Zip::ZipArchive::Write },
   { "readwrite", Zip::ZipArchive::ReadWrite },
   { NULL, (Zip::ZipArchive::AccessMode)0 }
};

/*! Open a zip file
    @param filename The file's name
    @param accessMode The mode in which to open the file (default Read)
    @return Returns true on success, false otherwise
    @sa closeArchive, openFileForRead, openFileForWrite, closeFile
*/
ConsoleMethodWithDocs(ZipObject, openArchive, ConsoleBool, 3, 4, (filename, [accessMode = Read]?))
{
   Zip::ZipArchive::AccessMode mode = Zip::ZipArchive::Read;

   if(argc > 3)
   {
      for(S32 i = 0;gModeMap[i].strMode;++i)
      {
         if(dStricmp(gModeMap[i].strMode, argv[3]) == 0)
         {
            mode = gModeMap[i].mode;
            break;
         }
      }
   }

   char buf[512];
   Con::expandPath(buf, sizeof(buf), argv[2]);

   return object->openArchive(buf, mode);
}

/*! Close the zip file
    @return No return value.
*/
ConsoleMethodWithDocs(ZipObject, closeArchive, ConsoleVoid, 2, 2, ())
{
   object->closeArchive();
}

//////////////////////////////////////////////////////////////////////////

/*! Open a file within the zip for reading
    @param filename The file's name to open in current zip file
    @return The file stream ID as an integer or zero on failure.
*/
ConsoleMethodWithDocs(ZipObject, openFileForRead, ConsoleInt, 3, 3, (filename))
{
   StreamObject *stream = object->openFileForRead(argv[2]);
   return stream ? stream->getId() : 0;
}

/*! Open a file within the zip for writing
    @param filename The file's name to open in current zip file
    @return The file stream ID as an integer or 0 on failure.
*/
ConsoleMethodWithDocs(ZipObject, openFileForWrite, ConsoleInt, 3, 3, (filename))
{
   StreamObject *stream = object->openFileForWrite(argv[2]);
   return stream ? stream->getId() : 0;
}

/*! Close a file within the zip
    @param stream The file stream ID
    @return No return value.
*/
ConsoleMethodWithDocs(ZipObject, closeFile, ConsoleVoid, 3, 3, (stream))
{
   StreamObject *stream = dynamic_cast<StreamObject *>(Sim::findObject(argv[2]));
   if(stream == NULL)
   {
      Con::errorf("ZipObject::closeFile - Invalid stream specified");
      return;
   }

   object->closeFile(stream);
}

//////////////////////////////////////////////////////////////////////////

/*! Add a file to the zip
    @param filename The name of the file
    @param pathInZip The internal (to the zip) path to the file
    @param replace Set whether to replace the file if one already exists with the name in the path (default true)
    @return Returns true on success and false otherwise
*/
ConsoleMethodWithDocs(ZipObject, addFile, ConsoleBool, 4, 5, (filename, pathInZip, [replace = true]?))
{
   // Left this line commented out as it was useful when i had a problem
   //  with the zip's i was creating. [2/21/2007 justind]
   // [tom, 2/21/2007] To is now a warnf() for better visual separation in the console
  // Con::errorf("zipAdd: %s", argv[2]);
   //Con::warnf("    To: %s", argv[3]);

   return object->addFile(argv[2], argv[3], argc > 4 ? dAtob(argv[4]) : true);
}

/*! Extract a file from the zip
    @param pathInZip The internal path of the desired file
    @param filename The file's name
    @return Returns true on success and false otherwise
*/
ConsoleMethodWithDocs(ZipObject, extractFile, ConsoleBool, 4, 4, (pathInZip, filename))
{
   return object->extractFile(argv[2], argv[3]);
}

/*! Delete a file from the zip 
    @param pathInZip The full internal path of the file
    @return Returns true on success and false otherwise.
*/
ConsoleMethodWithDocs(ZipObject, deleteFile, ConsoleBool, 3, 3, (pathInZip))
{
   return object->deleteFile(argv[2]);
}

//////////////////////////////////////////////////////////////////////////

/*! Get number of files in the zip 
    @return Returns the number of files found in zip
*/
ConsoleMethodWithDocs(ZipObject, getFileEntryCount, ConsoleInt, 2, 2, ())
{
   return object->getFileEntryCount();
}

/*! Get file entry. 
    @param index Index to file entry
    @return Returns tab separated string containing filename, uncompressed size, compressed size, compression method and CRC32
*/
ConsoleMethodWithDocs(ZipObject, getFileEntry, ConsoleString, 3, 3, (index))
{
   return object->getFileEntry(dAtoi(argv[2]));
}

ConsoleMethodGroupEndWithDocs(ZipObject)
