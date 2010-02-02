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
* Description:  Virtual phonebook view implementation
*
*/
 

#include "CContactView.h"

// VPbkSimStore
#include "CFieldTypeMappings.h"
#include "CContactStore.h"
#include "CViewContact.h"
#include "CRemoteStore.h"
#include "CRemoteView.h"
#include "CContactLink.h"
#include "VPbkSimStoreError.h"
#include "CSupportedFieldTypes.h"
#include "CFindView.h"
#include "RemoteViewPreferences.h"
#include "CSortOrderAcquirerList.h"

// VPbkEngUtils
#include <CVPbkAsyncOperation.h>
#include <CVPbkAsyncCallback.h>

// VPbkEng
#include <CVPbkSortOrder.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkFieldTypeRefsList.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkSendEventUtility.h>
#include <VPbkError.h>

// VPbkSimStoreImpl
#include <CVPbkSimFieldTypeFilter.h>
#include <CVPbkSimContactView.h>
#include <RVPbkStreamedIntArray.h>
#include <MVPbkSimContact.h>


namespace VPbkSimStore {

// ============================= LOCAL FUNCTIONS ===============================

// --------------------------------------------------------------------------
// ConvertSortOrderL
// Converts VPbk sort order to native sort order
// --------------------------------------------------------------------------
//
void ConvertSortOrderL( const MVPbkFieldTypeList& aSource, 
    RVPbkSimFieldTypeArray& aTarget, CFieldTypeMappings& aMappings )
    {
    TInt typeCount = aSource.FieldTypeCount();
    for ( TInt i = 0; i < typeCount; ++i )
        {
        const MVPbkFieldType& vpbkType = aSource.FieldTypeAt( i );
        TVPbkSimCntFieldType simType = 
            aMappings.Match( vpbkType );
        if ( simType != EVPbkSimUnknownType )
            {
            aTarget.AppendL( simType );
            }
        }
    __ASSERT_DEBUG( aTarget.Count() > 0, 
        VPbkSimStore::Panic( VPbkSimStore::EZeroSortOrder ) );
    }

// --------------------------------------------------------------------------
// ConvertSortOrderL
// Converts VPbk sort order to native sort order
// --------------------------------------------------------------------------
//
void ConvertSortOrderL( const RVPbkSimFieldTypeArray& aSource, 
        CVPbkFieldTypeRefsList& aTarget, CFieldTypeMappings& aMappings )
    {
    TInt typeCount = aSource.Count();
    for ( TInt i = 0; i < typeCount; ++i )
        {
        const MVPbkFieldType* vpbkType = aMappings.Match( aSource[i] );
        if ( vpbkType )
            {
            aTarget.AppendL( *vpbkType );
            }
        }
    __ASSERT_DEBUG( aTarget.FieldTypeCount() > 0, 
        VPbkSimStore::Panic( VPbkSimStore::EZeroSortOrder ) );
    }

// --------------------------------------------------------------------------
// ConvertFieldTypeFilter
// Converts VPbk field type filter to native filter
// --------------------------------------------------------------------------
//
void ConvertFieldTypeFilter( CVPbkFieldTypeSelector& aSource,
        CVPbkSimFieldTypeFilter& aTarget,
        const MVPbkFieldTypeList& aFieldTypeList,
        CFieldTypeMappings& aMappings )
    {
    TInt typeCount = aFieldTypeList.FieldTypeCount();
    for ( TInt i = 0; i < typeCount; ++i )
        {
        const MVPbkFieldType& fieldType =
            aFieldTypeList.FieldTypeAt( i );

        if ( aSource.IsFieldTypeIncluded( fieldType ) )
            {
            // Match was found, now find out the which
            // SIM field type maps to the match
            TVPbkSimCntFieldType simType = EVPbkSimUnknownType;

            const TVPbkFieldVersitProperty* versitProperty =
                aSource.MatchingVersitProperty( fieldType );
            if ( versitProperty )
                {
                simType = aMappings.Match( *versitProperty );
                }
            else
                {
                TVPbkNonVersitFieldType nonVersitType =
                    aSource.MatchingNonVersitType( fieldType );

                if ( nonVersitType != EVPbkNonVersitTypeNone )
                    {
                    simType = aMappings.Match( nonVersitType );
                    }
                }

            // Construct a filter based on the SIM field type
            TUint16 filteringFlag =
                aTarget.FilteringFlagForSimFieldType( simType );
            aTarget.AppendFilteringFlag( filteringFlag );
            
            // Add also additional number to the filtering
            // if main number is included
            if ( filteringFlag &
                CVPbkSimFieldTypeFilter::ESimFilterCriteriaGsmNumber )
                {
                aTarget.AppendFilteringFlag(
                    CVPbkSimFieldTypeFilter::ESimFilterCriteriaAdditionalNumber );
                }            
            }
        }
    }

// --------------------------------------------------------------------------
// ConvertSortPolicy
// Convert between EVPbkSortedSimView and EVPbkUnsortedContactView
// --------------------------------------------------------------------------
//
TVPbkSimViewConstructionPolicy ConvertSortPolicy( 
        TVPbkContactViewSortPolicy aVPbkSortPolicy )
    {
    TVPbkSimViewConstructionPolicy sortPolicy = EVPbkSortedSimView;
    if ( aVPbkSortPolicy == EVPbkUnsortedContactView )
        {
        sortPolicy = EVPbkUnsortedSimView;
        }
    return sortPolicy;
    }

// --------------------------------------------------------------------------
// FindFieldTypeIndex
// --------------------------------------------------------------------------
//    
TInt FindFieldTypeIndex( const MVPbkFieldTypeList& aTypeList, 
        const MVPbkFieldType& aType )
    {
    TInt result = KErrNotFound;
    const TInt count = aTypeList.FieldTypeCount();
    for ( TInt i = 0; i < count && result == KErrNotFound; ++i )
        {
        if ( aTypeList.FieldTypeAt(i).IsSame( aType ) )
            {
            result = i;
            }
        }
    return result;
    }
    
// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CContactView::CContactView
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
inline CContactView::CContactView(CContactStore& aParentStore) :
    iParentStore(aParentStore),
    iViewState( ENotReady )
    {
    }

// --------------------------------------------------------------------------
// CContactView::~CContactView
// --------------------------------------------------------------------------
//
CContactView::~CContactView()
    {
    delete iViewDefinition;
    delete iObserverOp;
    delete iFilterObsOp;
    delete iCurrentContact;
    delete iSortOrder;
    if ( iNativeView )
        {
        iNativeView->Close( *this );
        delete iNativeView;
        }
    iObservers.Close();
    iFilteringObservers.Close();
    }

// --------------------------------------------------------------------------
// CContactView::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CContactView* CContactView::NewLC(MVPbkContactViewObserver& aObserver,
    CContactStore& aParentStore,
    const MVPbkFieldTypeList& aSortOrder,
    const CVPbkContactViewDefinition& aViewDefinition)
    {
    CContactView* self = new(ELeave) CContactView(aParentStore);
    CleanupStack::PushL(self);
    self->ConstructL(aSortOrder, aViewDefinition);
    self->AddObserverL(aObserver);
    return self;
    }

