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
* Description:  Sorted composite contact view.
*
*/


#include "CVPbkSortedCompositeContactView.h"

// Virtual Phonebook
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkViewContact.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkContactViewSortPolicy.h>
#include <MVPbkContactView.h>

// Debugging headers
#include <VPbkProfile.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Gets field text from field.
 *
 * @param aField    The field to get text from.
 * @return  Field text.
 */
inline TPtrC FieldText(const MVPbkBaseContactField& aField)
    {
    TPtrC result;
    const MVPbkContactFieldData& fieldData = aField.FieldData();
    if (fieldData.DataType() == EVPbkFieldStorageTypeText)
        {
        result.Set( MVPbkContactFieldTextData::Cast(fieldData).Text() );
        }
    return result;
    }

/**
 * Fallback for sort policy if there is no
 * sort policy plugin installed to the system.
 */
NONSHARABLE_CLASS(TSortPolicy) : public MVPbkContactViewSortPolicy
    {
    public:

        /**
         * Constructor.
         */
        TSortPolicy() {}

    private: // From MVPbkContactViewSortPolicy
        void SetSortOrderL(
                const MVPbkFieldTypeList& /* aSortOrder */)
            {}
        void SortStartL()
            {}
        void SortCompleted()
            {}
        TInt CompareContacts(
                const MVPbkViewContact& aLhs,
                const MVPbkViewContact& aRhs );
        TInt CompareItems(
                const MVPbkSortKeyArray& /* aLhs */, 
                const MVPbkSortKeyArray& /* aRhs */ ) const 
            {
            // Dummy, this method not used in this class
            return 0;
            };
    };

// --------------------------------------------------------------------------
// TSortPolicy::CompareContacts
// --------------------------------------------------------------------------
//
TInt TSortPolicy::CompareContacts
        ( const MVPbkViewContact& aLhs, const MVPbkViewContact& aRhs )
    {
    const MVPbkBaseContactFieldCollection& lhsFields = aLhs.Fields();
    const MVPbkBaseContactFieldCollection& rhsFields = aRhs.Fields();
    const TInt lhsCount = lhsFields.FieldCount();
    const TInt rhsCount = rhsFields.FieldCount();
    for (TInt i = 0; i < lhsCount && i < rhsCount; ++i)
        {
        const MVPbkBaseContactField& lhsField = lhsFields.FieldAt(i);
        const MVPbkBaseContactField& rhsField = rhsFields.FieldAt(i);
        const TInt result = FieldText(lhsField).CompareC(FieldText(rhsField));
        if (result != 0)
            {
            return result;
            }
        }
    return lhsCount - rhsCount;
    }

// --------------------------------------------------------------------------
// CompareSorting
// Comparison function, which utilizes sort policy.
// --------------------------------------------------------------------------
//
inline TInt CompareSorting
        ( const MVPbkViewContact* aLhs, const MVPbkViewContact* aRhs,
          MVPbkContactViewSortPolicy& aSortPolicy  )
    {
    // Default return value is one, which holds for the situation
    // where aLhs is NULL
    TInt ret = 1;
    if (!aLhs)
        {
        // Default holds
        }
    else if (!aRhs)
        {
        ret = -1;
        }
    else
        {
        // Use sort policy to compare contacts
        ret = aSortPolicy.CompareContacts( *aLhs, *aRhs );
        }

    return ret;
    }

} /// namespace


// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::CVPbkSortedCompositeContactView
// --------------------------------------------------------------------------
//
inline CVPbkSortedCompositeContactView::CVPbkSortedCompositeContactView(
        const MVPbkFieldTypeList& aFieldTypes,
        RFs& aFs) :
    iFieldTypes(aFieldTypes),
    iFs(aFs)
    {
    }

// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::~CVPbkSortedCompositeContactView
// --------------------------------------------------------------------------
//
CVPbkSortedCompositeContactView::~CVPbkSortedCompositeContactView()
    {
    delete iSortPolicy;
    }

// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::NewLC
// --------------------------------------------------------------------------
//
CVPbkSortedCompositeContactView* CVPbkSortedCompositeContactView::NewLC(
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder, 
        const MVPbkFieldTypeList& aFieldTypes,
        RFs& aFs)
	{
	CVPbkSortedCompositeContactView* self =
	    new (ELeave) CVPbkSortedCompositeContactView(aFieldTypes, aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aSortOrder);
    self->AddObserverL(aObserver);
	return self;
	}

// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::ConstructL
// --------------------------------------------------------------------------
//
inline void CVPbkSortedCompositeContactView::ConstructL
        (const MVPbkFieldTypeList& aSortOrder)
	{
	BaseConstructL(aSortOrder);
	
	// Instantiate sort policy
    CVPbkContactViewSortPolicy::TParam param(iFieldTypes, aSortOrder, &iFs);
    TRAPD(err, iSortPolicy = CVPbkContactViewSortPolicy::NewL(param));
    if (err == KErrNotFound)
        {
        // Use fallback sort policy
        iSortPolicy = new(ELeave) TSortPolicy;
        }
	}

// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::CreateFilteredViewLC
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CVPbkSortedCompositeContactView::CreateFilteredViewLC( 
        MVPbkContactViewObserver& aObserver,
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    // Create a new sorted composite view
    CVPbkSortedCompositeContactView* compositeView = 
        CVPbkSortedCompositeContactView::NewLC( 
            aObserver, SortOrder(), iFieldTypes, iFs );
    
    // If this composite is internal composite view then also
    // the created composite is internal.
    if ( CompositePolicy().InternalPolicy() )
        {
        compositeView->ApplyInternalCompositePolicyL();
        }
        
    // Create a new filtered subview from each subview of this view
    const TInt count = iSubViews.Count();
    for( TInt i(0); i < count; ++i )
        {
        MVPbkContactViewFiltering* filtering = 
            iSubViews[i]->iView->ViewFiltering();
        // Check if the subview supports filtering.
        if ( filtering )
            {
            // Filtering supported
            MVPbkContactViewBase* filteredSubView = 
                filtering->CreateFilteredViewLC( *compositeView, 
                    aFindWords, aAlwaysIncludedContacts );
            // Add the subview to the new composite view.
            compositeView->AddSubViewL( filteredSubView );
            CleanupStack::Pop(); //filteredSubView
            }
        }
    return compositeView;
    }
    
// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::DoBuildContactMappingL
// Builds view mapping.
// --------------------------------------------------------------------------
//
void CVPbkSortedCompositeContactView::DoBuildContactMappingL()
	{
	VPBK_PROFILE_START(VPbkProfile::ESortedCompositeViewMapping);
	// Reset contact mapping
    iContactMapping.Reset();
    
    // Prepare sort policy
    iSortPolicy->SortStartL();

    // Prepare subview cursors and store view count
    const TInt viewCount = iSubViews.Count();
    for (TInt i = 0; i < viewCount; ++i)
        {
        // If the view is not ready, or empty, the cursor
        // is initialized to KErrNotFound
        if (iSubViews[i]->iState != CSubViewData::EReady || 
            iSubViews[i]->iView->ContactCountL() == 0)
            {
            iSubViews[i]->iCursor = KErrNotFound;
            }
        // In other cases, the cursor is initialized to the first
        // contact of the view
        else
            {
            iSubViews[i]->iContactCursor =
                &iSubViews[i]->iView->ContactAtL(0);
            iSubViews[i]->iCursor = 0;
            }
        }

    // Get the count of all contacts of the composite
    TInt result = 0;
    const TInt subViewCount = iSubViews.Count();
    for (TInt i = 0; i < subViewCount; ++i)
        {
        if (iSubViews[i]->iState == CSubViewData::EReady)
            {
            result += iSubViews[i]->iView->ContactCountL();
            }
        }    
    const TInt contactCount = result;
    
    // Perform the loop as many times as there are total contacts.
    // The loop consists of two parts. First the next unprocessed
    // contact of all the subviews is inspected, and the one that
    // is sorted first among that group of candidates, is then added
    // to global contact mapping.
    //
    // Since the subviews are allready presorted, the next contact
    // to add to the list is always one of the contacts pointed
    // by the subview cursor.
    for (TInt contact = 0; contact < contactCount; ++contact)
        {
        // firstContact pointer is used to indicate the top-sorted contact
        // _from the group of the contacts which haven't yet been sorted_,
        // it is not meant to point to the first contact of the whole contacts
        // list
        const MVPbkViewContact* firstContact = NULL;
        TInt currentView = KErrNotFound;

        // Run through all views
        for (TInt view = 0; view < viewCount; ++view)
            {
            if (iSubViews[view]->iCursor == KErrNotFound)
                {
                // The view is either empty, not ready or
                // already completely processed,
                // so continue with the next view
                continue;
                }
            
            if ( CompareSorting
                    ( firstContact, iSubViews[view]->iContactCursor,
                    *iSortPolicy ) > 0 )
                {
                // iSubViews[view]->iContactCursor comes before
                // current firstContact, so it's the next candidate
                firstContact = iSubViews[view]->iContactCursor;
                currentView = view;
                }
            }
    
        // At this point we either have a top-sorted contact,
        // or then all the views are empty, not-ready or completely
        // processed
    
        if (firstContact)
            {
            // Initialize temporary viewData object with the view
            // of the first contact
            CSubViewData* viewData = iSubViews[currentView];
            // Make a mapping for the current view and its current contact,
            // viewData->iCursor indicates the contact which is to be added
            // next in to the contact list
            TContactMapping mapping;
            mapping.iViewIndex = currentView;
            mapping.iContactIndex = viewData->iCursor;
            // Append that mapping to global mapping
            iContactMapping.AppendL(mapping);
        
            // Move the view's cursor ahead
            ++viewData->iCursor;
            // Check did we move the cursor within the limits of the view
            if (viewData->iCursor < viewData->iView->ContactCountL())
                {
                // Cursor is within limits, update the contact cursor
                viewData->iContactCursor =
                    &viewData->iView->ContactAtL(viewData->iCursor);
                }
            else
                {
                // Cursor moved over the view limits, indicate it
                viewData->iCursor = KErrNotFound;
                }
            }
        }
    
    // Let sort know that everything has been sorted
    iSortPolicy->SortCompleted();
    VPBK_PROFILE_END(VPbkProfile::ESortedCompositeViewMapping);
	}

// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::DoHandleContactEventL
// Handles contact addition to the view.
// --------------------------------------------------------------------------
//
TInt CVPbkSortedCompositeContactView::DoHandleContactAdditionL
        (TInt aSubViewIndex, TInt aIndex)
    {
    TInt index = KErrNotFound;
    
    if (aSubViewIndex != KErrNotFound)
        {
        // Prepare sort policy
        iSortPolicy->SortStartL();

        // Make a mapping for the sub view index and the indexed contact,
        // retrieve the view base pointer from the subview (iSubViews[aSubViewIndex]->iView)
        TContactMapping mapping;
        mapping.iViewIndex = aSubViewIndex;
        mapping.iContactIndex = aIndex;

        // Insert in order
        index = FindInsertionPos(mapping);
        iContactMapping.InsertL(mapping, index);
        
        // Find out the new index
        TIdentityRelation<TContactMapping> comparisonOperator
            ( &CVPbkCompositeContactView::CompareMappings );
        index = iContactMapping.Find( mapping, comparisonOperator );
        
        // Let sort know that everything has been sorted
        iSortPolicy->SortCompleted();
        }

    return index;
    }

// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::ChangeSortOrderL
// Changes sort order to aSortOrder.
// --------------------------------------------------------------------------
//
void CVPbkSortedCompositeContactView::ChangeSortOrderL
        (const MVPbkFieldTypeList& aSortOrder)
    {
    // Change sort order
    iSortPolicy->SetSortOrderL(aSortOrder);

    // Call base class
    CVPbkCompositeContactView::ChangeSortOrderL(aSortOrder);
    }

// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::FindInsertionPos
// Find insertion position in iContactMapping using binary search
// RArray::InsertInOrderAllowRepeats is not used because. 
// Compare function is static and cannot access iSortPolicy. 
// --------------------------------------------------------------------------
//
TInt CVPbkSortedCompositeContactView::FindInsertionPos(
        const TContactMapping& aMapping) const
    {
    TInt count = iContactMapping.Count();
    TInt current = 0;

    while (count > 0) 
        {
        TInt half = count / 2;
        TInt pivot = current+half;
        if (CompareMappings(aMapping, iContactMapping[pivot]) < 0)
            {
            count = half;
            }
        else 
            {
            current = pivot + 1;
            count -= half + 1;
            }
        }
    return current;
    }

// --------------------------------------------------------------------------
// CVPbkSortedCompositeContactView::CompareMappings
// Comparison function for the mappings.
// Returns zero if the mappings are the same. Returns a negative value
// if aLhs comes first and positive if aRhs comes first.
// --------------------------------------------------------------------------
//
TInt CVPbkSortedCompositeContactView::CompareMappings(
        const TContactMapping& aLhs, 
        const TContactMapping& aRhs) const
    {
    TInt ret = KErrUnknown;

    if (aLhs.iViewIndex == aRhs.iViewIndex)
        {
        // The contacts belong to a same view. The underlaying view
        // has already sorted the contacts, so we only need to compare
        // the view indexes.
        ret = aLhs.iContactIndex - aRhs.iContactIndex;
        // If ret == 0, it means that new one has taken old one's place
        // and old one has moved on. So new one is less than old one, 
        // return negative.
        // If ret < 0, new one is less than old one, return negative.
        // If ret > 0, new one is greater than old one, return positive.
        if (ret == 0)
            {
            return -1;
            }
        }
    else
        {
        // The contacts belong to a different view, we have to compare
        // them by using sort policy

        const MVPbkViewContact* lhsContact = NULL;
        const MVPbkViewContact* rhsContact = NULL;

        if (aLhs.iViewIndex >= 0)
            {
            // In case of an error, lhsContact remains NULL
            TRAP_IGNORE( lhsContact =
                &iSubViews[aLhs.iViewIndex]->iView->ContactAtL( aLhs.iContactIndex ) );
            }
    
        if (aRhs.iViewIndex >= 0)
            {
            // In case of an error, rhsContact remains NULL
            TRAP_IGNORE( rhsContact =
                &iSubViews[aRhs.iViewIndex]->iView->ContactAtL( aRhs.iContactIndex ) );
            }

        ret = CompareSorting( lhsContact, rhsContact, *iSortPolicy );
        
        // If contacts are equal the order should stay same (first contacts
        // in view 0 then contacts in view 1, etc.)
        if ( ret == 0 )
            {
            ret = aLhs.iViewIndex - aRhs.iViewIndex;
            }
        }

    return ret;
    }
   
// End of File
