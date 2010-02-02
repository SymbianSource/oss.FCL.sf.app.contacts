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
* Description:  An observer interface for asynchronous phone requests
*
*/



#ifndef MVPBKSIMPHONEOBSERVER_H
#define MVPBKSIMPHONEOBSERVER_H

//  INCLUDES
#include <e32cmn.h>
#include <e32std.h>


// FORWARD DECLARATIONS
class MVPbkSimPhone;

// CLASS DECLARATION

/**
 * Virtual Phonebook SIM phone observer interface.
 * An observer interface for asynchronous phone requests
 */
class MVPbkSimPhoneObserver
    {
    public: // Data Types
        /**
         * Identifies the source of the error
         */
        enum TErrorIdentifier
            {
            /// Some system error
            ESystem,
            /// SIM card is not inserted 
            ESimCardNotInserted,
            /// Bluetooth SIM access profile active
            EBtSapActive
            };
          
    public: // New functions
        /**
         * Called after phone has been opened successfully
         * @param aPhone the phone that was opened
         */
        virtual void PhoneOpened( MVPbkSimPhone& aPhone ) = 0;
        
        /**
         * Called for asynchronous phone requests in error case
         * @param the phone that failed to open
         * @param aIdentifier the error identifier.
         * @param aError the error code.
         */
        virtual void PhoneError( MVPbkSimPhone& aPhone, 
            TErrorIdentifier aIdentifier, TInt aError ) = 0;

        /**
         * Called after service table is updated.
         * @param aServiceTable Contains flags TVPbkSimPhoneFlags
         */
        virtual void ServiceTableUpdated( TUint32 aServiceTable ) = 0;
        
        /**
         * Called when the status of Fixed Dialling changes in (U)SIM
         * @param aFDNStatus the new status of the FDN, MVPbkSimPhone::TFDNStatus
         */
        virtual void FixedDiallingStatusChanged(
            TInt aFDNStatus ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* SimPhoneObserverExtension( 
                TUid /*aExtensionUid*/ ) { return NULL; }

    protected:  // destructor
        /**
         * Destructor.
         */
        virtual ~MVPbkSimPhoneObserver() {}    

    };

#endif      // MVPBKSIMPHONEOBSERVER_H   
            
// End of File
