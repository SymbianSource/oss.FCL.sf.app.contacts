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
*		Phonebook contact iterator
*
*/


// INCLUDE FILES
#include "CPbkContactIter.h"
#include <cntitem.h>
#include "CPbkContactEngine.h"
#include "CPbkContactItem.h"

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
// CONSTANTS
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPostCond_FirstL,
    EPanicPostCond_NextL,
    EPanicPostCond_CurrentL,
    EPanicClassInvariant
    };

// LOCAL FUNCTIONS
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactIter");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// ==================== MEMBER FUNCTIONS ====================

/**
 * Constructor.
 * @param aEngine   Phonebook engine whose contacts to iterate.
 */
inline CPbkContactIter::CPbkContactIter
        (CPbkContactEngine& aEngine, TBool aUseMinimalRead)
    : iEngine(aEngine), 
    iCmIter(aEngine.Database()), 
    iUseMinimalRead(aUseMinimalRead)
    {
    // Other members reset by new(ELeave) in NewL and NewLC

    // PostCond
    __ASSERT_DEBUG(!iCurrentItem && !iCurrentPbkItem, 
        Panic(EPanicPostCond_Constructor));
    __TEST_INVARIANT;
    }

EXPORT_C CPbkContactIter* CPbkContactIter::NewL
        (CPbkContactEngine& aEngine, TBool aUseMinimalRead/*=EFalse*/)
    {
    return new(ELeave) CPbkContactIter(aEngine, aUseMinimalRead);
    }

EXPORT_C CPbkContactIter* CPbkContactIter::NewLC
        (CPbkContactEngine& aEngine, TBool aUseMinimalRead/*=EFalse*/)
    {
    CPbkContactIter* self = 
        new(ELeave) CPbkContactIter(aEngine, aUseMinimalRead);
    CleanupStack::PushL(self);
    return self;
    }

CPbkContactIter::~CPbkContactIter()
    {
    __TEST_INVARIANT;
    delete iCurrentPbkItem;
    delete iCurrentItem;
    }

EXPORT_C TContactItemId CPbkContactIter::FirstL()
    {
    __TEST_INVARIANT;

    // Move forward from the first contact item until a Phonebook contact card 
    // is found or end of DB is reached
    CContactItem* newCurrentItem = NULL;
    TContactItemId cid;
    for (cid = iCmIter.FirstL(); cid != KNullContactId; cid = iCmIter.NextL())
        {
        newCurrentItem = ReadPhonebookContactL(cid);
        if (newCurrentItem)
            {
            // Phonebook contact was found
            break;
            }
        }

    // No leaves -> switch safely to the new state
    delete iCurrentItem;
    iCurrentItem = newCurrentItem;
    delete iCurrentPbkItem;
    iCurrentPbkItem = NULL;

    // PostCond
    __ASSERT_DEBUG(
        (cid==KNullContactId && !iCurrentItem) || iCurrentItem->Id()==cid, 
        Panic(EPanicPostCond_FirstL));

    __TEST_INVARIANT;
    return cid;
    }

EXPORT_C TContactItemId CPbkContactIter::NextL()
    {
    __TEST_INVARIANT;

    // Move to next until a Phonebook contact card is found or end of DB reached
    CContactItem* newCurrentItem = NULL;
    TContactItemId cid;
    for (cid = iCmIter.NextL(); cid != KNullContactId; cid = iCmIter.NextL())
        {
        newCurrentItem = ReadPhonebookContactL(cid);
        if (newCurrentItem)
            {
            // Phonebook contact was found
            break;
            }
        }

    // No leaves -> switch safely to the new state
    delete iCurrentItem;
    iCurrentItem = newCurrentItem;
    delete iCurrentPbkItem;
    iCurrentPbkItem = NULL;

    // PostCond
    __ASSERT_DEBUG(
        (cid == KNullContactId && !iCurrentItem) || iCurrentItem->Id() == cid,
        Panic(EPanicPostCond_NextL));

    __TEST_INVARIANT;
    return cid;
    }

EXPORT_C void CPbkContactIter::Reset()
    {
    __TEST_INVARIANT;

    delete iCurrentItem;
    iCurrentItem = NULL;
    delete iCurrentPbkItem;
    iCurrentPbkItem = NULL;
    iCmIter.Reset();

    __TEST_INVARIANT;
    }

EXPORT_C CPbkContactItem* CPbkContactIter::CurrentL() const
    {
    __TEST_INVARIANT;

    if (iCurrentItem)
        {
        if (!iCurrentPbkItem || &iCurrentPbkItem->ContactItem() != iCurrentItem)
            {
            // Create a new Phonebook contact from the cached contact item
            CPbkContactItem* item = 
                CPbkContactItem::NewL(iCurrentItem, iEngine.FieldsInfo(), 
                    iEngine.ContactNameFormat());

            // Delete previous Phonebook contact
            delete iCurrentPbkItem;
            // Assign the new Phonebook contact
            iCurrentPbkItem = item;
            // iCurrentPbkItem owns iCurrentItem now
            iCurrentItem = NULL;
            // PostCond
            __ASSERT_DEBUG(iCurrentPbkItem && !iCurrentItem, 
                Panic(EPanicPostCond_CurrentL));
            }
        }

    __TEST_INVARIANT;
    return iCurrentPbkItem;
    }

EXPORT_C CPbkContactItem* CPbkContactIter::GetCurrentL() const
    {
    __TEST_INVARIANT;

    CPbkContactItem* item = CurrentL();
    iCurrentPbkItem = NULL;
    iCurrentItem = NULL;

    __TEST_INVARIANT;
    return item;
    }

EXPORT_C void CPbkContactIter::__DbgTestInvariant() const
    {
    __ASSERT_DEBUG(
        !iCurrentItem && !iCurrentPbkItem || 
        iCurrentPbkItem && !iCurrentItem ||
        !iCurrentPbkItem && iCurrentItem,
        Panic(EPanicClassInvariant));
    }

/**
 * Reads aId to a CContactItem and returns it if it is a standard 
 * contact card.
 */
CContactItem* CPbkContactIter::ReadPhonebookContactL(TContactItemId aId)
    {
    CContactItem* item = NULL;
    if (iUseMinimalRead)
        {
        item = iEngine.Database().ReadMinimalContactL(aId);
        }
    else
        {
        item = iEngine.Database().ReadContactL(aId);
        }

    if (item->Type()==KUidContactCard && 
        !item->IsDeleted() && 
        !item->IsSystem() && 
        !item->IsHidden())
        {
        // Item's type was ok, return it
        return item;
        }
    else
        {
        // item was not a standard card, destroy it
        delete item;
        return NULL;
        }
    }


// End of File
