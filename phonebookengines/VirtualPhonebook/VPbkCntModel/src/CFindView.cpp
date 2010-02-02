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
#include "CFindView.h"

// VPbkCntModel
#include "CRefineView.h"
#include "CViewBase.h"
#include "CContactBookmark.h"
#include "CContactStore.h"

// From Virtual Phonebook
#include <CVPbkContactNameConstructionPolicy.h>
#include <CVPbkContactFindPolicy.h>
#include <MVPbkContactBookmarkCollection.h>

// System includes
#include <cntdef.h>

// Debugging headers
#include <VPbkProfile.h>

namespace VPbkCntModel {

// --------------------------------------------------------------------------
// CFindView::CFindView
// --------------------------------------------------------------------------
//
inline CFindView::CFindView( CViewBase& aBaseView )
    :   CFindViewBase( aBaseView, aBaseView, ETrue )
    {
    }

// --------------------------------------------------------------------------
// CFindView::ConstructL
// --------------------------------------------------------------------------
//
void CFindView::ConstructL( const MDesCArray& aFindStrings,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
        MVPbkContactViewObserver& aExternalViewObserver, RFs& aRFs )
    {
    CVPbkContactFindPolicy::TParam findPolicyParams(
        iBaseView.Store().MasterFieldTypeList(), aRFs );
    iFindPolicy = CVPbkContactFindPolicy::NewL( findPolicyParams );
    BaseConstructL( aFindStrings, *iFindPolicy,
        aExternalViewObserver );
    SetAlwaysIncludedContactsL( aAlwaysIncludedContacts );
    }

// --------------------------------------------------------------------------
// CFindView::NewLC
// --------------------------------------------------------------------------
//
CFindView* CFindView::NewLC(
        const MDesCArray& aFindStrings,
        CViewBase& aParentView,
        MVPbkContactViewObserver& aExternalViewObserver,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
        RFs& aRFs )
    {
    CFindView* self =
        new ( ELeave ) CFindView( aParentView );
    CleanupStack::PushL( self );
    self->ConstructL( aFindStrings, aAlwaysIncludedContacts,
        aExternalViewObserver, aRFs );
    return self;
    }

// --------------------------------------------------------------------------
// CFindView::~CFindView
// --------------------------------------------------------------------------
//
CFindView::~CFindView()
    {
    delete iFindPolicy;
    iAlwaysIncluded.Close();
    iContactsModelMatchContacts.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CFindView::SetAlwaysIncludedContactsL
// --------------------------------------------------------------------------
//
void CFindView::SetAlwaysIncludedContactsL(
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    // Destroy old ids
    iAlwaysIncluded.Reset();

    if ( aAlwaysIncludedContacts )
        {
        MVPbkContactStore& store = ParentObject().ContactStore();
        const TInt count = aAlwaysIncludedContacts->Count();
        for ( TInt i = 0; i < count; ++i )
            {
            const CContactBookmark* bookmark =
                dynamic_cast<const CContactBookmark*>(
                    &aAlwaysIncludedContacts->At( i ) );
            // If bookmark was from VPbkCntModel and if it's from same store
            // as this view then it's added to array.
            if ( bookmark &&
                 &bookmark->ContactStore() == &store )
                {
                iAlwaysIncluded.AppendL( bookmark->ContactId() );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CFindView::MatchL
// --------------------------------------------------------------------------
//
void CFindView::MatchL(
        RPointerArray<CCntModelViewContact>& aMatchedContacts )
    {
    iContactsModelMatchContacts.ResetAndDestroy();

    VPBK_PROFILE_START(VPbkProfile::ECntModelFind);
    // Get matches from Contacts Model
    iBaseView.NativeView().ContactsMatchingPrefixL(
        FindStrings(), iContactsModelMatchContacts );
    VPBK_PROFILE_END(VPbkProfile::ECntModelFind);

    if (iAlwaysIncluded.Count() == 0)
        {
        // No always included contacts. The match can be done using
        // only the Contacts Model matched contacts.
        CViewContact* viewContact = CViewContact::NewL( iBaseView, SortOrder() );
        CleanupStack::PushL(viewContact);
        const TInt count = iContactsModelMatchContacts.Count();
        for (TInt i = 0; i < count; ++i)
            {
            viewContact->SetViewContact( *iContactsModelMatchContacts[i] );
            if ( IsMatchL( *viewContact ) )
                {
                // Keep the order of the contacts same and move contact
                // from iContactsModelMatchContacts to aMatchedContacts
                aMatchedContacts.AppendL(iContactsModelMatchContacts[i]);
                // Don't remove the contact to keep indexes correct.
                // Set to NULL so that ResetAndDestroy doesn't crash in
                // destructor
                iContactsModelMatchContacts[i] = NULL;
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

    iContactsModelMatchContacts.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CFindView::DoContactAddedToViewL
// --------------------------------------------------------------------------
//
void CFindView::DoContactAddedToViewL( MVPbkContactViewBase& aView,
        TInt aIndex, const MVPbkContactLink& /*aContactLink*/,
        RPointerArray<CCntModelViewContact>& aMatchedContacts )
    {
    if ( &iParentView == &aView )
        {
        const CViewContact& viewContact = static_cast<const CViewContact&>(
            iParentView.ContactAtL( aIndex ) );
        if ( IsMatchL( viewContact ) )
            {
            CCntModelViewContact* cnt =
                CCntModelViewContact::NewL( *viewContact.NativeContact() );
            CleanupStack::PushL( cnt );
            aMatchedContacts.InsertInOrderAllowRepeatsL( cnt,
                TLinearOrder<CCntModelViewContact>(
                    CCompareView::CompareFieldsL ) );
            CleanupStack::Pop( cnt );
            }
        }
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
// CFindView::MatchContactL
// --------------------------------------------------------------------------
//
void CFindView::MatchContactL( const CViewContact& aViewContact,
        RPointerArray<CCntModelViewContact>& aMatchedContacts )
    {
    // aContact matches if it's one of the always included contacts OR
    // if it's one of Contacts Model matched contacts AND it also
    // passes our own match.
    TInt matchArrayIndex = KErrNotFound;
    TBool matched = EFalse;
    if ( IsContactAlwaysIncluded( aViewContact ) )
        {
        // Remove from match array to save memory
        RemoveFromMatchArrayIfFound( aViewContact );
        matched = ETrue;
        }
    else if ( IsContactsModelMatchL( aViewContact, matchArrayIndex ) )
        {
        // Remove from match array to save memory
        delete iContactsModelMatchContacts[matchArrayIndex];
        iContactsModelMatchContacts.Remove( matchArrayIndex );

        if ( IsMatchL( aViewContact ) )
            {
            matched = ETrue;
            }
        }

    if ( matched )
        {
        // Contact matched.
        // CFindView owns its contacts so create a copy
        CCntModelViewContact* cnt =
            CCntModelViewContact::NewL( *aViewContact.NativeContact() );
        CleanupStack::PushL( cnt );
        aMatchedContacts.AppendL( cnt );
        CleanupStack::Pop( cnt );
        }
    }

// --------------------------------------------------------------------------
// CFindView::IsContactAlwaysIncluded
// --------------------------------------------------------------------------
//
TBool CFindView::IsContactAlwaysIncluded(
        const CViewContact& aContact ) const
    {
    return iAlwaysIncluded.Find( aContact.Id() ) != KErrNotFound;
    }

// --------------------------------------------------------------------------
// CFindView::IsContactsModelMatchL
// --------------------------------------------------------------------------
//
TBool CFindView::IsContactsModelMatchL( const CViewContact& aContact,
        TInt& aMatchArrayIndex ) const
    {
    // Contacts are in order in iContactsModelMatchContacts. Use binary
    // search to check if aContact is can be found from the array.
    aMatchArrayIndex = FindFromMatchArray( aContact );
    return aMatchArrayIndex != KErrNotFound;
    }

// --------------------------------------------------------------------------
// CFindView::RemoveFromMatchArrayIfFound
// --------------------------------------------------------------------------
//
void CFindView::RemoveFromMatchArrayIfFound( const CViewContact& aContact )
    {
    TInt index = FindFromMatchArray( aContact );
    if ( index != KErrNotFound )
        {
        delete iContactsModelMatchContacts[index];
        iContactsModelMatchContacts.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// CFindView::FindFromMatchArray
// --------------------------------------------------------------------------
//
TInt CFindView::FindFromMatchArray( const CViewContact& aContact ) const
    {
    return iContactsModelMatchContacts.FindInOrder(
        aContact.NativeContact(),
        TLinearOrder<CCntModelViewContact>( CCompareView::CompareFieldsL ) );
    }
} // namespace VPbkCntModel
// End of File
