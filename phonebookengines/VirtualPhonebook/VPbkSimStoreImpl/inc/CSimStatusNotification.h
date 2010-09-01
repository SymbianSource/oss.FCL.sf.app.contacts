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


#ifndef VPBKSIMSTOREIMPL_CSIMSTATUSNOTIFICATION_H
#define VPBKSIMSTOREIMPL_CSIMSTATUSNOTIFICATION_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CVPbkSimStateInformation;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class MSimStatusObserver;

// CLASS DECLARATION

/**
*  An active object that completes when SIM status changes
*
*/
NONSHARABLE_CLASS(CSimStatusNotification) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aStore the store to listen to.
        * @return a new instance of this class
        */
        static CSimStatusNotification* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSimStatusNotification();

    public: // New functions
    
        /**
         * Returns the current SIM status
         *
         * @return Current SIM status
         */
        TInt CurrentStatus() const;
        
        /**
        * Adds observer for the notification
        * @param aObserver the observer to add
        */
        void AddObserverL( MSimStatusObserver& aObserver );
       
        /**
        * Removes the observer
        * @param aObserver the observer to remove
        */
        void RemoveObserver( MSimStatusObserver& aObserver );
    
    private:    // New functions
        CSimStatusNotification();
        void ConstructL();
        void DoActivate();
        TInt ReadState();
       
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
        /// Own: the current SIM status
        TInt iSimStatus;
        /// Ref: An array of observers
        RPointerArray<MSimStatusObserver> iObservers;
        /// The P&S property for BT SAP notification
        RProperty iSimStatusProperty;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CSIMSTATUSNOTIFICATION_H
            
// End of File
