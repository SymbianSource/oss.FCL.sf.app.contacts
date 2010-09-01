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
*		Data structure for Contact model Contact IDs
*
*/


// INCLUDE FILES
#include    "CPbkContactIdSet.h"        // header

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_NewL,
    EPanicPostCond_NewLC,
    EPanicPostCond_NewLC_CPbkContactIdSet,
    EPanicPostCond_NewLC_CContactIdArray,
    EPanicPostCond_AddL_TContactItemId,
    EPanicPostCond_AddL_CPbkContactIdSet,
    EPanicPostCond_AddL_CContactIdArray,
    EPanicPostCond_GetContactIdArrayL,
    EPanicPostCond_Remove_TContactItemId,
    EPanicPostCond_Remove_CContactIdArray,
    EPanicPostCond_Remove_CPbkContactIdSet,
    EPanicInvariant_Duplicate,
    EPanicInvariant_Ordering
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

// Debug only
#ifdef _DEBUG

/**
 * Returns ETrue if aSet1 and aSet2 have the same contents.
 */
TBool SameContents(const CPbkContactIdSet& aSet1, const CPbkContactIdSet& aSet2)
    {
    const TInt count = aSet1.Count();
    if (count == aSet2.Count())
        {
        for (TInt i=0; i < count; ++i)
            {
            if (aSet1[i] != aSet2[i])
                {
                return EFalse;
                }
            }
        return ETrue;
        }
    return EFalse;
    }

/**
 * Returns ETrue if aArray contains all ids in aSet.
 */
TBool SameContents(const CPbkContactIdSet& aSet, const CContactIdArray& aArray)
    {
    const TInt count = aArray.Count();
    TInt found = aSet.Count();
    if (count >= found)
        {
        for (TInt i=0; i < count; ++i)
            {
            if (aSet.Find(aArray[i]))
                {
                --found;
                }
            else
                {
                return EFalse;
                }
            }
        return (found <= 0);  // all set members were found
        }
    return EFalse;
    }

/**
 * Returns ETrue if aSet1 contains all aSet2's members.
 */
TBool ContainsAllMembers(const CPbkContactIdSet& aSet1, const CPbkContactIdSet& aSet2)
    {
    const TInt count = aSet2.Count();
    if (count <= aSet1.Count())
        {
        for (TInt i=0; i < count; ++i)
            {
            if (!aSet1.Find(aSet2[i]))
                {
                return EFalse;
                }
            }
        return ETrue;
        }
    return EFalse;
    }

/**
 * Returns ETrue if aSet contains all ids in aArray.
 */
TBool ContainsAllMembers(const CPbkContactIdSet& aSet, const CContactIdArray& aArray)
    {
    const TInt count = aArray.Count();
    if (count <= aSet.Count())
        {
        for (TInt i=0; i < count; ++i)
            {
            if (!aSet.Find(aArray[i]))
                {
                return EFalse;
                }
            }
        return ETrue;
        }
    return EFalse;
    }

/**
 * Returns ETrue if aSet does not contain any id in aArray.
 */
TBool DoesNotContainAnyMember(const CPbkContactIdSet& aSet, const CContactIdArray& aArray)
    {
    const TInt count = aArray.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (aSet.Find(aArray[i]))
            {
            return EFalse;
            }
        }
    return ETrue;
    }

/**
 * Returns ETrue if aSet1 does not contain any id in aSet2.
 */
TBool DoesNotContainAnyMember(const CPbkContactIdSet& aSet1, const CPbkContactIdSet& aSet2)
    {
    const TInt count = aSet2.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (aSet1.Find(aSet2[i]))
            {
            return EFalse;
            }
        }
    return ETrue;
    }


void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactIdSet");
    User::Panic(KPanicText, aReason);
    }

#endif


}  // namespace


// ==================== MEMBER FUNCTIONS ====================

inline CPbkContactIdSet::CPbkContactIdSet()
    {
    }

