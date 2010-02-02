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
* Description:  An operation that deletes a contact from the (U)SIM using ETel
*                RMobilePhoneStore Delete
*
*/



#ifndef VPBKSIMSTOREIMPL_CDELETECMD_H
#define VPBKSIMSTOREIMPL_CDELETECMD_H

//  INCLUDES
#include "METelStoreNotificationObserver.h"
#include <MVPbkSimCommandObserver.h>
#include <MVPbkSimStoreOperation.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkSimContactObserver;
class RVPbkStreamedIntArray;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CStoreBase;

// CLASS DECLARATION

/**
*  An operation that deletes a contact from the (U)SIM.
*
*/
NONSHARABLE_CLASS(CDeleteCmd) : public CActive,
                                public MVPbkSimStoreOperation,
                                private METelStoreNotificationObserver,
                                private MVPbkSimCommandObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aStore the store from which the contact is deleted
        * @param aSimIndex the index of the contact to be deleted
        * @param aObserver an observer to notify after command is done
        * @return a new instance of this class
        */
        static CDeleteCmd* NewL( CStoreBase& aStore, 
                                 RVPbkStreamedIntArray& aIndexArray,
                                 MVPbkSimContactObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CDeleteCmd();

    public: // New functions    
        
        /**
        * Starts this operation
        */
        void Execute();

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
        * From CActive
        */
        TInt RunError( TInt aError );
    
    private:    // Functions from base classes
        
        /**
        * From METelStoreNotificationObserver
        */
        TBool ETelStoreChanged( TInt aSimIndex, TUint32 aEvents );
        
        /**
        * From METelStoreNotificationObserver
        */
        TBool ETelStoreChangeError( TInt aError );
        
        /**
        * From MVPbkSimCommandObserver
        */
        void CommandDone( MVPbkSimCommand& aCommand );

        /**
        * From MVPbkSimCommandObserver
        */
        void CommandError( MVPbkSimCommand& aCommand, TInt aError );
        
    private:    // Construction
    
        /**
        * C++ constructor.
        */
        CDeleteCmd( CStoreBase& aStore, 
                    MVPbkSimContactObserver& aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RVPbkStreamedIntArray& aIndexArray );
    
    private:    // New functions
        /// Starts deleting next contact or completes
        void DeleteNextL();
        /// Starts deleting contact in aSimIndex
        void DeleteNextL( TInt aSimIndex );
        /// Returns the index of next contact to delete
        TInt NextContactIndex();
        /// Completes the active object with aRequestResult
        void CompleteRequest( TInt aRequestResult );
        /// Called to complete this command
        void CompleteCommand();
        /// Handles error case
        void HandleError( TInt aError );
        /// Removes this instance from the ETel notification
        void RemoveNotificationObserver();
        
    private:    // Data
        /// the store from which the contact is deleted
        CStoreBase& iStore;
        /// sim indexes of the contacts to be deleted
        RArray<TInt> iIndexArray;
        /// the sim index that is deleted currently
        TInt iCurSimIndex;
        /// An observer for the error case
        MVPbkSimContactObserver& iObserver;
        /// Own: the update command to execute
        MVPbkSimCommand* iUpdateCommand;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CDELETECMD_H
            
// End of File
