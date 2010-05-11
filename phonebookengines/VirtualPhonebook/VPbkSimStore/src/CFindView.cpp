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


#include "CFindView.h"

// From VPbkSimStore
#include "CContactStore.h"
#include "CContactView.h"
#include "CViewContact.h"
#include "CContactLink.h"

// VPbkEng
#include <MVPbkSimStoreOperation.h>
#include <MVPbkSimCntView.h>
#include <CVPbkSimContact.h>
#include <RVPbkStreamedIntArray.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkSimCntStore.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactFindPolicy.h>
#include <CVPbkContactNameConstructionPolicy.h>
#include <CVPbkContactViewSortPolicy.h>
#include <MVPbkContactBookmarkCollection.h>
#include <RVPbkStreamedIntArray.h>

// System includes
#include <cntviewbase.h>
#include <featmgr.h>

// Debugging headers
#include <VPbkProfile.h>
#include <VPbkDebug.h>

namespace VPbkSimStore {

// --------------------------------------------------------------------------
// CFindView::CFindView
// --------------------------------------------------------------------------
//
inline CFindView::CFindView( CContactView& aAllContactsView ) :        
        CFindViewBase( aAllContactsView, aAllContactsView, ETrue  )
    {
    }

// --------------------------------------------------------------------------
// CFindView::~CFindView
// --------------------------------------------------------------------------
//
CFindView::~CFindView()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStore:CFindView(%x)::~CFindView"), this );
         
    delete iFindOperation;
    delete iFindPolicy;
    iAlwaysIncluded.Close();
    iSIMMatchedContacts.Close();
    }

// --------------------------------------------------------------------------
// CFindView::ConstructL
// --------------------------------------------------------------------------
//
void CFindView::ConstructL( const MDesCArray& aFindStrings,
        MVPbkContactViewObserver& aExternalViewObserver,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
        RFs& aRFs )
    {
    CVPbkContactFindPolicy::TParam findPolicyParams(
        iAllContactsView.Store().MasterFieldTypeList(), aRFs );
    iFindPolicy = CVPbkContactFindPolicy::NewL( findPolicyParams );
    BaseConstructL( aExternalViewObserver, aFindStrings, 
        *iFindPolicy );
    
    SetAlwaysIncludedContactsL( aAlwaysIncludedContacts );
    }
    
// --------------------------------------------------------------------------
// CFindView::NewLC
// --------------------------------------------------------------------------
//
CFindView* CFindView::NewLC(      
        const MDesCArray& aFindStrings,
        CContactView& aAllContactsView,
        MVPbkContactViewObserver& aExternalViewObserver,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
        RFs& aRFs )
    {
    CFindView* self = new ( ELeave ) CFindView( aAllContactsView );
    CleanupStack::PushL( self );
    self->ConstructL( aFindStrings, aExternalViewObserver,
        aAlwaysIncludedContacts, aRFs );
    CleanupStack::Check( self );
    return self;
    }

