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
* Description:  Phonebook 2 SIM copy results.
*
*/


#ifndef TPSU2SIMCOPYRESULTS_H
#define TPSU2SIMCOPYRESULTS_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
 *  Phonebook 2 SIM copy results.
 */
class TPsu2SimCopyResults
    {
    public:  // Constructors and destructor

        /**
         * Constructor.
         */
        TPsu2SimCopyResults();

        /**
         * C++ constructor.
         * Used for showing multiple copy notes.
         *
         * @param aCopied   Number of succesfully copied contacts.
         * @param aTotal    Total number of contacts to copy.
         */
        TPsu2SimCopyResults(
                TInt aCopied,
                TInt aTotal );

        /**
         * C++ constructor.
         * Used for showing single copy notes.
         *
         * @param aCopied   Number of succesfully copied contacts.
         * @param aName     Name of the contact.
         */
        TPsu2SimCopyResults(
                TInt aCopied,
                const TDesC& aName );

    public: // Interface

        /**
         * Shows the copy results note.
         */
        void ShowNoteL();

        /**
         * Shows the copy error note.
         */
        void ShowErrorNoteL();

        /**
         * Shows the partly copied note.
         */
        void ShowPartlyCopiedNoteL();

    private: // Implementation
        void ShowSingleCopyNoteL();
        void ShowMultipleCopyNoteL();

    private: // Data
        /// Own: Number of succesfully copied contacts
        TInt iCopied;
        /// Own: Number of contacts that were not copied
        TInt iNotCopied;
        /// Own: Contact name
        TPtrC iName;
    };

#endif // TPSU2SIMCOPYRESULTS_H

// End of File
