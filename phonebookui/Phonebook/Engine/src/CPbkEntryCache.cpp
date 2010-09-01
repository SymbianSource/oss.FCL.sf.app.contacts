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
*		Fixed-size cache of TPbkContactEntry instances
*
*/


// INCLUDE FILES
#include "CPbkEntryCache.h"
#include "CPbkContactEngine.h"
#include "CPbkContactItem.h"
#include <MPbkContactNameFormat.h>

// MACROS
#ifdef _DEBUG
    #define DEBUG_INC(var) ++(var)
#else
    #define DEBUG_INC(var)
#endif

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicPostCond_GetEntryL,
    EPanicPostCond_PreLoadL,
    EPanicPostCond_Find,
    EPanicInvariant1,
    EPanicInvariant2,
    EPanicInvariant3,
    EPanicInvariant4
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkEntryCache");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// ==================== MEMBER FUNCTIONS ====================

inline CPbkEntryCache::CPbkEntryCache
        (CPbkContactEngine& aEngine) : 
    iEntryQueue(_FOFF(TEntry,iLink)), 
    iEngine(aEngine)
    // Other members are reset by CBase::operator new
    {
    iEntryLoader = this;
    __ASSERT_DEBUG(!iEntries && iEntryLoader, 
        Panic(EPanicPostCond_Constructor));
    }

inline void CPbkEntryCache::ConstructL
        (TInt aCapacity)
    {
    __ASSERT_DEBUG(aCapacity > 0, Panic(EPanicPreCond_ConstructL));
    __ASSERT_DEBUG(!iEntries, Panic(EPanicPreCond_ConstructL));

    // Allocate one array block with all the cache entries
    iEntries = new(ELeave) TEntry[aCapacity];
    iCapacity = aCapacity;

    // Enqueue entries
    TEntry* const end = iEntries + iCapacity;
    for (TEntry* entry=iEntries; entry!=end; ++entry)
        {
        iEntryQueue.AddLast(*entry);
        }

    __ASSERT_DEBUG(iEntries, Panic(EPanicPostCond_ConstructL));
    __ASSERT_DEBUG(iCapacity == aCapacity, Panic(EPanicPostCond_ConstructL));
    __TEST_INVARIANT;
    }

EXPORT_C CPbkEntryCache* CPbkEntryCache::NewL
        (CPbkContactEngine& aEngine, 
        TInt aCapacity)
    {
    CPbkEntryCache* self = new(ELeave) CPbkEntryCache(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aCapacity);
    CleanupStack::Pop(self);
    return self;
    }

CPbkEntryCache::~CPbkEntryCache()
    {
    __TEST_INVARIANT;
    for (TInt i = iCapacity-1; i >= 0; --i)
        {
        iEntries[i].iEntry.Destroy();
        }
    delete [] iEntries;
    }

EXPORT_C TInt CPbkEntryCache::Capacity() const
    {
    __TEST_INVARIANT;
    return iCapacity;
    }

EXPORT_C const TPbkContactEntry& CPbkEntryCache::GetEntryL
        (TContactItemId aId)
    {
    __TEST_INVARIANT;

    TPbkContactEntry* entry = Find(aId);
    if (!entry)
        {
        TEntry* victim = iEntryQueue.Last();
        entry = &victim->iEntry;
        iEntryLoader->LoadEntryL(*entry, iEngine, aId);
        Touch(*victim);
        DEBUG_INC(iPerfStats.iMissCount);
        }
    else
        {
        DEBUG_INC(iPerfStats.iHitCount);
        }

    __TEST_INVARIANT;
    __ASSERT_DEBUG(entry == DebugFind(aId), Panic(EPanicPostCond_GetEntryL));
    return (*entry);
    }

EXPORT_C void CPbkEntryCache::PreloadL
        (TContactItemId aId)
    {
    __TEST_INVARIANT;

    if (!Find(aId))
        {
        TEntry* victim = iEntryQueue.Last();
        iEntryLoader->LoadEntryL(victim->iEntry, iEngine, aId);
        Touch(*victim);
        DEBUG_INC(iPerfStats.iMissCount);
        }
    else
        {
        DEBUG_INC(iPerfStats.iHitCount);
        }

    __ASSERT_DEBUG(DebugFind(aId), Panic(EPanicPostCond_PreLoadL));
    __TEST_INVARIANT;
    }

EXPORT_C void CPbkEntryCache::Unload
        (TContactItemId aId)
    {
    __TEST_INVARIANT;

    TEntry* entry = FindEntry(aId);
    if (entry)
        {
        entry->iEntry.Reset();
        // Move entry to the end of the queue
        entry->iLink.Deque();
        iEntryQueue.AddLast(*entry);
        }
    }

