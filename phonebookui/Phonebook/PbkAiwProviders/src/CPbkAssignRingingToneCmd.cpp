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
*     Implements assign ringing tone functionality.
*
*/


// INCLUDE FILES
#include "CPbkAssignRingingToneCmd.h"

#include <aknnotedialog.h>
#include <StringLoader.h>
#include <PbkAiwProvidersRes.rsg>
#include <PbkView.rsg>
#include <CPbkContactEngine.h>
#include <AknNoteWrappers.h>
#include <AknWaitDialog.h>
#include <CPbkContactItem.h>
#include <MPbkCommandObserver.h>
#include <CPbkRingingToneFetch.h>
#include <AiwCommon.h>
#include <AiwCommon.hrh>
#include <CPbkFFSCheck.h>

#include <pbkdebug.h>


/// Unnamed namespace for local definitions
namespace {

// Utility to check file existence without BAFL
inline TBool FileExists( RFs& aFs, const TDesC& aFileName )
    {
    TEntry ignore;
    return (aFs.Entry(aFileName, ignore) == KErrNone);
    }

#ifdef _DEBUG
enum TPanicCode
    {
    ERunL_InvalidState
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkAssignRingingToneCmd");
    User::Panic(KPanicText, aReason);
    }

#endif  // _DEBUG
}

// ================= MEMBER FUNCTIONS =======================

