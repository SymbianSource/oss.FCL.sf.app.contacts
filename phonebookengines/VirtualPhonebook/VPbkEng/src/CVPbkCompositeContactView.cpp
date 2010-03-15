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
* Description:  Composite contact view.
*
*/


#include "CVPbkCompositeContactView.h"
#include <MVPbkFieldType.h>
#include <CVPbkSortOrder.h>
#include <CVPbkAsyncCallback.h>
#include <MVPbkContactLink.h>
#include "CVPbkEventArrayItem.h"
#include <VPbkError.h>

// Debugging headers
#include <VPbkProfile.h>
#include <VPbkDebug.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
    enum TPanic
        {
        EPanicLogic_CreateBookmarkLC = 3
        };

    void Panic(TPanic aPanic)
        {
        _LIT(KPanicCat, "CVPbkCompositeContactView");
        User::Panic(KPanicCat, aPanic);
        }
#endif // _DEBUG

/// Observer granulatiry
const TInt KObserverArrayGranularity = 4;

/////////////////////////////////////////////////////////////////////////////
// Event sending functions for different amount of parameters
//

// --------------------------------------------------------------------------
// SendEventToObservers
// --------------------------------------------------------------------------
//
template <class NotifyFunc>
void SendEventToObservers( MVPbkContactViewBase& aView,
        RPointerArray<MVPbkContactViewObserver>& aObservers,
        NotifyFunc aNotifyFunc )
    {
    const TInt count = aObservers.Count();
    for (TInt i = count - 1; i >= 0; --i)
        {
        MVPbkContactViewObserver* observer = aObservers[i];
        (observer->*aNotifyFunc)(aView);
        }
    }

