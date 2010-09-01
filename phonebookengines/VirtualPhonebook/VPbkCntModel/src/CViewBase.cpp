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
* Description:  Contacts Model store contact view implementation.
*
*/


#include "CViewBase.h"

// INCLUDES

// VPbkCntModel
#include "CContactStore.h"
#include "CFieldFactory.h"
#include "CContact.h"
#include "CFieldTypeList.h"
#include "CContactLink.h"
#include "CContactBookmark.h"
#include "CFindView.h"
#include "VPbkCntModelRemoteViewPreferences.h"
#include "CSortOrderAcquirerList.h"

// VPbkEng
#include <MVPbkContactViewObserver.h>
#include <MVPbkFieldType.h>
#include <VPbkError.h>
#include <CVPbkSortOrder.h>
#include <CVPbkAsyncCallback.h>
#include <VPbkPrivateUid.h>     // KFindPluingUID
#include <VPbkSendEventUtility.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <cntitem.h>

// Debugging headers
#include <VPbkDebug.h>

namespace VPbkCntModel {

// CONSTANTS

const TInt KObserverArrayGranularity = 4;
    
// --------------------------------------------------------------------------
// CViewBase::CViewBase
// --------------------------------------------------------------------------
//
CViewBase::CViewBase( CContactStore& aParentStore ) :
        iObservers( KObserverArrayGranularity ),
        iParentStore( aParentStore )
    {
    }

// --------------------------------------------------------------------------
// CViewBase::~CViewBase
// --------------------------------------------------------------------------
//
CViewBase::~CViewBase()
    {
    delete iObserverOp;
    delete iFilterObsOp;
    delete iEventLink;
    if (iView)
        {
        iView->Close(*this);
        }
    delete iCurrentContact;
    iObservers.Close();
    iFilteringObservers.Close();
    delete iViewDefinition;
    delete iSortOrder;
    iParentStore.RemoveObserver(*this);
    }

// --------------------------------------------------------------------------
// CViewBase::ConstructL
// --------------------------------------------------------------------------
//
void CViewBase::ConstructL(
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder )
    {
    iParentStore.AddObserverL(*this);
    iSortOrder = CVPbkSortOrder::NewL(aSortOrder);
    iViewDefinition = CVPbkContactViewDefinition::NewL( aViewDefinition );

    if ( iParentStore.IsOpened() )
        {
        InitializeViewL( aViewDefinition, aSortOrder );
        }

    iObserverOp =
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>::NewL();
    iFilterObsOp =
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>::NewL();

    AddObserverL(aObserver);
    }

// --------------------------------------------------------------------------
// CViewBase::ConstructL
// --------------------------------------------------------------------------
//
void CViewBase::ConstructL(
        const CVPbkContactViewDefinition& aViewDefinition,
        const MVPbkFieldTypeList& aSortOrder )
    {
    iParentStore.AddObserverL(*this);
    iSortOrder = CVPbkSortOrder::NewL(aSortOrder);
    iViewDefinition = CVPbkContactViewDefinition::NewL( aViewDefinition );

    if ( iParentStore.IsOpened() )
        {
        InitializeViewL( aViewDefinition, aSortOrder );
        }

    iObserverOp =
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>::NewL();
    iFilterObsOp =
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>::NewL();
    }

// --------------------------------------------------------------------------
// CViewBase::ChangeSortOrderL
// --------------------------------------------------------------------------
//
void CViewBase::ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder)
    {
    // If this view is a remote view in Contacts server a strict policy
    // must be applied for changing sortorder. Shared views are used
    // widely in S60 applications so their sortorder must be controlled
    // by S60 Phonebook application.
    LeaveIfIncorrectSortOrderL( aSortOrder );
    
    CVPbkSortOrder* newSortOrder = CVPbkSortOrder::NewL(aSortOrder);
    CleanupStack::PushL(newSortOrder);

    RContactViewSortOrder viewSortOrder = CreateSortOrderL(aSortOrder);
    CleanupClosePushL(viewSortOrder);

    if ( !DoChangeSortOrderL( *iViewDefinition, viewSortOrder ) )
        {
        TeardownView();
        InitializeViewL( *iViewDefinition, aSortOrder );
        }
    
    CleanupStack::PopAndDestroy(); // viewSortOrder
    CleanupStack::Pop(); // newSortOrder

    delete iSortOrder;
    iSortOrder = newSortOrder;
    }

