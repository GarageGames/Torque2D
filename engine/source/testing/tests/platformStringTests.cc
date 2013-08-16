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

#define PLATFORM_UNITTEST_STRING_DESTINATION_BUFFERSIZE     1024

//-----------------------------------------------------------------------------
TEST( PlatformStringTests, dStrlenTest )
{
    // Check length of string.
    ASSERT_EQ( (S32)dStrlen("GarageGames"), 11 ) << "String length failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrcatTest )
{
    char source[] = { "Games" };
    char destination[] = { "Garage\0XXXXX" };

    // Concatenate.
    char* pResult = dStrcat( destination, source );

    // Check result.
    ASSERT_EQ( pResult, destination );
    ASSERT_STREQ( "GarageGames", pResult ) << "Concatenation failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrncatTest )
{
    char source[] = { "Games" };
    char destination[] = { "Garage\0XXXXX" };

    // Concatenate.
    char* pResult = dStrncat( destination, source, 3 );

    // Check result.
    ASSERT_EQ( pResult, destination );
    ASSERT_STREQ( "GarageGam", pResult ) << "Explicit-length concatenation failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrcatlTest )
{
    char destination[] = { "Garage\0XXXXX" };

    // Concatenate.
    char* pResult = dStrcatl( destination, sizeof(destination), "G", "am", "e", "s", NULL );

    // Check result.
    ASSERT_STREQ( "GarageGames", destination ) << "Variadic concatenation failed.";
    ASSERT_STREQ( "GarageGames", pResult ) << "Variadic concatenation failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrcmp8Test )
{
    UTF8 source1[] = { "garageGames" };
    UTF8 source2[] = { "GarageGames" };
    UTF8 source3[] = { "GarageGame" };
    UTF8 destination[] = { "GarageGames" };

    // Compare.
    ASSERT_GT( dStrcmp( source1, destination ), 0 ) << "Case-sensitive UTF8 compare failed.";
    ASSERT_EQ( dStrcmp( source2, destination ), 0 ) << "Case-sensitive UTF8 compare failed.";
    ASSERT_LT( dStrcmp( source3, destination ), 0 ) << "Case-sensitive UTF8 compare failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrcmp16Test )
{
    UTF16 source1[] = { 1001, 2000, 3000, 4000, 0 };
    UTF16 source2[] = { 1000, 2000, 3000, 4000, 0 };
    UTF16 source3[] = { 1000, 2000, 3000, 0 };

    UTF16 destination[] = { 1000, 2000, 3000, 4000, 0 };

    // Compare.
    ASSERT_LT( 0, dStrcmp( source1, destination ) ) << "Case-sensitive UTF16 compare failed.";
    ASSERT_EQ( 0, dStrcmp( source2, destination ) ) << "Case-sensitive UTF16 compare failed.";
    ASSERT_GT( 0, dStrcmp( source3, destination ) ) << "Case-sensitive UTF16 compare failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStricmpTest )
{
    char source1[] = { "GaRaGeGaMeS!" };
    char source2[] = { "GaRaGeGaMeS" };
    char source3[] = { "GaRaGeGaMe" };
    char destination[] = { "GarageGames" };

    // Compare.
    ASSERT_GT( dStricmp( source1, destination ), 0 ) << "Case-insensitive compare failed.";
    ASSERT_EQ( dStricmp( source2, destination ), 0 ) << "Case-insensitive compare failed.";
    ASSERT_LT( dStricmp( source3, destination ), 0 ) << "Case-insensitive compare failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrncmpTest )
{
    char source1[] = { "GG*" };
    char source2[] = { "GG!" };
    char source3[] = { "GG " };
    char destination[] = { "GG!" };

    // Compare.
    ASSERT_GT( dStrncmp( source1, destination, sizeof(destination) ), 0 ) << "Case-sensitive compare failed.";
    ASSERT_EQ( dStrncmp( source2, destination, sizeof(destination) ), 0 ) << "Case-sensitive compare failed.";
    ASSERT_LT( dStrncmp( source3, destination, sizeof(destination) ), 0 ) << "Case-sensitive compare failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrnicmpTest )
{
    char source1[] = { "gG*" };
    char source2[] = { "gg!" };
    char source3[] = { "Gg " };
    char destination[] = { "GG!" };

    // Compare.
    ASSERT_GT( dStrnicmp( source1, destination, sizeof(destination) ), 0 ) << "Case-insensitive compare failed.";
    ASSERT_EQ( dStrnicmp( source2, destination, sizeof(destination) ), 0 ) << "Case-insensitive compare failed.";
    ASSERT_LT( dStrnicmp( source3, destination, sizeof(destination) ), 0 ) << "Case-insensitive compare failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrcpyTest )
{
    char source[] = { "GarageGames" };
    char destination[sizeof(source)+1];

    // Copy.
    char* pResult = dStrcpy( destination, source );

    // Check results.
    ASSERT_EQ( pResult, destination );
    ASSERT_STREQ( "GarageGames", destination ) << "Copy failed.";
    ASSERT_STREQ( pResult, destination ) << "Copy failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrcpylTest )
{
    char destination[] = { "GarageGames" };

    // Copy.
    char* pResult = dStrcpyl( destination, sizeof(destination), "G", "ar", "a", "ge", "Ga", "me", "s", NULL );

    // Check result.
    ASSERT_EQ( pResult, destination );
    ASSERT_STREQ( "GarageGames", destination ) << "Variadic copy failed.";
    ASSERT_STREQ( "GarageGames", pResult ) << "Variadic copy failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStruprTest )
{
    char source[] = { "GarageGames" };

    // Convert.
    char* pResult = dStrupr( source );

    // Check results.
    ASSERT_STREQ( "GARAGEGAMES", pResult ) << "Uppercase conversion failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrlwrTest )
{
    char source[] = { "GarageGames" };

    // Convert.
    char* pResult = dStrlwr( source );

    // Check results.
    ASSERT_STREQ( "garagegames", pResult ) << "Lowercase conversion failed.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dToupperTest )
{
    // Convert.
    S32 index = 0;
    for ( char character = 'a'; character <= 'z'; character++, index++ )
    {
        // Check results.
        ASSERT_EQ( (char)('A' + index), dToupper( character) ) << "Uppercase conversion incorrect.";
    }
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dTolowerTest )
{
    // Convert.
    S32 index = 0;
    for ( char character = 'A'; character <= 'Z'; character++, index++ )
    {
        // Check results.
        ASSERT_EQ( (char)('a' + index), dTolower( character) ) << "Lowercase conversion incorrect.";
    }
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrchrTest )
{
    char source[] = { "GarageGames" };

    // Find.
    char* pResult1 = dStrchr( source, 'm' );
    char* pResult2 = dStrchr( source, 'z' );

    // Check results.
    ASSERT_STREQ( "mes", pResult1 ) << "Could not find character.";
    ASSERT_EQ( 0, pResult2 ) << "Should not have found character,";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrchrCONSTTest )
{
    char source[] = { "GarageGames" };

    // Find.
    const char* pResult1 = dStrchr( (const char*)source, 'm' );
    const char* pResult2 = dStrchr( (const char*)source, 'z' );

    // Check results.
    ASSERT_STREQ( "mes", pResult1 ) << "Could not find character.";
    ASSERT_EQ( 0, pResult2 ) << "Should not have found character,";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrrchrTest )
{
    char source[] = { "GarageGames" };

    // Find.
    char* pResult1 = dStrrchr( source, 'G' );
    char* pResult2 = dStrrchr( source, 'z' );

    // Check results.
    ASSERT_STREQ( "Games", pResult1 ) << "Could not find character.";
    ASSERT_EQ( 0, pResult2 ) << "Should not have found character,";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrrchrCONSTTest )
{
    char source[] = { "GarageGames" };

    // Find.
    const char* pResult1 = dStrrchr( (const char*)source, 'G' );
    const char* pResult2 = dStrrchr( (const char*)source, 'z' );

    // Check results.
    ASSERT_STREQ( "Games", pResult1 ) << "Could not find character.";
    ASSERT_EQ( 0, pResult2 ) << "Should not have found character,";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrspnTest )
{
    char source[] = { "Garage_Games!" };

    // Find.
    const S32 result = (S32)dStrspn( source, "Gargeams" );

    // Check results.
    ASSERT_EQ( 6, result ) << "Invalid index for string find.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrcspnTest )
{
    char source[] = { "Garage_Games!" };

    // Find.
    const S32 result = (S32)dStrcspn( source, "_!" );

    // Check results.
    ASSERT_EQ( 6, result ) << "Invalid index for string find.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrstrTest )
{
    char source1[] = { "GarageGames" };
    char source2[] = { "Games" };

    // Find.
    char* pResult = dStrstr( source1, source2 );

    // Check results.
    ASSERT_STREQ( "Games", pResult ) << "Could not find string.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrstrCONSTTest )
{
    char source1[] = { "GarageGames" };
    char source2[] = { "Games" };

    // Find.
    const char* pResult = dStrstr( (const char*)source1, (const char*)source2 );

    // Check results.
    ASSERT_STREQ( "Games", pResult ) << "Could not find string.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrtokTest )
{
    char source[] = { "Mich,Melv\nKyle\tRichard\nRon" };
    char separators[] = { ",\n\t\n" };

    // Find.
    char* pResult1 = dStrtok( source, separators );
    char* pResult2 = dStrtok( NULL, separators );
    char* pResult3 = dStrtok( NULL, separators );
    char* pResult4 = dStrtok( NULL, separators );
    char* pResult5 = dStrtok( NULL, separators );
    char* pResult6 = dStrtok( NULL, separators );

    // Check results.
    ASSERT_STREQ( "Mich", pResult1 ) << "Token find is incorrect.";
    ASSERT_STREQ( "Melv", pResult2 ) << "Token find is incorrect.";
    ASSERT_STREQ( "Kyle", pResult3 ) << "Token find is incorrect.";
    ASSERT_STREQ( "Richard", pResult4 ) << "Token find is incorrect.";
    ASSERT_STREQ( "Ron", pResult5 ) << "Token find is incorrect.";
    ASSERT_EQ( 0, pResult6 ) << "Token find is incorrect.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dStrrevTest )
{
    char destination[] = { "GarageGames" };

    // Find.
    const S32 result = dStrrev( destination );

    // Check results.
    ASSERT_STREQ( "semaGegaraG", destination ) << "String reverse is incorrect.";
    ASSERT_EQ( 5, result ) << "String reverse is incorrect.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dAtoiTest )
{
    char source1[] = { "16384" };
    char source2[] = { "-16384" };
    char source3[] = { "256.5" };
    char source4[] = { "-256.5" };

    // Find.
    const S32 result1 = dAtoi( source1 );
    const S32 result2 = dAtoi( source2 );
    const S32 result3 = dAtoi( source3 );
    const S32 result4 = dAtoi( source4 );

    // Check results.
    ASSERT_EQ( 16384, result1 ) << "Conversion incorrect.";
    ASSERT_EQ( -16384, result2 ) << "Conversion incorrect.";
    ASSERT_EQ( 256, result3 ) << "Conversion incorrect.";
    ASSERT_EQ( -256, result4 ) << "Conversion incorrect.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dAtofTest )
{
    char source1[] = { "123.45" };
    char source2[] = { "-123.45" };
    char source3[] = { "256.0" };
    char source4[] = { "-256.0" };

    // Find.
    const F32 result1 = dAtof( source1 );
    const F32 result2 = dAtof( source2 );
    const F32 result3 = dAtof( source3 );
    const F32 result4 = dAtof( source4 );

    // Check results.
    ASSERT_EQ( 123.45f, result1 ) << "Conversion incorrect.";
    ASSERT_EQ( -123.45f, result2 ) << "Conversion incorrect.";
    ASSERT_EQ( 256.0f, result3 ) << "Conversion incorrect.";
    ASSERT_EQ( -256.0f, result4 ) << "Conversion incorrect.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dAtobTest )
{
    char source1[] = { "1" };
    char source2[] = { "0" };
    char source3[] = { "100" };
    char source4[] = { "-100" };
    char source5[] = { "true" };
    char source6[] = { "false" };
    char source7[] = { "I refute it thus!" };

    // Find.
    const bool result1 = dAtob( source1 );
    const bool result2 = dAtob( source2 );
    const bool result3 = dAtob( source3 );
    const bool result4 = dAtob( source4 );
    const bool result5 = dAtob( source5 );
    const bool result6 = dAtob( source6 );
    const bool result7 = dAtob( source7 );

    // Check results.
    ASSERT_EQ( true, result1 ) << "Conversion incorrect.";
    ASSERT_EQ( false, result2 ) << "Conversion incorrect.";
    ASSERT_EQ( true, result3 ) << "Conversion incorrect.";
    ASSERT_EQ( true, result4 ) << "Conversion incorrect.";
    ASSERT_EQ( true, result5 ) << "Conversion incorrect.";
    ASSERT_EQ( false, result6 ) << "Conversion incorrect.";
    ASSERT_EQ( false, result7 ) << "Conversion incorrect.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dItoaTest )
{
    char destination1[64];
    char destination2[64];
    char destination3[64];
    char destination4[64];

    // Find.
    const S32 result1 = dItoa( 16384, destination1 );
    const S32 result2 = dItoa( -16384, destination2 );
    const S32 result3 = dItoa( 256, destination3 );
    const S32 result4 = dItoa( -256, destination4 );

    // Check results.
    ASSERT_STREQ( "16384", destination1 ) << "Conversion incorrect.";
    ASSERT_STREQ( "-16384", destination2 ) << "Conversion incorrect.";
    ASSERT_STREQ( "256", destination3 ) << "Conversion incorrect.";
    ASSERT_STREQ( "-256", destination4 ) << "Conversion incorrect.";
    ASSERT_EQ( 5, result1 ) << "Conversion incorrect.";
    ASSERT_EQ( 5+1, result2 ) << "Conversion incorrect.";
    ASSERT_EQ( 3, result3 ) << "Conversion incorrect.";
    ASSERT_EQ( 3+1, result4 ) << "Conversion incorrect.";
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dIsalnumTest )
{
    char source1[] = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    char source2[] = { "!$%^&*()+-={}[]~#@':;?/>.<,|`" };

    // Check results on source#1.
    const S32 sourceLength1 = dStrlen( source1 );
    for ( S32 index = 0; index < sourceLength1; ++index )
    {
        ASSERT_EQ( true, dIsalnum(source1[index]) );
    }

    // Check results on source#2.
    const S32 sourceLength2 = dStrlen( source2 );
    for ( S32 index = 0; index < sourceLength2; ++index )
    {
        ASSERT_EQ( false, dIsalnum(source2[index]) );
    }
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dIsalphaTest )
{
    char source1[] = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" };    
    char source2[] = { "!$%^&*()+-={}[]~#@':;?/>.<,|`" };
    char source3[] = { "0123456789" };

    // Check results on source#1.
    const S32 sourceLength1 = dStrlen( source1 );
    for ( S32 index = 0; index < sourceLength1; ++index )
    {
        ASSERT_EQ( true, dIsalpha(source1[index]) );
    }

    // Check results on source#2.
    const S32 sourceLength2 = dStrlen( source2 );
    for ( S32 index = 0; index < sourceLength2; ++index )
    {
        ASSERT_EQ( false, dIsalpha(source2[index]) );
    }

    // Check results on source#3.
    const S32 sourceLength3 = dStrlen( source3 );
    for ( S32 index = 0; index < sourceLength3; ++index )
    {
        ASSERT_EQ( false, dIsalpha(source3[index]) );
    }
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dIsdigitTest )
{
    char source1[] = { "0123456789" };
    char source2[] = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    char source3[] = { "!$%^&*()+-={}[]~#@':;?/>.<,|`" };

    // Check results on source#1.
    const S32 sourceLength1 = dStrlen( source1 );
    for ( S32 index = 0; index < sourceLength1; ++index )
    {
        ASSERT_EQ( true, dIsdigit(source1[index]) );
    }

    // Check results on source#2.
    const S32 sourceLength2 = dStrlen( source2 );
    for ( S32 index = 0; index < sourceLength2; ++index )
    {
        ASSERT_EQ( false, dIsdigit(source2[index]) );
    }

    // Check results on source#3.
    const S32 sourceLength3 = dStrlen( source3 );
    for ( S32 index = 0; index < sourceLength3; ++index )
    {
        ASSERT_EQ( false, dIsdigit(source3[index]) );
    }
}

//-----------------------------------------------------------------------------

TEST( PlatformStringTests, dIsspaceTest )
{
    char source1[] = { "\t\r " };
    char source2[] = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    char source3[] = { "!$%^&*()+-={}[]~#@':;?/>.<,|`" };

    // Check results on source#1.
    const S32 sourceLength1 = dStrlen( source1 );
    for ( S32 index = 0; index < sourceLength1; ++index )
    {
        ASSERT_EQ( true, dIsspace(source1[index]) );
    }

    // Check results on source#2.
    const S32 sourceLength2 = dStrlen( source2 );
    for ( S32 index = 0; index < sourceLength2; ++index )
    {
        ASSERT_EQ( false, dIsspace(source2[index]) );
    }

    // Check results on source#3.
    const S32 sourceLength3 = dStrlen( source3 );
    for ( S32 index = 0; index < sourceLength3; ++index )
    {
        ASSERT_EQ( false, dIsspace(source3[index]) );
    }
}
#endif // TORQUE_SHIPPING
