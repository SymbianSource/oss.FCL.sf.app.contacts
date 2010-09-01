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
*           Provides phonebook delete contacts command class methods.
*
*/


// INCLUDE FILES
#include "CPbkDeleteContactsCmd.h"
#include <Phonebook.rsg>
#include <eikprogi.h>
#include <CPbkProgressNoteWrapper.h>
#include <CPbkContactViewListControl.h>
#include <CPbkContactEngine.h>
#include <cntdb.h>

#include "CPbkAppUi.h"
#include <MPbkCommandObserver.h>

#include <pbkdebug.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ResetWhenDestroyed = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkDeleteContactsCmd");
    User::Panic(KPanicText,aReason);
    }
#endif

// LOCAL CONSTANTS
const TInt KB = 1024; // kB
const TInt PbkFreeSpaceForDelete = 8192; // 8kB

}


// ================= MEMBER FUNCTIONS =======================

inline CPbkDeleteContactsCmd::CPbkDeleteContactsCmd
        (CPbkContactEngine& aEngine,
        CPbkContactViewListControl& aUiControl) :
    CActive(EPriorityIdle),
    iEngine(aEngine),
    iUiControl(aUiControl),
    iFocusId(KNullContactId),
    iFreeSpaceRequiredToDelete(PbkFreeSpaceForDelete),  // 8 kB
    iDialogDismissed(ETrue) // there is no dialog at this point
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContactsCmd::CPbkDeleteContactsCmd(0x%x)"),
        this);

    CActiveScheduler::Add(this);
    }

inline TBool CPbkDeleteContactsCmd::MoreContactsToDelete() const
    {
    return (iContacts && iContacts->Count() > 0);
    }

inline TInt64 CPbkDeleteContactsCmd::FreeSpaceOnDbDrive() const
    {
    TVolumeInfo volInfo;
    volInfo.iFree = 0;
    iEngine.FsSession().Volume(volInfo,iDbDrive);
    return volInfo.iFree;
    }

inline void CPbkDeleteContactsCmd::ConstructL
        (const CContactIdArray& aContacts)
    {
    iContacts = CContactIdArray::NewL(&aContacts);
    iContacts->ReverseOrder();
    User::LeaveIfError(iSharedDataClient.Connect());
    TDriveUnit dbDrive;
    iEngine.Database().DatabaseDrive(dbDrive);
    iDbDrive = dbDrive;
    }

CPbkDeleteContactsCmd* CPbkDeleteContactsCmd::NewL
        (CPbkContactEngine& aEngine,
        const CContactIdArray& aContacts,
        CPbkContactViewListControl& aUiControl)
    {
    CPbkDeleteContactsCmd* self = new(ELeave)
        CPbkDeleteContactsCmd(aEngine, aUiControl);
    CleanupStack::PushL(self);
    self->ConstructL(aContacts);
    CleanupStack::Pop(self);
    return self;
    }

void CPbkDeleteContactsCmd::ResetWhenDestroyed
        (CPbkDeleteContactsCmd** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this,
        Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

CPbkDeleteContactsCmd::~CPbkDeleteContactsCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContactsCmd::~CPbkDeleteContactsCmd(0x%x)"),
        this);

    Cancel();
    if (static_cast<CPbkAppUi*>(CEikonEnv::Static()->AppUi())->
        IsRunningForeground())
        {
        // Unblank and redraw UI control if still visible
        iUiControl.SetBlank(EFalse);
        }

    DeleteProgressNote();
    iSharedDataClient.Close();
    delete iContacts;

    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    iUnderDestruction = ETrue;
    }

void CPbkDeleteContactsCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContactsCmd::ExecuteLD(0x%x)"),
        this);
    CleanupStack::PushL(this);

    // Calculate new focus
    iFocusId = KNullContactId;
    const TInt focusIndex = iUiControl.NextUnmarkedIndexFromFocus();
    if (focusIndex >= 0)
        {
        iFocusId = iUiControl.ContactIdAtL(focusIndex);
        }

    // Blank UI control to avoid flicker
    iUiControl.SetBlank(ETrue);

    // delete previous note
    DeleteProgressNote();

    // display progress note
    CAknProgressDialog* progressDialog = new(ELeave) CAknProgressDialog(
            reinterpret_cast<CEikDialog**>(NULL), ETrue);
    progressDialog->PrepareLC(R_QTN_PHOB_NOTE_CLEARING_PB);
    iProgressDlgInfo = progressDialog->GetProgressInfoL();
    iProgressDlgInfo->SetFinalValue(iContacts->Count());
    progressDialog->SetCallback(this);
    iDialogDismissed = EFalse;
    progressDialog->RunLD();

    iProgressDialog = progressDialog;

    // issue request for entry deletion
    IssueRequest();

    CleanupStack::Pop(this);
    }