// --------------------------------------------------------------------------
// CContactView::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CContactView::ConstructL( const MVPbkFieldTypeList& aSortOrder,
    const CVPbkContactViewDefinition& aViewDefinition )
    {
    // Copy the sort order
    iSortOrder = CVPbkSortOrder::NewL( aSortOrder );
    
    // Create current view contact
    iCurrentContact = CViewContact::NewL( *this, *iSortOrder );

    iViewDefinition = CVPbkContactViewDefinition::NewL( aViewDefinition );
    
    // Convert VPbk sort order to sim sort order
    RVPbkSimFieldTypeArray sortOrder;
    CleanupClosePushL( sortOrder );
    ConvertSortOrderL( aSortOrder, sortOrder, 
        iParentStore.FieldTypeMappings() );

    CVPbkSimFieldTypeFilter* filter = new ( ELeave ) CVPbkSimFieldTypeFilter;
    CVPbkFieldTypeSelector* selector = iViewDefinition->FieldTypeFilter();
    if ( selector )
	    {
        // First construct filter for supported SIM fields
        ConvertFieldTypeFilter( *selector, *filter,
	        iParentStore.MasterFieldTypeList(),
                iParentStore.FieldTypeMappings() );
	    }

    // Sorted or SIM index ordered view.
    TVPbkSimViewConstructionPolicy sortPolicy = ConvertSortPolicy( 
            iViewDefinition->SortPolicy() );
            
    if ( RemoteViewDefinition( *iViewDefinition ) )
        {
        // Create a remote view from remote store
        iNativeView = iParentStore.NativeStore().CreateViewL( sortOrder,
            sortPolicy, RemoteViewName( *iViewDefinition ), filter );
        }
    else
        {
        // Create a local view
         CVPbkSimContactView* view = CVPbkSimContactView::NewLC( sortOrder,
            sortPolicy, iParentStore.NativeStore(), 
            RemoteViewName( *iViewDefinition ), filter );
        CleanupStack::Pop( view );
        iNativeView = view;
        }  
    
    iNativeView->OpenL( *this );
    CleanupStack::PopAndDestroy(); // sortOrder

    iObserverOp = 
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>::NewL();
    iFilterObsOp = 
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>::NewL();
    }

