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
*     Recent list duplicate cleaner
*
*/


// INCLUDE FILES

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#endif
#include <logview.h>
#include "CLogsClearDuplicates.h"

// MODULE DATA STRUCTURES


// CONSTANTS


// ----------------------------------------------------------------------------
// CLogsClearDuplicates::NewL
// ----------------------------------------------------------------------------
//
CLogsClearDuplicates* CLogsClearDuplicates::NewL( RFs& aFsSession )
    {
    CLogsClearDuplicates* self = new (ELeave) CLogsClearDuplicates( aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::CLogsClearDuplicates
// ----------------------------------------------------------------------------
//
CLogsClearDuplicates::CLogsClearDuplicates( RFs& aFsSession ) :
    CActive( EPriorityStandard ),
    iFsSession( aFsSession )
    {
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::ConstructL()
    {
    iLogClient = CLogClient::NewL( iFsSession );
    
    // Initialize database view for log events having "read" flag unset 
    iFlagClearView = CLogViewRecent::NewL( *iLogClient, *this );
    iFilterFlagClear = CLogFilter::NewL();
    iFilterFlagClear->SetFlags( KLogEventRead );
    iFilterFlagClear->SetNullFields( ELogFlagsField );
    
    // Initialize database view for log events having "ALS" flag set 
    iFlagClearViewALS = CLogViewRecent::NewL( *iLogClient, *this );
    iFilterFlagClearALS = CLogFilter::NewL();
    iFilterFlagClearALS->SetFlags( KLogEventALS );
    iClearViewALSIsSet = EFalse;  	
	
    CActiveScheduler::Add( this ); 
    SetFlagClearViewL();
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::~CLogsClearDuplicates
// ----------------------------------------------------------------------------
//
CLogsClearDuplicates::~CLogsClearDuplicates()
    {
    Cancel();     
    delete iFlagClearView;
    delete iFilterFlagClear;
    
    delete iFlagClearViewALS;
    delete iFilterFlagClearALS;
    
    delete iLogClient;
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::DoCancel()
    {     
    if( iFlagClearView )
        {
        iFlagClearView->Cancel();
        }

    if( iFlagClearViewALS )
        {
        iFlagClearViewALS->Cancel();
        }
        
    if( iLogClient )
        {
        iLogClient->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::RunL
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::RunL()
    {
    // If iClearViewALSIsSet is false, the asynchronous request made in 
    // SetFlagClearViewL is finished. So we can proceed to call SetFlagClearViewALSL.
    if ( !iClearViewALSIsSet )
    	{
    	SetFlagClearViewALSL();
    	}
    
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::SetFlagClearViewALSL
// 
// Note: this will only get called if CLogsClearDuplicates::SetFlagClearViewL()
// calls SetActive (there are unread events in the view)
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::SetFlagClearViewALSL()
    {
    Cancel();
    
    if( iFlagClearViewALS->SetRecentListL( ELogsMissedModel, *iFilterFlagClearALS, iStatus ))
        {
        iClearViewALSIsSet = ETrue;  // Set to ETrue, so RunL won't call this again
        SetActive();   
        }
    }
    
// ----------------------------------------------------------------------------
// CLogsClearDuplicates::SetFlagClearViewL
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::SetFlagClearViewL()
    {
    Cancel();
    
    if( iFlagClearView->SetRecentListL( ELogsMissedModel, *iFilterFlagClear, iStatus ) )
        {
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::HandleLogViewChangeEventAddedL
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::HandleLogViewChangeEventAddedL(
    TLogId /*aId*/,
    TInt /*aViewIndex*/,
    TInt /*aChangeIndex*/,
    TInt aTotalChangeCount )
    {
    if( aTotalChangeCount && !IsActive() )
        {
        iClearViewALSIsSet = EFalse;
        SetFlagClearViewL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::HandleLogViewChangeEventChangedL
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::HandleLogViewChangeEventChangedL(
    TLogId /*aId*/,
    TInt /*aViewIndex*/,
    TInt /*aChangeIndex*/,
    TInt /*aTotalChangeCount*/ ) // no need to do anything in change notification
    {
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::HandleLogViewChangeEventDeletedL
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::HandleLogViewChangeEventDeletedL(
    TLogId /*aId*/,
    TInt /*aViewIndex*/,
    TInt /*aChangeIndex*/,
    TInt /*aTotalChangeCount*/ ) // no need to do anything in delete notification
    {
    }

// ----------------------------------------------------------------------------
// CLogsClearDuplicates::ClearDuplicatesL
// ----------------------------------------------------------------------------
//
void CLogsClearDuplicates::ClearDuplicatesL( 
    TBool aDuplicates,
    CLogViewRecent* aView )
    {  
    // aDuplicates is true if there are events in missed view that have duplicates.
    // Clear all duplicates from provided CLogViewRecent
    if( aDuplicates && aView )
        {
        aView->ClearDuplicatesL();
        }
    

    // First flag all "not read" events to the secondary number (ALS) as "read". 
    // This has to be done separately cause just calling 
    // iFlagClearView->SetFlagsL(KLogEventRead) below would overwrite the ALS flag.  
    if( aView && iFlagClearViewALS->CountL() )  
        {        
        iFlagClearViewALS->SetFlagsL( KLogEventRead | KLogEventALS );
        }
        
    //Then flag all remaining "not read" events to "read" status. The "read" flag needs to be set
    //to prevent the already processd duplicates to show up later again. This is 
    //because an existing event turn to be a duplicate again if a new similar event 
    //is later inserted to db. So ClearDuplicatesL() just is not enough for this.
    if( aView && iFlagClearView->CountL() )  
        {        
        iFlagClearView->SetFlagsL( KLogEventRead );
        }
  	
    // 	
    iClearViewALSIsSet = EFalse;
   
    }
