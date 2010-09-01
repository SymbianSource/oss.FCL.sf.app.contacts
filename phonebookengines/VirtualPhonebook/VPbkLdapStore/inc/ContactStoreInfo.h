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
* Description:  CContactStoreInfo - Contact store information.
*
*/


#ifndef __CONTACTSTOREINFO_H__
#define __CONTACTSTOREINFO_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mvpbkcontactstoreinfo.h>

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContactStore;

/**
*  CContactStoreInfo - contact store information.
* 
*/
class CContactStoreInfo : public CBase,
                          public MVPbkContactStoreInfo
    {
    public:     // CContactStoreInfo public constructors and destructor

        /**
        @function   NewL
        @discussion Create CContactStoreInfo object
        @param      aStore Contact store
        @return     Pointer to instance of CContactStoreInfo
        */
        static CContactStoreInfo* NewL(CContactStore& aStore);

        /**
        @function  ~CContactStoreInfo
        @discussion CContactStoreInfo destructor
        */
        ~CContactStoreInfo();

    public:     // Methods from MVPbkContactStoreInfo

        /**
        @function   MaxNumberOfContactsL
        @discussion Returns the maximum number of contacts in store.
        @return     KVPbkStoreInfoUnlimitedNumber
        */
        TInt MaxNumberOfContactsL() const;

        /**
        @function   NumberOfContactsL
        @discussion Returns the current number of contacts in the store.
        @return     Number of contacts in the store
        */
        TInt NumberOfContactsL() const;
        
        /**
        @function   ReservedMemoryL
        @discussion Returns the amount of memory that this store reserves.
        @return     Amount of memory that this store reserves
        */
        TInt64 ReservedMemoryL() const;
        
        /**
        @function   FreeMemoryL
        @discussion Returns the amount of memory that can be used.
        @return     Amount of memory that this store reserves
        */
        TInt64 FreeMemoryL() const;
        
        /**
        @function   MaxNumberOfGroupsL
        @discussion Returns the maximum number of groups.
        @return     Maximum number of groups
        */
        TInt MaxNumberOfGroupsL() const;
        
        /**
        @function   NumberOfGroupsL
        @discussion Returns amount of groups in the store.
        @return     Number of groups
        */
        TInt NumberOfGroupsL() const;

    private:    // CContactStoreInfo private constructors
        /**
        @function   CContactStoreInfo
        @discussion CContactStoreInfo default contructor
        @param      aStore Contact store
        */
        CContactStoreInfo(CContactStore& aStore);

    private:    // CContactStoreInfo private member variables

        /// Contact store reference
        CContactStore& iStore;

    };

} // End of namespace LDAPStore
#endif // __CONTACTSTOREINFO_H__
