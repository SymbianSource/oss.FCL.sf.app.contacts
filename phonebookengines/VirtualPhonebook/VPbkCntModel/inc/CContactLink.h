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
* Description:  The virtual phonebook contact link
*
*/



#ifndef CCONTACTLINK_H
#define CCONTACTLINK_H

// INCLUDES
#include <MVPbkContactLink.h>
#include <MVPbkStreamable.h>
#include <MVPbkContactLinkPacking.h>
#include <MVPbkContactBookmark.h>
#include <e32base.h>
#include <cntdef.h>
#include <e32des8.h>
#include "TVPbkContactStoreUriPtr.h"

class RReadStream;
class CVPbkContactLinkArray;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATIONS

/**
 * A externalizable Contact link that can be referenced from external systems. 
 */
NONSHARABLE_CLASS( CContactLink ): 
        public CBase, 
        public MVPbkContactLink,
        public MVPbkStreamable,
        public MVPbkContactLinkPacking,
        public MVPbkContactBookmark
    {
    public: // constructor
        /**
         * Construct a new link from aLinkBuf. 
         * @param aStoreUri  The complete URI of this store
         */
        static CContactLink* NewLC( CContactStore& aContactStore, 
                                   RReadStream& aStream );

        /**
         * Construct a new link from aId. 
         * @param aStoreUri  The complete URI of this store
         */
        static CContactLink* NewLC(
                CContactStore& aContactStore, 
                TContactItemId aId,
                TInt aFieldId = KErrNotFound );

        ~CContactLink();

    public: // interface
        /**
         * Returns ETrue if this aLink's aComponent is from this store. 
         * @param aStoreUri The complete URI of this store
         * @param aLink The link
         * @aComponent  The component that is compared
         */
        static TBool Matches( const TDesC& aStoreUri, 
                const MVPbkContactLink& aLink,
                TVPbkContactStoreUriPtr::TVPbkContactStoreUriComponent 
                                                        aComponent );

        /**
         * Returns ETrue if this aLink is from this store. 
         * The URI of the link matches this stores URI.
         * @param aStoreUri The complete URI of this store
         * @param aLink The link
         */
        static TBool Matches( const TVPbkContactStoreUriPtr& aStoreUri, 
                             const MVPbkContactLink& aLink );

        /**
         * Returns the contact model contact id of this link.
         */
        TContactItemId ContactId() const;
        
        /**
         * Returns the contact field id of this link.
         */
        TInt ContactFieldId() const;
        
        /**
         * Sets the contact model contact id of this link.
         */
        void SetContactId( TContactItemId aContactId );

        /**
         * Sets the contact field id of this link.
         */
        void SetContactFieldId( TInt aContactFieldId );

        /**
         * Contact database filename with file type extension.
         */
        const TPtrC ContactDbFileName() const;

    public: // from MVPbkContactLink
        MVPbkContactStore& ContactStore() const;
        TBool IsSame( const MVPbkContactLink& aOther ) const;
        TBool RefersTo( const MVPbkBaseContact& aContact ) const;
        const MVPbkStreamable* Streamable() const;
        const MVPbkContactLinkPacking& Packing() const;
        MVPbkContactLink* CloneLC() const;

    public: // From MVPbkStreamable
        void ExternalizeL( RWriteStream& aStream ) const;
        TInt ExternalizedSize() const;

    public: // from MVPbkContactLinkPacking
        TInt InternalPackedSize() const;
        void PackInternalsL( RWriteStream& aStream ) const;

    private: // implementation
        CContactLink( CContactStore& aContactStore );
        void CommonConstructL();
        void ConstructL( RReadStream& aStream );
        void ConstructL( TContactItemId aId, TInt aFieldId );

    private: // data members
        /// Ref: Contact store this link belongs to.
        CContactStore& iContactStore;
        /// Own: Contact item id
        TContactItemId iId;
        /// Own: Contact field id
        TInt iFieldId;
        /// Own: a link array for externalizing
        CVPbkContactLinkArray* iLinkArray;
    };

// INLINE FUNCTIONS

} // namespace VPbkCntModel

#endif // CCONTACTLINK_H
//End of file


