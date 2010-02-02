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
*     Class for setting a missed calls event and its duplicates read. This is needed only for 
*     clearing the new missed icon and duplicate counters for one missed call event if a call or 
*     message is initiated. 
*     
*
*/


// INCLUDE FILES
#include <logcli.h>
#include <logview.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logwrap.hrh>
#else
#include <logwrap.hrh>
#include <logfilterandeventconstants.hrh>
#endif
#include "CLogsClearNewMissed.h"
#include "MLogsObserver.h"
#include "LogsUID.h"
#include "LogsEngConsts.h"




// ----------------------------------------------------------------------------
// CLogsClearNewMissed::NewL
// ----------------------------------------------------------------------------
//
CLogsClearNewMissed* CLogsClearNewMissed::NewL( 
    RFs& aFsSession)
    {
    CLogsClearNewMissed* self = new (ELeave) CLogsClearNewMissed( aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::CLogsClearNewMissed
// ----------------------------------------------------------------------------
//
CLogsClearNewMissed::CLogsClearNewMissed( 
    RFs& aFsSession ) :
        CActive( EPriorityStandard ),
        iFsSession( aFsSession ),
        iPhase( EUnInitilized )
    {
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsClearNewMissed::ConstructL()
    {
    iLogClient = CLogClient::NewL( iFsSession );
    iPhase = EUnInitilized;
   
    CActiveScheduler::Add( this ); 
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::~CLogsClearNewMissed
// ----------------------------------------------------------------------------
//
CLogsClearNewMissed::~CLogsClearNewMissed()
    {
    Cancel(); 
    delete iLogViewRecent;
    delete iRecentFilter;
    delete iLogViewDuplicates;
    delete iDuplicateFilter;
    delete iLogClient;    
    delete iLogEvent;
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsClearNewMissed::DoCancel()
    {    
    if (iLogViewDuplicates)
        {
        iLogViewDuplicates->Cancel();
        }
        
    if (iLogViewRecent)
        {
        iLogViewRecent->Cancel();
        }
    
    if( iLogClient )
        {
        iLogClient->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::SetNewMissedCallRead
// ----------------------------------------------------------------------------
//
void CLogsClearNewMissed::SetNewMissedCallRead(TLogId aLogId, TInt aDuplicates) 
    {
    iDuplicates = aDuplicates;
    
    // First we must read the event from the log db 
    if ( !IsActive() && (iPhase == EFinished || iPhase == EUnInitilized ) )
        {
        TRAPD(err, GetEventL(aLogId));
        if (err)
            {
            // If there was a leave, just let this fail. Nothing to do.
            }
        }
    else
        {
        // If still active, we don't do anything. The UI should prevent this from being possible
        }
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::GetEventL
// ----------------------------------------------------------------------------
//    
void CLogsClearNewMissed::GetEventL(TLogId aLogId) 
    {
    delete iLogEvent;
    iLogEvent = NULL;
    iLogEvent = CLogEvent::NewL();
    
    // When this asynchronous call finishes, the log event will contain all the event data
    // corresponding to the event id. 
    iLogEvent->SetId(aLogId);
    iLogClient->GetEvent( *iLogEvent, iStatus );
      
    iPhase = EGetEvent;
    SetActive();  
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::RunL
// ----------------------------------------------------------------------------
//
void CLogsClearNewMissed::RunL()
    {
    if (iStatus != KErrNone)
        {
        iPhase = EFinished;
        }
    else    
        {
        switch(iPhase)
            {
            // 1.st phase: event data fetched from db. 
            case EGetEvent: 
                {      
                // If event has duplicates continue to setting the CLogViewRecent if possible
                if ( TrySetRecentViewL() )
                    {
                    iPhase = ESetRecentView;
                    SetActive();
                    } 
                // Otherwise just set this event read. 
                else
                    {  
                    iLogEvent->SetFlags( iLogEvent->Flags() | KLogEventRead );                                  
                    iLogClient->ChangeEvent( *iLogEvent, iStatus );
                    
                    iPhase = EChangeEvent;
                    SetActive();  
                    }                
                break;
                }
           
            case EChangeEvent: // If event has duplicates this state is skipped.    
                {
                // Nothing to do anymore
                iPhase = EFinished;
                break;
                }
                   
            // Recent view set, set the duplicate view 
            case ESetRecentView:       
                {              
                if ( TrySetDuplicatesViewL() )
                    {
                    iPhase = ESetDuplicatesView;
                    SetActive(); 
                    }
                else
                    {
                    // if there are no duplicates after all, just set the KLogEventRead flag 
                    // for the recent view
                    SetReadFlagsL();
                    iPhase = EFinished;
                    }
                break;
                }
                
            // Duplicate view set, set KLogEventRead flag for the views
            case ESetDuplicatesView: 
                {    
                SetReadFlagsL();
                iPhase = EFinished;
                break;
                }
 
            default:
                {
                break;
                }
            }    
        }
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::TrySetDuplicatesViewL
// ----------------------------------------------------------------------------
//
TBool CLogsClearNewMissed::TrySetDuplicatesViewL() 
    {
    if( !iLogViewDuplicates )
        {
        iLogViewDuplicates = CLogViewDuplicate::NewL( *iLogClient );
        }
    
    // Duplicates filter is always the same, no need to recreate
    // or "reset" after it has once been set
    if( !iDuplicateFilter)
        {
        iDuplicateFilter = CLogFilter::NewL();
        iDuplicateFilter->SetFlags( KLogEventRead );
        iDuplicateFilter->SetNullFields( ELogFlagsField );    
        }
             
    //DuplicatesL refreshes duplicate event view with the duplicates of the current 
    //event in the recent event list view. Is an asynchronous request.
    //rc = False, if there are no events in the view. 
    TBool rc;
    rc = iLogViewRecent->DuplicatesL( *iLogViewDuplicates, 
                                      *iDuplicateFilter, iStatus );                   
    if (rc)
        {
        // Duplicates view set and contains events
        return ETrue;
        }
    else 
        {
        // If no duplicates after all revert back to recent view.
        return EFalse;
        }
    }
              
// ----------------------------------------------------------------------------
// CLogsClearNewMissed::TrySetRecentViewL
// ----------------------------------------------------------------------------
//
TBool CLogsClearNewMissed::TrySetRecentViewL()
    {
    // If there are no duplicates or event has neither Number or Remoteparty
    // set, then stop now and revert to just setting the latest event read
    if ( iDuplicates == 0 ||
         ( iLogEvent->Number().Length() == 0 &&
           iLogEvent->RemoteParty().Length() == 0 )   )
        {            
        return EFalse;
        }
    
    // Otherwise continue to settin the recent view, which is needed to get the duplicates view
    // of the recent event.
    //
    if( !iLogViewRecent )
        {
        iLogViewRecent = CLogViewRecent::NewL( *iLogClient );
        }
    //
    SetRecentFilterL();
    
    
    TInt rc;
    rc = iLogViewRecent->SetRecentListL( ( TInt8 )ELogsMissedModel, *iRecentFilter, iStatus );    //asynch request

    if( rc == 1 )
        {
        // If just one event in the list, like it should, continue to getting the duplicates view.
        return ETrue;
        }
    else  
        {
        // No events in the view after all or multiple events found, return EFalse
        // will revert to just setting the latest event read
        return EFalse;
        }
      
    }

// ----------------------------------------------------------------------------
// CLogsClearNewMissed::SetRecentFilterL   
// ----------------------------------------------------------------------------
//
void CLogsClearNewMissed::SetRecentFilterL()
    {
    // Always start fresh with the filter, so delete the old one and create new.
    delete iRecentFilter;
    iRecentFilter = NULL;
    iRecentFilter = CLogFilter::NewL();
    
    if (iLogEvent->Number().Length() != 0)
        {
        iRecentFilter->SetNumber(iLogEvent->Number()); 
        }
    
    if (iLogEvent->RemoteParty().Length() != 0 )
        {   
        iRecentFilter->SetRemoteParty(iLogEvent->RemoteParty()); 
        }
    
    }

 
// ----------------------------------------------------------------------------
// CLogsClearNewMissed::SetReadFlagsL
// ----------------------------------------------------------------------------
//
void CLogsClearNewMissed::SetReadFlagsL()
    {
    iLogViewRecent->SetFlagsL( iLogEvent->Flags() | KLogEventRead );
    
    // By setting the duplicates view to read, the duplicates counter is also reseted
    // incase there will be new missed calls from the same number after SetReadFlagsL
    // (and CLogsClearDuplicates has not been run).
    if (iLogViewDuplicates && iPhase == ESetDuplicatesView)
        { 
        iLogViewDuplicates->SetFlagsL( iLogEvent->Flags() | KLogEventRead ); 
        }
    }
            
// ----------------------------------------------------------------------------
// CLogsClearNewMissed::RunError
// ----------------------------------------------------------------------------
//
TInt CLogsClearNewMissed::RunError(TInt aError)
	{
	if( aError == KErrAccessDenied )
		{
		return KErrNone;
		}
	else
		{
		return aError;
		}
	}
