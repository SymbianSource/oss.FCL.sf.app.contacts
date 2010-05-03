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
#if defined(USE_ORBIT_KEYMAP)
#include <hbinputkeymapfactory.h>
#endif

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
    delete iKeyMap;
    iKeyMap = NULL;
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
    
    
    // When instantiating keymap was moved to here, it removed a resource leak.
    iKeyMap = CPcsKeyMap::NewL();
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::SetupL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::SetupL()
    {
#if defined(USE_ORBIT_KEYMAP)
    // Create singleton outside actual test cases so that it is not treated as
    // resource leak, since it can't be deleted.
    HbKeymapFactory::instance();
#endif
    }
    
// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::Teardown
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::Teardown()
    {
    }


// TEST CASES

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_NewLL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_NewLL()
    {
#if defined(USE_ORBIT_KEYMAP)
    const TInt KAmountOfKeys = 10; // Must have same value as in cpcskeymap.cpp

    // Each numeric key has been mapped
    EUNIT_ASSERT_EQUALS( KAmountOfKeys, iKeyMap->iKeyMapping.count() );
    for (TInt i = 0; i < KAmountOfKeys; ++i)
        {
        EUNIT_ASSERT( iKeyMap->iKeyMapping.at(i).length() > 0 );
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
    
    
    _LIT( KAlphaLongString, "adg   1230 0zbzb0 0 e56e101at 00  " );
    _LIT( KNumericLongString, "2340001230009292000035631012800000" );
    numericBuf = iKeyMap->GetNumericKeyStringL( KAlphaLongString, ETrue );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericLongString );
    CleanupStack::PopAndDestroy( numericBuf );
	}

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetNumericKeyStringSeparatorL
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetNumericKeyStringSeparatorL()
    {
    _LIT( KAlpha, "abjqmca" );
    _LIT( KNumeric, "2257622" );
    HBufC* numericBuf = iKeyMap->GetNumericKeyStringL( KAlpha, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumeric );
    CleanupStack::PopAndDestroy( numericBuf );
    numericBuf = NULL;


    _LIT( KAlphaOneChar, "m" );
    _LIT( KNumericOneChar, "6" );
    numericBuf = iKeyMap->GetNumericKeyStringL( KAlphaOneChar, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericOneChar );
    CleanupStack::PopAndDestroy( numericBuf );
    numericBuf = NULL;
    

    _LIT( KAlphaWithSpaces, " kjh gfdsa qwert " );
    _LIT( KNumericWithSpaces, " 554 43372 79378 " ); // The leading space remains
    numericBuf = iKeyMap->GetNumericKeyStringL( KAlphaWithSpaces, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericWithSpaces );
    CleanupStack::PopAndDestroy( numericBuf );
    

    _LIT( KAlphaLongString, "adg   1230 0zbzb0 0 e56e101at 00  " );
    _LIT( KNumericLongString, "234   1230 092920 0 356310128 00  " );  
    numericBuf = iKeyMap->GetNumericKeyStringL( KAlphaLongString, EFalse );
    CleanupStack::PushL( numericBuf );
    EUNIT_ASSERT_EQUALS( *numericBuf, KNumericLongString );
    CleanupStack::PopAndDestroy( numericBuf );
    }

// -----------------------------------------------------------------------------
// UT_CPcsKeyMap::UT_GetNumericKeyStringWithNumbersL
// Source string contains numbers
// -----------------------------------------------------------------------------
//
void UT_CPcsKeyMap::UT_GetNumericKeyStringWithNumbersL()
    {
    _LIT( KAlpha, "11098" );
    _LIT( KNumeric, "11098" );
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
#if defined(USE_ORBIT_KEYMAP)
	// Orbit keymap code is used
    _LIT( KNumeric, " 141[5]8" );
#else
	// Hardcoded keymap is used
    _LIT( KNumeric, " +4-[5]8" );
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
