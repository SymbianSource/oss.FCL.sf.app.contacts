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
* Description:  A command that uses ETel RMobilePhoneStore GetInfo request
*
*/



#ifndef VPBKSTOREIMPL_CGETINFOCMD_H
#define VPBKSTOREIMPL_CGETINFOCMD_H

//  INCLUDES
#include <MVPbkSimCommand.h>
#include <e32base.h>
#include <etelmm.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CStoreBase;

// CLASS DECLARATION

/**
*  A command that uses ETel RMobilePhoneStore GetInfo request
*
*/
NONSHARABLE_CLASS(CGetInfoCmd) : public CActive,
                                 public MVPbkSimCommand
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore the store for getting information
        * @return a new instance of this class
        */
        static CGetInfoCmd* NewLC( CStoreBase& aStore );
        
        /**
        * Destructor.
        */
        virtual ~CGetInfoCmd();

    public: // Functions from base classes

        /**
        * From CChainCmdBase
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
        
    private:    // Construction

        /**
        * C++ constructor.
        */
        CGetInfoCmd( CStoreBase& aStore );

    private:    // Data
        /// the store for requesting and updating information
        CStoreBase& iStore;
        /// Ref: only one observer supported
        MVPbkSimCommandObserver* iObserver;
    };

} // namespace VPbkSimStoreImpl
#endif      // VPBKSTOREIMPL_CGETINFOCMD_H
            
// End of File
