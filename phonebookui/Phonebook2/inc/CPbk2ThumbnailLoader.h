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
* Description:  Phonebook 2 thumbnail loader.
*
*/


// INCLUDES
#include "CPbk2ThumbnailPopup.h"
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactOperationBase;
class CVPbkContactManager;
class MVPbkContactLink;
class MVPbkStoreContact;

// CLASS DECLARATION

NONSHARABLE_CLASS(CPbk2ThumbnailLoader) :
        public CBase,
        private MVPbkSingleContactOperationObserver,
        private MPbk2ThumbnailPopupObserver
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager       Virtual Phonebook contact manager.
         */
        IMPORT_C static CPbk2ThumbnailLoader* NewL(
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2ThumbnailLoader();

    public: // Interface

        /**
         * Loads a contact and shows its thumbnail if it has one.
         *
         * @param aContactLink          Contact link.
         * @param aListBox              Listbox presenting the contact.
         * @param aThumbnailLocation    Location where thumbnail is drawn.
         */
        IMPORT_C void LoadThumbnailL(
                MVPbkContactLink* aContactLink,
                const CEikListBox* aListBox,
                TPbk2ThumbnailLocation aThumbnailLocation );

        /**
         * Shows given contact's thumbnail if it has one.
         *
         * @param aStoreContact         Contact.
         * @param aListBox              Listbox presenting the contact.
         * @param aThumbnailLocation    Location where thumbnail is drawn.
         */
        IMPORT_C void LoadThumbnail(
                MVPbkStoreContact* aStoreContact,
                const CEikListBox* aListBox,
                TPbk2ThumbnailLocation aThumbnailLocation );

        /**
         * Hides the current thumbnail and cancels the retrieving of next
         * contact if it's currently being retrieved.
         */
        IMPORT_C void HideThumbnail();

        /**
         * Redraws the thumbnail.
         */
        IMPORT_C void Refresh();

        /**
         * Resets the loader. Calls HideThumbnail() and deletes
         * the currently loaded contact.
         */
        IMPORT_C void Reset();

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MPbk2ThumbnailPopupObserver
        void PopupLoadComplete();
        void PopupLoadFailed(
                TInt aError );

    private: // Implementation
        CPbk2ThumbnailLoader(
                CVPbkContactManager& aContactManager );
        void ConstructL();

    private: // Data
        /// Own: Thumbnail popup control
        CPbk2ThumbnailPopup* iThumbnailPopup;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: List box
        const CEikListBox* iListBox;
    };

// End of File
