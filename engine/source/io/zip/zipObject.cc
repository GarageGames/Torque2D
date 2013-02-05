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

#include "io/zip/zipObject.h"
#include "memory/safeDelete.h"

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

ZipObject::ZipObject()
{
   mZipArchive = NULL;
}

ZipObject::~ZipObject()
{
   closeArchive();
}

IMPLEMENT_CONOBJECT(ZipObject);

//////////////////////////////////////////////////////////////////////////
// Protected Methods
//////////////////////////////////////////////////////////////////////////

StreamObject *ZipObject::createStreamObject(Stream *stream)
{
   for(S32 i = 0;i < mStreamPool.size();++i)
   {
      StreamObject *so = mStreamPool[i];

      if(so == NULL)
      {
         // Reuse any free locations in the pool
         so = new StreamObject(stream);
         so->registerObject();
         mStreamPool[i] = so;
         return so;
      }
      
      if(so->getStream() == NULL)
      {
         // Existing unused stream, update it and return it
         so->setStream(stream);
         return so;
      }
   }

   // No free object found, create a new one
   StreamObject *so = new StreamObject(stream);
   so->registerObject();
   mStreamPool.push_back(so);

   return so;
}

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

bool ZipObject::openArchive(const char *filename, Zip::ZipArchive::AccessMode mode /* = Read */)
{
   closeArchive();

   mZipArchive = new Zip::ZipArchive;
   if(mZipArchive->openArchive(filename, mode))
      return true;

   SAFE_DELETE(mZipArchive);
   return false;
}

void ZipObject::closeArchive()
{
   if(mZipArchive == NULL)
      return;

   for(S32 i = 0;i < mStreamPool.size();++i)
   {
      StreamObject *so = mStreamPool[i];
      if(so && so->getStream() != NULL)
         closeFile(so);
      
      SAFE_DELETE_OBJECT(mStreamPool[i]);
   }
   mStreamPool.clear();

   mZipArchive->closeArchive();
   SAFE_DELETE(mZipArchive);
}

//////////////////////////////////////////////////////////////////////////

StreamObject * ZipObject::openFileForRead(const char *filename)
{
   if(mZipArchive == NULL)
      return NULL;

   Stream *stream;
   if((stream = mZipArchive->openFile(filename, Zip::ZipArchive::Read)))
      return createStreamObject(stream);

   return NULL;
}

StreamObject * ZipObject::openFileForWrite(const char *filename)
{
   if(mZipArchive == NULL)
      return NULL;

   Stream *stream;
   if((stream = mZipArchive->openFile(filename, Zip::ZipArchive::Write)))
      return createStreamObject(stream);

   return NULL;
}

void ZipObject::closeFile(StreamObject *stream)
{
   if(mZipArchive == NULL)
      return;

#ifdef TORQUE_DEBUG
   bool found = false;
   for(S32 i = 0;i < mStreamPool.size();++i)
   {
      StreamObject *so = mStreamPool[i];
      if(so && so == stream)
      {
         found = true;
         break;
      }
   }

   AssertFatal(found, "ZipObject::closeFile() - Attempting to close stream not opened by this ZipObject");
#endif

   mZipArchive->closeFile(stream->getStream());
   stream->setStream(NULL);
}

//////////////////////////////////////////////////////////////////////////

bool ZipObject::addFile(const char *filename, const char *pathInZip, bool replace /* = true */)
{
   return mZipArchive->addFile(filename, pathInZip, replace);
}

bool ZipObject::extractFile(const char *pathInZip, const char *filename)
{
   return mZipArchive->extractFile(pathInZip, filename);
}

bool ZipObject::deleteFile(const char *filename)
{
   return mZipArchive->deleteFile(filename);
}

//////////////////////////////////////////////////////////////////////////

S32 ZipObject::getFileEntryCount()
{
   return mZipArchive ? mZipArchive->numEntries() : 0;
}

