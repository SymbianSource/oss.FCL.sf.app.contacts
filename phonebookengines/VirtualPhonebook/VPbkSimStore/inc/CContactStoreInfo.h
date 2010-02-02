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



#ifndef VPBKSIMSTORE_CCONTACTSTOREINFO_H
#define VPBKSIMSTORE_CCONTACTSTOREINFO_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactStoreInfo.h>

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CContactStore;

// CLASS DECLARATION

/**
*  A store info implementation for the sim stores
*
*/
NONSHARABLE_CLASS( CContactStoreInfo ): 
        public CBase,
        public MVPbkContactStoreInfo
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore the sim store implementation
        * @return a new instance of this class
        */
        static CContactStoreInfo* NewL(CContactStore& aStore);
        
        /**
        * Destructor.
        */
        ~CContactStoreInfo();
    
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
        CContactStoreInfo(CContactStore& aStore);
        
    private:    // Data
        CContactStore& iStore;
    };
} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CCONTACTSTOREINFO_H
            
// End of File