// --------------------------------------------------------------------------
// CViewBase::SortOrder
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CViewBase::SortOrder() const
    {
    return *iSortOrder;
    }

// --------------------------------------------------------------------------
// CViewBase::RefreshL
// --------------------------------------------------------------------------
//
void CViewBase::RefreshL()
    {
    // make the view sort itself again to refresh contents
    RContactViewSortOrder viewSortOrder = CreateSortOrderL(*iSortOrder);
    CleanupClosePushL(viewSortOrder);

    DoChangeSortOrderL( *iViewDefinition, viewSortOrder );

    CleanupStack::PopAndDestroy(); // viewSortOrder
    }

// --------------------------------------------------------------------------
// CViewBase::ContactCountL
// --------------------------------------------------------------------------
//
TInt CViewBase::ContactCountL() const
    {
    TInt result = 0;
    if (iViewReady)
        {
        result = iView->CountL();
        }
    return result;
    }

// --------------------------------------------------------------------------
// CViewBase::ContactAtL
// --------------------------------------------------------------------------
//
const MVPbkViewContact& CViewBase::ContactAtL(TInt aIndex) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= ContactCountL() )
        {
        User::Leave( KErrArgument );
        }

    const ::CViewContact& viewContact = iView->ContactAtL(aIndex);
    iCurrentContact->SetViewContact(viewContact);
    return *iCurrentContact;
    }

// --------------------------------------------------------------------------
// CViewBase::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CViewBase::CreateLinkLC(TInt aIndex) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= ContactCountL() )
        {
        User::Leave( KErrArgument );
        }

    TContactItemId contactId = iView->AtL(aIndex);
    return CContactLink::NewLC(iParentStore, contactId);
    }

// --------------------------------------------------------------------------
// CViewBase::IndexOfLinkL
// --------------------------------------------------------------------------
//
TInt CViewBase::IndexOfLinkL(const MVPbkContactLink& aContactLink) const
    {
    TInt result = KErrNotFound;

    if (&aContactLink.ContactStore() == &ContactStore() && iView )
        {
        const CContactLink& link = static_cast<const CContactLink&>(aContactLink);
        result = iView->FindL(link.ContactId());
        }

    return result;
    }

// --------------------------------------------------------------------------
// CViewBase::AddObserverL
// --------------------------------------------------------------------------
//
void CViewBase::AddObserverL(MVPbkContactViewObserver& aObserver)
    {
    CVPbkAsyncObjectCallback<MVPbkContactViewObserver>* callback =
        VPbkEngUtils::CreateAsyncObjectCallbackLC(
            *this,
            &CViewBase::DoAddObserverL,
            &CViewBase::AddObserverError,
            aObserver);
    iObserverOp->CallbackL( callback );
    CleanupStack::Pop( callback );

    /// Insert to first position because event are send in reverse order.
    /// Last inserted gets notifcation last.
    iObservers.InsertL( &aObserver, 0 );
    }

// --------------------------------------------------------------------------
// CViewBase::RemoveObserver
// --------------------------------------------------------------------------
//
void CViewBase::RemoveObserver(MVPbkContactViewObserver& aObserver)
    {
    iObserverOp->CancelCallback( &aObserver );
    const TInt index( iObservers.Find( &aObserver ) );
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// CViewBase::MatchContactStore
// --------------------------------------------------------------------------
//
TBool CViewBase::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iParentStore.MatchContactStore(aContactStoreUri);
    }

// --------------------------------------------------------------------------
// CViewBase::MatchContactStoreDomain
// --------------------------------------------------------------------------
//
TBool CViewBase::MatchContactStoreDomain
        ( const TDesC& aContactStoreDomain ) const
    {
    return iParentStore.MatchContactStoreDomain( aContactStoreDomain );
    }

