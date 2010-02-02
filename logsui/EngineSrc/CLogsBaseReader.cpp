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
*     Implements interface for Logs reader. Common functionality.
*
*/


// INCLUDE FILES
#include <logcli.h>
#include "CLogsBaseReader.h"
#include "MLogsObserver.h"
#include "CLogsEvent.h"
#include "MLogsEventArray.h"
#include "LogsEngConsts.h"
#include <featmgr.h>
#include "CLogsEventData.h"
#include "CLogsEngine.h"


// CONSTANTS
const TInt KDeleteCountInit = -1;

// CONSTANTS FOR DEBUGGING
#ifdef _DEBUG
_LIT(KPanicMsg,"CLogsBaseReader");
#endif

// ----------------------------------------------------------------------------
// CLogsBaseReader::CLogsBaseReader
// ----------------------------------------------------------------------------
//
CLogsBaseReader::CLogsBaseReader( 
    RFs& aFsSession, 
    MLogsEventArray& aEventArray, 
    TLogsEventStrings& aStrings,
    TLogsModel aModelId,
    MLogsObserver* aObserver,
    CLogsEngine* aLogsEngineRef
     ) :
        CActive( EPriorityStandard ),
        iFsSession( aFsSession ),
        iEventArray( aEventArray ),
        iStrings( aStrings ),
        iObserver( aObserver ),
        iLogsEngineRef( aLogsEngineRef ),
        iPhase( EInitial ),
        iModelId( aModelId ),
        iIndex( 0 ),
        iInterrupted( EFalse ),
        iState( EStateUndefined ),
        iDirty( ETrue ),
        iActivated( EFalse ),
        iDeleteChangeCount( KDeleteCountInit ),
        iStaticEmerg( ETrue )
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::~CLogsBaseReader
// ----------------------------------------------------------------------------
//
CLogsBaseReader::~CLogsBaseReader()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::BaseConstructL
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::BaseConstructL()
    {
    iLogClientRef = iLogsEngineRef->CLogClientRef();     
    
    if ( FeatureManager::FeatureSupported( KFeatureIdProtocolCdma ) )
        {
        iStaticEmerg = EFalse;
        }
    }


// ----------------------------------------------------------------------------
// CLogsBaseReader::IsInterrupted
// ----------------------------------------------------------------------------
//
TBool CLogsBaseReader::IsInterrupted() const
    {
    return iInterrupted;
    }


