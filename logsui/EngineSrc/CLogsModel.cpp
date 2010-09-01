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
*     Produces objects that implement MLogsModel
*
*/


// INCLUDE FILES
#include "CLogsModel.h"
#include "MLogsReader.h"
#include "MLogsEvent.h"
#include "MLogsStateHolder.h"
#include "CLogsEventGetter.h"
#include "CLogsReaderFactory.h"
#include "CLogsGetEventFactory.h"
#include "MLogsEventArray.h"
// CONSTANTS

// ----------------------------------------------------------------------------
// CLogsModel::NewL
// ----------------------------------------------------------------------------
//
CLogsModel* CLogsModel::NewL( 
    RFs& aFsSession,
    TLogsModel aModel,
    TLogsEventStrings& aStrings,
    CLogsEngine* aLogsEngineRef )
     
    {
    CLogsModel* self = new (ELeave) CLogsModel( aFsSession, aModel, aStrings, aLogsEngineRef );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsModel::CLogsModel
// ----------------------------------------------------------------------------
//
CLogsModel::CLogsModel( 
    RFs& aFsSession,
    TLogsModel aModel,
    TLogsEventStrings& aStrings,
    CLogsEngine* aLogsEngineRef ) :
        iFsSession( aFsSession ),
        iModel( aModel ), 
        iStrings( aStrings ),
        iState( EStateUndefined ),
        iLogsEngineRef( aLogsEngineRef )     
    {
    }

// ----------------------------------------------------------------------------
// CLogsModel::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsModel::ConstructL()
    {
    iEventArray = CLogsGetEventFactory::LogsEventArrayL( iModel, 20 );
    iWrapper = CLogsEventGetter::NewL();    
    }

// ----------------------------------------------------------------------------
// CLogsModel::~CLogsModel
// ----------------------------------------------------------------------------
//
CLogsModel::~CLogsModel()
    {
    if( iEventArray )
        {
        iEventArray->Reset();
        }

    delete iEventArray;
    delete iWrapper;
    delete iReader;
    }

// ----------------------------------------------------------------------------
// CLogsModel::Delete
// ----------------------------------------------------------------------------
//
void CLogsModel::Delete( TInt aIndex )
	{
	iEventArray->Delete( aIndex );
	}

// ----------------------------------------------------------------------------
// CLogsModel::Count
// ----------------------------------------------------------------------------
//
TInt CLogsModel::Count()
    {
    return iEventArray->Count();
    }

// ----------------------------------------------------------------------------
// CLogsModel::KickStartL
// ----------------------------------------------------------------------------
//
void CLogsModel::KickStartL( TBool aReset )
    {
    if( iObserver && iEventArray && aReset )              
        {
        iState = EStateInitializing; //This informs observers that e.g. a complete re-read of recent events is about to begin
                                     // so that they can e.g. change their view focus settings accordingly
                                     // (e.g. CLogsRecentListView::StateChangedL ).
        iEventArray->Reset();        //Delete the old rows as it might take too long to read the new correct contents
        iObserver->StateChangedL( this );
        }
    
    if( iReader )
        {
        // call ActivateL first, cause KickStartL is called directly from CLogsEngine too
        iReader->ActivateL();  
        iReader->StartL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsModel::At
// ----------------------------------------------------------------------------
//
const MLogsEventGetter* CLogsModel::At( TInt aIndex )
    {
    if( aIndex < 0 || aIndex >= Count() )
        {
        iWrapper->Wrap( NULL );
        return NULL;
        }
        
    return iWrapper->Wrap(&iEventArray->At( aIndex ) );  //Log events in iEventArray 
    }

// ----------------------------------------------------------------------------
// CLogsModel::SetObserver
// ----------------------------------------------------------------------------
//
void CLogsModel::SetObserver( MLogsObserver* aObserver )
    {
    iObserver = aObserver;
    }

//This function can be called several times without calling DoDeactivate between calls
// ----------------------------------------------------------------------------
// CLogsModel::DoActivateL
// ----------------------------------------------------------------------------
//
void CLogsModel::DoActivateL( TDoActivate aDoActivate )
    {
    if( !iReader )
        {
        TRAPD( err, iReader = CLogsReaderFactory::LogsReaderL( iFsSession, 
                                        *iEventArray, iStrings, iModel, this, iLogsEngineRef ) );
		if( err == KErrAccessDenied )
			{
			delete iReader;
			iReader = NULL;
			return;
			}
        User::LeaveIfError(err);
        }
 
    iReader->ActivateL();  //This needs to be done before any other iReader call

    //Update list only if refresh requested, otherwise leave as it is
    if( aDoActivate != EActivateOnly )
        {
        if( iReader->IsDirty() )
            {
            //ETrue: reset list before reading
            KickStartL( aDoActivate == EResetAndRefresh ? ETrue : EFalse );
            }
        else if( iReader->IsInterrupted() )
            {
            iReader->ContinueL();
            }
        }        
    }

// ----------------------------------------------------------------------------
// CLogsModel::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLogsModel::DoDeactivate( 
    TClearMissedCalls aClearMissedCalls,
    TDoDeactivate aDisconnectDb )
    {
    if( iModel == ELogsMissedModel && 
        aClearMissedCalls == ENormalOperation ) 
        {
        if( iReader ) 
            {
            TInt count(iEventArray->Count());
            for(TInt i = 0; i < count; i++)
                {
                iEventArray->At(i).SetDuplicates(0);
                }
            //We need to clear duplicates from the db too
            TInt err;        
            TRAP( err, iReader->ClearDuplicatesL() );
            }
        }

    /**************************************************************************************************        
     * This did not help enough for EMSH-6JDFBV, therefore original below still in use.
    //if( aDisconnectDb ) //Needs to be done always, otherwise happened a bit too late when MT call is received.
        {
        //iReader->Stop(); //This seems to be not enough for EMSH-6JDFBV, two rows below work a bit better instead
        iReader->Interrupt();   
        iReader->DeActivate( ); 
        }
    **************************************************************************************************/

    //For main model we cannot delete the reader, otherwise the use must wait long for the lines to show up again 
    //in the event view. Also we cannot update the view in CLogsEventListView::StateChangedL too frequently when the 
    //model is being reread. 
    if( iModel == ELogsMainModel )
        {
        if( iReader )
            {
            iReader->DeActivate();
            iReader->Interrupt();
            }
        }
    else
        {
        if( aDisconnectDb == ECloseDBConnectionAndResetArray ||
            aDisconnectDb == ECloseDBConnection )
            {
            //The rows below stop effectively cpu and I/O consumption in view of EMSH-6JDFBV. However, 
            //they also kill user's perceived UI responsiviness so aDisconnectDb should be used sparingly. 
            //Because reader may be deleted when reading of db is ongoing, we need also explicitly inform 
            //observer that we are finished.
            delete iReader; 
            iReader = NULL; 
            iState = EStateReaderDeletedOrStopped; 
            
            if (aDisconnectDb == ECloseDBConnectionAndResetArray)
                {
                iState = EStateArrayReseted; 
                iEventArray->Reset();  
                }  
            
            if( iObserver )
                {
                TInt err;        
                TRAP( err, iObserver->StateChangedL( this ) );
                }
            }
        // Special case, just reset the array. This is needed to avoid flicker of 
        // old list since reread of events is done when regaining foreground
        else if (aDisconnectDb == EResetOnlyArray || aDisconnectDb == EResetOnlyArrayWithDirty )
            {
            iState = EStateArrayReseted;
            iEventArray->Reset(); 
            
            if ( aDisconnectDb == EResetOnlyArrayWithDirty )
                {
                if( iReader )
                    {
                    iReader->SetDirty();
                    }              
                }
            
            if( iObserver )
                {
                TInt err;        
                TRAP( err, iObserver->StateChangedL( this ) );
                }
            }
       
        }
    }

// ----------------------------------------------------------------------------
// CLogsModel::Reset
// ----------------------------------------------------------------------------
//
void CLogsModel::Reset()
    {
    iWrapper->Wrap( NULL );
    }

// ----------------------------------------------------------------------------
// CLogsModel::State
// ----------------------------------------------------------------------------
//
TLogsState CLogsModel::State() const
    {
    return iState;
    }

// ----------------------------------------------------------------------------
// CLogsModel::ConfigureL
// ----------------------------------------------------------------------------
//
void CLogsModel::ConfigureL( const MLogsReaderConfig* aConfig )
    {
    if( iReader )
        {
        iReader->Stop();
        iReader->ConfigureL( aConfig );
        KickStartL( ETrue );
        }
    } 

// ----------------------------------------------------------------------------
// CLogsModel::StateChangedL
// ----------------------------------------------------------------------------
//
void CLogsModel::StateChangedL( MLogsStateHolder* aHolder )
    {
    if( aHolder->State() == EStateClearLogFinished )
        {
        KickStartL( ETrue );
        }

/*
    if( aHolder->State() == EStateFreeToDelete )
        {
        delete iReader;
        iReader = NULL;
        }
    else
*/    
        {
        iState = aHolder->State();
        if( iObserver )
            {
            iObserver->StateChangedL( this );
            }
        }
    }