// --------------------------------------------------------------------------
// CViewBase::CreateBookmarkLC
// --------------------------------------------------------------------------
//
MVPbkContactBookmark* CViewBase::CreateBookmarkLC(TInt aIndex) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );

    TContactItemId contactId = iView->AtL(aIndex);
    // Link implements also the bookmark interface in this store
    return CContactBookmark::NewLC( contactId, iParentStore );
    }

// --------------------------------------------------------------------------
// CViewBase::IndexOfBookmarkL
// --------------------------------------------------------------------------
//
TInt CViewBase::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark) const
    {
    TInt result = KErrNotFound;
    // Bookmark is implemeted as a link in this store
    const CContactBookmark* bookmark =
        dynamic_cast<const CContactBookmark*>(&aContactBookmark);
    if (bookmark &&
        &bookmark->ContactStore() == &ContactStore() )
        {
        result = iView->FindL( bookmark->ContactId() );
        }

    return result;
    }

// --------------------------------------------------------------------------
// CViewBase::AddFilteringObserverL
// --------------------------------------------------------------------------
//
void CViewBase::AddFilteringObserverL(
        MFilteredViewSupportObserver& aObserver )
    {
    // Insert observer in callback function. That ensures that the observer
    // will always get the event asynchronously.

    CVPbkAsyncObjectCallback<MFilteredViewSupportObserver>* callback =
        VPbkEngUtils::CreateAsyncObjectCallbackLC(
            *this,
            &CViewBase::DoAddFilteringObserverL,
            &CViewBase::DoAddFilteringObserverError,
            aObserver);

    iFilterObsOp->CallbackL( callback );
    CleanupStack::Pop( callback );
    }