// ----------------------------------------------------------------------------
// CLogsBaseReader::Interrupt
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::Interrupt()
    {
    if( iPhase != EInitial && iPhase != EDone )    
        {
        iInterrupted = ETrue;
        iState = EStateInterrupted;
        }
    else if( iPhase == EDone )
        {
        iState = EStateInterrupted;
        iPhase = EFilter;
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::State
// ----------------------------------------------------------------------------
//
TLogsState CLogsBaseReader::State() const
    {
    return iState;
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::NextL
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::NextL()
    {
    if( ! iInterrupted )
        {
        if( DoNextL() ) //Implemented in  derived reader
            {
            iState = EStateActive;
            SetActive();
            }
        else
            {
            iState = EStateFinished;
            ReadingFinishedL();
            if( iObserver )
                {
                iObserver->StateChangedL( this );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::SetObserver
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::SetObserver( MLogsObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::ContinueL
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::ContinueL()
    {
    iInterrupted = EFalse;
    
    if( ! IsActive() )
        {
        NextL();
        }
    }


// ----------------------------------------------------------------------------
// CLogsBaseReader::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::DoCancel()   //From CActive
    {
    iPhase = EInitial;
    iState = EStateUndefined;
    iLogClientRef->Cancel();  
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::RunL
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::RunL()
    {
    
    if( iStatus != KErrNone )   
        {
        iState = EStateError;
        if( iObserver )
            {
            iObserver->StateChangedL( this ); 
            }
        }
    else
        {
        switch( iPhase )
            {
            case EFilter:
                if( ViewCountL() == 0 )
                    {
                    iEventArray.Reset();
                    iPhase = EDone;
                    }
                break;

            case ERead:
                {
                const CLogEvent& sourceEvent = Event();

                //If operator notifies voicemail using proprietary CPHS-message there is no number.
                //Sms stack is logging it using number="CPHS" so we need to skip those entries (NSIA-6SP9GC).
                _LIT(kCphs, "CPHS");
                const TDesC& nbr = sourceEvent.Number();
                
                if( nbr == kCphs ) 
                    {
                    break;
                    }

                if( iEventArray.Count() <= iIndex )     //Append new entry to event array
                    { 
                    // create new event 
                    CLogsEvent* event = CLogsEvent::NewLC(); 
                    // Event() reads current event from Logs database
                    ConstructEventL( *event, sourceEvent ); //dest, source
                    // Append to end of array, when reading array first time
                    iEventArray.AppendL( *event );      //Ownership transferred
                    CleanupStack::Pop( event );    
                    }
                else                                    //Rewrite new entry on top of an old entry in event array
                    {
                    //Event() reads current event from Logs database
                    ConstructEventL( ( iEventArray.At( iIndex ) ), sourceEvent );  //dest, source                    
                    }

                // read duplicate counts if missed list and we're in phase of reading of the counts too.
                if( iModelId == ELogsMissedModel &&
                    iReadMissedDuplicateCounts )
                    {
                    iPhase = EDuplicate; 
                    }
                else
                    {
                    ++iIndex;
                    }

                if( iIndex >= ViewCountL() )
                    {
                    iPhase = EDone;
                    }            
                break;
                }

            case EDuplicate:

                iPhase = ERead;

                iEventArray.At( iIndex ).SetDuplicates(                     //Retrieve duplicate count just retrieved in 
                            static_cast<TInt8>( DuplicateCountL() + 1 ) );  //in call of NextL() 
                ++iIndex;

                if( iIndex >= ViewCountL() )
                    {
                    iPhase = EDone;
                    }
                break;

            case EInitial: // flow-through
            case EDone:
            default:
                break;
            }   //end switch

        if( iPhase == EDone )
            {
            //All events read
            if( iModelId == ELogsMissedModel && !iReadMissedDuplicateCounts )  
                {
                //Finish first phase of reading missed calls, 
                iReadMissedDuplicateCounts = ETrue;
                iState = EStateSemiFinished; //in order avoid scrollbar flicker etc in UI when StateChangedL is called                
                
                if( iObserver )
                    {
                    iObserver->StateChangedL( this ); 
                    }
                //Then immediately continue rereading missed calls now including the missed counts too
                StartL(); 
                }
            else
                {
                //Reading finished
                ReadingFinishedL();    
                iState = EStateFinished; //Inform UI etc all reading is now done

                if( iObserver )
                    {
                    iObserver->StateChangedL( this );
                    }
                }
            }
        else
            {
            //Continue to next event
            if( iObserver )
                {
                iObserver->StateChangedL( this );
                }
            NextL();
            }
        }   //endif
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::RunError
// ----------------------------------------------------------------------------
//
TInt CLogsBaseReader::RunError(TInt aError)
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

// ----------------------------------------------------------------------------
// CLogsBaseReader::ReadingFinishedL
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::ReadingFinishedL()
    {
    CheckArrayLengthL();
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::CheckArrayLengthL
//
// If there are old items in the end of iEventArray that are not part of what was just read from db,
// get rid of those. This is called when data is (re)read from db.
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::CheckArrayLengthL()
    {
    TInt count( ViewCountL() );   //Item count in Log db view
    
    for( TInt i = iEventArray.Count(); i > count; i-- )
        {
        iEventArray.Delete( i - 1 );
        }
        
    iEventArray.Compress();
    }


// ----------------------------------------------------------------------------
// CLogsBaseReader::DuplicateCountL
// ----------------------------------------------------------------------------
//
TInt CLogsBaseReader::DuplicateCountL() const
    {
    return 0;
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::HandleLogViewChangeEventAddedL
//
// Called by Log Database engine when it notifies us that it has added an event to database
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::HandleLogViewChangeEventAddedL( 
    TLogId /*aId*/, 
    TInt /*aViewIndex*/, 
    TInt /*aChangeIndex*/, 
    TInt aTotalChangeCount )
    {
    //Comment: HandleLogViewChangeEventAddedL seems to be called only once (and aTotalChangeCount is 1) even if 
    //there are multiple entries added to database in a batch. This seems to happen at least in wins emulator in Symbian 80a_200432.
    //If problems in this area or changed behaviour, we need to consider same kind of optimization to here as is in 
    //HandleLogViewChangeEventDeletedL

    iDeleteChangeCount = KDeleteCountInit;   //-1   
    HandleViewChangeL( aTotalChangeCount );
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::HandleLogViewChangeEventChangedL
//
// Called by Log Database engine when it notifies us that it has changed an event in the database
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::HandleLogViewChangeEventChangedL( 
    TLogId /*aId*/, 
    TInt /*aViewIndex*/, 
    TInt /*aChangeIndex*/, 
    TInt aTotalChangeCount )
    {
    iDeleteChangeCount = KDeleteCountInit;  //-1
    HandleViewChangeL( aTotalChangeCount );
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::HandleLogViewChangeEventDeletedL
//
// Called by Log Database engine when it notifies us that it has deleted an event in the database
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::HandleLogViewChangeEventDeletedL( 
    TLogId /*aId*/, 
    TInt /*aViewIndex*/, 
    TInt /*aChangeIndex*/, 
    TInt aTotalChangeCount )
    {
    //In order to prevent to re-reading the database multiple times, we call HandleViewChangeL only once. This is
    //because HandleLogViewChangeEventDeletedL is called as many times as is the number of entries are deleted e.g. when 
    //deleteting old entries from database happens. However, aTotalChangeCount contains total number of deletions in
    //a batch, so we can optimize the call to HandleViewChangeL to happen only even if we're called multiple times.
    
    //Note that CLogsEngine::StateChangedL is called by Logs engine when recent list entry has been deleted from db (it starts
    //rereading of view data from db)

    if( aTotalChangeCount == 1 || ( aTotalChangeCount != iDeleteChangeCount ) )
        {
        iDeleteChangeCount = aTotalChangeCount;
        HandleViewChangeL( aTotalChangeCount );  
        }  
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::HandleViewChangeL
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::HandleViewChangeL( TInt aTotalChangeCount )
    {
    if( aTotalChangeCount > 0 )
        {
        iDirty = ETrue;
        }

    if( iActivated  && iDirty )
        {
        StartL();                
        } 
    }


// ----------------------------------------------------------------------------
// CLogsBaseReader::ActivateL
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::ActivateL()  //Overridden method in derived classes may leave, that's why
    {                              // name of this function indicates that this may leave.
    iActivated = ETrue;
    iReadMissedDuplicateCounts = EFalse;//ETrue: Read missed calls including duplicate counts.    
                                        //EFalse: Read missed calls view twice, first without duplicate counts,
                                        //then again including duplicate counts (EMSH-6JDFBV).
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::DeActivate
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::DeActivate( )
    {
    iActivated = EFalse;
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::IsDirty
// ----------------------------------------------------------------------------
//
TBool CLogsBaseReader::IsDirty() const
    {
    return iDirty;
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::SetDirty
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::SetDirty()
    {
    iDirty = ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsBaseReader::BaseConstructEventL
// ----------------------------------------------------------------------------
//
void CLogsBaseReader::BaseConstructEventL( 
    MLogsEvent& aDest, 
    const CLogEvent& aSource )
    {
    aDest.InitializeEventL( aSource, iStrings, iModelId );
    }