// --------------------------------------------------------------------------
// CContactView::ParentObject
// --------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CContactView::ParentObject() const
    {
    return iParentStore;
    }

// --------------------------------------------------------------------------
// CContactView::Type
// --------------------------------------------------------------------------
//
TVPbkContactViewType CContactView::Type() const
    {
    return EVPbkContactsView;
    }

// --------------------------------------------------------------------------
// CContactView::ChangeSortOrderL
// --------------------------------------------------------------------------
//
void CContactView::ChangeSortOrderL( const MVPbkFieldTypeList& aSortOrder )
    {
    // Check that S60 shared view sort order is not changed illegally.
    LeaveIfIncorrectSortOrderL( aSortOrder );
    
    // Create a new sort order
    CVPbkSortOrder* sortOrder = CVPbkSortOrder::NewL( aSortOrder );
    CleanupStack::PushL( sortOrder );
    
    // Change remote view sort order
    RVPbkSimFieldTypeArray nativeOrder;
    CleanupClosePushL( nativeOrder );
    ConvertSortOrderL( *sortOrder, nativeOrder, 
        iParentStore.FieldTypeMappings() );
    iNativeView->ChangeSortOrderL( nativeOrder );
    CleanupStack::PopAndDestroy( &nativeOrder );
    
    // Swap
    delete iSortOrder;
    iSortOrder = sortOrder;
    CleanupStack::Pop( sortOrder );
    }

// --------------------------------------------------------------------------
// CContactView::SortOrder
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CContactView::SortOrder() const
    {
    return *iSortOrder;
    }

// --------------------------------------------------------------------------
// CContactView::RefreshL
// --------------------------------------------------------------------------
//
void CContactView::RefreshL()
    {
    // Rebuild view
    RVPbkSimFieldTypeArray sortOrder;
    CleanupClosePushL( sortOrder );
    ConvertSortOrderL( *iSortOrder, sortOrder, 
        iParentStore.FieldTypeMappings() );
    iNativeView->ChangeSortOrderL( sortOrder );
    CleanupStack::PopAndDestroy(); // sortOrder
    }

// --------------------------------------------------------------------------
// CContactView::ContactCountL
// --------------------------------------------------------------------------
//
TInt CContactView::ContactCountL() const
    {
    TInt res = 0;
    if ( State() == EReady )
        {
        res = iNativeView->CountL();    
        }
    return res;
    }

// --------------------------------------------------------------------------
// CContactView::ContactAtL
// --------------------------------------------------------------------------
//
const MVPbkViewContact& CContactView::ContactAtL( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0, 
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= iNativeView->CountL() )
        {
        User::Leave( KErrArgument );
        }

    iCurrentContact->SetSimContactL( iNativeView->ContactAtL( aIndex ) );
    return *iCurrentContact; 
    }

// --------------------------------------------------------------------------
// CContactView::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CContactView::CreateLinkLC( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0, 
        VPbkError::Panic(VPbkError::EInvalidContactIndex) );
    if ( aIndex >= iNativeView->CountL() )
        {
        User::Leave( KErrArgument );
        }

    TInt simIndex = MapViewIndexToSimIndexL( aIndex );
    return CContactLink::NewLC( iParentStore, simIndex );
    }

