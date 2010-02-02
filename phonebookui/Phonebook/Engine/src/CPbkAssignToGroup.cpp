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
*        Assign multiple contact to a group process.
*
*/


// INCLUDE FILES
#include "CPbkAssignToGroup.h"
#include <cntdef.h>
#include <cntitem.h>
#include "CPbkContactEngine.h"

#include <PbkDebug.h>

// LOCAL CONSTANTS AND MACROS

/// The interval to test DB compression
const TInt KCompressionInterval = 10;


// ================= MEMBER FUNCTIONS =======================

inline TBool CPbkAssignToGroup::MoreContactsToAssign() const
    {
    return (iContacts && iContacts->Count() > 0);
    }

inline void CPbkAssignToGroup::Cancel()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroup::Cancel(0x%x)"), 
        this);

    // CloseContactL cannot leave despite its name
    iEngine.CloseContactL(iGroupId);
    delete iGroup;
    iGroup = NULL;
    delete iContacts;
    iContacts = NULL;
    }

inline CPbkAssignToGroup::CPbkAssignToGroup
        (CPbkContactEngine& aEngine,
        TContactItemId aGroupId) :
    iEngine(aEngine),
    iGroupId(aGroupId)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroup::CPbkAssignToGroup(0x%x)"), 
        this);
    }

inline void CPbkAssignToGroup::ConstructL
        (const CContactIdArray& aContacts)
    {
    iTotalContactsToAssign = aContacts.Count();
    iContacts = CContactIdArray::NewL(&aContacts);
    iContacts->ReverseOrder();
    // Lock the group for the duration of this process
    iGroup = iEngine.OpenContactGroupL(iGroupId);
    }

EXPORT_C CPbkAssignToGroup* CPbkAssignToGroup::NewLC
        (CPbkContactEngine& aEngine,
        const CContactIdArray& aContacts,
        TContactItemId aGroupId)
    {
    CPbkAssignToGroup* self = new(ELeave) CPbkAssignToGroup(aEngine, aGroupId);
    CleanupStack::PushL(self);
    self->ConstructL(aContacts);
    return self;
    }

CPbkAssignToGroup::~CPbkAssignToGroup()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroup::~CPbkAssignToGroup(0x%x)"), 
        this);

    Cancel();
    }

void CPbkAssignToGroup::StepL()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroup::StepL()"), 
        this);
    
    if (iAssignedCount % KCompressionInterval == 0)
        {
        iEngine.CheckFileSystemSpaceAndCompressL();
        }

    if (MoreContactsToAssign())
        {
        const TInt index = iContacts->Count() - 1;
        const TContactItemId id = (*iContacts)[index];
        iEngine.AddContactToGroupL(id, iGroupId);
        iContacts->Remove(index);
        ++iAssignedCount;

        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("    assigned contact %d"), id);
        }
    }

TInt CPbkAssignToGroup::TotalNumberOfSteps()
    {
    return iTotalContactsToAssign;
    }

TBool CPbkAssignToGroup::IsProcessDone() const
    {
    return !MoreContactsToAssign();
    }

void CPbkAssignToGroup::ProcessFinished()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroup::ProcessFinished()"), 
        this);

    Cancel();
    }

TInt CPbkAssignToGroup::HandleStepError(TInt aError)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkAssignToGroup::HandleStepError(%d)"), aError);

    Cancel();
    return aError;
    }

void CPbkAssignToGroup::ProcessCanceled()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkAssignToGroup::ProcessCanceled()"), 
        this);

    Cancel();
    }


//  End of File  