// --------------------------------------------------------------------------
// CFindView::SetAlwaysIncludedContactsL
// --------------------------------------------------------------------------
//    
void CFindView::SetAlwaysIncludedContactsL( 
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    iAlwaysIncluded.Reset();

    if ( aAlwaysIncludedContacts )
        {
        MVPbkContactStore& store = ParentObject().ContactStore();
        const TInt count = aAlwaysIncludedContacts->Count();
        for ( TInt i = 0; i < count; ++i )
            {
            const CContactLink* bookmark =            
                dynamic_cast<const CContactLink*>( 
                    &aAlwaysIncludedContacts->At( i ) );
            // If bookmark was from VPbkCntModel and if it's from same store
            // as this view then it's added to array.
            if ( bookmark &&
                 &bookmark->ContactStore() == &store )
                {
                iAlwaysIncluded.AppendL( bookmark->SimIndex() );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CFindView::ContactViewReady
// --------------------------------------------------------------------------
//    
void CFindView::ContactViewReady( MVPbkContactViewBase& /*aView*/ )
    {
    // CFindView is only interested in ContactViewReadyForFiltering that
    // will start an asynchronous native match.
    }
    
// --------------------------------------------------------------------------
// CFindView::ContactViewReadyForFiltering
// --------------------------------------------------------------------------
//    
void CFindView::ContactViewReadyForFiltering(
        MParentViewForFiltering& aView )
    {
    // Cancel existing match if aready active.
    delete iFindOperation;
    iFindOperation = NULL;
    
    // Parent view is ready. Start asynchronous find from native SIM view.
    TRAPD( error,
        {
        iFindOperation = 
            iAllContactsView.NativeView().ContactMatchingPrefixL(
                FindStrings(), *this );
        });
    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStore:CFindView(%x)::ContactViewReadyForFiltering:\
        findOp(%x)"), this, iFindOperation ); 
        
    if ( error != KErrNone )
        {
        ContactViewError( aView, error, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CFindView::MatchL
// --------------------------------------------------------------------------
//
void CFindView::MatchL( RPointerArray<MVPbkSimContact>& aMatchedContacts )
    {
    CleanupResetAndDestroyPushL( aMatchedContacts );
    if (iAlwaysIncluded.Count() == 0)
        {
        // No always included contacts. The match can be done using
        // only the SIM view matched contacts.
        CViewContact* viewContact = CViewContact::NewL( iAllContactsView, 
            SortOrder() );
        CleanupStack::PushL(viewContact);
        MVPbkSimCntStore& simStore = iAllContactsView.Store().NativeStore();
        const TInt count = iSIMMatchedContacts.Count();
        for (TInt i = 0; i < count; ++i)
            {
            const TDesC8* cntBuf = simStore.AtL( iSIMMatchedContacts[i] );
            if ( cntBuf )
                {
                CVPbkSimContact* cnt = 
                    CVPbkSimContact::NewL( *cntBuf, simStore );
                CleanupStack::PushL( cnt );
                viewContact->SetSimContactL( *cnt );
                if ( IsMatchL( *viewContact ) )
                    {
                    // Keep the order of the contacts same and move contact
                    // from iContactsModelMatchContacts to aMatchedContacts
                    aMatchedContacts.AppendL( cnt );
                    CleanupStack::Pop( cnt );
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( cnt );
                    }
                }
            }   
        CleanupStack::PopAndDestroy( viewContact );
        }
    else
        {
        // Do it slowly by looping all the parent view contacts.
        const TInt contactCount = iParentView.ContactCountL();
        for ( TInt i = 0; i < contactCount; ++i )
            {
            // iParentView is always VPbkCntModel view and the contacts type
            // is CViewContact
            const CViewContact& candidate = static_cast<const CViewContact&>(
                iParentView.ContactAtL( i ) );
            MatchContactL( candidate, aMatchedContacts );
            }
        }
    
    // Free the memory
    iSIMMatchedContacts.Reset(); 
    CleanupStack::Pop( &aMatchedContacts );
    }
        
// --------------------------------------------------------------------------
// CFindView::ViewFindCompleted
// --------------------------------------------------------------------------
//      
void CFindView::ViewFindCompleted( 
        MVPbkSimCntView& aSimCntView,
        const RVPbkStreamedIntArray& aSimIndexArray )
    {
    TRAPD( error, ViewFindCompletedL( aSimCntView, aSimIndexArray ) );
    VPBK_PROFILE_END(VPbkProfile::ESimStoreFind);    
    
    if ( error != KErrNone )
        {
        ContactViewError( *this, error, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CFindView::ViewFindError
// --------------------------------------------------------------------------
//                  
void CFindView::ViewFindError( 
        MVPbkSimCntView& /*aSimCntView*/, 
        TInt aError )
    {
    ContactViewError( *this, aError, EFalse );
    }

// --------------------------------------------------------------------------
// CFindView::UpdateFilterL
// --------------------------------------------------------------------------
//
void CFindView::UpdateFilterL( 
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    SetFindStringsL( aFindWords );
    SetAlwaysIncludedContactsL( aAlwaysIncludedContacts );
    ActivateContactMatchL();
    }

// --------------------------------------------------------------------------
// CFindView::IsNativeMatchingRequestActive
// --------------------------------------------------------------------------
//
TBool CFindView::IsNativeMatchingRequestActive()
    {
    if ( iFindOperation )
        {
        return ETrue;
        }
    return EFalse;
    }
    
// --------------------------------------------------------------------------
// CFindView::IsContactAlwaysIncluded
// --------------------------------------------------------------------------
//
TBool CFindView::IsContactAlwaysIncluded( const CViewContact& aContact ) const
    {
    return iAlwaysIncluded.Find( aContact.SimIndex() ) != KErrNotFound;
    }
    
// --------------------------------------------------------------------------
// CFindView::DoContactAddedToViewL
// --------------------------------------------------------------------------
//
void CFindView::DoContactAddedToViewL(
        MVPbkContactViewBase& aView, 
        TInt aIndex, 
        const MVPbkContactLink& /*aContactLink*/,
        RPointerArray<MVPbkSimContact>& aMatchedContacts )
    {
    CleanupResetAndDestroyPushL( aMatchedContacts );
    if ( &iParentView == &aView )
        {
        const CViewContact& viewContact = static_cast<const CViewContact&>(
            iParentView.ContactAtL( aIndex ) );
        if ( IsMatchL( viewContact ) )
            {
            MVPbkSimCntStore& simStore = 
                iAllContactsView.Store().NativeStore();
            const TDesC8* cntBuf = simStore.AtL( viewContact.SimIndex() );
            if ( cntBuf )
                {
                CVPbkSimContact* cnt = 
                    CVPbkSimContact::NewL( *cntBuf, simStore );
                CleanupStack::PushL( cnt );
                
                // We have to insert the new contact to the correct
                // location. VPbk sort policy is used for that.
                CVPbkContactViewSortPolicy::TParam param(
                    iAllContactsView.Store().MasterFieldTypeList(), SortOrder() );
                CVPbkContactViewSortPolicy* sortPolicy = 
                    CVPbkContactViewSortPolicy::NewL( param );
                CleanupStack::PushL( sortPolicy );
                sortPolicy->SortStartL();
                CViewContact* current = 
                    CViewContact::NewL( iAllContactsView, SortOrder() );
                CleanupStack::PushL( current );
                
                const TInt count = aMatchedContacts.Count();
                TInt index = KErrNotFound;
                for ( TInt i = 0; i < count && index == KErrNotFound; ++i )
                    {
                    current->SetSimContactL( *aMatchedContacts[i] );
                    TInt res = 
                        sortPolicy->CompareContacts( viewContact, *current );
                    // CompareContacts returns -1, 0 or 1.
                    if ( res < 0 )
                        {
                        index = i;
                        }
                    }
                sortPolicy->SortCompleted();
                CleanupStack::PopAndDestroy( 2, sortPolicy ); // current
                if ( index != KErrNotFound )
                    {
                    // Contact is inserted to matched contacts list
                    aMatchedContacts.InsertL( cnt, index );
                    }
                else
                    {
                    // List is empty so append contact to it
                    aMatchedContacts.AppendL( cnt );
                    }
                CleanupStack::Pop( cnt );
                }
            }
        }
    CleanupStack::Pop( &aMatchedContacts );
    }

// --------------------------------------------------------------------------
// CFindView::ViewFindCompletedL
// --------------------------------------------------------------------------
//  
void CFindView::ViewFindCompletedL( 
        MVPbkSimCntView& /*aSimCntView*/,
        const RVPbkStreamedIntArray& aSimIndexArray )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStore:CFindView(%x)::ViewFindCompletedL:found %d contacts"),
         this, aSimIndexArray.Count() );
         
    // Native SIM view find is ready.
    
    // Clean the table
    iSIMMatchedContacts.Reset();
    
    // Copy matched indexes
    const TInt count( aSimIndexArray.Count() );
    for ( TInt i(0); i < count; ++i )
        {
        iSIMMatchedContacts.AppendL( aSimIndexArray[i] );
        }
    
    // The operation is not needed after indexes have been copied
    delete iFindOperation;
    iFindOperation = NULL;
    
    // Call base class matching function. It will send internal observer
    // event that this view is ready for filtering.
    MatchContactsL();
    // Send view ready event to both internal&external observers.
    SendViewStateEventToObservers();
    }

// --------------------------------------------------------------------------
// CFindView::MatchContactL
// --------------------------------------------------------------------------
//  
void CFindView::MatchContactL( const CViewContact& aViewContact,
            RPointerArray<MVPbkSimContact>& aMatchedContacts )
    {
    CleanupResetAndDestroyPushL( aMatchedContacts );
    // aContact matches if it's one of the always included contacts OR
    // (if it's one of Contacts Model matched contacts AND it also
    // passes our own match).
    TInt matchArrayIndex = KErrNotFound;
    TBool matched = EFalse;
    if ( IsContactAlwaysIncluded( aViewContact ) )
        {
        // Remove from match array to save memory
        RemoveFromMatchArrayIfFound( aViewContact );
        matched = ETrue;
        }
    else if ( IsSIMMatchL( aViewContact, matchArrayIndex ) )
        {
        // Remove from match array to save memory
        iSIMMatchedContacts.Remove( matchArrayIndex );
        iSIMMatchedContacts.Compress();
        
        if ( IsMatchL( aViewContact ) )
            {
            matched = ETrue;
            }
        }
    
    if ( matched )
        {
        // Contact matched.
        // CFindView owns its contacts so create a copy
        MVPbkSimCntStore& simStore = iAllContactsView.Store().NativeStore();
        const TDesC8* cntBuf = simStore.AtL( aViewContact.SimIndex() );
        if ( cntBuf )
            {
            CVPbkSimContact* cnt = 
                CVPbkSimContact::NewL( *cntBuf, simStore );
            CleanupStack::PushL( cnt );
            aMatchedContacts.AppendL( cnt );
            CleanupStack::Pop( cnt );
            }
        }
    CleanupStack::Pop( &aMatchedContacts ); 
    }

// --------------------------------------------------------------------------
// CFindView::RemoveFromMatchArrayIfFound
// --------------------------------------------------------------------------
//  
void CFindView::RemoveFromMatchArrayIfFound( const CViewContact& aViewContact )
    {
    TInt index = FindFromMatchArray( aViewContact );
    if ( index != KErrNotFound )
        {
        iSIMMatchedContacts.Remove( index );
        iSIMMatchedContacts.Compress();
        }
    }

// --------------------------------------------------------------------------
// CFindView::IsSIMMatchL
// --------------------------------------------------------------------------
//  
TBool CFindView::IsSIMMatchL( const CViewContact& aContact,
        TInt& aMatchArrayIndex ) const
    {
    aMatchArrayIndex = FindFromMatchArray( aContact );
    return aMatchArrayIndex != KErrNotFound;
    }

// --------------------------------------------------------------------------
// CFindView::FindFromMatchArray
// --------------------------------------------------------------------------
//  
TInt CFindView::FindFromMatchArray( const CViewContact& aContact ) const
    {
    return iSIMMatchedContacts.Find( aContact.SimIndex() );
    }
} // namespace VPbkSimStore 
// End of File
