/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The remote sim view implementation
*  Version     : %version: be1neux1#21.1.4 %
*
*/



// INCLUDE FILES
#include "CRemoteView.h"

// From Virtual Phonebook
#include "CRemoteStore.h"
#include "VPbkSimStoreError.h"
#include <MVPbkSimViewObserver.h>
#include <VPbkSimServerOpCodes.h>
#include <CVPbkSimContactBuf.h>
#include <CVPbkAsyncOperation.h>
#include <CVPbkAsyncCallback.h>
#include <CVPbkSimFieldTypeFilter.h>
#include <MVPbkSimViewFindObserver.h>
#include <VPbkSimStoreTemplateFunctions.h>

#include <s32mem.h>

namespace VPbkSimStore {

inline void SendEvent(
    RPointerArray<MVPbkSimViewObserver>& aObservers,
    void ( MVPbkSimViewObserver::*aEvent )( MVPbkSimCntView& ),
    CRemoteView& aThis )
    {
    for ( TInt i = aObservers.Count() - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aEvent )( aThis );
        }    
    }
    
inline void SendEvent(
    RPointerArray<MVPbkSimViewObserver>& aObservers,
    void ( MVPbkSimViewObserver::*aEvent )( MVPbkSimCntView&, TInt ),
    CRemoteView& aThis,
    TInt aParam )
    {
    for ( TInt i = aObservers.Count() - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aEvent )( aThis, aParam );
        }    
    }
    
inline void SendEvent(
    RPointerArray<MVPbkSimViewObserver>& aObservers,
    void ( MVPbkSimViewObserver::*aEvent )
        ( MVPbkSimViewObserver::TEvent, TInt, TInt ),
    MVPbkSimViewObserver::TEvent aEventType,
    TInt aParam1,
    TInt aParam2 )
    {
    for ( TInt i = aObservers.Count() - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aEvent )( aEventType, aParam1, aParam2 );
        }    
    }


// ============================ MEMBER FUNCTIONS ============================
// --------------------------------------------------------------------------
// CRemoteView::CViewFindOperation::CViewFindOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CRemoteView::CViewFindOperation::CViewFindOperation( 
	    const MDesCArray& aFindStrings,             
        MVPbkSimViewFindObserver& aObserver,
        RVPbkSimCntView& aSimCntViewSession,
        MVPbkSimCntView& aSimCntView ):
    CActive( EPriorityStandard ),
    iFindStrings( aFindStrings ),
    iObserver( aObserver ),
    iSimCntViewSession( aSimCntViewSession ),
    iSimCntView(aSimCntView),
    iSimMatchResultBufPtr( NULL, 0 )
    {
    }

// --------------------------------------------------------------------------
// CRemoteView::CViewFindOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CRemoteView::CViewFindOperation::ConstructL()
    {
    CActiveScheduler::Add( this );           
    }

