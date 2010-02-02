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
*     Main list reader
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logengevents.h>
#include <logfilterandeventconstants.hrh>
#endif
#include <logview.h>
#include "CLogsMainReader.h"
#include "MLogsObserver.h"
#include "MLogsEvent.h"
#include "MLogsReaderConfig.h"
#include "LogsApiConsts.h"  //additional event uids
#include "MLogsEventArray.h"
// CONSTANTS

// ----------------------------------------------------------------------------
// CLogsMainReader::NewL
// ----------------------------------------------------------------------------
//
CLogsMainReader* CLogsMainReader::NewL(
    RFs& aFsSession,
    MLogsEventArray& aEventArray,
    TLogsEventStrings& aStrings,
    TLogsModel aModelId,
    MLogsObserver* aObserver,
    CLogsEngine* aLogsEngineRef )
    {
    CLogsMainReader* self = new (ELeave) CLogsMainReader
                                        (   aFsSession
                                        ,   aEventArray
                                        ,   aStrings
                                        ,   aModelId
                                        ,   aObserver
                                        ,   aLogsEngineRef
                                        );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsMainReader::CLogsMainReader
// ----------------------------------------------------------------------------
//
CLogsMainReader::CLogsMainReader(
    RFs& aFsSession,
    MLogsEventArray& aEventArray,
    TLogsEventStrings& aStrings,
    TLogsModel aModelId,
    MLogsObserver* aObserver,
    CLogsEngine* aLogsEngineRef ) :
        CLogsBaseReader(
            aFsSession,
            aEventArray,
            aStrings,
            aModelId,
            aObserver,
            aLogsEngineRef )
    {
    }

// ----------------------------------------------------------------------------
// CLogsMainReader::~CLogsMainReader
// ----------------------------------------------------------------------------
//
CLogsMainReader::~CLogsMainReader()
    {
    Cancel();    
    delete iLogViewEvent;

    if( iFilterList )
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        }
    }

// ----------------------------------------------------------------------------
// CLogsMainReader::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsMainReader::DoCancel()
    {
    CLogsBaseReader::DoCancel();    
    iLogViewEvent->Cancel();
    }

// ----------------------------------------------------------------------------
// CLogsMainReader::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsMainReader::ConstructL()
    {
    BaseConstructL();   
    iLogClientRef->SetGlobalChangeObserverL( this );
    iLogViewEvent = CLogViewEvent::NewL( *iLogClientRef, *this );
    CActiveScheduler::Add( this ); 
    }

// ----------------------------------------------------------------------------
// CLogsMainReader::DoNextL
// ----------------------------------------------------------------------------
//
TBool CLogsMainReader::DoNextL()
    {
    TBool rc( EFalse );

    switch( iPhase )
        {
        case ERead:
            rc = iLogViewEvent->NextL( iStatus );
            break;

        case EFilter:
            rc = iLogViewEvent->FirstL( iStatus );
            if( rc )
                {
                iPhase = ERead;
                
                }
            else
                {
                iPhase = EDone;
                iObserver->StateChangedL( this );
                }
            break;

        default:
            break;
        } 

    return rc;
    }


// ----------------------------------------------------------------------------
// CLogsMainReader::SetOneFilterLC
// ----------------------------------------------------------------------------
//
CLogFilter* CLogsMainReader::SetOneFilterLC( TUid aFilterType )
    {
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( aFilterType );

    //For message type events we only include events that have already been 
    //searched for remoty party information (CLogsSMSEventUpdater::RunL)
    if( aFilterType == KLogShortMessageEventTypeUid || 
        aFilterType == KLogsEngMmsEventTypeUid )
        {
        filter->SetFlags( KLogEventContactSearched );
        }
    return filter;
    }


// ----------------------------------------------------------------------------
// CLogsMainReader::SetOneFilterLC
// ----------------------------------------------------------------------------
//
CLogFilter* CLogsMainReader::SetOneFilterLC( TLogString& aDirection )
    {
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetDirection( aDirection );
    return filter;
    }

