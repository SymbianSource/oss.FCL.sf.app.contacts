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
*       Provides methods for Assign ringing tone to group members process.
*
*/


// INCLUDE FILES
#include "CPbkSetToneToGroup.h"
#include <cntdef.h>
#include <cntitem.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>
#include <CPbkFieldInfo.h>
#include <CPbkFieldsInfo.h>

#include <PbkDebug.h>

// LOCAL CONSTANTS AND MACROS

/// The interval to test DB compression
const TInt KCompressionInterval = 10;


// ================= MEMBER FUNCTIONS =======================

inline TBool CPbkSetToneToGroup::MoreContactsToSet() const
    {
    return (iContacts && iContacts->Count() > 0);
    }

inline void CPbkSetToneToGroup::Cancel()
    {
    delete iContacts;
    iContacts = NULL;
    }

inline CPbkSetToneToGroup::CPbkSetToneToGroup
        (CPbkContactEngine& aEngine) :
    iEngine(aEngine)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroup::CPbkSetToneToGroup(0x%x)"), 
        this);
    }

inline void CPbkSetToneToGroup::ConstructL
        (const CContactIdArray& aContacts,
        const TDesC& aRingingToneName)
    {
    iRingingToneFile = aRingingToneName.AllocL();
    iTotalContactsToSet = aContacts.Count();
    iContacts = CContactIdArray::NewL(&aContacts);
    iContacts->ReverseOrder();
    }

EXPORT_C CPbkSetToneToGroup* CPbkSetToneToGroup::NewLC
        (CPbkContactEngine& aEngine,
        const CContactIdArray& aContacts,
        const TDesC& aRingingToneName)
    {
    CPbkSetToneToGroup* self = new(ELeave) CPbkSetToneToGroup(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aContacts, aRingingToneName);
    return self;
    }

CPbkSetToneToGroup::~CPbkSetToneToGroup()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroup::~CPbkSetToneToGroup(0x%x)"), 
        this);

    delete iContacts;
    delete iRingingToneFile;
    }

EXPORT_C const TDesC& CPbkSetToneToGroup::RingingToneFile() const
    {
    return *iRingingToneFile;
    }

void CPbkSetToneToGroup::StepL()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroup::StepL()"), 
        this);
    
    if (iSetCount % KCompressionInterval == 0)
        {
        iEngine.CheckFileSystemSpaceAndCompressL();
        }

    if (MoreContactsToSet())
        {
        const TInt index = iContacts->Count() - 1;
        const TContactItemId id = (*iContacts)[index];

        CPbkContactItem* item = iEngine.OpenContactLCX(id);
        SetRingingToneL(*item);
        iEngine.CommitContactL(*item);
        CleanupStack::PopAndDestroy(2); // item, lock

        iContacts->Remove(index);
        ++iSetCount;

        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("    set contact %d"), id);
        }
    }

TInt CPbkSetToneToGroup::TotalNumberOfSteps()
    {
    return iTotalContactsToSet;
    }

TBool CPbkSetToneToGroup::IsProcessDone() const
    {
    return !MoreContactsToSet();
    }

void CPbkSetToneToGroup::ProcessFinished()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroup::ProcessFinished()"), 
        this);

    Cancel();
    }

TInt CPbkSetToneToGroup::HandleStepError(TInt aError)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkSetToneToGroup::HandleStepError(%d)"), aError);

    if (aError != KErrNone)
        {
        Cancel();
        }

    return aError;
    }

void CPbkSetToneToGroup::ProcessCanceled()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSetToneToGroup::ProcessCanceled()"), 
        this);

    Cancel();
    }

void CPbkSetToneToGroup::SetRingingToneL
        (CPbkContactItem& aItem)
    {
    TPbkContactItemField* field = aItem.FindField(EPbkFieldIdPersonalRingingToneIndication);

    if (iRingingToneFile->Compare(KNullDesC) == 0)
        {
        // remove ringing tone field
        if (field)
            {
            aItem.RemoveField(aItem.FindFieldIndex(*field));
            }
        }
    else
        {
        // ringing tone selected
        if (field)
            {
            field->TextStorage()->SetTextL(*iRingingToneFile);
            }
        else
            {
            // create ringing tone field
            CPbkFieldInfo* fieldInfo = iEngine.FieldsInfo().Find(EPbkFieldIdPersonalRingingToneIndication);
            TPbkContactItemField& newField = aItem.AddFieldL(*fieldInfo);
            newField.TextStorage()->SetTextL(*iRingingToneFile);
            }
        }
    }

//  End of File  
