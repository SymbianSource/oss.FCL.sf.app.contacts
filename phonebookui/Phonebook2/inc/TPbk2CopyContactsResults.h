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
* Description:  Phonebook 2 copy contacts result note.
*
*/


#ifndef TPBK2COPYCONTACTSRESULTS_H
#define TPBK2COPYCONTACTSRESULTS_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
 * Phonebook 2 copy contacts result note.
 */
class TPbk2CopyContactsResults
    {
    public: // Constructors

        /**
         * Constructor.
         * For displaying an error note.
         */
        IMPORT_C TPbk2CopyContactsResults();

        /**
         * Constructor.
         * For displaying multiple copy notes.
         *
         * @param aCopied   Number of copied contacts.
         * @param aTotal    Total number of contacts in the copy operation.
         */
        IMPORT_C TPbk2CopyContactsResults(
                TInt aCopied,
                TInt aTotal );

        /**
         * C++ constructor.
         * For displaying a single copy note.
         *
         * @param aCopied   Number of copied contacts.
         * @param aName     Name of the contact which was in the copy.
         */
        IMPORT_C TPbk2CopyContactsResults(
                TInt aCopied,
                const TDesC& aName );

    public: // Interface

        /**
         * Shows the copy results note
         */
        IMPORT_C void ShowNoteL();

        /**
         * Shows the copy error note
         */
        IMPORT_C void ShowErrorNoteL();

        /**
         * Sets the text resource id for "one contact copied" note.
         * If not set, default is used.
         * Default = R_QTN_PBCOP_NOTE_CONTACT_COPIED_PB2.
         *
         * @param aResId    Resource id of the note.
         */
        IMPORT_C void SetOneContactCopiedTextRes(
                TInt aResId );

        /**
         * Sets the text resource id for "one contact NOT copied" note
         * If not set, default is used.
         * Default = R_QTN_PBCOP_NOTE_ENTRY_NOT_COPIED.
         *
         * @param aResId    Resource id of the note.
         */
        IMPORT_C void SetOneContactNotCopiedTextRes(
                TInt aResId );

        /**
         * Sets the text resource id for "multiple contacts copied" note.
         * If not set, default is used.
         * Default = R_QTN_PBCOP_NOTE_N_ENTRY_COPY_PB.
         *
         * @param aResId    Resource id of the note.
         */
        IMPORT_C void SetMultipleContactsCopiedTextRes(
                TInt aResId );

    private: // Data
        /// Own: Number of copied contacts
        TInt iCopied;
        /// Own: Number of not copied contacts
        TInt iNotCopied;
        /// Own: Name of the contact
        TPtrC iName;
        /// Own: Resource id of the one contact copied note
        TInt iOneContactCopiedResId;
        /// Own: Resource id of the one contact not copied note
        TInt iOneContactNotCopiedResId;
        /// Own: Resource id of the multiple contacts copied note
        TInt iMultipleContactsCopiedResId;
    };

#endif // TPBK2COPYCONTACTSRESULTS_H

// End of File
