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
*     Implements assign image functionality.
*
*/


// INCLUDE FILES
#include "CPbkAssignImageCmd.h"

#include <aknnotedialog.h>
#include <StringLoader.h>
#include <PbkAiwProvidersRes.rsg>
#include <PbkView.rsg>
#include <CPbkContactEngine.h>
#include <AknNoteWrappers.h>
#include <AknWaitDialog.h>
#include <CPbkContactItem.h>
#include <CPbkThumbnailManager.h>
#include <CPbkDrmManager.h>
#include <MPbkCommandObserver.h>
#include <AiwCommon.h>
#include <AiwCommon.hrh>
#include <CPbkFFSCheck.h>

#include <pbkdebug.h>


/// Unnamed namespace for local definitions
namespace {


#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_PbkImageSetComplete,
    EPanicPreCond_PbkImageSetFailed,
    EPanicPreCond_PbkImageGetComplete,
    EPanicPreCond_PbkImageGetFailed,
    ERunL_InvalidState
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkAssignImageCmd");
    User::Panic(KPanicText, aReason);
    }

#endif  // _DEBUG
}

// ================= MEMBER FUNCTIONS =======================

CPbkAssignImageCmd* CPbkAssignImageCmd::NewL
        (const TDesC& aFileName, CContactIdArray* aContacts,
        TPbkFieldId aFieldId, CPbkContactEngine& aEngine,
        const CAiwGenericParamList& aInParamList,
        MAiwNotifyCallback* aNotifyCallback)
    {
	CPbkAssignImageCmd* self = new(ELeave) CPbkAssignImageCmd
        (aFileName, aContacts, aFieldId, aEngine,
        aInParamList, aNotifyCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

CPbkAssignImageCmd::CPbkAssignImageCmd
        (const TDesC& aFileName, CContactIdArray* aContacts,
        TPbkFieldId aFieldId, CPbkContactEngine& aEngine,
        const CAiwGenericParamList& aInParamList,
        MAiwNotifyCallback* aNotifyCallback) :
        CActive(CActive::EPriorityIdle),
        iEngine(aEngine), iContacts(aContacts),
        iFieldId(aFieldId), iFileName(aFileName),
        iInParamList(aInParamList),
        iNotifyCallback(aNotifyCallback)
    {
    CActiveScheduler::Add(this);
    }

void CPbkAssignImageCmd::ConstructL()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkAssignImageCmd::ConstructL start"));

    iImageManager = CPbkThumbnailManager::NewL(iEngine);
    iPbkFFSCheck = CPbkFFSCheck::NewL();
    
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkAssignImageCmd::ConstructL end"));
    }

CPbkAssignImageCmd::~CPbkAssignImageCmd()
    {
    Cancel();
    delete iWaitNote;
    delete iItem;
    delete iImageOperation;
    delete iImageManager;
    delete iContacts;
    delete iBitmap;
    delete iTitleForNote;
    delete iPbkFFSCheck;
    }


void CPbkAssignImageCmd::AssignImageL()
    {    
    TInt count = iContacts->Count();

    delete iItem;
    iItem = NULL;
    
    delete iBitmap;
    iBitmap = NULL;

    // Read the last contact of the array. Array is zerobased,
    // therefore reduce one
    iItem = iEngine.OpenContactL((*iContacts)[count-1]);
    
    // Remove the last contact from the array
    iContacts->Remove(count-1);

	if (ConfirmAssignL(*iItem, iFieldId))
		{		
        SetImageAsyncL();
        ShowWaitNoteL();
		}
	else
		{
        // Close contact and continue
        iEngine.CloseContactL(iItem->Id());
        IssueRequest();
		}
    }
    
inline TBool CPbkAssignImageCmd::ConfirmAssignL
        (CPbkContactItem& aItem, TPbkFieldId /*aId*/)
	{
	TBool ret = ETrue;
	
    // Check does the contact already have a image
    if (iImageManager->HasThumbnail(aItem))
        {
        DismissWaitNoteL();

        // Ask the user for confirmation
        HBufC* title = aItem.GetContactTitleL();
        CleanupStack::PushL(title);
        HBufC* prompt = StringLoader::LoadL(R_QTN_PHOB_FETCH_IMG_REPLACE,
            *title);
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
	
	return ret;
	}

void CPbkAssignImageCmd::SetImageAsyncL()
    {    
    // Set image to contact
    delete iImageOperation;
    iImageOperation = NULL;
    
    // Check disk space before assigning. 
    // If not enough memory then leaves with KErrDiskFull.
    iPbkFFSCheck->FFSClCheckL();
    
    if (!iBitmap)
        {
        iImageOperation = iImageManager->SetThumbnailAsyncL
            (*this, *iItem, iFileName);
        }
    else
        {
        iImageOperation = iImageManager->SetThumbnailAsyncL
            (*this, *iItem, *iBitmap);
        }
    }

void CPbkAssignImageCmd::ShowEndNotesL()
    {
    DismissWaitNoteL();

    if (iImagesAdded > 0)
        {
        HBufC* prompt = NULL;

        // Select prompt
        if (iImagesAdded == 1)
            {
            prompt = StringLoader::LoadL(R_QTN_PHOB_FETCH_NOTE_IMG_ADD_ONE,
                *iTitleForNote);
            CleanupStack::PushL(prompt);
            }
        else if (iImagesAdded > 1)
            {
            prompt = StringLoader::LoadLC(R_QTN_PHOB_FETCH_NOTE_IMG_ADD_SEV,
                iImagesAdded);
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

void CPbkAssignImageCmd::FetchImageL()
    {
    // If we are to continue the image set process
    // get the image from the previous item,
    // so there's no need to perform scale and other
    // operations for every contact
    delete iImageOperation;
    iImageOperation = NULL;
    iImageOperation = iImageManager->GetThumbnailAsyncL
        (*this, *iItem);
    }

void CPbkAssignImageCmd::PbkThumbnailSetComplete
        (MPbkThumbnailOperation& 
        #ifdef _DEBUG
        aOperation
        #endif
        )
    {
    __ASSERT_DEBUG(&aOperation == iImageOperation, 
        Panic(EPanicPreCond_PbkImageSetComplete));

    // Commit changes
    TRAPD(err, iEngine.CommitContactL(*iItem, ETrue));
    delete iImageOperation;
    iImageOperation = NULL;        
    if (err == KErrNone)
        {
        // Save contact title for note
        HBufC* title = NULL;        
        TRAP(err, title = iItem->GetContactTitleL());        
        if (err == KErrNone)
            {
            delete iTitleForNote;
            iTitleForNote = title;
            // Increase the count only if commit and title name
            // fetch succeeded
            ++iImagesAdded;    
            }
        else
            {
            CCoeEnv::Static()->HandleError(err);
            }        
        }
    else
        {
        CCoeEnv::Static()->HandleError(err);
        }
    
    // Continue
    IssueRequest();
    }

void CPbkAssignImageCmd::PbkThumbnailSetFailed
        (MPbkThumbnailOperation& 
        #ifdef _DEBUG
        aOperation
        #endif
        , TInt aError)
    {
    __ASSERT_DEBUG(&aOperation == iImageOperation, 
        Panic(EPanicPreCond_PbkImageSetFailed));

    delete iImageOperation;
    iImageOperation = NULL;

    // We can ignore the error, report only the
    // original error to user
    TRAP_IGNORE(iEngine.CloseContactL(iItem->Id()));

    // Report the failure to the user
    CCoeEnv::Static()->HandleError(aError);

    // Continue
    IssueRequest();
    }


void CPbkAssignImageCmd::PbkThumbnailGetComplete
        (MPbkThumbnailOperation& 
        #ifdef _DEBUG
        aOperation
        #endif
        , CFbsBitmap* aBitmap)
    {
    __ASSERT_DEBUG(&aOperation == iImageOperation, 
        Panic(EPanicPreCond_PbkImageSetComplete));

    // Store the bitmap
    iBitmap = aBitmap;

    // Continue
    IssueRequest();
    }

void CPbkAssignImageCmd::PbkThumbnailGetFailed
        (MPbkThumbnailOperation& 
        #ifdef _DEBUG
        aOperation
        #endif
        , TInt /*aError*/)
    {
    __ASSERT_DEBUG(&aOperation == iImageOperation, 
        Panic(EPanicPreCond_PbkImageGetFailed));

    // Continue
    IssueRequest();
    }


void CPbkAssignImageCmd::ShowWaitNoteL()
    {
    if (!iWaitNote)
        {
        CAknWaitDialog* waitNote = new(ELeave)
        CAknWaitDialog(reinterpret_cast<CEikDialog**>(&iWaitNote));
        waitNote->ExecuteLD(R_QTN_GEN_NOTE_PROCESSING);
        iWaitNote = waitNote;
        }
    }

void CPbkAssignImageCmd::DismissWaitNoteL()
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

void CPbkAssignImageCmd::ExecuteLD()
    {
    CleanupStack::PushL(this);
    
    // Check is the image DRM protected
    CPbkDrmManager* drmManager = CPbkDrmManager::NewL();
    CleanupStack::PushL(drmManager);
    if (drmManager->IsProtectedFile(iFileName))
        {
        // Show user copyright note
        HBufC* prompt = 
            CCoeEnv::Static()->AllocReadResourceLC(R_QTN_DRM_NOT_ALLOWED);
        CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
        dlg->ExecuteLD(*prompt);
        CleanupStack::PopAndDestroy(); // prompt
        IssueStopRequest(KErrCancel);
        }
    else
        {        
        iState = EAssigningFirst;
        IssueRequest();
        }
    CleanupStack::PopAndDestroy(drmManager);
    CleanupStack::Pop(this);
    }

void CPbkAssignImageCmd::AddObserver(MPbkCommandObserver& aObserver)
    {
    iCmdObserver = &aObserver;
    }

void CPbkAssignImageCmd::RunL()
    {
    switch (iState)
        {
        case EAssigningFirst:
            {                        
            AssignImageL();

            if (iContacts->Count() > 0)
                {
                // Fetch the assigned image for the next contacts, if any
                iState = EFetching;
                }
            else
                {
                // Only one contact was selected, stop
                iState = EStopping;
                }
            break;
            }
        case EFetching:
            {
            FetchImageL();

            iState = EAssigning;
            break;
            }
        case EAssigning:
            {
            if (iContacts->Count() > 0)
                {
                // Assign image to next contact
                AssignImageL();
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


void CPbkAssignImageCmd::DoCancel()
    {
    // Safe to ignore this, notify is not so critical
    TRAP_IGNORE(DoSendNotifyEventL(KAiwEventCanceled));
    }
    

TInt CPbkAssignImageCmd::RunError(TInt aError)
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

void CPbkAssignImageCmd::DoSendNotifyEventL(TInt aEvent)
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

void CPbkAssignImageCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbkAssignImageCmd::IssueStopRequest(TInt /*aError*/)
    {
    iState = EStopping;
    if (!IsActive())
        {
        IssueRequest();
        }
    }

// End of File