// --------------------------------------------------------------------------
// CContactView::IndexOfLinkL
// --------------------------------------------------------------------------
//
TInt CContactView::IndexOfLinkL( const MVPbkContactLink& aContactLink ) const
    {
    if ( &aContactLink.ContactStore() == &iParentStore )
        {
        TInt simIndex = 
            static_cast<const CContactLink&>( aContactLink ).SimIndex();
        return iNativeView->MapSimIndexToViewIndexL( simIndex );
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CContactView::AddObserverL
// --------------------------------------------------------------------------
//
void CContactView::AddObserverL( MVPbkContactViewObserver& aObserver )
    {
    CVPbkAsyncObjectCallback<MVPbkContactViewObserver>* callback =
        VPbkEngUtils::CreateAsyncObjectCallbackLC(
            *this, 
            &CContactView::DoAddObserverL, 
            &CContactView::AddObserverError, 
            aObserver);
    
    iObserverOp->CallbackL( callback );
    CleanupStack::Pop( callback );

    /// Insert to first position because event are send in reverse order.
    /// Last inserted gets notifcation last.
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.InsertL( &aObserver, 0 );
        }
    }

// --------------------------------------------------------------------------
// CContactView::RemoveObserver
// --------------------------------------------------------------------------
//
void CContactView::RemoveObserver( MVPbkContactViewObserver& aObserver )
    {
    iObserverOp->CancelCallback( &aObserver );
    TInt pos = iObservers.Find( &aObserver );
    if ( pos != KErrNotFound )
        {
        iObservers.Remove( pos );
        }
    }

// --------------------------------------------------------------------------
// CContactView::MatchContactStore
// --------------------------------------------------------------------------
//
TBool CContactView::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iParentStore.MatchContactStore(aContactStoreUri);
    }
        
// --------------------------------------------------------------------------
// CContactView::MatchContactStoreDomain
// --------------------------------------------------------------------------
//
TBool CContactView::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain) const
    {
    return iParentStore.MatchContactStoreDomain(aContactStoreDomain);
    }

// --------------------------------------------------------------------------
// CContactView::CreateBookmarkLC
// --------------------------------------------------------------------------
//
MVPbkContactBookmark* CContactView::CreateBookmarkLC(TInt aIndex) const
    {
    __ASSERT_ALWAYS( aIndex >= 0, 
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );

    TInt simIndex = MapViewIndexToSimIndexL( aIndex );
    // This store implements bookmark as a link
    return CContactLink::NewLC( iParentStore, simIndex );
    }

// --------------------------------------------------------------------------
// CContactView::IndexOfBookmarkL
// --------------------------------------------------------------------------
//
TInt CContactView::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark) const
    {
    // Bookmark is implemented as a link in this store
    const CContactLink* link = 
        dynamic_cast<const CContactLink*>( &aContactBookmark );
    if ( link && ( &link->ContactStore() == &iParentStore ) )
        {
        return iNativeView->MapSimIndexToViewIndexL( link->SimIndex() );
        }
    return KErrNotFound;        
    }

// --------------------------------------------------------------------------
// CContactView::AddFilteringObserverL
// --------------------------------------------------------------------------
//    
void CContactView::AddFilteringObserverL( 
        MFilteredViewSupportObserver& aObserver )
    {
    // Add observer in the callback to ensure that observer will get the
    // event asynchrnously all times.
    CVPbkAsyncObjectCallback<MFilteredViewSupportObserver>* callback =
        VPbkEngUtils::CreateAsyncObjectCallbackLC(
            *this, 
            &CContactView::DoAddFilteringObserverL, 
            &CContactView::DoAddFilteringObserverError, 
            aObserver);
    
    iFilterObsOp->CallbackL( callback );
    CleanupStack::Pop( callback );
    }

