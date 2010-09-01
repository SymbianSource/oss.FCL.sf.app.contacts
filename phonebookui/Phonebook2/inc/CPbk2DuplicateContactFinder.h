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
* Description:  Utility class for finding duplicate contacts
*                from the stores.
*
*/


#ifndef CPBK2DUPLICATECONTACTFINDER_H
#define CPBK2DUPLICATECONTACTFINDER_H

#include <e32base.h>
#include <MVPbkContactFindObserver.h>
#include <MVPbkSingleContactOperationObserver.h>

class CVPbkContactManager;
class MVPbkContactStore;
class MVPbkBaseContact;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkFieldTypeList;
class MPbk2ContactNameFormatter;
class MPbk2DuplicateContactObserver;
class MVPbkBaseContactField;

/**
 * Utility class for finding duplicate contacts from the stores.
 * Finds duplicates from stores by using the name formatter.
 */
class CPbk2DuplicateContactFinder : public CActive,
                                    private MVPbkContactFindObserver,
                                    private MVPbkSingleContactOperationObserver
    {
    public: // Construction and destruction

        /**
         * Constructs a duplicate finder that finds duplicate contacts
         * from all the stores.
         *
         * @param aContactManager       Virtual Phonebook contact manager
         *                              that owns the field types and can
         *                              be used for finding the stores.
         * @param aNameFormatter        Name formatter for recognizing
         *                              duplicate contacts.
         * @param aFieldTypesForFind    These are the field types that are
         *                              usedfor the find. Most probably the
         *                              same field types as are in the sort
         *                              order. These define also the order
         *                              for getting the find text.
         *                              E.g if last name is the first in
         *                              the list and first name is the second
         *                              one then the last name is taken from
         *                              the given contact and that text is
         *                              used for the find. If there was no
         *                              last name in the contact then the
         *                              first name text is taken.
         * @param aDuplicateContacts    The result array for
         *                              duplicate contacts.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2DuplicateContactFinder* NewL(
                CVPbkContactManager& aContactManager,
                MPbk2ContactNameFormatter& aNameFormatter,
                const MVPbkFieldTypeList& aFieldTypesForFind,
                RPointerArray<MVPbkStoreContact>& aDuplicateContacts );

        /**
         * Destructor.
         */
        virtual ~CPbk2DuplicateContactFinder();

    public: // Interface

        /**
         * Starts asynchronous duplicate find from the given store.
         *
         * @param aContact              The source contact whose duplicates
         *                              are searched for.
         * @param aStore                A store for creating find operation.
         * @param aObserver             An observer to notify after the
         *                              checking has been done.
         * @param aMaxDuplicatesToFind  Finder stops if the it finds more
         *                              than max amount of duplicates.
         */
        IMPORT_C void StartL(
                const MVPbkBaseContact& aContact,
                MVPbkContactStore& aStore,
                MPbk2DuplicateContactObserver& aObserver,
                TInt aMaxDuplicatesToFind );
        /**
         * Starts asynchronous duplicate find from all the stores.
         *
         * @param aContact              The source contact whose duplicates
         *                              are searched for.
         * @param aObserver             An observer to notify after the
         *                              checking has been done.
         * @param aMaxDuplicatesToFind  Finder stops if the it finds more
         *                              than max amount of duplicates.
         */
        IMPORT_C void StartL(
                const MVPbkBaseContact& aContact,
                MPbk2DuplicateContactObserver& aObserver,
                TInt aMaxDuplicatesToFind );

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // From MVPbkContactFindObserver
        void FindCompleteL(
                MVPbkContactLinkArray* aResults );
        void FindFailed(
                TInt aError );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // Implementation
        CPbk2DuplicateContactFinder(
                CVPbkContactManager& aContactManager,
                MPbk2ContactNameFormatter& aNameFormatter,
                const MVPbkFieldTypeList& aFieldTypesForFind,
                RPointerArray<MVPbkStoreContact>& aDuplicateContacts );
        void ConstructL();
        void IssueRequest();
        TPtrC FindText();
        MVPbkContactOperationBase* FindOperationL(
                const TDesC& aFindText );
        void StartFindL();
        void RetrieveContactL();
        void CompleteL();
        TBool LastContactRetrieved();
        void DestroyOperation();
        void CheckDuplicateL(
                MVPbkStoreContact* aContact );
        TBool IsFieldMatched( const MVPbkBaseContact* aContact, TInt aFieldId );
        const MVPbkBaseContactField* FindFieldById( const MVPbkBaseContact* aContact, 
                TInt aFieldId );

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// ReF: Name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: These are the field types that are used for find
        const MVPbkFieldTypeList& iFieldTypesForFind;
        /// Own: A pointer to find text
        TPtrC iFindText;
        /// Ref: A store for single store check
        MVPbkContactStore* iStore;
        /// Ref: Observer
        MPbk2DuplicateContactObserver* iObserver;
        /// Ref:: A contact whose duplicates are searched for
        const MVPbkBaseContact* iContactToCompare;
        /// Own: a title of iContactToCompare
        HBufC* iContactTitle;
        /// Own: Find operation
        MVPbkContactOperationBase* iContactOperation;
        /// Own: Results of the find
        MVPbkContactLinkArray* iFindResults;
        /// Ref: Duplicate contacts
        RPointerArray<MVPbkStoreContact>& iDuplicates;
        /// An index in iFindResults for retrieving.
        TInt iContactIndex;
        /// Internal state of the finder
        TInt iState;
        /// The maximum amount of duplicates to find
        TInt iMaxDuplicatesToFind;
    };

#endif // CPBK2DUPLICATECONTACTFINDER_H

// End of File
