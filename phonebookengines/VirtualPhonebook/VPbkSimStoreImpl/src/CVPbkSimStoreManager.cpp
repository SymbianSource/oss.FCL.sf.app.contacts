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
* Description:  A manager class for creating sim stores
*
*/



// INCLUDE FILES
#include "CVPbkSimStoreManager.h"

#include "CPhone.h"
#include "CSatRefreshNotifier.h"
#include "CBtSapNotification.h"
#include "TStoreParams.h"

#include <VPbkSimStoreImplError.h>
#include <MVPbkSimCntStore.h>
#include <CVPbkSimStateInformation.h>
#include <VPbkSimStoreCommonUtil.h>

// ============================ LOCAL FUNCTIONS ================================

namespace {

inline TInt FindStore( RPointerArray<MVPbkSimCntStore>& aStoreArray,
    TVPbkSimStoreIdentifier aIdentifier )
    {
    const TInt count = aStoreArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aStoreArray[i]->Identifier() == aIdentifier )
            {
            return i;
            }
        }
    return KErrNotFound;
    }
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkSimStoreManager::CVPbkSimStoreManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CVPbkSimStoreManager::CVPbkSimStoreManager()
    {
    }

// -----------------------------------------------------------------------------
// CVPbkSimStoreManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
inline void CVPbkSimStoreManager::ConstructL()
    {
    iSatNotifier = VPbkSimStoreImpl::CSatRefreshNotifier::NewL();
    iSimStateInfo = CVPbkSimStateInformation::NewL();
    iBtSapNotifier = 
        VPbkSimStoreImpl::CBtSapNotification::NewL( *iSimStateInfo );
    iBtSapNotifier->ActivateL();
    iPhone = new( ELeave ) VPbkSimStoreImpl::CPhone(
        *iSatNotifier, *iSimStateInfo, *iBtSapNotifier);
    }

// -----------------------------------------------------------------------------
// CVPbkSimStoreManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimStoreManager* CVPbkSimStoreManager::NewL()
    {
    CVPbkSimStoreManager* self = new(ELeave) CVPbkSimStoreManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CVPbkSimStoreManager::~CVPbkSimStoreManager()
    {
    iStoreArray.ResetAndDestroy();
    delete iPhone;
    delete iSatNotifier;
    delete iBtSapNotifier;
    delete iSimStateInfo;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStoreManager::Phone
// -----------------------------------------------------------------------------
//
EXPORT_C MVPbkSimPhone& CVPbkSimStoreManager::Phone()
    {
    return *iPhone;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStoreManager::StoreL
// -----------------------------------------------------------------------------
//
EXPORT_C MVPbkSimCntStore& CVPbkSimStoreManager::StoreL(
    TVPbkSimStoreIdentifier aIdentifier )
    {
    TInt index = FindStore( iStoreArray, aIdentifier );
    if ( index != KErrNotFound )
        {
        return *iStoreArray[index];
        }
    else
        {
        // Get the maximum length of the phone number according to Phonebook
        // specification
        TInt maxNumberLength = 
            VPbkSimStoreCommonUtil::SystemMaxPhoneNumberLengthL();
        VPbkSimStoreImpl::TStoreParams params( aIdentifier, *iPhone,
            *iSatNotifier, *iBtSapNotifier, *iSimStateInfo, 
            maxNumberLength );
        MVPbkSimCntStore* store = iPhone->CreateStoreLC( params );
        iStoreArray.AppendL( store );
        CleanupStack::Pop(); // store
        return *store;
        }
    }

//  End of File