EXPORT_C CPbkContactIdSet* CPbkContactIdSet::NewL()
    {
    CPbkContactIdSet* self = new(ELeave) CPbkContactIdSet;

#ifdef _DEBUG    
    self->__DbgTestInvariant();
    __ASSERT_DEBUG(self->Count()==0, Panic(EPanicPostCond_NewL));
#endif

    return self;
    }

EXPORT_C CPbkContactIdSet* CPbkContactIdSet::NewLC()
    {
    CPbkContactIdSet* self = new(ELeave) CPbkContactIdSet;
    CleanupStack::PushL(self);

#ifdef _DEBUG    
    self->__DbgTestInvariant();
    __ASSERT_DEBUG(self->Count()==0, Panic(EPanicPostCond_NewLC));
#endif

    return self;
    }

EXPORT_C CPbkContactIdSet* CPbkContactIdSet::NewL(const CPbkContactIdSet& aSet)
    {
    // NewLC checks precond
    CPbkContactIdSet* self = CPbkContactIdSet::NewLC(aSet);
    CleanupStack::Pop(self);
    // NewLC checks postcond
    return self;
    }

EXPORT_C CPbkContactIdSet* CPbkContactIdSet::NewLC(const CPbkContactIdSet& aSet)
    {
    CPbkContactIdSet* self = CPbkContactIdSet::NewLC();

    const TInt count = aSet.iIds.Count();
    for (TInt i=0; i < count; ++i)
        {
        User::LeaveIfError(self->iIds.Append(aSet.iIds[i]));
        }

#ifdef _DEBUG    
    self->__DbgTestInvariant();
    __ASSERT_DEBUG(SameContents(*self,aSet) && self->Count()==aSet.Count(), 
        Panic(EPanicPostCond_NewLC_CPbkContactIdSet));
#endif

    return self;
    }

EXPORT_C CPbkContactIdSet* CPbkContactIdSet::NewL(const CContactIdArray& aArray)
    {
    // NewLC checks precond
    CPbkContactIdSet* self = CPbkContactIdSet::NewLC(aArray);
    CleanupStack::Pop(self);
    // NewLC checks postcond
    return self;
    }

EXPORT_C CPbkContactIdSet* CPbkContactIdSet::NewLC(const CContactIdArray& aArray)
    {
    CPbkContactIdSet* self = CPbkContactIdSet::NewLC();
    self->AddL(aArray);

#ifdef _DEBUG    
    self->__DbgTestInvariant();
    __ASSERT_DEBUG(SameContents(*self,aArray) && aArray.Count() >= self->Count(), 
        Panic(EPanicPostCond_NewLC_CContactIdArray));
#endif

    return self;
    }

CPbkContactIdSet::~CPbkContactIdSet()
    {
    __TEST_INVARIANT;

    iIds.Close();
    }

EXPORT_C TBool CPbkContactIdSet::Find(TContactItemId aId) const
    {
    __TEST_INVARIANT;

    return (iIds.FindInOrder(aId) != KErrNotFound);
    }

EXPORT_C void CPbkContactIdSet::AddL(TContactItemId aId)
    {
    __TEST_INVARIANT;

  	TInt pos;
    if (iIds.FindInOrder(aId,pos) == KErrNotFound)
        {
        User::LeaveIfError(iIds.Insert(aId,pos));
        }

    __TEST_INVARIANT;
    __ASSERT_DEBUG(Find(aId), Panic(EPanicPostCond_AddL_TContactItemId));
    }

EXPORT_C void CPbkContactIdSet::AddL(const CPbkContactIdSet& aSet)
    {
    __TEST_INVARIANT;

    // Add all new ids in aSet to the end of this set's array
    const TInt count = aSet.Count();
    for (TInt i=0; i < count; ++i)
        {
        const TContactItemId contactId = aSet[i];
        if (iIds.FindInOrder(contactId) == KErrNotFound)
            {
            const TInt err = iIds.Append(contactId);
            if (err != KErrNone)
                {
                // Undo all appends
                for (TInt ii=iIds.Count()-1; i > 0; --i, --ii)
                    {
                    iIds.Remove(ii);
                    }
                // Forward the error to caller
                User::Leave(err);
                }
            }
        }

    // Sort this set's array to "merge in" added elements
    iIds.Sort();

    __TEST_INVARIANT;
    __ASSERT_DEBUG(ContainsAllMembers(*this,aSet),
        Panic(EPanicPostCond_AddL_CPbkContactIdSet));
    }

