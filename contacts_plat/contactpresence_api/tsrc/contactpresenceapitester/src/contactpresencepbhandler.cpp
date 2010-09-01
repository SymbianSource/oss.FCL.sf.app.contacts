/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include "contactpresenceapitester.h"
#include "contactpresencephonebook.h"
#include "contactpresencepbhandler.h"

// -----------------------------------------------------------------------------
// CContactPresencePbHandler::CContactPresencePbHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactPresencePbHandler::CContactPresencePbHandler( CContactPresenceApiTester& aTesterMain,
                                                      CContactPresencePhonebook& aPhonebook ) :
                                                      CActive( EPriorityStandard ),
                                                      iTesterMain( aTesterMain ),
                                                      iPhonebook( aPhonebook )
    {
    }

// -----------------------------------------------------------------------------
// CContactPresencePbHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactPresencePbHandler::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CContactPresencePbHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactPresencePbHandler* CContactPresencePbHandler::NewL( CContactPresenceApiTester& aTesterMain,
                                                            CContactPresencePhonebook& aPhonebook )
    {
    CContactPresencePbHandler* self = new ( ELeave ) CContactPresencePbHandler( aTesterMain, aPhonebook );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CContactPresencePbHandler::~CContactPresencePbHandler()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CContactPresencePbHandler::Activate
// 
// -----------------------------------------------------------------------------
//
void CContactPresencePbHandler::Activate( TPbHandlerState aHandlerState )
    {
    iHandlerState = aHandlerState;
    SetActive();
    }

// -----------------------------------------------------------------------------
// CContactPresencePbHandler::RunL
// 
// -----------------------------------------------------------------------------
//
void CContactPresencePbHandler::RunL()
    {
    switch( iHandlerState )
        {
        case EPbHandlerFetchContact:
            {   
            HBufC8* link = iPhonebook.FetchLinkLC( iPhonebook.FetchContactIDL() );
            iTesterMain.StoreOneContactLinkL( link, iPhonebook.ContactManager() );
            CleanupStack::Pop();  //link
            }
            break;
        case EPbHandlerCreateContact:
        default:
            break;
        }
    TInt completion = iStatus.Int() > KErrNone ? KErrNone : iStatus.Int();
    iTesterMain.RequestComplete( completion );
    }

// -----------------------------------------------------------------------------
// CContactPresencePbHandler::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CContactPresencePbHandler::DoCancel()
    {
    iTesterMain.RequestComplete( KErrCancel );
    }


//  End of File
