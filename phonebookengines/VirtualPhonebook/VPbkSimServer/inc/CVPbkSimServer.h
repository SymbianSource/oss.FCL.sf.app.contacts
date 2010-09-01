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
* Description:  The main class of the server.
*
*/



#ifndef CVPBKSIMSERVER_H
#define CVPBKSIMSERVER_H

//  INCLUDES
#include <e32base.h>
#include <VPbkSimStoreCommon.h>

// FORWARD DECLARATIONS
class CVPbkSimStoreManager;
class CVPbkSimCommandStore;
class MVPbkSimCommand;
class MVPbkSimCntStore;
class MVPbkSimPhone;

// CLASS DECLARATION

/**
 * The main class of the server.
 *
 */
NONSHARABLE_CLASS(CVPbkSimServer) : 
        public CPolicyServer
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVPbkSimServer* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CVPbkSimServer();

    public: // New functions
        
        /**
        * Creates a new object container from the container index.
        * Use RemoveContainer() to delete it.
        * @return a reference to the new container
        */
        CObjectCon* NewContainerL();

        /**
        * Removes the container from the index.
        * @param aContainer the container that is removed.
        */
        void RemoveContainer( CObjectCon& aContainer );
        
        /**
        * Returns the store manager
        * @return the store manager
        */
        inline CVPbkSimStoreManager& StoreManager();
        
        /**
        * Appends a delete/save command to the execution queue.
        * One command runs at a time.
        * @param aCommand a new command. Ownership is taken immediately.
        */
        void AppendModificationCmdL( MVPbkSimCommand* aCommand );

        /**
        * Cancels the command.
        * @param aCommand the command to cancel
        */
        void CancelCommand( MVPbkSimCommand& aCommand );

        /**
        * Increases the session count in the server
        */
        void IncreaseNumberOfSessions();

        /**
        * Decreases the session count and start asynchronous
        * shutdown if no sessions left
        */
        void DecreaseNumberOfSessions();

    public: // Functions from base classes

        /**
        * From CServer2
        */
        CSession2* NewSessionL( const TVersion& aVersion, 
            const RMessage2& aMessage ) const;

        /**
        * From CServer2
        */                    
        TCustomResult CustomSecurityCheckL( const RMessage2& aMsg, 
                                            TInt& aAction, 
                                            TSecurityInfo& aMissing );
        
    private: // Construction

        /**
        * C++ default constructor.
        */
        CVPbkSimServer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
     
    private:    // Data
        /// An index for object containers of the sessions
        CObjectConIx* iObjectContainerIndex;
        /// Own: store manager owns sim stores
        CVPbkSimStoreManager* iStoreManager;
        /// Own: a shared phone for the stores
        MVPbkSimPhone* iPhone;
        /// Own: a command store for the delete/save commands
        CVPbkSimCommandStore* iModificationCmdStore;
        /// Asynchronous shutdown of the server
        CPeriodic* iAsyncShutDown;
        /// number of sessions
        TInt iNumOfSessions;
    };

// INLINE FUNCTIONS
inline CVPbkSimStoreManager& CVPbkSimServer::StoreManager()
    {
    return *iStoreManager;
    }

#endif      // CVPBKSIMSERVER_H
            
// End of File