void CPbkDeleteContactsCmd::AddObserver
        (MPbkCommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

void CPbkDeleteContactsCmd::DoCancel()
    {
    delete iContacts;
    iContacts = NULL;
    }

void CPbkDeleteContactsCmd::RunL()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContactsCmd::StepL()"),
        this);

    CContactDatabase& db = iEngine.Database();
    CompressIfRequired();
    if (MoreContactsToDelete())
        {
        const TInt sizeBefore = db.FileSize();        
        iSharedDataClient.RequestFreeDiskSpaceLC(iFreeSpaceRequiredToDelete);        
        const TInt index = iContacts->Count() - 1;
        const TContactItemId id = (*iContacts)[index];
        iContacts->Remove(index);

        db.DeleteContactL(id);
        ++iDeletedCount;
        CleanupStack::PopAndDestroy();  // RequestFreeDiskSpaceLC

        // Calculate how much database grew rounded to next kB
        TInt sizeDiff = db.FileSize() - sizeBefore + KB;
        sizeDiff -= sizeDiff % KB;
        if (sizeDiff > iFreeSpaceRequiredToDelete)
            {
            // Update maximum size required for deletion
            iFreeSpaceRequiredToDelete = sizeDiff;
            }

        // Incrementing progress of the process
        iProgressDlgInfo->SetAndDraw(iDeletedCount);

        // issue request to delete next item
        IssueRequest();
        }
    else
        {
        // Deletion process completed, all items have been deleted
        // show result and delete progress note
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkDeleteContactsCmd::RunL process completed start"));

        // process is completed, all entries have been copied
        FinishProcess();
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkDeleteContactsCmd::RunL process completed end"));
        }

    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContactsCmd::RunL end"));
    }

TInt CPbkDeleteContactsCmd::RunError
        (TInt aError)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkDeleteContactsCmd::HandleStepError(%d)"), aError);

    TInt result = aError;
    switch (aError)
        {
        case KErrNotFound:  // FALLTHROUGH
        case KErrInUse:     // FALLTHROUGH
            {
			// Ignore these errors
			// KErrNotFound means that somebody got the contact first
			// KErrInUse means that the contact is open
            result = KErrNone;
            break;
            }

        default:  // Something more serious happened -> give up
            {
            Cancel();
            FinishProcess();
            break;
            }
        }

    return result;
    }

void CPbkDeleteContactsCmd::FinishProcess()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContactsCmd::FinishProcess(0x%x)"),
        this);

    // Final compress
    CompressIfRequired();

    iUiControl.HandleMarkableListUpdateAfterCommandExecution();

    // Set the new focus
    if (iFocusId != KNullContactId)
        {
        TInt index = -1;
        TRAP_IGNORE(index = iUiControl.FindContactIdL(iFocusId));
        if (index >= 0)
            {
            iUiControl.SetCurrentItemIndex(index);
            }
        }

    // Unblank and redraw UI control
    iUiControl.SetBlank(EFalse);

    DeleteProgressNote();
    }

void CPbkDeleteContactsCmd::DeleteProgressNote()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkDeleteContactsCmd::DeleteProgressNote start"));

    if (iProgressDialog && !iDialogDismissed)
        {
        // deletes the dialog
        TRAPD(err, iProgressDialog->ProcessFinishedL());
        if (err != KErrNone)
            {
            delete iProgressDialog;
            }
        iProgressDialog = NULL;
        }

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkDeleteContactsCmd::DeleteProgressNote end"));
    }

void CPbkDeleteContactsCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbkDeleteContactsCmd::CompressIfRequired()
    {
    // Cancel any async compress first
    iEngine.CancelCompress();
    CContactDatabase& db = iEngine.Database();
    // Compress synchronously always if compression is required by the DB
    // or file system free space is below what is needed for deletion
    if (db.CompressRequired() ||
        FreeSpaceOnDbDrive() < iFreeSpaceRequiredToDelete)
        {
        TRAP_IGNORE(db.CompactL());
        }
    }

void CPbkDeleteContactsCmd::DialogDismissedL(TInt /*aButtonId*/)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkDeleteContactsCmd::DialogDismissedL"));
    
    // Cancel deleting process if it is still ongoing
    Cancel();
    iDialogDismissed = ETrue;
    
    if (static_cast<CPbkAppUi*>(CEikonEnv::Static()->AppUi())->
        IsRunningForeground())
        {
        // Unblank and redraw UI control if still visible
        iUiControl.SetBlank(EFalse);
        }
        
    // notify command owner that the command has finished
    iCommandObserver->CommandFinished(*this);
    }

//  End of File
