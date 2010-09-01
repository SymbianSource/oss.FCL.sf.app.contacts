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
* Description:  Phonebook 2 assign note service.
*
*/


#ifndef TPBK2ASSIGNNOTESERVICE_H
#define TPBK2ASSIGNNOTESERVICE_H

// INCLUDES
#include <e32base.h>
#include <TVPbkContactStoreUriPtr.h>

// FORWARD DECLARATIONS
class MPbk2ContactNameFormatter;
class CPbk2StorePropertyArray;
class MVPbkStoreContact;
class MVPbkContactStore;

// CLASS DECLARATION

/**
 * Phonebook 2 assign note service.
 * Responsible for:
 * - displaying all the notes used in contact assign service
 */
class TPbk2AssignNoteService
    {
    public: // Construction

        /**
         * Constructor.
         */
        TPbk2AssignNoteService();

    public: // Interface

        /**
         * Displays information note.
         *
         * @param aResourceId   Id of the resource where the note is defined.
         */
        void ShowInformationNoteL(
                TInt aResourceId ) const;

        /**
         * Displays store specific information note.
         *
         * @param aResourceId       Id of the resource where the note
         *                          is defined.
         * @param aStoreUri         Contact store URI.
         * @param aStoreProperties  Phonebook 2 contact store properties.
         */
        void ShowInformationNoteL(
                TInt aResourceId,
                TVPbkContactStoreUriPtr aStoreUri,
                const CPbk2StorePropertyArray& aStoreProperties ) const;

        /**
         * Displays confirmation note.
         *
         * @param aResourceId   Id of the resource where the note is defined.
         * @param aText         Additional text.
         * @return  Query result.
         *          @see CAknConfirmationNote::ExecuteLD
         */
        TInt ShowConfirmationNoteL(
                TInt aResourceId,
                const TDesC& aText ) const;

        /**
         * Displays replace detail query.
         *
         * @param aContactNameFormatter     Contact name formatter.
         * @param aStoreContact             Store contact.
         * @return  Query result.
         *          @see CAknQueryDialog::ExecuteLD
         */
        TInt ShowReplaceDetailQueryL(
                MPbk2ContactNameFormatter& aContactNameFormatter,
                MVPbkStoreContact& aStoreContact ) const;

        /**
         * Displays detail added note.
         *
         * @param aCount    Number of contacts the detail was added to.
         */
        void ShowDetailAddedNoteL(
                TInt aCount ) const;

        /**
         * Displays can not add detail note.
         *
         * @param aContactNameFormatter     Contact name formatter.
         * @param aStoreContact             Store contact.
         */
        void ShowCanNotAddDetailNoteL(
                MPbk2ContactNameFormatter& aContactNameFormatter,
                MVPbkStoreContact& aStoreContact ) const;

        /**
         * Displays create new contact to phone memory query.
         *
         * @return  Query result.
         *          @see CAknQueryDialog::ExecuteLD
         */
        TInt ShowCreateNewToPhoneQueryL() const;

        /**
         * Shows store full note.
         *
         * @param aTargetStore      Contact store.
         * @param aStoreProperties  Phonebook 2 contact store properties.
         */
        void ShowStoreFullNoteL(
                const MVPbkContactStore& aTargetStore,
                const CPbk2StorePropertyArray& aStoreProperties ) const;
    };

#endif // TPBK2ASSIGNNOTESERVICE_H

// End of File
