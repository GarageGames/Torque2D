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

#include "sim/simBase.h"
#include "io/zip/zipArchive.h"
#include "collection/vector.h"
#include "io/streamObject.h"

#ifndef _ZIPOBJECT_H_
#define _ZIPOBJECT_H_

/// @addtogroup zip_group
// @{

//////////////////////////////////////////////////////////////////////////
/// @brief Script wrapper for Zip::ZipArchive.
//////////////////////////////////////////////////////////////////////////
class ZipObject : public SimObject
{
   typedef SimObject Parent;

protected:
   Zip::ZipArchive *mZipArchive;

   // StreamObjects are pooled and reused to avoid creating tons of SimObjects
   VectorPtr<StreamObject *> mStreamPool;

   StreamObject *createStreamObject(Stream *stream);

public:
   ZipObject();
   virtual ~ZipObject();
   DECLARE_CONOBJECT(ZipObject);

   // Methods for accessing the archive
   /// @see Zip::ZipArchive::openArchive()
   bool openArchive(const char *filename, Zip::ZipArchive::AccessMode mode = Zip::ZipArchive::Read);
   /// @see Zip::ZipArchive::closeArchive()
   void closeArchive();

   // Stream based file system style interface
   /// @see Zip::ZipArchive::openFile()
   StreamObject *openFileForRead(const char *filename);
   /// @see Zip::ZipArchive::openFile()
   StreamObject *openFileForWrite(const char *filename);
   /// @see Zip::ZipArchive::closeFile()
   void closeFile(StreamObject *stream);

   // Alternative archiver style interface
   /// @see Zip::ZipArchive::addFile()
   bool addFile(const char *filename, const char *pathInZip, bool replace = true);
   /// @see Zip::ZipArchive::extractFile()
   bool extractFile(const char *pathInZip, const char *filename);
   /// @see Zip::ZipArchive::deleteFile()
   bool deleteFile(const char *filename);


   // Methods for access the list of files
   S32 getFileEntryCount();
   const char *getFileEntry(S32 idx);
};

// @}

#endif // _ZIPOBJECT_H_
