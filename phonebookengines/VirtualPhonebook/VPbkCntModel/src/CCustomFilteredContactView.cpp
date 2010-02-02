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
* Description:  Contacts Model store custom filtered contact
*                view implementation. Derives from MVPbkContactViewBase,
*                performs the custom filtering.
*
*/


#include "CCustomFilteredContactView.h"

// VPbkCntModel
#include "CContactStore.h"
#include "CContact.h"
#include "CCustomFilteredNativeContactView.h"

// VPbkEng
#include <MVPbkFieldTypeSelector.h>
#include <CVPbkFilteredContactView.h>
#include <MVPbkContactSelector.h>


namespace VPbkCntModel {

// --------------------------------------------------------------------------
// CCustomFilteredContactView::CCustomFilteredContactView
// --------------------------------------------------------------------------
//
CCustomFilteredContactView::CCustomFilteredContactView
          ( CContactStore& aContactStore,
            const MVPbkFieldTypeSelector* aFieldTypeSelector,
            MCustomContactViewObserver& aCustomContactViewObserver,
            MVPbkContactSelector* aContactSelector ) :
                CViewBase( aContactStore ),
                iContactStore( aContactStore ),
                iFieldTypeSelector( aFieldTypeSelector ),
                iCustomContactViewObserver( aCustomContactViewObserver ),
                iContactSelector( aContactSelector )
    {
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::ConstructL
// --------------------------------------------------------------------------
//
void CCustomFilteredContactView::ConstructL(
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aViewObserver,                
        const MVPbkFieldTypeList& aSortOrder,
        CContactViewBase& aNativeViewBase)
    {
    // iView should be assign first because is used by base class during the 
    // construction.
    iView = &aNativeViewBase;
    ConstructL( aViewDefinition, aViewObserver, aSortOrder );
    // Set the customically filtered view parameters
    iCustomFilteredContactView->SetNativeBaseView( aNativeViewBase );
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::~CCustomFilteredContactView
// --------------------------------------------------------------------------
//
CCustomFilteredContactView::~CCustomFilteredContactView()
    {
    delete iCustomFilteredContactView;
    delete iFilteredView;
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::ConstructL
// --------------------------------------------------------------------------
//
void CCustomFilteredContactView::ConstructL( 
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder )
    {
    // Base construct has to be done first
    CViewBase::ConstructL( aViewDefinition, aSortOrder );

    // Construct the custom filtered native view
    iCustomFilteredContactView = CCustomFilteredNativeContactView::NewL
        ( iContactStore );

    // This object is the base view and contact selector
    // for the Virtual Phonebook filtered view. The base view calls
    // are handled by CViewBase.
    // The selector logic is implemented here in this class.
    iFilteredView = CVPbkFilteredContactView::NewL
        ( *this, aObserver, *this );
    iCustomFilteredContactView->SetCustomFilterView( *iFilteredView );
    }
    
// --------------------------------------------------------------------------
// CCustomFilteredContactView::ContactViewBase
// --------------------------------------------------------------------------
//
CContactViewBase& CCustomFilteredContactView::ContactViewBase()
    {
    return *iCustomFilteredContactView;
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::Type
// --------------------------------------------------------------------------
//
TVPbkContactViewType CCustomFilteredContactView::Type() const
    {
    return EVPbkContactsView;
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::DoInitializeViewL
// --------------------------------------------------------------------------
//
void CCustomFilteredContactView::DoInitializeViewL( 
        const CVPbkContactViewDefinition& /*aViewDefinition*/,
        RContactViewSortOrder& /*aViewSortOrder*/ )
    {
    // Required view initialization is already done
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::DoTeardownView
// --------------------------------------------------------------------------
//
void CCustomFilteredContactView::DoTeardownView()
    {
    /// Do nothing
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::DoChangeSortOrderL
// --------------------------------------------------------------------------
//
TBool CCustomFilteredContactView::DoChangeSortOrderL( 
        const CVPbkContactViewDefinition& /*aViewDefinition*/,
        RContactViewSortOrder& /*aSortOrder*/ )
    {
    // Sort order not changed.
    return EFalse;
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::IsContactIncluded
// --------------------------------------------------------------------------
//
TBool CCustomFilteredContactView::IsContactIncluded
        ( const MVPbkBaseContact& aContact )
    {
    TBool ret = EFalse;

    // In case of an error, ignore it.
    // The contact will then not be included in the view
    TRAP_IGNORE(
        ret = IsContactIncludedL( aContact )
        );

    return ret;
    }

// --------------------------------------------------------------------------
// CCustomFilteredContactView::HandleContactViewEvent
// --------------------------------------------------------------------------
//
void CCustomFilteredContactView::HandleContactViewEvent
        ( const CContactViewBase& aView,
          const TContactViewEvent& aEvent )
    {
    // Notify the custom view observer first
    iCustomContactViewObserver.HandleCustomContactViewEvent( aView, aEvent );
    CViewBase::HandleContactViewEvent( aView, aEvent );
    }


inline TBool CCustomFilteredContactView::IsContactIncludedL
        ( const MVPbkBaseContact& aContact )
    {
    TBool fieldSelectorAccepted = ETrue;
    TBool contactSelectorAccepted = ETrue;
    if ( iFieldTypeSelector )
        {
        fieldSelectorAccepted = IsContactIncludedByFieldSelectorL( aContact );
        }
    if ( iContactSelector )
        {
        contactSelectorAccepted = iContactSelector->IsContactIncluded( aContact );
        }
    // The contact is included if neither selector returns false.
    return fieldSelectorAccepted && contactSelectorAccepted;
    }

inline TBool CCustomFilteredContactView::IsContactIncludedByFieldSelectorL
        ( const MVPbkBaseContact& aContact )
    {
    TBool ret = EFalse;

    // First we have to get the contact id
    const CViewContact& viewContact =
        static_cast<const CViewContact&>( aContact );
    TContactItemId id = viewContact.Id();

    // Then we have to load the native store contact
    CContactItem* item = iContactStore.NativeDatabase().ReadContactLC( id );
    // Make a VPbk store contact out of it
    CContact* contact = CContact::NewL( iContactStore, item );
    CleanupStack::Pop( item ); // ownership was taken away
    CleanupStack::PushL( contact );

    MVPbkStoreContactFieldCollection& fields = contact->Fields();
    TInt fieldCount = fields.FieldCount();

    for ( TInt i = 0; i < fieldCount; ++i )
        {
        const MVPbkStoreContactField& field = fields.FieldAt( i );
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if ( fieldType && iFieldTypeSelector->IsFieldTypeIncluded( *fieldType ) )
            {
            ret = ETrue;
            break;
            }
        }

    CleanupStack::PopAndDestroy( contact );

    return ret;
    }

} // namespace VPbkCntModel

// End of File