// --------------------------------------------------------------------------
// SendEventToObservers
// --------------------------------------------------------------------------
//
template <class NotifyFunc, class ParamType1, class ParamType2>
void SendEventToObservers( MVPbkContactViewBase& aView,
        RPointerArray<MVPbkContactViewObserver>& aObservers,
        NotifyFunc aNotifyFunc, ParamType1 aParam1, const ParamType2& aParam2 )
    {
    const TInt count = aObservers.Count();
    for (TInt i = count - 1; i >= 0; --i)
        {
        MVPbkContactViewObserver* observer = aObservers[i];
        (observer->*aNotifyFunc)(aView, aParam1, aParam2);
        }
    }

} /// namespace

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::CSubViewData::~CSubViewData
// --------------------------------------------------------------------------
//
CVPbkCompositeContactView::CSubViewData::~CSubViewData()
    {
    delete iView;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::CVPbkCompositeContactView
// --------------------------------------------------------------------------
//
CVPbkCompositeContactView::CVPbkCompositeContactView() :
        iAsyncOperation( CActive::EPriorityStandard ),
        iObservers( KObserverArrayGranularity )
    {
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::~CVPbkCompositeContactView
// --------------------------------------------------------------------------
//
CVPbkCompositeContactView::~CVPbkCompositeContactView()
    {
    delete iCompositePolicy;
    delete iSortOrder;
    iSubViews.ResetAndDestroy();
    iObservers.Close();
    iContactMapping.Reset();
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::BaseConstructL
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::BaseConstructL
        (const MVPbkFieldTypeList& aSortOrder)
    {
    // Create sort order
    iSortOrder = CVPbkSortOrder::NewL(aSortOrder);

    // Always apply internal policy
    iCompositePolicy =
            CVPbkInternalCompositeViewPolicy::NewL( *this, iObservers );

    // External policy's view event buffering mechanism does not work when
    // native contact views reside in a separate process, therefore we
    // always apply internal policy.
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::AddSubViewL
// Adds subview to this composite view.
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::AddSubViewL(MVPbkContactViewBase* aSubView, TInt  aViewId)
    {
    CSubViewData* subViewData =
        new(ELeave) CSubViewData(CSubViewData::ENotKnown);
    CleanupStack::PushL(subViewData);
    User::LeaveIfError(iSubViews.Append(subViewData));
    CleanupStack::Pop(subViewData);

    subViewData->iView = aSubView;
    subViewData->iViewId = aViewId;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ActualContactCountL
// --------------------------------------------------------------------------
//
TInt CVPbkCompositeContactView::ActualContactCountL() const
    {
    const TInt subViewCount = iSubViews.Count();
    TInt contactCount = 0;
    for (TInt i = 0; i < subViewCount; ++i)
        {
        if ( iSubViews[i]->iState == CSubViewData::EReady )
            {
            if ( iSubViews[i]->iView->Type() == EVPbkCompositeView )
                {
                // The view can be safely casted because
                // CVPbkCompositeContactView is VPbkEng internal
                // and EVPbkCompositeView type view is always
                // derived from CVPbkCompositeContactView.
                contactCount += static_cast<CVPbkCompositeContactView*>(
                    iSubViews[i]->iView )->ActualContactCountL();
                }
            else
                {
                contactCount += iSubViews[i]->iView->ContactCountL();
                }
            }
        }
    return contactCount;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ApplyInternalCompositePolicyL
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::ApplyInternalCompositePolicyL()
    {
    if ( !iCompositePolicy || !iCompositePolicy->InternalPolicy() )
        {
        MVPbkCompositeContactViewPolicy* newPolicy =
                CVPbkInternalCompositeViewPolicy::NewL( *this, iObservers );
        delete iCompositePolicy;
        iCompositePolicy = newPolicy;
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::Type
// Returns view type.
// --------------------------------------------------------------------------
//
TVPbkContactViewType CVPbkCompositeContactView::Type() const
    {
    return EVPbkCompositeView;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ChangeSortOrderL
// Changes sort order.
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::ChangeSortOrderL
        (const MVPbkFieldTypeList& aSortOrder)
    {
    const TInt subViewCount = iSubViews.Count();
    for (TInt i = 0; i < subViewCount; ++i)
        {
        // All ready subviews go to unknown state because
        // they need to be re-sorted.
        // State is known again when an event comes.
        if (iSubViews[i]->iState == CSubViewData::EReady)
            {
            iSubViews[i]->iState = CSubViewData::ENotKnown;
            iSubViews[i]->iView->ChangeSortOrderL(aSortOrder);
            }
        }

    // Create new sort order and take it in use
    MVPbkFieldTypeList* sortOrder = CVPbkSortOrder::NewL(aSortOrder);
    delete iSortOrder;
    iSortOrder = sortOrder;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::SortOrder
// Returns current sort order.
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CVPbkCompositeContactView::SortOrder() const
    {
    return *iSortOrder;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::RefreshL
// Refreshes all subviews
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::RefreshL()
    {
    const TInt subViewCount = iSubViews.Count();
    for (TInt i = 0; i < subViewCount; ++i)
        {
        iSubViews[i]->iView->RefreshL();
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ContactCountL
// Returns contact count.
// --------------------------------------------------------------------------
//
TInt CVPbkCompositeContactView::ContactCountL() const
    {
    return iCompositePolicy->ContactCountL();
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ContactAtL
// Returns contact at aIndex.
// --------------------------------------------------------------------------
//
const MVPbkViewContact& CVPbkCompositeContactView::ContactAtL
        (TInt aIndex) const
    {
    __ASSERT_ALWAYS(
        aIndex >= 0, VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= iContactMapping.Count() )
        {
        User::Leave( KErrArgument );
        }

    const TContactMapping& mapping = iContactMapping[aIndex];
    MVPbkContactViewBase* view = iSubViews[mapping.iViewIndex]->iView;
    return view->ContactAtL( mapping.iContactIndex );
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::CreateLinkLC
// Creates link from a contact at aIndex.
// --------------------------------------------------------------------------
//
MVPbkContactLink* CVPbkCompositeContactView::CreateLinkLC(TInt aIndex) const
    {
    __ASSERT_ALWAYS(
        aIndex >= 0, VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= iContactMapping.Count() )
        {
        User::Leave( KErrArgument );
        }

    const TContactMapping& mapping = iContactMapping[aIndex];
    MVPbkContactViewBase* view = iSubViews[mapping.iViewIndex]->iView;
    return view->CreateLinkLC( mapping.iContactIndex );
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::IndexOfLinkL
// Returns index of aContactLink.
// --------------------------------------------------------------------------
//
TInt CVPbkCompositeContactView::IndexOfLinkL
        (const MVPbkContactLink& aContactLink) const
    {
    TIdentityRelation<TContactMapping> comparisonOperator
        (&CVPbkCompositeContactView::CompareMappings);
    TInt result = KErrNotFound;

    // Search the link from all the subviews
    const TInt subViewCount = iSubViews.Count();
    for (TInt i = 0; i < subViewCount; ++i)
        {
        // Check that the view is ready before using it.
        TInt index = KErrNotFound;
        if (iSubViews[i]->iState == CSubViewData::EReady)
            {
            index = iSubViews[i]->iView->IndexOfLinkL(aContactLink);
            }

        if (index != KErrNotFound)
            {
            // Establish contact mapping for view and contact index
            TContactMapping mapping;
            mapping.iViewIndex = i;
            mapping.iContactIndex = index;
            // Find the composite index by comparing the two mappings
            result = iContactMapping.Find(mapping, comparisonOperator);
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::AddObserverL
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::AddObserverL
        (MVPbkContactViewObserver& aObserver)
    {
    // Add the observer only if it is not already added
    TInt err( iObservers.InsertInAddressOrder(&aObserver) );
    if ( err != KErrNone && err != KErrAlreadyExists)
        {
        User::Leave( err );
        }

    TRAP(err,
        {
        VPbkEngUtils::MAsyncCallback* notifyObserver =
            VPbkEngUtils::CreateAsyncCallbackLC(
                *this,
                &CVPbkCompositeContactView::DoAddObserverL,
                &CVPbkCompositeContactView::AddObserverError,
                aObserver);
        iAsyncOperation.CallbackL(notifyObserver);
        CleanupStack::Pop(notifyObserver);
        });

    if (err != KErrNone)
        {
        RemoveObserver(aObserver);
        User::Leave(err);
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::RemoveObserver
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::RemoveObserver
        (MVPbkContactViewObserver& aObserver)
    {
    const TInt index = iObservers.FindInAddressOrder( &aObserver );
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::MatchContactStore
// Check does any of the subviews match given contact store.
// --------------------------------------------------------------------------
//
TBool CVPbkCompositeContactView::MatchContactStore
        (const TDesC& aContactStoreUri) const
    {
    const TInt count = iSubViews.Count();
    TBool result = EFalse;
    for (TInt i = 0; i < count && !result; ++i)
        {
        result = iSubViews[i]->iView->MatchContactStore(aContactStoreUri);
        }
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::MatchContactStoreDomain
// Check does any of the subviews match given contact store domain.
// --------------------------------------------------------------------------
//

 TBool CVPbkCompositeContactView::MatchContactStoreDomain
        (const TDesC& aContactStoreDomain) const
    {
    const TInt count = iSubViews.Count();
    TBool result = EFalse;
    for (TInt i = 0; i < count && !result; ++i)
        {
        result = iSubViews[i]->iView->MatchContactStoreDomain
            (aContactStoreDomain);
        }
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::CreateBookmarkLC
// Creates a bookmark for given index.
// --------------------------------------------------------------------------
//
MVPbkContactBookmark* CVPbkCompositeContactView::CreateBookmarkLC
        (TInt aIndex) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    __ASSERT_DEBUG(ContactCountL() > aIndex,
            Panic(EPanicLogic_CreateBookmarkLC));

    const TContactMapping& mapping = iContactMapping[aIndex];
    MVPbkContactViewBase* view = iSubViews[mapping.iViewIndex]->iView;
    return view->CreateBookmarkLC( mapping.iContactIndex );
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::IndexOfBookmarkL
// Returns the index of given bookmark.
// --------------------------------------------------------------------------
//
TInt CVPbkCompositeContactView::IndexOfBookmarkL
        (const MVPbkContactBookmark& aContactBookmark) const
    {
    TIdentityRelation<TContactMapping> comparisonOperator(
        &CVPbkCompositeContactView::CompareMappings);
    TInt result = KErrNotFound;

    // Search the bookmark from all the subviews
    const TInt subViewCount = iSubViews.Count();
    for (TInt i = 0; i < subViewCount; ++i)
        {
        // Check that the view is ready before using it.
        TInt index(KErrNotFound);
        if (iSubViews[i]->iState == CSubViewData::EReady)
            {
            index = iSubViews[i]->iView->IndexOfBookmarkL(aContactBookmark);
            }

        if (index != KErrNotFound)
            {
            // Establish contact mapping for view and contact index
            TContactMapping mapping;
            mapping.iViewIndex = i;
            mapping.iContactIndex = index;
            // Find the composite index by comparing the two mappings
            result = iContactMapping.Find(mapping, comparisonOperator);
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ViewFiltering
// --------------------------------------------------------------------------
//
MVPbkContactViewFiltering* CVPbkCompositeContactView::ViewFiltering()
    {
    // Composite views support filtering and is also a filtered view.
    // The subclass must implement the interface.
    return this;
    }

TAny* CVPbkCompositeContactView::ContactViewBaseExtension(TUid aExtensionUid)
    {
    MVPbkContactViewBaseChildAccessExtension* extChild = NULL;
    if ( aExtensionUid == TUid::Uid(KVPbkViewBaseExtChildAccess) )
        {
        extChild = this;
        }
    return extChild;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ContactViewReady
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::ContactViewReady(MVPbkContactViewBase& aView)
    {
    TRAPD( res, HandleContactViewReadyL( aView ) );
    if ( res != KErrNone )
        {
        // There was an error when building a composite. Reset composite
        // inform client that this view unavailable and there is an error
        ResetContactMapping();
        SendViewUnavailableEvent();
        SendViewErrorEvent( res, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::ContactViewUnavailable
        (MVPbkContactViewBase& aView)
    {
    TRAPD( res, HandleContactViewUnavailableL( aView ) );
    if ( res != KErrNone )
        {
        // There was an error when building a composite. Reset composite
        // inform client that this view unavailable and there is an error
        ResetContactMapping();
        SendViewUnavailableEvent();
        SendViewErrorEvent( res, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ContactAddedToView
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::ContactAddedToView
        (MVPbkContactViewBase& aView, TInt aIndex,
        const MVPbkContactLink& aContactLink)
    {
    TInt compositeIndex = HandleContactAddition(aView, aIndex);

    TRAPD( error, iCompositePolicy->HandleViewEventsL(
        CVPbkEventArrayItem::EAdded, aView, compositeIndex, aContactLink ) );
    if ( error != KErrNone )
        {
        // If error occurs reset policy
        iCompositePolicy->Reset();
        SendViewErrorEvent( error, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::ContactRemovedFromView
        (MVPbkContactViewBase& aView, TInt aIndex,
        const MVPbkContactLink& aContactLink)
    {
    TInt compositeIndex( KErrNotFound );
    compositeIndex = HandleContactRemoval(aView, aIndex);
    TRAPD( error, iCompositePolicy->HandleViewEventsL(
        CVPbkEventArrayItem::ERemoved, aView, compositeIndex,
            aContactLink ) );
    if ( error != KErrNone )
        {
        // If error occurs reset policy
        iCompositePolicy->Reset();
        SendViewErrorEvent( error, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ContactViewError
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::ContactViewError
        (MVPbkContactViewBase& /*aView*/, TInt aError, TBool aErrorNotified)
    {
    SendViewErrorEvent( aError, aErrorNotified );
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ContactViewObserverExtension
// --------------------------------------------------------------------------
//
TAny* CVPbkCompositeContactView::ContactViewObserverExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == KVPbkContactViewObserverExtension2Uid )
        {
        return static_cast<MVPbkContactViewObserverExtension*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::FilteredContactRemovedFromView
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::FilteredContactRemovedFromView(
		MVPbkContactViewBase& aView )
    {
    const TInt count = iObservers.Count();

    for( TInt i = 0; i < count; i++ )
        {
        MVPbkContactViewObserver* observer = iObservers[i];

        TAny* extension = observer->ContactViewObserverExtension(
              KVPbkContactViewObserverExtension2Uid );

        if( extension )
            {
            MVPbkContactViewObserverExtension* contactViewExtension =
                  static_cast<MVPbkContactViewObserverExtension*>( extension );

            if( contactViewExtension )
                {
                contactViewExtension->FilteredContactRemovedFromView( aView );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::UpdateFilterL
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::UpdateFilterL(
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    const TInt count = iSubViews.Count();
    for( TInt i = 0; i < count; ++i )
        {
        MVPbkContactViewFiltering* filtering =
            iSubViews[i]->iView->ViewFiltering();
        // Check if the subview supports filtering.
        if ( filtering )
            {
            // Filtering supported
            filtering->UpdateFilterL( aFindWords, aAlwaysIncludedContacts );
            // Set state to unknown. NOTE: this set so that the composite
            // becomes ready state when all subviews have notified something.
            // Otherwise the composite would notify its own observers every
            // time a subview notifies composite. This is not wanted because
            // client must get only one notification after update.
            // On the other hand a client can still use this view while it's
            // updating itself so in that point of view the composite is not
            // in unknown state but in update state.
            iSubViews[i]->iState = CSubViewData::ENotKnown;
            }
        }
    }

TInt CVPbkCompositeContactView::ChildViewCount() const
    {
    return iSubViews.Count();
    }

MVPbkContactViewBase& CVPbkCompositeContactView::ChildViewAt( TInt aIndex )
    {
    return *iSubViews[aIndex]->iView;
    }

MVPbkContactViewBase* CVPbkCompositeContactView::GetChildViewById( TInt aId)
    {
    MVPbkContactViewBase* view = NULL;
    const TInt count = iSubViews.Count();
    for (TInt i = 0; i < count; ++i)
        {
        view=iSubViews[i]->iView;
        if (iSubViews[i]->iViewId == aId)
           {
           return view;
           }
        }
    return NULL;
    }

TInt CVPbkCompositeContactView::GetViewId()
    {
     return iViewId;
    }
// --------------------------------------------------------------------------
// CVPbkCompositeContactView::SendViewReadyEvent
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::SendViewReadyEvent()
    {
    VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING(
        "CVPbkCompositeContactView::SendViewReadyEvent(0x%x)"), this );
    // Cancel operation to avoid unnecessary event sending in DoAddObserverL
    iAsyncOperation.Purge();
    SendEventToObservers( *this, iObservers,
        &MVPbkContactViewObserver::ContactViewReady );
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::SendViewUnavailableEvent
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::SendViewUnavailableEvent()
    {
    VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING(
        "CVPbkCompositeContactView::SendViewUnavailableEvent(0x%x)"), this );
    // Cancel operation to avoid unnecessary event sending in DoAddObserverL
    iAsyncOperation.Purge();
    SendEventToObservers( *this, iObservers,
        &MVPbkContactViewObserver::ContactViewUnavailable );
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::SendViewErrorEvent
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::SendViewErrorEvent( TInt aError,
        TBool aErrorNotified )
    {
    SendEventToObservers(*this, iObservers,
        &MVPbkContactViewObserver::ContactViewError, aError, aErrorNotified);
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::CompareMappings
// --------------------------------------------------------------------------
//
TBool CVPbkCompositeContactView::CompareMappings
        (const TContactMapping& aLhs, const TContactMapping& aRhs)
    {
    // Mappings match if both the contact index and the view index match
    return (aLhs.iContactIndex == aRhs.iContactIndex &&
            aLhs.iViewIndex == aRhs.iViewIndex);
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::IsCompositeReady
// --------------------------------------------------------------------------
//
TBool CVPbkCompositeContactView::IsCompositeReady() const
    {
    // Composite view is ready if states of all sub views are known and
    // at least one of them is ready.
    return AllSubViewsKnown() && AnySubViewReady();
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::CompositePolicy
// --------------------------------------------------------------------------
//
MVPbkCompositeContactViewPolicy&
        CVPbkCompositeContactView::CompositePolicy() const
    {
    return *iCompositePolicy;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::FindSubViewIndex
// --------------------------------------------------------------------------
//
TInt CVPbkCompositeContactView::FindSubViewIndex
        (MVPbkContactViewBase& aView) const
    {
    TInt result = KErrNotFound;
    const TInt count = iSubViews.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iSubViews[i]->iView == &aView)
            {
            result = i;
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::AllSubViewsKnown
// Returns true if all the subviews are known.
// --------------------------------------------------------------------------
//
TBool CVPbkCompositeContactView::AllSubViewsKnown() const
    {
    TBool ret = ETrue;
    const TInt count = iSubViews.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iSubViews[i]->iState == CSubViewData::ENotKnown)
            {
            ret = EFalse;
            break;
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::AnySubViewReady
// Returns true if any of the subviews is ready.
// --------------------------------------------------------------------------
//
TBool CVPbkCompositeContactView::AnySubViewReady() const
    {
    TBool ret = EFalse;
    const TInt count = iSubViews.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iSubViews[i]->iState == CSubViewData::EReady)
            {
            ret = ETrue;
            break;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::ResetContactMapping
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::ResetContactMapping()
    {
    iContactMapping.Reset();
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::RemoveContactMappingsFromView
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::RemoveContactMappingsFromView(
        MVPbkContactViewBase& aView )
    {
    TInt subviewIndex = FindSubViewIndex( aView );
    if ( subviewIndex != KErrNotFound )
        {
        const TInt count = iContactMapping.Count();
        for ( TInt i = count - 1; i >= 0; --i )
            {
            if ( iContactMapping[i].iViewIndex == subviewIndex )
                {
                iContactMapping.Remove( i );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::HandleContactViewReadyL
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::HandleContactViewReadyL(
        MVPbkContactViewBase& aView )
    {
    VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING(
        "CVPbkCompositeContactView::HandleContactViewReadyL(0x%x)"), this );

    // Find matching subview and set its state to ready
    TInt subViewIndex = FindSubViewIndex(aView);
    if (subViewIndex != KErrNotFound)
        {
        iSubViews[subViewIndex]->iState = CSubViewData::EReady;
        }

    // Check that composite's sort order up to date with subviews.
    UpdateSortOrderL();

    // If composite is ready, build view mapping and notify observers
    if ( IsCompositeReady() )
        {
        VPBK_PROFILE_START(VPbkProfile::ECompositeContactViewMapping);
        DoBuildContactMappingL();
        VPBK_PROFILE_END(VPbkProfile::ECompositeContactViewMapping);
        SendViewReadyEvent();
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::HandleContactViewUnavailableL
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::HandleContactViewUnavailableL(
        MVPbkContactViewBase& aView )
    {
    VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING(
        "CVPbkCompositeContactView::ContactViewUnavailable(0x%x)"), this );

    // Find matching subview and set its state
    TInt subViewIndex = FindSubViewIndex(aView);
    if (subViewIndex != KErrNotFound)
        {
        iSubViews[subViewIndex]->iState = CSubViewData::EUnavailable;
        }

    if ( IsCompositeReady() )
        {
        // Composite is still in ready state. The mapping must be updated
        // because one of the subviews became unavailable.
        DoBuildContactMappingL();
        // Call observers so that they know about update.
        SendViewReadyEvent();
        }
    else
        {
        if ( AllSubViewsKnown() )
            {
            // Composite is unavailable. Notify observers.
            ResetContactMapping();
            SendViewUnavailableEvent();
            }
        else
            {
            // Composite is still under construction.

            // Dont't send event here because the state of the composite is
            // not yet known and sending event can cause problems for client
            // e.g unwanted changes in the UI state.
            RemoveContactMappingsFromView( aView );
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::CVPbkCompositeContactView
// --------------------------------------------------------------------------
//
TInt CVPbkCompositeContactView::HandleContactRemoval
        ( MVPbkContactViewBase& aView, TInt aIndex )
    {
    TInt index = KErrNotFound;

    // Find correct subview
    TInt subViewIndex = FindSubViewIndex( aView );
    if ( subViewIndex != KErrNotFound )
        {
        TIdentityRelation<TContactMapping> comparisonOperator
            ( &CVPbkCompositeContactView::CompareMappings );
        // Establish contact mapping for view and contact index
        TContactMapping mapping;
        mapping.iViewIndex = subViewIndex;
        mapping.iContactIndex = aIndex;
        // Find the composite index by comparing the two mappings
        index = iContactMapping.Find( mapping, comparisonOperator );

        if ( index > KErrNotFound )
            {
            // Loop through the rest of the contacts of the subview
            // and modify their global contact mapping information
            for (TInt i = index; i < iContactMapping.Count(); ++i)
                {
                if (iContactMapping[i].iViewIndex == subViewIndex)
                    {
                    // Subtract one because one contact was deleted
                    // from the list before the current index
                    --iContactMapping[i].iContactIndex;
                    }
                }
            // Finally remove the deleted index from global mapping
            iContactMapping.Remove( index );
            }
        else
            {
            // mapping inconsistent with the underlying view, rebuild it
            DoBuildContactMappingL();
            }
        }
    return index;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::HandleContactAddition
// --------------------------------------------------------------------------
//
TInt CVPbkCompositeContactView::HandleContactAddition
        (MVPbkContactViewBase& aView, TInt aIndex)
    {
    TInt index = KErrNotFound;

    // Find correct subview
    TInt subViewIndex = FindSubViewIndex(aView);
    if (subViewIndex != KErrNotFound)
        {
        TRAPD( err, index = DoHandleContactAdditionL
            ( subViewIndex, aIndex ) );
        if (err != KErrNone)
            {
            index = err;
            }
        else
        	{
        	// We have to fix the indexes of all the succeeding
            // contacts in the view where the contact addition took place
            for ( TInt i = index + 1; i < iContactMapping.Count(); ++i )
                {
                if (iContactMapping[i].iViewIndex == subViewIndex)
                    {
                    ++iContactMapping[i].iContactIndex;
                    }
                }
        	}
        }
    return index;
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::DoAddObserverL
// Notifies composite view readiness to observer.
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::DoAddObserverL
        (MVPbkContactViewObserver& aObserver)
    {
    // Check if aObserver is still observer of this view and the view is ready.
    // View is considered to be ready if all subviews are ready (iIsReady)
    // or if there are no subviews
    if ( iObservers.FindInAddressOrder( &aObserver ) != KErrNotFound )
        {
        if( IsCompositeReady() )
            {
            // If this view is ready and there was no error tell it to the observer
            aObserver.ContactViewReady(*this);
            }
        else if ( AllSubViewsKnown() )
            {
            // All subviews are known but the composite is not ready ->
            // view is unavailable.
            aObserver.ContactViewUnavailable( *this );
            }
        // If this view was not ready and there was no error, observer will
        // be called back in HandleContactViewEvent
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::AddObserverError
// Notifies composite view problems to observer.
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::AddObserverError
        (MVPbkContactViewObserver& aObserver, TInt aError)
    {
    // Check if aObserver is still observer of this view
    if (iObservers.FindInAddressOrder( &aObserver ) != KErrNotFound)
        {
        aObserver.ContactViewError(*this, aError, EFalse);
        }
    }

// --------------------------------------------------------------------------
// CVPbkCompositeContactView::UpdateSortOrderL
// --------------------------------------------------------------------------
//
void CVPbkCompositeContactView::UpdateSortOrderL()
    {
    const MVPbkFieldTypeList* sortOrder = NULL;

    // Inspect the sort order of subviews. If all ready subviews have
    // same sort order then update composite's sort order.
    // This is done because subviews can be shared views whose sort order
    // is not changed to client's sort order when a new handle is created.
    TBool subViewsHaveSameOrder = ETrue;
    const TInt count = iSubViews.Count();
    for ( TInt i = 0; i < count && subViewsHaveSameOrder; ++i )
        {
        if ( iSubViews[i]->iState == CSubViewData::EReady )
            {
            const MVPbkFieldTypeList& curViewsOrder =
                    iSubViews[i]->iView->SortOrder();
            if ( sortOrder )
                {
                subViewsHaveSameOrder =
                    VPbkFieldTypeList::IsSame( *sortOrder, curViewsOrder );
                }
            sortOrder = &curViewsOrder;
            }
        }

    if ( subViewsHaveSameOrder && sortOrder &&
         !VPbkFieldTypeList::IsSame( *sortOrder, *iSortOrder ) )
        {
        CVPbkSortOrder* newOrder = CVPbkSortOrder::NewL( *sortOrder );
        delete iSortOrder;
        iSortOrder = newOrder;
        }
    }

void CVPbkCompositeContactView::SetViewId(TInt aViewId)
    {
    iViewId = aViewId;
    }
// --------------------------------------------------------------------------
// CVPbkExternalCompositeViewPolicy::CVPbkExternalCompositeViewPolicy
// --------------------------------------------------------------------------
//
CVPbkExternalCompositeViewPolicy::CVPbkExternalCompositeViewPolicy(
        CVPbkCompositeContactView& aCompositeView,
        RPointerArray<MVPbkContactViewObserver>& aObservers )
        :   iCompositeView( aCompositeView ),
            iObservers( aObservers )
    {
    }

// --------------------------------------------------------------------------
// CVPbkExternalCompositeViewPolicy::NewL
// --------------------------------------------------------------------------
//
CVPbkExternalCompositeViewPolicy* CVPbkExternalCompositeViewPolicy::NewL(
        CVPbkCompositeContactView& aCompositeView,
        RPointerArray<MVPbkContactViewObserver>& aObservers )
    {
    CVPbkExternalCompositeViewPolicy* self =
        new ( ELeave ) CVPbkExternalCompositeViewPolicy( aCompositeView,
            aObservers );
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkExternalCompositeViewPolicy::~CVPbkExternalCompositeViewPolicy
// --------------------------------------------------------------------------
//
CVPbkExternalCompositeViewPolicy::~CVPbkExternalCompositeViewPolicy()
    {
    iEventArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CVPbkExternalCompositeViewPolicy::HandleViewEventsL
// --------------------------------------------------------------------------
//
void CVPbkExternalCompositeViewPolicy::HandleViewEventsL(
        CVPbkEventArrayItem::TViewEventType aEvent,
        MVPbkContactViewBase& /*aSubview*/,
        TInt aIndex, const
        MVPbkContactLink& aContactLink )
    {
    // Append all view events to proper event array.
    CVPbkEventArrayItem* item =
        CVPbkEventArrayItem::NewLC( aIndex, aContactLink, aEvent );

    iEventArray.AppendL( item );
    CleanupStack::Pop(); // item

    // Check if the composite is up to date. Note that if the underlying
    // native store view resides in a separate process or thread,
    // this check may "accidentally" pass whilst the native view is still
    // updating itself. This means that the composite might fall out of sync
    // anytime after this check and it will then mean severe problems to the
    // client, since if it asks for ContactCountL the answer will
    // be zero, as the sync check is implemented in
    // CVPbkExternalCompositeViewPolicy::ContactCountL too.
    // The count of zero might be in severe contradiction with the
    // view events the client just received.
    // If the native view resides in the  same process, there are no risks.
    if (iCompositeView.CompositeContactCountL() ==
            iCompositeView.ActualContactCountL() )
        {
        // Composite mapping is valid according to contact counts. Flush
        // the event cache.
        TInt eventCount( iEventArray.Count() );

        for( TInt i = 0; i < eventCount; ++i )
            {
            if ( iEventArray[ i ]->Event() == CVPbkEventArrayItem::ERemoved )
                {
                SendEventToObservers( iCompositeView, iObservers,
                    &MVPbkContactViewObserver::ContactRemovedFromView,
                    iEventArray[ i ]->Index(),
                    *iEventArray[ i ]->Link() );
                }
            else
                {
                SendEventToObservers( iCompositeView, iObservers,
                    &MVPbkContactViewObserver::ContactAddedToView,
                    iEventArray[ i ]->Index(),
                    *iEventArray[ i ]->Link() );
                }
            }
        iEventArray.ResetAndDestroy();
        }
    }

// --------------------------------------------------------------------------
// CVPbkExternalCompositeViewPolicy::Reset
// --------------------------------------------------------------------------
//
void CVPbkExternalCompositeViewPolicy::Reset()
    {
    iEventArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CVPbkExternalCompositeViewPolicy::ContactCountL
// --------------------------------------------------------------------------
//
TInt CVPbkExternalCompositeViewPolicy::ContactCountL() const
    {
    TInt compositeCount = iCompositeView.CompositeContactCountL();

    if ( compositeCount != iCompositeView.ActualContactCountL() )
        {
        // If composite count is different as the contact count in subviews
        // it means that composite hasn't got all view events yet and its
        // contact mapping is not in valid state. It's too heavy to build
        // the whole mapping so zero is returned to inform client that
        // it must not call ContactAtL because it can panic.
        compositeCount = 0;
        }
    return compositeCount;
    }

// --------------------------------------------------------------------------
// CVPbkExternalCompositeViewPolicy::InternalPolicy
// --------------------------------------------------------------------------
//
TBool CVPbkExternalCompositeViewPolicy::InternalPolicy() const
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CVPbkInternalCompositeViewPolicy::CVPbkInternalCompositeViewPolicy
// --------------------------------------------------------------------------
//
CVPbkInternalCompositeViewPolicy::CVPbkInternalCompositeViewPolicy(
        CVPbkCompositeContactView& aCompositeView,
        RPointerArray<MVPbkContactViewObserver>& aObservers )
        :   iCompositeView( aCompositeView ),
            iObservers( aObservers )
    {
    }

// --------------------------------------------------------------------------
// CVPbkInternalCompositeViewPolicy::NewL
// --------------------------------------------------------------------------
//
CVPbkInternalCompositeViewPolicy* CVPbkInternalCompositeViewPolicy::NewL(
        CVPbkCompositeContactView& aCompositeView,
        RPointerArray<MVPbkContactViewObserver>& aObservers )
    {
    CVPbkInternalCompositeViewPolicy* self =
        new ( ELeave ) CVPbkInternalCompositeViewPolicy( aCompositeView,
            aObservers );
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkInternalCompositeViewPolicy::HandleViewEventsL
// --------------------------------------------------------------------------
//
void CVPbkInternalCompositeViewPolicy::HandleViewEventsL(
        CVPbkEventArrayItem::TViewEventType aEvent,
        MVPbkContactViewBase& /*aSubview*/,
        TInt aIndex, const
        MVPbkContactLink& aContactLink )
    {
    if ( aEvent == CVPbkEventArrayItem::ERemoved )
        {
        SendEventToObservers( iCompositeView, iObservers,
            &MVPbkContactViewObserver::ContactRemovedFromView,
            aIndex,
            aContactLink );
        }
    else
        {
        SendEventToObservers( iCompositeView, iObservers,
            &MVPbkContactViewObserver::ContactAddedToView,
            aIndex,
            aContactLink );
        }
    }

// --------------------------------------------------------------------------
// CVPbkInternalCompositeViewPolicy::Reset
// --------------------------------------------------------------------------
//
void CVPbkInternalCompositeViewPolicy::Reset()
    {
    // No cached data to reset
    }

// --------------------------------------------------------------------------
// CVPbkInternalCompositeViewPolicy::ContactCountL
// --------------------------------------------------------------------------
//
TInt CVPbkInternalCompositeViewPolicy::ContactCountL() const
    {
    return iCompositeView.CompositeContactCountL();
    }

// --------------------------------------------------------------------------
// CVPbkInternalCompositeViewPolicy::InternalPolicy
// --------------------------------------------------------------------------
//
TBool CVPbkInternalCompositeViewPolicy::InternalPolicy() const
    {
    return ETrue;
    }

// End of File
