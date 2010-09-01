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


#include "CCustomFilteredGroupView.h"

// VPbkCntModel
#include "CContactStore.h"
#include "CContact.h"
#include "cntitem.h"
#include "CCustomFilteredNativeContactView.h"

// VPbkEng
#include <MVPbkFieldTypeSelector.h>
#include <CVPbkFilteredContactView.h>
#include <MVPbkContactLinkArray.h>
#include <CVPbkContactViewDefinition.h>


namespace VPbkCntModel {

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::CCustomFilteredGroupView
// --------------------------------------------------------------------------
//
CCustomFilteredGroupView::CCustomFilteredGroupView
          ( CContactStore& aContactStore,
            const MVPbkFieldTypeSelector* aFieldTypeSelector,
            MCustomContactViewObserver& aCustomContactViewObserver ) :
                CViewBase( aContactStore ),
                iContactStore( aContactStore ),
                iFieldTypeSelector( aFieldTypeSelector ),
                iCustomContactViewObserver( aCustomContactViewObserver )
    {
    }

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::ConstructL
// --------------------------------------------------------------------------
//
void CCustomFilteredGroupView::ConstructL(
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
// CCustomFilteredGroupView::~CCustomFilteredGroupView
// --------------------------------------------------------------------------
//
CCustomFilteredGroupView::~CCustomFilteredGroupView()
    {
    delete iCustomFilteredContactView;
    delete iFilteredView;
    }

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::ConstructL
// --------------------------------------------------------------------------
//
void CCustomFilteredGroupView::ConstructL( 
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
// CCustomFilteredGroupView::ContactViewBase
// --------------------------------------------------------------------------
//
CContactViewBase& CCustomFilteredGroupView::ContactViewBase()
    {
    return *iCustomFilteredContactView;
    }

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::Type
// --------------------------------------------------------------------------
//
TVPbkContactViewType CCustomFilteredGroupView::Type() const
    {
    return EVPbkGroupsView;
    }

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::DoInitializeViewL
// --------------------------------------------------------------------------
//
void CCustomFilteredGroupView::DoInitializeViewL( 
        const CVPbkContactViewDefinition& /*aViewDefinition*/,
        RContactViewSortOrder& /*aViewSortOrder*/ )
    {
    // Required view initialization is already done
    }

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::DoTeardownView
// --------------------------------------------------------------------------
//
void CCustomFilteredGroupView::DoTeardownView()
    {
    /// Do nothing
    }

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::DoChangeSortOrderL
// --------------------------------------------------------------------------
//
TBool CCustomFilteredGroupView::DoChangeSortOrderL( 
        const CVPbkContactViewDefinition& /*aViewDefinition*/,
        RContactViewSortOrder& /*aSortOrder*/ )
    {
    // Sort order not changed.
    return EFalse;
    }

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::IsContactIncluded
// --------------------------------------------------------------------------
//
TBool CCustomFilteredGroupView::IsContactIncluded
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
// CCustomFilteredGroupView::HandleContactViewEvent
// --------------------------------------------------------------------------
//
void CCustomFilteredGroupView::HandleContactViewEvent
        ( const CContactViewBase& aView,
          const TContactViewEvent& aEvent )
    {
    // Notify the custom view observer first
    iCustomContactViewObserver.HandleCustomContactViewEvent( aView, aEvent );
    CViewBase::HandleContactViewEvent( aView, aEvent );
    }

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::IsContactFieldIncludedL
// --------------------------------------------------------------------------
//
inline TBool CCustomFilteredGroupView::IsContactFieldIncludedL(
		const CContactIdArray* aGroupMembers,
		const TInt& aMemberCount,
		const MVPbkFieldTypeSelector& aFieldTypeSelector )
	{
	TBool ret = EFalse;
	
	for ( TInt i=0; i < aMemberCount && !ret; ++i )
		{
        const TContactItemId contactId = ( *aGroupMembers )[i];
        CContactItem* item = iContactStore.NativeDatabase().
            ReadContactLC( contactId ) ;

        // Make a VPbk store contact
        CContact* contact = CContact::NewL( iContactStore, item );
        CleanupStack::Pop( item ); // ownership was taken away
        CleanupStack::PushL( contact );

        MVPbkStoreContactFieldCollection& fields = contact->Fields();
        TInt fieldCount = fields.FieldCount();

        for ( TInt i = 0; i < fieldCount; ++i )
            {
            const MVPbkStoreContactField& field = fields.FieldAt( i );
            const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
            if ( fieldType && 
            		aFieldTypeSelector.IsFieldTypeIncluded( *fieldType ) )
                {
                ret = ETrue;
                break;
                }
            }

        CleanupStack::PopAndDestroy( contact );
    	}
	
	return ret;
	}

// --------------------------------------------------------------------------
// CCustomFilteredGroupView::IsContactIncludedL
// --------------------------------------------------------------------------
//
inline TBool CCustomFilteredGroupView::IsContactIncludedL
        ( const MVPbkBaseContact& aContact )
    {
    TBool ret = EFalse;

    // First we have to get the contact id
    const CViewContact& viewContact =
        static_cast<const CViewContact&>( aContact );
    TContactItemId id = viewContact.Id();

    // Then we have to load the native contact group
    CContactGroup* group = static_cast<CContactGroup*>
        ( iContactStore.NativeDatabase().ReadContactLC( id ) );

    const CContactIdArray* groupMembers = group->ItemsContained();
    if ( groupMembers )
        {
        CVPbkContactViewDefinition& viewDefinition = ViewDefinition();
        const TInt memberCount = groupMembers->Count();
        
    	if( memberCount > 0 )
    		{
    		if( iFieldTypeSelector )
    			{
    			ret = IsContactFieldIncludedL( groupMembers, memberCount, *iFieldTypeSelector );
    			}
    		else
    			{
    			// No group filter given, so group is included no matter what
    			ret = ETrue;
    			}
    		}
    	else if( !viewDefinition.FlagIsOn( EVPbkExcludeEmptyGroups ) )
			{
			ret = ETrue;
			}
        }

    CleanupStack::PopAndDestroy( group );

    return ret;
    }

} // namespace VPbkCntModel

// End of File
