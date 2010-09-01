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


#ifndef __CREATORSERVER_H__
#define __CREATORSERVER_H__

// INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactAttributeManager.h>
#include "creatordefines.h"

_LIT( KCmsCreatorLogDir,                            "TestFramework\\cms" );
_LIT( KCmsCreatorLogFile,                           "cmstester.txt" );
const TInt KLogBufferMaxSize                        = 2000;
const TUint KAppDefaultHeapSize                     = 0x10000;

//FORWARD DECLARATIONs
class MVPbkFieldType;
class MVPbkContactViewBase;
class MVPbkStoreContactField;
class CCreatorServerAttribute;
class CCmsTesterContactCreator;

// CLASS DECLARATION
NONSHARABLE_CLASS( CCreatorServer ) : public CBase,
                                      public MVPbkContactObserver,
                                      public MVPbkContactViewObserver,
                                      public MVPbkSetAttributeObserver
    {
    public:             // Constructors and destructor
        
        /**
        * Start the server thread
        * @return TInt Error code
        */
        static TInt StartThread();

        /**
        * Create the main server object
        * @return CServer2* Main server object
        */
        static CCreatorServer* NewL( const TDesC& aStoreUri );
        
        /**
        * Create the main server object
        * @return CServer2* Main server object
        */
        static TInt CreateXSPStoreL();
        
        /**
        * Create the main server object
        * @return CServer2* Main server object
        */
        static void WriteSPSettingsL();
        
        /**
        * Create the main server object
        * @return CServer2* Main server object
        */
        void CreateContactsL();
        
        /**
        * Destructor
        */
        virtual ~CCreatorServer();

    public:

        /**
        * Return a file session handle
        * @return RFs& File session
        */
        RFs& FileSession();

    public:  //static

        /**
        * Return a file session handle
        * @return RFs& File session
        */
        static void WriteToLog( TRefByValue<const TDesC8> aFmt,... );

    private:  //From MVPbkContactViewObserver

        /**
        * Called when a view is ready for use. This function
        * may also be called if view is already ready. Then
        * it means that the view has somehow refreshed
        * and the observer may have to take this into account.
        *
        * @param aView     Contact view sending the event.
        */
        void ContactViewReady( MVPbkContactViewBase& aView );

        /**
        * Called when a view is unavailable for a while. When the view is
        * again available it will call ContactViewReady(). The
        * contents of the view may change completely while it is
        * unavailable.
        *
        * @param aView     Contact view sending the event.
        */
        void ContactViewUnavailable( MVPbkContactViewBase& aView );

        /**
        * Called when a contact has been added to the view.
        *
        * @param aView         Contact view sending the event.
        * @param aIndex        Index where the contact was added
        *                      in the view.
        * @param aContactLink  Link to the added contact.
        *                      Valid only during this call.
        */
        void ContactAddedToView( MVPbkContactViewBase& aView, TInt aIndex, 
                                 const MVPbkContactLink& aContactLink );

        /**
        * Called when a contact has been removed from a view.
        *
        * @param aView         Contact view sending the event.
        * @param aIndex        Index where the contact was removed
        *                      from the view.
        * @param aContactLink  Link to the removed contact.
        *                      Valid only during this call.
        */
        void ContactRemovedFromView( MVPbkContactViewBase& aView, TInt aIndex, 
                                     const MVPbkContactLink& aContactLink );

        /**
        * Called when an error occurs in the view.
        * If client decides to destroy the view then it should do
        * it asynchronously because the view can access member data
        * after a call to this.
        *
        * @param aView             Contact view sending the event.
        * @param aError            Error code of the failure.
        * @param aErrorNotified    ETrue if the implementation has already 
        *                          notified user about the error using e.g
        *                          an ECOM plug-in,
        *                          EFalse otherwise.
        */
        void ContactViewError( MVPbkContactViewBase& aView, TInt aError, TBool aErrorNotified );

    private:  //From MVPbkContactObserver
        
        /**
        * Called when a contact operation has succesfully completed.
        *
        * NOTE: If you use Cleanupstack for MVPbkStoreContact use 
        * MVPbkStoreContact::PushL or CleanupDeletePushL from e32base.h.
        * (Do Not Use CleanupStack::PushL(TAny*) because then the virtual 
        * destructor of the M-class won't be called when the object 
        * is deleted).
        *
        * @param aResult   The result of the operation. The client takes
        *                  the ownership of the iStoreContact immediately
        *                  if set in aResult.
        */
        void ContactOperationCompleted(TContactOpResult aResult);

        /**
        * Called when a contact operation has failed.
        *
        * @param aOpCode           The operation that failed.
        * @param aErrorCode        System error code of the failure.
        *							KErrAccessDenied when EContactCommit 
        *							means that contact hasn't been locked.
        * @param aErrorNotified    ETrue if the implementation has already
        *                          notified user about the error, 
        *                          EFalse otherwise.
        */
        void ContactOperationFailed (TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);

    private:  //From MVPbkSetAttributeObserver

        /**
        * Attribute operation completed.
        * @param aOperation    Contact operation that was completed.
        */
        void AttributeOperationComplete( MVPbkContactOperationBase& aOperation );

        /**
        * Attribute operation failed.
        * @param aOperation    Contact operation that failed.
        * @param aError        Error code.
        */
        void AttributeOperationFailed( MVPbkContactOperationBase& aOperation, TInt aError );

    private:
        
        /**
        * C++ constructor is private
        * @return CCmsServer
        */
        CCreatorServer();
        
        /**
        * The Symbian OS second-phase constructor
        * @return void
        */
        void ConstructL( const TDesC& aStoreUri );
        
        /**
        * The Symbian OS second-phase constructor
        * @return void
        */
        HBufC8* BinaryDataLC( const TDesC& aFilePath );
        
        /**
        * The Symbian OS second-phase constructor
        * @return void
        */
        void CreateXSPContactL();
        
        /**
        * The Symbian OS second-phase constructor
        * @return void
        */
        void CreateDefaultContactL();

        /**
        * The Symbian OS second-phase constructor
        * @return void
        */
        void DoCreateImppContactL();        
            
        /**
        * The Symbian OS second-phase constructor
        * @return void
        */
        void SetDefaultAttributeL( MVPbkStoreContactField* aField,  TInt aDefaultType );

    private: // Data

        RFs                                         iFileSession;
        TInt                                        iContactCountXsp;
        TInt									    iContactCountDefault;
        TBool                                       iDefaultStore;
        MVPbkStoreContact*                          iContact;
        MVPbkContactViewBase*                       iContactView;
        CCmsTesterContactCreator*                   iContactCreator;
    };

#endif

// End of File
