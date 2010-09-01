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
*     Updates SMS events contact name to match one in contact model
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logengevents.h>
#endif
#include <logview.h>
#include <logcli.h>
#include "CLogsSMSEventUpdater.h"
#include "MLogsObserver.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS 
const TInt KFieldId = 5;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::
// ----------------------------------------------------------------------------
//
CLogsSMSEventUpdater::CLogsSMSEventUpdater(
    RFs& aFsSession,
    MLogsObserver* aObserver ) :
        CLogsBaseUpdater( aFsSession, aObserver )
    {
    }

// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::
// ----------------------------------------------------------------------------
//
CLogsSMSEventUpdater* CLogsSMSEventUpdater::NewL(
    RFs& aFsSession,
    MLogsObserver* aObserver,
    CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags )
    {
    CLogsSMSEventUpdater* self = new ( ELeave ) CLogsSMSEventUpdater( aFsSession, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aMatchFlags );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsSMSEventUpdater::ConstructL( CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags )
    {
    BaseConstructL( aMatchFlags );
    iLogViewEvent = CLogViewEvent::NewL( *iLogClient, *this );
    iFilter->SetEventType( KLogShortMessageEventTypeUid );
    iFilter->SetFlags( KLogEventContactSearched );
    iFilter->SetNullFields( ELogFlagsField );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::~CLogsSMSEventUpdater
// ----------------------------------------------------------------------------
//
CLogsSMSEventUpdater::~CLogsSMSEventUpdater()
    {
    }


// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::StartRunningL
// ----------------------------------------------------------------------------
//
void CLogsSMSEventUpdater::StartRunningL()
    {
    Cancel();
    
    if( iLogViewEvent->SetFilterL( *iFilter, iStatus ) )  //returns true, if found log events
        {
        iState = EStateActive;
        iPhase = EFilter;
        SetActive();
        }
    else if( iObserver )
        {  
        iState = EStateSMSUpdaterFinished;
        iObserver->StateChangedL( this );
        }
    }
    
// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::ContinueRunningL
// ----------------------------------------------------------------------------
//
void CLogsSMSEventUpdater::ContinueRunningL( TInt aFieldId )
    {
    if( aFieldId != KErrNotFound )    //Set remote party etc only if a contact was found
        {
        TBuf<KFieldId> fieldText;
        fieldText.Append( aFieldId );
    	iEvent->SetRemoteParty( *iName );  //iName set in the base class
    	
        // iEvent->SetSubject( fieldText );
        }
    
    //We set ContactSearched flag on, so next time we'll skip this entry and do not 
    //attempt again update remote party (even if we did not find remote party for it).
    //Also, KLogEventContactSearched events only are accepted to 
    //EventListView (CLogsMainReader::SetOneFilterLC).
    iEvent->SetFlags( KLogEventContactSearched ); 
    iLogClient->ChangeEvent( *iEvent, iStatus );
    SetActive();
    }


// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::RunL
// ----------------------------------------------------------------------------
//
void CLogsSMSEventUpdater::RunL()
    {
    if( iStatus == KErrNone )
        {
            switch( iPhase )
            {
			case EFilter:
				if( iLogViewEvent->FirstL( iStatus ) )  //If there's at least one entry in view, let's process it
					{
                    iState = EStateActive;
					iPhase = ERead;
                    SetActive();
					}
                else
                    {
                    iState = EStateFinished;
                    }
				break;

            case ERead:
                {

				iEvent->CopyL( iLogViewEvent->Event() );
                iPhase = EUpdate;

                // SAT sms events are not updated since they do not have number    
                SearchRemotePartyL( iEvent->Number() );

                //SetActive() is called when base class has got the contact and calls ContinueRunningL
                }
                break;

			case EUpdate:
                if( iLogViewEvent->FirstL( iStatus ) )  //If there's still at least one entry in view, let's continue 
                    {                                   //processing it (this assumes that the event updated above has
					iPhase = ERead;                     //has already disappeared from view and the "next" shows now as "first"
                    SetActive();                        //in view).
                    }
                else
                    {
                    iState = EStateFinished;
                    }
         		break;

			default:
				break;
            }
        }
    
    if( iObserver ) //( <--not waiting here iState = EStateFinished!)
        {
        iObserver->StateChangedL( this );               //Inform for each change the observer that there's change in db (Maybe it's
        }                                               //worthwhile to do it immediately (as here) instead of waiting iState to reach EStateFinished.  
                                                        //This way more rereads are started but we get speedier update of list too. 
                                                        //Note that change of db (above) also invokes call of HandleLogViewChangeEventAddedL (below) too.
    }                                                   
    

// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::HandleLogViewChangeEventAddedL
//
// When new event added to db, we need to start checking it in case the new event 
// is messaging event (it is not shown before we update it by KLogEventContactSearched)
// ----------------------------------------------------------------------------
//
void CLogsSMSEventUpdater::HandleLogViewChangeEventAddedL(
    TLogId /*aId*/,
    TInt /*aViewIndex*/,
    TInt /*aChangeIndex*/,
    TInt aTotalChangeCount )
    {
    if( aTotalChangeCount > 0 )
        {
        StartL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::HandleLogViewChangeEventChangedL
//
// is this needed (as we don't know for sure if a messaging event is eventually 
// constructed by changing the db, we cannot only trust to HandleLogViewChangeEventAddedL 
// to take care of it, so let's keep the below too just in case...
// ----------------------------------------------------------------------------
//
void CLogsSMSEventUpdater::HandleLogViewChangeEventChangedL(
    TLogId /*aId*/,
    TInt /*aViewIndex*/,
    TInt /*aChangeIndex*/,
    TInt aTotalChangeCount )
    {
    if( aTotalChangeCount > 0 )
        {
        StartL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsSMSEventUpdater::HandleLogViewChangeEventDeletedL
//
// No need to do anything when delete notification received.
// ----------------------------------------------------------------------------
//
void CLogsSMSEventUpdater::HandleLogViewChangeEventDeletedL(
    TLogId /*aId*/,
    TInt /*aViewIndex*/,
    TInt /*aChangeIndex*/,
    TInt /*aTotalChangeCount*/ )
    {
    }


// End of File