// --------------------------------------------------------------------------
// CRemoteView::CViewFindOperation::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CRemoteView::CViewFindOperation* CRemoteView::CViewFindOperation::NewL( 
        const MDesCArray& aFindStrings, 
        MVPbkSimViewFindObserver& aObserver,
        RVPbkSimCntView& aSimCntViewSession,
        MVPbkSimCntView& aSimCntView )
    {
    CViewFindOperation* self = 
        new( ELeave ) CViewFindOperation( aFindStrings, aObserver, 
                                          aSimCntViewSession, aSimCntView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CRemoteView::CViewFindOperation::~CViewFindOperation()
    {
    Cancel();    
    delete iSimMatchResultBuf;
    }

// --------------------------------------------------------------------------
// CRemoteView::CViewFindOperation::ActivateL
// --------------------------------------------------------------------------
//
void CRemoteView::CViewFindOperation::ActivateL()
    {
    SetActive();
    iSimCntViewSession.ContactMatchingPrefixL( iFindStrings, 
                                               iResultBufferSize, 
                                               iStatus );    
    }

// --------------------------------------------------------------------------
// CRemoteView::CViewFindOperation::RunL
// --------------------------------------------------------------------------
//
void CRemoteView::CViewFindOperation::RunL()
    {
    RVPbkStreamedIntArray matchedIndexes;
    CleanupClosePushL( matchedIndexes );

    TInt result = iStatus.Int();
    switch ( result )
        {
        case KErrNone:
            {    
            //Delete old buffer        
            if ( iSimMatchResultBuf )
                {
                delete iSimMatchResultBuf;
                iSimMatchResultBuf = NULL;
                }
            
            //Create buffer
            iSimMatchResultBuf = HBufC8::NewL( iResultBufferSize );
            iSimMatchResultBufPtr.Set( iSimMatchResultBuf->Des() );                
              
            //Get contact matching results
            iSimCntViewSession.ContactMatchingResultL( iSimMatchResultBufPtr );
                            
            //Unpack results
            RDesReadStream stream( iSimMatchResultBufPtr );
            CleanupClosePushL( stream );
            matchedIndexes.InternalizeL( stream );
            CleanupStack::PopAndDestroy(); // stream
                        
            iObserver.ViewFindCompleted( iSimCntView, matchedIndexes );            
            break;
            }            
        default:
            {
            iObserver.ViewFindError( iSimCntView, result );
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // matchedIndexes
    }
  
// --------------------------------------------------------------------------
// CRemoteView::CViewFindOperation::DoCancel
// --------------------------------------------------------------------------
//
void CRemoteView::CViewFindOperation::DoCancel()
    {
    iSimCntViewSession.CancelAsyncRequest( EVPbkSimSrvContactMatchingPrefix );
    }

// --------------------------------------------------------------------------
// CRemoteView::CViewFindOperation::RunError
// --------------------------------------------------------------------------
//
TInt CRemoteView::CViewFindOperation::RunError( TInt aError )
    {
    iObserver.ViewFindError( iSimCntView, aError );
    return KErrNone;
    }    

// --------------------------------------------------------------------------
// CRemoteView::CRemoteView
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CRemoteView::CRemoteView( CRemoteStore& aStore,
        TVPbkSimViewConstructionPolicy aConstructionPolicy,
        CVPbkSimFieldTypeFilter* aFilter ) :
            CActive( EPriorityStandard ),
            iStore( aStore ),
            iConstructionPolicy( aConstructionPolicy ),
            iFilter( aFilter )
    {
    }

// --------------------------------------------------------------------------
// CRemoteView::~CRemoteView
// Destructor
// --------------------------------------------------------------------------
// 
CRemoteView::~CRemoteView()
    {
    delete iObserverOp;
    iObservers.Reset();
    iSortOrder.Close();
    iStore.RemoveObserver( *this );
    Cancel();
    delete iCurrentContact;
    iSimView.Close();
    delete iViewName;
    delete iFilter;
    }

// --------------------------------------------------------------------------
// CRemoteView::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CRemoteView* CRemoteView::NewL( CRemoteStore& aStore,
        const RVPbkSimFieldTypeArray& aSortOrder, 
        TVPbkSimViewConstructionPolicy aConstructionPolicy,
        const TDesC& aViewName,
        CVPbkSimFieldTypeFilter* aFilter )
    {
    CRemoteView* self = 
        new( ELeave ) CRemoteView( aStore, aConstructionPolicy, aFilter );
    CleanupStack::PushL( self );
    self->ConstructL( aSortOrder, aViewName );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CRemoteView::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CRemoteView::ConstructL( const RVPbkSimFieldTypeArray& aSortOrder,
    const TDesC& aViewName )
    {
    CActiveScheduler::Add( this );
    SetSortOrderL( aSortOrder );
    iViewName = aViewName.AllocL();
    iCurrentContact = CVPbkSimContactBuf::NewL( iStore );
    iStore.AddObserverL( *this );
    iObserverOp = CVPbkAsyncObjectOperation<MVPbkSimViewObserver>::NewL();
    }


// --------------------------------------------------------------------------
// CRemoteView::RunL
// --------------------------------------------------------------------------
//
void CRemoteView::RunL()
    {
    if ( iStatus == KErrNone )
        {
        switch ( iEventData.iEvent )
            {   
            case EVPbkSimViewOpen:
                {
                SendEvent(iObservers, &MVPbkSimViewObserver::ViewReady, *this);
                break;
                }
            case EVPbkSimViewNotAvailable:
                {
                SendEvent(iObservers, &MVPbkSimViewObserver::ViewNotAvailable, *this);
                break;
                }
            case EVPbkSimContactAdded:
                {
                if(	iEventData.iData != KErrNotFound )
                	{
                    SendEvent(iObservers, &MVPbkSimViewObserver::ViewContactEvent, 
                              MVPbkSimViewObserver::EContactAdded, 
                              iEventData.iData, iEventData.iOpData ); 
                	}
                break;                
                }
            case EVPbkSimContactDeleted:
                {
                SendEvent(iObservers, &MVPbkSimViewObserver::ViewContactEvent, 
                          MVPbkSimViewObserver::EContactDeleted, 
                          iEventData.iData, iEventData.iOpData );
                break;
                }
            case EVPbkSimViewError:
                {
                SendEvent(iObservers, &MVPbkSimViewObserver::ViewError, 
                          *this, iEventData.iOpData );
                break;
                }
            case EVPbkSimUnknown: // FALLTHROUGH
            default:
                {
                __ASSERT_DEBUG( EFalse, VPbkSimStore::Panic( 
                    VPbkSimStore::EUnknownViewEventFromServer ) );
                SendEvent(iObservers, &MVPbkSimViewObserver::ViewError, 
                          *this, KErrUnknown );
                break;
                }
            }
        }
    else
        {
        SendEvent(iObservers, &MVPbkSimViewObserver::ViewError, 
                  *this, iStatus.Int() );
        }

    ActivateViewNotification();
    }

// --------------------------------------------------------------------------
// CRemoteView::DoCancel
// --------------------------------------------------------------------------
//
void CRemoteView::DoCancel()
    {
    iSimView.CancelAsyncRequest( EVPbkSimSrvViewEventNotification );
    }

// --------------------------------------------------------------------------
// CRemoteView::Name
// --------------------------------------------------------------------------
//
const TDesC& CRemoteView::Name() const
    {
    return *iViewName;
    }
    
// --------------------------------------------------------------------------
// CRemoteView::ParentStore
// --------------------------------------------------------------------------
//
MVPbkSimCntStore& CRemoteView::ParentStore() const
    {
    return iStore;
    }

// --------------------------------------------------------------------------
// CRemoteView::OpenL
// --------------------------------------------------------------------------
//
void CRemoteView::OpenL( MVPbkSimViewObserver& aObserver )
    {
    TInt index = iObservers.Find(&aObserver);
    __ASSERT_DEBUG( index == KErrNotFound, 
        Panic( ERemoteViewObserverAlreadySet ) );
    iObservers.AppendL(&aObserver);
    
    // Open view subsession only if the store has been opened
    // This is actually called only from CContactView::ConstructL so
    // it's called only once -> not need to make else-clause for async
    if ( iStore.IsOpen() && !iSimView.SubSessionHandle())
        {
        iSimView.OpenL( iStore.StoreSession(), iSortOrder, iConstructionPolicy, 
            *iViewName, iFilter );
        ActivateViewNotification();
        }
    else
        {
        // Complete with "View unavailable" event
        CVPbkAsyncObjectCallback<MVPbkSimViewObserver>* callback =
            VPbkEngUtils::CreateAsyncObjectCallbackLC(
                *this, 
                &CRemoteView::DoViewUnavailableL,
                &CRemoteView::DoViewUnavailableError,
                aObserver );
        
        iObserverOp->CallbackL( callback );
        CleanupStack::Pop( callback ); // openCallback
        }
    }

// --------------------------------------------------------------------------
// CRemoteView::Close
// --------------------------------------------------------------------------
//
void CRemoteView::Close( MVPbkSimViewObserver& aObserver )
    {
    // Cancel the possible call back
    iObserverOp->CancelCallback( &aObserver );
    
    // Remove from the observer list
    TInt index = iObservers.Find(&aObserver);
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    
    // If it was the last user then close the connection to the server.
    if (iObservers.Count() == 0)
        {
        Cancel();
        iSimView.Close();
        }
    }
        
// --------------------------------------------------------------------------
// CRemoteView::CountL
// --------------------------------------------------------------------------
// 
TInt CRemoteView::CountL() const
    {
    return iSimView.CountL();
    }

// --------------------------------------------------------------------------
// CRemoteView::ContactAtL
// --------------------------------------------------------------------------
// 
MVPbkSimContact& CRemoteView::ContactAtL( TInt aIndex )
    {
    iCurrentContact->SetL( iSimView.ContactAtL( aIndex ) );
    return *iCurrentContact;
    }

// --------------------------------------------------------------------------
// CRemoteView::ChangeSortOrderL
// --------------------------------------------------------------------------
// 
void CRemoteView::ChangeSortOrderL( const RVPbkSimFieldTypeArray& aSortOrder )
    {
    if ( iSimView.SubSessionHandle() ) 
        {
        SetSortOrderL( aSortOrder );
        iSimView.ChangeSortOrderL( aSortOrder );        
        }
    }

// --------------------------------------------------------------------------
// CRemoteView::MapSimIndexToViewIndexL
// --------------------------------------------------------------------------
// 
TInt CRemoteView::MapSimIndexToViewIndexL( TInt aSimIndex )
    {
    return iSimView.FindViewIndexL( aSimIndex );
    }

// --------------------------------------------------------------------------
// CRemoteView::ContactMatchingPrefixL
// --------------------------------------------------------------------------
//     
MVPbkSimStoreOperation* CRemoteView::ContactMatchingPrefixL(
        const MDesCArray& aFindStrings, 
        MVPbkSimViewFindObserver& aObserver )
    {    
    CViewFindOperation* operation = CViewFindOperation::NewL(
        aFindStrings, aObserver, iSimView, *this );
    operation->ActivateL();
    return operation;
    }

// --------------------------------------------------------------------------
// CRemoteView::SortOrderL
// --------------------------------------------------------------------------
//
const RVPbkSimFieldTypeArray& CRemoteView::SortOrderL() const
    {
    HBufC8* sortOrder = iSimView.SortOrderL();
    CleanupStack::PushL( sortOrder );
    RDesReadStream readStream( *sortOrder );
    readStream.PushL();
    iSortOrder.Reset();
    readStream >> iSortOrder;
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( sortOrder );
    return iSortOrder;
    }
    
// --------------------------------------------------------------------------
// CRemoteView::MapSimIndexToViewIndexL
// --------------------------------------------------------------------------
//
void CRemoteView::StoreReady( MVPbkSimCntStore& /*aStore*/ )
    {
    if ( !iSimView.SubSessionHandle() )
        {
        TRAPD( result, iSimView.OpenL( iStore.StoreSession(), iSortOrder, 
            iConstructionPolicy, *iViewName, iFilter ) );
        if (result != KErrNone)
            {
            SendEvent(iObservers, &MVPbkSimViewObserver::ViewError, 
                      *this, result );
            }
        ActivateViewNotification();
        // Cancel any ongoing "View unavailable" events
        iObserverOp->Purge();
        }
    }

// --------------------------------------------------------------------------
// CRemoteView::StoreError
// --------------------------------------------------------------------------
//
void CRemoteView::StoreError( MVPbkSimCntStore& /*aStore*/, TInt aError )
    {
    // Cancel any ongoing view events
    iObserverOp->Purge();
    SendEvent(iObservers, &MVPbkSimViewObserver::ViewError, 
              *this, aError );
    }

// --------------------------------------------------------------------------
// CRemoteView::StoreNotAvailable
// --------------------------------------------------------------------------
//
void CRemoteView::StoreNotAvailable( MVPbkSimCntStore& /*aStore*/ )
    {
    // Cancel any ongoing view events
    iObserverOp->Purge();
    SendEvent(iObservers, &MVPbkSimViewObserver::ViewNotAvailable, *this);
    }

// --------------------------------------------------------------------------
// CRemoteView::StoreNotAvailable
// --------------------------------------------------------------------------
//
void CRemoteView::StoreContactEvent( TEvent /*aEvent*/, TInt /*aSimIndex*/ )
    {
    
    }

// --------------------------------------------------------------------------
// CRemoteView::ActivateViewNotification
// --------------------------------------------------------------------------
// 
void CRemoteView::ActivateViewNotification()
    {
    if ( !IsActive() && iSimView.SubSessionHandle() )
        {
        iSimView.ListenToViewEvents( iStatus, iEventData );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CRemoteView::SetSortOrderL
// --------------------------------------------------------------------------
// 
void CRemoteView::SetSortOrderL( const RVPbkSimFieldTypeArray& aSortOrder )
    {
    iSortOrder.Reset();
    const TInt count = aSortOrder.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iSortOrder.AppendL( aSortOrder[i] );
        }
    }

// --------------------------------------------------------------------------
// CRemoteView::IssueRequest
// --------------------------------------------------------------------------
// 
void CRemoteView::IssueRequest( TInt aError )
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aError);
    SetActive();
    }

// --------------------------------------------------------------------------
// CRemoteView::DoViewUnavailableL
// --------------------------------------------------------------------------
// 
void CRemoteView::DoViewUnavailableL( MVPbkSimViewObserver& aObserver )
    {
    aObserver.ViewNotAvailable( *this );
    }

// --------------------------------------------------------------------------
// CRemoteView::DoViewUnavailableError
// --------------------------------------------------------------------------
// 
void CRemoteView::DoViewUnavailableError( MVPbkSimViewObserver& aObserver, 
        TInt aError )
    {
    aObserver.ViewError( *this, aError );
    }
} // namespace VPbkSimStore

//  End of File  
