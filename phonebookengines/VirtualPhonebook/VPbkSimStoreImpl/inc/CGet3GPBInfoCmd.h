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
* Description:  A command that queries 3G ADN store information from ETel 
*                custom API
*
*/



#ifndef VPBKSIMSTOREIMPL_CGET3GPBINFOCMD_H
#define VPBKSIMSTOREIMPL_CGET3GPBINFOCMD_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimCommand.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CUsimAdnStore;

// CLASS DECLARATION

/**
*  A command that queries 3G ADN store information from ETel custom API
*/
NONSHARABLE_CLASS(CGet3GPBInfoCmd) : public CActive,
                                     public MVPbkSimCommand
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aStore the store for making request and updating info.
        * @return a new instance of this class
        */
        static CGet3GPBInfoCmd* NewLC( CUsimAdnStore& aStore );
        
        /**
        * Destructor.
        */
        virtual ~CGet3GPBInfoCmd();

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
        
    private: // Functions from base classes
        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        void DoCancel();
        
    private:

        /**
        * C++ default constructor.
        */
        CGet3GPBInfoCmd( CUsimAdnStore& aStore );

    private:    // Data
        /// the store for making request and updating info.
        CUsimAdnStore& iStore;
        /// Ref: only one observer supported
        MVPbkSimCommandObserver* iObserver;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CGET3GPBINFOCMD_H
            
// End of File
