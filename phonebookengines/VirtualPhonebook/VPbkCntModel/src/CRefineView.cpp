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
#include "CRefineView.h"

// VPbkCntModel
#include "CViewBase.h"
#include <MVPbkContactLink.h>

// Debugging headers
#include <VPbkProfile.h>

namespace VPbkCntModel {

// --------------------------------------------------------------------------
// CRefineView::CRefineView
// --------------------------------------------------------------------------
//
inline CRefineView::CRefineView( MParentViewForFiltering& aParentView,
        CViewBase& aBaseView,
        MAlwaysIncludedContacts& aAlwaysIncluded ) 
        :   CFindViewBase( aParentView, aBaseView, EFalse ),
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
        CViewBase& aBaseView,
        MVPbkContactViewObserver& aExternalViewObserver,
        MVPbkContactFindPolicy& aContactFindPolicy,
        MAlwaysIncludedContacts& aAlwaysIncluded )
    {
    CRefineView* self = new ( ELeave ) CRefineView( aParentView, aBaseView, 
            aAlwaysIncluded );
    CleanupStack::PushL( self );
    self->BaseConstructL( aFindStrings, 
        aContactFindPolicy, aExternalViewObserver  );
    return self; 
    }

// --------------------------------------------------------------------------
// CRefineView::MatchL
// --------------------------------------------------------------------------
//
void CRefineView::MatchL( 
        RPointerArray<CCntModelViewContact>& aMatchedContacts )
    {
    CleanupClosePushL( aMatchedContacts ); 
    if ( iIdleUpdate )
        {
        iIdleUpdate->Cancel();
        }
        
    const TInt contactCount = iParentView.ContactCountL();
    for ( TInt i = 0; i < contactCount; ++i )
        {
        // iParentView is always VPbkCntModel view and the contacts type
        // is CViewContact
        const CViewContact& candidate = static_cast<const CViewContact&>(
            iParentView.ContactAtL( i ) );
        if ( IsContactAlwaysIncluded( candidate ) || IsMatchL( candidate ) )
            {
            // iParentView is always CFindView or CRefineView. The contacts type
            // is CViewContact. It can be safely casted. Const cast be done
            // because aContact is actually owned by CFindView.
            CCntModelViewContact* nativeContact = 
                const_cast<CCntModelViewContact*>( candidate.NativeContact() );
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
        RPointerArray<CCntModelViewContact>& aMatchedContacts )
    {
    if ( &iParentView == &aView && 
         IsMatchL( iParentView.ContactAtL( aIndex )))
        {
        // iParentView is always CFindView or CRefineView the contacts type
        // is CViewContact. It can be safely casted.
        const CViewContact& contact = static_cast<const CViewContact&>(
            iParentView.ContactAtL( aIndex ) );
        CCntModelViewContact* nativeContact = 
            const_cast<CCntModelViewContact*>( contact.NativeContact() );
        // Insert in correct location.
        aMatchedContacts.InsertInOrderL( nativeContact, 
                TLinearOrder<CCntModelViewContact>(
                    CCompareView::CompareFieldsL ) );
        }
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
        
    // Ignore aAlwaysIncludedContacts. This is
    // because in VPbkCntModel implementation CRefineView is bound to
    // it parent view CFindView. CFindView owns always included contacts.
    SetFindStringsL( aFindWords );
    // Start idle update.
    if ( !iIdleUpdate->IsActive() )
        {
        iIdleUpdate->Start( TCallBack( StartUpdateCallback, this ) ); 
        }
    }
    
// --------------------------------------------------------------------------
// CRefineView::IsContactAlwaysIncluded
// --------------------------------------------------------------------------
//
TBool CRefineView::IsContactAlwaysIncluded( 
        const CViewContact& aContact ) const
    {
    // Refine view doesn't keep always included contacts itself but it
    // forwards request to CFindView
    return iAlwaysIncluded.IsContactAlwaysIncluded( aContact );
    }

// --------------------------------------------------------------------------
// CRefineView::StartUpdateCallback
// --------------------------------------------------------------------------
//
TInt CRefineView::StartUpdateCallback( TAny* aThis )
    {
    CRefineView* view = static_cast<CRefineView*>( aThis );
    TRAPD( res, view->ActivateContactMatchL() )
    if ( res != KErrNone )
        {
        view->ContactViewError( *view, res, EFalse );
        }
    // Do not continue idle
    return EFalse;
    }
} // namespace VPbkCntModel

// End of File
