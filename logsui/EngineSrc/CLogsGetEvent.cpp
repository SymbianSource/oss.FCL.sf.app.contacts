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
*     Implements interface for log event getting
*
*/


// INCLUDE FILES
// #include <PbkFields.hrh>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logengevents.h>
#endif
#include <logcli.h>
#include <logview.h>
#include <logviewchangeobserver.h>
#include <featmgr.h>
#include "CLogsGetEvent.h"
#include "CLogsEventData.h"
#include "CLogsEvent.h"
#include "MLogsObserver.h"
#include "CLogsEventGetter.h"
#include "LogsApiConsts.h"  //additional event uids

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

//========================================CLogsGetEventNotify=============================================
//
class CLogsGetEventNotify : public CActive, 
                            public MLogViewChangeObserver
    {
    public:
        /**
         * Symbian OS constructor. 
         *
         * @param aFsSession ref. to file server session
         * @param aGetEvent pointer to Logs reader
         * @return New instance of this class
         */
        static CLogsGetEventNotify* NewL
                                    (   RFs& aFsSession
                                    ,   CLogsGetEvent* aGetEvent
                                    );

        /**
         *  Destructor
         */
        ~CLogsGetEventNotify();

    private:
        /**
         * Default constructor. 
         */
        CLogsGetEventNotify();

        /**
         * Second phase constructor. 
         */
        void ConstructL();

        /**
         * C++ constructor. 
         *
         * @param aFsSession ref. to file server session
         * @param aGetEvent pointer to Logs reader
         */
        CLogsGetEventNotify( RFs& aFsSession, CLogsGetEvent* aGetEvent );

    public:
        /**
         *  Sets Log ID
         *  @param aId Log ID
         */
        void SetId( TLogId aId );

    private:
        /**
         *  Creates and returns a filter.
         *  Leaves filter object on cleanupstack.
         *
         *  @param  aFilterType     Filter type id.
         *
         *  @return  Pointer to created and configured filter.
         */
        CLogFilter* SetOneFilterLC( TUid aFilterType );

    public: // from MLogViewChangeObserver
        void HandleLogViewChangeEventAddedL
                        (   TLogId aId
                        ,   TInt aViewIndex
                        ,   TInt aChangeIndex
                        ,   TInt aTotalChangeCount
                        );

        void HandleLogViewChangeEventChangedL
                        (   TLogId aId
                        ,   TInt aViewIndex
                        ,   TInt aChangeIndex
                        ,   TInt aTotalChangeCount
                        );

        void HandleLogViewChangeEventDeletedL
                        (   TLogId aId
                        ,   TInt aViewIndex
                        ,   TInt aChangeIndex
                        ,   TInt aTotalChangeCount
                        );

    private: /// from CActive
        void DoCancel();

    protected: /// from CActive
        void RunL();
        TInt RunError(TInt aError);

    private: // data
        /// Own:
        TLogId              iId;

        /// Ref: File server session
        RFs&                iFsSession;

        /// Own:
        CLogsGetEvent*      iGetEvent;
        
        /// Own: Log client
        CLogClient*         iLogClient;

        /// Own:
        CLogViewEvent*      iEventView;

        /// Own: Filterlist
        CLogFilterList*     iFilterList;
    };
//    
//========================================CLogsGetEventNotify=============================================

    
    

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

CLogsGetEvent::CLogsGetEvent
    (   RFs& aFsSession
    ,   TLogsEventStrings& aStrings
    ,   MLogsObserver* aObserver
    )
    : CActive( EPriorityHigh )
    , iFsSession( aFsSession )
    , iStrings( aStrings )
    , iObserver( aObserver )
    , iState( EStateUndefined )
    , iStaticEmerg( ETrue )
    {
    }