// --------------------------------------------------------------------------
// CContactView::RemoveFilteringObserver
// --------------------------------------------------------------------------
//    
void CContactView::RemoveFilteringObserver( 
        MFilteredViewSupportObserver& aObserver )
    {
    iFilterObsOp->CancelCallback( &aObserver );	
    const TInt index( iFilteringObservers.Find( &aObserver ) );
    if ( index != KErrNotFound )
        {
        iFilteringObservers.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CContactView::IsNativeMatchingRequestActive
// --------------------------------------------------------------------------
//     
TBool CContactView::IsNativeMatchingRequestActive()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CContactView::ViewFiltering
// --------------------------------------------------------------------------
//     
MVPbkContactViewFiltering* CContactView::ViewFiltering()
    {
    return this;
    }
    
// --------------------------------------------------------------------------
// CContactView::CreateFilteredViewLC
// --------------------------------------------------------------------------
//     
MVPbkContactViewBase* CContactView::CreateFilteredViewLC( 
        MVPbkContactViewObserver& aObserver,
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )  
    {
    CFindView* findView = CFindView::NewLC( aFindWords, *this, aObserver, 
        aAlwaysIncludedContacts, Store().ContactStoreDomainFsSession() );
    findView->ActivateContactMatchL();
    return findView;
    }

// --------------------------------------------------------------------------
// CContactView::UpdateFilterL
// --------------------------------------------------------------------------
//
void CContactView::UpdateFilterL( 
        const MDesCArray& /*aFindWords*/,
        const MVPbkContactBookmarkCollection* /*aAlwaysIncludedContacts*/ )
    {
    // This view itself is not a filtered view.
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CContactView::ViewReady
// --------------------------------------------------------------------------
//
void CContactView::ViewReady( MVPbkSimCntView& aView )
    {
    // Due to fact that native view can be a remote view the sort order
    // must be up to date with the actual view in server side.
    TInt inspectionResult = KErrNone;
    TRAPD( res, inspectionResult = InspectSortOrderL() );
    if ( res == KErrNone )
        {
        res = inspectionResult;
        }
        
    if ( res == KErrNone )
        {
        iViewState = EReady;
        iCurrentContact->SetSortOrder( *iSortOrder );
        SendViewStateEvent();
        }
    else
        {
        ViewError( aView, res );
        }
    }

// --------------------------------------------------------------------------
// CContactView::ViewError
// --------------------------------------------------------------------------
//
void CContactView::ViewError( MVPbkSimCntView& /*aView*/, TInt aError )
    {
    iViewState = ENotAvailable;
    SendViewErrorEvent( aError );
    }

// --------------------------------------------------------------------------
// CContactView::ViewNotAvailable
// --------------------------------------------------------------------------
//
void CContactView::ViewNotAvailable( MVPbkSimCntView& /*aView*/ )
    {
    iViewState = ENotAvailable;
    SendViewStateEvent();
    }

// --------------------------------------------------------------------------
// CContactView::ViewContactEvent
// --------------------------------------------------------------------------
//
void CContactView::ViewContactEvent( TEvent aEvent, TInt aIndex, 
    TInt aSimIndex )
    {
    CContactLink* link = NULL;
    TRAPD( result,
        {
        link = CContactLink::NewLC( iParentStore, aSimIndex );
        CleanupStack::Pop( link );
        });
    if ( result != KErrNone )
        {
        ViewError( *iNativeView, result );
        return;
        }
    
    switch ( aEvent )
        {
        case EContactAdded:
            {
            // First to external observers of this view
            VPbkEng::SendViewEventToObservers( *this, aIndex, *link,
                iObservers,
                &MVPbkContactViewObserver::ContactAddedToView, 
                &MVPbkContactViewObserver::ContactViewError );
            // Then to internal filtering observers
            VPbkEng::SendViewEventToObservers( *this, aIndex, *link,
                iFilteringObservers,
                &MVPbkContactViewObserver::ContactAddedToView, 
                &MVPbkContactViewObserver::ContactViewError );
            break;
            }
        case EContactDeleted:
            {
            // First to external observers of this view
            VPbkEng::SendViewEventToObservers( *this, aIndex, *link,
                iObservers,
                &MVPbkContactViewObserver::ContactRemovedFromView, 
                &MVPbkContactViewObserver::ContactViewError );
            // Then to internal filtering observers
            VPbkEng::SendViewEventToObservers( *this, aIndex, *link,
                iFilteringObservers,
                &MVPbkContactViewObserver::ContactRemovedFromView, 
                &MVPbkContactViewObserver::ContactViewError );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, 
                VPbkSimStore::Panic( VPbkSimStore::EUnknownSimViewEvent ) );
            SendViewErrorEvent( KErrUnknown );
            break;
            }
        }

    delete link;
    }

// --------------------------------------------------------------------------
// CContactView::InspectSortOrderL
// --------------------------------------------------------------------------
//
TInt CContactView::InspectSortOrderL()
    {
    // Get sort order from SIM view
    const RVPbkSimFieldTypeArray& nativeOrder = iNativeView->SortOrderL();
    CVPbkFieldTypeRefsList* fieldTypes = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( fieldTypes );
    // Convert to VPbk format
    ConvertSortOrderL( nativeOrder, *fieldTypes, 
        iParentStore.FieldTypeMappings() );
    
    // Check that all native field types are found from iSortOrder that
    // came from client. Types must be also in same order in nativeOrder
    // as in iSortOrder but there can be types in iSortOrder that are not
    // supported by nativeOrder.
    TInt result = KErrNone;
    TInt typeIndex = KErrNotFound;
    const TInt nativeCount = fieldTypes->FieldTypeCount();
    for ( TInt i = 0; i < nativeCount && result == KErrNone; ++i )
        {
        TInt index = FindFieldTypeIndex( *iSortOrder, 
                fieldTypes->FieldTypeAt(i) );
        // Check the order of native order type against iSortOrder type
        if ( index <= typeIndex )
            {
            // If this is the case then client has tried to create a 
            result = KErrArgument;
            }
        typeIndex = index;
        }
    
    CleanupStack::PopAndDestroy( fieldTypes );
    return result;
    }
    
// --------------------------------------------------------------------------
// CContactView::MapViewIndexToSimIndexL
// --------------------------------------------------------------------------
//
TInt CContactView::MapViewIndexToSimIndexL( TInt aViewIndex ) const
    {
    MVPbkSimContact& cnt = iNativeView->ContactAtL( aViewIndex );
    return cnt.SimIndex();
    }

// --------------------------------------------------------------------------
// CContactView::DoAddObserverL
// --------------------------------------------------------------------------
//
void CContactView::DoAddObserverL(MVPbkContactViewObserver& aObserver)
    {
    if ( iViewState == EReady ) 
        {
        // If this view is ready and there was no error,
        // tell it to the observer
        aObserver.ContactViewReady( *this );
        }
    else if ( iViewState == ENotAvailable )
        {
        // AddObserverL must always make a callback according VPbk API
        // documentation. If iViewState is ENotReady then the call back
        // will be caused by CRemoteView::OpenL
        aObserver.ContactViewUnavailable( *this );
        }
    }

// --------------------------------------------------------------------------
// CContactView::AddObserverError
// --------------------------------------------------------------------------
//
void CContactView::AddObserverError( MVPbkContactViewObserver& aObserver, 
        TInt aError )
    {
    aObserver.ContactViewError(*this, aError, EFalse);
    }

// --------------------------------------------------------------------------
// CViewBase::DoAddFilteringObserverL
// --------------------------------------------------------------------------
//    
void CContactView::DoAddFilteringObserverL(
        MFilteredViewSupportObserver& aObserver )
    {
    /// Insert to first position because event are send in reverse order.
    /// Last inserted gets notifcation last.
    iFilteringObservers.InsertL( &aObserver, 0 );
    
    if ( iViewState == EReady ) 
        {
        // If this view is ready and there was no error,
        // tell it to the observer
        aObserver.ContactViewReadyForFiltering( *this );
        aObserver.ContactViewReady( *this );
        }
    else if ( iViewState == ENotAvailable )
        {
        // If iViewState is ENotReady then the call back
        // will be caused by CRemoteView::OpenL
        aObserver.ContactViewUnavailableForFiltering( *this );
        aObserver.ContactViewUnavailable( *this );
        }
    }

// --------------------------------------------------------------------------
// CContactView::DoAddFilteringObserverError
// --------------------------------------------------------------------------
//    
void CContactView::DoAddFilteringObserverError(
        MFilteredViewSupportObserver& aObserver, TInt aError )
    {
    aObserver.ContactViewError(*this, aError, EFalse);
    }

// --------------------------------------------------------------------------
// CContactView::PurgeObserverCallbacks
// --------------------------------------------------------------------------
//        
void CContactView::PurgeObserverCallbacks()
    {
    // Cancel all async observer callbacks
    iObserverOp->Purge();
    }

// --------------------------------------------------------------------------
// CContactView::SendViewStateEvent
// --------------------------------------------------------------------------
//        
void CContactView::SendViewStateEvent()
    {
    if ( iViewState != ENotReady )
        {
        // Cancel async operation to avoid double message sending
        // to observer in DoAddObserverL
        PurgeObserverCallbacks();
        
        void (MVPbkContactViewObserver::*obsNotifyFunc)(
                MVPbkContactViewBase&) = 
                    &MVPbkContactViewObserver::ContactViewReady;
        void (MFilteredViewSupportObserver::*filtObsNotifyFunc)(
            MParentViewForFiltering&) = 
                &MFilteredViewSupportObserver::ContactViewReadyForFiltering;
        
        if ( iViewState == ENotAvailable )
            {
            obsNotifyFunc = &MVPbkContactViewObserver::ContactViewUnavailable;
            filtObsNotifyFunc = 
            &MFilteredViewSupportObserver::ContactViewUnavailableForFiltering;
            }
        
        // Due to filtered view stack it must be ensured that internal
        // filtered views know first about changes because CRefineViews
        // have pointers to contacts that must not be invalidated.
        // On the other hand the events to external observers must come
        // first from lowest view in the view stack.
        // See MVPbkContactViewFiltering interface.
        VPbkEng::SendViewEventToObservers( *this, iFilteringObservers, 
            filtObsNotifyFunc, &MVPbkContactViewObserver::ContactViewError );
        VPbkEng::SendViewEventToObservers( *this, iObservers, 
            obsNotifyFunc, &MVPbkContactViewObserver::ContactViewError );
        VPbkEng::SendViewEventToObservers( *this, iFilteringObservers, 
            obsNotifyFunc, &MVPbkContactViewObserver::ContactViewError );
        }
    }

// --------------------------------------------------------------------------
// CContactView::SendViewStateEvent
// --------------------------------------------------------------------------
//        
void CContactView::SendViewErrorEvent( TInt aError )
    {
    // Cancel async operation to avoid double message sending
    // to observer in DoAddObserverL
    PurgeObserverCallbacks();
    
    /// Then report the failure to client so that in handle it.
    TBool errorIsNotified = EFalse;
    VPbkEng::SendEventToObservers( *this, aError, errorIsNotified,
        iObservers, &MVPbkContactViewObserver::ContactViewError );
    VPbkEng::SendEventToObservers( *this, aError, errorIsNotified,
        iFilteringObservers, &MVPbkContactViewObserver::ContactViewError );
    }

// --------------------------------------------------------------------------
// CContactView::LeaveIfIncorrectSortOrderL
// --------------------------------------------------------------------------
//
void CContactView::LeaveIfIncorrectSortOrderL( 
        const MVPbkFieldTypeList& aSortOrder )
    {
    if ( RemoteViewDefinition( *iViewDefinition ) )
        {
        // Get allowed sort orders via ECOM
        CVPbkSortOrderAcquirer::TSortOrderAcquirerParam param(
            iParentStore.MasterFieldTypeList() );
        CSortOrderAcquirerList* ecomList = 
                CSortOrderAcquirerList::NewLC(param);
    
        const TInt count = ecomList->Count();
        for ( TInt i = 0; i < count; ++i )
            {
            CVPbkSortOrderAcquirer& acquirer = ecomList->At( i );
            // Sort order is incorrect if
            // 1) acquirer is for this store
            // 2) this is a shared view and acquirer is for the
            //    same named view.
            // 3) aSortOrder is not same as acquirer's sort order.
            if ( acquirer.ApplySortOrderToStoreL( 
                    iParentStore.StoreProperties().Uri() ) &&
                 ecomList->FindInfo(acquirer)->DisplayName().CompareC(
                    RemoteViewName( *iViewDefinition ) ) == 0 &&
                 !VPbkFieldTypeList::IsSame( acquirer.SortOrder(), 
                    aSortOrder) )
                {
                User::Leave( KErrArgument );
                }
            }
        CleanupStack::PopAndDestroy( ecomList );
        }
    }
} // namespace VPbkSimStore

// End of File
