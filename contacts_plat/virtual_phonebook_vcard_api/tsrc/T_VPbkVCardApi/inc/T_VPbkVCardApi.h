/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  STIF test module for testing VirtualPhonebook VCard API
*
*/



#ifndef T_VPBKVCARDAPI_H
#define T_VPBKVCARDAPI_H

#include <e32def.h>
#include <s32file.h>
#include <eikenv.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <mvpbkcontactstoreListobserver.h>
#include <mvpbkcontactobserver.h>
#include <mvpbksinglecontactoperationobserver.h>
#include <mvpbkcontactcopyobserver.h>
#include <MVPbkContactOperationBase.h>
// DEFINES
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// FORWARD DECLARATIONS
class CVPbkVCardEng;
class CVPbkContactManager;
class MVPbkContactStore;

NONSHARABLE_CLASS(CT_VPbkVCardApi) : 
				public CScriptBase,
		          public MVPbkContactStoreListObserver,
		          public MVPbkContactObserver,
		          public MVPbkSingleContactOperationObserver,
		          public MVPbkContactCopyObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_VPbkVCardApi* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CT_VPbkVCardApi();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
        // From MVPbkContactStoreListObserver

    	void StoreReady( MVPbkContactStore& aContactStore );
        void StoreUnavailable( MVPbkContactStore& aContactStore,
            TInt aReason );
        void HandleStoreEventL( MVPbkContactStore& aContactStore, 
            TVPbkContactStoreEvent aStoreEvent );
        void OpenComplete();

        // From MVPbkContactObserver

        void ContactOperationCompleted( TContactOpResult aResult );
        void ContactOperationFailed( TContactOp aOpCode, TInt aErrorCode,
            TBool aErrorNotified );

        // From MVPbkSingleContactOperationObserver

        void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, TInt aError );

        // From MVPbkContactCopyObserver

        void FieldAddedToContact( MVPbkContactOperationBase& aOperation );
        void FieldAddingFailed( MVPbkContactOperationBase& aOperation, 
            TInt aError );
        void ContactsSaved( MVPbkContactOperationBase& aOperation,
            MVPbkContactLinkArray* aResults );
        void ContactsSavingFailed( MVPbkContactOperationBase& aOperation,
            TInt aError );

    private:

        /**
        * C++ default constructor.
        */
    	CT_VPbkVCardApi( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Test method for creating VCard engine
        * @return Symbian OS error code.
        */
        virtual TInt CreateVCardEng();

        /**
        * Test method for deleting VCard engine
        * @return Symbian OS error code.
        */
        virtual TInt DeleteVCardEng();

        /**
        * Test method for importing VCard into store
        * @return Symbian OS error code.
        */
        virtual TInt ImportVCardToStore();

        /**
        * Test method for importing VCard into contacts
        * @return Symbian OS error code.
        */
        virtual TInt ImportVCardToContacts();
        
        /**
        * Test method for importing Business card into store
        * @return Symbian OS error code.
        */
        virtual TInt ImportBusinessCardToStore();
        
        /**
        * Test method for importing Business card into contacts
        * @return Symbian OS error code.
        */
        virtual TInt ImportBusinessCardToContacts();

        /**
        * Tests 
        * @return Symbian OS error code.
        */
        virtual TInt SupportsFieldType();


        void CreateVCardEngL();
        void ImportVCardToStoreL();
        void ImportVCardToContactsL();
        void ImportBusinessCardToStoreL();
        void ImportBusinessCardToContactsL();
        
        void Delete();
        
        CVPbkVCardEng*          iEngine;
        CVPbkContactManager*	iContactManager;
        RFs                     iFs;
        RFileReadStream         iSource;
        MVPbkContactStore*      iContactStore;
        RPointerArray< MVPbkStoreContact > iImportedContacts;
        CEikonEnv* 				iEikEnv;
        MVPbkContactOperationBase* ioperation;
    };

#endif      // T_VPBKVCARDAPI_H

// End of File
