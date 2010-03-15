/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Filtered contact view
*
*/


#include "CVPbkFilteredContactView.h"
#include <MVPbkFieldType.h>
#include <CVPbkSortOrder.h>
#include <CVPbkAsyncCallback.h>
#include <MVPbkContactSelector.h>
#include <MVPbkViewContact.h>
#include <CVPbkAsyncOperation.h>
#include <VPbkError.h>
#include "CVPbkFindView.h"

#include <VPbkDebug.h>


namespace {
const TInt KObserverArrayGranularity = 4;

// ---------------------------------------------------------------------------
// Event sending function for one reference parameter
// ---------------------------------------------------------------------------
//
template <class NotifyFunc>
void SendEventToObservers(MVPbkContactViewBase& aView,
                          RPointerArray<MVPbkContactViewObserver>& iObservers,
                          NotifyFunc aNotifyFunc)
    {
    const TInt count = iObservers.Count();
    for (TInt i = 0; i < count; ++i)
        {
        MVPbkContactViewObserver* observer = iObservers[i];
        (observer->*aNotifyFunc)(aView);
        }
    }

// ---------------------------------------------------------------------------
// Event sending functions for 3 parameters
// ---------------------------------------------------------------------------
//
template <class NotifyFunc, class ParamType1, class ParamType2>
void SendEventToObservers(MVPbkContactViewBase& aView,
                          RPointerArray<MVPbkContactViewObserver>& iObservers,
                          NotifyFunc aNotifyFunc,
                          ParamType1 aParam1,
                          const ParamType2& aParam2)
    {
    for (TInt i = 0; i < iObservers.Count(); ++i)
        {
        MVPbkContactViewObserver* observer = iObservers[i];
        (observer->*aNotifyFunc)(aView, aParam1, aParam2);
        }
    }

} // namespace


// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::CVPbkFilteredContactView
// ---------------------------------------------------------------------------
//
CVPbkFilteredContactView::CVPbkFilteredContactView(
        MVPbkContactViewBase& aBaseView,
        MVPbkContactSelector& aContactSelector,
        const MVPbkFieldTypeList* aMasterFieldTypeList ) :
    iBaseView(aBaseView),
    iContactSelector(aContactSelector),
    iMasterFieldTypeList( aMasterFieldTypeList ),
    iObservers( KObserverArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::ConstructL
// ---------------------------------------------------------------------------
//
inline void CVPbkFilteredContactView::ConstructL(
        MVPbkContactViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ConstructL(0x%x)"), this);

    iObservers.InsertInAddressOrder( &aObserver );
    iAsyncOperation = new(ELeave) VPbkEngUtils::CVPbkAsyncOperation;
    iBaseView.AddObserverL(*this);

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ConstructL(0x%x) end"), this);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkFilteredContactView* CVPbkFilteredContactView::NewL(
        MVPbkContactViewBase& aBaseView,
        MVPbkContactViewObserver& aObserver,
        MVPbkContactSelector& aContactSelector)
    {
    CVPbkFilteredContactView* self = new(ELeave) CVPbkFilteredContactView
        (aBaseView, aContactSelector, NULL );
    CleanupStack::PushL(self);
    self->ConstructL( aObserver );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkFilteredContactView* CVPbkFilteredContactView::NewL(
        MVPbkContactViewBase& aBaseView,
        MVPbkContactViewObserver& aObserver,
        MVPbkContactSelector& aContactSelector,
        const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    CVPbkFilteredContactView* self = new(ELeave) CVPbkFilteredContactView
        (aBaseView, aContactSelector, &aMasterFieldTypeList );
    CleanupStack::PushL(self);
    self->ConstructL( aObserver );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::~CVPbkFilteredContactView
// ---------------------------------------------------------------------------
//
CVPbkFilteredContactView::~CVPbkFilteredContactView()
    {
    iObservers.Close();
    iContactMapping.Reset();
    delete iAsyncOperation;
    iBaseView.RemoveObserver(*this);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::Type
// ---------------------------------------------------------------------------
//
TVPbkContactViewType CVPbkFilteredContactView::Type() const
    {
    return iBaseView.Type();
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::ChangeSortOrderL
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::ChangeSortOrderL(
        const MVPbkFieldTypeList& aSortOrder )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ChangeSortOrderL(0x%x)"), &aSortOrder);

    iBaseView.ChangeSortOrderL(aSortOrder);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::SortOrder
// ---------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CVPbkFilteredContactView::SortOrder() const
    {
    return iBaseView.SortOrder();
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::RefreshL
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::RefreshL()
    {
    iBaseView.RefreshL();
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::ContactCountL
// ---------------------------------------------------------------------------
//
TInt CVPbkFilteredContactView::ContactCountL() const
    {
    return iContactMapping.Count();
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::ContactAtL
// ---------------------------------------------------------------------------
//
const MVPbkViewContact& CVPbkFilteredContactView::ContactAtL(
        TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= iContactMapping.Count() )
        {
        User::Leave( KErrArgument );
        }

    return iBaseView.ContactAtL(iContactMapping[aIndex]);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::CreateLinkLC
// ---------------------------------------------------------------------------
//
MVPbkContactLink* CVPbkFilteredContactView::CreateLinkLC( TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= iContactMapping.Count() )
        {
        User::Leave( KErrArgument );
        }

    return iBaseView.ContactAtL(iContactMapping[aIndex]).CreateLinkLC();
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::IndexOfLinkL
// ---------------------------------------------------------------------------
//
TInt CVPbkFilteredContactView::IndexOfLinkL(
        const MVPbkContactLink& aContactLink ) const
    {
    TInt baseIndex = iBaseView.IndexOfLinkL(aContactLink);
    return iContactMapping.Find(baseIndex);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::AddObserverL
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::AddObserverL(
        MVPbkContactViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::AddObserverL(0x%x)"), &aObserver);

    TInt err( iObservers.InsertInAddressOrder( &aObserver ) );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }

    VPbkEngUtils::MAsyncCallback* notifyObserver =
        VPbkEngUtils::CreateAsyncCallbackLC(*this,
                &CVPbkFilteredContactView::DoAddObserverL,
                &CVPbkFilteredContactView::AddObserverError,
                aObserver);
    iAsyncOperation->CallbackL(notifyObserver);
    CleanupStack::Pop(notifyObserver);

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::AddObserverL(0x%x) end"), &aObserver);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::DoAddObserverL
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::DoAddObserverL(
        MVPbkContactViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::DoAddObserverL(0x%x)"), &aObserver);

// Check if aObserver is still observer of this view
    if ( iObservers.FindInAddressOrder( &aObserver ) != KErrNotFound )
        {
        if (iIsReady)
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
            ("CVPbkFilteredContactView::DoAddObserverL(0x%x) contact view ready"),
                &aObserver);

            // If this view is ready and there was no error,
            // tell it to the observer
            aObserver.ContactViewReady(*this);
            }
        }
    // If this view was not ready and there was no error,
    // observer will be called back in ContactViewReady

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::DoAddObserverL(0x%x) end"), &aObserver);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::AddObserverError
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::AddObserverError(
        MVPbkContactViewObserver& aObserver, TInt aError )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::AddObserverError(0x%x, %d)"), &aObserver, aError);

    aObserver.ContactViewError(*this, aError, EFalse);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::RemoveObserver
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::RemoveObserver(
        MVPbkContactViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::RemoveObserverError(0x%x)"), &aObserver);

    const TInt index = iObservers.FindInAddressOrder( &aObserver );
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::MatchContactStore
// ---------------------------------------------------------------------------
//
TBool CVPbkFilteredContactView::MatchContactStore(
        const TDesC& aContactStoreUri ) const
    {
    return iBaseView.MatchContactStore(aContactStoreUri);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::MatchContactStoreDomain
// ---------------------------------------------------------------------------
//
TBool CVPbkFilteredContactView::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain ) const
    {
    return iBaseView.MatchContactStoreDomain(aContactStoreDomain);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::CreateBookmarkLC
// ---------------------------------------------------------------------------
//
MVPbkContactBookmark* CVPbkFilteredContactView::CreateBookmarkLC(
        TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );

    return iBaseView.ContactAtL(iContactMapping[aIndex]).CreateBookmarkLC();
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::IndexOfBookmarkL
// ---------------------------------------------------------------------------
//
TInt CVPbkFilteredContactView::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark ) const
    {
    TInt baseIndex = iBaseView.IndexOfBookmarkL(aContactBookmark);
    return iContactMapping.Find(baseIndex);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::ViewFiltering
// ---------------------------------------------------------------------------
//
MVPbkContactViewFiltering* CVPbkFilteredContactView::ViewFiltering()
    {
    if ( iMasterFieldTypeList )
        {
        return this;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::CreateFilteredViewLC
// ---------------------------------------------------------------------------
//
MVPbkContactViewBase* CVPbkFilteredContactView::CreateFilteredViewLC(
        MVPbkContactViewObserver& aObserver,
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::CreateFilteredViewLC(0x%x)"), this);

    if ( iMasterFieldTypeList )
        {
        return CVPbkFindView::NewLC( *this,
                                     aObserver,
                                     aFindWords,
                                     aAlwaysIncludedContacts,
                                     *iMasterFieldTypeList );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::UpdateFilterL
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::UpdateFilterL(
        const MDesCArray& /*aFindWords*/,
        const MVPbkContactBookmarkCollection* /*aAlwaysIncludedContacts*/ )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::BuildViewMappingL
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::BuildViewMappingL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::BuildViewMappingL(0x%x)"), this);

    iContactMapping.Reset();

    const TInt count = iBaseView.ContactCountL();
    for (TInt i = 0; i < count; ++i)
        {
        if (iContactSelector.IsContactIncluded(iBaseView.ContactAtL(i)))
            {
            iContactMapping.AppendL(i);
            }
        }

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::BuildViewMappingL(0x%x) end"), this);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::HandleBuildViewMapping
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::HandleBuildViewMapping()
    {
    TRAPD( res, BuildViewMappingL() );
    if ( res == KErrNone )
        {
        // Mapping was succesfully built. Send ready event
        SendEventToObservers( *this, iObservers,
            &MVPbkContactViewObserver::ContactViewReady );
        }
    else
        {
        // Building the mapping failed. Leave mappings as they are and
        // send error
        SendEventToObservers( *this, iObservers,
            &MVPbkContactViewObserver::ContactViewError, res, EFalse );
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFilteredContactView::ContactViewReady
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::ContactViewReady( MVPbkContactViewBase& aView )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactViewReady(0x%x)"), &aView);

    iIsReady = ETrue;
    HandleBuildViewMapping();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactViewReady(0x%x) end"), &aView);
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFilteredContactView::ContactViewUnavailable
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::ContactViewUnavailable(
        MVPbkContactViewBase& aView )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactViewUnavailable(0x%x)"), &aView);

    iIsReady = EFalse;
    SendEventToObservers
        (*this, iObservers, &MVPbkContactViewObserver::ContactViewUnavailable);

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactViewUnavailable(0x%x)"), &aView);
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFilteredContactView::ContactViewError
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::ContactViewError( MVPbkContactViewBase& aView,
        TInt aError, TBool aErrorNotified )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactViewError(0x%x)"), &aView);

    iIsReady = EFalse;
    SendEventToObservers(*this, iObservers,
        &MVPbkContactViewObserver::ContactViewError, aError, aErrorNotified);

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactViewError(0x%x)"), &aView);
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFilteredContactView::ContactAddedToView
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::ContactAddedToView
        (MVPbkContactViewBase& /*aView*/, TInt aIndex,
        const MVPbkContactLink& aContactLink)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactAddedToView(0x%x %d)"), this, aIndex);

    TInt filteredIndex(0);
    TRAPD( res, filteredIndex = UpdateViewMappingAfterAddingL( aIndex ) );
    if ( res == KErrNone )
        {
        if ( filteredIndex != KErrNotFound )
            {
            // Mapping was succesfully updated and the new contact
            // belongs to the filtered view.
            SendEventToObservers( *this, iObservers,
                &MVPbkContactViewObserver::ContactAddedToView, filteredIndex,
                aContactLink );
            }
        }
    else
        {
        SendEventToObservers( *this, iObservers,
            &MVPbkContactViewObserver::ContactViewError, res, EFalse );
        }

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactAddedToView(0x%x)"), this);
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFilteredContactView::ContactRemovedFromView
// ---------------------------------------------------------------------------
//
void CVPbkFilteredContactView::ContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/, TInt aIndex,
        const MVPbkContactLink& aContactLink)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactRemovedFromView(0x%x %d)"), this, aIndex);

    const TInt filteredIndex = UpdateViewMappingAfterDeleting( aIndex );
    if ( filteredIndex != KErrNotFound )
        {
        // Mapping was succesfully updated and the contact removed from the mapping
        SendEventToObservers( *this, iObservers,
            &MVPbkContactViewObserver::ContactRemovedFromView, filteredIndex,
            aContactLink );
        }
    else
        {
        const TInt count = iObservers.Count();

        for( TInt i = 0; i < count; i++ )
           {
           MVPbkContactViewObserver* observer = iObservers[i];
         
           TAny* extension = observer->ContactViewObserverExtension(
                   KVPbkContactViewObserverExtension2Uid );

           if( extension )
               {
               MVPbkContactViewObserverExtension* contactViewExtension =
                       static_cast<MVPbkContactViewObserverExtension*>( extension );

               if( contactViewExtension )
                   {
                   contactViewExtension->FilteredContactRemovedFromView( *this );
                   }
               }
           }
        }

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFilteredContactView::ContactRemovedFromView(0x%x)"), this);
    }

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::UpdateViewMappingAfterAdding
//
// Update the mapping when a new contact is added to the base view
// Returns its mapped index if it is added to the mapping
// or KErrNotFound if it's filtered out
// ---------------------------------------------------------------------------
//
TInt CVPbkFilteredContactView::UpdateViewMappingAfterAddingL( TInt aBaseIndex )
	{
	// first update the mapping for all elements which > aBaseIndex
	TInt filteredIndex;
	// find first elemnt which >= aBaseIndex
	// NOTE: if iContactMapping contains aBaseIndex element,
	// 		 it refers to a view contact which is already at aBaseIndex+1
	TInt err = iContactMapping.FindInOrder( aBaseIndex, filteredIndex );
	for ( TInt i = filteredIndex; i < iContactMapping.Count(); ++i )
		{
		++iContactMapping[i];
		}

	// addd a new contact to the mapping
	if ( iContactSelector.IsContactIncluded( iBaseView.ContactAtL (aBaseIndex) ) )
		{
		User::LeaveIfError( iContactMapping.Insert( aBaseIndex, filteredIndex ) );
		}
	else
		{
		filteredIndex = KErrNotFound;
		}

	return filteredIndex;
	}

// ---------------------------------------------------------------------------
// CVPbkFilteredContactView::UpdateViewMappingAfterDeleting
//
// Update the mapping when a new contact is deleted from the base view
// Returns its mapped index if it was removed from the mapping
// or KErrNotFound otherwise
// ---------------------------------------------------------------------------
//
TInt CVPbkFilteredContactView::UpdateViewMappingAfterDeleting( TInt aBaseIndex )
	{
    // if the contact is in mappings, filteredIndex contains its index
    // otherwise, the index of the next element greater than aIndex
	TInt filteredIndex;
	const TInt err = iContactMapping.FindInOrder( aBaseIndex, filteredIndex );

    // mapping should be updated in any case even if deleted contact
    // is not in the mapping
    // because all items in the mappings which > aIndex should be updated
    for ( TInt i = filteredIndex; i < iContactMapping.Count(); ++i )
        {
        // Subtract one because one contact was deleted
        // from the list before the current index
        --iContactMapping[i];
        }

    if ( err == KErrNone )
    	{
    	iContactMapping.Remove( filteredIndex );
    	}
    else
    	{
    	filteredIndex = KErrNotFound;
    	}

    return filteredIndex;
	}

// End of File
