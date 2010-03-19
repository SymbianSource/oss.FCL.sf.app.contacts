/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Unit test class for testing CPcsKeyMap
*
*/

// INTERNAL INCLUDES
#include "t_cpcskeymap.h"
#include "cpcskeymap.h"

// SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>


// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::NewL
// -----------------------------------------------------------------------------
//
UT_CPcsKeyMap* UT_CPcsKeyMap::NewL()
    {
    UT_CPcsKeyMap* self = UT_CPcsKeyMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::NewLC
// -----------------------------------------------------------------------------
//
UT_CPcsKeyMap* UT_CPcsKeyMap::NewLC()
    {
    UT_CPcsKeyMap* self = new( ELeave ) UT_CPcsKeyMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::~UT_CPcsKeyMap
// -----------------------------------------------------------------------------
//
UT_CPcsKeyMap::~UT_CPcsKeyMap()
    {
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_CPcsKeyMap
// -----------------------------------------------------------------------------
//
UT_CPcsKeyMap::UT_CPcsKeyMap()
    {
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::ConstructL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::SetupL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::SetupL()
    {    
	iKeyMap = CPcsKeyMap::NewL();
    }
    
// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::Teardown
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::Teardown()
    {
    delete iKeyMap;
	iKeyMap = NULL;
    }


// TEST CASES

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_NewLL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_NewLL()
    {    
#if 0 //remove this line
    // Each numeric key has been mapped
    const TInt KAmountOfNumericKeys = 10; // keys 0-9
    EUNIT_ASSERT_EQUALS( KAmountOfNumericKeys, iKeyMap->iKeyMapping.count() );
    for (TInt i = 0; i < KAmountOfNumericKeys; ++i)
        {
        EUNIT_ASSERT( iKeyMap->iKeyMapping[i].length() > 0 );
        }
#endif
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetNumericKeyStringL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetNumericKeyStringL()
	{
	_LIT( KAlpha, "abjqmca" );
	_LIT( KNumeric, "2257622" );
	HBufC* numericBuf = iKeyMap->GetNumericKeyStringL( KAlpha, ETrue );
	CleanupStack::PushL( numericBuf );
	EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
	CleanupStack::PopAndDestroy( numericBuf );
	numericBuf = NULL;

	
	_LIT( KAlphaOneChar, "m" );
	_LIT( KNumericOneChar, "6" );
	numericBuf = iKeyMap->GetNumericKeyStringL( KAlphaOneChar, ETrue );
	CleanupStack::PushL( numericBuf );
	EUNIT_ASSERT_EQUALS( *numericBuf, KNumericOneChar );
	CleanupStack::PopAndDestroy( numericBuf );
	numericBuf = NULL;
	

	_LIT( KAlphaWithSpaces, "kjhgfdsa qwert " );
	_LIT( KNumericWithSpaces, "554433720793780" );	
	numericBuf = iKeyMap->GetNumericKeyStringL( KAlphaWithSpaces, ETrue );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericWithSpaces );
    CleanupStack::PopAndDestroy( numericBuf );
	}

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetNumericKeyStringSeparatorL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetNumericKeyStringSeparatorL()
    {
    _LIT( KAlpha, "abjqmca" );
    _LIT( KNumeric, " 2257622" ); // Space at beginning
    HBufC* numericBuf = iKeyMap->GetNumericKeyStringL( KAlpha, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
    CleanupStack::PopAndDestroy( numericBuf );
    numericBuf = NULL;


    _LIT( KAlphaOneChar, "m" );
    _LIT( KNumericOneChar, " 6" );
    numericBuf = iKeyMap->GetNumericKeyStringL( KAlphaOneChar, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericOneChar );
    CleanupStack::PopAndDestroy( numericBuf );
    numericBuf = NULL;
    

    _LIT( KAlphaWithSpaces, " kjh gfdsa qwert " );
    _LIT( KNumericWithSpaces, "  554 43372 79378 " ); // two spaces at beginning
    numericBuf = iKeyMap->GetNumericKeyStringL( KAlphaWithSpaces, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericWithSpaces );
    CleanupStack::PopAndDestroy( numericBuf );
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetNumericKeyStringWithNumbersL
// Source string contains numbers
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetNumericKeyStringWithNumbersL()
    {
    _LIT( KAlpha, "11" );
    _LIT( KNumeric, " 11" ); // Space at beginning
    HBufC* numericBuf = iKeyMap->GetNumericKeyStringL( KAlpha, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
    CleanupStack::PopAndDestroy(numericBuf);
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetNumericKeyStringWithSpecialCharactersL
// Source string contains numbers
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetNumericKeyStringWithSpecialCharactersL()
    {
    _LIT( KAlpha, " +g-[5]t" );
#if 1 // if hardcoded keymap is used
    _LIT( KNumeric, "  +4-[5]8" ); // Extra space at beginning
#else
    // if real Orbit keymap code is used
    _LIT( KNumeric, "  141[5]8" ); // Extra space at beginning
#endif
    HBufC* numericBuf = iKeyMap->GetNumericKeyStringL( KAlpha, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
    CleanupStack::PopAndDestroy(numericBuf);
    }


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CPcsKeyMap,
    "UT_CPcsKeyMap",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "UT_CPcsKeyMap",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_NewLL, Teardown )

EUNIT_TEST(
    "GetNumericKeyString - test ",
    "UT_CPcsKeyMap",
    "GetNumericKeyString",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericKeyStringL, Teardown )

EUNIT_TEST(
    "GetNumericKeyString - test separator",
    "UT_CPcsKeyMap",
    "GetNumericKeyString",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericKeyStringSeparatorL, Teardown )

EUNIT_TEST(
    "GetNumericKeyString - test numbers",
    "UT_CPcsKeyMap",
    "GetNumericKeyString",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericKeyStringWithNumbersL, Teardown )

EUNIT_TEST(
    "GetNumericKeyString - test special characters",
    "UT_CPcsKeyMap",
    "GetNumericKeyString",
    "FUNCTIONALITY",
    SetupL, UT_GetNumericKeyStringWithSpecialCharactersL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
