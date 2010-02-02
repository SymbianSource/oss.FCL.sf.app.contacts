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
* Description:  A class for managing find operations
*
*/



#ifndef VPBKSIMSERVER_CFINDMANAGER_H
#define VPBKSIMSERVER_CFINDMANAGER_H

//  INCLUDES
#include "SimServerErrors.h"
#include <e32base.h>
#include <MVPbkSimFindObserver.h>

// FORWARD DECLARATIONS
class MVPbkSimStoreOperation;

namespace VPbkSimServer {

// CLASS DECLARATION

/**
*  A class for managing find operations
*
*/
NONSHARABLE_CLASS(CFindManager) : 
        public CBase,
        public MVPbkSimFindObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return a new instance of this class
        */
        static CFindManager* NewL();

        /**
        * Destructor.
        */
        virtual ~CFindManager();

    public: // New functions
        
        /**
        * Sets the find manager
        * @param aMessage the message to complete after find is ready
        * @param aOperation the find operation that is deleted when find
        *                   completes or when the operation is cancelled.
        *                   !!!Ownership is taken immediately
        * @exception KErrInUse if the observer is in use
        */
        void SetL( const RMessage2& aMessage, 
            MVPbkSimStoreOperation* aOperation );

        /**
        * Cancels message when notification arrives
        */
        void CancelMessage();
        
        /**
        * Returns ETrue if iMessage is still waiting to be completed
        */
        TBool IsActive() const;
        
        /**
        * Panics the client process with aPanicCode
        *
        * @param aPanicCode the panic code to use
        */
        void PanicClient( TClientPanicCode aPanicCode );

    public: // Functions from base classes

        /**
        * From MVPbkSimFindObserver
        */
        void FindCompleted( MVPbkSimCntStore& aStore,
            const RVPbkStreamedIntArray& aSimIndexArray );

        /**
        * From MVPbkSimFindObserver
        */
        void FindError( MVPbkSimCntStore& aStore, TInt aError );
        
    private:    // Construction

        /**
        * C++ default constructor.
        */
        CFindManager();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:  // New functions
        
        /// Handles the completing of find request
        void HandleFindCompleteL( 
            const RVPbkStreamedIntArray& aSimIndexArray );
        /// Completes the find request with given result
        void CompleteRequest( TInt result );
        
    private:    // Data
        /// Own: a buffer for serialized indexes
        HBufC8* iIndexBuffer;
        /// Message that is waiting to be completed
        RMessage2 iMessage;
        /// Own: the store operation for the find
        MVPbkSimStoreOperation* iStoreOperation;
    };

} // namespace VPbkSimServer 

#endif      // VPBKSIMSERVER_CFINDMANAGER_H
            
// End of File
