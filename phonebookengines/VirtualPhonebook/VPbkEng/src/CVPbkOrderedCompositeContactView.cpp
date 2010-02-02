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
* Description:  Ordered composite contact view.
*
*/


#include "CVPbkOrderedCompositeContactView.h"

// Debugging headers
#include <VPbkProfile.h>

// --------------------------------------------------------------------------
// CVPbkOrderedCompositeContactView::CVPbkOrderedCompositeContactView
// --------------------------------------------------------------------------
//
inline CVPbkOrderedCompositeContactView::CVPbkOrderedCompositeContactView()
    {
    }

// --------------------------------------------------------------------------
// CVPbkOrderedCompositeContactView::NewLC
// --------------------------------------------------------------------------
//
CVPbkOrderedCompositeContactView* CVPbkOrderedCompositeContactView::NewLC(
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder)
	{
	CVPbkOrderedCompositeContactView* self =
        new(ELeave) CVPbkOrderedCompositeContactView;
	CleanupStack::PushL(self);
	self->ConstructL(aSortOrder);
    self->AddObserverL(aObserver);
	return self;
	}

// --------------------------------------------------------------------------
// CVPbkOrderedCompositeContactView::ConstructL
// --------------------------------------------------------------------------
//
inline void CVPbkOrderedCompositeContactView::ConstructL
        (const MVPbkFieldTypeList& aSortOrder)
	{
	BaseConstructL(aSortOrder);
	}

// --------------------------------------------------------------------------
// CVPbkOrderedCompositeContactView::CreateFilteredViewLC
// --------------------------------------------------------------------------
//	
MVPbkContactViewBase* 
    CVPbkOrderedCompositeContactView::CreateFilteredViewLC( 
        MVPbkContactViewObserver& aObserver,
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    CVPbkOrderedCompositeContactView* compositeView = 
        CVPbkOrderedCompositeContactView::NewLC( 
            aObserver, SortOrder() );
    
    // If this composite is internal composite view then also
    // the created composite is internal.
    if ( CompositePolicy().InternalPolicy() )
        {
        compositeView->ApplyInternalCompositePolicyL();
        }
        
    const TInt count( iSubViews.Count() );
    for( TInt i(0); i < count; ++i )
        {
        MVPbkContactViewFiltering* filtering = 
            iSubViews[i]->iView->ViewFiltering();
        if ( filtering )
            {
            MVPbkContactViewBase* filteredSubView = 
                filtering->CreateFilteredViewLC( *compositeView, 
                    aFindWords, aAlwaysIncludedContacts );
            compositeView->AddSubViewL( filteredSubView );
            CleanupStack::Pop(); //filteredSubView
            }
        }
    return compositeView;
    }

// --------------------------------------------------------------------------
// CVPbkOrderedCompositeContactView::DoBuildContactMappingL
// Builds view mapping.
// --------------------------------------------------------------------------
//
void CVPbkOrderedCompositeContactView::DoBuildContactMappingL()
	{
	VPBK_PROFILE_START(VPbkProfile::EOrderedCompositeViewMapping);
    // Reset the mappings	
    iContactMapping.Reset();

    TContactMapping contactMapping;
    const TInt viewCount = iSubViews.Count();
    for (TInt i = 0; i < viewCount; ++i)
        {
        // Ignore not ready views
        if (iSubViews[i]->iState != CSubViewData::EReady)
            {
            continue;
            }

        // Build mapping
        contactMapping.iViewIndex = i;
        const TInt contactCount = iSubViews[i]->iView->ContactCountL();
        for (TInt j = 0; j < contactCount; ++j)
            {
            contactMapping.iContactIndex = j;
            User::LeaveIfError(iContactMapping.Append(contactMapping));
            }
        }
    VPBK_PROFILE_END(VPbkProfile::EOrderedCompositeViewMapping);        
	}

// --------------------------------------------------------------------------
// CVPbkOrderedCompositeContactView::DoHandleContactAdditionL
// Handles contact addition to the view.
// --------------------------------------------------------------------------
//
TInt CVPbkOrderedCompositeContactView::DoHandleContactAdditionL
        (TInt aSubViewIndex, TInt aIndex)
    {
    TInt index = KErrNotFound;
    
    if (aSubViewIndex != KErrNotFound)
        {
        // Build mapping
        TContactMapping mapping;
        mapping.iViewIndex = aSubViewIndex;
        mapping.iContactIndex = aIndex;

        // Insert in order
        User::LeaveIfError( iContactMapping.InsertInOrderAllowRepeats
            ( mapping,
            &CVPbkOrderedCompositeContactView::CompareOrderedMappings ) );

        // Find out the new index
        TIdentityRelation<TContactMapping> comparisonOperator
            ( &CVPbkCompositeContactView::CompareMappings );
        index = iContactMapping.Find( mapping, comparisonOperator );
        }

    return index;
    }

// --------------------------------------------------------------------------
// CVPbkOrderedCompositeContactView::CompareOrderedMappings
// Comparison function for the mappings.
// Returns zero if the mappings are the same. Returns a negative value
// if aLhs comes first and positive if aRhs comes first.
// --------------------------------------------------------------------------
//
TInt CVPbkOrderedCompositeContactView::CompareOrderedMappings
        ( const TContactMapping& aLhs, const TContactMapping& aRhs )
    {
    // First compare view indexes
    TInt ret = aLhs.iViewIndex - aRhs.iViewIndex;
    
    if ( ret == 0 )
        {
        // If view indexes are the same, then compare the contact indexes
        ret = aLhs.iContactIndex - aRhs.iContactIndex;
        }

    return ret;
    }

// End of File
