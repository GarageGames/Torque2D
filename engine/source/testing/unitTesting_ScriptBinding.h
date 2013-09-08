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

#ifndef TORQUE_SHIPPING

/*! @defgroup UnitTesting Unit Testing
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Runs all the registered unit tests.
*/
ConsoleFunctionWithDocs( runAllUnitTests, S32, 1, 1, () )
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

/*! @} */ // end group UnitTesting

#endif // TORQUE_SHIPPING