// --------------------------------------------------------------------------
// CViewBase::RemoveFilteringObserver
// --------------------------------------------------------------------------
//
void CViewBase::RemoveFilteringObserver(
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
// CViewBase::ViewFiltering
// --------------------------------------------------------------------------
//
MVPbkContactViewFiltering* CViewBase::ViewFiltering()
    {
    // Both contact view and group view support filtering
    return this;
    }

// --------------------------------------------------------------------------
// CViewBase::CreateFilteredViewLC
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CViewBase::CreateFilteredViewLC(
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
// CViewBase::UpdateFilterL
// --------------------------------------------------------------------------
//
void CViewBase::UpdateFilterL(
        const MDesCArray& /*aFindWords*/,
        const MVPbkContactBookmarkCollection* /*aAlwaysIncludedContacts*/ )
    {
    // CViewBase implementations are "all contacts" views and not itself
    // filtered views. Only creating a filtered view is supported.
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CViewBase::ParentObject
// --------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CViewBase::ParentObject() const
    {
    return iParentStore;
    }

// --------------------------------------------------------------------------
// CViewBase::StoreReady
// --------------------------------------------------------------------------
//
void CViewBase::StoreReady( MVPbkContactStore& /*aContactStore*/ )
    {
    if ( !iViewReady )
        {
        TeardownView();
        TRAPD( error, InitializeViewL( *iViewDefinition, *iSortOrder ) );

        if ( error != KErrNone )
            {
            iViewReady = EFalse;
            SendViewErrorEvent( error );
            }
        }
    }

// --------------------------------------------------------------------------
// CViewBase::StoreUnavailable
// --------------------------------------------------------------------------
//
void CViewBase::StoreUnavailable
        ( MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/ )
    {
    TeardownView();
    iViewReady = EFalse;
    SendViewStateEvent();
    }

// --------------------------------------------------------------------------
// CViewBase::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CViewBase::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    // No need to handle this here, events are received from the view
    }

// --------------------------------------------------------------------------
// CViewBase::HandleContactViewEvent
// --------------------------------------------------------------------------
//
void CViewBase::HandleContactViewEvent
        ( const CContactViewBase& /*aView*/,
          const TContactViewEvent& aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case TContactViewEvent::EUnavailable:
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) EUnavailable"),
                this );

            iViewReady = EFalse;
            SendViewStateEvent();
            break;
            }
        case TContactViewEvent::EReady:
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) EReady"),
                this);

            iViewReady = ETrue;
            SendViewStateEvent();
            break;
            }
        case TContactViewEvent::EItemAdded:
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) EItemAdded\
                 index %d id %d"), this, aEvent.iInt, aEvent.iContactId);
            iViewReady = ETrue;
            iEventLink->SetContactId(aEvent.iContactId);
            VPbkEng::SendViewEventToObservers( *this, aEvent.iInt, *iEventLink,
                iObservers, &MVPbkContactViewObserver::ContactAddedToView,
                &MVPbkContactViewObserver::ContactViewError );
            VPbkEng::SendViewEventToObservers( *this, aEvent.iInt, *iEventLink,
                iFilteringObservers,
                &MVPbkContactViewObserver::ContactAddedToView,
                &MVPbkContactViewObserver::ContactViewError );
            break;
            }
        case TContactViewEvent::EItemRemoved:
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) EItemRemoved\
                index %d id %d"),
                this, aEvent.iInt, aEvent.iContactId);

            iViewReady = ETrue;
            iEventLink->SetContactId( aEvent.iContactId );
            VPbkEng::SendViewEventToObservers( *this, aEvent.iInt, *iEventLink,
                iObservers,
                &MVPbkContactViewObserver::ContactRemovedFromView,
                &MVPbkContactViewObserver::ContactViewError );
            VPbkEng::SendViewEventToObservers( *this, aEvent.iInt, *iEventLink,
                iFilteringObservers,
                &MVPbkContactViewObserver::ContactRemovedFromView,
                &MVPbkContactViewObserver::ContactViewError );
            break;
            }
        case TContactViewEvent::ESortOrderChanged:
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) ESortOrderChanged"),
                this);

            iViewReady = ETrue;
            // Update the new sort order to the view contact
            TRAPD( res, DoUpdateTypeListL() );
            if ( res != KErrNone )
                {
                SendViewErrorEvent( res );
                }
            else
                {
                SendViewStateEvent();
                }
            break;
            }
        case TContactViewEvent::ESortError:
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) ESortError"),
                this);

            iViewReady = EFalse;
            SendViewErrorEvent( aEvent.iInt );
            break;
            }
        case TContactViewEvent::EServerError:
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) EServerError"),
                this);

            iViewReady = EFalse;
            SendViewErrorEvent( aEvent.iInt );
            break;
            }

        case TContactViewEvent::EIndexingError:
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) EIndexingError"),
                this);

            iViewReady = EFalse;
            SendViewErrorEvent( aEvent.iInt );
            break;
            }
        case TContactViewEvent::EGroupChanged: // FALLTHROUGH
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) EGroupChanged"),
                this);

            // We don't send any events because EItemRemoved and/or
            // EItemAdded events are generated and those handle the UI
            // refreshing. If the view is not ready we are expecting
            // EReady event from CntModel to refresh the view.
            }
        default:
            {
            // Default is: do nothing
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
                ("CViewBase::HandleContactViewEvent(0x%x) eventType %d"),
                this, aEvent.iEventType );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CViewBase::HandleCustomContactViewEvent
