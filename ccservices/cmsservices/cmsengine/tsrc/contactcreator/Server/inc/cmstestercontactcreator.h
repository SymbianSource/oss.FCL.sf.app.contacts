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


#ifndef __CMSTESTERCONTACTCREATOR__
#define __CMSTESTERCONTACTCREATOR__

// INCLUDES
#include <barsc.h>
#include <e32base.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkSingleContactOperationObserver.h>

// Consts
_LIT( KCmsXSPStore,                             "c:cmsteststore.cdb" );
_LIT( KCmsCreatorDbUri,                         "cntdb://c:contacts.cdb" );
_LIT( KCmsCreatorSimDbUri,                      "sim://global_onb" );
_LIT( KCmsContactStore,                         "cntdb://c:cmsteststore.cdb" );

_LIT( KVpbkResourcePath,                        "\\resource\\VPbkEng.rsc" );
_LIT( KCreatorResourcePath,                     "\\resource\\creatorserver.rsc" );

// Forward declarations
class RFs;
class CVPbkSortOrder;
class TResourceReader;
class MVPbkContactLink;
class MVPbkContactStore;
class MVPbkStoreContact;     
class CVPbkContactManager;
class MVPbkContactViewBase;
class MVPbkContactLinkArray;
class MVPbkContactStoreList;
class MVPbkContactOperationBase;
class CVPbkContactViewDefinition;

// CLASS DECLARATION
NONSHARABLE_CLASS( CCmsTesterContactCreator ) : public CBase,
                               public MVPbkContactStoreObserver,
                               public MVPbkContactStoreListObserver,
                               public MVPbkSingleContactOperationObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS Constructor
        * @return CCmsTesterContactCreator* 
        */
        static CCmsTesterContactCreator* NewL( RFs& aFileSession, const TDesC& aStoreUri );

        /**
        * Initialise contact stores.
        * Must be called before doing anything else
        *
        * @param TRequestStatus& Client's request status
        * @return void 
        */
        void InitStoresL( MVPbkContactViewObserver& aViewObserver );

        MVPbkContactStore& ContactStore() const;

        CVPbkContactManager& ContactManager() const;

        
       
        /**
        * Destructor
        */
        virtual ~CCmsTesterContactCreator();
    
    private:

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

    private:  //From MVPbkContactStoreListObserver

        /**
        * Called when the opening process is complete, ie. all stores have been reported
        * either failed or successful open.
        */
        void OpenComplete();
        
    private:  //From MVPbkContactStoreObserver

        /**
        * Called when a contact store is ready to use.
        */
        void StoreReady( MVPbkContactStore& aContactStore );

        /**
        * Called when a contact store becomes unavailable.
        * Client may inspect the reason of the unavailability and decide whether or not
        * it will keep the store opened (ie. listen to the store events).
        * @param aContactStore The store that became unavailable.
        * @param aReason The reason why the store is unavailable.
        *                This is one of the system wide error codes.
        */
        void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );

        /**
         * Called when changes occur in the contact store.
         * @see TVPbkContactStoreEvent
         * @param aContactStore The store the event occurred in.
         * @param aStoreEvent   Event that has occured.
         */
        void HandleStoreEventL( MVPbkContactStore& aContactStore, TVPbkContactStoreEvent aStoreEvent );

    private:
        
        /**
        * C++ constructor
        * @return CXmlFormatter
        */
        CCmsTesterContactCreator( RFs& aFileSession );
        
        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        void ConstructL( const TDesC& aStoreUri );
        
        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        void SetupContactViewL();
        
        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        CVPbkSortOrder* CreateSortOrderLC( TInt aResourceId );
       
        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        void OpenResourceFileL( RResourceFile& aResFile, const TDesC& aResourceFile );
        
        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        void CreateResourceReaderLC( TResourceReader& aReader, TInt aResourceId );

        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        void ResetData();

        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        void ResetData( MVPbkContactOperationBase& aOperation );
        /**
        * Symbian OS second-phase constructor
        * @return void
        */
        CVPbkContactViewDefinition* CreateViewDefLC( TInt aResourceId );
        
    private: //Data
        
        enum TSelectorState
            {
            ESelectorIdle = 0,
            ESelectorInit,
            ESelectorFetchContact
            };

        RFs&                                iFileSession;
        TBool                               iStoreReady;
        HBufC*                              iStoreUri;
        RResourceFile                       iCmsResourceFile;
        RResourceFile                       iVpbkResourceFile;
        TSelectorState                      iSelectorState;
        TRequestStatus*                     iClientStatus;
        MVPbkContactLink*                   iContactLink;
        MVPbkContactViewBase*               iContactView;
        MVPbkContactStoreList*              iStoreList;
        MVPbkContactStore*                  iContactStore;
        CVPbkContactManager*                iContactManager; 
        MVPbkContactLinkArray*              iLinkArray;
        MVPbkContactViewObserver*           iViewObserver;
        MVPbkContactOperationBase*          iOperation;
       
    };

#endif  //__CMSTESTERCONTACTCREATOR__


// End of File
