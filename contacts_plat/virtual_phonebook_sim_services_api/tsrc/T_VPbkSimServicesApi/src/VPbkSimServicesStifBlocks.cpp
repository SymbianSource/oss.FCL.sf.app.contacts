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
* Description:  STIF test module for testing VirtualPhonebook SimServices API
*
*/


#include "T_VPbkSimServicesApi.h"

#include <VPbkSimStoreFactory.h>
#include <MVPbkSimPhone.h>
#include <MVPbkSimStateInformation.h>

// -----------------------------------------------------------------------------
// CVPbkSimServicesApi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CT_VPbkSimServicesApi::Delete()
    {
    delete iPhone;
    iPhone = 0;
    
    delete iSimState;
    iSimState = 0;
    }

// -----------------------------------------------------------------------------
// CT_VPbkSimServicesApi::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CT_VPbkSimServicesApi::RunMethodL( CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "TestSimPhone", 
        		CT_VPbkSimServicesApi::TestSimPhone ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );    
    }

TInt CT_VPbkSimServicesApi::TestSimPhone() 
	{
	TRAPD( err, TestSimPhoneL() );
	return err;
	}

void CT_VPbkSimServicesApi::TestSimPhoneL()
	{
	delete iPhone;
	iPhone = 0;
	iPhone = VPbkSimStoreFactory::CreatePhoneL();
	
	delete iSimState;
	iSimState = 0;
	iSimState = VPbkSimStoreFactory::GetSimStateInformationL();
	}

// End of File
