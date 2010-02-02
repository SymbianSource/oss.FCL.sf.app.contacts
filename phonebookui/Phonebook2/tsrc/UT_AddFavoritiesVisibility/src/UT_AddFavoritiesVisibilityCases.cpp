/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// [INCLUDE FILES] - do not remove
#include "UT_AddFavoritiesVisibility.h"
#include "stifunitmacros.h"
#include <CVPbkContactTestUtils.h>
#include <centralrepository.h>
#include "CPbk2AddFavoritesVisibilityImpl.h"
#include "CCallbackTimer.h"

const TUint32 KCRUidPhonebook = 0x101f8794;
const TInt KCmdId = 100;

//  TESTCASE("AlwaysON", "CUT_AddFavoritiesVisibility", SetupAlwaysON, TestAlwaysOnL, Teardown)
//  TESTCASE("AlwaysOFF", "CUT_AddFavoritiesVisibility", SetupAlwaysOFF, TestAlwaysOFFL, Teardown)
//  TESTCASE("FirstTimeUse", "CUT_AddFavoritiesVisibility", SetupFirstTimeUse, TestFirstTimeUseL, Teardown)
//  TESTCASE("TCPromotionMinus1", "CUT_AddFavoritiesVisibility", SetupTCPromotionMinus1, TestAlwaysOFFL, Teardown)
//  TESTCASE("TCPromotionMinus5000", "CUT_AddFavoritiesVisibility", SetupTCPromotionMinus5000, TestAlwaysOFFL, Teardown)
//  TESTCASE("TCPromotion3", "CUT_AddFavoritiesVisibility", SetupTCPromotion3, TestAlwaysOFFL, Teardown)
//  TESTCASE("TCPromotion9999", "CUT_AddFavoritiesVisibility", SetupTCPromotion9999, TestAlwaysOFFL, Teardown)
//  TESTCASE("TestLeaveInClientCallback", "CUT_AddFavoritiesVisibility", SetupAlwaysON, TestLeaveInClientCallbackL, Teardown)

void CUT_AddFavoritiesVisibility::TestCombinationL
    (const TInt aNumTCs, const TInt aTotalNumContacts,
    const TBool aCallbackExpected, const TBool aExpectedVisibility)                     
        {
        iCallbackReceived = EFalse;
        iCallbackExpected = aCallbackExpected;    
        iExpectedVisibility = aExpectedVisibility;
        
        iVisibilityImpl->NotifyVisibilityChange
        (aNumTCs, aTotalNumContacts); // either will get a callback immidiately 
        // or no callback as there is no visibility transition (ie.
        // from ETrue to EFalse or vice versa)
        iCallbackTimer->StartWaitingTillCallbackOrTimeOutL();
                
        CheckResult();
        }

void CUT_AddFavoritiesVisibility::CheckResult()
    {
        if((iCallbackExpected && !iCallbackReceived) ||
          (!iCallbackExpected &&  iCallbackReceived))
            {
            STIF_ASSERT(EFalse); // fail test
            }
        else if(!iCallbackExpected && !iCallbackReceived)
            {
            // callback not expected, query the result from class under test
            STIF_ASSERT(iExpectedVisibility == iVisibilityImpl->Visibility());            
            }
        else if(iCallbackExpected && iCallbackReceived)
            {
            STIF_ASSERT(iExpectedVisibility == iCallbackRes);
            }        
    
    }       
    
void CUT_AddFavoritiesVisibility::TestAlwaysOFFL()
    {
    TestSettingAsObserver();
        
    // Out of bounds
    //  Top Contacts: -10
    //  Contacts:     -10 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(-10, -10, EFalse, EFalse);
    
    //  Top Contacts: -1
    //  Contacts:     -1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNotFound, KErrNotFound, EFalse, EFalse);

    //  Top Contacts: -1
    //  Contacts:      0 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNotFound, KErrNone, EFalse,    EFalse);

    //  Top Contacts:  0
    //  Contacts:     -1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNone, KErrNotFound, EFalse,  EFalse);
 
    //  Top Contacts: 0
    //  Contacts:     0 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNone, KErrNone, EFalse, EFalse);

    // 1 Contact Case
    //  Top Contacts: 0
    //  Contacts:     1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNone, 1, EFalse, EFalse);

    // An out of boundary test
    //  Top Contacts: 1
    //  Contacts:     0 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(1, KErrNone, EFalse, EFalse);

    // 1 Top Contact 
    //  Top Contacts: 1
    //  Contacts:     1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse    
    TestCombinationL(1, 1, EFalse, EFalse);

    //1 Top + 1 Non Top
    //  Top Contacts: 1
    //  Contacts:     2 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse    
    TestCombinationL( 1, 2, EFalse, EFalse);

    //5 Top + 10 Non Top
    //  Top Contacts: 1
    //  Contacts:     2 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse    
    TestCombinationL( 5, 15, EFalse, EFalse);
            
    }

