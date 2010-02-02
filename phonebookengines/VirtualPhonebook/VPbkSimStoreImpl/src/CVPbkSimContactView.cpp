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
* Description:  The contact view
*
*/



// INCLUDE FILES
#include <CVPbkSimContactView.h>

#include "CStoreBase.h"
#include "CContactArray.h"
#include "CSimCntSortUtil.h"
#include "CContactArray.h"
#include "VPbkSimStoreImplError.h"

#include <CVPbkSimContactBuf.h>
#include <CVPbkSimContact.h>
#include <CVPbkSimCntField.h>
#include <CVPbkAsyncOperation.h>
#include <CVPbkAsyncCallback.h>
#include <CVPbkSimFieldTypeFilter.h>
#include <CVPbkContactFindPolicy.h>
#include <MVPbkSimViewObserver.h>
#include <MVPbkSimViewFindObserver.h>
#include <MVPbkSimStoreOperation.h>
#include <RVPbkStreamedIntArray.h>
#include <TVPbkSimStoreProperty.h>
#include <VPbkSimStoreTemplateFunctions.h>
#include <VPbkDebug.h>

// CONSTANTS

enum TVPbkSimViewState
    {
    /// View has no clients. It doesn' listen to store/contact array events
    ENotReady,
    /// View has clients but the store is not available or the view is resorting.
    /// It listens to store events but not contact array events.
    EUnvavailable,
    /// View has clients and it's ready to use.
    EReady
    };

namespace VPbkSimStoreImpl {

/**
* An active object for contact matching
*/
NONSHARABLE_CLASS(CContactMatchingOperation) : 
        public CActive,
        public MVPbkSimStoreOperation
    {
    public: // Construction and destruction
        static CContactMatchingOperation* NewL( 
            const MDesCArray& aFindStrings, 
            MVPbkSimViewFindObserver& aObserver,
            MVPbkSimCntView& aSimCntView );
        ~CContactMatchingOperation();

    public: // New functions
        void Activate();

    public: // Functions from CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    private:
        /**
        * C++ constructor
        */
        CContactMatchingOperation( 
            const MDesCArray& aFindStrings, 
            MVPbkSimViewFindObserver& aObserver,
            MVPbkSimCntView& aSimCntView );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Contact matched
        *
        * @param aSimContact SIM contact to be matched
        * @param aType SIM contact field to be matched with find string
        * @return ETrue, if contact matched, EFalse if not
        */
        TBool ContactMatchedL( 
            MVPbkSimContact& aSimContact,
            TVPbkSimCntFieldType aType );

    private:       
        /// Ref: strings used in find        
        const MDesCArray& iFindStrings;
        /// Ref: Observer that is interested in the result of the find
        MVPbkSimViewFindObserver& iObserver;
        /// Ref: View of SimStore to existing contacts
        MVPbkSimCntView& iSimCntView;
        /// Own: The result of the find
        RVPbkStreamedIntArray iMatchingResults;
        /// Own: Makes a string comparison
        CVPbkContactFindPolicy* iContactFindPolicy;
    };    

// ============================ MEMBER FUNCTIONS ============================
// --------------------------------------------------------------------------
// CContactMatchingOperation::CContactMatchingOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CContactMatchingOperation::CContactMatchingOperation( 
	    const MDesCArray& aFindStrings, 
        MVPbkSimViewFindObserver& aObserver,
        MVPbkSimCntView& aSimCntView )
        :   CActive( EPriorityStandard ),
            iFindStrings( aFindStrings ),
            iObserver( aObserver ),
            iSimCntView( aSimCntView )
    {
    }

// --------------------------------------------------------------------------
// CContactMatchingOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CContactMatchingOperation::ConstructL()
    {
    CActiveScheduler::Add( this );     
    
    //Create contact find policy
    iContactFindPolicy = CVPbkContactFindPolicy::NewL();       
    }

