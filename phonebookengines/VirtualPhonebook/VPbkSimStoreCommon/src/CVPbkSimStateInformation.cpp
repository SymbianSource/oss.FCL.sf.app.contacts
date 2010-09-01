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
* Description:  Offers different sim status information
*
*/


// INCLUDE FILES

#include "CVPbkSimStateInformation.h"

// From Virtual Phonebook
#include <VPbkSimStateDefinitions.h>
#include <MVPbkSimPhone.h>

// System includes
#include <featmgr.h> // Feature manager
#include <sacls.h> // Global variables
#include <centralrepository.h> // central repository

#include <VPbkDebug.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CVPbkSimStateInformation::CVPbkSimStateInformation
// ---------------------------------------------------------------------------
//
CVPbkSimStateInformation::CVPbkSimStateInformation()
    {
    }

// ---------------------------------------------------------------------------
// CVPbkSimStateInformation::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkSimStateInformation::ConstructL()
    {
    FeatureManager::InitializeLibL();
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkSimStateInformation* CVPbkSimStateInformation::NewL()
    {
    CVPbkSimStateInformation* self = new(ELeave) CVPbkSimStateInformation;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::~CVPbkSimStateInformation
// -----------------------------------------------------------------------------
//
CVPbkSimStateInformation::~CVPbkSimStateInformation()
    {
    FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::SimInserted
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkSimStateInformation::SimInsertedL()
    {
    TBool result = EFalse;

    RProperty property;   
    TInt simStatus( KErrNotFound );
    
    TInt ret( property.Get(
        KVPbkSimStatusPSCategory, 
        VPbkSimStoreImpl::SimStatusPSKey, 
        simStatus ) );
    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CVPbkSimStateInformation::SimInsertedL: res %d val %d"),
        ret, simStatus);
        
    if ( simStatus == VPbkSimStoreImpl::SimOkStatus && ret == KErrNone )
        {
        result = ETrue;
        }
    
    property.Close();
    return result;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::BTSapEnabled
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkSimStateInformation::BTSapEnabledL()
    {
    TBool result( EFalse );
    if ( FeatureManager::FeatureSupported( KFeatureIdBtSap ) )
        {
        TInt state( KErrNotFound );
        RProperty property;
        TInt ret( property.Get(
            KVPbkBTSapStatusPSCategory,
            VPbkSimStoreImpl::BTSapStatusPSKey,
            state ) );
        property.Close();

        if ( state == VPbkSimStoreImpl::BTSapOkStatus && ret == KErrNone )
            {
            result = ETrue;
            }

        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "CVPbkSimStateInformation::BTSapEnabledL: result %d val %d"),
            result, state);
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::ActiveFdnBlocksAdnStoreL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkSimStateInformation::ActiveFdnBlocksAdnStoreL(
        MVPbkSimPhone& aSimPhone)
    {
    // Set default to not to block ADN
    TBool result = EFalse;
    
    // Check if the FDN is active
    MVPbkSimPhone::TFDNStatus fdnStatus = aSimPhone.FixedDialingStatus();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CVPbkSimStateInformation::ActiveFdnBlocksAdnStoreL: FDN status %d"),
        fdnStatus );
        
    if (fdnStatus == MVPbkSimPhone::EFdnIsActive || 
        fdnStatus == MVPbkSimPhone::EFdnIsPermanentlyActive)
        {
        // TRAPPED because the name space is actually from Pbk2.
        // If it's not there return the default value
        TRAPD(error,
            {
            CRepository* repository = CRepository::NewLC(
                VPbkSimStoreImpl::KLocalVaritationNameSpaceUid);
            TInt keyValue = KErrNotFound;
            // ADN blocked if SIM card and variation is on.
            TInt ret = repository->Get(
                VPbkSimStoreImpl::KLocalVariationFlagsKey, keyValue);
            if (ret == KErrNone && 
                !(keyValue & VPbkSimStoreImpl::KEnableAdnWhenFdnOn) &&
                !aSimPhone.USimAccessSupported())
                {
                result = ETrue;
                }
            CleanupStack::PopAndDestroy(repository);
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
                "CVPbkSimStateInformation::ActiveFdnBlocksAdnStoreL: ret %d val %d"),
                ret, keyValue );
            });         
        
        // CRepository::NewLC leaves with KErrNotFound if there is no
        // such namespace
        if (error != KErrNone && error != KErrNotFound)
            {
            User::Leave(error);
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CVPbkSimStateInformation::NewSimCardL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVPbkSimStateInformation::NewSimCardL()
    {
    TBool result = EFalse;
    RProperty property;
    
    TInt simOwnedStatus( KErrNotFound );
    TInt ret( property.Get( 
        KVPbkSimStatusPSCategory,
        VPbkSimStoreImpl::SimOwnedStatusPSKey, 
        simOwnedStatus ) );
    property.Close();
    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CVPbkSimStateInformation::NewSimCardL: ret %d val %d"),
        ret, simOwnedStatus );
        
    if (ret == KErrNone && 
        simOwnedStatus == VPbkSimStoreImpl::SimNotOwnedStatus)
        {
        result = ETrue;
        }
        
    return result;
    }
    
//  End of File  
