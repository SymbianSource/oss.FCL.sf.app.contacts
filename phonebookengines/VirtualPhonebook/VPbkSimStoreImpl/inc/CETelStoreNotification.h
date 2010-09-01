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
* Description:  An active object that completes when there is a change in
*                ETel/TSY store
*
*/


#ifndef VPBKSIMSTOREIMPL_CETELSTORENOTIFICATION_H
#define VPBKSIMSTOREIMPL_CETELSTORENOTIFICATION_H

//  INCLUDES
#include <e32base.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CStoreBase;
class METelStoreNotificationObserver;

// CLASS DECLARATION

/**
*  An active object that completes when there is a change in ETel/TSY store
*
*/
NONSHARABLE_CLASS(CETelStoreNotification) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        * @param aStore the store to listen to.
        */
        CETelStoreNotification( CStoreBase& aStore );

        /**
        * Destructor.
        */
        virtual ~CETelStoreNotification();

    public: // New functions
        
        /**
        * Activates this notification. Call ETel NotifyStoreEvent.
        */
        void Activate();
        
        /**
        * Sets the high priority observer
        * @param aHighPriorityObserver the observer to set
        */
        void SetHighPriorityObserver(
            METelStoreNotificationObserver& aHighPriorityObserver );
       
        /**
        * Removes the observer
        * @param aHighPriorityObserver the observer to remove
        */
        void RemoveHighPriorityObserver(
            METelStoreNotificationObserver& aHighPriorityObserver);
    
    private:    // New functions
        TBool NotifyObserver( METelStoreNotificationObserver* aObserver,
            TInt aResult );
            
    private:  // Functions from base classes
        
        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    private:    // Data
        // The index of the entry that has been changed
        TInt iIndex;
        // The events that ETel sends
        TUint32 iEvents;
        /// The store to listen to
        CStoreBase& iStore;
        /// Ref: the observer that is called first
        METelStoreNotificationObserver* iHighPriorityObserver;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CETELSTORENOTIFICATION_H
            
// End of File
