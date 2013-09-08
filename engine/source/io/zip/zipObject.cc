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

#include "zipObject_ScriptBinding.h"

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

