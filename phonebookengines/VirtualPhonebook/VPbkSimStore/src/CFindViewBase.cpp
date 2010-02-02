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
* Description:  Sim store filtered contact view implementation.
*
*/


#include "CFindViewBase.h"

#include "CRefineView.h"
#include "CContactStore.h"
#include "CContactLink.h"
#include "CContactView.h"
#include "CViewContact.h"

// Virtual Phonebook
#include <VPbkError.h>
#include <MVPbkSimContact.h>
#include <CVPbkSimContact.h>
#include <CVPbkSimCntField.h>
#include <MVPbkSimStoreOperation.h>
#include <CVPbkAsyncCallback.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactNameConstructionPolicy.h>
#include <MVPbkContactFindPolicy.h>
#include <VPbkSendEventUtility.h>

// System includes
#include <cntviewbase.h>
#include <featmgr.h>

namespace VPbkSimStore {

#ifdef _DEBUG
enum TCFindViewBasePanicCode
    {
    EPreCond_SetFindStringsL = 1    
    };

void Panic( TCFindViewBasePanicCode aPanicCode )
    {
    _LIT( KPanicText, "CFindViewBase" );
    User::Panic( KPanicText, aPanicCode );
    }
#endif // _DEBUG

const TInt KGranularity( 4 );

// --------------------------------------------------------------------------
// CFindViewBase::CFindViewBase
// --------------------------------------------------------------------------
//
CFindViewBase::CFindViewBase( MParentViewForFiltering& aParentView,
        CContactView& aAllContactsView,
        TBool aOwnsMatchedContacts ) 
        :   iParentView( aParentView ),
            iAllContactsView( aAllContactsView ),
            iOwnsContacts( aOwnsMatchedContacts )
    {
    }

// --------------------------------------------------------------------------
// CFindViewBase::BaseConstructL
// --------------------------------------------------------------------------
//
void CFindViewBase::BaseConstructL( 
        MVPbkContactViewObserver& aExternalViewObserver,
        const MDesCArray& aFindStrings,
        MVPbkContactFindPolicy& aFindPolicy )
    {                        
    iObserverOp = 
        CVPbkAsyncObjectOperation<MVPbkContactViewObserver>::NewL();
    iFilterObsOp = 
        CVPbkAsyncObjectOperation<MFilteredViewSupportObserver>::NewL();
    
    // Copy find words
    iFindStrings = new(ELeave)CDesCArrayFlat( KGranularity );
    SetFindStringsL( aFindStrings );
    
    iCurrentContact = CViewContact::NewL( iAllContactsView, SortOrder() );
    
    // Assign contact find policy
    iContactFindPolicy = &aFindPolicy;
    
    //Create field type list
    iFieldTypeRefsList = CVPbkFieldTypeRefsList::NewL();
    
    // Append the observer who created the view. Events must be always send
    // to this observer first.
    iObservers.AppendL( &aExternalViewObserver );
    }

// --------------------------------------------------------------------------
// CFindViewBase::~CFindViewBase
// --------------------------------------------------------------------------
//
CFindViewBase::~CFindViewBase()
    {   
    delete iObserverOp;
    delete iFilterObsOp;
    iObservers.Close();
    iFilteringObservers.Close();
    ResetContacts();
    delete iCurrentContact;
    delete iFindStrings;
    delete iFieldTypeRefsList;
    iParentView.RemoveFilteringObserver( *this );
    }

// --------------------------------------------------------------------------
// CFindViewBase::ActivateContactMatchL
// --------------------------------------------------------------------------
//
void CFindViewBase::ActivateContactMatchL()
    {
    // If used from update then we must renew the observing
    iParentView.RemoveFilteringObserver( *this );
    // Get the state of the parent view asynchrnounsly
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
    if ( aIndex >= iMatchedContacts.Count() )
        {
        User::Leave( KErrArgument );
        }

    iCurrentContact->SetSimContactL( *iMatchedContacts[ aIndex ] );
    return *iCurrentContact;     
    }    

// --------------------------------------------------------------------------
// CFindViewBase::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CFindViewBase::CreateLinkLC(
        TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0, 
        VPbkError::Panic(VPbkError::EInvalidContactIndex) );
    if ( aIndex >= iMatchedContacts.Count() )
        {
        User::Leave( KErrArgument );
        }
        
    TInt simIndex = iMatchedContacts[aIndex]->SimIndex();
    return CContactLink::NewLC( iAllContactsView.Store(), simIndex );
    }

