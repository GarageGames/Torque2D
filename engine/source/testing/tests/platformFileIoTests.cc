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

// We don't want tests in a shipping version.
#ifndef TORQUE_SHIPPING

#ifndef _UNIT_TESTING_H_
#include "testing/unitTesting.h"
#endif

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _PLATFORM_FILEIO_H_
#include "platform/platformFileIO.h"
#endif

//-----------------------------------------------------------------------------

#define PLATFORM_UNITTEST_FILEIO_FILE           "_unitTestFile_RemoveMe.txt"
#define PLATFORM_UNITTEST_FILEIO_FILEMESSAGE    "Write a line of text."

//-----------------------------------------------------------------------------
TEST( PlatformFileIOTests, FileWriteRead )
{
    File testWriteFile;
    File::Status fileStatus;

    // Open the file for writing.
    fileStatus = testWriteFile.open( PLATFORM_UNITTEST_FILEIO_FILE, File::Write );

    // Check the file was opened.
    ASSERT_EQ( fileStatus, File::Ok ) << "Failed to open file for (over)write.";

    // Check we can write to the file.
    ASSERT_TRUE( testWriteFile.hasCapability( File::FileWrite ) ) << "File cannot be written to.";

    // Check test message length (we may not have executed any string tests yet).
    const S32 fileMessageLength = (S32)dStrlen(PLATFORM_UNITTEST_FILEIO_FILEMESSAGE);
    ASSERT_GT( fileMessageLength, 0 ) << "Test message must be a non-zero length.";

    // Write to the file.
    U32 bytesWritten;
    fileStatus = testWriteFile.write( fileMessageLength, PLATFORM_UNITTEST_FILEIO_FILEMESSAGE, &bytesWritten );

    // Check write operation.
    ASSERT_EQ( fileStatus, File::Ok ) << "Test message write operation failed.";
    ASSERT_EQ( bytesWritten, fileMessageLength ) << "Test message was not written fully.";

    // Check position and size.
    ASSERT_EQ( testWriteFile.getPosition(), bytesWritten ) << "File position is invalid.";
    ASSERT_EQ( testWriteFile.getSize(), bytesWritten ) << "File size is incorrect.";    

    // Close the file.
    fileStatus = testWriteFile.close();

    // Check status.
    ASSERT_EQ( fileStatus, File::Closed ) << "Write file was not closed.";

    /// Test file reading...
    File testReadFile;

    // Open the file for reading.
    fileStatus = testReadFile.open( PLATFORM_UNITTEST_FILEIO_FILE, File::Read );

    // Check the file was opened.
    ASSERT_EQ( fileStatus, File::Ok ) << "Failed to open file for read.";

    // Check we can read from the file.
    ASSERT_TRUE( testReadFile.hasCapability( File::FileRead ) ) << "File cannot be read from.";

    // Create a read buffer.
    char* pReadBuffer = new char[fileMessageLength+1];
    pReadBuffer[fileMessageLength] = 0;
    U32 bytesRead;

    // Read from the file.
    fileStatus = testReadFile.read( fileMessageLength, pReadBuffer, &bytesRead );

    // Check read operation.
    ASSERT_EQ( fileStatus, File::Ok ) << "Test message read operation failed.";
    ASSERT_EQ( bytesRead, fileMessageLength ) << "Test message was not read fully.";

    // Check position and size.
    ASSERT_EQ( testReadFile.getPosition(), bytesRead ) << "File position is invalid.";
    ASSERT_EQ( testReadFile.getSize(), bytesRead ) << "File size is incorrect."; 

    // Check contents.
    ASSERT_STREQ( PLATFORM_UNITTEST_FILEIO_FILEMESSAGE, pReadBuffer ) << "Test message read incorrectly.";

    // Destroy the read buffer.
    delete [] pReadBuffer;

    // Close the file.
    fileStatus = testReadFile.close();

    // Check status.
    ASSERT_EQ( fileStatus, File::Closed ) << "Read file was not closed.";

    // Check the file has been deleted.
    ASSERT_TRUE( Platform::fileDelete( PLATFORM_UNITTEST_FILEIO_FILE ) );
}
//-----------------------------------------------------------------------------

#endif // TORQUE_SHIPPING
