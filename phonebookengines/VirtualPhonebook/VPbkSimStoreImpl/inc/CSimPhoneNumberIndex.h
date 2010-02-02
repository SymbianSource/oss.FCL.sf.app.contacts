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
* Description:  An index for sim phone numbers
*
*/


#ifndef VPBKSIMSTORE_CSIMPHONENUMBERINDEX_H
#define VPBKSIMSTORE_CSIMPHONENUMBERINDEX_H

//  INCLUDES
#include <e32base.h>
#include "CContactArray.h"
#include <VPbkPhoneNumberIndex.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CStoreBase;

// CLASS DECLARATION

/**
*  An index for sim phone numbers
*
*/
NONSHARABLE_CLASS(CSimPhoneNumberIndex) : public CBase,
                                          public MContactArrayObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSimPhoneNumberIndex* NewL( CStoreBase& aParentStore );
        
        /**
        * Destructor.
        */
        virtual ~CSimPhoneNumberIndex();

    public:  // Functions from base classes
        
        /**
        * From MContactArrayObserver
        */
        void ContactAdded( MVPbkSimContact& aSimContact );
            
        /**
        * From MContactArrayObserver
        */
        void ContactRemoved( MVPbkSimContact& aSimContact );
        
        /**
        * From MContactArrayObserver
        */
        void ContactChanged( MVPbkSimContact& aOldContact,
            MVPbkSimContact& aNewContact );
            
    public: // New functions
        
        /**
        * Return the actual phone number index implementation
        * @return the actual phone number index implementation
        */
        inline VPbkEngUtils::CPhoneNumberIndex<MVPbkSimContact>& Index() const;

        /**
        * Resets and builds the index again 
        */
        void ReBuildL();

    private:

        /**
        * C++ constructor.
        */
        CSimPhoneNumberIndex( CStoreBase& aParentStore );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // New functions
        /// Builds the index from the contact array
        void ResetAndBuildIndexL();
        /// Appends phone numbers from the contact to the index
        void AppendNumbersL( MVPbkSimContact& aContact );
    private:    // Data
        /// Parent store of the index
        CStoreBase& iParentStore;
        /// Own: a phone number index for fast number matching
        VPbkEngUtils::CPhoneNumberIndex<MVPbkSimContact>* iNumberIndex;
    };

// INLINE FUNCTIONS

inline VPbkEngUtils::CPhoneNumberIndex<MVPbkSimContact>& 
    CSimPhoneNumberIndex::Index() const
    {
    return *iNumberIndex;
    }
} // namespace VPbkSimStore 
#endif      // VPBKSIMSTORE_CSIMPHONENUMBERINDEX_H

            
// End of File