// --------------------------------------------------------------------------
// CFindViewBase::IndexOfLinkL
// --------------------------------------------------------------------------
//    
TInt CFindViewBase::IndexOfLinkL(
        const MVPbkContactLink& aContactLink ) const
    {
    if ( &aContactLink.ContactStore() == &iAllContactsView.Store() )
        {
        TInt simIndex = 
            static_cast<const CContactLink&>( aContactLink ).SimIndex();
                    
        const TInt count( iMatchedContacts.Count() );
        for ( TInt i(0); i < count; ++i )
            {
            if ( iMatchedContacts[i]->SimIndex() == simIndex )
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
void CFindViewBase::ChangeSortOrderL(
        const MVPbkFieldTypeList& aSortOrder )
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
void CFindViewBase::AddObserverL(
        MVPbkContactViewObserver& aObserver )
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
    // Events must be send in the same order as the observers were added.
    iObservers.InsertL( &aObserver, 0 );
    }

// --------------------------------------------------------------------------
// CFindViewBase::RemoveObserver
// --------------------------------------------------------------------------
//    
void CFindViewBase::RemoveObserver(
        MVPbkContactViewObserver& aObserver )
    {
    iObserverOp->CancelCallback( &aObserver );
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
TBool CFindViewBase::MatchContactStore(
        const TDesC& aContactStoreUri ) const
    {
    return iParentView.MatchContactStore( aContactStoreUri );
    }

// --------------------------------------------------------------------------
// CFindViewBase::MatchContactStoreDomain
// --------------------------------------------------------------------------
//    
TBool CFindViewBase::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain ) const      
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
        VPbkError::Panic(VPbkError::EInvalidContactIndex) );
    if ( aIndex >= iMatchedContacts.Count() )
        {
        User::Leave( KErrArgument );
        }
        
    TInt simIndex = iMatchedContacts[aIndex]->SimIndex();
    return CContactLink::NewLC( iAllContactsView.Store(), simIndex );
    }

