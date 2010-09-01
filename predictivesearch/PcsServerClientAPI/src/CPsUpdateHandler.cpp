/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is the client side internal file to handle
*               cache update notifications sent by the server using
*               Publish and Subscribe framework.
*
*/

#include <CPsRequestHandler.h>
#include "CPsUpdateHandler.h"


// -----------------------------------------------------------------------------
// CPsUpdateHandler::CPsUpdateHandler
// C++ constructor
// -----------------------------------------------------------------------------
CPsUpdateHandler::CPsUpdateHandler( CPSRequestHandler& aRequestHandler, TInt aPsKey ) :
    CActive( EPriorityStandard ),
    iRequestHandler( aRequestHandler ),
    iPsKey( aPsKey )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CPsUpdateHandler::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
CPsUpdateHandler* CPsUpdateHandler::NewLC( CPSRequestHandler& aRequestHandler, TInt aPsKey )
    {
    CPsUpdateHandler* self = new (ELeave) CPsUpdateHandler( aRequestHandler, aPsKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CPsUpdateHandler::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
CPsUpdateHandler* CPsUpdateHandler::NewL( CPSRequestHandler& aRequestHandler, TInt aPsKey )
    {
    CPsUpdateHandler* self = CPsUpdateHandler::NewLC( aRequestHandler, aPsKey );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPsUpdateHandler::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
void CPsUpdateHandler::ConstructL()
    {
    TInt err = iProperty.Attach( KPcsInternalUidCacheStatus, iPsKey );
    User::LeaveIfError(err);
    
    iProperty.Subscribe(iStatus);
    SetActive();
    }

// -----------------------------------------------------------------------------
// CPsUpdateHandler::~CPsUpdateHandler
// Destructor
// -----------------------------------------------------------------------------
CPsUpdateHandler::~CPsUpdateHandler()
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CPsUpdateHandler::DoCancel
// Cancel subscription to the property
// -----------------------------------------------------------------------------
void CPsUpdateHandler::DoCancel()
    {
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// CPsUpdateHandler::RunL
// Handle change in the subscribed property
// -----------------------------------------------------------------------------
void CPsUpdateHandler::RunL()
    {
    iProperty.Subscribe(iStatus);
    SetActive();
    
    // Notify all observers about the update
    switch ( iPsKey )
        {
        case EPsKeyContactRemovedCounter:
            iRequestHandler.NotifyCachingStatus( ECacheUpdateContactRemoved, 0 );
            break;
        case EPsKeyContactModifiedCounter:
            iRequestHandler.NotifyCachingStatus( ECacheUpdateContactModified, 0 );
            break;
        case EPsKeyContactAddedCounter:
            iRequestHandler.NotifyCachingStatus( ECacheUpdateContactAdded, 0 );
            break;
        default:
            break;
        }
    }

// end of file
