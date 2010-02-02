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
* Description:  A base class for store list implementations
*
*/


#include <CVPbkContactStoreList.h>

#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkError.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CVPbkContactStoreList::CVPbkContactStoreList
// ---------------------------------------------------------------------------
//
CVPbkContactStoreList::CVPbkContactStoreList()
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreList::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactStoreList* CVPbkContactStoreList::NewL()
    {
    CVPbkContactStoreList* self = new( ELeave ) CVPbkContactStoreList;
    return self;
    }
    
// ---------------------------------------------------------------------------
// CVPbkContactStoreList::~CVPbkContactStoreList
// ---------------------------------------------------------------------------
//
CVPbkContactStoreList::~CVPbkContactStoreList()
    {
    iStoresToOpen.Close();
    iStores.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreList::AppendL
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactStoreList::AppendL( MVPbkContactStore* aStore )
    {
    iStores.AppendL( aStore );
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreList::Count
// ---------------------------------------------------------------------------
//
TInt CVPbkContactStoreList::Count() const
    {
    return iStores.Count();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreList::Count
// ---------------------------------------------------------------------------
//    
MVPbkContactStore& CVPbkContactStoreList::At( TInt aIndex ) const
    {
    __ASSERT_DEBUG( aIndex < Count(), 
        VPbkError::Panic( VPbkError::EInvalidStoreIndex ) );
    return *iStores[aIndex];
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreList::Find
// ---------------------------------------------------------------------------
//        
MVPbkContactStore* CVPbkContactStoreList::Find( 
        const TVPbkContactStoreUriPtr& aUri ) const
    {
    MVPbkContactStore* result = NULL;
    
    const TInt count = iStores.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if ( iStores[i]->StoreProperties().Uri().Compare(aUri, 
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) == 0 )
            {
            result = iStores[i];
            break;        
            }
        }
    return result;
    }
    
// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreList::OpenAllL
// ---------------------------------------------------------------------------
//        
void CVPbkContactStoreList::OpenAllL( 
        MVPbkContactStoreListObserver& aObserver )
    {
    if (iObserver)
        {
        // Leave as documented in MVPbkContactStoreList
        User::Leave( KErrInUse );
        }
    
    iObserver = &aObserver;
    iStoresToOpen.Reset();
    
    const TInt count = Count();
    for (TInt i = 0; i < count; ++i)
        {
        MVPbkContactStore& store = At( i );
        store.OpenL( aObserver );
        store.OpenL( *this );
        // Append stores to list that is used the monitor signalling
        iStoresToOpen.AppendL( &store );
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreList::CloseAll
// ---------------------------------------------------------------------------
//        
void CVPbkContactStoreList::CloseAll( 
        MVPbkContactStoreListObserver& aObserver )
    {
    const TInt count = Count();
    if ( iObserver == &aObserver )
        {
        // If observer is the same that called OpenAllL then stop operning
        for (TInt i = 0; i < count; ++i)
            {
            MVPbkContactStore& store = At( i );
            At(i).Close( *this );
            At(i).Close( aObserver );
            } 
        iObserver = NULL;   
        }
    else
        {
        // If observer is not the same that called OpenAllL then
        // close stores for that observer only. The observer that
        // called OpenAllL is still waiting the OpenComplete
        for (TInt i = 0; i < count; ++i)
            {
            MVPbkContactStore& store = At( i );
            At(i).Close( aObserver );
            } 
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreList::StoreReady
// ---------------------------------------------------------------------------
//        
void CVPbkContactStoreList::StoreReady( 
        MVPbkContactStore& aContactStore )
    {
    TInt index = iStoresToOpen.Find( &aContactStore );
    if ( index != KErrNotFound )
        {
        iStoresToOpen.Remove( index );
        }
    
    SignalOpenComplete();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreList::StoreUnavailable
// ---------------------------------------------------------------------------
//            
void CVPbkContactStoreList::StoreUnavailable( 
        MVPbkContactStore& aContactStore, TInt /*aReason*/ )
    {
    TInt index = iStoresToOpen.Find( &aContactStore );
    if ( index != KErrNotFound )
        {
        iStoresToOpen.Remove( index );
        }
    
    SignalOpenComplete();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreList::HandleStoreEventL
// ---------------------------------------------------------------------------
//                
void CVPbkContactStoreList::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // List is not interested in store events
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreList::SignalOpenComplete
// ---------------------------------------------------------------------------
//                
void CVPbkContactStoreList::SignalOpenComplete()
    {
    if ( iObserver && iStoresToOpen.Count() == 0 )
        {
        MVPbkContactStoreListObserver* observer = iObserver;
        iObserver = NULL;
        observer->OpenComplete();
        }
    }
    
//End of file