CLogsGetEvent* CLogsGetEvent::NewL
    (   RFs& aFsSession
    ,   TLogsEventStrings& aStrings
    ,   MLogsObserver* aObserver
    )
    {
    CLogsGetEvent* self = new (ELeave) CLogsGetEvent
                                        (   aFsSession
                                        ,   aStrings
                                        ,   aObserver
                                        );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

void CLogsGetEvent::ConstructL()
    {
    iLogClient = CLogClient::NewL( iFsSession );
    iNotify = CLogsGetEventNotify::NewL( iFsSession, this );    
    iLogEvent = CLogEvent::NewL();
    CLogsEvent* event = CLogsEvent::NewLC( );      
    
    CleanupStack::Pop(); // event
    iEvent = event;
    iWrapper = CLogsEventGetter::NewL( );   
    
    if ( FeatureManager::FeatureSupported( KFeatureIdProtocolCdma ) )
        {
        iStaticEmerg = EFalse;
        }
        
    CActiveScheduler::Add( this );
    }

CLogsGetEvent::~CLogsGetEvent()
    {
    Cancel();
    delete iLogClient;
    delete iLogEvent;
    delete iEvent;
    delete iWrapper;
    delete iNotify;
    }

void CLogsGetEvent::Get( TLogId aId )
    {
    Cancel();
    iLogEvent->SetId( aId );
    iNotify->SetId( aId );
    iLogClient->GetEvent( *iLogEvent, iStatus ); //Get details from db.This is an asynchronous request
    iState = EStateActive;        
    SetActive();
    }

const MLogsEventGetter* CLogsGetEvent::Event()
    {
    return iWrapper->Wrap( iEvent );
    }

const CLogEvent* CLogsGetEvent::LogEvent() const
    {
    return iLogEvent;
    }

void CLogsGetEvent::SetObserver( MLogsObserver* aObserver )
    {
    iObserver = aObserver;      //Observer to indicate when asynchronous retrieval is ok
    }

void CLogsGetEvent::DoCancel()
    {      
    iLogClient->Cancel();
    }

void CLogsGetEvent::RunL()
    {
    ConstructEventL( *iEvent, *iLogEvent );  
    iState = EStateFinished;    
    
    if( iObserver )
        {
        iObserver->StateChangedL( this ); //Indicate observer: Asynch retrieval is now done 
        }                                 // on CLogsDetailControlContainer::StateChangedL
    }

TInt CLogsGetEvent::RunError(TInt aError)
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

TLogsState CLogsGetEvent::State() const
    {
    return iState;
    }

void CLogsGetEvent::ConstructEventL( MLogsEvent& aDest, const CLogEvent& aSource )
    {
    aDest.InitializeEventL( aSource, iStrings, ELogsDetailModel );
    }


//========================================CLogsGetEventNotify=============================================
//
CLogsGetEventNotify* CLogsGetEventNotify::NewL
    (   RFs& aFsSession
    ,   CLogsGetEvent* aGetEvent
    )
    {
    CLogsGetEventNotify* self = new (ELeave) CLogsGetEventNotify
                                            (   aFsSession
                                            ,   aGetEvent
                                            );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CLogsGetEventNotify::CLogsGetEventNotify
    (   RFs& aFsSession
    ,   CLogsGetEvent* aGetEvent
    )
    : CActive( EPriorityStandard )
    , iFsSession( aFsSession )
    , iGetEvent( aGetEvent )
    {
    }

void CLogsGetEventNotify::ConstructL()
    {
    iLogClient = CLogClient::NewL( iFsSession );
    iEventView = CLogViewEvent::NewL( *iLogClient, *this );
    iFilterList = new ( ELeave ) CLogFilterList;
    iFilterList->AppendL( SetOneFilterLC( KLogShortMessageEventTypeUid ) );
    CleanupStack::Pop();
    iFilterList->AppendL( SetOneFilterLC( KLogsEngMmsEventTypeUid ) );
    CleanupStack::Pop();
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
    
    CActiveScheduler::Add( this ); 

    TRAPD( err, iEventView->SetFilterL( *iFilterList, iStatus ) );
    if( err == KErrAccessDenied )
        {
        return;
        }
    User::LeaveIfError( err );
    SetActive();
    }

CLogFilter* CLogsGetEventNotify::SetOneFilterLC( TUid aFilterType )
    {
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( aFilterType );
    return filter;
    }

CLogsGetEventNotify::~CLogsGetEventNotify()
    {
    Cancel();     
    delete iEventView;
    delete iLogClient;
    
    if( iFilterList )
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        iFilterList = NULL;
        }
    }

void CLogsGetEventNotify::DoCancel()
    {     
    if( iEventView )
        {
        iEventView->Cancel();
        }
        
    if( iLogClient )
        {
        iLogClient->Cancel();
        }
    }

void CLogsGetEventNotify::RunL()
    {
    }

TInt CLogsGetEventNotify::RunError(TInt aError)
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

void CLogsGetEventNotify::HandleLogViewChangeEventAddedL
    (   TLogId /*aId*/
    ,   TInt /*aViewIndex*/
    ,   TInt /*aChangeIndex*/
    ,   TInt /*aTotalChangeCount*/
    )
    {  
    }

void CLogsGetEventNotify::HandleLogViewChangeEventChangedL
    (   TLogId aId
    ,   TInt /*aViewIndex*/
    ,   TInt /*aChangeIndex*/
    ,   TInt aTotalChangeCount
    )
    {   
    if( aTotalChangeCount && ( iId == aId ) )
        {
        iGetEvent->Get( iId );
        }
    }

void CLogsGetEventNotify::HandleLogViewChangeEventDeletedL
    (   TLogId /*aId*/
    ,   TInt /*aViewIndex*/
    ,   TInt /*aChangeIndex*/
    ,   TInt /*aTotalChangeCount*/
    )
    {    
    }

void CLogsGetEventNotify::SetId( TLogId aId )
    {
    iId = aId;
    }
//    
//========================================CLogsGetEventNotify=============================================

    
    

//  End of File
