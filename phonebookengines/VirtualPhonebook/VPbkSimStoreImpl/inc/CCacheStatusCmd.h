/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A command that queries the current TSY cache status.
*
*/



#ifndef VPBKSIMSTOREIMPL_CCACHESTATUSCMD_H
#define VPBKSIMSTOREIMPL_CCACHESTATUSCMD_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimCommand.h>
#include <rmmcustomapi.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CCustomStore;

// CLASS DECLARATION

/**
*  A command that queries the current TSY cache status.
*
*/
NONSHARABLE_CLASS(CCacheStatusCmd) : public CActive,
                                     public MVPbkSimCommand
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore a store that supports notification
        * @param aCacheStatus the TSY cache status
        * @return a new instance of this class
        */
        static CCacheStatusCmd* NewLC( CCustomStore& aStore, 
            RMmCustomAPI::TPndCacheStatus& aCacheStatus );
        
        /**
        * Destructor.
        */
        virtual ~CCacheStatusCmd();

    public: // Functions from base classes

        /**
        * From MVPbkSimCommand
        */
        void Execute();

        /**
        * From MVPbkSimCommand
        */
        void AddObserverL( MVPbkSimCommandObserver& aObserver );

        /**
        * From MVPbkSimCommand
        */
        void CancelCmd();
        
    private: // Functions from base classes
        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        void DoCancel();
        
    private:

        /**
        * C++ default constructor.
        */
        CCacheStatusCmd( 
            CCustomStore& aStore,
            RMmCustomAPI::TPndCacheStatus& aCacheStatus );

    private:    // Data
        /// a store that supports notification
        CCustomStore& iStore;
        /// The TSY cache status which is updated if notification completes
        RMmCustomAPI::TPndCacheStatus& iCacheStatus;
        /// A descriptor for the ETel store name
        TName iStoreName;
        /// Ref: only one observer supported
        MVPbkSimCommandObserver* iObserver;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CCACHESTATUSCMD_H
            
// End of File