// ----------------------------------------------------------------------------
// CLogsMainReader::SetOneFilterLC
// ----------------------------------------------------------------------------
//
CLogFilter* CLogsMainReader::SetOneFilterLC(
    const TDesC* aNumber,
    const TDesC* aName )
    {
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    
    if( aNumber->Length() > 0 )
        {
        filter->SetNumber( *aNumber );
        }
    else
        {
        filter->SetNullFields( ELogNumberField );
        }
        
    if( aName->Length() > 0 )
        {
        filter->SetRemoteParty( *aName );
        }
    else
        {
        filter->SetNullFields( ELogRemotePartyField );
        }
        
    return filter;
    }    



// ----------------------------------------------------------------------------
// CLogsMainReader::SetFilterL
// ----------------------------------------------------------------------------
//
void CLogsMainReader::SetFilterL(
    const TLogsFilter aFilter,
    const TDesC* aNumber,
    const TDesC* aName )
    {
    //We first empty the event array so that listing new filtered entries in the UI will always start from
    //beginning of an empty list without any old entries. This is because user may very quickly scroll to end 
    //of list in UI by scrolling up in the top of the list. This may cause problems because old lines will be 
    //soon deleted or replaced by new filtered lines.
    iEventArray.Reset();

    if( iFilterList )
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        iFilterList = NULL;
        }
        
    iFilterList = new ( ELeave ) CLogFilterList;

    switch( aFilter )
        {
        case ELogsFilterVoice:
            iFilterList->AppendL( SetOneFilterLC( KLogCallEventTypeUid ) );
            CleanupStack::Pop();
            //Show also Poc info events here
            iFilterList->AppendL( SetOneFilterLC( KLogsEngPocInfoEventTypeUid ) );
            CleanupStack::Pop();
            break;

        case ELogsFilterData:
            iFilterList->AppendL( SetOneFilterLC( KLogDataEventTypeUid ) );
            CleanupStack::Pop();
            break;

        case ELogsFilterMessages:
            iFilterList->AppendL( SetOneFilterLC( KLogShortMessageEventTypeUid ) );
            CleanupStack::Pop();
            // Mms events not (yet) logged to log db:
            // iFilterList->AppendL( SetOneFilterLC( KLogsEngMmsEventTypeUid ) );
            // CleanupStack::Pop();
            break;

        case ELogsFilterPacket:
            iFilterList->AppendL( SetOneFilterLC( KLogPacketDataEventTypeUid ) );
            CleanupStack::Pop();
            break;

        case ELogsFilterWlan:
            iFilterList->AppendL( SetOneFilterLC( KLogsEngWlanEventTypeUid ) );
            CleanupStack::Pop();
            break;

        case ELogsFilterPerson:
            iFilterList->AppendL( SetOneFilterLC( aNumber, aName ) );
            CleanupStack::Pop();
            break;

        case ELogsFilterIncoming:
            iFilterList->AppendL( SetOneFilterLC( iStrings.iInDirection ) );
            CleanupStack::Pop();
            iFilterList->AppendL( SetOneFilterLC( iStrings.iInDirectionAlt ) );
            CleanupStack::Pop();
            break;

        case ELogsFilterOutgoing:
            iFilterList->AppendL( SetOneFilterLC( iStrings.iOutDirection ) );
            CleanupStack::Pop();
            iFilterList->AppendL( SetOneFilterLC( iStrings.iOutDirectionAlt ) );
            CleanupStack::Pop();
            break;

        case ELogsFilterAllComms:
            iFilterList->AppendL( SetOneFilterLC( KLogShortMessageEventTypeUid ) );
            CleanupStack::Pop();
            // Mms events not (yet) logged to log db:            
            // iFilterList->AppendL( SetOneFilterLC( KLogsEngMmsEventTypeUid ) );
            // CleanupStack::Pop();
            iFilterList->AppendL( SetOneFilterLC( KLogCallEventTypeUid ) );
            CleanupStack::Pop();
            iFilterList->AppendL( SetOneFilterLC( KLogDataEventTypeUid ) );
            CleanupStack::Pop();
            iFilterList->AppendL( SetOneFilterLC( KLogFaxEventTypeUid ) );
            CleanupStack::Pop();
            iFilterList->AppendL( SetOneFilterLC( KLogPacketDataEventTypeUid ) );
            CleanupStack::Pop();
            iFilterList->AppendL( SetOneFilterLC( KLogsEngWlanEventTypeUid ) );
            CleanupStack::Pop();
            iFilterList->AppendL( SetOneFilterLC( KLogsEngPocInfoEventTypeUid ) );
            CleanupStack::Pop();
            break;

        default:            
            break;
        }
    }


