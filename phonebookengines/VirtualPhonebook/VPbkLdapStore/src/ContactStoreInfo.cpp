/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CContactStoreInfo implementation
*
*/


// INCLUDE FILES
#include "contactstoreinfo.h"
#include "contactstore.h"

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CContactStoreInfo::CContactStoreInfo
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactStoreInfo::CContactStoreInfo(CContactStore& aStore)
:   iStore(aStore)
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// CContactStoreInfo::NewL
// CContactStoreInfo two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreInfo* CContactStoreInfo::NewL(CContactStore& aStore)
    {
    return new(ELeave) CContactStoreInfo(aStore);
    }
// -----------------------------------------------------------------------------
// CContactStoreInfo::~CContactStoreInfo
// CContactStoreInfo Destructor
// -----------------------------------------------------------------------------
//
CContactStoreInfo::~CContactStoreInfo()
    {
    }

// -----------------------------------------------------------------------------
//                  MVPbkContactStoreInfo implementation
// -----------------------------------------------------------------------------
// CContactStoreInfo::MaxNumberOfContactsL
// Returns the maximum number of contacts in store.
// -----------------------------------------------------------------------------
//
TInt CContactStoreInfo::MaxNumberOfContactsL() const
    {
    return KVPbkStoreInfoUnlimitedNumber;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::NumberOfContactsL
// Returns the current number of contacts in the store.
// -----------------------------------------------------------------------------
//
TInt CContactStoreInfo::NumberOfContactsL() const
    {
    // Constantly changing returned currently cached count
    return iStore.ContactCount();
    }
        
// -----------------------------------------------------------------------------
// CContactStoreInfo::ReservedMemoryL
// Returns the amount of memory that this store reserves.
// -----------------------------------------------------------------------------
//
TInt64 CContactStoreInfo::ReservedMemoryL() const
    {
    // Remote store
    return 0;
    }

// -----------------------------------------------------------------------------
// CContactStoreInfo::FreeMemoryL
// Returns the amount of memory that can be used.
// -----------------------------------------------------------------------------
//
inline TInt64 CContactStoreInfo::FreeMemoryL() const
{
    // Remote store
    return 0;
}

// -----------------------------------------------------------------------------
// CContactStoreInfo::MaxNumberOfGroupsL
// Returns the maximum number of groups.
// -----------------------------------------------------------------------------
//
TInt CContactStoreInfo::MaxNumberOfGroupsL() const
    {
    return KVPbkStoreInfoUnlimitedNumber;
    }
        
// -----------------------------------------------------------------------------
// CContactStoreInfo::NumberOfGroupsL
// Returns the maximum number of groups.
// -----------------------------------------------------------------------------
//
TInt CContactStoreInfo::NumberOfGroupsL() const
    {
    // No groups
    return 0;
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