void CUT_AddFavoritiesVisibility::TestAlwaysOnL()
    {

    TestSettingAsObserver();
    
    // Out of bounds
    //  Top Contacts: -10
    //  Contacts:     -10 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(-10, -10, EFalse, EFalse);
    
    //  Top Contacts: -1
    //  Contacts:     -1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNotFound, KErrNotFound, EFalse, EFalse);

    //  Top Contacts: -1
    //  Contacts:      0 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNotFound, KErrNone, EFalse,    EFalse);

    //  Top Contacts:  0
    //  Contacts:     -1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNone, KErrNotFound, EFalse,  EFalse);
 
    //  Top Contacts: 0
    //  Contacts:     0 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNone, KErrNone, EFalse, EFalse);

    // 1 Contact Case
    //  Top Contacts: 0
    //  Contacts:     1 
    //  Callback Expected: Yes  
    //  Expected Visibility: ETrue
    TestCombinationL(KErrNone, 1, ETrue, ETrue);

    // An out of boundary test
    //  Top Contacts: 1
    //  Contacts:     0 
    //  Callback Expected: Yes  (previous visibility was ETrue)
    //  Expected Visibility: EFalse
    TestCombinationL(1, KErrNone, ETrue, EFalse);

    // 1 Top Contact 
    //  Top Contacts: 1
    //  Contacts:     1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse    
    TestCombinationL(1, 1, EFalse, EFalse);

    //1 Top + 1 Non Top
    //  Top Contacts: 1
    //  Contacts:     2 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse    
    TestCombinationL( 1, 2, EFalse, EFalse);

    //5 Top + 10 Non Top
    //  Top Contacts: 1
    //  Contacts:     2 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse    
    TestCombinationL( 5, 15, EFalse, EFalse);
 
    // Test EFalse -> ETrue transition
    //  Top Contacts: 0
    //  Contacts:     15 
    //  Callback Expected: Yes (Previous visibility was EFalse) 
    //  Expected Visibility: ETrue    
    TestCombinationL( 0, 15, ETrue, ETrue);

     // Test ETrue -> EFalse transition
    //  Top Contacts: 1
    //  Contacts:     2 
    //  Callback Expected: Yes (Previous visibility was ETrue) 
    //  Expected Visibility: EFalse    
    TestCombinationL( 1, 2, ETrue, EFalse);
           
    }

void CUT_AddFavoritiesVisibility::TestLeaveInClientCallbackL()
    {
    TestSettingAsObserver();    	
    iTestLeave = EFalse;
        
    // Test EFalse -> ETrue transition
    //  Top Contacts: 0
    //  Contacts:     15 
    //  Callback Expected: Yes (Previous visibility was EFalse) 
    //  Expected Visibility: EFalse    
    TestCombinationL( 0, 15, ETrue, ETrue);

    iTestLeave = ETrue;
    
    // now we need to get callbacks to emulate leave,
    // emulation should cause visibility off
       
    //  Test ETrue -> EFalse transition
    //  Top Contacts: 1
    //  Contacts:     2 
    //  Callback Expected: Yes (Previous visibility was ETrue) 
    //  Expected Visibility: EFalse    
    TestCombinationL( 1, 2, ETrue, EFalse);
    
    }

void CUT_AddFavoritiesVisibility::TestFirstTimeUseL()
    {
    TestSettingAsObserver();
    
    // Out of bounds
    //  Top Contacts: -10
    //  Contacts:     -10 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(-10, -10, EFalse, EFalse);
    
    //  Top Contacts: -1
    //  Contacts:     -1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNotFound, KErrNotFound, EFalse, EFalse);

    //  Top Contacts: -1
    //  Contacts:      0 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNotFound, KErrNone, EFalse,    EFalse);

    //  Top Contacts:  0
    //  Contacts:     -1 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNone, KErrNotFound, EFalse,  EFalse);
 
    //  Top Contacts: 0
    //  Contacts:     0 
    //  Callback Expected: No  
    //  Expected Visibility: EFalse
    TestCombinationL(KErrNone, KErrNone, EFalse, EFalse);

    // Transition from EFalse --> ETrue
    // 1 Contact Case
    //  Top Contacts: 0
    //  Contacts:     1 
    //  Callback Expected: Yes (previous visibility was EFalse) 
    //  Expected Visibility: ETrue
    TestCombinationL(KErrNone, 1, ETrue, ETrue); 

    // Transition from ETrue --> EFalse
    // 1 Contact Case
    //  Top Contacts: 2
    //  Contacts:     3 
    //  Callback Expected: Yes (previous visibility was ETrue) 
    //  Expected Visibility: ETrue
    //  ****** Now the cenrep key is in Always OFF mode! ********
    TestCombinationL(2, 3, ETrue, EFalse); 

    TestAlwaysOFFL(); // all OFF cases should be applicable now       
    }
    