// --------------------------------------------------------------------------
// CVPbkSimContactViewFindOperation::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CContactMatchingOperation* CContactMatchingOperation::NewL( 
        const MDesCArray& aFindStrings, 
        MVPbkSimViewFindObserver& aObserver,
        MVPbkSimCntView& aSimCntView )
    {
    CContactMatchingOperation* self = 
        new( ELeave ) CContactMatchingOperation( aFindStrings, aObserver, 
                                                 aSimCntView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CContactMatchingOperation::~CContactMatchingOperation()
    {        
    delete iContactFindPolicy;
    iMatchingResults.Close();
    Cancel();
    // iContactFindPolicy is an ECOM plugin so call FinalClose after delete.
    REComSession::FinalClose();
    }

// --------------------------------------------------------------------------
// CContactMatchingOperation::Activate
// --------------------------------------------------------------------------
//
void CContactMatchingOperation::Activate()
    {        
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// --------------------------------------------------------------------------
// CContactMatchingOperation::ContactMatchedL
// --------------------------------------------------------------------------
//    
TBool CContactMatchingOperation::ContactMatchedL( 
        MVPbkSimContact& aSimContact,
        TVPbkSimCntFieldType aType )
    {
    TBool result( EFalse );    
    MVPbkSimContact::TFieldLookup lookup = 
            aSimContact.FindField( aType );            
                        
    if ( !lookup.EndOfLookup() )
        {                        
        const TDesC& data =                     
            aSimContact.ConstFieldAt( lookup.Index() ).Data();
                    
        const TInt findCount( iFindStrings.MdcaCount() );
        for ( TInt i(0); i < findCount; ++i )
            {
            // Match refine
            if ( iContactFindPolicy->MatchRefineL( 
                 data, iFindStrings.MdcaPoint( i ) ) )
                {
                iMatchingResults.AppendIntL( aSimContact.SimIndex() );
                result = ETrue;
                break;
                }              
            }                    
        }
    
    return result;
    }
    
// --------------------------------------------------------------------------
// CContactMatchingOperation::RunL
// --------------------------------------------------------------------------
//
void CContactMatchingOperation::RunL()
    {
	const TInt count( iSimCntView.CountL() );
	for ( TInt i(0); i < count; ++i )
		{        
		MVPbkSimContact& simContact = iSimCntView.ContactAtL( i );        
        MVPbkSimContact::TFieldLookup lookupSimName = simContact.FindField( EVPbkSimName );                                      
        //try to fetch contact with EVPbkSimName
        if ( !lookupSimName.EndOfLookup() )
        	{
            ContactMatchedL( simContact, EVPbkSimName ); 
			}
        //if contact without name, then try to fetch it with EVPbkSimGsmNumber
        else
            {
            MVPbkSimContact::TFieldLookup lookupGsmNum = simContact.FindField( EVPbkSimGsmNumber );
            if ( !lookupGsmNum.EndOfLookup() )
            	{
            	ContactMatchedL( simContact, EVPbkSimGsmNumber ); 
                }
            }                              
		}        
	iObserver.ViewFindCompleted( iSimCntView, iMatchingResults );
    }
    
// --------------------------------------------------------------------------
// CContactMatchingOperation::DoCancel
// --------------------------------------------------------------------------
//
void CContactMatchingOperation::DoCancel()
    {
    // Operation has no handles to any services -> do nothing
    }

// --------------------------------------------------------------------------
// CContactMatchingOperation::RunError
// --------------------------------------------------------------------------
//
TInt CContactMatchingOperation::RunError( TInt aError )
    {    
    iObserver.ViewFindError( iSimCntView, aError ); 
    return KErrNone;
    }    

} // namespace VPbkSimStoreImpl


// --------------------------------------------------------------------------
// CVPbkSimContactView::CVPbkSimContactView
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CVPbkSimContactView::CVPbkSimContactView( MVPbkSimCntStore& aParentStore, 
    TVPbkSimViewConstructionPolicy aConstructionPolicy ) 
    :   iParentStore( aParentStore ),
        iConstructionPolicy( aConstructionPolicy ),
        iViewState( ENotReady )
    {
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::~CVPbkSimContactView
// --------------------------------------------------------------------------
//
CVPbkSimContactView::~CVPbkSimContactView()
    {
    delete iIdleSort;
    SetToNotReadyState();
    delete iAsyncOperation;
    iObservers.Close();
    iSortOrder.Close();
    delete iViewName;
    delete iSortUtil;
    delete iCurrentContact;
    delete iTempContactForSorting;
    delete iFilter;
    }
    
// --------------------------------------------------------------------------
// CVPbkSimContactView::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkSimContactView* CVPbkSimContactView::NewLC( 
        const RVPbkSimFieldTypeArray& aSortOrder,
        TVPbkSimViewConstructionPolicy aConstructionPolicy, 
        MVPbkSimCntStore& aParentStore, const TDesC& aViewName,
        CVPbkSimFieldTypeFilter* aFilter )
    {
    CVPbkSimContactView* self = 
            new ( ELeave ) CVPbkSimContactView( aParentStore, 
                aConstructionPolicy  );
    CleanupStack::PushL( self );
    self->ConstructL( aSortOrder, aViewName );
    self->iFilter = aFilter;
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::ConstructL( const RVPbkSimFieldTypeArray& aSortOrder, 
    const TDesC& aViewName )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView(0x%x)::ConstructL name=%S"), 
        this, &aViewName );
    
    iIdleSort = CIdle::NewL( CActive::EPriorityStandard );
    ResetAndCopySortOrderL( aSortOrder );
    iViewName = aViewName.AllocL();
    iAsyncOperation = 
            CVPbkAsyncObjectOperation<MVPbkSimViewObserver>::NewL();
    iCurrentContact = CVPbkSimContactBuf::NewL( iParentStore );
    iTempContactForSorting = CVPbkSimContactBuf::NewL( iParentStore );
    }
    
// --------------------------------------------------------------------------
// CVPbkSimContactView::Name
// --------------------------------------------------------------------------
//
const TDesC& CVPbkSimContactView::Name() const
    {
    return *iViewName;
    }
    
// --------------------------------------------------------------------------
// CVPbkSimContactView::ParentStore
// --------------------------------------------------------------------------
//
MVPbkSimCntStore& CVPbkSimContactView::ParentStore() const
    {
    return iParentStore;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::OpenL
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::OpenL( MVPbkSimViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
    "VPbkSimStoreImpl: CVPbkSimContactView(0x%x)::OpenL name=%S,aObserver=0x%x,state=%d"),
    this, iViewName, &aObserver, iViewState );
    
    if ( ViewStateNotReady() )
        {
        // Call close first to ensure store is not opened many times
        iParentStore.Close( *this );
        // Open a store asynchronously.
        iParentStore.OpenL( *this );
        }
    else 
        {
        void( CVPbkSimContactView::* notifyFuncPtr)( MVPbkSimViewObserver& ) =
                &CVPbkSimContactView::DoViewOpenCallbackL;    
        if ( ViewStateUnavailable() )
            {
            notifyFuncPtr = &CVPbkSimContactView::DoViewUnavailableCallbackL;
            }
            
        CVPbkAsyncObjectCallback<MVPbkSimViewObserver>* callback =
            VPbkEngUtils::CreateAsyncObjectCallbackLC(
                *this, 
                notifyFuncPtr, 
                &CVPbkSimContactView::DoViewErrorCallback,
                aObserver );
        iAsyncOperation->CallbackL(callback);
        CleanupStack::Pop(); // callBack
        }
    
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::Close
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::Close( MVPbkSimViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::Close name=%S,aObserver=0x%x,state=%d"),
        iViewName, &aObserver, iViewState );
    
    iAsyncOperation->CancelCallback( &aObserver );
    
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0 )
        {
        iObservers.Remove( index );
        if ( iObservers.Count() == 0 )    
            {     
            SetToNotReadyState();
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::CountL
// --------------------------------------------------------------------------
//        
TInt CVPbkSimContactView::CountL() const
    {
    return iViewContacts.Count();
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::ContactAtL
// --------------------------------------------------------------------------
//        
MVPbkSimContact& CVPbkSimContactView::ContactAtL( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iViewContacts.Count() )
        {
        User::Leave( KErrArgument );
        }

    const MVPbkSimContact* fullContact = 
        iParentStore.ContactAtL( iViewContacts[aIndex].iSimIndex );
    
    // Check that view is up to date
    __ASSERT_DEBUG( fullContact, 
            Panic( VPbkSimStoreImpl::EViewArrayNotUpToDate ) );
    if ( !fullContact )
        {
        User::Leave( KErrNotFound );
        }
    
    CVPbkSimContact* cnt = 
        CVPbkSimContact::NewLC( iParentStore );
    // Copy fields that have same type as in the sortorder
    TInt count = iSortOrder.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        MVPbkSimContact::TFieldLookup lookup = 
            fullContact->FindField( iSortOrder[i] );
        if ( !lookup.EndOfLookup() )
            {
            CVPbkSimCntField* field = cnt->CreateFieldLC( iSortOrder[i] );
            field->SetDataL( fullContact->ConstFieldAt( lookup.Index() ).Data() );
            cnt->AddFieldL( field );
            CleanupStack::Pop( field );
            }
        }

    cnt->SetSimIndex( fullContact->SimIndex() );
    iCurrentContact->SetL( cnt->ETelContactL() );
    CleanupStack::PopAndDestroy( cnt );
    return *iCurrentContact;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::ChangeSortOrderL
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::ChangeSortOrderL( const RVPbkSimFieldTypeArray& aSortOrder )
    {
    // Copy the new sort order to the view
    ResetAndCopySortOrderL( aSortOrder );
    
    // Change state only if the view is currently ready, Otherwise
    // the view is already waiting sorting or store event.
    if ( ViewStateReady() )
        {
        // Change to unavailable: view will be ready after sorting.
        SetToUnavailableState();
        // Resort view.
        StartSorting();
        }
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::MapSimIndexToViewIndexL
// --------------------------------------------------------------------------
//
TInt CVPbkSimContactView::MapSimIndexToViewIndexL( TInt aSimIndex )
    {
    TViewContact cnt( aSimIndex );
    return iViewContacts.Find( cnt, 
            TIdentityRelation<TViewContact>( &CVPbkSimContactView::CompareViewContactSimIndex ) );
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::ContactMatchingPrefixL
// --------------------------------------------------------------------------
//
MVPbkSimStoreOperation* CVPbkSimContactView::ContactMatchingPrefixL(
        const MDesCArray& aFindStrings, 
        MVPbkSimViewFindObserver& aObserver )
    {
    VPbkSimStoreImpl::CContactMatchingOperation* operation = 
            VPbkSimStoreImpl::CContactMatchingOperation::NewL( aFindStrings, 
                aObserver, *this );
    operation->Activate();    
    return operation;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::SortOrderL
// --------------------------------------------------------------------------
//
const RVPbkSimFieldTypeArray& CVPbkSimContactView::SortOrderL() const
    {
    return iSortOrder;
    }
    
// --------------------------------------------------------------------------
// CVPbkSimContactView::StoreReady
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::StoreReady( MVPbkSimCntStore& /*aStore*/ )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::StoreReady"));
        
    // Store has become ready or has been updated. Update the view too.
    StartSorting();
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::StoreReady
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::StoreError( MVPbkSimCntStore& /*aStore*/, TInt aError )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::StoreError %d"), aError);
        
    SetToUnavailableState();
    VPbkSimStoreImpl::SendObserverMessageRV( iObservers, 
            &MVPbkSimViewObserver::ViewError, *this, aError );
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::StoreNotAvailable
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::StoreNotAvailable( MVPbkSimCntStore& /*aStore*/ )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::StoreNotAvailable"));
        
    // Reset view until StoreReady is called again
    SetToUnavailableState();
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::StoreContactEvent
// --------------------------------------------------------------------------
//    
void CVPbkSimContactView::StoreContactEvent( TEvent aEvent, TInt aSimIndex )
    {    
    TRAPD( res, HandleStoreContactEventL( aEvent, aSimIndex ) );
    if ( res != KErrNone )
        {
        // The view is not up to date any more.
        SetToUnavailableState();
        VPbkSimStoreImpl::SendObserverMessageRV( iObservers, 
                &MVPbkSimViewObserver::ViewError, *this, res );
        }
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::Sort
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::Sort()
    {
    TRAPD( res, SortL() );
    if ( res != KErrNone )
        {
        SortError( res );
        }
    }
    
// --------------------------------------------------------------------------
// CVPbkSimContactView::SortL
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::SortL()
    {
    // Get the size of the store
    TVPbkGsmStoreProperty properties;
    User::LeaveIfError( iParentStore. GetGsmStoreProperties( properties ) );
    TInt lastIndex = properties.iTotalEntries;
    
    // Loop all contacts of the store.
    for ( TInt i = KVPbkSimStoreFirstETelIndex; i <= lastIndex; ++i )
        {
        const MVPbkSimContact* cnt = iParentStore.ContactAtL( i );
        if ( cnt )
            {
            InsertViewContactL( *cnt );
            }
        }

    // All contacts have been sorted and the view is ready
    SetToReadyState();
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::SortError
// --------------------------------------------------------------------------
//
TInt CVPbkSimContactView::SortError( TInt aError )
    {
    iAsyncOperation->Purge();
    VPbkSimStoreImpl::SendObserverMessageRV( iObservers, 
        &MVPbkSimViewObserver::ViewError, *this, aError );
    return KErrNone;
    }


// --------------------------------------------------------------------------
// CVPbkSimContactView::IsSorting
// --------------------------------------------------------------------------
//
TBool CVPbkSimContactView::IsSorting() const
    {
    return iIdleSort->IsActive();
    }   
    
// --------------------------------------------------------------------------
// CVPbkSimContactView::StartSorting
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::StartSorting()
    {
    iIdleSort->Cancel();
    iIdleSort->Start( 
            TCallBack( CVPbkSimContactView::IdleSortCallback, this ) );
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::HandleStoreContactEventL
// --------------------------------------------------------------------------
//    
void CVPbkSimContactView::HandleStoreContactEventL( TEvent aEvent, TInt aSimIndex )
    {    
    switch( aEvent )
        {
        case EContactAdded:
            {
            HandleContactAddedL( aSimIndex );
            break;
            }
        case EContactDeleted:
            {
            HandleContactRemoved( aSimIndex );
            break;
            }
        case EContactChanged:
            {
            HandleContactChangedL( aSimIndex );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::HandleContactAddedL
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::HandleContactAddedL( TInt aSimIndex )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::HandleContactAdded: simIndex %d"), 
        aSimIndex );
        
    const MVPbkSimContact* addedCnt = iParentStore.ContactAtL( aSimIndex );
    if ( addedCnt )
        {
        TInt viewIndex = InsertViewContactL( *addedCnt );
        VPbkSimStoreImpl::SendObserverMessageVVV( iObservers, 
            &MVPbkSimViewObserver::ViewContactEvent,
            MVPbkSimViewObserver::EContactAdded, viewIndex, aSimIndex );
        }
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::HandleContactRemoved
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::HandleContactRemoved( TInt aSimIndex )
    {    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::HandleContactRemoved: simIndex %d"), 
        aSimIndex );
        
    TInt removedIndex = RemoveViewContact( aSimIndex );
    VPbkSimStoreImpl::SendObserverMessageVVV( iObservers, 
            &MVPbkSimViewObserver::ViewContactEvent,
            MVPbkSimViewObserver::EContactDeleted, removedIndex, aSimIndex );
    }
        
// --------------------------------------------------------------------------
// CVPbkSimContactView::HandleContactChangedL
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::HandleContactChangedL( TInt aSimIndex )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::HandleContactChanged: simIndex %d"), 
        aSimIndex );
        
    // Changed contact is a bit complicated from the view point of view
    // due to case of contacts that have same name
    // E.g the view has following contacts
    // index 0: Jill
    // index 1: Jill
    // index 2: Jill
    // Then editing any of them must not change the position of the
    // contact in the view if the name remains same.
    
    // Get the changed contact
    const MVPbkSimContact* changedCnt = iParentStore.ContactAtL( aSimIndex );
    
    if (  changedCnt )
        {
        // Get current position of the changed contact
        TInt curIndex = iViewContacts.Find( aSimIndex, 
                TIdentityRelation<TViewContact>( CompareViewContactSimIndex ) );
        
        // Remove the contact from the view
        TInt nextContactIndex = curIndex + 1;
        
        // Investigate if we can ignore the event from view point of i.e if
        // the position of the contacts is still same.
        TBool keepInSamePos = EFalse;
        const TInt viewCntCount = iViewContacts.Count();
        if ( PassesFilter( *changedCnt, iFilter ) &&
             nextContactIndex >= 0 && nextContactIndex < viewCntCount )
            {
            // Create a temp copy of changedCnt becuase next ContactAtL will
            // overwrite the reference.
            CVPbkSimContactBuf* temp = CVPbkSimContactBuf::NewLC( 
                changedCnt->ETelContactL(), iParentStore );
            // Get the next contact
            const MVPbkSimContact* nextCnt = iParentStore.ContactAtL( 
                    iViewContacts[nextContactIndex].iSimIndex );
            // Compare names of the changed contact and the next contact
            if ( iSortUtil->Compare( *temp, *nextCnt ) == 0 )
                {
                // Next contact has a same name. This means that we don't
                // need to modify the view at all.
                keepInSamePos = ETrue;
                }
            CleanupStack::PopAndDestroy( temp );
            }
        
        if ( keepInSamePos )
            {
            // Though there was no need to change anything the events must still
            // be sent to observers.
            VPbkSimStoreImpl::SendObserverMessageVVV( iObservers, 
                &MVPbkSimViewObserver::ViewContactEvent,
                MVPbkSimViewObserver::EContactDeleted, curIndex, aSimIndex );
            VPbkSimStoreImpl::SendObserverMessageVVV( iObservers, 
                &MVPbkSimViewObserver::ViewContactEvent,
                MVPbkSimViewObserver::EContactAdded, curIndex, aSimIndex );
            }
        else
            {
            HandleContactRemoved( aSimIndex );
            HandleContactAddedL( aSimIndex );
            }
        }
    }

        
// --------------------------------------------------------------------------
// CVPbkSimContactView::InsertViewContactL
// --------------------------------------------------------------------------
//
TInt CVPbkSimContactView::InsertViewContactL( 
        const MVPbkSimContact& aContact )
    {
    TInt index = KErrNotFound;
    if ( PassesFilter( aContact, iFilter ) )
        {
        // Use temp contact because Sorting uses ContactAtL
        iTempContactForSorting->SetL( aContact.ETelContactL() );
        if ( iConstructionPolicy == EVPbkSortedSimView )
            {
            index = SortedIndexL( *iTempContactForSorting );
            }
        else
            {
            index = UnsortedIndex( *iTempContactForSorting );
            }
        TViewContact cnt( iTempContactForSorting->SimIndex() );
        iViewContacts.InsertL( cnt, index );
        }
    return index;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::SortedIndexL
// --------------------------------------------------------------------------
//
TInt CVPbkSimContactView::SortedIndexL( const MVPbkSimContact& aLeft )
    {
    TInt first = 0;
    TInt len = iViewContacts.Count();

    while ( len > 0 )
        {
        TInt half = len / 2;
        TInt middle = first + half;
        TInt simIndex = iViewContacts[middle].iSimIndex;
        const MVPbkSimContact* right = 
            iParentStore.ContactAtL( simIndex );
        if ( iSortUtil->Compare( aLeft, *right ) < 0 )
            {
            len = half;
            }
        else
            {
            first = middle + 1;
            len = len - half - 1;
            }
        }

    len = iViewContacts.Count();
    if ( first < len )
        {
        TInt simIndex = iViewContacts[first].iSimIndex;
        if ( iSortUtil->Compare( aLeft, 
             *iParentStore.ContactAtL( simIndex ) ) == 0 )
            {
            ++first;
            }
        }

    __ASSERT_DEBUG( first <= len, 
            Panic( VPbkSimStoreImpl::EInvalidSortedIndex ) );
    if ( first > len )
        {
        first = len;
        }

    return first;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::UnsortedIndex
// --------------------------------------------------------------------------
//
TInt CVPbkSimContactView::UnsortedIndex( const MVPbkSimContact& aLeft )
    {
    const TInt count = iViewContacts.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aLeft.SimIndex() < iViewContacts[i].iSimIndex )
            {
            return i;
            }
        }
    return count;
    }
    
// --------------------------------------------------------------------------
// CVPbkSimContactView::RemoveViewContact
// --------------------------------------------------------------------------
//
TInt CVPbkSimContactView::RemoveViewContact( TInt aSimIndex )
    {
    TInt index = iViewContacts.Find( aSimIndex, 
        TIdentityRelation<TViewContact>( CompareViewContactSimIndex ) );
        
    if ( index != KErrNotFound )
        {
        iViewContacts.Remove( index );
        }
        
    return index;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::ResetAndCopySortOrderL
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::ResetAndCopySortOrderL( const RVPbkSimFieldTypeArray& aSource )
    {
    // Reset old sort order
    iSortOrder.Reset();
    // Copy the new one
    TInt count = aSource.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iSortOrder.AppendL( aSource[i] );
        }
    // Recreate sort util with new sort order
    VPbkSimStoreImpl::CSimCntSortUtil* sortUtil = 
            VPbkSimStoreImpl::CSimCntSortUtil::NewL( iSortOrder );
    delete iSortUtil;
    iSortUtil = sortUtil;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::Reset
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::Reset()
    {
    iViewContacts.Reset();
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::PassesFilter
// --------------------------------------------------------------------------
//
inline TBool CVPbkSimContactView::PassesFilter( const MVPbkSimContact& aContact,
        const CVPbkSimFieldTypeFilter* aFilter ) const
    {
    // Initialize to 'fail'
    TBool ret = EFalse;

    if ( aFilter )
        {
        // Get filtering criteria
        TUint16 criteria = aFilter->FilteringCriteria();

        if ( criteria &
                CVPbkSimFieldTypeFilter::ESimFilterCriteriaGsmNumber )
            {
            MVPbkSimContact::TFieldLookup lookup =
                aContact.FindField( EVPbkSimGsmNumber );
            ret = !lookup.EndOfLookup();
            }

        if ( !ret && criteria &
                CVPbkSimFieldTypeFilter::ESimFilterCriteriaAdditionalNumber )
            {
            MVPbkSimContact::TFieldLookup lookup =
                aContact.FindField( EVPbkSimAdditionalNumber );
            ret = !lookup.EndOfLookup();
            }

        if ( !ret && criteria &
                CVPbkSimFieldTypeFilter::ESimFilterCriteriaEmailAddress )
            {
            MVPbkSimContact::TFieldLookup lookup =
                aContact.FindField( EVPbkSimEMailAddress );
            ret = !lookup.EndOfLookup();
            }

        if ( !ret && criteria &
                CVPbkSimFieldTypeFilter::ESimFilterCriteriaName )
            {
            MVPbkSimContact::TFieldLookup lookup =
                aContact.FindField( EVPbkSimName );
            ret = !lookup.EndOfLookup();
            }

        if ( !ret && criteria &
                CVPbkSimFieldTypeFilter::ESimFilterCriteriaSecondName )
            {
            MVPbkSimContact::TFieldLookup lookup =
                aContact.FindField( EVPbkSimNickName );
            ret = !lookup.EndOfLookup();
            }

        if ( !ret && criteria == 0 )
            {
            ret = ETrue;
            }
        }
    else
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::SetToReadyState
// --------------------------------------------------------------------------
//
void CVPbkSimContactView::SetToReadyState()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::SetToReadyState"));
    // Cancel any async callback because event is sent to all observer
    iAsyncOperation->Purge();
    
    iViewState = EReady;
    VPbkSimStoreImpl::SendObserverMessageR( iObservers, 
            &MVPbkSimViewObserver::ViewReady, *this );
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::SetToNotReadyState
// In Not Ready State the view doesn't have observer and it 
// doesn't listen to any events. It waits that a client calls OpenL.
// --------------------------------------------------------------------------
//    
void CVPbkSimContactView::SetToNotReadyState()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::SetToNotReadyState"));
        
    Reset();
    iParentStore.Close( *this );
    iViewState = ENotReady;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::SetToUnavailableState
// In Unavailable state the view doesn' listen to contact events. It listens
// to store event to get Store Ready event.
// --------------------------------------------------------------------------
//        
void CVPbkSimContactView::SetToUnavailableState()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CVPbkSimContactView::SetToUnavailableState"));
        
    Reset();
    iAsyncOperation->Purge();
    iViewState = EUnvavailable;
    VPbkSimStoreImpl::SendObserverMessageR( iObservers, 
            &MVPbkSimViewObserver::ViewNotAvailable, *this );
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::ViewStateReady
// --------------------------------------------------------------------------
//               
TBool CVPbkSimContactView::ViewStateReady() const
    {
    return iViewState == EReady;
    }
    
// --------------------------------------------------------------------------
// CVPbkSimContactView::ViewStateUnavailable
// --------------------------------------------------------------------------
//               
TBool CVPbkSimContactView::ViewStateUnavailable() const
    {
    return iViewState == EUnvavailable;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::ViewStateNotReady
// --------------------------------------------------------------------------
//           
TBool CVPbkSimContactView::ViewStateNotReady() const
    {
    return iViewState == ENotReady;
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::DoViewOpenCallbackL
// --------------------------------------------------------------------------
//           
void CVPbkSimContactView::DoViewOpenCallbackL( MVPbkSimViewObserver& aObserver )
    {
    aObserver.ViewReady( *this );
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::DoViewUnavailableCallbackL
// --------------------------------------------------------------------------
//           
void CVPbkSimContactView::DoViewUnavailableCallbackL( MVPbkSimViewObserver& aObserver )
    {
    aObserver.ViewNotAvailable( *this );
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::DoViewErrorCallback
// --------------------------------------------------------------------------
//           
void CVPbkSimContactView::DoViewErrorCallback( MVPbkSimViewObserver& aObserver, 
        TInt aError )
    {
    aObserver.ViewError( *this, aError );
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::CompareViewContactSimIndex
// --------------------------------------------------------------------------
//           
TBool CVPbkSimContactView::CompareViewContactSimIndex( const TViewContact& aLeft, 
        const TViewContact& aRight )
    {
    return aLeft.iSimIndex == aRight.iSimIndex;    
    }

// --------------------------------------------------------------------------
// CVPbkSimContactView::IdleSortCallback
// --------------------------------------------------------------------------
//           
TInt CVPbkSimContactView::IdleSortCallback( TAny* aThis )
    {
    static_cast<CVPbkSimContactView*>( aThis )->Sort();
    // Don't continue idle i.e return false value
    return 0;
    }
//  End of File  
