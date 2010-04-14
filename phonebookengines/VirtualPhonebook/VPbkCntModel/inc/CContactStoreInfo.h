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



#ifndef VPBKCNTMODEL_CCONTACTSTOREINFO_H
#define VPBKCNTMODEL_CCONTACTSTOREINFO_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactStoreInfo.h>

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;
class CContactStoreDomain;

// CLASS DECLARATION

/**
*  A store info implementation for the contact model
*
*/
NONSHARABLE_CLASS( CContactStoreInfo ): public CBase,
                                        public MVPbkContactStoreInfo
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore the contact model store
        * @param aDomain the contact model store domain
        * @return a new instance of this class
        */
        static CContactStoreInfo* NewL(CContactStore& aStore,
            CContactStoreDomain& aDomain);
        
        /**
        * Destructor.
        */
        ~CContactStoreInfo();

    public: // New functions
        
    public: // Functions from base classes
        
        /**
        * From MVPbkContactStoreInfo
        */
        TInt MaxNumberOfContactsL() const;
        
        /**
        * From MVPbkContactStoreInfo
        */
        TInt NumberOfContactsL() const;
        
        /**
        * From MVPbkContactStoreInfo
        */
        TInt64 ReservedMemoryL() const;
        
        /**
        * From MVPbkContactStoreInfo
        */
        TInt64 FreeMemoryL() const;
        
        /**
        * From MVPbkContactStoreInfo
        */
        TInt MaxNumberOfGroupsL() const;
        
        /**
        * From MVPbkContactStoreInfo
        */
        TInt NumberOfGroupsL() const;
        
    private: // Construction

        /**
        * C++ default constructor.
        */
        CContactStoreInfo(CContactStore& aStore, CContactStoreDomain& aDomain);

    private:    // Data
        CContactStore& iStore;
        CContactStoreDomain& iDomain;
        mutable TBool iGroupsFetched;
    };
} // namespace VPbkCntModel
#endif      // VPBKCNTMODEL_CCONTACTSTOREINFO_H
            
// End of File
