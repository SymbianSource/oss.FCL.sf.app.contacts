/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 copy contacts command.
*
*/


// INCLUDE FILES
#include "CPbk2CopyContactsCmd.h"

// Phonebook 2
#include "TPbk2CopyContactsResults.h"
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <Pbk2Commands.rsg>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2DriveSpaceCheck.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactCopier.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactBookmark.h>

// System includes
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <coemain.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KOneSelected = 1;

enum TPbk2CopyContactsCmdState
    {
    EPbk2CopyContactsCmdInit,
    EPbk2CopyContactsCmdRetrieve,
    EPbk2CopyContactsCmdCopy,
    EPbk2CopyContactsCmdShowResults,
    EPbk2CopyContactsCmdComplete
    };

#ifdef _DEBUG
enum TPanicCode
    {
    EPreCond_ConstructL,
    EInvalidStoreSelection
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2CopyContactsCmd");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::CPbk2CopyContactsCmd
// --------------------------------------------------------------------------
//
CPbk2CopyContactsCmd::CPbk2CopyContactsCmd(
        MPbk2ContactUiControl& aUiControl ) :
    CActive( EPriorityStandard ),
    iStoreUiControl( &aUiControl ),
    iState( EPbk2CopyContactsCmdInit )
    {
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::~CPbk2CopyContactsCmd
// --------------------------------------------------------------------------
//
CPbk2CopyContactsCmd::~CPbk2CopyContactsCmd()
    {
    Cancel();
    
    if ( iNameListUiControl  )
        {
        iNameListUiControl->RegisterCommand( NULL );
        }    

    delete iFocusedContactBookmark;
    delete iCopiedContacts;
    delete iDecorator;
    delete iContactLinks;
    delete iCopyOperation;
    delete iCopier;
    delete iContact;
    delete iRetrieveOperation;
    delete iDriveSpaceCheck;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2CopyContactsCmd* CPbk2CopyContactsCmd::NewL(
        MPbk2ContactUiControl& aUiControl)
    {
    CPbk2CopyContactsCmd* self =
        new( ELeave ) CPbk2CopyContactsCmd(aUiControl);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();

    // safe since this is a contructor reference parameter
    iNameListUiControl = iStoreUiControl->ParentControl();
    if ( !iNameListUiControl ) 
        {
        iNameListUiControl = iStoreUiControl;
        }
    __ASSERT_DEBUG(iNameListUiControl, Panic(EPreCond_ConstructL));
   
    iNameListUiControl->RegisterCommand( this );
    
    CActiveScheduler::Add(this);

    iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL(
            R_GENERAL_COPY_PROGRESS_NOTE, ETrue);
    iDecorator->SetObserver(*this);
    iContactLinks = CVPbkContactLinkArray::NewL();
    iDriveSpaceCheck = CPbk2DriveSpaceCheck::NewL
        ( CCoeEnv::Static()->FsSession() );
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::RunL()
    {
    switch (iState)
        {
        case EPbk2CopyContactsCmdInit:
            {
            InitL();
            break;
            }
        case EPbk2CopyContactsCmdRetrieve:
            {
            RetrieveL();
            break;
            }
        case EPbk2CopyContactsCmdCopy:
            {
            StartCopyL();
            break;
            }
        case EPbk2CopyContactsCmdShowResults:
            {
            ShowResultsL();
            break;
            }
        default:
            {
            FinishCommandL();
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::DoCancel()
    {
    delete iCopyOperation;
    iCopyOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2CopyContactsCmd::RunError(TInt aError)
    {
    HandleError(aError);
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::ExecuteLD()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iNameListUiControl == &aUiControl)
        {
        iStoreUiControl = NULL;
        iNameListUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::StepComplete
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::StepComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aStepSize)
    {
    iCopiedSuccessfully += aStepSize;
    iDecorator->ProcessAdvance(aStepSize);
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::StepFailed
// --------------------------------------------------------------------------
//
TBool CPbk2CopyContactsCmd::StepFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aStepSize*/, TInt aError)
    {
    HandleError(aError);
    // Stop copy operation
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::OperationComplete
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::OperationComplete(
        MVPbkContactOperationBase& /*aOperation*/)
    {
    iDecorator->ProcessStopped();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    Cancel();
    delete iCopyOperation;
    iCopyOperation = NULL;
    iState = EPbk2CopyContactsCmdShowResults;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    iContact = aContact;
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    iState = EPbk2CopyContactsCmdCopy;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError)
    {
    HandleError(aError);
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::InitL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::InitL()
    {
    if (iStoreUiControl)
        {        
        // Get target store
        CArrayPtr<MVPbkContactStore>* selected =
                iStoreUiControl->SelectedContactStoresL();

        __ASSERT_DEBUG(selected && selected->Count() == KOneSelected,
            Panic(EInvalidStoreSelection));

        iTargetStore = (*selected)[0];
        delete selected;
    
        iDriveSpaceCheck->SetStore( iTargetStore->StoreProperties().Uri() );
        iDriveSpaceCheck->DriveSpaceCheckL();
        // Get selected contacts
        MVPbkContactLinkArray* links = 
            iNameListUiControl->SelectedContactsOrFocusedContactL();
        if ( links )
            {
            CleanupDeletePushL(links);
            // Create links that are not from target store
            CreateLinkArrayL(*links);
            CleanupStack::PopAndDestroy(); // links

            const TInt contactCount = iContactLinks->Count();
            if (contactCount == 0)
                {
                // Nothing to copy
                ShowSelectedContactsInfoNoteL();
                iState = EPbk2CopyContactsCmdComplete;
                }
            if (contactCount == KOneSelected)
                {
                iState = EPbk2CopyContactsCmdRetrieve;
                }
            else if (contactCount > KOneSelected)
                {
                iState = EPbk2CopyContactsCmdCopy;
                }
            }
        else
            {
            iState = EPbk2CopyContactsCmdComplete;
            }        
        }
    else
        {
        // UI control doenst exist anymore, end
        iState = EPbk2CopyContactsCmdComplete;
        }
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::RetrieveL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::RetrieveL()
    {
    iRetrieveOperation =
        iAppServices->
            ContactManager().RetrieveContactL((*iContactLinks)[0],
            *this );
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::StartCopyL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::StartCopyL()
    {
    if ( !iCopier )
        {
        iCopier = CVPbkContactCopier::NewL( iAppServices->ContactManager() );
        }

    if ( !iCopiedContacts )
        {
        iCopiedContacts = CVPbkContactLinkArray::NewL();
        }

    // Start copying

    if (iNameListUiControl)
        {
        // Blank UI control to avoid flicker
        iNameListUiControl->SetBlank( ETrue );
        
        // If find is NOT active then save focus. Copying will cause
        // the UI control to reset find text and focus. Otherwise the focus
        // must be kept over the copying process.
        if ( iNameListUiControl->FindTextL().Length() == 0 &&
             !iFocusedContactBookmark )
            {
            const MVPbkBaseContact* cnt = 
                    iNameListUiControl->FocusedContactL();
            if ( cnt )
                {
                iFocusedContactBookmark = cnt->CreateBookmarkLC();
                CleanupStack::Pop();
                }
            }
        }
        
    // Copy command doesn't need resulted contact links
    iCopiedContacts->ResetAndDestroy();
    iCopyOperation = iCopier->CopyContactsL(
        CVPbkContactCopier::EVPbkUsePlatformSpecificDuplicatePolicy,
        *iContactLinks, iTargetStore, *iCopiedContacts, *this );

    iDecorator->ProcessStartedL(iContactLinks->Count());
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::ShowResultsL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::ShowResultsL()
    {
    const TInt contactCount = iContactLinks->Count();
    if (iContact)
        {
        HBufC* title =
            iAppServices->NameFormatter().
                GetContactTitleL(
                    iContact->Fields(),
                    MPbk2ContactNameFormatter::EPreserveLeadingSpaces);
        CleanupStack::PushL(title);
        TPbk2CopyContactsResults results(iCopiedSuccessfully, *title);
        results.ShowNoteL();
        CleanupStack::PopAndDestroy(title);
        }
    else
        {
        TPbk2CopyContactsResults results(
            iCopiedSuccessfully, iContactLinks->Count());
        results.ShowNoteL();
        }
    iState = EPbk2CopyContactsCmdComplete;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::FinishCommandL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::FinishCommandL()
    {
    // After the command is done, wake up the UI control.
    if ( iNameListUiControl )
        {
        iNameListUiControl->SetBlank( EFalse );
        // Reset find due to case that if contact was not really copied
        // (=the target contact had already all the same fields) then
        // UI control doesn't receive any events and find text is not
        // reset by the UI control.
        iNameListUiControl->ResetFindL();
        if ( iFocusedContactBookmark )
            {
            // The focus must be set due to reason that when copying
            // causes a merge of contact data (=editing contact) the names
            // list UI control receives first 'removed' event that will set
            // focus to incorrect place.
            iNameListUiControl->SetFocusedContactL(
                    *iFocusedContactBookmark );    
            }

        iNameListUiControl->UpdateAfterCommandExecution();        
        }
    
    iCommandObserver->CommandFinished(*this);
    }
    
// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::CreateLinkArrayL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::CreateLinkArrayL
        ( MVPbkContactLinkArray& aSelection )
    {
    const TVPbkContactStoreUriPtr& targetUri =
        iTargetStore->StoreProperties().Uri();
    const TInt count = aSelection.Count();
    for (TInt i = 0; i < count; ++i)
        {
        TVPbkContactStoreUriPtr uri =
            aSelection[i].ContactStore().StoreProperties().Uri();
        if (targetUri.Compare(uri,
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) != 0)
            {
            iContactLinks->AppendL(aSelection[i].CloneLC());
            CleanupStack::Pop(); // created link
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::ShowSelectedContactsInfoNoteL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::ShowSelectedContactsInfoNoteL()
    {
    HBufC* prompt = StringLoader::LoadLC(R_QTN_PHOB_NOTE_NO_COPY_TO_PHONE);
    CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
    dlg->ExecuteLD(*prompt);
    CleanupStack::PopAndDestroy(prompt);
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsCmd::HandleError
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsCmd::HandleError(TInt aError)
    {
    if (iNameListUiControl)
        {
        iNameListUiControl->SetBlank( EFalse );
        iNameListUiControl->UpdateAfterCommandExecution(); 
        }
        
    if (aError != KErrNone)
        {
        // Copy operation is responsible to stop operation
        CCoeEnv::Static()->HandleError(aError);

        iDecorator->ProcessStopped();
        }
    iCommandObserver->CommandFinished(*this);
    }

//  End of File
