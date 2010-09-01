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
* Description:  A composite command that runs subcommands parallel.
*
*/



#ifndef VPBKSIMSTOREIMPL_CPARALLELCOMPOSITECMD_H
#define VPBKSIMSTOREIMPL_CPARALLELCOMPOSITECMD_H

//  INCLUDES
#include "CCompositeCmdBase.h"

// FORWARD DECLARATIONS

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  A composite command that runs subcommands parallel.
*/
NONSHARABLE_CLASS(CParallelCompositeCmd) : public CCompositeCmdBase
    {
    public: // Data types
        enum TTerminationPolicy
            {
            /// Composite terminates when the first subcommand is done.
            EFirstSubCommandDone,
            /// Composite terminates when all subcommands are done.
            EAllSubCommandsDone
            };
        
    public: // Construction and Destruction

        /**
        * Two-phased constructor.
        *
        * @param aTerminationPolicy Defines how the composite terminates
        * @return A new instance of this class
        */
        static CParallelCompositeCmd* NewL( 
                TTerminationPolicy aTerminationPolicy );
        
        /**
        * Two-phased constructor.
        *
        * @param aTerminationPolicy Defines how the composite terminates
        * @return A new instance of this class in CleanupStack
        */
        static CParallelCompositeCmd* NewLC( 
                TTerminationPolicy aTerminationPolicy );
        
        /**
        * Destructor.
        */
        ~CParallelCompositeCmd();
    
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
        CParallelCompositeCmd( TTerminationPolicy aTerminationPolicy );
        
    private:    // Data
        /// Own: Defines when the composite terminates
        TTerminationPolicy iTerminationPolicy;
        /// Own: A counter for terminated subcommands.
        TInt iTerminatedCommands;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CPARALLELCOMPOSITECMD_H
            
// End of File
