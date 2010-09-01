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
* Description:  A command that queries (U)SIM service table and checks
*                if the store is supported. Updates the data in CVPbkPhone
*                so this command is needs to run only once.
*
*/



#ifndef VPBKSIMSTOREIMPL_CSERVICETABLECMD_H
#define VPBKSIMSTOREIMPL_CSERVICETABLECMD_H

//  INCLUDES
#include <MVPbkSimCommand.h>
#include <e32base.h>
#include <etelmm.h>
#include "VPbkSimStoreCommon.h"

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CPhone;

// CLASS DECLARATION

/**
*  A command that queries (U)SIM service table
*
*/
NONSHARABLE_CLASS(CServiceTableCmd) : public CActive,
                                      public MVPbkSimCommand
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aPhone the phone for making the request
        * @param aServiceTable a table to update with TVPbkSimPhoneFlags
        * @return a new instance of this class
        */
        static CServiceTableCmd* NewLC( CPhone& aPhone, 
                TUint32& aServiceTable );

        /**
        * Destructor.
        */
        virtual ~CServiceTableCmd();

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
        
    private: // From CActive
        void RunL();                
        void DoCancel();
        TInt RunError( TInt aError );
        
        
    private:    // Construction

        /**
        * C++ constructor.
        */
        CServiceTableCmd( CPhone& aPhone, TUint32& aServiceTable );

    private:    // Data
        /// The phone for requesting servicetable
        CPhone& iPhone;
        /// The identifier of the store
        TUint32& iVPbkServiceTable;
        /// ETrue if USIM supported
        TBool iUSimSupported;
        /// The (U)SIM service table
        RMobilePhone::TMobilePhoneServiceTableV1 iServiceTable;
        // A package for service table
        RMobilePhone::TMobilePhoneServiceTableV1Pckg iServiceTablePckg;
        /// Ref: only one observer supported
        MVPbkSimCommandObserver* iObserver;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CSERVICETABLECMD_H
            
// End of File