EXPORT_C void CPbkEntryCache::Flush()
    {
    TEntry* const end = iEntries + iCapacity;
    for (TEntry* entry = iEntries; entry != end; ++entry)
        {
        entry->iEntry.Reset();
        }
    }

void CPbkEntryCache::LoadEntryL
            (TPbkContactEntry& aEntry, 
            CPbkContactEngine& aEngine, 
            TContactItemId aContactId)
    {
    CPbkContactItem* contact = aEngine.ReadMinimalContactLC(aContactId);

    // Reserve space for name
    MPbkContactNameFormat& nameFormatter = aEngine.ContactNameFormat();
    const TInt titleLength = nameFormatter.ContactListTitleLength(*contact);
    aEntry.ReserveNameMaxLengthL(titleLength);

    // Get the contact icon id
    const TPbkIconId iconId = contact->ContactIconIdL();

    // All resources have been pre-allocated -> assign data
    aEntry.SetContactItemId(aContactId);
    if (titleLength > 0)
        {
        TPtr namePtr = aEntry.NameBuf();
        nameFormatter.GetContactListTitle(*contact,namePtr);
        }
    else
        {
        aEntry.ResetName();
        }
    aEntry.SetIconId(iconId);

    // Cleanup
    CleanupStack::PopAndDestroy(contact);
    }

/**
 * Returns a cache entry if present, NULL otherwise.
 */
TPbkContactEntry* CPbkEntryCache::Find
        (TContactItemId aId)
    {
    __TEST_INVARIANT;

    TPbkContactEntry* entry = NULL;
    TEntry* queueEntry = FindEntry(aId);
    if (queueEntry)
        {
        // Entry found
        Touch(*queueEntry);
        entry = &(queueEntry->iEntry);
        }

    __ASSERT_DEBUG(!entry || 
        (entry->ContactId()==aId && (&(iEntryQueue.First()->iEntry) == entry)),
        Panic(EPanicPostCond_Find));
    __TEST_INVARIANT;
    return entry;
    }

/**
 * Call when an entry is accessed (="touched"). Increases the entry's priority
 * for keeping it in this cache.
 */
void CPbkEntryCache::Touch(TEntry& aEntry)
    {
    aEntry.iLink.Deque();
    iEntryQueue.AddFirst(aEntry);
    }

CPbkEntryCache::TEntry* CPbkEntryCache::FindEntry
        (TContactItemId aId) const
    {
    // Search the cache brute-force. Searching in usage order using 
    // iEntryQueue is probably slower in modern cached processors due to
    // nonlocality of access.
    TEntry* const end = iEntries + iCapacity;
    for (TEntry* entry = iEntries; entry != end; ++entry)
        {
        if (entry->iEntry.ContactId() == aId)
            {
            return entry;
            }
        }
    return NULL;
    }

#ifdef _DEBUG

const TPbkContactEntry* CPbkEntryCache::DebugFind
        (TContactItemId aId) const
    {
    const TPbkContactEntry* entry = NULL;
    const TEntry* queueEntry = FindEntry(aId);
    if (queueEntry)
        {
        entry = &(queueEntry->iEntry);
        }
    return entry;
    }

void CPbkEntryCache::__DbgTestInvariant() const
    {
    if (!iEntries)
        {
        // Partial construction
        return;
        }

    __ASSERT_ALWAYS(iCapacity > 0, Panic(EPanicInvariant1));

    TInt count = 0;
    TDblQueIter<TEntry> iter(CONST_CAST(CPbkEntryCache*,this)->iEntryQueue);
    const TEntry* queueEntry;
    while (TInt(queueEntry = iter++))
        {
        ++count;
        // Each entry should be in iEntries array
        __ASSERT_ALWAYS(queueEntry >= iEntries && 
            queueEntry < iEntries+iCapacity, 
            Panic(EPanicInvariant2));
        }
    
    // All the entries should be in the queue
    __ASSERT_ALWAYS(count == iCapacity, Panic(EPanicInvariant3));

    // There should not be duplicate non-null entries
    for (TInt i=0; i < iCapacity; ++i)
        {
        const TContactItemId cid = iEntries[i].iEntry.ContactId();
        if (cid != KNullContactId)
            {
            for (TInt j=i+1; j < iCapacity; ++j)
                {
                __ASSERT_ALWAYS(iEntries[j].iEntry.ContactId() != cid,
                    Panic(EPanicInvariant4));
                }
            }
        }
    }

#endif // _DEBUG

// End of File
