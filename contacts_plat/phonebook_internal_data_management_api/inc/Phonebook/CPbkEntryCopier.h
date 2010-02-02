/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*		Offers interface for adding entries to Phonebook.
*
*/


#ifndef __CPbkEntryCopier_H__
#define __CPbkEntryCopier_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include <cntdef.h>     // TContactItemId


// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class MPbkAddNewDataState;


// CLASS DECLARATION
/**
 * Observer interface for insertion process.
 */
class MPbkEntryCopyObserver
    {
    public:
        /**
         * The copy was successful
         * @param aContactId The contact id the entry was copied to.
         */
        virtual void CopySuccess(TContactItemId aContactId) = 0;

        /**
         * There was a copy error
         * @param aError error that occurred
         */
        virtual void CopyError(TInt aError) = 0;
    };

/**
 * Entry copier.
 */
class CPbkEntryCopier : public CBase
    {
    public: // interface
        /** 
         * Creates a new instance of this class.
         *
		 * @param aEngine Phonebook engine
         * @param aObserver Entry copying observer
         * @return Newly created entry copier.
         */
        IMPORT_C static CPbkEntryCopier* NewL
                (CPbkContactEngine& aEngine, MPbkEntryCopyObserver& aObserver);

        /**
         * Inserts a new entry asynchronously to the Phonebook.
         *
		 * @param aEntry Entry to copy
         */
        IMPORT_C void AddToPhonebookL(CPbkContactItem& aEntry);

        /**
         * Destructor
         */
        ~CPbkEntryCopier();

    private: // implementation
        CPbkEntryCopier(CPbkContactEngine& aEngine,
						MPbkEntryCopyObserver& aObserver);
        void ConstructL();
        void ProceedToNextState(TInt aState, TInt aError);
        void DoProceedToNextStateL(TInt aState);
        friend class CPbkAddNewDataBaseState;
        friend class CPbkInsertToPhonebookState;
    
    private: // member data
        /// Ref: Contact engine
        CPbkContactEngine& iEngine;
        /// Ref: process observer
        MPbkEntryCopyObserver& iObserver;
        /// Entry to copy
        CPbkContactItem* iEntry;
        /// Own: state object
        RPointerArray<MPbkAddNewDataState> iStateArray;
        /// Own: the contact inserted
        TContactItemId iNewContactId;
    };

#endif // __CPbkEntryCopier_H__

// End of File
