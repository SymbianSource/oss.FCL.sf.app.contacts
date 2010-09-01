/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook VCard active incremental
*
*/


#include "CVPbkVCardActiveIncremental.h"

CVPbkVCardActiveIncremental::CVPbkVCardActiveIncremental(
    TInt                                            aPriority, 
    MVPbkVCardIncremental&                          aIncremental,
    CVPbkVCardData&                                 aData,
    MVPbkSingleContactOperationObserver&            aObserver)
    :   CActive(aPriority), 
        iIncremental( aIncremental ),
        iData( aData ),
        iObserver( aObserver )
    {
    CActiveScheduler::Add(this);
    }

void CVPbkVCardActiveIncremental::Start()
    {
    if (!IsActive())
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    }

void CVPbkVCardActiveIncremental::StartWaiting()
    {
    if (!IsActive())
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KRequestPending);
        SetActive();
        }
    }

void CVPbkVCardActiveIncremental::Complete( TInt aError )
    {
    if ( IsActive() && iStatus == KRequestPending )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, aError );
        User::WaitForRequest( iStatus );
        }
    }

CVPbkVCardActiveIncremental::~CVPbkVCardActiveIncremental()
    {
    CActive::Cancel();
    }

void CVPbkVCardActiveIncremental::RunL()
    {
    User::LeaveIfError( iStatus.Int() );

    TInt steps( 0 );
    TInt retval( iIncremental.Next( steps ) );
    User::LeaveIfError( retval );

    if ( steps > 0 )
        {
        Start();
        }      
    else
        {
        iIncremental.CloseL();
        }
    }

void CVPbkVCardActiveIncremental::DoCancel()
    {
    // do nothing
    }

TInt CVPbkVCardActiveIncremental::RunError( TInt aError )
    {
    iIncremental.HandleLeave( aError );
    return KErrNone;
    }

void CVPbkVCardActiveIncremental::Cancel()
    {
    CActive::Cancel();
    }

void CVPbkVCardActiveIncremental::StartL()
    {
    Start();
    }
// End of file
