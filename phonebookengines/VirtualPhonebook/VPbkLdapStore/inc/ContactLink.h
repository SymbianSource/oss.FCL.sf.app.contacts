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
* Description:  CContactLink - Contact link class.
*
*/


#ifndef __CONTACTLINK_H__
#define __CONTACTLINK_H__

// INCLUDES
#include <e32std.h>
#include <mvpbkcontactlink.h>
#include <mvpbkcontactlinkpacking.h>

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATION

/**
*  CContactLink - contact link.
*  Contact link class can be used as bookmark as well.
*/
class CContactLink : public CBase,
                     public MVPbkContactLink, 
                     public MVPbkContactLinkPacking
    {
    public:     // CContactLink public constructors and destructor

        /**
        @function   NewL
        @discussion Create CContactLink object
        @param      aStore Contact store
        @param      aIndex Contact index
        @param      aType  Contact field type id
        @return     Pointer to instance of CContactLink
        */
        static CContactLink* NewL(CContactStore& aStore,
                            TInt aIndex,TInt aType = 0);
        /**
        @function   NewLC
        @discussion Create CContactLink object
        @param      aStore Contact store
        @param      aIndex Contact index
        @param      aType  Contact field type id
        @return     Pointer to instance of CContactLink
        */
        static CContactLink* NewLC(CContactStore& aStore,
                            TInt aIndex,TInt aType = 0);
        /**
        @function  ~CContactLink
        @discussion CContactLink destructor
        */
        ~CContactLink();

    public:     // CContactLink public methods

        /**
        @function   Index
        @discussion Returns linked contact index.
        @return     Linked contact index.
        */
        TInt Index() const;

        /**
        @function   Type
        @discussion Returns linked contact field type id.
        @return     Linked contact field type id.
        */
        TInt Type() const;

        /**
        @function   Store
        @discussion Returns linked store.
        @return     Linked store.
        */
        CContactStore& Store() const;

    public:     // Methods from MVPbkContactLink

        /**
        @function   ContactStore
        @discussion Returns linked store.
        @return     Linked store.
        */
        MVPbkContactStore& ContactStore() const;
        
        /**
        @function   IsSame
        @discussion Returns true if this link refers to the same contact.
        @param      aOther Contact link to compare
        @return     True, this link refers to the same contact or false.
        */
        TBool IsSame(const MVPbkContactLink& aOther) const;
        
        /**
        @function   RefersTo
        @discussion Returns true if this link refers to the same contact.
        @param      aContact Contact to compare
        @return     True, this link refers to the same contact or false.
        */
        TBool RefersTo(const MVPbkBaseContact& aContact) const;

        /**
        @function   Streamable
        @discussion This contact link is not streamable.
        @return     Null, not streamable.
        */
        const MVPbkStreamable* Streamable() const { return NULL; }

        /**
        @function   Packing
        @discussion Returns contact link packing interface.
        @return     Packing interface.
        */
        const MVPbkContactLinkPacking& Packing() const;
        
        /**
        @function   CloneLC
        @discussion Clones this link.
        @return     Cloned link
        */
        MVPbkContactLink* CloneLC() const;

    public:     // Methods from MVPbkContactLinkPacking

        /**
        @function   InternalPackedSize
        @discussion Returns packed size.
        @return     Packed size
        */
        TInt InternalPackedSize() const;
        
        /**
        @function   PackInternalsL
        @discussion Packs contact link.
        @param      aStream Stream to pack.
        */
        void PackInternalsL(RWriteStream& aStream) const;

    private:    // CContactLink private constructors
        /**
        @function   CContactLink
        @discussion CContactLink default contructor
        @param      aStore Contact store
        @param      aIndex Contact index
        @param      aType  Contact field type id
        */
        CContactLink(CContactStore& aStore,TInt aIndex = 0,TInt aType = 0);

    private:    // CContactLink private member variables

        /// Contact store reference
        CContactStore& iStore;

        /// Contact index
        TInt iIndex;

        /// Contact field type id
        TInt iType;
    };

} // End of namespace LDAPStore
#endif // __CONTACTLINK_H__
