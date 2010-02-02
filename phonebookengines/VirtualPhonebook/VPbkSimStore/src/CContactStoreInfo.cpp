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
* Description:  A store info implementation for the sim stores
*
*/



// INCLUDE FILES
#include "CContactStoreInfo.h"

#include "CContactStore.h"
#include "CFieldTypeMappings.h"
#include <MVPbkSimCntStore.h>
#include <TVPbkSimStoreProperty.h>

namespace VPbkSimStore {


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactStoreInfo::CContactStoreInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactStoreInfo::CContactStoreInfo(CContactStore& aStore)
:   iStore(aStore)
    {
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreInfo* CContactStoreInfo::NewL(CContactStore& aStore)
    {
    return new(ELeave) CContactStoreInfo(aStore);
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
    TVPbkGsmStoreProperty prop;
    User::LeaveIfError(
        iStore.NativeStore().GetGsmStoreProperties(prop));
    return prop.iTotalEntries;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::NumberOfContactsL
// -----------------------------------------------------------------------------
//                
TInt CContactStoreInfo::NumberOfContactsL() const
    {
    TVPbkGsmStoreProperty prop;
    User::LeaveIfError(
        iStore.NativeStore().GetGsmStoreProperties(prop));
    return prop.iUsedEntries;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::ReservedMemoryL
// -----------------------------------------------------------------------------
//        
TInt64 CContactStoreInfo::ReservedMemoryL() const
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::FreeMemoryL
// -----------------------------------------------------------------------------
//        
TInt64 CContactStoreInfo::FreeMemoryL() const
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::MaxNumberOfGroupsL
// -----------------------------------------------------------------------------
//
TInt CContactStoreInfo::MaxNumberOfGroupsL() const
    {
    // USIM groups not yet supported
    return 0;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::NumberOfGroupsL
// -----------------------------------------------------------------------------
//
TInt CContactStoreInfo::NumberOfGroupsL() const
    {
    // USIM groups not yet supported
    return 0;
    }
} // namespace VPbkSimStore
//  End of File  
