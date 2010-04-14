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
* Description:  Contacts Model store filtered contact view implementation.
*
*/


// INCLUDES
#include "CFindViewBase.h"

// VPbkCntModel
#include "CContactStore.h"
#include "CRefineView.h"
#include "CViewBase.h"
#include "CContactLink.h"
#include "CContactBookmark.h"

// VPbkEng
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactFieldIterator.h>
#include <VPbkError.h>
#include <CVPbkAsyncCallback.h>
#include <MVPbkContactNameConstructionPolicy.h>
#include <MVPbkContactFindPolicy.h>
#include <VPbkSendEventUtility.h>

// System includes
#include <cntviewbase.h>

namespace{
    #ifdef _DEBUG
        enum TPanic
            {
            EPreCond_SetFindStringsL = 1
            };

        void Panic(TPanic aPanic)
            {
            _LIT(KPanicCat, "CFindViewBase");
            User::Panic(KPanicCat, aPanic);
            }
    #endif // _DEBUG
} //namespace

namespace VPbkCntModel {

// CONSTANTS
const TInt KGranularity( 4 );

// --------------------------------------------------------------------------
// CCompareView::CCompareView
// --------------------------------------------------------------------------
//
CCompareView::CCompareView( const CContactDatabase& aDb ) :
        CContactViewBase( aDb )
    {
    }

// --------------------------------------------------------------------------
// CCompareView::CompareFieldsL
// --------------------------------------------------------------------------
//
TInt CCompareView::CompareFieldsL( const ::CViewContact &aFirst,
            const ::CViewContact &aSecond )
    {
    return CContactViewBase::CompareFieldsL( aFirst, aSecond );
    }

// --------------------------------------------------------------------------
// CFindViewBase::CFindViewBase
// --------------------------------------------------------------------------
//
CFindViewBase::CFindViewBase(
        MParentViewForFiltering& aParentView,
        CViewBase& aBaseView,
        TBool aOwnsContacts ) :
            iParentView( aParentView ),
            iBaseView( aBaseView ),
            iOwnsContacts( aOwnsContacts ),
            iDestroyed( NULL )
    {
    }

// --------------------------------------------------------------------------
// CFindViewBase::BaseConstructL
// --------------------------------------------------------------------------
//
void CFindViewBase::BaseConstructL( const MDesCArray& aFindStrings,
        MVPbkContactFindPolicy& aFindPolicy,
        MVPbkContactViewObserver& aExternalViewObserver )
    {
    iObserverOp =
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>::NewL();
    iFilterObsOp =
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>::NewL();

    iFindStrings = new(ELeave)CDesCArrayFlat( KGranularity );
    SetFindStringsL( aFindStrings );

    // Set contact find policy
    iContactFindPolicy = &aFindPolicy;

    // Add the external observer
    iObservers.AppendL( &aExternalViewObserver );

    //Create field type list
    iFieldTypeRefsList = CVPbkFieldTypeRefsList::NewL();

    iViewContact = CViewContact::NewL( iBaseView, SortOrder() );
    }

// --------------------------------------------------------------------------
// CFindViewBase::~CFindViewBase
// --------------------------------------------------------------------------
//
CFindViewBase::~CFindViewBase()
    {
    delete iObserverOp;
    delete iFilterObsOp;
    ResetContacts();
    iMatchedContacts.Close();
    iParentView.RemoveFilteringObserver( *this );
    iObservers.Close();
    iFilteringObservers.Close();

    delete iViewContact;
    delete iFieldTypeRefsList;
    delete iFindStrings;
    if ( iDestroyed )
        {
        *iDestroyed = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::ActivateContactMatchL
// --------------------------------------------------------------------------
//
void CFindViewBase::ActivateContactMatchL()
    {
    // Remove first if this called second time when updating.
    iParentView.RemoveFilteringObserver( *this );
    // This will cause an asynchrnous view event to this view from the
    // parent view.
    iParentView.AddFilteringObserverL( *this );
    }

// --------------------------------------------------------------------------
// CFindViewBase::ParentObject
// --------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CFindViewBase::ParentObject() const
    {
    return iParentView.ParentObject();
    }

// --------------------------------------------------------------------------
// CFindViewBase::RefreshL
// --------------------------------------------------------------------------
//
void CFindViewBase::RefreshL()
    {
    iParentView.RefreshL();
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactCountL
// --------------------------------------------------------------------------
//
TInt CFindViewBase::ContactCountL() const
    {
    return iMatchedContacts.Count();
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactAtL
// --------------------------------------------------------------------------
//
const MVPbkViewContact& CFindViewBase::ContactAtL(
        TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );

    if ( aIndex >= ContactCountL() )
        {
        User::Leave( KErrArgument );
        }

    const ::CViewContact& viewContact = *iMatchedContacts[ aIndex ];
    iViewContact->SetViewContact(viewContact);
    return *iViewContact;
    }

// --------------------------------------------------------------------------
// CFindViewBase::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CFindViewBase::CreateLinkLC(
        TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );

    if ( aIndex >= ContactCountL() )
        {
        User::Leave( KErrArgument );
        }

    TContactItemId contactId = iMatchedContacts[aIndex]->Id();
    return CContactLink::NewLC( iBaseView.Store(), contactId );
    }

// --------------------------------------------------------------------------
// CFindViewBase::IndexOfLinkL
// --------------------------------------------------------------------------
//
TInt CFindViewBase::IndexOfLinkL(
        const MVPbkContactLink& aContactLink ) const
    {
    if (&aContactLink.ContactStore() == &iParentView.ContactStore())
        {
        const CContactLink& link =
            static_cast<const CContactLink&>(aContactLink);
        const TInt count( iMatchedContacts.Count() );
        for ( TInt i(0); i < count; ++i )
            {
            if ( iMatchedContacts[ i ]->Id() == link.ContactId() )
                {
                return i;
                }
            }
        }

    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CFindViewBase::Type
// --------------------------------------------------------------------------
//
TVPbkContactViewType CFindViewBase::Type() const
    {
    return iParentView.Type();
    }

// --------------------------------------------------------------------------
// CFindViewBase::ChangeSortOrderL
// --------------------------------------------------------------------------
//
void CFindViewBase::ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder)
    {
    iParentView.ChangeSortOrderL( aSortOrder );
    }

// --------------------------------------------------------------------------
// CFindViewBase::SortOrder
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CFindViewBase::SortOrder() const
    {
    return iParentView.SortOrder();
    }

// --------------------------------------------------------------------------
// CFindViewBase::AddObserverL
// --------------------------------------------------------------------------
//
void CFindViewBase::AddObserverL( MVPbkContactViewObserver& aObserver )
    {
    CVPbkAsyncObjectCallback<MVPbkContactViewObserver>* callback =
        VPbkEngUtils::CreateAsyncObjectCallbackLC(
            *this,
            &CFindViewBase::DoAddObserver,
            &CFindViewBase::DoAddObserverError,
            aObserver);
    iObserverOp->CallbackL( callback );
    CleanupStack::Pop( callback );

    // Insert to first position because events are send in reverse order.
    iObservers.InsertL( &aObserver, 0 );
    }

// --------------------------------------------------------------------------
// CFindViewBase::RemoveObserver
// --------------------------------------------------------------------------
//
void CFindViewBase::RemoveObserver(MVPbkContactViewObserver& aObserver)
    {
    const TInt index = iObservers.Find( &aObserver );
    if ( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::MatchContactStore
// --------------------------------------------------------------------------
//
TBool CFindViewBase::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iParentView.MatchContactStore( aContactStoreUri );
    }

// --------------------------------------------------------------------------
// CFindViewBase::MatchContactStoreDomain
// --------------------------------------------------------------------------
//
TBool CFindViewBase::MatchContactStoreDomain(const TDesC& aContactStoreDomain) const
    {
    return iParentView.MatchContactStoreDomain( aContactStoreDomain );
    }

// --------------------------------------------------------------------------
// CFindViewBase::CreateBookmarkLC
// --------------------------------------------------------------------------
//
MVPbkContactBookmark* CFindViewBase::CreateBookmarkLC(
        TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );

    TContactItemId contactId = iMatchedContacts[aIndex]->Id();
    // Link implements also the bookmark interface in this store
    return CContactBookmark::NewLC( contactId, iBaseView.Store() );
    }

// --------------------------------------------------------------------------
// CFindViewBase::IndexOfBookmarkL
// --------------------------------------------------------------------------
//
TInt CFindViewBase::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark ) const
    {
    // Bookmark is implemented in this store
    const CContactBookmark* bookmark =
        dynamic_cast<const CContactBookmark*>(&aContactBookmark);
    if (bookmark &&
        &bookmark->ContactStore() == &iParentView.ContactStore() )
        {
        const TInt count( iMatchedContacts.Count() );
        for ( TInt i(0); i < count; ++i )
            {
            if ( iMatchedContacts[ i ]->Id() == bookmark->ContactId() )
                {
                return i;
                }
            }
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CFindViewBase::AddFilteringObserverL
// --------------------------------------------------------------------------
//
void CFindViewBase::AddFilteringObserverL(
        MFilteredViewSupportObserver& aObserver )
    {
    // Insert observer in callback function. That ensures that the observer
    // will always get the event asynchronously.

    CVPbkAsyncObjectCallback<MFilteredViewSupportObserver>* callback =
        VPbkEngUtils::CreateAsyncObjectCallbackLC(
            *this,
            &CFindViewBase::DoAddFilteringObserverL,
            &CFindViewBase::DoAddFilteringObserverError,
            aObserver);

    iFilterObsOp->CallbackL( callback );
    CleanupStack::Pop( callback );
    }

// --------------------------------------------------------------------------
// CFindViewBase::RemoveFilteringObserver
// --------------------------------------------------------------------------
//
void CFindViewBase::RemoveFilteringObserver(
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
// CFindViewBase::ViewFiltering
// --------------------------------------------------------------------------
//
MVPbkContactViewFiltering* CFindViewBase::ViewFiltering()
    {
    // This view supports further filtering.
    return this;
    }

// --------------------------------------------------------------------------
// CFindViewBase::CreateFilteredViewLC
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CFindViewBase::CreateFilteredViewLC(
        MVPbkContactViewObserver& aObserver,
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* /*aAlwaysIncludedContacts*/ )
    {
    // NOTE: aAlwaysIncludedContacts is commented because in VPbkCntModel
    // filtered views are linked. CFindView->CRefineView->CRefineView etc.
    // Only the CFindView saves the always included contacts and they
    // are used also by all CRefineView instances through
    // MAlwaysIncludedContacts interface.
    CRefineView* refineView = CRefineView::NewLC( aFindWords, *this,
        iBaseView, aObserver, *iContactFindPolicy,
        *this );
    refineView->ActivateContactMatchL();
    return refineView;
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactViewReady
// --------------------------------------------------------------------------
//
void CFindViewBase::ContactViewReady(
        MVPbkContactViewBase& /*aView*/ )
    {
    SendViewStateEventToObservers();
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CFindViewBase::ContactViewUnavailable(
        MVPbkContactViewBase& /*aView*/ )
    {
    SendViewStateEventToObservers();
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactAddedToView
// --------------------------------------------------------------------------
//
void CFindViewBase::ContactAddedToView(
        MVPbkContactViewBase& aView,
        TInt aIndex,
        const MVPbkContactLink& aContactLink )
    {
    TRAPD( error, HandleContactAddedToViewL( aView, aIndex, aContactLink ) );
    if ( error != KErrNone )
        {
        ContactViewError( aView, error, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CFindViewBase::ContactRemovedFromView(
        MVPbkContactViewBase& aView,
        TInt aIndex,
        const MVPbkContactLink& aContactLink )
    {
    TRAPD( error, HandleContactRemovedFromViewL( aView, aIndex, aContactLink ) );
    if ( error != KErrNone )
        {
        ContactViewError( aView, error, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactViewError
// --------------------------------------------------------------------------
//
void CFindViewBase::ContactViewError( MVPbkContactViewBase& /*aView*/,
        TInt aError, TBool aErrorNotified )
    {
    iViewReady = EFalse;
    ResetContacts();
    SendContactViewErrorEvent( aError, aErrorNotified );
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactViewReadyForFiltering
// --------------------------------------------------------------------------
//
void CFindViewBase::ContactViewReadyForFiltering(
        MParentViewForFiltering& aView )
    {
    TRAPD( error, DoContactViewReadyForFilteringL( aView ) );
    if ( error != KErrNone )
        {
        ContactViewError( aView, error, EFalse );
        }
    else
        {
        ContactViewReady( aView );
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactViewUnavailableForFiltering
// --------------------------------------------------------------------------
//
void CFindViewBase::ContactViewUnavailableForFiltering(
        MParentViewForFiltering& /*aView*/ )
    {
    iViewReady = EFalse;
    ResetContacts();
    VPbkEng::SendViewEventToObservers( *this, iFilteringObservers,
        &MFilteredViewSupportObserver::ContactViewUnavailableForFiltering,
        &MVPbkContactViewObserver::ContactViewError );
    }

// --------------------------------------------------------------------------
// CFindViewBase::IsMatchL
// --------------------------------------------------------------------------
//
TBool CFindViewBase::IsMatchL( const MVPbkViewContact& aViewContact )
    {
    return iContactFindPolicy->MatchContactNameL( *iFindStrings,
                                                  aViewContact );
    }

// --------------------------------------------------------------------------
// CFindViewBase::FindStrings
// --------------------------------------------------------------------------
//
const MDesCArray& CFindViewBase::FindStrings() const
    {
    return *iFindStrings;
    }

// -------------------------------------------------------------------------
// CFindViewBase::SetFindStringsL
// --------------------------------------------------------------------------
//
void CFindViewBase::SetFindStringsL( const MDesCArray& aFindStrings )
    {
    __ASSERT_DEBUG( iFindStrings, Panic( EPreCond_SetFindStringsL ) );

    iFindStrings->Reset();
    const TInt count = aFindStrings.MdcaCount();
    for ( TInt i = 0; i < count; ++i )
        {
        iFindStrings->AppendL( aFindStrings.MdcaPoint( i ) );
        }
    }

// -------------------------------------------------------------------------
// CFindViewBase::SendViewStateEventToObservers
// --------------------------------------------------------------------------
//
void CFindViewBase::SendViewStateEventToObservers()
    {
    // Cancel any new AddObserverL callbacks to avoid duplicate event
    // sending.
    iObserverOp->Purge();

    void (MVPbkContactViewObserver::*notifyFunc)(MVPbkContactViewBase&);
    notifyFunc = &MVPbkContactViewObserver::ContactViewReady;

    if ( !iViewReady )
        {
        notifyFunc = &MVPbkContactViewObserver::ContactViewUnavailable;
        }

    VPbkEng::SendViewEventToObservers( *this, iObservers,
        notifyFunc, &MVPbkContactViewObserver::ContactViewError );
    VPbkEng::SendViewEventToObservers( *this, iFilteringObservers,
        notifyFunc, &MVPbkContactViewObserver::ContactViewError );
    }

// -------------------------------------------------------------------------
// CFindViewBase::DoContactViewReadyForFilteringL
// --------------------------------------------------------------------------
//
void CFindViewBase::DoContactViewReadyForFilteringL(
        MVPbkContactViewBase& /*aView*/ )
    {
    ContactMatchL();
    iViewReady = ETrue;
    // Send ContactViewReadyForFiltering event so that other views in
    // stack can rebuild themselves before external observer are notified.
    VPbkEng::SendViewEventToObservers( *this, iFilteringObservers,
        &MFilteredViewSupportObserver::ContactViewReadyForFiltering,
        &MVPbkContactViewObserver::ContactViewError );
    }

// --------------------------------------------------------------------------
// CFindViewBase::HandleContactAddedToViewL
// --------------------------------------------------------------------------
//
void CFindViewBase::HandleContactAddedToViewL( MVPbkContactViewBase& aView,
        TInt aIndex, const MVPbkContactLink& aContactLink )
    {
    if ( &iParentView == &aView )
        {
        // Let sub class do the addition if needed
        DoContactAddedToViewL( aView, aIndex, aContactLink,
            iMatchedContacts );

        // Get the index of the new contact in this view.
        TInt index( IndexOfLinkL( aContactLink ) );
        if ( index != KErrNotFound )
            {
            VPbkEng::SendViewEventToObservers( *this, index, aContactLink,
                iObservers,
                &MVPbkContactViewObserver::ContactAddedToView,
                &MVPbkContactViewObserver::ContactViewError );
            VPbkEng::SendViewEventToObservers( *this, index, aContactLink,
                iFilteringObservers,
                &MVPbkContactViewObserver::ContactAddedToView,
                &MVPbkContactViewObserver::ContactViewError );
            }
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::HandleContactRemovedFromViewL
// --------------------------------------------------------------------------
//
void CFindViewBase::HandleContactRemovedFromViewL(
        MVPbkContactViewBase& aView, TInt /*aIndex*/,
        const MVPbkContactLink& aContactLink )
    {
    if ( &iParentView == &aView )
        {
        TInt index = KErrNotFound;

        CCntModelViewContact* removedContact = NULL;
        CViewContact* viewContact = CViewContact::NewL( iBaseView, SortOrder() );
        CleanupStack::PushL( viewContact );
        const TInt count( iMatchedContacts.Count() );
        for ( TInt i= 0; i < count && !removedContact; ++i  )
            {
            CCntModelViewContact* contact = iMatchedContacts[i];
            viewContact->SetViewContact( *contact );

            // Filter away the removed contact
            if ( aContactLink.RefersTo( *viewContact ) )
                {
                // Remove contact from the array
                iMatchedContacts.Remove( i );
                // Save the removed index for the observers
                index = i;
                // At this point nobody owns removedContact.
                removedContact = contact;
                }
            }
        CleanupStack::PopAndDestroy( viewContact );
        TBool takeCareOfOwnership = (removedContact && iOwnsContacts);

        // Notice: we can not delete the contact instance before
        // all observers have received the event. This is because
        // in case of CRefineView, iMatchedContacts contains references
        // to CCntModelViewContact instances. If we delete the contact
        // before notifying observer then the there will be invalid
        // pointer.
        if ( takeCareOfOwnership )
            {
            CleanupStack::PushL( removedContact );
            }

        if ( index != KErrNotFound )
            {
            VPbkEng::SendViewEventToObservers( *this, index, aContactLink,
                iObservers,
                &MVPbkContactViewObserver::ContactRemovedFromView,
                &MVPbkContactViewObserver::ContactViewError );
            VPbkEng::SendViewEventToObservers( *this, index, aContactLink,
                iFilteringObservers,
                &MVPbkContactViewObserver::ContactRemovedFromView,
                &MVPbkContactViewObserver::ContactViewError );
            }

        if ( takeCareOfOwnership)
            {
            // After all observers have received the Removed -event, it's
            // safe to actually destroy the contact.
            CleanupStack::PopAndDestroy( removedContact );
            }
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::DoAddObserver
// --------------------------------------------------------------------------
//
void CFindViewBase::DoAddObserver( MVPbkContactViewObserver& aObserver )
    {
    if (iViewReady)
        {
        // If this view is ready and there was no error,
        // tell it to the observer
        aObserver.ContactViewReady( *this );
        }
    else
        {
        aObserver.ContactViewUnavailable( *this );
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::DoAddObserverError
// --------------------------------------------------------------------------
//
void CFindViewBase::DoAddObserverError(
        MVPbkContactViewObserver& /*aObserver*/, TInt /*aError*/ )
    {
    // DoAddObserver doesn't leave so nothing to implement here
    }

// --------------------------------------------------------------------------
// CFindViewBase::DoAddFilteringObserverL
// --------------------------------------------------------------------------
//
void CFindViewBase::DoAddFilteringObserverL(
        MFilteredViewSupportObserver& aObserver )
    {
    // Insert to first position because events are send in reverse order.
    iFilteringObservers.InsertL( &aObserver, 0 );

    if (iViewReady)
        {
        aObserver.ContactViewReadyForFiltering( *this );
        aObserver.ContactViewReady( *this );
        }
    else
        {
        aObserver.ContactViewUnavailableForFiltering( *this );
        aObserver.ContactViewUnavailable( *this );
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::DoAddFilteringObserverError
// --------------------------------------------------------------------------
//
void CFindViewBase::DoAddFilteringObserverError(
        MFilteredViewSupportObserver& aObserver, TInt aError )
    {
    // See DoAddFilteringObserverL. If it leaves then adding the aObserver
    // failed.
    aObserver.ContactViewError( *this, aError, EFalse );
    }

// --------------------------------------------------------------------------
// CFindViewBase::ContactMatchL
// --------------------------------------------------------------------------
//
void CFindViewBase::ContactMatchL()
    {
    // Reset current content
    ResetContacts();
    // Subclass matches
    MatchL( iMatchedContacts );
    }


// --------------------------------------------------------------------------
// CFindViewBase::ResetContacts
// --------------------------------------------------------------------------
//
void CFindViewBase::ResetContacts()
    {
    if ( iOwnsContacts )
        {
        iMatchedContacts.ResetAndDestroy();
        }
    else
        {
        iMatchedContacts.Reset();
        }
    }

// --------------------------------------------------------------------------
// CFindViewBase::SendContactViewErrorEvent
// --------------------------------------------------------------------------
//
void CFindViewBase::SendContactViewErrorEvent( TInt aError,
        TBool aErrorNotified )
    {
    TBool destroy = EFalse;
    iDestroyed = &destroy;

    // Send first to external observers...
    VPbkEng::SendEventToObserversWhenNotDestroyed( *this, aError, aErrorNotified, iObservers,
        &MVPbkContactViewObserver::ContactViewError, destroy );
    // ...then to internal. This ensures that events come first from lower
    // level find view.
    VPbkEng::SendEventToObserversWhenNotDestroyed( *this, aError, aErrorNotified,
        iFilteringObservers, &MVPbkContactViewObserver::ContactViewError, destroy );
    
    if ( !destroy )
        {
        iDestroyed = NULL;
        }
    }
} // namespace VPbkCntModel
// End of File
