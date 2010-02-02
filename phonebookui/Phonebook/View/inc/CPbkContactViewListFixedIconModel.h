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
*    Phonebook contact view listbox model.
*
*/


#ifndef __CPbkContactViewListFixedIconModel_H__
#define __CPbkContactViewListFixedIconModel_H__

//  INCLUDES
#include "CPbkContactViewListModelCommon.h"

//  FORWARD DECLARATIONS
class MPbkContactNameFormat;
class CPbkViewContactNameFormatter;

/// Use for testing only!!!
class CPbkContactViewListModel_TestAccess;


// CLASS DECLARATION

/**
 * Listbox model for CPbkContactViewListControl.
 */
NONSHARABLE_CLASS(CPbkContactViewListFixedIconModel) : 
        public CPbkContactViewListModelCommon
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aParams @see PbkContactViewListModelFactory
         */
        static CPbkContactViewListFixedIconModel* NewL(
            PbkContactViewListModelFactory::TParams& aParams);

        /**
         * Destructor.
         */ 
        ~CPbkContactViewListFixedIconModel();

    private:  // from CPbkContactViewListModelBase
        void FormatBufferForContactL(const CViewContact& aViewContact) const;

    private: // From MPbkContactViewListModel
        void PurgeEntry(TContactItemId /*aContactId*/) {}
        void SetEntryLoader(MPbkContactEntryLoader& /*aContactEntryLoader*/) {}
		void FlushCache() {}

    private:  // Implementation
        CPbkContactViewListFixedIconModel(
            PbkContactViewListModelFactory::TParams& aParams);
        void ConstructL();

        /// Use for testing only!!!
        friend class CPbkContactViewListModel_TestAccess;

    private:  // data
        /// Ref: contact name formatter
        MPbkContactNameFormat& iNameFormatter;
        /// Own: view contact name formatter
        CPbkViewContactNameFormatter* iViewContactNameFormatter;
    };

#endif  // __CPbkContactViewListFixedIconModel_H__

// End of File
