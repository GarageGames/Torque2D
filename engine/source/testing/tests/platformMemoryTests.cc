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

//-----------------------------------------------------------------------------

#define PLATFORM_UNITTEST_MEMORY_BUFFERSIZE     16384

//-----------------------------------------------------------------------------
TEST( PlatformMemoryTests, dMallocrAnddFreeTest )
{
    // Allocate some memory.
    void* pResult = dMalloc_r( PLATFORM_UNITTEST_MEMORY_BUFFERSIZE, __FILE__, __LINE__ );

    // Check.
    ASSERT_NE( (void*)0, pResult ) << "Memory not allocated.";

    // Set memory (ensure no exceptions).
    for( U32 index = 0; index < PLATFORM_UNITTEST_MEMORY_BUFFERSIZE; ++index )
    {
        *((U8*)pResult) = index % 255;
    }

    // Free memory (ensure no exceptions).
    dFree( pResult );
}

//-----------------------------------------------------------------------------

TEST( PlatformMemoryTests, dReallocrAnddFreeTest )
{
    // Allocate some memory.
    void* pResult = dMalloc_r( PLATFORM_UNITTEST_MEMORY_BUFFERSIZE, __FILE__, __LINE__ );

    // Check.
    ASSERT_NE( (void*)0, pResult ) << "Memory not allocated.";

    // Reallocate some memory.
    pResult = dRealloc_r( pResult, PLATFORM_UNITTEST_MEMORY_BUFFERSIZE * 2, __FILE__, __LINE__ );

    // Check.
    ASSERT_NE( (void*)0, pResult ) << "Memory not allocated.";

    // Set memory (ensure no exceptions).
    for( U32 index = 0; index < (PLATFORM_UNITTEST_MEMORY_BUFFERSIZE*2); ++index )
    {
        *((U8*)pResult) = index % 255;
    }

    // Free memory (ensure no exceptions).
    dFree( pResult );
}

//-----------------------------------------------------------------------------

TEST( PlatformMemoryTests, dMemcpyTest )
{
    U8 source[] = { 0,1,2,3,4,5,6,7,8,9 };
    U8 destination[sizeof(source)];

    // Copy.
    void* result = dMemcpy( destination, source, sizeof(source) );

    // Check.
    ASSERT_EQ( result, destination ) << "Destination buffer incorrect.";

    // Check,
    for( U32 index = 0; index < sizeof(source); ++index )
    {
        ASSERT_EQ( index, source[index] ) << "Source memory value is incorrect.";
        ASSERT_EQ( index, destination[index] ) << "Destination memory value is incorrect.";
    }
}

//-----------------------------------------------------------------------------

TEST( PlatformMemoryTests, dMemmoveTest )
{
    U8 destination[] = { 0,1,2,3,4,5,6,7,8,9 };

    // Move.
    void* result = dMemmove( destination, destination+1, sizeof(destination)-1 );

    // Check.
    ASSERT_EQ( result, destination ) << "Destination buffer incorrect.";

    // Check,
    for( U32 index = 0; index < (sizeof(destination)-1); ++index )
    {
        ASSERT_EQ( index+1, destination[index] ) << "Destination memory value is incorrect.";
    }
}

//-----------------------------------------------------------------------------

TEST( PlatformMemoryTests, dMemsetTest )
{
    U8 destination[] = { 0,1,2,3,4,5,6,7,8,9 };

    const U8 magic = 128;

    // Set.
    void* result = dMemset( destination, magic, sizeof(destination)-1 );

    // Check.
    ASSERT_EQ( result, destination ) << "Destination buffer incorrect.";

    // Check,
    for( U32 index = 0; index < (sizeof(destination)-1); ++index )
    {
        ASSERT_EQ( magic, destination[index] ) << "Destination memory value is incorrect.";
    }

    // Check.
    ASSERT_EQ( 9, destination[sizeof(destination)-1] ) << "Destination memory value is incorrect.";
}

//-----------------------------------------------------------------------------

TEST( PlatformMemoryTests, dMemcmpTest )
{
    U8 source1[] = { 0,1,2,3,4,5,6,7,8,10 };
    U8 source2[] = { 0,1,2,3,4,5,6,7,8,9 };
    U8 source3[] = { 0,1,2,3,4,5,6,7,8,0 };

    U8 destination[] = { 0,1,2,3,4,5,6,7,8,9 };

    // Compare.
    const S32 result1 = dMemcmp( source1, destination, sizeof(destination) );
    const S32 result2 = dMemcmp( source2, destination, sizeof(destination) );
    const S32 result3 = dMemcmp( source3, destination, sizeof(destination) );

    // Check.
    ASSERT_LT( 0, result1 ) << "Memory compare is incorrect.";
    ASSERT_EQ( 0, result2 ) << "Memory compare is incorrect.";
    ASSERT_GT( 0, result3 ) << "Memory compare is incorrect.";
}

#endif // TORQUE_SHIPPING