const char *ZipObject::getFileEntry(S32 idx)
{
   if(mZipArchive == NULL)
      return "";

   const Zip::CentralDir &dir = (*mZipArchive)[idx];
   char buffer[1024];
   dSprintf(buffer, sizeof(buffer), "%s\t%d\t%d\t%d\t%08x",
            dir.mFilename, dir.mUncompressedSize, dir.mCompressedSize,
            dir.mCompressMethod, dir.mCRC32);

   return StringTable->insert(buffer, true);
}

//////////////////////////////////////////////////////////////////////////
// Console Methods
//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(ZipObject, openArchive, bool, 3, 4, "(filename, [accessMode = Read]) Open a zip file"
              "@param filename The file's name\n"
              "@param accessMode The mode in which to open the file (default Read)\n"
              "@return Returns true on success, false otherwise"
              "@sa closeArchive, openFileForRead, openFileForWrite, closeFile" )
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

ConsoleMethod(ZipObject, closeArchive, void, 2, 2, "() Close the zip file\n"
              "@return No return value.")
{
   object->closeArchive();
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(ZipObject, openFileForRead, S32, 3, 3, "(filename) Open a file within the zip for reading\n"
              "@param filename The file's name to open in current zip file\n"
              "@return The file stream ID as an integer or zero on failure.")
{
   StreamObject *stream = object->openFileForRead(argv[2]);
   return stream ? stream->getId() : 0;
}

ConsoleMethod(ZipObject, openFileForWrite, S32, 3, 3, "(filename) Open a file within the zip for writing\n"
              "@param filename The file's name to open in current zip file\n"
              "@return The file stream ID as an integer or 0 on failure.")
{
   StreamObject *stream = object->openFileForWrite(argv[2]);
   return stream ? stream->getId() : 0;
}

ConsoleMethod(ZipObject, closeFile, void, 3, 3, "(stream) Close a file within the zip"
              "@param stream The file stream ID\n"
              "@return No return value.")
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

ConsoleMethod(ZipObject, addFile, bool, 4, 5, "(filename, pathInZip[, replace = true]) Add a file to the zip\n"
              "@param filename The name of the file\n"
              "@param pathInZip The internal (to the zip) path to the file\n"
              "@param replace Set whether to replace the file if one already exists with the name in the path (default true)"
              "@return Returns true on success and false otherwise")
{
   // Left this line commented out as it was useful when i had a problem
   //  with the zip's i was creating. [2/21/2007 justind]
   // [tom, 2/21/2007] To is now a warnf() for better visual separation in the console
  // Con::errorf("zipAdd: %s", argv[2]);
   //Con::warnf("    To: %s", argv[3]);

   return object->addFile(argv[2], argv[3], argc > 4 ? dAtob(argv[4]) : true);
}

ConsoleMethod(ZipObject, extractFile, bool, 4, 4, "(pathInZip, filename) Extract a file from the zip"
              "@param pathInZip The internal path of the desired file\n"
              "@param filename The file's name\n"
              "@return Returns true on success and false otherwise")
{
   return object->extractFile(argv[2], argv[3]);
}

ConsoleMethod(ZipObject, deleteFile, bool, 3, 3, "(pathInZip) Delete a file from the zip" 
              "@param pathInZip The full internal path of the file\n"
              "@return Returns true on success and false otherwise.")
{
   return object->deleteFile(argv[2]);
}

//////////////////////////////////////////////////////////////////////////

ConsoleMethod(ZipObject, getFileEntryCount, S32, 2, 2, "() Get number of files in the zip\n" 
              "@return Returns the number of files found in zip")
{
   return object->getFileEntryCount();
}

ConsoleMethod(ZipObject, getFileEntry, const char *, 3, 3, "(index) Get file entry.\n" 
              "@param index Index to file entry\n"
              "@return Returns tab separated string containing filename, uncompressed size, compressed size, compression method and CRC32")
{
   return object->getFileEntry(dAtoi(argv[2]));
}
