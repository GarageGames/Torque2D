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

#include "platform/platform.h"
#include "io/stream.h"
#include "io/fileStream.h"
#include "io/filterStream.h"
#include "io/zip/zipCryptStream.h"
#include "algorithm/crc.h"
#include "io/resource/resourceManager.h"

#include "console/console.h"

#include "io/zip/zipArchive.h"
#include "io/zip/compressor.h"
#include "io/zip/zipTempStream.h"
#include "io/zip/zipStatFilter.h"

#ifdef TORQUE_ZIP_AES
#include "core/zipAESCryptStream.h"
#include "io/zip/crypto/aes.h"
#endif

#include "memory/safeDelete.h"

namespace Zip
{

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////

ZipArchive::ZipArchive()
{
   mStream = NULL;
   mMode = Read;
   
   mDiskStream = NULL;

   mFilename = NULL;

   mRoot = NULL;
}

ZipArchive::~ZipArchive()
{
   closeArchive();
}

//////////////////////////////////////////////////////////////////////////
// Protected Methods
//////////////////////////////////////////////////////////////////////////

bool ZipArchive::readCentralDirectory()
{
   mEntries.clear();
   SAFE_DELETE(mRoot);
   mRoot = new ZipEntry;
   mRoot->mName = StringTable->EmptyString;
   mRoot->mIsDirectory = true;
   mRoot->mCD.setFilename("");

   if(! mEOCD.findInStream(mStream))
      return false;

   if(! mEOCD.read(mStream))
      return false;

   if(mEOCD.mDiskNum != mEOCD.mStartCDDiskNum ||
      mEOCD.mNumEntriesInThisCD != mEOCD.mTotalEntriesInCD)
   {
      if(isVerbose())
         Con::errorf("ZipArchive::readCentralDirectory - %s: Zips that span multiple disks are not supported.", mFilename ? mFilename : "<no filename>");
      return false;
   }

   if(! mStream->setPosition(mEOCD.mCDOffset))
      return false;

   for(S32 i = 0;i < mEOCD.mNumEntriesInThisCD;++i)
   {
      ZipEntry *ze = new ZipEntry;
      if(! ze->mCD.read(mStream))
      {
         delete ze;

         if(isVerbose())
            Con::errorf("ZipArchive::readCentralDirectory - %s: Error reading central directory.", mFilename ? mFilename : "<no filename>");
         return false;
      }

      insertEntry(ze);
   }

   return true;
}

//////////////////////////////////////////////////////////////////////////

void ZipArchive::insertEntry(ZipEntry *ze)
{
   char path[1024];
   dStrncpy(path, ze->mCD.mFilename, sizeof(path));
   path[sizeof(path) - 1] = 0;

   for(U32 i = 0;i < dStrlen(path);++i)
   {
      if(path[i] == '\\')
         path[i] = '/';
   }

   ZipEntry *root = mRoot;

   char *ptr = path, *slash = NULL;
   do
   {
   	slash = dStrchr(ptr, '/');
      if(slash)
      {
         // Add the directory
         *slash = 0;

         ZipEntry *newEntry;
         if((newEntry = root->mChildren.retrieve(ptr)) == NULL)
         {
            newEntry = new ZipEntry;
            newEntry->mParent = root;
            newEntry->mName = StringTable->insert(ptr, true);
            newEntry->mIsDirectory = true;
            newEntry->mCD.setFilename(path);

            root->mChildren.insert(newEntry, ptr);
         }

         root = newEntry;
         
         *slash = '/';
         ptr = slash + 1;
      }
      else
      {
         // Add the file.
         if(*ptr)
         {
            ze->mIsDirectory = false;
            ze->mName = StringTable->insert(ptr, true);
            ze->mParent = root;
            root->mChildren.insert(ze, ptr);
            mEntries.push_back(ze);
         }
         else
         {
            // [tom, 2/6/2007] If ptr is empty, this was a directory entry. Since
            // we created a new entry for it above, we need to delete the old
            // pointer otherwise it will leak as it won't have got inserted.

            delete ze;
         }
      }
   } while(slash);
}

void ZipArchive::removeEntry(ZipEntry *ze)
{
   if(ze == mRoot)
   {
      // [tom, 2/1/2007] We don't want to remove the root as it should always
      // be removed through closeArchive()
      AssertFatal(0, "ZipArchive::removeEntry - Attempting to remove the root");
      return;
   }

   // Can't iterate the hash table, so we can't do this safely
   AssertFatal(!ze->mIsDirectory, "ZipArchive::removeEntry - Cannot remove a directory");

   // See if we have a temporary file for this entry
   VectorPtr<ZipTempStream *>::iterator i;
   for(i = mTempFiles.begin();i != mTempFiles.end();++i)
   {
      if((*i)->getCentralDir() == &ze->mCD)
      {
         SAFE_DELETE(*i);
         mTempFiles.erase(i);

         break;
      }
   }
   
   // Remove from the tree
   VectorPtr<ZipEntry *>::iterator j;
   for(j = mEntries.begin();j != mEntries.end();++j)
   {
      if(*j == ze)
      {
         mEntries.erase(j);
         break;
      }
   }

   // [tom, 2/2/2007] This must be last, as ze is no longer valid once it's
   // removed from the parent.
   ZipEntry *z = ze->mParent->mChildren.remove(ze->mName);
   delete z;
}

//////////////////////////////////////////////////////////////////////////

CentralDir *ZipArchive::findFileInfo(const char *filename)
{
   ZipEntry *ze = findZipEntry(filename);
   return ze ? &ze->mCD : NULL;
}

ZipArchive::ZipEntry *ZipArchive::findZipEntry(const char *filename)
{
   char path[1024];
   dStrncpy(path, filename, sizeof(path));
   path[sizeof(path) - 1] = 0;

   for(U32 i = 0;i < dStrlen(path);++i)
   {
      if(path[i] == '\\')
         path[i] = '/';
   }

   ZipEntry *root = mRoot;

   char *ptr = path, *slash = NULL;
   do
   {
      slash = dStrchr(ptr, '/');
      if(slash)
      {
         // Find the directory
         *slash = 0;

         ZipEntry *newEntry;
         if((newEntry = root->mChildren.retrieve(ptr)) == NULL)
            return NULL;

         root = newEntry;

         ptr = slash + 1;
      }
      else
      {
         // Find the file
         ZipEntry *ze;
         if((ze = root->mChildren.retrieve(ptr)) != NULL)
            return ze;
      }
   } while(slash);

   return NULL;
}

//////////////////////////////////////////////////////////////////////////

Stream *ZipArchive::createNewFile(const char *filename, Compressor *method)
{
   ZipEntry *ze = new ZipEntry;
   ze->mIsDirectory = false;
   ze->mCD.setFilename(filename);
   insertEntry(ze);

   ZipTempStream *stream = new ZipTempStream(&ze->mCD);
   if(stream->open())
   {
      Stream *retStream = method->createWriteStream(&ze->mCD, stream);
      if(retStream == NULL)
      {
         delete stream;
         return NULL;
      }

      ZipStatFilter *filter = new ZipStatFilter(&ze->mCD);
      if(! filter->attachStream(retStream))
      {
         delete filter;
         delete retStream;
         delete stream;
         return NULL;
      }

      ze->mCD.mCompressMethod = method->getMethod();
      ze->mCD.mInternalFlags |= CDFileOpen;

      return filter;
   }

   return NULL;
}

void ZipArchive::updateFile(ZipTempStream *stream)
{
   CentralDir *cd = stream->getCentralDir();
   
   // [tom, 1/23/2007] Uncompressed size and CRC32 are updated by ZipStatFilter
   cd->mCompressedSize = stream->getStreamSize();
   cd->mInternalFlags |= CDFileDirty;
   cd->mInternalFlags &= ~CDFileOpen;

   // Upper byte should be zero, lower is version as major * 10 + minor
   cd->mVersionMadeBy = (TORQUE_GAME_ENGINE / 100) & 0xff;
   cd->mExtractVer = 20;

   U32 dosTime = currentTimeToDOSTime();
   cd->mModTime = dosTime & 0x0000ffff;
   cd->mModDate = (dosTime & 0xffff0000) >> 16;

   mTempFiles.push_back(stream);
}

//////////////////////////////////////////////////////////////////////////

U32 ZipArchive::localTimeToDOSTime(const Platform::LocalTime &t)
{
   U16 year = t.year;
   if(year > 80)
      year -= 80;

   return (((t.monthday) + (32 * (t.month+1)) + (512 * year)) << 16) | ((t.sec/2) + (32* t.min) + (2048 * (U32)t.hour));
}

U32 ZipArchive::currentTimeToDOSTime()
{
   Platform::LocalTime t;
   Platform::getLocalTime(t);
   return localTimeToDOSTime(t);
}

//////////////////////////////////////////////////////////////////////////

// [tom, 1/24/2007] The general idea here is we want to create a new file,
// copy any data from the old zip file and add the new stuff. Once the new
// zip is created, delete the old one and rename the new one.

bool ZipArchive::rebuildZip()
{
   char newZipName[1024];
   FileStream tempFile;
   Stream *zipFile = mStream;

   // FIXME [tom, 1/24/2007] Temporary for expediting testing
   if(mFilename == NULL)
      return false;

  if(mMode == ReadWrite)
  {
     dSprintf(newZipName, sizeof(newZipName), "%s.new", mFilename);

     if(! tempFile.open(newZipName, mMode == Write ? FileStream::Write : FileStream::ReadWrite))
        return false;

     zipFile = &tempFile;
  }

   // Copy any unmodified files
   for(S32 i = 0;i < mEntries.size();++i)
   {
      ZipEntry *entry = mEntries[i];

      // [tom, 1/24/2007] Directories are internal only for lookup purposes
      if( entry->mIsDirectory || ((entry->mCD.mInternalFlags & (CDFileDirty | CDFileDeleted)) != 0) )
         continue;

      copyFileToNewZip(&entry->mCD, zipFile);
   }

   // Copy any dirty files
   for(S32 i = 0;i < mTempFiles.size();++i)
   {
      ZipTempStream *zts = mTempFiles[i];

      writeDirtyFileToNewZip(zts, zipFile);
      zts->close();
      delete zts;
      mTempFiles[i] = NULL;
   }
   mTempFiles.clear();

   // Write central directory
   mEOCD.mCDOffset = zipFile->getPosition();
   mEOCD.mNumEntriesInThisCD = 0;
   
   for(S32 i = 0;i < mEntries.size();++i)
   {
      ZipEntry *entry = mEntries[i];

      // [tom, 1/24/2007] Directories are internal only for lookup purposes
      if(entry->mIsDirectory || (entry->mCD.mInternalFlags & CDFileDeleted) != 0)
         continue;

      ++mEOCD.mNumEntriesInThisCD;
      if(! entry->mCD.write(zipFile))
         break;
   }

   mEOCD.mCDSize = zipFile->getPosition() - mEOCD.mCDOffset;
   mEOCD.mTotalEntriesInCD = mEOCD.mNumEntriesInThisCD;

   mEOCD.mDiskNum = 0;
   mEOCD.mStartCDDiskNum = 0;

   mEOCD.write(zipFile);

   if(mMode == ReadWrite)
   {
      // Close file, replace old zip with it
      tempFile.close();

      // [tom, 2/1/2007] The disk stream must be closed else we can't rename
      // the file. Since rebuildZip() is only called from closeArchive() this
      // should be safe.
      if(mDiskStream)
      {
         mDiskStream->close();

         delete mDiskStream;
         mDiskStream = NULL;
      }

      char oldRename[512];
      dSprintf(oldRename, sizeof(oldRename), "%s.old", mFilename);

      if(! Platform::fileRename(mFilename, oldRename))
         return false;

      if(! Platform::fileRename(newZipName, mFilename))
         return false;

      Platform::fileDelete(oldRename);
   }
   return true;
}

bool ZipArchive::writeDirtyFileToNewZip(ZipTempStream *fileStream, Stream *zipStream)
{
   CentralDir *cdir = fileStream->getCentralDir();
   FileHeader fh(*cdir);
   fh.mFilename = NULL;
   fh.setFilename(cdir->mFilename);

   cdir->mLocalHeadOffset = zipStream->getPosition();

   // Write header and file
   if(! fh.write(zipStream))
      return false;

   if(! fileStream->rewind())
      return false;

   return zipStream->copyFrom(fileStream);
}

bool ZipArchive::copyFileToNewZip(CentralDir *cdir, Stream *newZipStream)
{
   // [tom, 1/24/2007] Using the stored compressor allows us to copy the raw
   // data regardless of compression method without having to re-compress it.
   Compressor *comp = Compressor::findCompressor(Stored);
   if(comp == NULL)
      return false;

   if(! mStream->setPosition(cdir->mLocalHeadOffset))
      return false;

   // Copy file header
   // FIXME [tom, 1/24/2007] This will currently not copy the extra fields
   FileHeader fh;
   if(! fh.read(mStream))
      return false;

   cdir->mLocalHeadOffset = newZipStream->getPosition();

   if(! fh.write(newZipStream))
      return false;

   // Copy file data
   Stream *readS = comp->createReadStream(cdir, mStream);
   if(readS == NULL)
      return false;

   bool ret = newZipStream->copyFrom(readS);

   // [tom, 1/24/2007] closeFile() just frees the relevant filters and
   // thus it is safe to call from here.
   closeFile(readS);

   return ret;
}

//////////////////////////////////////////////////////////////////////////
// Public Methods
//////////////////////////////////////////////////////////////////////////

void ZipArchive::setFilename(const char *filename)
{
   SAFE_FREE(mFilename);
   if(filename)
      mFilename = dStrdup(filename);
}

//////////////////////////////////////////////////////////////////////////

bool ZipArchive::openArchive(const char *filename, AccessMode mode /* = Read */)
{
   if(mode != Read && mode != Write && mode != ReadWrite)
      return false;

   closeArchive();

   mDiskStream = new FileStream;
   if(mDiskStream->open(filename, (FileStream::AccessMode)mode))
   {
      setFilename(filename);

      if(openArchive(mDiskStream, mode))
         return true;
   }
   
   // Cleanup just in case openArchive() failed
   closeArchive();

   return false;
}

bool ZipArchive::openArchive(Stream *stream, AccessMode mode /* = Read */)
{
   if(mode != Read && mode != Write && mode != ReadWrite)
      return false;

   mStream = stream;
   mMode = mode;

   if(mode == Read || mode == ReadWrite)
   {
      bool ret = readCentralDirectory();
      if(mode == Read)
         return ret;

      return true;
   }
   else
   {
      mEntries.clear();
      SAFE_DELETE(mRoot);
      mRoot = new ZipEntry;
      mRoot->mName = StringTable->EmptyString;
      mRoot->mIsDirectory = true;
      mRoot->mCD.setFilename("");
   }

   return true;
}

void ZipArchive::closeArchive()
{
   if(mMode == Write || mMode == ReadWrite)
      rebuildZip();

   // Free any remaining temporary files
   for(S32 i = 0;i < mTempFiles.size();++i)
   {
      SAFE_DELETE(mTempFiles[i]);
   }
   mTempFiles.clear();

   // Close the zip file stream and clean up
   if(mDiskStream)
   {
      mDiskStream->close();

      delete mDiskStream;
      mDiskStream = NULL;
   }

   mStream = NULL;

   SAFE_FREE(mFilename);
   SAFE_DELETE(mRoot);
   mEntries.clear();
}

//////////////////////////////////////////////////////////////////////////

Stream * ZipArchive::openFile(const char *filename, AccessMode mode /* = Read */)
{
   ZipEntry *ze = findZipEntry(filename);

   if(mode == Read)
   {
      if(ze == NULL)
         return NULL;

      return openFileForRead(&ze->mCD);
   }

   if(mode == Write)
   {
      if(ze)
      {
         if(ze->mCD.mInternalFlags & CDFileOpen)
         {
            if(isVerbose())
               Con::errorf("ZipArchive::openFile - File %s is already open", filename);
            return NULL;
         }
         
         // Remove the old entry so we can create a new one
         removeEntry(ze);
         ze = NULL;
      }

      return createNewFile(filename, Compressor::findCompressor(Deflated));
   }

   if(isVerbose())
      Con::errorf("ZipArchive::openFile - Files within zips can only be opened as read or write, but not both at the same time.");

   return NULL;
}

void ZipArchive::closeFile(Stream *stream)
{
   FilterStream *currentStream, *nextStream;

   nextStream = dynamic_cast<FilterStream*>(stream);
   while (nextStream)
   {
      currentStream = nextStream;
      stream = currentStream->getStream();

      currentStream->detachStream();

      nextStream = dynamic_cast<FilterStream*>(stream);

      delete currentStream;
   }

   ZipTempStream *tempStream = dynamic_cast<ZipTempStream *>(stream);
   if(tempStream && (tempStream->getCentralDir()->mInternalFlags & CDFileOpen))
   {
      // [tom, 1/23/2007] This is a temporary file we are writing to
      // so we need to update the relevant information in the header.
      updateFile(tempStream);
   }
}

//////////////////////////////////////////////////////////////////////////

Stream *ZipArchive::openFileForRead(const CentralDir *fileCD)
{
   if(mMode != Read && mMode != ReadWrite)
      return NULL;

   if((fileCD->mInternalFlags & (CDFileDeleted | CDFileOpen)) != 0)
      return NULL;

   Stream *stream = mStream;

   if(fileCD->mInternalFlags & CDFileDirty)
   {
      // File is dirty, we need to read from the temporary file
      for(S32 i = 0;i < mTempFiles.size();++i)
      {
         if(mTempFiles[i]->getCentralDir() == fileCD)
         {
            // Found the temporary file
            if(! mTempFiles[i]->rewind())
            {
               if(isVerbose())
                  Con::errorf("ZipArchive::openFile - %s: %s is dirty, but could not rewind temporary file?", mFilename ? mFilename : "<no filename>", fileCD->mFilename);
               return NULL;
            }

            stream = mTempFiles[i];
            break;
         }
      }

      if(stream == mStream)
      {
         if(isVerbose())
            Con::errorf("ZipArchive::openFile - %s: %s is dirty, but no temporary file found?", mFilename ? mFilename : "<no filename>", fileCD->mFilename);
         return NULL;
      }
   }
   else
   {
      // Read from the zip file directly
      if(! mStream->setPosition(fileCD->mLocalHeadOffset))
      {
         if(isVerbose())
            Con::errorf("ZipArchive::openFile - %s: Could not locate local header for file %s", mFilename ? mFilename : "<no filename>", fileCD->mFilename);
         return NULL;
      }

      FileHeader fh;
      if(! fh.read(mStream))
      {
         if(isVerbose())
            Con::errorf("ZipArchive::openFile - %s: Could not read local header for file %s", mFilename ? mFilename : "<no filename>", fileCD->mFilename);
         return NULL;
      }
   }

   Stream *attachTo = stream;
   U16 compMethod = fileCD->mCompressMethod;

   if(fileCD->mFlags & Encrypted)
   {
      if(fileCD->mCompressMethod == AESEncrypted)
      {
         // [tom, 1/19/2007] Whilst AES support does exist, I'm not including it in TGB
         // to avoid having to deal with crypto export legal issues.
         Con::errorf("ZipArchive::openFile - %s: File %s is AES encrypted, but AES is not supported in this version.", mFilename ? mFilename : "<no filename>", fileCD->mFilename);
      }
      else
      {
         ZipCryptRStream *cryptStream = new ZipCryptRStream;
         cryptStream->setPassword(DEFAULT_ZIP_PASSWORD);
         cryptStream->setFileEndPos(stream->getPosition() + fileCD->mCompressedSize);
         if(! cryptStream->attachStream(stream))
         {
            delete cryptStream;
            return NULL;
         }

         attachTo = cryptStream;
      }
   }

   Compressor *comp = Compressor::findCompressor(compMethod);
   if(comp == NULL)
   {
      if(isVerbose())
         Con::errorf("ZipArchive::openFile - %s: Unsupported compression method (%d) for file %s", mFilename ? mFilename : "<no filename>", fileCD->mCompressMethod, fileCD->mFilename);
      return NULL;
   }

   return comp->createReadStream(fileCD, attachTo);
}

//////////////////////////////////////////////////////////////////////////

bool ZipArchive::addFile(const char *filename, const char *pathInZip, bool replace /* = true */)
{
   Stream *source = ResourceManager->openStream(filename);
   if(source == NULL)
      return false;

   const CentralDir *cd = findFileInfo(pathInZip);
   if(! replace && cd && (cd->mInternalFlags & CDFileDeleted) == 0)
      return false;

   Stream *dest = openFile(pathInZip, Write);
   if(dest == NULL)
   {
      ResourceManager->closeStream(source);
      return false;
   }

   bool ret = dest->copyFrom(source);

   closeFile(dest);
   ResourceManager->closeStream(source);

   return ret;
}

bool ZipArchive::extractFile(const char *pathInZip, const char *filename, bool *crcFail /* = NULL */)
{
   if(crcFail)
      *crcFail = false;

   const CentralDir *realCD = findFileInfo(pathInZip);
   if(realCD == NULL)
      return false;
   
   FileStream dest;
   if(! ResourceManager->openFileForWrite(dest, filename))
      return false;

   Stream *source = openFile(pathInZip, Read);
   if(source == NULL)
   {
      dest.close();
      return false;
   }

   // [tom, 2/7/2007] CRC checking the lazy man's way
   // ZipStatFilter only fails if it doesn't have a central directory, so this is safe
   CentralDir fakeCD;
   ZipStatFilter zsf(&fakeCD);
   zsf.attachStream(source);

   bool ret = dest.copyFrom(&zsf);

   zsf.detachStream();

   if(ret && fakeCD.mCRC32 != realCD->mCRC32)
   {
      if(crcFail)
         *crcFail = true;

      if(isVerbose())
         Con::errorf("ZipArchive::extractFile - CRC failure extracting file %s", pathInZip);
      ret = false;
   }
   
   closeFile(source);
   dest.close();

   return ret;
}

bool ZipArchive::deleteFile(const char *filename)
{
   if(mMode != Write && mMode != ReadWrite)
      return false;

   CentralDir *cd = findFileInfo(filename);
   if(cd == NULL)
      return false;

   cd->mInternalFlags |= CDFileDeleted;

   // CodeReview [tom, 2/9/2007] If this is a file we have a temporary file for,
   // we should probably delete it here rather then waiting til the archive is closed.

   return true;
}

//////////////////////////////////////////////////////////////////////////

bool ZipArchive::isVerbose()
{
   return Con::getBoolVariable("$Pref::Zip::Verbose");
}

void ZipArchive::setVerbose(bool verbose)
{
   Con::setBoolVariable("$Pref::Zip::Verbose", verbose);
}

} // end namespace Zip