// ============================ MEMBER FUNCTIONS ===============================

void CUT_AddFavoritiesVisibility::SetupTCPromotion9999()
    {
    STIF_LOG("SetupTCPromotion9999");
    
    #ifdef _DEBUG
    // Set panic with code 0 to acceptable exit reason
    TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0 ); // EPanicCorruptCenRepKey    
    #endif // _DEBUG
    
    Setup(9999);    
    }


void CUT_AddFavoritiesVisibility::SetupTCPromotion3()
    {    
    STIF_LOG("SetupTCPromotion3");

    #ifdef _DEBUG
    // Set panic with code 0 to acceptable exit reason
    TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0 ); // EPanicCorruptCenRepKey    
    #endif // _DEBUG
    
    Setup(3);    
    }

void CUT_AddFavoritiesVisibility::SetupTCPromotionMinus5000()
    {
    STIF_LOG("SetupTCPromotionMinus5000");

    #ifdef _DEBUG
    // Set panic with code 0 to acceptable exit reason
    TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0 ); // EPanicCorruptCenRepKey    
    #endif // _DEBUG
    
    Setup(5000);    
    }

void CUT_AddFavoritiesVisibility::SetupTCPromotionMinus1()
    {
    STIF_LOG("SetupTCPromotionMinus1");   

    #ifdef _DEBUG
    // Set panic with code 0 to acceptable exit reason
    TestModuleIf().SetExitReason( CTestModuleIf::EPanic, 0 ); // EPanicCorruptCenRepKey    
    #endif // _DEBUG
    
    Setup(-1); // EVisibilityUndefined
    }

void CUT_AddFavoritiesVisibility::SetupAlwaysON()
    {
    STIF_LOG("SetupAlwaysON");
    Setup(EVisibilityAlwaysON);    
    }
    
void CUT_AddFavoritiesVisibility::Setup(TInt aVal)      
    {
    STIF_LOG("Setup");

    // Read local variation flags
    iRepository = CRepository::NewL( TUid::Uid( KCRUidPhonebook ) );            
    SetTCPromotionL(aVal);
                
    iCallbackTimer = CCallbackTimer::NewL();

    delete iVisibilityImpl;
    iVisibilityImpl = NULL;
    iAlreadyAddedAsObserver = EFalse;    
    iVisibilityImpl = CPbk2AddFavoritesVisibilityImpl::NewL(KCmdId);
       
    }
    
void CUT_AddFavoritiesVisibility::SetupAlwaysOFF()
    {
    STIF_LOG("SetupAlwaysOFF");    
    Setup(EVisibilityAlwaysOFF);    
    }
    
void CUT_AddFavoritiesVisibility::SetupFirstTimeUse()
    {
    STIF_LOG("SetupFirstTimeUse");    
    Setup(EVisibilityUntilFirstFavorite);     
    }
    
void CUT_AddFavoritiesVisibility::TestSettingAsObserver()    
    {
    if(!iAlreadyAddedAsObserver)
        {
        // we are supposed to receive async callback as we add ourselves
        // as observer
        iCallbackReceived = EFalse;
        iCallbackExpected = ETrue;    
        iExpectedVisibility = EFalse;
        iVisibilityImpl->SetVisibilityObserver(this);  // async callback  
        iAlreadyAddedAsObserver = ETrue;
        iCallbackTimer->StartWaitingTillCallbackOrTimeOutL();        
        CheckResult();        
        }
    }    
          
void CUT_AddFavoritiesVisibility::Teardown()
    {
    STIF_LOG("Teardown");
    delete iCallbackTimer; iCallbackTimer = NULL;
    delete iRepository; iRepository = NULL;
    delete iVisibilityImpl; iVisibilityImpl = NULL;
    iAlreadyAddedAsObserver = EFalse;    
    }
       
//*  TESTCASE("OK test", "CNone", Setup, TestPassed, Teardown)
/*
void CUT_AddFavoritiesVisibility::TestPassed()
    {
    STIF_LOG("The Ultimate Answer %d", 42);        

    TInt* p = NULL;
    TInt i = 5;
    STIF_ASSERT_NULL( p );
    STIF_ASSERT( !p );
    STIF_ASSERT_NOT_NULL( &i );
    
    TInt* p1 = &i;
    p1++;
    STIF_ASSERT_SAME( &i, &i );
    STIF_ASSERT_NOT_SAME( &i, p1 );
    }
*/
//  [End of File] - do not remove
