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
* Description:  A call back for contact operation e.g lock
*
*/



#ifndef VPBKSIMSTORE_CCONTACTOPERATIONCALLBACK_H
#define VPBKSIMSTORE_CCONTACTOPERATIONCALLBACK_H

//  INCLUDES
#include <CVPbkAsyncOperation.h>
#include <MVPbkContactObserver.h>

namespace VPbkSimStore {

// CLASS DECLARATION

/**
*  A call back for contact operation e.g lock
*
*/
NONSHARABLE_CLASS( CContactOperationCallback ): 
        public CBase,
        public VPbkEngUtils::MAsyncCallback
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        CContactOperationCallback( 
            MVPbkContactObserver::TContactOpResult& aOpResult,
            MVPbkContactObserver& aObserver, TInt aResult );

        /**
        * Destructor.
        */
        ~CContactOperationCallback();

    public: // Functions from base classes

        /**
        * From MAsyncCallback.
        */
        void operator()();

        /**
        * From MAsyncCallback.
        */
        void Error( TInt aError );
    
    public:    // Data
        /// Owns the contact in iOpResult until observer takes ownership
        MVPbkContactObserver::TContactOpResult iOpResult;
        MVPbkContactObserver& iObserver;
        TInt iResult;
    };
} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CCONTACTOPERATIONCALLBACK_H
            
// End of File
