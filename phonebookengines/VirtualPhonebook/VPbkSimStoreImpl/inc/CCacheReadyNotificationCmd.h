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
* Description:  A command that completes when TSY cache is ready
*
*/



#ifndef VPBKSIMSTOREIMPL_CCACHEREADYNOTIFICATIONCMD_H
#define VPBKSIMSTOREIMPL_CCACHEREADYNOTIFICATIONCMD_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimCommand.h>
#include <rmmcustomapi.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CCustomStore;
class CCacheStatusCmd;

// CLASS DECLARATION

/**
*  A command that completes when TSY cache is ready.
*/
NONSHARABLE_CLASS(CCacheReadyNotificationCmd) : public CActive,
                                                public MVPbkSimCommand
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore a store that supports notification
        * @param aCacheStatus the TSY cache status
        * @return a new instance of this class
        */
        static CCacheReadyNotificationCmd* NewLC( CCustomStore& aStore,
            RMmCustomAPI::TPndCacheStatus& aCacheStatus,
            CCacheStatusCmd& aCacheStatusCmd );
        
        /**
        * Destructor.
        */
        virtual ~CCacheReadyNotificationCmd();

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
        CCacheReadyNotificationCmd( CCustomStore& aStore, 
            RMmCustomAPI::TPndCacheStatus& aCacheStatus,
            CCacheStatusCmd& aCacheStatusCmd );

    private:    // Data
        /// a store that supports notification
        CCustomStore& iStore;
        /// The name of the phonebook which is becomes ready
        TName iETelStoreName;
        /// The TSY cache status which is updated if notfication completes
        RMmCustomAPI::TPndCacheStatus& iCacheStatus;
        /// Ref: A command that can get current TSY cache state.
        CCacheStatusCmd& iCacheStatusCmd;
        /// Ref: only one observer supported
        MVPbkSimCommandObserver* iObserver;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CCACHEREADYNOTIFICATIONCMD_H
            
// End of File