// --------------------------------------------------------------------------
// CFindViewBase::IndexOfBookmarkL
// --------------------------------------------------------------------------
//     
TInt CFindViewBase::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark ) const
    {
    // Bookmark is implemented as a link in this store
    const CContactLink* link = 
        dynamic_cast<const CContactLink*>( &aContactBookmark );
    if ( link && ( &link->ContactStore() == &iAllContactsView.Store() ) )
        {
        TInt simIndex = link->SimIndex();
                    
        const TInt count( iMatchedContacts.Count() );
        for ( TInt i(0); i < count; ++i )
            {
            if ( iMatchedContacts[i]->SimIndex() == simIndex )
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
    // Find view supports further filtering using CRefineView
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
    // NOTE: aAlwaysIncludedContacts is commented because in VPbkSimStore
    // filtered views are linked. CFindView->CRefineView->CRefineView etc.
    // Only the CFindView saves the always included contacts and they
    // are used also by all CRefineView instances through 
    // MAlwaysIncludedContacts interface.
    CRefineView* refineView = CRefineView::NewLC( aFindWords, *this, 
        iAllContactsView, aObserver, *iContactFindPolicy, *this );
    refineView->ActivateContactMatchL();
    return refineView;
    }   
    
// --------------------------------------------------------------------------
// CFindViewBase::ContactViewUnavailable
// --------------------------------------------------------------------------
//    
void CFindViewBase::ContactViewUnavailable(
        MVPbkContactViewBase& aView )
    {
    if ( &iParentView == &aView )
        {
        SendViewStateEventToObservers();
        }
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
void CFindViewBase::ContactViewError(
        MVPbkContactViewBase& /*aView*/, 
        TInt aError, 
        TBool aErrorNotified )
    {
    // This function can be called from the parent view or from the subclass.
    // In both cases this view is not usable anymore.
    iViewReady = EFalse;
    ResetContacts();
    
    // Send first to external observers...
    VPbkEng::SendEventToObservers( *this, aError, aErrorNotified, iObservers, 
        &MVPbkContactViewObserver::ContactViewError );
    // ...then to internal. This ensures that events come first from lower
    // level find view.
    VPbkEng::SendEventToObservers( *this, aError, aErrorNotified, 
        iFilteringObservers, &MVPbkContactViewObserver::ContactViewError );
    }     

// --------------------------------------------------------------------------
// CFindViewBase::ContactViewUnavailableForFiltering
// --------------------------------------------------------------------------
//    
void CFindViewBase::ContactViewUnavailableForFiltering( 
        MParentViewForFiltering& aView )
    {
    if ( &iParentView == &aView )
        {
        iViewReady = EFalse;
        ResetContacts();
        // Send events first to filtered views that are built on this
        // view so that they can update them selves before notifying
        // any external observers.
        // Parent view will call ContactViewUnvailable after this and then
        // the external observers will get the view event too.
        VPbkEng::SendViewEventToObservers( *this, iFilteringObservers, 
        &MFilteredViewSupportObserver::ContactViewUnavailableForFiltering,
        &MVPbkContactViewObserver::ContactViewError );
        }
    }
    
// --------------------------------------------------------------------------
// CFindViewBase::MatchContactsL
// --------------------------------------------------------------------------
//
void CFindViewBase::MatchContactsL()
    {
    // Destroy old matches
    ResetContacts();
    // Do the match
    MatchL( iMatchedContacts );
    iViewReady = ETrue;
    // Send events to filtered views that are built on this
    // view so that they can update them selves before notifying
    // any external observers.
    // It's subclass responsibility to call SendViewStateEventToObservers
    // when external observers are allowed to get view event.
    VPbkEng::SendViewEventToObservers( *this, iFilteringObservers, 
        &MFilteredViewSupportObserver::ContactViewReadyForFiltering,
        &MVPbkContactViewObserver::ContactViewError );
    }
    
// --------------------------------------------------------------------------
// CFindViewBase::IsMatchL
// --------------------------------------------------------------------------
//
TBool  CFindViewBase::IsMatchL( const MVPbkViewContact& aViewContact )
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

// --------------------------------------------------------------------------
// CFindViewBase::SetFindStringsL
// --------------------------------------------------------------------------
//                
void CFindViewBase::SetFindStringsL( const MDesCArray& aFindStrings )
    {
    __ASSERT_DEBUG( iFindStrings, Panic( EPreCond_SetFindStringsL ) );
    
    iFindStrings->Reset();
    const TInt count( aFindStrings.MdcaCount() );
    for ( TInt i(0); i < count; ++i )
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
    // Empty implementation
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
    aObserver.ContactViewError( *this, aError, EFalse );
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
        MVPbkContactViewBase& aView, TInt /*aIndex*/, const 
        MVPbkContactLink& aContactLink )
    {
    if ( &iParentView == &aView )
        {
        // We know that link is always VPbkSimStore::CContactLink
        const CContactLink& link = 
            static_cast<const CContactLink&>(aContactLink);
        
        TInt index = KErrNotFound;
        MVPbkSimContact* removedContact = NULL;
        const TInt simIndex = link.SimIndex();
        const TInt count( iMatchedContacts.Count() );    
        for ( TInt i= 0; i < count && !removedContact; ++i  )
            {                                                      
            MVPbkSimContact* contact = iMatchedContacts[i];        
            
            // Filter away the removed contact
            if ( contact->SimIndex() == simIndex )
                {
                // Remove contact from the array
                iMatchedContacts.Remove( i );
                // Save the removed index for the observers
                index = i;
                // At this point nobody owns removedContact.
                removedContact = contact;
                }   
            } 
        TBool takeCareOfOwnership = (removedContact && iOwnsContacts);
        
        // Notice: we can not delete the contact instance before
        // all observers have received the event. This is because
        // in case of CRefineView, iMatchedContacts contains references 
        // to MVPbkSimContact instances. If we delete the contact
        // before notifying observer then the there will be invalid
        // pointer.
        if ( takeCareOfOwnership )
            {
            CleanupDeletePushL( removedContact );
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
            CleanupStack::PopAndDestroy(); // removedContact
            }
        }
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
} // namespace VPbkSimStore 
// End of File
