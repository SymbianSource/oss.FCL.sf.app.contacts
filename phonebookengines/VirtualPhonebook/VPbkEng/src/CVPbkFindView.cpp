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
#include <eikenv.h>
#include "CVPbkFindView.h"
#include <MVPbkFieldType.h>
#include <CVPbkSortOrder.h>
#include <CVPbkAsyncCallback.h>
#include <MVPbkViewContact.h>
#include <CVPbkAsyncOperation.h>
#include <VPbkError.h>

#include <CVPbkContactFindPolicy.h>
#include <MVPbkContactBookmarkCollection.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>


#include <VPbkDebug.h>

namespace {
const TInt KObserverArrayGranularity( 4 );
const TInt KGranularity( 2 );

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
    for (TInt i = count - 1; i >= 0; --i)
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
    const TInt count = iObservers.Count();
    for (TInt i = count - 1; i >= 0; --i)
        {
        MVPbkContactViewObserver* observer = iObservers[i];
        (observer->*aNotifyFunc)(aView, aParam1, aParam2);
        }
    }

} // namespace


// ---------------------------------------------------------------------------
// CVPbkFindView::CVPbkFindView
// ---------------------------------------------------------------------------
// 
CVPbkFindView::CVPbkFindView(
        MVPbkContactViewBase& aBaseView,        
        const MVPbkFieldTypeList* aMasterFieldTypeList ) :
    iBaseView(aBaseView),
    iMasterFieldTypeList( aMasterFieldTypeList ),
    iObservers( KObserverArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::ConstructL
// ---------------------------------------------------------------------------
// 
inline void CVPbkFindView::ConstructL( 
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ConstructL(0x%x)"), this);
    
    iAsyncOperation = new(ELeave) VPbkEngUtils::CVPbkAsyncOperation;
    
    iFindStrings = new(ELeave)CDesCArrayFlat( KGranularity );
    SetFindStringsL( aFindWords );
    
    SetAlwaysIncludedContactsL( aAlwaysIncludedContacts );

    
    CEikonEnv* eikonEnv = CEikonEnv::Static();
     
    if ( eikonEnv )
        {
        CVPbkContactFindPolicy::TParam param =
            CVPbkContactFindPolicy::TParam(
                *iMasterFieldTypeList,
                eikonEnv->FsSession() );
    
        // Create contact find policy
        iContactFindPolicy = CVPbkContactFindPolicy::NewL( param );
        }
    
    iBaseView.AddObserverL(*this);

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ConstructL(0x%x) end"), this);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::NewL
// ---------------------------------------------------------------------------
// 
CVPbkFindView* CVPbkFindView::NewLC(
        MVPbkContactViewBase& aBaseView,
        MVPbkContactViewObserver& aObserver,
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts,
        const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    CVPbkFindView* self = 
        new(ELeave) CVPbkFindView(
            aBaseView, &aMasterFieldTypeList );
    CleanupStack::PushL(self);
    self->ConstructL( aFindWords, aAlwaysIncludedContacts );
    self->AddObserverL(aObserver);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::~CVPbkFindView
// ---------------------------------------------------------------------------
// 
CVPbkFindView::~CVPbkFindView()
    {    
    iContactMapping.Reset();   
    iBaseView.RemoveObserver(*this); 
    iObservers.Close();
    delete iFindStrings;
    delete iContactFindPolicy;
    delete iAsyncOperation;        
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::Type
// ---------------------------------------------------------------------------
// 
TVPbkContactViewType CVPbkFindView::Type() const
    {
    return iBaseView.Type();
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::ChangeSortOrderL
// ---------------------------------------------------------------------------
// 
void CVPbkFindView::ChangeSortOrderL(
        const MVPbkFieldTypeList& aSortOrder )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ChangeSortOrderL(0x%x)"), &aSortOrder);

    iBaseView.ChangeSortOrderL(aSortOrder);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::SortOrder
// ---------------------------------------------------------------------------
// 
const MVPbkFieldTypeList& CVPbkFindView::SortOrder() const
    {
    return iBaseView.SortOrder();
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::RefreshL
// ---------------------------------------------------------------------------
// 
void CVPbkFindView::RefreshL()
    {
    iBaseView.RefreshL();
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::ContactCountL
// ---------------------------------------------------------------------------
// 
TInt CVPbkFindView::ContactCountL() const
    {
    return iContactMapping.Count();
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::ContactAtL
// ---------------------------------------------------------------------------
// 
const MVPbkViewContact& CVPbkFindView::ContactAtL(
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
// CVPbkFindView::CreateLinkLC
// ---------------------------------------------------------------------------
// 
MVPbkContactLink* CVPbkFindView::CreateLinkLC( TInt aIndex ) const
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
// CVPbkFindView::IndexOfLinkL
// ---------------------------------------------------------------------------
// 
TInt CVPbkFindView::IndexOfLinkL(
        const MVPbkContactLink& aContactLink ) const
    {
    TInt baseIndex = iBaseView.IndexOfLinkL(aContactLink);
    return iContactMapping.Find(baseIndex);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::AddObserverL
// ---------------------------------------------------------------------------
// 
void CVPbkFindView::AddObserverL( 
        MVPbkContactViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::AddObserverL(0x%x)"), &aObserver);

    VPbkEngUtils::MAsyncCallback* notifyObserver =
        VPbkEngUtils::CreateAsyncCallbackLC(*this,
                &CVPbkFindView::DoAddObserverL,
                &CVPbkFindView::AddObserverError,
                aObserver);
    iAsyncOperation->CallbackL(notifyObserver);
    CleanupStack::Pop(notifyObserver);

    /// Insert to first position because event are send in reverse order.
    /// Last inserted gets notifcation last.
    iObservers.InsertL( &aObserver, 0 );
    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::AddObserverL(0x%x) end"), &aObserver);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::DoAddObserverL
// ---------------------------------------------------------------------------
// 
void CVPbkFindView::DoAddObserverL(
        MVPbkContactViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::DoAddObserverL(0x%x)"), &aObserver);

    if ( iObservers.Find( &aObserver ) != KErrNotFound )
        {
        if ( iIsReady )
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
            ("CVPbkFindView::DoAddObserverL(0x%x) contact view ready"), 
                &aObserver);

            // If this view is ready and there was no error,
            // tell it to the observer
            aObserver.ContactViewReady( *this );
            }
        else
            {
            aObserver.ContactViewUnavailable( *this );
            }    
        }
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::DoAddObserverL(0x%x) end"), &aObserver);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::AddObserverError
// ---------------------------------------------------------------------------
// 
void CVPbkFindView::AddObserverError(
        MVPbkContactViewObserver& aObserver, TInt aError )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::AddObserverError(0x%x, %d)"), &aObserver, aError);

    aObserver.ContactViewError(*this, aError, EFalse);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::RemoveObserver
// ---------------------------------------------------------------------------
// 
void CVPbkFindView::RemoveObserver(
        MVPbkContactViewObserver& aObserver )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::RemoveObserverError(0x%x)"), &aObserver);

    const TInt index = iObservers.Find( &aObserver );
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::MatchContactStore
// ---------------------------------------------------------------------------
// 
TBool CVPbkFindView::MatchContactStore(
        const TDesC& aContactStoreUri ) const
    {
    return iBaseView.MatchContactStore(aContactStoreUri);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::MatchContactStoreDomain
// ---------------------------------------------------------------------------
// 
TBool CVPbkFindView::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain ) const
    {
    return iBaseView.MatchContactStoreDomain(aContactStoreDomain);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::CreateBookmarkLC
// ---------------------------------------------------------------------------
// 
MVPbkContactBookmark* CVPbkFindView::CreateBookmarkLC(
        TInt aIndex ) const
    {
    return iBaseView.ContactAtL(iContactMapping[aIndex]).CreateBookmarkLC();
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::IndexOfBookmarkL
// ---------------------------------------------------------------------------
// 
TInt CVPbkFindView::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark ) const
    {
    TInt baseIndex = iBaseView.IndexOfBookmarkL(aContactBookmark);
    return iContactMapping.Find(baseIndex);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::ViewFiltering
// ---------------------------------------------------------------------------
//         
MVPbkContactViewFiltering* CVPbkFindView::ViewFiltering()
    {    
    return this;
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::CreateFilteredViewLC
// ---------------------------------------------------------------------------
//    
MVPbkContactViewBase* CVPbkFindView::CreateFilteredViewLC( 
        MVPbkContactViewObserver& aObserver,
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::CreateFilteredViewLC(0x%x)"), this);
        
    return CVPbkFindView::NewLC( *this, 
                                 aObserver, 
                                 aFindWords, 
                                 aAlwaysIncludedContacts,
                                 *iMasterFieldTypeList );    
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::UpdateFilterL
// ---------------------------------------------------------------------------
//     
void CVPbkFindView::UpdateFilterL( 
        const MDesCArray& aFindWords,
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::UpdateFilterL(0x%x)"), this);
        
    SetFindStringsL( aFindWords );
    SetAlwaysIncludedContactsL( aAlwaysIncludedContacts );
    
    // Remove first if this called second time when updating.
    iBaseView.RemoveObserver( *this );
    // This will cause an asynchrnous view event to this view from the
    // parent view.
    iBaseView.AddObserverL( *this );
    }
    
// ---------------------------------------------------------------------------
// CVPbkFindView::BuildViewMappingL
// ---------------------------------------------------------------------------
//         
void CVPbkFindView::BuildViewMappingL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::BuildViewMappingL(0x%x)"), this);

    iContactMapping.Reset();

    // Append always included contact
    if ( iAlwaysIncludedContacts )
        {
        const TInt alwaysIncluded( iAlwaysIncludedContacts->Count() );
        for ( TInt i(0); i < alwaysIncluded; ++i )
            {
            TInt index( 
                iBaseView.IndexOfBookmarkL( 
                    iAlwaysIncludedContacts->At( i ) ) );
            if ( index > KErrNotFound )
                {
                User::LeaveIfError( iContactMapping.InsertInOrder(index) );
                }        
            }    
        }
    
    // Append matched contacts    
    const TInt countContacts = iBaseView.ContactCountL();
    for (TInt i(0); i < countContacts; ++i)
        {
        const MVPbkViewContact& contact = iBaseView.ContactAtL( i );
        
        TBool match( ETrue );

        if ( iContactFindPolicy && 
                !iContactFindPolicy->MatchContactNameL( *iFindStrings, contact ))
            {
            match = EFalse;      
            }

        if ( match && iContactMapping.FindInOrder( i ) == KErrNotFound ) 
            {                    
            User::LeaveIfError( iContactMapping.InsertInOrder( i ) );
            }            
        }

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::BuildViewMappingL(0x%x) end"), this);
    }

// ---------------------------------------------------------------------------
// CVPbkFindView::HandleBuildViewMapping
// ---------------------------------------------------------------------------
//         
void CVPbkFindView::HandleBuildViewMapping()
    {
    TRAPD( res, BuildViewMappingL() );
    if ( res == KErrNone )
        {
        // Mapping was succesfully built. Send ready event
        SendViewEventToObservers( 
            &MVPbkContactViewObserver::ContactViewReady );
        }
    else
        {
        // Building the mapping failed. Leave mappings as they are and
        // send error
        SendViewErrorEventToObservers( res, EFalse );
        }
    }
    
// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFindView::ContactViewReady
// ---------------------------------------------------------------------------
//         
void CVPbkFindView::ContactViewReady( MVPbkContactViewBase& aView )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ContactViewReady(0x%x)"), &aView);

    iIsReady = ETrue;
    HandleBuildViewMapping();
    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ContactViewReady(0x%x) end"), &aView);
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFindView::ContactViewUnavailable
// ---------------------------------------------------------------------------
//         
void CVPbkFindView::ContactViewUnavailable( 
        MVPbkContactViewBase& aView )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ContactViewUnavailable(0x%x)"), &aView);

    iIsReady = EFalse;
    SendViewEventToObservers(
        &MVPbkContactViewObserver::ContactViewUnavailable );

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ContactViewUnavailable(0x%x)"), &aView);
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFindView::ContactViewError
// ---------------------------------------------------------------------------
//         
void CVPbkFindView::ContactViewError( MVPbkContactViewBase& aView, 
        TInt aError, TBool aErrorNotified )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ContactViewError(0x%x)"), &aView);

    iIsReady = EFalse;
    SendViewErrorEventToObservers( aError, aErrorNotified );

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CVPbkFindView::ContactViewError(0x%x)"), &aView);
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFindView::ContactAddedToView
// ---------------------------------------------------------------------------
//         
void CVPbkFindView::ContactAddedToView
        (MVPbkContactViewBase& /*aView*/, TInt aIndex,
        const MVPbkContactLink& aContactLink)
    {
    // To keep indexes in iContactMapping up to date rebuild mappings.
    TRAPD( res, BuildViewMappingL() );
    if ( res == KErrNone )
        {
        // Do a binary search to mapping
        TInt index = iContactMapping.FindInOrder( aIndex );
        if ( index != KErrNotFound )
            {
            // Mapping was succesfully updated and the new contact
            // belongs to filter.
            SendEventToObservers( *this, iObservers,
                &MVPbkContactViewObserver::ContactAddedToView, index, 
                aContactLink );
            }
        }
    else
        {
        SendViewErrorEventToObservers( res, EFalse );
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactViewBase.
// CVPbkFindView::ContactRemovedFromView
// ---------------------------------------------------------------------------
//         
void CVPbkFindView::ContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/, TInt aIndex, 
        const MVPbkContactLink& aContactLink)
    {
    // Do a binary search to mapping
    TInt index = iContactMapping.FindInOrder( aIndex );
    
    // To keep indexes in iContactMapping up to date rebuild mappings.
    TRAPD( res, BuildViewMappingL() );
    if ( res == KErrNone )
        {
        if ( index != KErrNotFound )
            {
            // Mapping was succesfully updated and the new contact
            // belongs to filter.
            SendEventToObservers( *this, iObservers,
                &MVPbkContactViewObserver::ContactRemovedFromView, index, 
                aContactLink );
            }
        }
    else
        {
        SendViewErrorEventToObservers( res, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CVPbkFindView::SetFindStringsL
// --------------------------------------------------------------------------
// 
void CVPbkFindView::SetFindStringsL( const MDesCArray& aFindWords )
    {
    iFindStrings->Reset();
    
    const TInt count( aFindWords.MdcaCount() );
    for ( TInt i(0); i < count; ++i )
        {
        iFindStrings->AppendL( aFindWords.MdcaPoint( i ) );
        }
    }

// --------------------------------------------------------------------------
// CVPbkFindView::SetAlwaysIncludedContactsL
// --------------------------------------------------------------------------
// 
void CVPbkFindView::SetAlwaysIncludedContactsL( 
        const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts )
    {
    iAlwaysIncludedContacts = aAlwaysIncludedContacts;
    }

// --------------------------------------------------------------------------
// CVPbkFindView::SendViewEventToObservers
// --------------------------------------------------------------------------
// 
void CVPbkFindView::SendViewEventToObservers( 
    void( MVPbkContactViewObserver::* aObserverFunc)( MVPbkContactViewBase& ) )
    {
    // Cancel async operation to avoid duplicate event sending to observers
    iAsyncOperation->Purge();
    SendEventToObservers( *this, iObservers, aObserverFunc );
    }

// --------------------------------------------------------------------------
// CVPbkFindView::SendViewErrorEventToObservers
// --------------------------------------------------------------------------
// 
void CVPbkFindView::SendViewErrorEventToObservers( TInt aError, TBool 
        aErrorNotified )
    {
    // Cancel async operation to avoid duplicate event sending to observers
    iAsyncOperation->Purge();
    SendEventToObservers( *this, iObservers,
        &MVPbkContactViewObserver::ContactViewError, 
        aError, aErrorNotified );
    }
// End of File
