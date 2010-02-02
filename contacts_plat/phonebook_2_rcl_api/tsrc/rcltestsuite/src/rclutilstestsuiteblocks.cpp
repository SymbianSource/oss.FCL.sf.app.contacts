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
* Description:  Remote Contact Lookup test suite source file
*
*/


// SYSTEM INCLUDES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <S32MEM.H>


// USER INCLUDES
#include "rclutilstestsuite.h"
#include "rclutilstestsuitedefs.h"
#include "testsuiteinputdata.h"

#include <cpbk2remotecontactlookupaccounts.h>
#include <tpbkxremotecontactlookupprotocolaccountid.h>
#include <cpbkxremotecontactlookupprotocolaccount.h>


void CleanupResetAndDestroyAccArray(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<RPointerArray<CPbkxRemoteContactLookupProtocolAccount>*>(aObj)->ResetAndDestroy();
        }
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
void CRclUtilsTestSuite::Delete() 
    {
    REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
TInt CRclUtilsTestSuite::RunMethodL( 
    CStifItemParser& aItem ) 
    {

     TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
		ENTRY( "TestDefaultAccountIdForNullL", CRclUtilsTestSuite::TestDefaultAccountIdForNullL ),
		ENTRY( "TestDefaultAccountIdL", CRclUtilsTestSuite::TestDefaultAccountIdL ),
		ENTRY( "TestGetAccountForNullL", CRclUtilsTestSuite::TestGetAccountForNullL ),
		ENTRY( "TestGetAccountL", CRclUtilsTestSuite::TestGetAccountL ),
		ENTRY( "TestGetAllAccountsL", CRclUtilsTestSuite::TestGetAllAccountsL ),
		ENTRY( "TestGetSpecificAccountsL", CRclUtilsTestSuite::TestGetSpecificAccountsL ),		
		
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

TInt CRclUtilsTestSuite::TestDefaultAccountIdForNullL()
    {
    TInt err = KErrGeneral;
    //PreCondition : Set Default Account Id to NULL
    //Get the Original Value and Set protocolid to NULL
    TPbkxRemoteContactLookupProtocolAccountId orig_Account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();    
    const TPbkxRemoteContactLookupProtocolAccountId emptyId( TUid::Uid( 0 ), 0 );
    CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(emptyId);
            
    TPbkxRemoteContactLookupProtocolAccountId account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();            
    if ( account.iProtocolUid == TUid::Uid(0) )
        {
        err = KErrNone;
        }    
    
    //PostCondition : ReSet Default Account Id 
    CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(orig_Account);
    
    return err;
    }
	

TInt CRclUtilsTestSuite::TestDefaultAccountIdL()
    {
    TInt err = KErrGeneral;
    //PreCondition : Set Default Account Id to Dummy Adapter UID & Account Id
    //Get the Original Value and Set protocolid to Dummy Adapter UID & Account Id
    TPbkxRemoteContactLookupProtocolAccountId orig_Account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();    
    const TPbkxRemoteContactLookupProtocolAccountId dummyAdapterId( TUid::Uid( KRCLDummyAdapterProtocolUid ), KRCLDummyAdapterAccountUid );
    CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(dummyAdapterId);    
    
    TPbkxRemoteContactLookupProtocolAccountId account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();            
    if ( account.iProtocolUid == TUid::Uid(KRCLDummyAdapterProtocolUid) && account.iAccountId == KRCLDummyAdapterAccountUid )
        {
        err = KErrNone;
        }    
    
    //PostCondition : ReSet Default Account Id 
    CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(orig_Account);
    
    return err;
    }

TInt CRclUtilsTestSuite::TestGetAccountForNullL()
    {
    TInt err = KErrGeneral;
    //PreCondition : Set Default Account Id to NULL
    //Get the Original Value and Set protocolid to NULL
    TPbkxRemoteContactLookupProtocolAccountId orig_Account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();    
    const TPbkxRemoteContactLookupProtocolAccountId emptyId( TUid::Uid( 0 ), 0 );
    CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(emptyId);
    
    TPbkxRemoteContactLookupProtocolAccountId account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();
    CPbkxRemoteContactLookupProtocolAccount* protocol = CPbk2RemoteContactLookupAccounts::GetAccountL( account );
    
    if (!protocol)
        {
        err = KErrNone;
        }
    else
        {
        delete protocol;
        }
    
    //PostCondition : ReSet Default Account Id 
    CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(orig_Account);
    
    return err;
    }


TInt CRclUtilsTestSuite::TestGetAccountL()
    {
    TInt err = KErrGeneral;
    //PreCondition : Set Default Account Id to Dummy Adapter UID & Account Id
    //Get the Original Value and Set protocolid to Dummy Adapter UID & Account Id
    TPbkxRemoteContactLookupProtocolAccountId orig_Account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();    
    const TPbkxRemoteContactLookupProtocolAccountId dummyAdapterId( TUid::Uid( KRCLDummyAdapterProtocolUid ), KRCLDummyAdapterAccountUid );
    CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(dummyAdapterId);    
    
    TPbkxRemoteContactLookupProtocolAccountId account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();
    CPbkxRemoteContactLookupProtocolAccount* protocol = CPbk2RemoteContactLookupAccounts::GetAccountL( account );
    
    if (protocol && 
            protocol->Id().iProtocolUid == TUid::Uid(KRCLDummyAdapterProtocolUid) 
            && protocol->Id().iAccountId == KRCLDummyAdapterAccountUid)
        {
        err = KErrNone;
        }
    
    delete protocol;
    
    //PostCondition : ReSet Default Account Id 
    CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(orig_Account);
    
    return err;
    }


TInt CRclUtilsTestSuite::TestGetAllAccountsL()
    {
    TInt err = KErrGeneral;    
    
    CPbk2RemoteContactLookupAccounts* rclAccount = CPbk2RemoteContactLookupAccounts::NewLC();    
    
    RPointerArray<CPbkxRemoteContactLookupProtocolAccount> protocolAccounts;
    TCleanupItem cleanupItemAcc(CleanupResetAndDestroyAccArray, &protocolAccounts);
    CleanupStack::PushL(cleanupItemAcc);
        
    rclAccount->GetAllAccountsL(protocolAccounts);
    
    if ( protocolAccounts.Count() )
        {
        err = KErrNone;
        }
    
    CleanupStack::PopAndDestroy(2); // rclAccount, cleanupItemAcc    
    
    return err;
    }


TInt CRclUtilsTestSuite::TestGetSpecificAccountsL()
    {
    TInt err = KErrGeneral;    
    
    CPbk2RemoteContactLookupAccounts* rclAccount = CPbk2RemoteContactLookupAccounts::NewL();
    CleanupStack::PushL(rclAccount);
    
    RPointerArray<CPbkxRemoteContactLookupProtocolAccount> protocolAccounts;
    TCleanupItem cleanupItemAcc(CleanupResetAndDestroyAccArray, &protocolAccounts);
    CleanupStack::PushL(cleanupItemAcc);
        
    
    rclAccount->GetAccountsL(protocolAccounts, TUid::Uid(KRCLDummyAdapterProtocolUid));
    
    if ( protocolAccounts.Count() == 1)
        {
        err = KErrNone;
        }
    
    CleanupStack::PopAndDestroy(2); // rclAccount, cleanupItemAcc
    
    return err;
    }


//End of files
