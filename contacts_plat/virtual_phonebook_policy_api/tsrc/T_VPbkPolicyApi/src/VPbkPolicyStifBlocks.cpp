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
* Description:  STIF test module for testing VirtualPhonebook Policy API
*
*/


#include "T_VPbkPolicyApi.h"

#include <CVPbkContactCopyPolicyManager.h>
#include <MVPbkContactCopyPolicy.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>

_LIT( KStoreURI, "cntdb://c:testcontacts.cdb" );

// -----------------------------------------------------------------------------
// CVPbkPolicyApi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CT_VPbkPolicyApi::Delete()
    {
    delete iContactManager;
    iContactManager = 0;
    }

// -----------------------------------------------------------------------------
// CT_VPbkPolicyApi::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CT_VPbkPolicyApi::RunMethodL( CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestPolicyManager", 
        		CT_VPbkPolicyApi::TestPolicyManager ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );    
    }

// -----------------------------------------------------------------------------
// CT_VPbkPolicyApi::TestPolicyManager
// -----------------------------------------------------------------------------
//
TInt CT_VPbkPolicyApi::TestPolicyManager() 
	{
	TRAPD( err, TestPolicyManagerL() );
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkPolicyApi::TestPolicyManagerL
// -----------------------------------------------------------------------------
//
void CT_VPbkPolicyApi::TestPolicyManagerL()
	{
	CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
    uriArray->AppendL( TVPbkContactStoreUriPtr( KStoreURI ) );
	iContactManager = CVPbkContactManager::NewL( *uriArray );
	
    CleanupStack::PopAndDestroy( uriArray );
    Delete();
	}

// End of File
