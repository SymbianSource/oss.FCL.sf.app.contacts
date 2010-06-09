/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMS Server
*       
*
*/


#ifndef __CMSSERVER_H__
#define __CMSSERVER_H__

// INCLUDES
#include <f32file.h>
#include "cmscommondefines.h"
#include "MCmsPhonebookOperationsObserver.h"

//FORWARD DECLARATIONS
class CCmsPhonebookProxy;
class MPresenceTrafficLights;
class MVPbkContactLink;
class CCmsServerSession;
class CCmsServerContact;
class CCmsPhonebookProxy;
class CRCSEProfileRegistry;

/** 
 * CCmsScheduler is used instead of default active scheduler to
 * make customized handling of leaves in RunL methods of active 
 * objects.
 */
class CCmsScheduler : public CActiveScheduler
    {
    public:  // from base class

        /**
        * Error handling that ignores leaves of
        * active object RunL methods
        */
        void Error( TInt aError ) const;
    };

/**
 * CCmsServer class is responsible for starting the server thread and creating
 * sessions for its clients. 
 */ 
NONSHARABLE_CLASS( CCmsServer ) : public CServer2,
                                  public MCmsPhonebookOperationsObserver                        
    {
    public: // Constructors and destructor
        
        /**
        * Start the server thread
        *
        * @return Error code
        */
        static TInt StartThreadL();

        /**
        * Create a new session
        *
        * @param TVersion The version of the client interface
        * @param RMessage2& Kernel message
        * @return The new session
        */
        CSession2* NewSessionL( const TVersion& aVersion,
        						const RMessage2& /*aMessage*/ ) const;
        
        /**
        * Destructor
        */
        ~CCmsServer();

    private: // Constructors
        
        /**
        * Create the main server object
        *
        * @return Main server object
        */
        static CCmsServer* NewLC();
        
        /**
        * Private default constructor
        * 
        * @param aPriority Active object priority
        */
        CCmsServer( TInt aPriority );
        
        /**
        * The Symbian OS second-phase constructor
        */
        void ConstructL();
    
    public:
        
        /**
        * Return a reference to phonebook proxy
        * 
        * @return Reference to phonebook proxy
        */
        CCmsPhonebookProxy& PhonebookProxyHandle();

        /**
        * Return a reference to file session handle
        *
        * @return File session handle
        */
        RFs& FileSession();
        
        /**
         * Return a reference to RCSE Profile registry.
         * Uses lazy initialization
         * 
         * @return RCSE Profile Registry instance 
         */
        CRCSEProfileRegistry& RCSEProfileRegistryL();
        
    public:  //static
        
        /**
        * Panic the server
        *
        * @param aPanic Panic code
        */
        static void PanicServer( TCmsServerPanic aPanic );
            
    public:  // From MCmsPhonebookOperationsObserver
    
        void StoreOpenComplete();  
        void CmsSingleContactOperationComplete( TInt aError );
    
    private:            // Data
        
        /// Global handler to file server session, available for all
        /// CMS sessions. Owned.
        RFs                                     iFS;

        /// Phonebook proxy which is responsible for common phonebook 
        /// operations. Available for all CMS sessions. Owned.
        CCmsPhonebookProxy*                     iPhonebookProxy;
        
        /// Owned. Shared RCSE Profile registry instance for all sessions.
        /// Constructing this instance is slow and therefore it's kept
        /// in server where it can be quickly aquired.
        CRCSEProfileRegistry*                   iRCSEProfileRegistry;
    };

#endif

// End of File
