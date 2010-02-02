/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Includes AoC's and CT's commonly needed functions, multi mode ETEL version.
*
*/


//  INCLUDE FILES  
#include <e32svr.h>

#include <secui.h>
#include <SecUiSecurityHandler.h>
#include <SecUi.rsg>

#include <exterror.h>

#include "CLogsMMEAocUtil.h"

// CONSTANTS

#ifdef __WINS__ 
// for message to user
_LIT(KMissingImplementation,"AoC functionality missing");
// Aoc support variable
const TInt KAocSupport = 1; // information service
// currency / unit mode
const TReal KCurrencyMode = 1.5; // currency mode
// call cost limit
const TInt KCallCostLimit = 0; // Off
#endif


// ================= MEMBER FUNCTIONS =======================

//  Standard creation function.
CLogsAocUtil* CLogsAocUtil::NewL( )
    {
    CLogsAocUtil* self = new ( ELeave ) CLogsAocUtil( );
    CleanupStack::PushL( self );
	self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

//  Destructor
CLogsAocUtil::~CLogsAocUtil()
    {
    CloseEtelConnection();
    //delete iCustomEtel;
    }

void CLogsAocUtil::RunL()
    {
    if( iWait.IsStarted() )
        { 
        iWait.AsyncStop();
        }    
    }

void CLogsAocUtil::DoCancel()
    {
    if( iCancelStatus != KErrNone )
        {
        iPhone.CancelAsyncRequest( iCancelStatus );
        }
    }

//  Default constructor
void CLogsAocUtil::ConstructL()
    {
    //iCustomEtel = CLogsCustomEtel::NewL();
    CActiveScheduler::Add( this ); 
    }

//  Default constructor
CLogsAocUtil::CLogsAocUtil( )
        //: iAocSupport( KErrNotFound )
    {
    }


TInt CLogsAocUtil::AskSecCodeL()
    {
    TInt etelOk = OpenEtelConnection();
    if( etelOk != KErrNone )
        {
        return etelOk;
        }
    TBool rc(ETrue);
    TSecUi::InitializeLibL();

    TInt err;
	TRAP( err,
		CSecurityHandler* handler = new (ELeave) CSecurityHandler( iPhone );
		CleanupStack::PushL( handler );
		rc = handler->AskSecCodeL( );
		CleanupStack::PopAndDestroy(); // handler
		);

    TSecUi::UnInitializeLib();
    
    CloseEtelConnection();
    if( !rc )
        {
        return KErrCancel;
        }
    return KErrNone;

    }


/*****************************************************
*	Series 60 Customer / ETel
*	Series 60  ETel API
*****************************************************/
TInt CLogsAocUtil::GetAirTimeDuration( TTimeIntervalSeconds& /* aTime */ )
    {
    TInt rc = KErrNone;
    /*
#ifdef __WINS__ 
    User::InfoPrint(KMissingImplementation);
#endif
    aTime = 0;
    aTime = 0;
	TInt rc = OpenEtelConnection();
    if( rc != KErrNone )
        {
        return rc;
        }
    //rc = iPhone.GetAirTimeDuration( aTime ); 
    CloseEtelConnection();
    */
    return rc;
    
    }

// End of file
