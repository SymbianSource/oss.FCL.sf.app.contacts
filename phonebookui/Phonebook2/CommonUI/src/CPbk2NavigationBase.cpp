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
* Description:  Phonebook 2 contact navigator base.
*
*/


#include "CPbk2NavigationBase.h"

// Phonebook 2
#include <MPbk2NavigationObserver.h>
#include <MPbk2ContactNavigationLoader.h>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactLink.h>

// System Includes
#include <coemain.h>

// Debugging headers
#include <Pbk2Debug.h>


// --------------------------------------------------------------------------
// CPbk2NavigationBase::CPbk2NavigationBase
// --------------------------------------------------------------------------
//
CPbk2NavigationBase::CPbk2NavigationBase
        ( MPbk2NavigationObserver& aObserver,
          MPbk2ContactNavigationLoader& aNavigationLoader,
          TArray<MVPbkContactStore*> aStoreList ):
            iNavigationObserver( aObserver ),
            iNavigationLoader( aNavigationLoader ),
            iStoreList( aStoreList )
    {
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::~CPbk2NavigationBase
// --------------------------------------------------------------------------
//
CPbk2NavigationBase::~CPbk2NavigationBase()
    {
    RemoveViewObserver();
    delete iOwnedView;
    
    CloseStores();
    
    delete iIdleNotifier;
    delete iDeletedContactInfo;
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::BaseConstructL
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::BaseConstructL()
    {
    iDeletedContactInfo = new( ELeave ) CDeletedContactInfo( *this );
    
    const TInt storeCount = iStoreList.Count();
    for (TInt i = 0; i < storeCount; ++i)
        {
        MVPbkContactStore& store = *iStoreList[i];
        // Asynchronous call to open a store
        store.OpenL( *this );
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2NavigationBase::SetScrollViewL
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::SetScrollViewL
        ( MVPbkContactViewBase* aScrollView, TBool aOwnershipTransfered )
    {
    // If there was a scroll view set, remove observer and delete it
    if ( iScrollView )
        {
        iScrollView->RemoveObserver( *this );
        delete iOwnedView;
        iOwnedView = NULL;
        }

    iScrollView = NULL;

    // Set new scroll view and add observer
    if ( aScrollView )
        {
        iScrollView = aScrollView;
        if ( aOwnershipTransfered )
            {
            iOwnedView = aScrollView;
            }
        iScrollView->AddObserverL( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::CurrentContactLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPbk2NavigationBase::CurrentContactLC() const
    {
    MVPbkContactLink* link = iDeletedContactInfo->FocusedContactOrNullLC();
    if ( !link )
        {
        CleanupStack::Pop( link );
        link = iNavigationLoader.ContactLinkLC();
        }
    return link;
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::ContactViewReady
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2VoidNavigation::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::ContactRemovedFromView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    TRAPD( res, HandleContactRemovedFromViewL( aView,
            aIndex, aContactLink ) );
    if ( res != KErrNone )
        {
        CCoeEnv::Static()->HandleError( res );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt /*aError*/,
          TBool /*aErrorNotified*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::StoreReady
        ( MVPbkContactStore& /*aContactStore*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::StoreUnavailable
        ( MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::HandleStoreEventL
        ( MVPbkContactStore& aContactStore,
          TVPbkContactStoreEvent aStoreEvent )
    {
    iDeletedContactInfo->HandleStoreEventL( aContactStore, aStoreEvent );
    
    switch ( aStoreEvent.iEventType )
        {
        case TVPbkContactStoreEvent::EContactChanged:
            {
            MVPbkContactLink* currentContact =
                iNavigationLoader.ContactLinkLC();

            if ( currentContact && aStoreEvent.iContactLink &&
                currentContact->IsSame( *aStoreEvent.iContactLink ) )
                {
                // Reload current contact
                iNavigationLoader.ChangeContactL( *currentContact );
                }
            CleanupStack::PopAndDestroy(); // currentContact
            break;
            }
        case TVPbkContactStoreEvent::EGroupDeleted: // FALLTHROUGH
        case TVPbkContactStoreEvent::EContactDeleted:
            {
            // Start the call back after we have got event from both
            // store and view. But if we do not have iScrollView then only
            // store event is enough.
            if ( iDeletedContactInfo->DeletedAndRemoved() || !iScrollView )
                {
                StartIdleCallBackL();
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::HandleContactRemovedFromViewL
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::HandleContactRemovedFromViewL( 
        MVPbkContactViewBase& aView,
        TInt aIndex,
        const MVPbkContactLink& aContactLink )
    {
    iDeletedContactInfo->HandleContactRemovedFromViewL( aView,
            aIndex, aContactLink );
    
    if ( iDeletedContactInfo->DeletedAndRemoved() )
        {
        StartIdleCallBackL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::CloseStores
// --------------------------------------------------------------------------
//        
void CPbk2NavigationBase::CloseStores()
    {
    for ( TInt i = 0; i < iStoreList.Count(); ++i )
        {
        MVPbkContactStore& store = *iStoreList[i];
        store.Close( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::RemoveViewObserver
// --------------------------------------------------------------------------
//        
void CPbk2NavigationBase::RemoveViewObserver()
    {
    if ( iScrollView )
        {
        iScrollView->RemoveObserver( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::StartIdleCallBackL
// --------------------------------------------------------------------------
//    
void CPbk2NavigationBase::StartIdleCallBackL()
    {
    // The viewed contact was deleted, signal the
    // observer asynchronously that the contact was deleted
    if ( !iIdleNotifier )
        {
        iIdleNotifier = CIdle::NewL( CActive::EPriorityStandard );
        }
    else
        {
        iIdleNotifier->Cancel();
        }
    iIdleNotifier->Start
        ( TCallBack( &IdleNotifierCallBack, this ) );
    }
    
// --------------------------------------------------------------------------
// CPbk2NavigationBase::IdleNotifierCallBack
// --------------------------------------------------------------------------
//
TInt CPbk2NavigationBase::IdleNotifierCallBack( TAny* aSelf )
    {
    static_cast<CPbk2NavigationBase*>( aSelf )->HandleIdleNotify();
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::HandleIdleNotify
// --------------------------------------------------------------------------
//
void CPbk2NavigationBase::HandleIdleNotify()
    {
    iNavigationObserver.HandleNavigationEvent
        ( MPbk2NavigationObserver::EContactDeleted );
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::CDeletedContactInfo::CDeletedContactInfo
// --------------------------------------------------------------------------
//
CPbk2NavigationBase::CDeletedContactInfo::CDeletedContactInfo( 
        CPbk2NavigationBase& aNavigationBase )
        :   iNavigationBase ( aNavigationBase ),
            iRemovedContactIndex( KErrNotFound )
    {
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::CDeletedContactInfo::~CDeletedContactInfo
// --------------------------------------------------------------------------
//     
CPbk2NavigationBase::CDeletedContactInfo::~CDeletedContactInfo()
    {
    delete iDeletedContact;
    delete iRemovedContact;
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::CDeletedContactInfo::HandleContactRemovedFromViewL
// --------------------------------------------------------------------------
//             
void CPbk2NavigationBase::CDeletedContactInfo::HandleContactRemovedFromViewL(
        MVPbkContactViewBase& /*aView*/,
        TInt aIndex,
        const MVPbkContactLink& aContactLink )
    {
    /// Handle delete event but not if we already have the information
    /// that the current contact was deleted.
    if ( !DeletedAndRemoved() )
        {
        MVPbkContactLink* curContact = 
                iNavigationBase.NavigationLoader().ContactLinkLC();
        
        // Save the removed contact if it's the current contact
        if ( curContact && curContact->IsSame( aContactLink ) )
            {
            delete iRemovedContact;
            iRemovedContact = curContact;
            CleanupStack::Pop(); // curContact
            iRemovedContactIndex = aIndex;
            }
        else
            {
            CleanupStack::PopAndDestroy(); // curContact
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::CDeletedContactInfo::HandleStoreEventL
// --------------------------------------------------------------------------
//                     
void CPbk2NavigationBase::CDeletedContactInfo::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent aStoreEvent )
    {
    /// Handle delete event but not if we already have the information
    /// that the current contact was deleted.
    if ( !DeletedAndRemoved() &&
         (aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactDeleted
         || aStoreEvent.iEventType == TVPbkContactStoreEvent::EGroupDeleted))
        {
        // Save the deleted contact if it's the current contact.
        MVPbkContactLink* curContact = 
            iNavigationBase.NavigationLoader().ContactLinkLC();
        if ( curContact && aStoreEvent.iContactLink && 
             curContact->IsSame( *aStoreEvent.iContactLink ) )
            {
            delete iDeletedContact;
            iDeletedContact = curContact;
            CleanupStack::Pop(); // curContact
            }
        else
            {
            CleanupStack::PopAndDestroy(); // curContact
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::CDeletedContactInfo::DeletedAndRemoved
// --------------------------------------------------------------------------
//                             
TBool CPbk2NavigationBase::CDeletedContactInfo::DeletedAndRemoved() const
    {
    TBool res = EFalse;
    if ( iDeletedContact && iRemovedContact && 
         iRemovedContact->IsSame( *iDeletedContact ) )
        {
        res = ETrue;
        }
    return res;
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::CDeletedContactInfo::FocusedContactOrNullLC
// --------------------------------------------------------------------------
//                             
MVPbkContactLink* 
        CPbk2NavigationBase::CDeletedContactInfo::FocusedContactOrNullLC()
    {
    MVPbkContactLink* res = NULL;
    MVPbkContactViewBase* view = iNavigationBase.ScrollView();
    
    if ( DeletedAndRemoved() && view )
        {
        TInt index = iRemovedContactIndex;
        if ( index >= 0 )
            {
            const TInt lastIndex = view->ContactCountL() - 1;
            if ( index > lastIndex )
                {
                index = lastIndex;
                }
            
            // Returns a link to the contact that is in the same position
            // as the removed one or in the last position.   
            if ( index >= 0 )
                {
                res = view->CreateLinkLC( index );
                }
            }
        }
    
    if ( !res )
        {
        // LC -semantics, push also NULL to cleanup stack
        CleanupStack::PushL( res );
        }
        
    return res;
    }
// End of File
