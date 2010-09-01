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
* Description:  Implements a contact link to sim contact
*
*/



#ifndef VPBKSIMSTORE_CCONTACTLINK_H
#define VPBKSIMSTORE_CCONTACTLINK_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactLinkPacking.h>
#include <MVPbkContactBookmark.h>

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATION

/**
*  Implements a contact link to sim contact
*
*/
NONSHARABLE_CLASS( CContactLink ): 
        public CBase,
        public MVPbkContactLink, 
        public MVPbkContactLinkPacking,
        public MVPbkContactBookmark
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore the store to which this link belongs
        * @param aSimIndex the index of the contact in the sim store
        * @return a new instance of this class
        */
        static CContactLink* NewLC( CContactStore& aStore, 
            TInt aSimIndex, TInt aFieldId = KErrNotFound );
        
        /**
        * Two-phased constructor.
        * @param aStore the store to which this link belongs
        * @param aReadStream a stream that contains the sim index
        * @return a new instance of this class
        */    
        static CContactLink* NewLC( CContactStore& aStore, 
            RReadStream& aReadStream );
        
    public: // New functions
        
        /**
        * An utility function to check if given link is valid for the store
        * @param aStore the store in which the link should belong
        * @param aLink the link that's valitated
        * @return ETrue if the link is valid
        */
        static TBool IsValid( const MVPbkContactStore& aStore,
            const MVPbkContactLink& aLink );

        /**
        * Returns the sim index of the link
        * @return the sim index of the link
        */
        inline TInt SimIndex() const;
        
        /**
        * Returns the id of the field
        * @return the id of the field
        */
        inline TInt FieldId() const;

    public: // Functions from base classes

        /**
        * From MVPbkContactLink
        */
        MVPbkContactStore& ContactStore() const;
        
        /**
        * From MVPbkContactLink
        */
        TBool IsSame( const MVPbkContactLink& aOther ) const;
        
        /**
        * From MVPbkContactLink
        */
        TBool RefersTo( const MVPbkBaseContact& aContact ) const;

        /**
        * From MVPbkContactLink
        * SIM contact link is only a transient link.
        */
        const MVPbkStreamable* Streamable() const { return NULL; }

        /**
        * From MVPbkContactLink
        */
        const MVPbkContactLinkPacking& Packing() const;
        
        /**
        * From MVPbkContactLink
        */
        MVPbkContactLink* CloneLC() const;
        
        /**
        * From MVPbkContactLinkPacking
        */
        TInt InternalPackedSize() const;
        
        /**
        * From MVPbkContactLinkPacking
        */
        void PackInternalsL( RWriteStream& aStream ) const;
    
    private:    // Construction
        
        /**
        * C++ constructor.
        */
        CContactLink( CContactStore& aStore );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TInt aSimIndex, TInt aFieldId );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RReadStream& aReadStream );
        
    private:    // Data
        /// The store of the contact
        CContactStore& iStore;
        /// The sim index of the contact
        TInt iSimIndex;
        /// Field index if this is a field link
        TInt iFieldId;
         
    };

// INLINE FUNCTIONS
inline TInt CContactLink::SimIndex() const
    {
    return iSimIndex;
    }
    
inline TInt CContactLink::FieldId() const
    {
    return iFieldId;
    }
} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CCONTACTLINK_H
            
// End of File