EXPORT_C void CPbkContactIdSet::AddL(const CContactIdArray& aArray)
    {
    __TEST_INVARIANT;

    // Add all new ids in aArray to the end of this set's array
    const TInt count = aArray.Count();
    for (TInt i=0; i < count; ++i)
        {
        const TContactItemId contactId = aArray[i];
        if (iIds.FindInOrder(contactId) == KErrNotFound)
            {
            const TInt err = iIds.Append(contactId);
            if (err != KErrNone)
                {
                // Undo all appends
                for (TInt ii=iIds.Count()-1; i > 0; --i, --ii)
                    {
                    iIds.Remove(ii);
                    }
                // Forward the error to caller
                User::Leave(err);
                }
            }
        }

    // Sort this set's array to "merge in" added elements
    iIds.Sort();

    __TEST_INVARIANT;
    __ASSERT_DEBUG(ContainsAllMembers(*this,aArray), 
        Panic(EPanicPostCond_AddL_CContactIdArray));
    }

EXPORT_C CContactIdArray* CPbkContactIdSet::GetContactIdArrayL() const
    {
    __TEST_INVARIANT;

    CContactIdArray* array = CContactIdArray::NewLC();
    const TInt count = iIds.Count();
    for (TInt i=0; i < count; ++i)
        {
        array->AddL(iIds[i]);
        }
    CleanupStack::Pop(); // array
    __ASSERT_DEBUG(SameContents(*this,*array), Panic(EPanicPostCond_GetContactIdArrayL));
    return array;
    }

EXPORT_C void CPbkContactIdSet::Remove(TContactItemId aId)
    {
    __TEST_INVARIANT;

    const TInt index = iIds.FindInOrder(aId);
    if (index >= 0)
        {
        iIds.Remove(index);
        }

    __TEST_INVARIANT;
    __ASSERT_DEBUG(!Find(aId), Panic(EPanicPostCond_Remove_TContactItemId));
    }

EXPORT_C void CPbkContactIdSet::Remove(const CContactIdArray& aArray)
    {
    __TEST_INVARIANT;

    const TInt count = aArray.Count();
    for (TInt i=0; i < count; ++i)
        {
	    Remove(aArray[i]);
        }

    __TEST_INVARIANT;
    __ASSERT_DEBUG(DoesNotContainAnyMember(*this,aArray),
        Panic(EPanicPostCond_Remove_CContactIdArray));
    }

EXPORT_C void CPbkContactIdSet::Remove(const CPbkContactIdSet& aSet)
    {
    __TEST_INVARIANT;

    const TInt count = aSet.Count();
    for (TInt i=0; i < count; ++i)
        {
	    Remove(aSet[i]);
        }

    __TEST_INVARIANT;
    __ASSERT_DEBUG(DoesNotContainAnyMember(*this,aSet),
        Panic(EPanicPostCond_Remove_CPbkContactIdSet));
    }

// Checks that this set is in a consistent state. Note that invariant code
// loops through the set which means that calling the invariant may make
// performance look bad in debugging builds.
// Same comment applies to some complex postcondtions.
EXPORT_C void CPbkContactIdSet::__DbgTestInvariant() const
    {
#ifdef _DEBUG
    const TInt count = iIds.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (i < count-1)
            {
            __ASSERT_DEBUG(iIds[i] != iIds[i+1], Panic(EPanicInvariant_Duplicate));
            __ASSERT_DEBUG(iIds[i] < iIds[i+1], Panic(EPanicInvariant_Ordering));
            }
        }
#endif
    }

// End of File
