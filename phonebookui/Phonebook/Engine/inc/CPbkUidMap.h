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
*      Map of UIDs.
*
*/

#ifndef __CPbkUidMap_H__
#define __CPbkUidMap_H__

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 * Map of UIDs.
 */
NONSHARABLE_CLASS(CPbkUidMap) : 
        public CBase
    {
    public: // Interface
        /**
         * Constructor.
         */
        CPbkUidMap();

        /**
         * Destructor.
         */
        ~CPbkUidMap();

        /**
         * Inserts aUid into this map. UIDs already in the map are ignored.
         */
        void InsertL(const TUid& aUid);

        /**
         * Returns the number of unique UIDs in this map.
         */
        TInt Count() const;

        /**
         * Returns the unique index assigned for aUid by this map or 
         * KErrNotFound if aUid is not present on the map. Indexes are
         * changed if InsertL is used to insert a new UID into this map.
         */
        TInt IndexOf(const TUid& aUid) const;

    private:  // Data
        /// Own: sorted array of UIDs
        RArray<TInt> iUids;
    };


// INLINE FUNCTIONS

inline CPbkUidMap::CPbkUidMap()
    {
    }

inline CPbkUidMap::~CPbkUidMap()
    {
    iUids.Close();
    }

inline TInt CPbkUidMap::Count() const
    {
    return iUids.Count();
    }

inline void CPbkUidMap::InsertL(const TUid& aUid)
    {
    TInt index;
    if (iUids.FindInOrder(aUid.iUid,index) == KErrNotFound)
        {
        User::LeaveIfError(iUids.Insert(aUid.iUid,index));
        }
    }

inline TInt CPbkUidMap::IndexOf(const TUid& aUid) const
    {
    return iUids.FindInOrder(aUid.iUid);
    }

#endif // __CPbkUidMap_H__

// End of File
