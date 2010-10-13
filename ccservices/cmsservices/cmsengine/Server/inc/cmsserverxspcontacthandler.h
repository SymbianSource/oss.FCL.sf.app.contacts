/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CMSSERVERXSPCONTACTHANDLER__
#define __CMSSERVERXSPCONTACTHANDLER__

// INCLUDES
#include <e32base.h>
#include "cmscontact.h"
#include <VPbkEng.rsg>
#include <badesca.h>
#include <MVPbkBatchOperationObserver.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <CVPbkContactLinkArray.h>

class MVPbkContactLink;
class MVPbkStoreContact;
class CCmsServerContact;
class CCmsPhoneBookProxy;
class CVPbkContactManager;
class MVPbkContactLinkArray;
class CCmsServerAsyncContact;
class CCmsServerContactAttributes;
class MPresenceTrafficLights;

// CLASS DECLARATION
NONSHARABLE_CLASS( CCmsServerXSPContactHandler ) : public CBase,
                       public MVPbkContactFindFromStoresObserver,
                       public MVPbkSingleContactOperationObserver
    {
    public:  
        
        /**
        * Start the server thread
        * @return TInt Error code
        */
        static CCmsServerXSPContactHandler* NewL( CCmsServerContact& aContact,
                                                  CCmsServer& aCmsServer );
        
        /**
        * Start the server thread
        * @return TInt Error code
        */
        void SaveMessage( const RMessage2& aMessage );
        
        /**
        * Start the server thread
        * @return TInt Error code
        */
        TInt ContactCount() const;
        
        /**
        * Start the server thread
        * @return TInt Error code
        */
        MVPbkStoreContact* Contact( TInt aIndex ) const;
        
        /**
        * Start the server thread
        * @return TInt Error code
        */
        void Cancel();
        
        /**
        * Start the server thread
        * @return TInt Error code
        */
        virtual ~CCmsServerXSPContactHandler();
        
        void NumberOfFinds( TInt aCount );
        void NumberOfStoresInFind( TInt aStoresNumber );
        
    private:
        
        /**
        * Start the server thread
        * @return TInt Error code
        */
        CCmsServerXSPContactHandler( CCmsServerContact& aContact,
                                     CCmsServer& aCmsServer );
    
    private:  //From MVPbkContactFindFromStoresObserver
        
        /**
        * This is called when one store completes its find. If this
        * leaves then FindFromStoreFailed is called. Client takes 
        * the ownership of all results immediately!
        *
        * FindFromStoresOperationComplete is called after
        * all stores have called either this or FindFromStoreFailed.
        *
        * @param aStore    The store whose find results are available
        * @param aResultsFromStore The find results from the store.
        *                          Client takes the ownership immediately!
        */
        void FindFromStoreSucceededL( MVPbkContactStore& aStore, MVPbkContactLinkArray* aResultsFromStore );

        /**
        * Called when a find operation of one store fails. Doesn't stop
        * the find from other stores in the case there are multiple stores.
        *
        * @param aStore    The failed store.
        * @param aError    A system wide error code from the 
        *                  store implementation.
        */
        void FindFromStoreFailed( MVPbkContactStore& aStore, TInt aError );

        /**
        * This is called after all stores have called either 
        * FindFromStoreSucceed or FindFromStoreFailed. It's called 
        * also when there are no stores that offer find operation.
        */
        void FindFromStoresOperationComplete();

    private:  //From MVPbkSingleContactOperationObserver

         /**
         * Called when operation is completed.
         *
         * @param aOperation the completed operation.
         * @param aContact  the contact returned by the operation.
         *                  Client must take the ownership immediately.
         *
         *                  !!! NOTICE !!!
         *                  If you use Cleanupstack for MVPbkStoreContact
         *                  Use MVPbkStoreContact::PushL or
         *                  CleanupDeletePushL from e32base.h.
         *                  (Do Not Use CleanupStack::PushL(TAny*) because
         *                  then the virtual destructor of the M-class
         *                  won't be called when the object is deleted).
         */
        void VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation,
                                                 MVPbkStoreContact* aContact );

         /**
         * Called if the operation fails.
         *
         * @param aOperation    the failed operation.
         * @param aError        error code of the failure.
         */
        void VPbkSingleContactOperationFailed( MVPbkContactOperationBase& aOperation, TInt aError );
        
    private:  //Data
        
        TInt                                iCompletionCode;
        TBool                               iActive;
        RMessage2                           iMessage;
        CCmsServerContact&                  iServerContact;
        RPointerArray<MVPbkStoreContact>    iContactArray;
        TInt iNumberOfFinds;
        TInt iNumberOfCompletedFinds;
        CVPbkContactLinkArray* iContactLinks;
        TInt iNumberOfStoresInFind;
        TInt iNumberOfCompletedStoresInFind;
        CCmsServer& iCmsServer;
    };

#endif  //__CMSSERVERXSPCONTACTHANDLER__


// End of File