CPbkAssignRingingToneCmd* CPbkAssignRingingToneCmd::NewL
        (const TDesC& aFileName, CContactIdArray* aContacts,
        CPbkContactEngine& aEngine,
        const CAiwGenericParamList& aInParamList,
        MAiwNotifyCallback* aNotifyCallback)
    {
    CPbkAssignRingingToneCmd* self = new(ELeave) CPbkAssignRingingToneCmd
        (aFileName, aContacts, aEngine, aInParamList, aNotifyCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkAssignRingingToneCmd::CPbkAssignRingingToneCmd
        (const TDesC& aFileName, CContactIdArray* aContacts,
        CPbkContactEngine& aEngine,
        const CAiwGenericParamList& aInParamList,
        MAiwNotifyCallback* aNotifyCallback):
        CActive(CActive::EPriorityIdle),
        iEngine(aEngine), iContacts(aContacts),
        iFileName(aFileName),
        iInParamList(aInParamList),
        iNotifyCallback(aNotifyCallback)
    {
    CActiveScheduler::Add(this);
    }

void CPbkAssignRingingToneCmd::ConstructL()
    {
    iPbkFFSCheck = CPbkFFSCheck::NewL();
    }

CPbkAssignRingingToneCmd::~CPbkAssignRingingToneCmd()
    {
    Cancel();
    delete iWaitNote;
    delete iItem;
    delete iContacts;
    delete iTitleForNote;
    delete iPbkFFSCheck;
    }


void CPbkAssignRingingToneCmd::AssignRingingToneL()
    {
    TInt count = iContacts->Count();

    delete iItem;
    iItem = NULL;

    // Read the last contact of the array. Array is zerobased,
    // therefore reduce one
    iItem = iEngine.OpenContactL((*iContacts)[count-1]);

    // Remove the last contact from the array
    iContacts->Remove(count-1);

    if (ConfirmAssignL(*iItem))
        {
        SetRingingToneL();
        IssueRequest();
        ShowWaitNoteL();
        }
    else
        {
        // Close contact and continue
        iEngine.CloseContactL(iItem->Id());
        IssueRequest();
        }
    }

/**
 * Returns ETrue, if it's ok to assign.
 */
inline TBool CPbkAssignRingingToneCmd::ConfirmAssignL(CPbkContactItem& aItem)
    {
    TBool ret = ETrue;

    // Check does the contact already have a ringing tone
    TPbkContactItemField* field =
        aItem.FindField(EPbkFieldIdPersonalRingingToneIndication);

    if (field && (field->StorageType() == KStorageTypeText))
        {
        // Check if the existing ringing tone file actually exists
        if ( FileExists(iEngine.FsSession(), field->Text()) )
            {
            DismissWaitNoteL();

            // Ask the user for confirmation
            HBufC* title = aItem.GetContactTitleL();
            CleanupStack::PushL(title);
            HBufC* prompt = StringLoader::LoadL(
                R_QTN_MG_CONTACT_RINGINGTONE_REPLACE, *title);
            CleanupStack::PopAndDestroy(title);
            CleanupStack::PushL(prompt);
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            CleanupStack::PushL(dlg);
            dlg->SetPromptL(*prompt);
            CleanupStack::Pop(); // dlg

            if(!dlg->ExecuteLD(R_PBKAIW_GENERAL_CONFIRMATION_QUERY))
                {
                ret = EFalse;
                }
            CleanupStack::PopAndDestroy(prompt);
            }
        }

    return ret;
    }

void CPbkAssignRingingToneCmd::SetRingingToneL()
    {

    // Check disk space before assigning.
    // If not enough memory then leaves with KErrDiskFull.
    iPbkFFSCheck->FFSClCheckL();

    CPbkRingingToneFetch* toneFetch = CPbkRingingToneFetch::NewL(iEngine);
    CleanupStack::PushL(toneFetch);
    toneFetch->SetRingingToneL(*iItem, iFileName);
    iEngine.CommitContactL(*iItem);
    CleanupStack::PopAndDestroy(toneFetch);


    // The title must be fetched and save here otherwise the contact
    // title displayed in ShowEndNotesL function may be wrong.
    HBufC* title = iItem->GetContactTitleL();
    delete iTitleForNote;
    iTitleForNote = title;

    ++iTonesAdded;
    }

void CPbkAssignRingingToneCmd::ShowEndNotesL()
    {
    DismissWaitNoteL();

    if (iTonesAdded > 0)
        {
        HBufC* prompt = NULL;

        // Select prompt
        if (iTonesAdded == 1)
            {
            prompt = StringLoader::LoadL(
                R_QTN_MG_CONTACT_RINGINGTONE_ADD_ONE, *iTitleForNote);
            CleanupStack::PushL(prompt);
            }
        else if (iTonesAdded > 1)
            {
            prompt = StringLoader::LoadLC(
                R_QTN_MG_CONTACT_RINGINGTONE_ADD_SEVERAL, iTonesAdded);
            }

        // Prepare the note
        CAknNoteWrapper * note = new (ELeave) CAknNoteWrapper;
        CleanupStack::PushL(note);
        note->SetTextL(*prompt);
        CleanupStack::Pop(note);
        CleanupStack::PopAndDestroy(prompt);

        // Show the note
        note->ExecuteLD(R_PBKAIW_GENERAL_NOTE);
        }

    // Destroy itself (observer performs deletion of this command)
    iCmdObserver->CommandFinished(*this);
    DoSendNotifyEventL(KAiwEventCompleted);
    }


void CPbkAssignRingingToneCmd::ShowWaitNoteL()
    {
    if (!iWaitNote)
        {
        CAknWaitDialog* waitNote = new(ELeave)
        CAknWaitDialog(reinterpret_cast<CEikDialog**>(&iWaitNote));
        waitNote->ExecuteLD(R_QTN_GEN_NOTE_PROCESSING);
        iWaitNote = waitNote;
        }
    }

void CPbkAssignRingingToneCmd::DismissWaitNoteL()
    {
    if (iWaitNote)
        {
        TRAPD(err, iWaitNote->ProcessFinishedL());
        if (err != KErrNone)
            {
            delete iWaitNote;
            iWaitNote = NULL;
            }
        }
    }

void CPbkAssignRingingToneCmd::ExecuteLD()
    {
    iState = EAssigning;
    IssueRequest();
    }

void CPbkAssignRingingToneCmd::AddObserver(MPbkCommandObserver& aObserver)
    {
    iCmdObserver = &aObserver;
    }

void CPbkAssignRingingToneCmd::RunL()
    {
    switch (iState)
        {
        case EAssigning:
            {
            if (iContacts->Count() > 0)
                {
                // Assign ringing tone to next contact
                AssignRingingToneL();
                iState = EAssigning;
                }
            else
                {
                IssueStopRequest(KErrNone);
                }
            break;
            }
        case EStopping:
            {
            // Show notes to user
            ShowEndNotesL();
            break;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(ERunL_InvalidState));
            break;
            }
        }
    }


void CPbkAssignRingingToneCmd::DoCancel()
    {
    // Safe to ignore this, notify is not so critical
    TRAP_IGNORE(DoSendNotifyEventL(KAiwEventCanceled));
    }

TInt CPbkAssignRingingToneCmd::RunError(TInt aError)
    {
    if (iItem)
        {
        // We can ignore the error since stopping anyways
        TRAP_IGNORE(iEngine.CloseContactL(iItem->Id()));
        }
    CCoeEnv::Static()->HandleError(aError);
    IssueStopRequest(aError);
    return KErrNone;
    }

void CPbkAssignRingingToneCmd::DoSendNotifyEventL(TInt aEvent)
    {
    if (iNotifyCallback)
        {
        CAiwGenericParamList* eventParamList = CAiwGenericParamList::NewL();
        CleanupStack::PushL(eventParamList);
        iNotifyCallback->HandleNotifyL(
            KAiwCmdAssign,
            aEvent,
            *eventParamList,
            iInParamList);
        CleanupStack::PopAndDestroy(eventParamList);
        }
    }

void CPbkAssignRingingToneCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbkAssignRingingToneCmd::IssueStopRequest(TInt /*aError*/)
    {
    iState = EStopping;
    if (!IsActive())
        {
        IssueRequest();
        }
    }


// End of File

