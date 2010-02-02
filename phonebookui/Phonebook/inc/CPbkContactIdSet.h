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
*     Set data structure for contact model contact IDs.
*
*/


#ifndef __CPbkContactIdSet_H__
#define __CPbkContactIdSet_H__

//  INCLUDES
#include <cntdef.h>

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Data structure for Contact model Contact IDs.
 */
class CPbkContactIdSet : public CBase
	{
    public: // Interface
        /**
         * Constructor.
         */
	    IMPORT_C static CPbkContactIdSet* NewL();

        /**
         * Constructor.
         */
	    IMPORT_C static CPbkContactIdSet* NewLC();

        /**
         * Constructor. Copies the contents of this set from aSet.
         */
	    IMPORT_C static CPbkContactIdSet* NewL(const CPbkContactIdSet& aSet);

        /**
         * Constructor. Copies the contents of this set from aSet.
         */
	    IMPORT_C static CPbkContactIdSet* NewLC(const CPbkContactIdSet& aSet);

        /**
         * Constructor. Copies the contents of this set from aArray.
         */
        IMPORT_C static CPbkContactIdSet* NewL(const CContactIdArray& aArray);

        /**
         * Constructor. Copies the contents of this set from aArray.
         */
        IMPORT_C static CPbkContactIdSet* NewLC(const CContactIdArray& aArray);

        /**
         * Destructor.
         */
	    ~CPbkContactIdSet();

        /**
         * Returns TContactItemId at aIndex in this set.
         */
	    inline TContactItemId operator[](TInt aIndex) const;

        /**
         * Returns number of Ids in the set.
         */
	    inline TInt Count() const;

        /**
         * Removes all items from this set.
         */
	    inline void Reset();

        /**
         * Searches for aId from this set, returns ETrue if found.
         */
	    IMPORT_C TBool Find(TContactItemId aId) const;

        /**
         * Adds aId to this set.
         *
         * @exception KErrNoMemory  if this function leaves this set is not 
         *                          modified.
         * @postcond Find(aId) && Count() == old(Count()+1)
         */
	    IMPORT_C void AddL(TContactItemId aId);

        /**
         * Adds all Ids in aSet to this set.
         *
         * @exception KErrNoMemory  if this function leaves none of the Ids in 
         *                          aSet are added to this set.
         * @postcond for all Ids i in aSet: Find(i)==ETrue
         */
	    IMPORT_C void AddL(const CPbkContactIdSet& aSet);

        /**
         * Adds all Ids in aArray to this set.
         *
         * @exception KErrNoMemory  if this function leaves none of the Ids in 
         *                          aArray are added to this set.
         * @postcond for all Ids i in aArray: Find(i)==ETrue
         */
	    IMPORT_C void AddL(const CContactIdArray& aArray);

        /**
         * Removes aId from this set.
         */
	    IMPORT_C void Remove(TContactItemId aId);

        /**
         * Removes all Ids in aArray from this set.
         */
	    IMPORT_C void Remove(const CContactIdArray& aArray);

        /**
         * Removes all Ids in aSet from this set.
         */
        IMPORT_C void Remove(const CPbkContactIdSet& aSet);

        /**
         * Creates a CContactIdArray copy of this set's contents.
         *
         * @return  new CContactIdArray containing all contact Ids in this set.
         *          Caller is responsible of the returned array.
         */
        IMPORT_C CContactIdArray* GetContactIdArrayL() const;

        __DECLARE_TEST;

    private: // Implementation
	    CPbkContactIdSet();
        
    private: // Data
        /// Own: sorted array of contact Ids.
	    RArray<TInt> iIds;
	};

inline TContactItemId CPbkContactIdSet::operator[](TInt aIndex) const
    {
    // iIds checks aIndex
    return iIds[aIndex];
    }

inline TInt CPbkContactIdSet::Count() const
    {
    return iIds.Count();
    }

inline void CPbkContactIdSet::Reset()
    {
    iIds.Reset();
    }


#endif // __CPbkContactIdSet_H__

// End of File
