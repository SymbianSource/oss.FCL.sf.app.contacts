/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A composite command that runs subcommands sequential
*
*/



#ifndef VPBKSIMSTOREIMPL_CSEQUENTIALCOMPOSITECMD_H
#define VPBKSIMSTOREIMPL_CSEQUENTIALCOMPOSITECMD_H

//  INCLUDES
#include "CCompositeCmdBase.h"

// FORWARD DECLARATIONS

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  A composite command that runs subcommands parallel.
*/
NONSHARABLE_CLASS(CSequentialCompositeCmd) : public CCompositeCmdBase
    {
    public: // Data types
        
    public: // Construction and Destruction

        /**
        * Two-phased constructor.
        *
        * @return A new instance of this class
        */
        static CSequentialCompositeCmd* NewL();
        
        /**
        * Two-phased constructor.
        *
        * @return A new instance of this class in CleanupStack
        */
        static CSequentialCompositeCmd* NewLC();
        
        /**
        * Destructor.
        */
        ~CSequentialCompositeCmd();
    
    public: // Functions from base classes
     
        /**
        * From MVPbkSimCommand
        */
        void Execute();
        
    private: // Functions from base classes
        
        /**
        * From MVPbkSimCommandObserver
        */
        void CommandDone( MVPbkSimCommand& aCommand );

    private: // Implementation

        /**
        * C++ constructor.
        */
        CSequentialCompositeCmd();
        
    private:    // Data
        /// Own: A cursor to current subcommand
        TInt iCommandCursor;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CSEQUENTIALCOMPOSITECMD_H
            
// End of File
