/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class for folding view that can be expanded to its subview
*
*/


#include "CVPbkFoldingContactView.h"
#include <MVPbkContactStore.h>
#include <CVPbkSortOrder.h>
#include <MVPbkContactViewObserver.h>
#include <CVPbkAsyncCallback.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkError.h>

#include "TVPbkFoldingContactBookmark.h"
#include "CVPbkFoldingViewContact.h"

namespace {

#ifdef _DEBUG
    enum TPanic
        {
        EPreCond_ConstructL
        };
        
    void Panic( TPanic aPanic )
        {
        _LIT(KPanicCat, "CVPbkFoldingContactView");
        User::Panic( KPanicCat, aPanic );
        }
#endif // _DEBUG

// CONSTANTS
const TInt KObserverArrayGranularity = 4;

// Event sending functions for different amount of parameters

// ---------------------------------------------------------------------------
// SendEventToObservers
// For observer functions that take MVPbkContactViewBase as a parameter
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
// SendEventToObservers
// For observer functions that take MVPbkContactViewBase, and two other 
// parameters
// ---------------------------------------------------------------------------
//
template <class NotifyFunc, class ParamType1, class ParamType2>
void SendEventToObservers(MVPbkContactViewBase& aView, 
                          RPointerArray<MVPbkContactViewObserver>& iObservers, 
                          NotifyFunc aNotifyFunc,
                          ParamType1 aParam1,
                          ParamType2 aParam2)
    {
    const TInt count = iObservers.Count();
    for (TInt i = 0; i < count; ++i)
        {
        MVPbkContactViewObserver* observer = iObservers[i];
        (observer->*aNotifyFunc)(aView, aParam1, aParam2);
        }
    }

// ---------------------------------------------------------------------------
// DoMatchContactStore
// ---------------------------------------------------------------------------
//
TBool DoMatchContactStore(CVPbkContactViewDefinition& aViewDef,
        TVPbkContactStoreUriPtr::TVPbkContactStoreUriComponent aUriComponent, 
        const TDesC& aStoreUriComponentDes)
    {
    // Match store URI to the first view definition that is found starting
    // from the aViewDef. If not found then check subviews from left to right
    TBool result = EFalse;
    TVPbkContactStoreUriPtr uriPtr(aViewDef.Uri());
    if (uriPtr.Compare(aStoreUriComponentDes, aUriComponent) == 0)
        {
        result = ETrue;
        }
    else
        {
        const TInt count = aViewDef.SubViewCount();
        for (TInt i = 0; i < count && !result; ++i)
            {
            result = DoMatchContactStore(aViewDef.SubViewAt(i), aUriComponent, 
                aStoreUriComponentDes);
            }
        }
    return result;
    }
}

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::CVPbkFoldingContactView
// ---------------------------------------------------------------------------
//
CVPbkFoldingContactView::CVPbkFoldingContactView(
        const CVPbkContactManager& aContactManager) :
    iContactManager(aContactManager),
    iObservers( KObserverArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::ConstructL
// ---------------------------------------------------------------------------
//
inline void CVPbkFoldingContactView::ConstructL(
        const CVPbkContactViewDefinition& aViewDefinition,
        const MVPbkFieldTypeList& aSortOrder)
    {
    /// Folding view must have 1 sub view definition if it's used for
    /// expanding. If there is no sub view then this folding can not be
    /// exapanded and it's only used to show a named item.
    __ASSERT_DEBUG( aViewDefinition.SubViewCount() <= 1,
        Panic( EPreCond_ConstructL ) );
    
    /// Save the one and only subview definition for later usage
    iViewDefinition = CVPbkContactViewDefinition::NewL( aViewDefinition );
    /// Create the view contact
    iFoldingContact = CVPbkFoldingViewContact::NewL(*this);
    iSortOrder = CVPbkSortOrder::NewL(aSortOrder);
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::NewLC
// ---------------------------------------------------------------------------
//  
CVPbkFoldingContactView* CVPbkFoldingContactView::NewLC(
        MVPbkContactViewObserver& aObserver,
        const CVPbkContactViewDefinition& aViewDefinition, 
        const CVPbkContactManager& aContactManager,
        const MVPbkFieldTypeList& aSortOrder)
    {
    CVPbkFoldingContactView* self = new(ELeave) CVPbkFoldingContactView(aContactManager);
    CleanupStack::PushL(self);
    self->ConstructL(aViewDefinition, aSortOrder);
    self->AddObserverL(aObserver);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::~CVPbkFoldingContactView
// ---------------------------------------------------------------------------
//  
CVPbkFoldingContactView::~CVPbkFoldingContactView()
    {
    delete iViewDefinition;
    delete iSortOrder;
    delete iFoldingContact;
    iObservers.Close();
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::Name
// ---------------------------------------------------------------------------
//
const TDesC& CVPbkFoldingContactView::Name() const
    {
    return iViewDefinition->Name();
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::Type
// ---------------------------------------------------------------------------
//
TVPbkContactViewType CVPbkFoldingContactView::Type() const
    {
    return EVPbkFoldingView;
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::ChangeSortOrderL
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::ChangeSortOrderL(const MVPbkFieldTypeList& aSortOrder)
    {
    if (iObservers.Count() > 0)
        {
        CVPbkSortOrder* newSortOrder = CVPbkSortOrder::NewL(aSortOrder);
        delete iSortOrder;
        iSortOrder = newSortOrder;
        
        SendAsyncUnavailableAndReadyEventL();
        }    
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::SortOrder
// ---------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CVPbkFoldingContactView::SortOrder() const
    {
    return *iSortOrder;
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::RefreshL
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::RefreshL()
    {
    SendAsyncUnavailableAndReadyEventL();
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::ContactCountL
// ---------------------------------------------------------------------------
//
TInt CVPbkFoldingContactView::ContactCountL() const
    {
    // This is folding view => only this view is visible as a contact
    return 1;
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::ContactAtL
// ---------------------------------------------------------------------------
//
const MVPbkViewContact& CVPbkFoldingContactView::ContactAtL(TInt aIndex) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= ContactCountL() )
        {
        User::Leave( KErrArgument );
        }

    return *iFoldingContact;
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::CreateLinkLC
// ---------------------------------------------------------------------------
//
MVPbkContactLink* CVPbkFoldingContactView::CreateLinkLC(TInt aIndex) const
    {
    __ASSERT_ALWAYS( aIndex >= 0,
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );
    if ( aIndex >= ContactCountL() )
        {
        User::Leave( KErrArgument );
        }

    return iFoldingContact->CreateLinkLC();
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::IndexOfLinkL
// ---------------------------------------------------------------------------
//
TInt CVPbkFoldingContactView::IndexOfLinkL(
        const MVPbkContactLink& /*aContactLink*/) const
    {
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::AddObserverL
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::AddObserverL(
        MVPbkContactViewObserver& aObserver)
    {
    VPbkEngUtils::MAsyncCallback* notifyObserver =
        VPbkEngUtils::CreateAsyncCallbackLC(
            *this, 
            &CVPbkFoldingContactView::DoAddObserverL, 
            &CVPbkFoldingContactView::AddObserverError, 
            aObserver);
    iAsyncOperation.CallbackL(notifyObserver);
    CleanupStack::Pop(notifyObserver);
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::DoAddObserverL
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::DoAddObserverL(
        MVPbkContactViewObserver& aObserver)
    {
    TInt err( iObservers.InsertInAddressOrder( &aObserver ) );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
        
    // this view is always ready
    aObserver.ContactViewReady(*this);
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::AddObserverError
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::AddObserverError(
        MVPbkContactViewObserver& aObserver, TInt aError)
    {
    aObserver.ContactViewError(*this, aError, EFalse);
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::SendAsyncUnavailableAndReadyEventL
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::SendAsyncUnavailableAndReadyEventL()
    {
    // Send first unvavailable event...
    VPbkEngUtils::MAsyncCallback* notifyObserver = 
        VPbkEngUtils::CreateAsyncCallbackLC(
            *this, 
            &CVPbkFoldingContactView::DoSignalObserversViewUnavailable,
            &CVPbkFoldingContactView::DoSignalObserversViewError,
            *iObservers[0]);//Observer is actually not used by DoSignal*
    iAsyncOperation.CallbackL(notifyObserver);
    CleanupStack::Pop(notifyObserver);
    
    // ...then ready event. This is how views must behave.
    notifyObserver = VPbkEngUtils::CreateAsyncCallbackLC(
        *this,
        &CVPbkFoldingContactView::DoSignalObserversViewReady, 
        &CVPbkFoldingContactView::DoSignalObserversViewError,
        *iObservers[0]);//Observer is actually not used by DoSignal*
    iAsyncOperation.CallbackL( notifyObserver );
    CleanupStack::Pop( notifyObserver );
    }
    
// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::DoSignalObserversViewReady
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::DoSignalObserversViewReady(
        MVPbkContactViewObserver& /*aObserver*/)
    {
    SendEventToObservers(*this, iObservers, 
        &MVPbkContactViewObserver::ContactViewReady);
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::DoSignalObserversViewUnavailable
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::DoSignalObserversViewUnavailable(
        MVPbkContactViewObserver& /*aObserver*/)
    {
    SendEventToObservers( *this, iObservers, 
        &MVPbkContactViewObserver::ContactViewUnavailable );
    }
    
// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::DoSignalObserversViewError
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::DoSignalObserversViewError(
        MVPbkContactViewObserver& /* aObserver */, 
        TInt aError )
    {
    SendEventToObservers(*this, iObservers, 
        &MVPbkContactViewObserver::ContactViewError, aError, EFalse );
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::RemoveObserver
// ---------------------------------------------------------------------------
//
void CVPbkFoldingContactView::RemoveObserver(
        MVPbkContactViewObserver& aObserver)
    {
    TInt index( iObservers.FindInAddressOrder( &aObserver ) );
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::MatchContactStore
// ---------------------------------------------------------------------------
//
TBool CVPbkFoldingContactView::MatchContactStore(
        const TDesC& aContactStoreUri) const
    {
    return DoMatchContactStore(*iViewDefinition, 
        TVPbkContactStoreUriPtr::EContactStoreUriAllComponents,
        aContactStoreUri);
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::MatchContactStoreDomain
// ---------------------------------------------------------------------------
//
TBool CVPbkFoldingContactView::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain) const
    {
    return DoMatchContactStore(*iViewDefinition, 
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType,
        aContactStoreDomain);
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::CreateBookmarkLC
// ---------------------------------------------------------------------------
//
MVPbkContactBookmark* CVPbkFoldingContactView::CreateBookmarkLC(
        TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0, 
        VPbkError::Panic( VPbkError::EInvalidContactIndex ) );

    return iFoldingContact->CreateBookmarkLC();
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::IndexOfBookmarkL
// ---------------------------------------------------------------------------
//
TInt CVPbkFoldingContactView::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark) const
    {
    const TVPbkFoldingContactBookmark* bookmark = 
        dynamic_cast<const TVPbkFoldingContactBookmark*>(&aContactBookmark);
    if (bookmark && iFoldingContact == &bookmark->Contact())
        {
        // Folding view has always only one contact in index 0
        return 0;
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::ViewFiltering
// ---------------------------------------------------------------------------
//
MVPbkContactViewFiltering* CVPbkFoldingContactView::ViewFiltering()
    {
    // Folding view doesn't support filtering yet.
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// CVPbkFoldingContactView::ExpandLC
// ---------------------------------------------------------------------------
//
MVPbkContactViewBase* CVPbkFoldingContactView::ExpandLC(
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder) const
    {
    /// Folding view can be expanded if it has one sub view definition. 
    /// If it has more than one then the rest of subview definitions are 
    /// ignored.
    if ( iViewDefinition->SubViewCount() >= 1 )
        {
        // Create a view according to first subview definition.
        return iContactManager.CreateContactViewLC(
            aObserver, iViewDefinition->SubViewAt( 0 ), aSortOrder );
        }
    return NULL;
    }

//End of file
