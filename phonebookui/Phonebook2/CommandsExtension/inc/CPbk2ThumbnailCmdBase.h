/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Base class for thumbnail commands set and remove.
*
*/


#ifndef CPBK2THUMBNAILCMDBASE_H
#define CPBK2THUMBNAILCMDBASE_H

//  INCLUDES
#include <e32base.h>    // CBase
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MPbk2ContactRelocatorObserver.h>

// FORWARD DECLARATIONS
class CEikMenuPane;
class MPbk2ContactUiControl;
class CVPbkContactManager;
class CPbk2ImageManager;
class MVPbkStoreContact;
class MVPbkFieldType;
class MVPbkContactLink;
class CPbk2ContactRelocator;

// CLASS DECLARATION

/**
 * Thumbnail command event handling class.
 */
NONSHARABLE_CLASS(CPbk2ThumbnailCmdBase) : 
        public CBase,
        public MPbk2Command,
        public MVPbkContactObserver,        
        public MVPbkSingleContactOperationObserver,
        public MPbk2ContactRelocatorObserver

    {
    protected:  // Constructors and destructor
        /**
         * Destructor.
         */
        ~CPbk2ThumbnailCmdBase();
        
    protected: // Interface
        /**
         * Base constructor.
         */
        void BaseConstructL();

        /**
         * Dismisses process.
         *
         * @param aError    Error code.
         */
        void ProcessDismissed(
                TInt aError );
        
	public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver( MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    protected: // Interface
    	/**
    	 * Function will be called after the contact has retrieved
    	 * successfully.
    	 * If process should be end returns EFalse.
    	 * If process will be ended elsewhere (e.g. after some 
    	 * async operation) returns ETrue.
    	 * Default implementation does nothing and returns EFalse.
         *
         * @return EFalse if process should ended right now,
         *         ETrue otherwise.
    	 */
    	virtual TBool ExecuteCommandL()
                { return EFalse; }

		/**
		 * Will be called when the process is about to finish.
		 * Default implementation does nothing if aError == KErrNone,
		 * otherwise informs user about error.
         *
         * @param aError    Error code.
		 */
        virtual void FilterErrors(
                TInt aError );

    private: // From MVPbkContactObserver
    	void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // From MVPbkSingleContactOperationObserver
    	void VPbkSingleContactOperationComplete(
    		    MVPbkContactOperationBase& aOperation, 
    		    MVPbkStoreContact* aContact );
		void VPbkSingleContactOperationFailed(
        	    MVPbkContactOperationBase& aOperation,
                TInt aError );
        	
    private: // From MPbk2ContactRelocatorObserver
        void ContactRelocatedL(
                MVPbkStoreContact* aRelocatedContact );
        void ContactRelocationFailed(
                TInt aReason,
                MVPbkStoreContact* aContact );
        void ContactsRelocationFailed(
                TInt aReason,
                CVPbkContactLinkArray* aContacts );
        void RelocationProcessComplete();

    protected: // Implementation
        CPbk2ThumbnailCmdBase(
                MPbk2ContactUiControl& aUiControl );
        void ReadFieldTypeL();
        TBool RelocateContactL();

    protected: // Data
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Selected contacts from UI control
        MVPbkContactLink* iContactLink;
        /// Own: Thumbnail manager
        CPbk2ImageManager* iThumbnailManager;
    	/// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager* iContactManager;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Ref: Thumbnail field type
        const MVPbkFieldType* iFieldType;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Own: Focused field index
        TInt iFocusedFieldIndex;
    };

#endif // CPBK2THUMBNAILCMDBASE_H
            
// End of File
