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
* Description:  A store that uses ETel custom API due to cache in TSY
*
*/



#ifndef VPBKSIMSTOREIMPL_CCUSTOMSTORE_H
#define VPBKSIMSTOREIMPL_CCUSTOMSTORE_H

//  INCLUDES
#include "CBasicStore.h"
#include <rmmcustomapi.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CCompositeCmdBase;

// CLASS DECLARATION

/**
*  A store that uses ETel custom API due to cache in TSY
*
*/
NONSHARABLE_CLASS(CCustomStore) : public CBasicStore
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CCustomStore();

    public: // New functions
        
        /**
        * Returns the custom API subsession
        * @return the custom API subsession
        */
        inline RMmCustomAPI& CustomAPI();

    public: // Functions from base classes

        /**
        * From CStoreBase
        */
        void OpenETelStoreL();

        /**
        * From CStoreBase
        */
        void CloseETelStore();

        /**
        * From CStoreBase
        */
        MVPbkSimCommand* CreateInitCommandL( 
            MVPbkSimCommandObserver& aObserver );

    protected:  // New functions
        
        /**
        * Adds a command that waits TSY cache to be ready.
        * @param aCmdChain the command chain
        */
        void AddTSYCacheReadyCmdToChainL( CCompositeCmdBase& aCmdChain );

    protected:

        /**
        * C++ default constructor.
        * @param aParams the store parameters
        */
        CCustomStore( TStoreParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// ETel custom API subsession
        RMmCustomAPI iCustomAPI;
        /// The ETel/TSY cache status
        RMmCustomAPI::TPndCacheStatus iCacheStatus;
    };

// INLINE FUNCTIONS
inline RMmCustomAPI& CCustomStore::CustomAPI()
    {
    return iCustomAPI;
    }

} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CCUSTOMSTORE_H
            
// End of File
