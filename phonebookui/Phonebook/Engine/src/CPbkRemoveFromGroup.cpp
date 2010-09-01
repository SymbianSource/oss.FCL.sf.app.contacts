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
*		Removing multiple contact from a group process
*
*/


// INCLUDE FILES
#include "CPbkRemoveFromGroup.h"
#include <cntdef.h>
#include <cntitem.h>
#include "CPbkContactEngine.h"

#include <PbkDebug.h>

// LOCAL CONSTANTS AND MACROS

/// The interval to test DB compression
const TInt KCompressionInterval = 10;


// ================= MEMBER FUNCTIONS =======================

inline TBool CPbkRemoveFromGroup::MoreContactsToRemove() const
    {
    return (iContacts && iContacts->Count() > 0);
    }

inline void CPbkRemoveFromGroup::Cancel()
    {
    // CloseContactL cannot leave despite its name
    iEngine.CloseContactL(iGroupId);
    delete iGroup;
    iGroup = NULL;
    delete iContacts;
    iContacts = NULL;
    }

inline CPbkRemoveFromGroup::CPbkRemoveFromGroup
        (CPbkContactEngine& aEngine,
        TContactItemId aGroupId) :
    iEngine(aEngine),
    iGroupId(aGroupId)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroup::CPbkRemoveFromGroup(0x%x)"), 
        this);
    }

inline void CPbkRemoveFromGroup::ConstructL
        (const CContactIdArray& aContacts)
    {
    iTotalContactsToRemove = aContacts.Count();
    iContacts = CContactIdArray::NewL(&aContacts);
    iContacts->ReverseOrder();
    // Lock the group for the duration of this process
    iGroup = iEngine.OpenContactGroupL(iGroupId);
    }

EXPORT_C CPbkRemoveFromGroup* CPbkRemoveFromGroup::NewLC
        (CPbkContactEngine& aEngine,
        const CContactIdArray& aContacts,
        TContactItemId aGroupId)
    {
    CPbkRemoveFromGroup* self = new(ELeave) CPbkRemoveFromGroup(aEngine, aGroupId);
    CleanupStack::PushL(self);
    self->ConstructL(aContacts);
    return self;
    }

CPbkRemoveFromGroup::~CPbkRemoveFromGroup()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroup::~CPbkRemoveFromGroup(0x%x)"), 
        this);

    Cancel();
    }

void CPbkRemoveFromGroup::StepL()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroup::StepL()"), 
        this);
    
    if (iRemovedCount % KCompressionInterval == 0)
        {
        iEngine.CheckFileSystemSpaceAndCompressL();
        }

    if (MoreContactsToRemove())
        {
        const TInt index = iContacts->Count() - 1;
        const TContactItemId id = (*iContacts)[index];
        iEngine.RemoveContactFromGroupL(id, iGroupId);
        iContacts->Remove(index);
        ++iRemovedCount;

        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("    assigned contact %d"), id);
        }
    }

TInt CPbkRemoveFromGroup::TotalNumberOfSteps()
    {
    return iTotalContactsToRemove;
    }

TBool CPbkRemoveFromGroup::IsProcessDone() const
    {
    return !MoreContactsToRemove();
    }

void CPbkRemoveFromGroup::ProcessFinished()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroup::ProcessFinished()"), 
        this);
    
    Cancel();
    }

TInt CPbkRemoveFromGroup::HandleStepError(TInt aError)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkRemoveFromGroup::HandleStepError(%d)"), aError);

    Cancel();
    return aError;
    }

void CPbkRemoveFromGroup::ProcessCanceled()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroup::ProcessCanceled()"), 
        this);

    Cancel();
    }


//  End of File  
