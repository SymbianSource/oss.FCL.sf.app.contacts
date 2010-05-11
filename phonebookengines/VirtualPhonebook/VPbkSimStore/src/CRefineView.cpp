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


#include "CRefineView.h"

// From VPbkSimStore
#include "CContactStore.h"
#include "CViewContact.h"
#include "CContactView.h"

// From Virtual Phonebook
#include <MVPbkSimContact.h>
#include <CVPbkSimCntField.h>
#include <CVPbkContactFindPolicy.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactViewSortPolicy.h>

// System includes
#include <cntviewbase.h>
#include <featmgr.h>

// Debugging headers
#include <VPbkProfile.h>

namespace VPbkSimStore {

inline CRefineView::CRefineView( MParentViewForFiltering& aParentView,
        CContactView& aAllContactsView,
        MAlwaysIncludedContacts& aAlwaysIncluded ) 
        :   CFindViewBase( aParentView, aAllContactsView, EFalse ),
            iAlwaysIncluded( aAlwaysIncluded )
    {
    }

// --------------------------------------------------------------------------
// CRefineView::~CRefineView
// --------------------------------------------------------------------------
//
CRefineView::~CRefineView()
    {
    delete iIdleUpdate;
    }

// --------------------------------------------------------------------------
// CRefineView::NewLC
// --------------------------------------------------------------------------
//
CRefineView* CRefineView::NewLC(
        const MDesCArray& aFindStrings,
        MParentViewForFiltering& aParentView,
        CContactView& aAllContactsView,
        MVPbkContactViewObserver& aExternalViewObserver,
        MVPbkContactFindPolicy& aContactFindPolicy,
        MAlwaysIncludedContacts& aAlwaysIncluded )
    {
    CRefineView* self =
        new ( ELeave ) CRefineView( aParentView, aAllContactsView, 
            aAlwaysIncluded );
    CleanupStack::PushL( self );
    self->BaseConstructL( aExternalViewObserver, aFindStrings, 
        aContactFindPolicy  );
    return self;
    }

// --------------------------------------------------------------------------
// CRefineView::ContactViewReady
// --------------------------------------------------------------------------
//
void CRefineView::ContactViewReady( MVPbkContactViewBase& /*aView*/ )
    {
    // Send view ready event to all observers
    SendViewStateEventToObservers();
    }
    
// --------------------------------------------------------------------------
// CRefineView::ContactViewReadyForFiltering
// --------------------------------------------------------------------------
//
void CRefineView::ContactViewReadyForFiltering( 
        MParentViewForFiltering& /*aView*/ )
    {
    // Cancel update so that there won't be double matching in case
    // the parent view is updating itself and also this view is updating
    // itself.
    if ( iIdleUpdate )
        {
        iIdleUpdate->Cancel();
        }
        
    // Parent view is ready -> Call base class function to start
    // matching. MatchContactsL will also call internal observer interface
    // to send internal view events to higher level filtered views. This
    // way all the CRefineViews are updated before any external observer
    // gets the view event. 
    // External observers will get view event from this view when the
    // CFindView  (CFindView::ViewFindCompletedL) calls 
    // SendViewStateEventToObservers.
    TRAPD( res, MatchContactsL() );
    if ( res != KErrNone )
        {
        ContactViewError( *this, res, EFalse );
        }
    }
    
// --------------------------------------------------------------------------
// CRefineView::MatchL
// --------------------------------------------------------------------------
//
void CRefineView::MatchL( RPointerArray<MVPbkSimContact>& aMatchedContacts )
    {
    CleanupClosePushL( aMatchedContacts );
    const TInt contactCount = iParentView.ContactCountL();
    for ( TInt i = 0; i < contactCount; ++i )
        {
        // iParentView is always VPbkSimStore view and the contacts type
        // is CViewContact
        const CViewContact& candidate = static_cast<const CViewContact&>(
            iParentView.ContactAtL( i ) );
        if ( IsContactAlwaysIncluded( candidate ) || IsMatchL( candidate ) )
            {
            // iParentView is always CFindView or CRefineView. The contacts type
            // is CViewContact. It can be safely casted. Const cast be done
            // because aContact is actually owned by CFindView.
            MVPbkSimContact* nativeContact = 
                const_cast<MVPbkSimContact*>( candidate.NativeContact() );
            aMatchedContacts.AppendL( nativeContact );
            }
        }    
    CleanupStack::Pop();
    }

// --------------------------------------------------------------------------
// CRefineView::DoContactAddedToViewL
// --------------------------------------------------------------------------
//
void CRefineView::DoContactAddedToViewL( MVPbkContactViewBase& aView,
        TInt aIndex, const MVPbkContactLink& /*aContactLink*/,
        RPointerArray<MVPbkSimContact>& aMatchedContacts )
    {
    CleanupClosePushL( aMatchedContacts );
    if ( &iParentView == &aView && 
         IsMatchL( iParentView.ContactAtL( aIndex )))
        {
        // iParentView is always CFindView or CRefineView the contacts type
        // is CViewContact. It can be safely casted.
        const CViewContact& contact = static_cast<const CViewContact&>(
            iParentView.ContactAtL( aIndex ) );
        
        // We have to insert the new contact to the correct
        // location. VPbk sort policy is used for that.
        CVPbkContactViewSortPolicy::TParam param(
            iAllContactsView.Store().MasterFieldTypeList(), SortOrder() );
        CVPbkContactViewSortPolicy* sortPolicy = 
            CVPbkContactViewSortPolicy::NewL( param );
        CleanupStack::PushL( sortPolicy );
        sortPolicy->SortStartL();
        // Create a view contact instance for comparing contacts.
        CViewContact* current = 
            CViewContact::NewL( iAllContactsView, SortOrder() );
        CleanupStack::PushL( current );
        
        const TInt count = aMatchedContacts.Count();
        TInt index = KErrNotFound;
        for ( TInt i = 0; i < count && index == KErrNotFound; ++i )
            {
            current->SetSimContactL( *aMatchedContacts[i] );
            TInt res = 
                sortPolicy->CompareContacts( contact, *current );
            if ( res < 0 )
                {
                index = i;
                }
            }
        sortPolicy->SortCompleted();
        CleanupStack::PopAndDestroy( 2, sortPolicy );
        
        // Const cast be done
        // because nativeContact is actually owned by CFindView.
        MVPbkSimContact* nativeContact = 
            const_cast<MVPbkSimContact*>( contact.NativeContact() );
        if ( index != KErrNotFound )
            {
            aMatchedContacts.InsertL( nativeContact, index );
            }
        else
            {
            aMatchedContacts.AppendL( nativeContact );
            }
        }   
    CleanupStack::Pop();
    }

// --------------------------------------------------------------------------
// CRefineView::UpdateFilterL
// --------------------------------------------------------------------------
//
void CRefineView::UpdateFilterL( 
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* /*aAlwaysIncludedContacts*/ )
    {
    if ( !iIdleUpdate )
        {
        iIdleUpdate = CIdle::NewL( CActive::EPriorityStandard );
        }
        
    // NOTE:
    // aAlwaysIncludedContacts is not used by CRefineView. It uses 
    // MAlwaysIncludedContacts interface for that purpose.
    SetFindStringsL( aFindWords );
    // Start idle update.
    if ( !iIdleUpdate->IsActive() )
        {
        iIdleUpdate->Start( TCallBack( StartUpdateCallback, this ) ); 
        }
    }

// --------------------------------------------------------------------------
// CRefineView::IsNativeMatchingRequestActive
// --------------------------------------------------------------------------
//
TBool CRefineView::IsNativeMatchingRequestActive()
    {
    // Forward to CFindView
    return iParentView.IsNativeMatchingRequestActive();
    }
    
// --------------------------------------------------------------------------
// CRefineView::IsContactAlwaysIncluded
// --------------------------------------------------------------------------
//
TBool CRefineView::IsContactAlwaysIncluded( 
        const CViewContact& aContact ) const
    {
    return iAlwaysIncluded.IsContactAlwaysIncluded( aContact );
    }

// --------------------------------------------------------------------------
// CRefineView::StartUpdateCallback
// --------------------------------------------------------------------------
//
TInt CRefineView::StartUpdateCallback( TAny* aThis )
    {
    CRefineView* view = static_cast<CRefineView*>( aThis );
    // If the CFindView is doing async match from SIM server then
    // do nothing here and wait that parent view notifies this view.
    if ( !view->IsNativeMatchingRequestActive() )
        {
        TRAPD( res, view->ActivateContactMatchL() )
        if ( res != KErrNone )
            {
            view->ContactViewError( *view, res, EFalse );
            }
        }
    // Do not continue idle
    return EFalse;
    }
} // namespace VPbkSimStore

// End of File
