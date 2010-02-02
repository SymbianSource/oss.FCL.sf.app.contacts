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
*		Copies information from Phonebook entry to an existing entry.
*
*/


#ifndef __CPBKENTRYCOPYADDTOEXISTING_H__
#define __CPBKENTRYCOPYADDTOEXISTING_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include <cntdef.h>     // TContactItemId


// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkSimEntryCopierBase;
class CPbkContactItem;

// CLASS DECLARATION

/**
 * Class to insert SIM entry to existing aContactId.
 */
NONSHARABLE_CLASS(CPbkEntryCopyAddToExisting) : 
        public CBase
    {
    public: // interface
        /** 
         * Creates a new instance of this class.
		 * @param aEntry The entry to copy
		 * @param aEngine Reference to phonebook engine
		 * @param aContactId The contact where the entry is added to
         */
        static CPbkEntryCopyAddToExisting* NewLC
                (const CPbkContactItem& aEntry,
                CPbkContactEngine& aEngine,
                TContactItemId aContactId);
    
        /**
         * Copies the entry to Phonebook.
         * @return Contact id of the item that the data was copied to.
         */
        TContactItemId CopyL();

    private: // implementation
        CPbkEntryCopyAddToExisting
                (const CPbkContactItem& aEntry,
                CPbkContactEngine& aEngine,
                TContactItemId aContactId);

    private: // data members
        /// Ref: Sim entry
        const CPbkContactItem& iEntry;
        /// Ref: phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: Existing contact item id
        TContactItemId iContactId;
    };

#endif // __CPBKENTRYCOPYADDTOEXISTING_H__

// End of File
