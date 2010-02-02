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
* Description:  An active object that completes when bluetooth SIM access
*                profile status changes
*
*/


#ifndef VPBKSIMSTOREIMPL_CBTSAPNOTIFICATION_H
#define VPBKSIMSTOREIMPL_CBTSAPNOTIFICATION_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CVPbkSimStateInformation;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class MBtSapObserver;

// CLASS DECLARATION

/**
*  An active object that completes when BT SAP status changes
*
*/
NONSHARABLE_CLASS(CBtSapNotification) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aStore the store to listen to.
        * @return a new instance of this class
        */
        static CBtSapNotification* NewL( 
            CVPbkSimStateInformation& aSimStateInfo );
        
        /**
        * Destructor.
        */
        virtual ~CBtSapNotification();

    public: // New functions
        
        /**
        * Activates this notification.
        */
        void ActivateL();
        
        /**
        * Adds observer for the notification
        * @param aObserver the observer to add
        */
        void AddObserverL( MBtSapObserver& aObserver );
       
        /**
        * Removes the observer
        * @param aObserver the observer to remove
        */
        void RemoveObserver( MBtSapObserver& aObserver );
    
    private:    // New functions
        CBtSapNotification( CVPbkSimStateInformation& aSimStateInfo );
        void ConstructL();
        void DoActivate();
       
    private:  // Functions from base classes
        
        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();
        
        /**
        * From CActive
        */
        TInt RunError( TInt aError );

    private:    // Data
        /// Ref: the SIM state information
        CVPbkSimStateInformation& iSimStateInfo;
        /// Ref: An array of observers
        RPointerArray<MBtSapObserver> iObservers;
        /// The P&S property for BT SAP notification
        RProperty iBtSapProperty;
        /// Own: idle for deactivation event
        CIdle* iDeactivationIdle;
        /// Own: the current state of the BT SAP
        TBool iActive;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CBTSAPNOTIFICATION_H
            
// End of File
