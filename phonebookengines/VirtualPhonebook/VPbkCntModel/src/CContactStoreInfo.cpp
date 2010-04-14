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
* Description:  A store info implementation for the contact model
*
*/



// INCLUDE FILES
#include "CContactStoreInfo.h"

#include "CContactStore.h"
#include "CContactStoreDomain.h"
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>
#include <cntdb.h>

namespace VPbkCntModel {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactStoreInfo::CContactStoreInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactStoreInfo::CContactStoreInfo(CContactStore& aStore,
        CContactStoreDomain& aDomain)
        :   iStore(aStore),
            iDomain(aDomain)
    {
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreInfo* CContactStoreInfo::NewL(CContactStore& aStore,
        CContactStoreDomain& aDomain)
    {
    return new(ELeave) CContactStoreInfo(aStore,aDomain);
    }
    
// Destructor
CContactStoreInfo::~CContactStoreInfo()
    {
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::MaxNumberOfContactsL
// -----------------------------------------------------------------------------
//        
TInt CContactStoreInfo::MaxNumberOfContactsL() const
    {
    return KVPbkStoreInfoUnlimitedNumber;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::NumberOfContactsL
// -----------------------------------------------------------------------------
//                
TInt CContactStoreInfo::NumberOfContactsL() const
    {
    return iStore.NativeDatabase().CountL() - NumberOfGroupsL();
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::ReservedMemoryL
// -----------------------------------------------------------------------------
//        
TInt64 CContactStoreInfo::ReservedMemoryL() const
    {
    return iStore.NativeDatabase().FileSize();
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::FreeMemoryL
// -----------------------------------------------------------------------------
//        
TInt64 CContactStoreInfo::FreeMemoryL() const
    {
    TDriveUnit driveUnit;
    iStore.NativeDatabase().DatabaseDrive(driveUnit);
    
    TVolumeInfo volInfo;
    User::LeaveIfError(iDomain.FsSession().Volume(volInfo,driveUnit));
    return volInfo.iFree;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::MaxNumberOfGroupsL
// -----------------------------------------------------------------------------
//
TInt CContactStoreInfo::MaxNumberOfGroupsL() const
    {
    return KVPbkStoreInfoUnlimitedNumber;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::NumberOfGroupsL
// -----------------------------------------------------------------------------
//        
TInt CContactStoreInfo::NumberOfGroupsL() const
    {
    if( !iGroupsFetched )
        {
        CContactIdArray* arr = iStore.NativeDatabase().GetGroupIdListL();
        iGroupsFetched = ETrue;
        const TInt count = arr->Count();
        delete arr;
        return count;
        }
    return iStore.NativeDatabase().GroupCount();
    }
    
} // namespace VPbkCntModel
//  End of File  
