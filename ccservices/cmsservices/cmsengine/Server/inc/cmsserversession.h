/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       
*
*/


#ifndef __CMSSESSION_H__
#define __CMSSESSION_H__

// FORWARD DECLARATIONS
class CCmsServer;
class CCmsPhoneBookProxy;
class MCmsContactInterface;

/**
 * CCmsServerSession represents a session for a client thread on the
 * server side.
 */ 
NONSHARABLE_CLASS( CCmsServerSession ) : public CSession2
    {
    public:  // Constructors and destructor
        
        /**
        * Creates new server session
        *
        * @param CCmsServer* The main server object
        * @return CCmsSession* New session
        */ 
        static CCmsServerSession* NewL( CCmsServer* aServer );

        /**
        * Service an incoming request
        * @param RMessage2& Kernel message with client's data
        *
        * @return void
        */ 
        void ServiceL( const RMessage2& aMessage );
        
        /**
        * Destructor.
        */      
        ~CCmsServerSession();
    
    public: // Public methods
        
        /**
        * Notifies session that stores open is complete.
        */           
        void StoreOpenComplete();
        
        /**
        * Notifies session that contact retrieving is done.
        * 
        * @param aError Operation completion status
        */
        void CmsSingleContactOperationComplete( TInt aError );
    
    private:
        
        /**
        * Symbian OS second-phase constructor
        */ 
        void ConstructL();

        /**
        * C++ default constructor is private.
        *
        * @param CCmsServer* The main server object
        */      
        CCmsServerSession( CCmsServer* aServer );
        
        /**
        * Panic the client
        *
        * @param TInt Panic code
        */ 
        void PanicClient( TInt aPanic );
        
        /**
        * Creates new CCmsServerContact and fetches contact info
        *
        * @param aMessage Kernel message with client's data
        */
        void CreateContactL( const RMessage2& aMessage );
        
        /**
        * Fetches list of enabled (i.e. with some content) fields
        *
        * @param aMessage Kernel message with client's data
        */
        void EnabledFieldsL( const RMessage2& aMessage );

        /**
        * Fetches contact field
        * 
        * @param aMessage Kernel message with client's data
        */
        void FetchDataL( const RMessage2& aMessage );

        /**
        * Fetches contact identifier (ID)
        *
        * @param aMessage Kernel message with client's data
        */
        void FetchContactIdentifierL( const RMessage2& aMessage );
                        
        /**
        * Cancels ongoing async operation
        *
        * @param aMessage Kernel message with client's data
        */
        void CancelOperation( const RMessage2& aMessage );
        
        /**
        * Activates notifications
        *
        * @param aMessage Kernel message with client's data
        * @param aAskMore If ETrue, returns current presence status imemdiately
        *   from the presence cache and notify later about any changes
        */
        void OrderNotifyL( const RMessage2& aMessage, TBool aAskMore );  
         
        /**
        * Cancels notifications
        *
        * @param aMessage Kernel message with client's data
        */
        void CompleteNotify( const RMessage2& aMessage );
        
        /**
        * Finds services (IM, Voip, etc) availability
        *
        * @param aMessage Kernel message with client's data
        * @return 0 if not available, 1 otherwise
        */
        TInt FindServiceAvailabilityL( const RMessage2& aMessage );
        
        /**
        * Returns current contact's store
        *
        * @param aMessage Kernel message with client's data
        * @return Contact store, see TCmsContactStore from CCmsServerContact
        */
        TInt FindParentStoreL( const RMessage2& aMessage );
        
        /**
        * Finds a contact from xSP store
        *
        * @param aMessage Kernel message with client's data
        */
        void FindXSPContactL( const RMessage2& aMessage );
        
        /**
        * Cancels xSP contact search
        *
        * @param aMessage Kernel message with client's data
        */
        void CancelXSPContactFindL( const RMessage2& aMessage );
        
        /**
        * Fetches contact information
        *
        * @param aMessage Kernel message with client's data
        * @param aContactInterface Observer to be notified about
        *                          operation completion
        */
        void FetchContactL( const RMessage2& aMessage,
                            MCmsContactInterface* aContactInterface );
        
        /**
         * Gets contact action field count.
         */
        TInt GetContacActionFieldCountL( const RMessage2& aMessage );
        
        /**
        * Handles clients requests
        *
        * @param aMessage Kernel message with client's data
        */
        void DoServiceL( const RMessage2& aMessage );

    private: //Data
        
        /// CMS server instance. Not owned.
        CCmsServer*                             iCmsServer;
        
        // CCmsServerContact instance. Owned.
        CCmsServerContact*                      iServerContact;
        
        /// Service message to fetch a contact. Completed when contact 
        /// retrieving operation is done.
        RMessage2                               iCmsContactRetrieveMessage;
        
        /// Copy of the service message received from the client. It's saved for 
        /// delayed processing if stores are not opened yet.
        RMessage2                               iServiceMessage;
        
        /// ETrue, if FeatureManager library was initialized.
        TBool                                   iFeatureManagerInitialized;
    };


#endif

// End of File