// ----------------------------------------------------------------------------
// CLogsMainReader::Stop
// ----------------------------------------------------------------------------
//
void CLogsMainReader::Stop()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// CLogsMainReader::StartL
// ----------------------------------------------------------------------------
//
void CLogsMainReader::StartL()
    {
    iIndex = 0;
    Cancel();
    iDirty = EFalse;
    iInterrupted = EFalse;
   
    TBool rc( EFalse );

    if( ! iFilterList )
        {
        // opened first time
        SetFilterL( ELogsFilterAllComms );
        }

    if( iFilterList )
        {
        TRAPD(  err
             ,  rc = iLogViewEvent->SetFilterParseFilterByFilterL
                                        (   *iFilterList
                                        ,   iStatus
                                        )
             );
        if( err == KErrAccessDenied )
            {
            iDirty = ETrue;
            return;
            }
        User::LeaveIfError(err);
        }
    
    if( rc )
        {
        iPhase = EFilter;
        iState = EStateInitializing;
        SetActive();
        }
    else if( iObserver )
        {
        iEventArray.Reset();
        iPhase = EDone;
        iState = EStateFinished;
        iObserver->StateChangedL( this );
        }
    } 

// ----------------------------------------------------------------------------
// CLogsMainReader::ConfigureL
// ----------------------------------------------------------------------------
//
void CLogsMainReader::ConfigureL( const MLogsReaderConfig* aConfig ) 
    {
    SetFilterL
        (   aConfig->Filter()
        ,   &aConfig->Number()
        ,   &aConfig->RemoteParty()
        );
    }
       

// ----------------------------------------------------------------------------
// CLogsMainReader::ViewCountL
// ----------------------------------------------------------------------------
//
TInt CLogsMainReader::ViewCountL() const
    {
    return iLogViewEvent->CountL();
    }


// ----------------------------------------------------------------------------
// CLogsMainReader::Event
// ----------------------------------------------------------------------------
//
CLogEvent& CLogsMainReader::Event() const
    {
    //The RVCT compiler provides warnings "type qualifier on return type is meaningless"
    //for functions that return const values. In order to avoid these numerous warnings and 
    //const cascading, the CLogEvent is const_casted here.
    return const_cast<CLogEvent&>( iLogViewEvent->Event() );
    }

// ----------------------------------------------------------------------------
// CLogsMainReader::ConstructEventL
// ----------------------------------------------------------------------------
//
void CLogsMainReader::ConstructEventL(
    MLogsEvent& aDest,
    const CLogEvent& aSource )
    {
    BaseConstructEventL( aDest, aSource );
    }


// ----------------------------------------------------------------------------
// CLogsMainReader::HandleLogClientChangeEventL
//
// Called by Log Database engine when it notifies a change in database (e.g.
// Log database cleared)
// ----------------------------------------------------------------------------
//
void CLogsMainReader::HandleLogClientChangeEventL(
    TUid aChangeType,
    TInt /*aChangeParam1*/,
    TInt /*aChangeParam2*/,
    TInt /*aChangeParam3*/ )
    {
    if( aChangeType == KLogClientChangeEventLogCleared )
        {
        HandleViewChangeL();
        }
    }