// --------------------------------------------------------------------------
//
void CViewBase::HandleCustomContactViewEvent
        ( const CContactViewBase& aView,
          const TContactViewEvent& aEvent )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CViewBase::HandleCustomContactViewEvent(0x%x)"), &aView);

    // This function needs to behave like HandleContactViewEvent function
    // above, but without notifying observers. The observers will be notified
    // later, with the succeeding HandleContactViewEvent call.
    switch ( aEvent.iEventType )
        {
        case TContactViewEvent::EReady:             // FALLTHROUGH
        case TContactViewEvent::EItemAdded:         // FALLTHROUGH
        case TContactViewEvent::EItemRemoved:       // FALLTHROUGH
        case TContactViewEvent::ESortOrderChanged:  // FALLTHROUGH
        case TContactViewEvent::EGroupChanged:      // FALLTHROUGH
            {
            iViewReady = ETrue;
            break;
            }

        case TContactViewEvent::EUnavailable:       // FALLTHROUGH
        case TContactViewEvent::ESortError:         // FALLTHROUGH
        case TContactViewEvent::EServerError:       // FALLTHROUGH
        case TContactViewEvent::EIndexingError:     // FALLTHROUGH
            {
            iViewReady = EFalse;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CViewBase::CreateSortOrderL
// --------------------------------------------------------------------------
//
RContactViewSortOrder CViewBase::CreateSortOrderL
        ( const MVPbkFieldTypeList& aSortOrder )
    {
    RContactViewSortOrder sortOrder;
    CleanupClosePushL(sortOrder);

    const TInt count = aSortOrder.FieldTypeCount();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkFieldType& fieldType = aSortOrder.FieldTypeAt(i);
        // Ignores all field types that are not supported by this store
        const CContactItemField* field =
            iParentStore.FieldFactory().FindField(fieldType);
        if (field)
            {
            sortOrder.AppendL(field->ContentType().FieldType(0));
            }
        }
    CleanupStack::Pop(); // sortOrder

    return sortOrder;
    }

// --------------------------------------------------------------------------
// CViewBase::DoUpdateTypeListL
// --------------------------------------------------------------------------
//
void CViewBase::DoUpdateTypeListL()
    {
    MVPbkFieldTypeList* typeList = CFieldTypeList::NewLC
        (iView->SortOrderL(), iParentStore.FieldTypeMap());
    iCurrentContact->SetTypeListL(*typeList);

    CVPbkSortOrder* sortOrder = CVPbkSortOrder::NewL(*typeList);
    delete iSortOrder;
    iSortOrder = sortOrder;
    CleanupStack::PopAndDestroy(); // typeList
    }

// --------------------------------------------------------------------------
// CViewBase::DoAddObserverL
// --------------------------------------------------------------------------
//
void CViewBase::DoAddObserverL(MVPbkContactViewObserver& aObserver)
    {
    // Check if aObserver is still observer of this view
    if ( iObservers.Find( &aObserver ) != KErrNotFound )
        {
        if (iViewReady)
            {
            // If this view is ready and there was no error,
            // tell it to the observer
            aObserver.ContactViewReady(*this);
            }
        else if (!iViewReady && !iParentStore.IsOpened())
            {
            aObserver.ContactViewUnavailable(*this);
            }
        // If this view was not ready and there was no error, observer will
        // be called back in HandleContactViewEvent
        }
    }

// --------------------------------------------------------------------------
// CViewBase::AddObserverError
// --------------------------------------------------------------------------
//
void CViewBase::AddObserverError(MVPbkContactViewObserver& aObserver, TInt aError)
    {
    // Check if aObserver is still observer of this view
    if ( iObservers.Find( &aObserver ) != KErrNotFound)
        {
        aObserver.ContactViewError(*this, aError, EFalse);
        }
    }

// --------------------------------------------------------------------------
// CViewBase::DoAddFilteringObserverL
// --------------------------------------------------------------------------
//
void CViewBase::DoAddFilteringObserverL(
        MFilteredViewSupportObserver& aObserver )
    {
    /// Insert to first position because event are send in reverse order.
    /// Last inserted gets notifcation last.
    iFilteringObservers.InsertL( &aObserver, 0 );

    if (iViewReady)
        {
        // If this view is ready and there was no error.
        // Build filtered view first using the internal interface
        aObserver.ContactViewReadyForFiltering( *this );
        // Then let the filtered view to send event to its external
        // observers.
        // aObserver.ContactViewReady( *this ); // removed to function CFindViewBase::ContactViewReadyForFiltering(...)
        }
    else if (!iViewReady && !iParentStore.IsOpened())
        {
        aObserver.ContactViewUnavailableForFiltering( *this );
        aObserver.ContactViewUnavailable( *this );
        }
    }

// --------------------------------------------------------------------------
// CViewBase::DoAddFilteringObserverError
// --------------------------------------------------------------------------
//
void CViewBase::DoAddFilteringObserverError(
        MFilteredViewSupportObserver& aObserver, TInt aError )
    {
    // See DoAddFilteringObserverL. If it leaves then adding the aObserver
    // failed.
    aObserver.ContactViewError(*this, aError, EFalse);
    }

// --------------------------------------------------------------------------
// CViewBase::InitializeViewL
// --------------------------------------------------------------------------
//
void CViewBase::InitializeViewL( 
        const CVPbkContactViewDefinition& aViewDefinition,
        const MVPbkFieldTypeList& aSortOrder )
    {
    
    CViewContact* vievContact = CViewContact::NewL( *this, aSortOrder );
    if ( iCurrentContact )
        {
        delete iCurrentContact;
        iCurrentContact = NULL;
        }
    iCurrentContact = vievContact;
    vievContact = NULL;
    
    RContactViewSortOrder viewSortOrder = CreateSortOrderL( aSortOrder );
    CleanupClosePushL( viewSortOrder );

    // Let sub class create the concrete view to iView
    DoInitializeViewL( aViewDefinition, viewSortOrder );
    // Update sort order because shared view can have a different
    // sort order compared to aSortOrder. Creating a new remote view
    // handle to Contacts Model shared view doesn't change the sort order
    // in Contacts Server shared view.
    if ( iView )
        {
        DoUpdateTypeListL();
    
        // Set find plugin uid, without this cntmodel find does not work with
        // all variants.
        iView->SetViewFindConfigPlugin( TUid::Uid( KFindPluginUID ) );
        }

    CleanupStack::PopAndDestroy(); // viewSortOrder

    CContactLink* contactLink= CContactLink::NewLC( iParentStore, KNullContactId );
    CleanupStack::Pop( contactLink );
    if ( iEventLink )
        {
        delete iEventLink;
        iEventLink = NULL;
        }
    iEventLink = contactLink;
    contactLink = NULL;
    }

// --------------------------------------------------------------------------
// CViewBase::TeardownView
// --------------------------------------------------------------------------
//
void CViewBase::TeardownView()
    {
    delete iCurrentContact;
    iCurrentContact = NULL;

    if (iView)
        {
        iView->Close(*this);
        iView = NULL;
        }

    delete iEventLink;
    iEventLink = NULL;

    DoTeardownView();
    }

// --------------------------------------------------------------------------
// CViewBase::SendViewErrorEvent
// --------------------------------------------------------------------------
//
void CViewBase::SendViewErrorEvent( TInt aError )
    {
    // Cancel async operation to avoid double message sending
    // to observer in DoAddObserverL
    PurgeAsyncOperations();

    /// Then report the failure to client so that in handle it.
    TBool errorIsNotified = EFalse;
    VPbkEng::SendEventToObservers( *this, aError, errorIsNotified,
        iObservers, &MVPbkContactViewObserver::ContactViewError );
    VPbkEng::SendEventToObservers( *this, aError, errorIsNotified,
        iFilteringObservers, &MVPbkContactViewObserver::ContactViewError );
    }

// --------------------------------------------------------------------------
// CViewBase::SendViewStateEvent
// --------------------------------------------------------------------------
//
void CViewBase::SendViewStateEvent()
    {
    // Cancel async operation to avoid double message sending
    // to observer in DoAddObserverL
    PurgeAsyncOperations();

    void (MVPbkContactViewObserver::*obsNotifyFunc)(
            MVPbkContactViewBase&) =
                &MVPbkContactViewObserver::ContactViewReady;
    void (MFilteredViewSupportObserver::*filtObsNotifyFunc)(
            MParentViewForFiltering&) =
                &MFilteredViewSupportObserver::ContactViewReadyForFiltering;

    if ( !iViewReady )
        {
        obsNotifyFunc = &MVPbkContactViewObserver::ContactViewUnavailable;
        filtObsNotifyFunc =
            &MFilteredViewSupportObserver::ContactViewUnavailableForFiltering;
        }

    // Due to filtered view stack it must be ensured that internal filtered
    // views know first about changes because CRefineViews have pointers to
    // contacts that must not be invalidated.
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

// --------------------------------------------------------------------------
// CViewBase::PurgeAsyncOperations
// --------------------------------------------------------------------------
//
void CViewBase::PurgeAsyncOperations()
    {
    // Purge only iObserverOp callbacks because observers are added
    // already in AddObserverL. iFilterObsOp must not be purged because
    // filtering observers are added in a call back DoAddFilteringObserverL.
    iObserverOp->Purge();
    }

// --------------------------------------------------------------------------
// CViewBase::LeaveIfIncorrectSortOrderL
// --------------------------------------------------------------------------
//
void CViewBase::LeaveIfIncorrectSortOrderL( 
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
} // namespace VPbkCntModel

// End of File
