/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  .
*
*/


#include "spbcontentprovider.h"
#include "spbcontentproviderprivate.h"
#include <featmgr.h>
#include <MVPbkContactStore.h>

// LOCAL METHODS AND CONSTANTS
namespace {

inline void RemoveFlag( TInt32& aFlags, TInt aFlag )
    {
    aFlags &= ~aFlag;
    }
}

// ----------------------------------------------------------------------------
// CSpbContentProvider::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSpbContentProvider* CSpbContentProvider::NewL(
        CVPbkContactManager& aContactManager,
        CPbk2StoreManager& aStoreManager,
        TInt32 aFeatures)
    {
    CSpbContentProvider* self = new (ELeave) CSpbContentProvider;
    CleanupStack::PushL( self );
    self->ConstructL( aContactManager, aStoreManager, aFeatures );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSpbContentProvider::CSpbContentProvider
// ----------------------------------------------------------------------------
//
inline CSpbContentProvider::CSpbContentProvider()
    {
    }

// ----------------------------------------------------------------------------
// CSpbContentProvider::ConstructL
// ----------------------------------------------------------------------------
//
inline void CSpbContentProvider::ConstructL(
        CVPbkContactManager& aContactManager,
        CPbk2StoreManager& aStoreManager,
        TInt32 aFeatures )
    {
    FeatureManager::InitializeLibL();
    if( !FeatureManager::FeatureSupported( KFeatureIdFfContactsSocial ) )
        {
        // Status message and service icon is not fetched if FF_CONTACTS_SOCIAL
        // feature flag is not defined
        RemoveFlag( aFeatures, EStatusMessage ); 
        RemoveFlag( aFeatures, EServiceIcon ); 
        }
    FeatureManager::UnInitializeLib();
    iImpl = CSpbContentProviderPrivate::NewL(aContactManager, aStoreManager, aFeatures);
    }

// ----------------------------------------------------------------------------
// CSpbContentProvider::~CSpbContentProvider
// ----------------------------------------------------------------------------
//
CSpbContentProvider::~CSpbContentProvider()
    {
    delete iImpl;
    }

// ----------------------------------------------------------------------------
// CSpbContentProvider::AddObserverL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSpbContentProvider::AddObserverL( 
        MSpbContentProviderObserver& aObserver )
    {
    iImpl->AddObserverL( aObserver );
    }

// ----------------------------------------------------------------------------
// CSpbContentProvider::RemoveObserver
// ----------------------------------------------------------------------------
//
EXPORT_C void CSpbContentProvider::RemoveObserver( 
        MSpbContentProviderObserver& aObserver )
    {
    iImpl->RemoveObserver( aObserver );
    }

// ----------------------------------------------------------------------------
// CSpbContentProvider::GetContentL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSpbContentProvider::GetContentL(
        MVPbkContactLink& aLink,
        HBufC*& aText,
        TPbk2IconId& aIconId,
        TSpbContentType& aType )
    {
    iImpl->GetContentL( aLink, aText, aIconId, aType );
    }

// ----------------------------------------------------------------------------
// CSpbContentProvider::CreateServiceIconLC
// ----------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CSpbContentProvider::CreateServiceIconLC(
    const TPbk2IconId& aIconId )
    {
    return iImpl->CreateServiceIconLC( aIconId );
    }

// ----------------------------------------------------------------------------
// CSpbContentProvider::CleanContentL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSpbContentProvider::CleanContentL( MVPbkContactStore& aStore )
    {
    return iImpl->CleanContentL( aStore );
    }

// end of file
