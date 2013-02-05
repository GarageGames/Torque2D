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

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _UNIT_TESTING_H_
#include "testing/unitTesting.h"
#endif

#include "src/gtest.cc"
#include "src/gtest-death-test.cc"
#include "src/gtest-filepath.cc"
#include "src/gtest-port.cc"
#include "src/gtest-printers.cc"
#include "src/gtest-test-part.cc"
#include "src/gtest-typed-test.cc"

//-----------------------------------------------------------------------------

class TorqueUnitTestListener : public ::testing::EmptyTestEventListener
{
    // Called before a test starts.
    virtual void OnTestStart( const ::testing::TestInfo& testInfo )
    {
        Con::printf("> Starting Test '%s.%s'",
                testInfo.test_case_name(), testInfo.name());
    }

    // Called after a failed assertion or a SUCCEED() invocation.
    virtual void OnTestPartResult( const ::testing::TestPartResult& testPartResult )
    {
        if ( testPartResult.failed() )
        {
            Con::warnf(">> Failed with '%s' in '%s' at (line:%d)",
                    testPartResult.summary(),
                    testPartResult.file_name(),
                    testPartResult.line_number()
                    );
        }
        else
        {
            Con::printf(">> Passed with '%s' in '%s' at (line:%d)",
                    testPartResult.summary(),
                    testPartResult.file_name(),
                    testPartResult.line_number()
                    );
        }
    }

    // Called after a test ends.
    virtual void OnTestEnd( const ::testing::TestInfo& testInfo )
    {
        Con::printf("> Ending Test '%s.%s'",
                testInfo.test_case_name(), testInfo.name());
        Con::printBlankLine();
    }
};

//-----------------------------------------------------------------------------

ConsoleFunction( runAllUnitTests, S32, 1, 1, "() - Runs all the registered unit tests." )
{
    // Set-up some empty arguments.
    S32 testArgc = 0;
    char** testArgv = NULL;

    // Initialize Google Test.
    testing::InitGoogleTest( &testArgc, testArgv );

    // Fetch the unit test instance.
    testing::UnitTest& unitTest = *testing::UnitTest::GetInstance();

    // Fetch the unit test event listeners.
    testing::TestEventListeners& listeners = unitTest.listeners();

    // Release the default listener.
    delete listeners.Release( listeners.default_result_printer() );

    // Add the Torque unit test listener.
    listeners.Append( new TorqueUnitTestListener );

    Con::printBlankLine();
    Con::printSeparator();
    Con::printf( "Unit Tests Starting..." );
    Con::printBlankLine();

    const S32 result RUN_ALL_TESTS();

    Con::printBlankLine();
    Con::printf( "... Unit Tests Ended." );
    Con::printSeparator();
    Con::printBlankLine();

    return result;
}

#endif // TORQUE_SHIPPING