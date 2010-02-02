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
* Description:  A manager class for creating a sim store
*
*/



#ifndef CVPBKSIMSTOREMANAGER_H
#define CVPBKSIMSTOREMANAGER_H

//  INCLUDES
#include <e32base.h>
#include "VPbkSimStoreCommon.h"

// FORWARD DECLARATIONS
class MVPbkSimCntStore;
class MVPbkSimPhone;
class CVPbkSimStateInformation;

namespace VPbkSimStoreImpl {
// FORWARD DECLARATIONS
class CPhone;
class CSatRefreshNotifier;
class CBtSapNotification;
}

// CLASS DECLARATION

/**
*  A manager class for creating a sim store
*
*  @lib VPbkSimStoreImpl.lib
*/
class CVPbkSimStoreManager : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return a new instance of this class
        */
        IMPORT_C static CVPbkSimStoreManager* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CVPbkSimStoreManager();

    public: // New functions
        
        /**
        * Returns the phone
        * @return the phone
        */
        IMPORT_C MVPbkSimPhone& Phone();

        /**
        * Returns the store and creates it if not created.
        * 
        * @param aIdentifier the store identifier
        */
        IMPORT_C MVPbkSimCntStore& StoreL( 
            TVPbkSimStoreIdentifier aIdentifier );
        
    private:

        /**
        * C++ default constructor.
        */
        CVPbkSimStoreManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// Own: the store
        RPointerArray<MVPbkSimCntStore> iStoreArray;
        /// Own: a shared phone used by the stores
        VPbkSimStoreImpl::CPhone* iPhone;
        /// Own: a notifies observers when SIM files are refreshed
        VPbkSimStoreImpl::CSatRefreshNotifier* iSatNotifier;
        /// Own: the SIM state information
        CVPbkSimStateInformation* iSimStateInfo;
        /// Own: Bluetooth SIM access profile notifier
        VPbkSimStoreImpl::CBtSapNotification* iBtSapNotifier;
    };

#endif      // CVPBKSIMSTOREMANAGER_H
            
// End of File
