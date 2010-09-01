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


#ifndef __CPbkContactViewListCacheModel_H__
#define __CPbkContactViewListCacheModel_H__


//  INCLUDES
#include "CPbkContactViewListModelCommon.h"

//  FORWARD DECLARATIONS
class TPbkContactEntry;
class CPbkEntryCache;
class MPbkContactEntryLoader;

/// Use for testing only!!!
class CPbkContactViewListModel_TestAccess;

// CLASS DECLARATION

/**
 * Listbox model for CPbkContactViewListControl.
 */
NONSHARABLE_CLASS(CPbkContactViewListCacheModel) : 
        public CPbkContactViewListModelCommon
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aParams @see PbkContactViewListModelFactory
         */
        static CPbkContactViewListCacheModel* NewL
            (PbkContactViewListModelFactory::TParams& aParams);

        /**
         * Destructor.
         */ 
        ~CPbkContactViewListCacheModel();

    public:  // interface
        /**
         * Returns contact entry for aContactId.
         * If contact is a group returns NULL.
         * @param aContactId contact id for entry
         */
        const TPbkContactEntry* GetEntryL(TContactItemId aContactId) const;

        /**
         * Purges an entry from this model's cache.
         *
         * @param aContactId contact id for entry
         */
        void PurgeEntry(TContactItemId aContactId);

        /**
         * Sets contact entry loader.
         * @aContactEntryLoader loader to be set
         */
        void SetEntryLoader(MPbkContactEntryLoader& aContactEntryLoader);

        /**
         * Flushes cache.
         */
        void FlushCache();

    private:  // from CPbkContactViewListModelBase
        void FormatBufferForContactL(const CViewContact& aViewContact) const;

    private:  // Implementation
        CPbkContactViewListCacheModel
            (PbkContactViewListModelFactory::TParams& aParams);
        void ConstructL(PbkContactViewListModelFactory::TParams& aParams);
        /// Use for testing only!!!
        friend class CPbkContactViewListModel_TestAccess;

    private:  // data
        /// Own: cache of contact entries
        CPbkEntryCache* iEntryCache;
    };

#endif  // __CPbkContactViewListCacheModel_H__

// End of File
