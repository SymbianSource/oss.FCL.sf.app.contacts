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
*     Recent list reader
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#endif
#include <logview.h>
#include "CLogsRecentReader.h"
#include "MLogsObserver.h"
#include "MLogsEvent.h"
#include "MLogsEventArray.h"
#include "CLogsClearDuplicates.h"

// MODULE DATA STRUCTURES

// CONSTANTS

// ----------------------------------------------------------------------------
// CLogsRecentReader::NewL
// ----------------------------------------------------------------------------
//
CLogsRecentReader* CLogsRecentReader::NewL(
    RFs& aFsSession,
    MLogsEventArray& aEventArray,
    TLogsEventStrings& aStrings,
    TLogsModel aModelId,
    MLogsObserver* aObserver,
    CLogsEngine* aLogsEngineRef )                
    {
    CLogsRecentReader* self = new (ELeave) CLogsRecentReader
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
// CLogsRecentReader::CLogsRecentReader
// ----------------------------------------------------------------------------
//
CLogsRecentReader::CLogsRecentReader(
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
            aLogsEngineRef ),
        iDuplicates( EFalse )
    {
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::~CLogsRecentReader
// ----------------------------------------------------------------------------
//
CLogsRecentReader::~CLogsRecentReader()
    {    
    Cancel();

/*********************************************************************************
 Changed because of EMSH-6JDFBV:
 From Jan-2006 the CLogsModel::DoDeactivate can optionally destroy missed model.
 Therefore this would wrongly clear duplicates in case UI loses foreground e.g in
 case MT missed call when missed call view is in foreground (this would cause the
 duplicate count not to increase correctely).

    if( iModelId == ELogsMissedModel )
        {
        TInt err;        
        TRAP( err, ClearDuplicatesL( iDuplicates, iLogViewRecent ) );   
        }
*********************************************************************************/        

    delete iDuplicateView;
    delete iLogViewRecent;
    delete iDuplicateFilter;
    delete iClearDuplicates;
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::ClearDuplicatesL
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::ClearDuplicatesL()
    {
    if( iLogViewRecent && iClearDuplicates )
        {
        iClearDuplicates->ClearDuplicatesL( iDuplicates, iLogViewRecent );    
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::ConstructL()
    {
    // iSkipClearDuplicates = EFalse; //reset skipping    
    
    BaseConstructL();    
    
    if( iModelId == ELogsMissedModel )
        {
        iClearDuplicates = CLogsClearDuplicates::NewL( iFsSession );
        }

    iLogViewRecent = CLogViewRecent::NewL( *iLogClientRef, *this );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::ActivateL
//
// Optimisation related to CLogsRecentReader::DeActivate (EMSH-6JDFBV).
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::ActivateL(  )
    {
    CLogsBaseReader::ActivateL();
        
    // iSkipClearDuplicates = aSkipClearDuplicates;
        
    if( !iLogViewRecent )
        {
        iLogViewRecent = CLogViewRecent::NewL( *iLogClientRef, *this );    
        iDirty = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::DeActivate
//
// Optimisation for reducing Logs resource consumption when a MT call is received and 
// in the same time processing power is needed for ring tune retrieval etc. This function
// is not necessary if calling CLogsRecentReader::Stop() provides sufficient result (EMSH-6JDFBV).
// void CLogsRecentReader::DeActivate( TBool aSkipClearDuplicates )
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::DeActivate( )
    {
    CLogsBaseReader::DeActivate( );
        
    // iSkipClearDuplicates = aSkipClearDuplicates;
    
    Cancel();   //Calls syncronously DoCancel() below

    /* optimisation attempt for EMSH-6JDFBV *
        if( iClearDuplicates && !iSkipClearDuplicates )
            {
            delete iClearDuplicates; //We've already called cancel for iClearDuplicates in DoCancel()
            iClearDuplicates = NULL;
            }
    */

    delete iLogViewRecent;
    iLogViewRecent = NULL;
    iDirty = ETrue;
    
    //Reset event array, because otherwise navigating in UI between recent views takes too long to 
    //read last status from database. That may cause user to use wrong event line in view.
    //If performance is good enough, the below is not needed.    
    iEventArray.Reset(); 
    
    TInt err;        
    TRAP( err, iObserver->StateChangedL( this ) );
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::DoCancel()
    {
    if( iLogViewRecent )
        {
        iLogViewRecent->Cancel();
        }
    
    if( iDuplicateView )
        {
        iDuplicateView->Cancel();
        }
        
    /* optimisation attempt for EMSH-6JDFBV *
    if( iClearDuplicates && iSkipClearDuplicates )
        {
        iClearDuplicates->Cancel(); //calls cancel for it's own view + for iLogClient
        }
    */

    CLogsBaseReader::DoCancel();    //calls iLogClient->Cancel()        
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::DoNextL
//
// Called from CLogsBaseReader::NextL
// ----------------------------------------------------------------------------
//
TBool CLogsRecentReader::DoNextL()
    {
    TBool rc( EFalse );
    
    switch( iPhase )
        {
        case ERead:
            rc = iLogViewRecent->NextL( iStatus );
            break;

        case EDuplicate:    //BaseReader sets this phase if running missed model 
            if( !iDuplicateView )
                {
                iDuplicateView = CLogViewDuplicate::NewL( *iLogClientRef );
                }
                
            if( !iDuplicateFilter)
                {
                iDuplicateFilter = CLogFilter::NewL();
                iDuplicateFilter->SetFlags( KLogEventRead );
                iDuplicateFilter->SetNullFields( ELogFlagsField );
                }

            //DuplicatesL: refreshes duplicate event view with the duplicates of the current 
            //event in the recent event list view. Is an asynchronous request.
            //rc = False, if there are no events in the view. 
            rc = iLogViewRecent->DuplicatesL( *iDuplicateView, 
                                    *iDuplicateFilter, iStatus ); 

            if( ! rc )
                {
                // no duplicates in this duplicate view, go to next one
                ++iIndex;
                iPhase = ERead;
                rc = DoNextL(); // recursion
                } 
            else
                {
                iDuplicates = ETrue;
                }
            break;

        default:
            break;
        } 

    return rc;
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::StartL
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::StartL()
    {
    //Starting may be called in total refresh situations even if we're 
    //are not active at moment. In that case this model (recent, missed, or dialled) 
    //is e.g. in the background and we don't refresh it yet (relates to EMSH-6JDFBV)
    if( !iLogViewRecent )
        {
        return;
        }
    
    iIndex = 0;
    
    //The cancel() below seems consume quite much time on hw, so we don't call unless necessary
    if( IsActive() )
        {
        Cancel();
        }
    
        
    iDirty = EFalse;
    iInterrupted = EFalse;
    TBool rc = EFalse;
    iState = EStateInitializing; //This informs observers that e.g. a complete re-read of recent events is about to begin
                                 //so that they can e.g. change their view focus settings accordingly
                                 // (e.g. CLogsRecentListView::StateChangedL )

    // FIXME: Following statement takes more than 100 ms seconds to execute. The trapd statement is 
    // known to be expensive but it should not take that long. Seems to be a Symbian caused performance 
    // bottleneck cause the asynch request should not take that long to return. 
    TRAPD( err, rc = iLogViewRecent->SetRecentListL( ( TInt8 )iModelId, iStatus ) );    //asynch request
    if( err == KErrAccessDenied )
        {
        iDirty = ETrue;
        return;
        }
    User::LeaveIfError(err);

    if( rc )
        {
        iPhase = ERead;
        SetActive();
        }
    else 
        {
        Cancel();
        if( iObserver )
            {
            iEventArray.Reset();
            iPhase = EDone;
            iState = EStateFinished;
            iObserver->StateChangedL( this );
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::DuplicateCountL
// ----------------------------------------------------------------------------
//
TInt CLogsRecentReader::DuplicateCountL() const
    {
    if( !iDuplicateView )
        {
        return 0;
        }
        
    return iDuplicateView->CountL();
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::ReadingFinishedL
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::ReadingFinishedL()
    {
    CLogsBaseReader::ReadingFinishedL();
    delete iDuplicateView;
    iDuplicateView = NULL;
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::Stop
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::Stop()
    {
    Cancel();   //Calls syncronously CLogsRecentReader::DoCancel(). Takes care of calling 
                // also Cancel() for iClearDuplicates
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::ViewCountL
// ----------------------------------------------------------------------------
//
TInt CLogsRecentReader::ViewCountL() const
    {
    return iLogViewRecent->CountL();
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::Event
// ----------------------------------------------------------------------------
//
CLogEvent& CLogsRecentReader::Event() const
    {
    //The RVCT compiler provides warnings "type qualifier on return type is meaningless"
    //for functions that return const values. In order to avoid these numerous warnings and 
    //const cascading, the CLogEvent is const_casted here.
    return const_cast<CLogEvent&>( iLogViewRecent->Event() );
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::ConstructEventL
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::ConstructEventL
    (   MLogsEvent& aDest
    ,   const CLogEvent& aSource
    )
    {
    BaseConstructEventL( aDest, aSource );
    }

// ----------------------------------------------------------------------------
// CLogsRecentReader::ConfigureL
// ----------------------------------------------------------------------------
//
void CLogsRecentReader::ConfigureL( const MLogsReaderConfig* /*aConfig*/ )
    {
    User::Leave( KErrNotSupported );
    }


