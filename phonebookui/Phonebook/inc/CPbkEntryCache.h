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
*     Fixed-size cache of TPbkContactEntry instances
*
*/


#ifndef __CPbkEntryCache_H__
#define __CPbkEntryCache_H__

//  INCLUDES
#include    <TPbkContactEntry.h>
#include    <MPbkContactEntryLoader.h>

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Fixed-size cache of TPbkContactEntry instances.
 */
class CPbkEntryCache : 
        public CBase, public MPbkContactEntryLoader
    {
    public:
        /**
         * Creates a new instance of this class.
		 * @param aEngine reference to phonebook engine
         * @param aCapacity cache capacity
         */
        IMPORT_C static CPbkEntryCache* NewL(CPbkContactEngine& aEngine,
			TInt aCapacity);

        /**
         * Destructor.
         */
        ~CPbkEntryCache();

        /**
         * Returns this cache's capacity.
         */
        IMPORT_C TInt Capacity() const;

        /**
         * Retrieves an entry aId from this cache or loads the 
         * entry from the DB if not currently present in the cache.
         *
         * @exception KErrNotFound  if contact aId is not found in this cache
         *                          or in contact DB.
         */
        IMPORT_C const TPbkContactEntry& GetEntryL(TContactItemId aId);

        /**
         * Preloads entry aId into this cache if not already present.
         */
        IMPORT_C void PreloadL(TContactItemId aId);

        /**
         * Unloads an entry aId from this cache.
         */
        IMPORT_C void Unload(TContactItemId aId);

        /**
         * Flushes the cache completely.
         */
        IMPORT_C void Flush();

        /**
         * Sets the entry loader from aContactEntryLoader.
         */
        inline void SetEntryLoader(MPbkContactEntryLoader&
			aContactEntryLoader);

    public:  // Debug-only
        #ifdef _DEBUG
        /// Debug-only performance statistics for CPbkEntryCache
        struct TPerfStats
            {
            /// Own: hit count (debug only)
            TInt iHitCount;
            /// Own: miss count (debug only)
            TInt iMissCount;
            } iPerfStats;
        #endif

    private:  // from MPbkContactEntryLoader
        void LoadEntryL
            (TPbkContactEntry& aEntry, 
            CPbkContactEngine& aEngine, 
            TContactItemId aContactId);

    private:  // Implementation
        struct TEntry
            {
            TDblQueLink iLink;
            TPbkContactEntry iEntry;
            };
        CPbkEntryCache(CPbkContactEngine& aEngine);
        void ConstructL(TInt aCapacity);
        TPbkContactEntry* Find(TContactItemId aId);
        void Touch(TEntry& aEntry);
        TEntry* FindEntry(TContactItemId aId) const;
        const TPbkContactEntry* DebugFind(TContactItemId aId) const;
        void __DbgTestInvariant() const;

    private:  // Data
        /// Own: double queue of entries
        mutable TDblQue<TEntry> iEntryQueue;
        /// Own: memory block for all the cache entries.
        TEntry* iEntries;
        /// Own: Capacity.
        TInt iCapacity;
        /// Ref: Phonebook engine.
        CPbkContactEngine& iEngine;
        /// Ref: Contact entry loader
        MPbkContactEntryLoader* iEntryLoader;

    private:  // friend declarations
        /// For testing only
        friend class CPbkEntryCache_TestAccess;
    };


inline void CPbkEntryCache::SetEntryLoader
        (MPbkContactEntryLoader& aContactEntryLoader)
    {
    iEntryLoader = &aContactEntryLoader;
    }

#endif // __CPbkEntryCache_H__
            
// End of File
