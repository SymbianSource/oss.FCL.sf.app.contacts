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
* Description:  Phonebook 2 memory entry contact loader.
*
*/


#ifndef CPBK2MEMORYENTRYCONTACTLOADER_H
#define CPBK2MEMORYENTRYCONTACTLOADER_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2ContactNavigationLoader.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkContactOperationBase;
class MPbk2MemoryEntryView;

// CLASS DECLARATION

/**
 * Phonebook 2 memory entry contact loader utility class.
 */
class CPbk2MemoryEntryContactLoader : public CBase,
                                      public MPbk2ContactNavigationLoader,
                                      private MVPbkSingleContactOperationObserver,
                                      private MVPbkContactStoreObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aMemoryEntryAppView   Memory entry app view using the
         *              services of this class.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2MemoryEntryContactLoader* NewL(
                MPbk2MemoryEntryView& aMemoryEntryView );

        /**
         * Destructor.
         */
        ~CPbk2MemoryEntryContactLoader();

    public: // From MPbk2ContactNavigationLoader
        IMPORT_C void ChangeContactL(
                const MVPbkContactLink& aContactLink );
        IMPORT_C MVPbkContactLink* ContactLinkLC() const;

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // Implementation
        CPbk2MemoryEntryContactLoader(
                MPbk2MemoryEntryView& aMemoryEntryView );
        void RetrieveContactL();

    protected: // Data
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Ref: Memory entry appview
        MPbk2MemoryEntryView& iMemoryEntryView;
        /// Own: Contact link
        MVPbkContactLink* iContactLink;
        /// Ref: Contact store of the current contact
        MVPbkContactStore* iContactStore;
    };

#endif // CPBK2MEMORYENTRYCONTACTLOADER_H

// End of File
