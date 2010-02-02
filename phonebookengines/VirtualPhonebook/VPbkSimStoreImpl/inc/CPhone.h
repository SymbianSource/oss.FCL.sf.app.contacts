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
* Description:  Opens ETel RTelServer and RMobilePhone
*
*/



#ifndef VPBKSIMSTOREIMPL_CPHONE_H
#define VPBKSIMSTOREIMPL_CPHONE_H

//  INCLUDES

#include "MVPbkSimCommandObserver.h"
#include "MSimRefreshObject.h"
#include "MBtSapObserver.h"
#include "VPbkSimStoreCommon.h"
#include "MSimStatusObserver.h"
#include <MVPbkSimPhone.h>
#include <MVPbkSimPhoneObserver.h>
#include <e32base.h>
#include <etelmm.h>

// FORWARD DECLARATIONS
class MVPbkSimCntStore;
class CVPbkSimStateInformation;
template<typename MVPbkContactStoreObserver> class CVPbkAsyncObjectOperation;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CSatRefreshNotifier;
class CBtSapNotification;
class TStoreParams;
class CSimStatusNotification;

// CLASS DECLARATION

/**
*  Opens ETel RTelServer and RMobilePhone
*
*/
NONSHARABLE_CLASS(CPhone) : public CActive,
               public MVPbkSimPhone,
               public MSimStatusObserver,
               private MVPbkSimCommandObserver,
               private MSimRefreshObject,
               private MBtSapObserver
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        * @param aSatNotifier the notifer SIM application toolikit
        *        refresh events
        * @param aSimStateInfo offers SIM state information
        */
        CPhone( CSatRefreshNotifier& aSatNotifier,
            CVPbkSimStateInformation& aSimStateInfo,
            CBtSapNotification& aBtSapNotifier );
        
        /**
        * Destructor.
        */
        virtual ~CPhone();

    public: // New functions
        
        /**
        * Returns the ETel phone subsession
        */
        RMobilePhone& ETelPhone();
        
        /**
        * Creates and returns a store
        * @param aParams the store parameters
        * @return a new store instance.
        */
        MVPbkSimCntStore* CreateStoreLC( TStoreParams& aParams );
        
        /**
        * Adds observer. This is useful if the phone is already opened
        */
        void AddObserverL( MVPbkSimPhoneObserver& aObserver );
        
        /**
        * Removes observer. Must be called only if AddObserverL has
        * been called first
        */
        void RemoveObserver( MVPbkSimPhoneObserver& aObserver );

    public: // Functions from base classes
        
        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        void DoCancel();
        
        /**
        * From MVPbkSimPhone
        */
        void OpenL( MVPbkSimPhoneObserver& aObserver );

        /**
        * From MVPbkSimPhone
        */
        void Close( MVPbkSimPhoneObserver& aObserver );

        /**
        * From MVPbkSimPhone
        */
        TBool USimAccessSupported() const;

        /**
        * From MVPbkSimPhone
        */
        TUint32 ServiceTable() const;
        
        /**
        * From MVPbkSimPhone
        */
        TFDNStatus FixedDialingStatus() const;
        
        /**
        * From MVPbkSimPhone
        */
        void BtSapActivated();

        /**
        * From MVPbkSimPhone
        */
        void BtSapDeactivated();
        
        /**
        * From MVPbkSimPhone
        */
        void BtSapNotificationError( TInt aError );

    private:    // Functions from base classes

        /**
        * From MVPbkSimCommandObserver
        */
        void CommandDone( MVPbkSimCommand& aCommand );

        /**
        * From MVPbkSimCommandObserver
        */
        void CommandError( MVPbkSimCommand& aCommand, TInt aError );

        /**
        * From MSimRefreshObject
        */
        void SimRefreshed( TUint32 aSimRefreshFlags,
            MSimRefreshCompletion& aSimRefreshCompletion );

        /**
        * From MSimRefreshObject
        */
        TBool AllowRefresh( TUint32 aSimRefreshFlags );
        
        /**
        * From MSimRefreshObject
        */
        TUint32 ElementaryFiles();
        
    public: // From MSimStatusObserver
            
        void SimStatusOk();

        void SimStatusNotOk();

        void SimStatusUninitialized();
        
        void SimStatusNotificationError( TInt aError );

    private:    // New functions
        
        /// Returns async operation
        CVPbkAsyncObjectOperation<MVPbkSimPhoneObserver>* AsyncOpL();
        /// Starts intializing the phone, return ETrue if started
        TBool StartInitL();
        /// Opens a session to ETel Server and a phone subsession
        void OpenETelPhoneL();
        /// Start listening to BT SAP and SAT
        void ActivateBtSapAndSatRefreshL();
        /// Stop listening to BT SAP and SAT
        void DeActivateBtSapAndSatRefresh();
        /// Handles the BT SAP deactivation event
        void HandleBtSapDeactivationL();
        /// Starts service table command
        void LoadServiceTableL();
        /// Returns ETrue if SIM is usable
        TBool SimUsableL( MVPbkSimPhoneObserver::TErrorIdentifier& aErrorId );
        /// Activates FDN activity notification
        void ActivateFDNNotification();
        /// Tells Sat server that we are ready
        void CompleteSatRefresh();
        void DoOpenL(
                MVPbkSimPhoneObserver& aObserver );
        void DoOpenError(
                MVPbkSimPhoneObserver& aObserver,
                TInt aError );
        void DoOpenWithErrorL(
                MVPbkSimPhoneObserver& aObserver,
                MVPbkSimPhoneObserver::TErrorIdentifier aErrorId );
        

    private:    // Data
        /// Ref: a notifies observers when SIM files are refreshed
        CSatRefreshNotifier& iSatNotifier;
        /// Ref: offers SIM state information
        CVPbkSimStateInformation& iSimStateInfo;
        /// Ref: BT SAP notifier
        CBtSapNotification& iBtSapNotifier;
        /// The ETel server session
        RTelServer iTelServer;
        /// The ETel phone subsession
        RMobilePhone iPhone;
        /// An array for the observers
        RPointerArray<MVPbkSimPhoneObserver> iObservers;
        /// Own: a command for service table
        MVPbkSimCommand* iSTCommand;
        /// A table containing TVPbkSimPhoneFlags
        TUint32 iServiceTable;
        /// Own: a call back for async operations
        CVPbkAsyncObjectOperation<MVPbkSimPhoneObserver>* iAsyncOp;
        /// ETrue if phone has been asynchronously opened
        TBool iIsOpen;
        /// ETel FDN status
        RMobilePhone::TMobilePhoneFdnStatus iFdnStatus;
        /// Ref: the SAT refresh completion
        MSimRefreshCompletion* iSimRefreshCompletion;
        /// Own: the sim status notifier
        CSimStatusNotification* iSimStatus;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CPHONE_H
            
// End of File
