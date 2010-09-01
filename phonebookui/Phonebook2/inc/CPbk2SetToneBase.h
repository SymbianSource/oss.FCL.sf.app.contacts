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
* Description:  Phonebook 2 set ringing tone command base.
*
*/


#ifndef CPBK2SETTONEBASE_H
#define CPBK2SETTONEBASE_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MPbk2ContactRelocatorObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2ProcessDecorator.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkStoreContact;
class CPbk2RingingToneFetch;
class CPbk2ContactRetriever;
class MVPbkFieldType;
class MVPbkContactOperationBase;
class CPbk2ContactRelocator;
class MVPbkContactLinkArray;
class MVPbkContactLink;

// CLASS DECLARATIONS

/**
 * Phonebook 2 set ringing tone base command.
 */
class CPbk2SetToneBase :  
        public CActive,
        public MPbk2Command,
        protected MPbk2ProcessDecoratorObserver,
        protected MPbk2ContactRelocatorObserver,
        protected MVPbkContactObserver,
        protected MVPbkSingleContactOperationObserver
    {
    public: // Constructor and destructor
        /**
         * Destructor.
         */
        IMPORT_C ~CPbk2SetToneBase();

    protected: // From CActive
        IMPORT_C void RunL();
        IMPORT_C void DoCancel();
        IMPORT_C TInt RunError( TInt aError );

    protected: // From MPbk2Command
        IMPORT_C void ExecuteLD();
        IMPORT_C void ResetUiControl(MPbk2ContactUiControl& aUiControl);
        IMPORT_C void AddObserver( MPbk2CommandObserver& aObserver );

    protected: // From MPbk2ProcessDecoratorObserver
        IMPORT_C void ProcessDismissed(
                TInt aCancelCode );

    protected: // Implementation
        /** 
         * C++ constructor
         * @param aUiControl Ui control
         */
        IMPORT_C CPbk2SetToneBase(
                MPbk2ContactUiControl& aUiControl );

        /** 
         * Second phase constructor
         */
        IMPORT_C void BaseConstructL();

        /** 
         * Retrieves contact of the aLink
         * @param aLink Link to a contact to be retrieved
         */
        IMPORT_C void RetrieveContactL(
                const MVPbkContactLink& aLink );
                
        /** 
         * Relocates a contact
         * @param aContact A contact to relocate
         */                
        IMPORT_C TBool RelocateContactL(
                MVPbkStoreContact* aContact );
        /** 
         * Sets tone for contact and commits it
         * @param aContact Contact to set tone to.
         */                
        IMPORT_C void SetToneAndCommitL(
                MVPbkStoreContact* aContact );
                
        /** 
         * Runs one asynchronous cycle for this active object.
         */                
        IMPORT_C void IssueRequest();
        
        /** 
         * Sets control's state EStopping and runs one asynchronous cycle 
         * for this active object.
         */                
        IMPORT_C void IssueStopRequest();

    private: // Implementation
        TBool FetchRingingToneL();
        void RetrieveExistingRingingToneFilenameL(
                TFileName& aFilename );
        void ReadFieldTypeL();

    protected: // Data structures
        /// Process states
        enum TState
            {
            EFetchingRingingTone,
            ERetrievingContact,
            EStopping
            };

    protected: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Helper class for fetching ringing tone
        CPbk2RingingToneFetch* iToneFetch;
        /// Ref: The ringing tone field type
        const MVPbkFieldType* iFieldType;
        /// Own: A new filename of ringing tone
        TFileName iToneFilename;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Own: Selected contacts
        MVPbkContactLinkArray* iSelectedContacts;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Base contact
        MVPbkStoreContact* iStoreContact;
        /// Own: State of the command
        TState iState;
    };

#endif // CPBK2SETTONEBASE_H

// End of File
