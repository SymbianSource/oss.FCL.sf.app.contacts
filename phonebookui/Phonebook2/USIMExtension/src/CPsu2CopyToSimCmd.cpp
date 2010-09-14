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
* Description:  Phonebook 2 USIM UI Extension copy to SIM command.
*
*/


// INCLUDE FILES
#include "CPsu2CopyToSimCmd.h"

// Phonebook 2
#include "CPsu2CopyToSimFieldInfoArray.h"
#include "CPsu2SimContactProcessor.h"
#include "TPsu2SimCopyResults.h"
#include "CPsu2ContactCopyInspector.h"
#include <MPbk2ContactUiControl.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2CommandObserver.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <pbk2usimuires.rsg>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <pbk2commonui.rsg>

// Virtual Phonebook
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkFieldTypeRefsList.h>
#include <MVPbkContactStoreInfo.h>

// System includes
#include <avkon.hrh>
#include <exterror.h>
#include <coemain.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>

// debug
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KZero = 0;
const TInt KOne = 1;

enum TNextTask
    {
    EStartCommand,
    ERetrieveSourceContact,
    ECreateSimContacts,
    EStartCopy,
    ECopyContact,
    EShowResultsAndComplete,
    EComplete
    };

enum TTargetStoreStatus
	{
    EStoreNotOpen,
    EStoreOpen,
    EStoreNotAvailable
	};
#ifdef _DEBUG
enum TPanicCode
    {
    ECopyToSimMenuItemMustNotBeInMenu,
    EPreCond_CopyCurrentContactL,
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPsu2CopyToSimCmd");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::CPsu2CopyToSimCmd
// --------------------------------------------------------------------------
//
CPsu2CopyToSimCmd::CPsu2CopyToSimCmd( MPbk2ContactUiControl& aUiControl,
        MVPbkContactStore& aTargetStore ) :
    CActive( EPriorityStandard ),
    iUiControl( &aUiControl ),
    iTargetStore( aTargetStore ),
    iUpdateControl( ETrue ),
    iIsCopiedCompletely( ETrue ),
    iIsLastCopyFailedForSimIsFull( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::~CPsu2CopyToSimCmd
// --------------------------------------------------------------------------
//
CPsu2CopyToSimCmd::~CPsu2CopyToSimCmd()
    {   
    Cancel();
    delete iContactInspector;
    delete iDecorator;
    delete iSimContactProcessor;
    delete iCopyToSimTypeInfos;
    delete iContactRetriever;
    delete iLinkArray;
    delete iCurrentSourceContact;
    delete iCurrentSimContact;
    iSimContacts.ResetAndDestroy();
    // Deregister the observer to the target store.
    iTargetStore.Close( *this );

    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::NewL
// --------------------------------------------------------------------------
//
CPsu2CopyToSimCmd* CPsu2CopyToSimCmd::NewL(
        MPbk2ContactUiControl& aUiControl,
        MVPbkContactStore& aTargetStore)
    {
    CPsu2CopyToSimCmd* self =
        new( ELeave ) CPsu2CopyToSimCmd(aUiControl, aTargetStore);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();

    CActiveScheduler::Add(this);

    iLinkArray = CVPbkContactLinkArray::NewL();
    iContactInspector = CPsu2ContactCopyInspector::NewL
        ( iAppServices->NameFormatter() );

    // The command can be called from the generic Copy starter
    // or straight using Copy To SIM menu command
    // -> iUiControl can be name list control or
    // it can be a store list control
    // safe since reference in contructor
    iNameListUiControl = iUiControl->ParentControl();
    if (!iNameListUiControl)
        {
        iNameListUiControl = iUiControl;
        }

    iNameListUiControl->RegisterCommand( this );

    MVPbkContactLinkArray* linkArray =
        iNameListUiControl->SelectedContactsOrFocusedContactL();
    // There must be at least one item in the array, otherwise the menu
    // item shouldn't be there
    __ASSERT_DEBUG( linkArray && linkArray->Count() > 0,
        Panic( ECopyToSimMenuItemMustNotBeInMenu ) );

    CleanupDeletePushL( linkArray );
    // Copy links that belongs to the target store
    TVPbkContactStoreUriPtr targetUri = iTargetStore.StoreProperties().Uri();
    const TInt count = linkArray->Count();
    for (TInt i = 0; i < count; ++i)
        {
        TVPbkContactStoreUriPtr uri =
            (*linkArray)[i].ContactStore().StoreProperties().Uri();
        if ( targetUri.Compare(uri,
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) != 0)
            {
            iLinkArray->AppendL( (*linkArray)[i].CloneLC() );
            CleanupStack::Pop(); // created link
            }
        }
    CleanupStack::PopAndDestroy(); // linkArray

    // Create source to SIM field type mapping infos
    iCopyToSimTypeInfos = CPsu2CopyToSimFieldInfoArray::NewL(
        iAppServices->ContactManager().FieldTypes(),
        iAppServices->ContactManager().FsSession() );

    // Create a processor that knows how to build and split contacts
    iSimContactProcessor = CPsu2SimContactProcessor::NewL(
        iTargetStore,
        *iCopyToSimTypeInfos,
        iAppServices->NameFormatter(),
        iAppServices->ContactManager().FieldTypes(),
        iAppServices->ContactManager().FsSession() );

    iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL(
            R_PSU2_COPY_PROGRESS_NOTE, ETrue );
    iDecorator->SetObserver( *this );
    
    iTargetStoreStatus = EStoreNotOpen;
    // Register as an observer of the store, to make sure could get the store state
    // change notification.
    iTargetStore.OpenL( *this );
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::RunL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::RunL()
    {
    switch (iNextTask)
        {
        case EStartCommand:
            {
            StartL();
            break;
            }
        case ERetrieveSourceContact:
            {
            RetrieveSourceContactL();
            break;
            }
        case ECreateSimContacts:
            {
            CreateSimContactsL();
            break;
            }
        case EStartCopy:
            {
            StartCopyL();
            break;
            }
        case ECopyContact:
            {
            CopyCurrentContactL();
            break;
            }
        case EShowResultsAndComplete:
            {
            // If can't get the sim space here, and the former  
            // copy is failed, just show that sim is full 
            if ( (GetSimSpaceStatusL() == ESimSpaceUnknown) &&
                    iIsLastCopyFailedForSimIsFull )
                {
                ShowSimIsFullNoteL();
                }
            else
                {
                ShowResultsNoteL();    
                }
            
            CompleteL();
            break;
            }
        case EComplete: // FALLTHROUGH
        default:
            {
            CompleteL();
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPsu2CopyToSimCmd::RunError(TInt aError)
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPsu2CopyToSimCmd::RunError %d" ), aError );
    CCoeEnv::Static()->HandleError(aError);
    
    // If the copy operation is cancelled due to error occurred, update the 
    // ui control and reset the command pointer before it is destructed.
    if ( iNameListUiControl )
        {
        iNameListUiControl->UpdateAfterCommandExecution();
        }
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished(*this);
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::ExecuteLD()
    {
    iNextTask = EStartCommand;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iNameListUiControl == & aUiControl)
        {
        iUiControl = NULL;
        iNameListUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::StoreReady
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::StoreReady( MVPbkContactStore& /*aContactStore*/ )
	{
	iTargetStoreStatus = EStoreOpen;
	}

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::StoreUnavailable( MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
	{
	iTargetStoreStatus = EStoreNotAvailable;
	}

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::HandleStoreEventL( 
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/ )
	{
	// do nothing.
	}

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& /*aOperation*/,
    MVPbkStoreContact* aContact)
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPsu2CopyToSimCmd::VPbkSingleContactOperationComplete 0x%x" ),
        this );
    // This is called when a new source contact is loaded

    delete iCurrentSourceContact;
    iCurrentSourceContact = aContact;

    // Create SIM contacts from the source next
    iNextTask = ECreateSimContacts;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/, TInt aError)
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPsu2CopyToSimCmd::VPbkSingleContactOperationFailed 0x%x" ),
        this );
    // This is called when a new source contact loading failed
    iDecorator->ProcessAdvance(KOne);
    HandleError(aError);
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::ContactOperationCompleted
        (TContactOpResult /*aResult*/)
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPsu2CopyToSimCmd::ContactOperationCompleted 0x%x" ),
        this );
    // This called when one SIM contact has been copied
    // ( or deleted in additional number full case )
    iIsLastCopyFailedForSimIsFull = ETrue;
    if (iSimContacts.Count() == 0)
        {
        // The source contact has been successfully copied
        ++iCopied;
        MoveToNextContact();
        }
    else
        {
        // The source contact was splitted and all splitted sim contacts
        // are not yet copied
        CopyNextSimContact();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::ContactOperationFailed
    (TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/)
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPsu2CopyToSimCmd::ContactOperationFailed 0x%x ERROR: %d" ),
        this, aErrorCode );

    // This called when copying of a SIM contact failed
    TInt res = KErrNone;
    TInt currentSimContactDeleted( EFalse );
    iIsLastCopyFailedForSimIsFull = ETrue;
    if (iSimContactProcessor->HandleSimError(aErrorCode ))
        {
        // If the error was handled it means that the SIM contact
        // must be splitted and resulted contacts must be copied.
        TRAP(res, iSimContactProcessor->CreateFixedSimContactsL(
            *iCurrentSimContact, iSimContacts));

        if ( res == KErrNone && iIsCopiedCompletely )
            {
            iIsCopiedCompletely = !iSimContactProcessor->DetailsDropped();
            if ( iIsCopiedCompletely )
                {
                // Inspect that contact was copied completely
                // if sim contacts has created.
                if ( iIsCopiedCompletely && iSimContacts.Count() > 0 )
                    {
                    iIsCopiedCompletely = iContactInspector->IsCopiedCompletelyL
                        ( *iCurrentSimContact, iSimContacts );
                    }
                }
            }

        if ( res == KErrNone )
            {
            // Current contact is partially saved so delete it
            // to prevent duplicate data
            TRAP( res, iCurrentSimContact->DeleteL( *this ) );
            currentSimContactDeleted = ETrue;
            }
        }
    else
        {
        res = aErrorCode;
        }

    if (res != KErrNone)
        {
        // Stop command or move to next contact depending on the error
        HandleError(res);
        }
    else if (iSimContacts.Count() > 0)
        {
        // if delete contact is on, this will continue in
        // MVPbkContactObserver callback
        if ( !currentSimContactDeleted )
            {
            // There are still SIM contacts to copy
            CopyNextSimContact();
            }
        }
    else
        {
        // Current contact has been handled, move to next contact
        MoveToNextContact();
        }

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
    "CPsu2CopyToSimCmd::ContactOperationFailed 0x%x IsCopiedCompletely: %d" ),
    this, iIsCopiedCompletely );
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::ProcessDismissed(TInt aCancelCode)
    {
    if ( iNameListUiControl )
        {
        iNameListUiControl->SetBlank( EFalse );
        }

    if ( aCancelCode == KErrCancel || aCancelCode == EEikBidCancel )
        {
        Cancel();
        HandleCanceling();
        iNextTask = EShowResultsAndComplete;
        }
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::IssueRequest()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::StartL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::StartL()
    {
    if ( GetSimSpaceStatusL() == ESimSpaceIsFull )
        {
        ShowSimIsFullNoteL();
        iNextTask = EComplete;
        IssueRequest();
        }
    else
        {
        iCurrentIndex = 0;
        TInt contactCount = iLinkArray->Count();
        switch ( contactCount )
            {
            case KZero:
                {
                CompleteWithResults();
                break;
                }
            default:
                {
                iDecorator->ProcessStartedL( contactCount );
                iNextTask = ERetrieveSourceContact;
                IssueRequest();
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::HandleCanceling
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::HandleCanceling()
    {
    delete iContactRetriever;
    iContactRetriever = NULL;
    delete iCurrentSimContact;
    iCurrentSimContact = NULL;
    iSimContacts.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::RetrieveSourceContactL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::RetrieveSourceContactL()
    {
    if (iCurrentIndex >= iLinkArray->Count())
        {
        // End copying if all contacts have been read
        CompleteWithResults();
        }
    else
        {
        delete iContactRetriever;
        iContactRetriever = NULL;

        iContactRetriever = iAppServices->ContactManager().RetrieveContactL(
                (*iLinkArray)[iCurrentIndex], *this);
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::CreateSimContactsL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::CreateSimContactsL()
    {
    iSimContacts.ResetAndDestroy();
    iSimContactProcessor->CreateSimContactsL(
        *iCurrentSourceContact, iSimContacts );
    // Inspect that contact was copied completely
    // if sim contacts has created.
    if ( iIsCopiedCompletely && iSimContacts.Count() > 0 )
        {
        iIsCopiedCompletely = iContactInspector->IsCopiedCompletelyL
            ( *iCurrentSourceContact, iSimContacts );
        }
    if ( iSimContacts.Count() == 0 )
        {
        MoveToNextContact();
        }
    else
        {
        iNextTask = EStartCopy;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::StartCopyL()
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::StartCopyL()
    {
    if ( iNameListUiControl )
        {
        // Blank UI control to avoid names list view refreshing
        iNameListUiControl->SetBlank( ETrue );
        }

    CopyNextSimContact();
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::CopyCurrentContactL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::CopyCurrentContactL()
    {
    delete iCurrentSimContact;
    iCurrentSimContact = NextSimContact();
    
    __ASSERT_DEBUG(iCurrentSimContact, Panic(EPreCond_CopyCurrentContactL));
    // If the store is not open, just complete the copy process.
    if( iTargetStoreStatus != EStoreOpen )
    	{
    	CompleteWithResults();
    	}
    else
    	{
    	iCurrentSimContact->CommitL(*this);
    	}
    }
    
// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ShowResultsNoteL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::ShowResultsNoteL()
    {
    if (iLinkArray->Count() == KOne)
        {
        if (iCurrentSourceContact)
            {
            HBufC* title = iAppServices->NameFormatter().
                GetContactTitleL(iCurrentSourceContact->Fields(),
                    MPbk2ContactNameFormatter::EPreserveLeadingSpaces);
            CleanupStack::PushL(title);
            TPsu2SimCopyResults results(iCopied, *title);
            // Show partly copied note if necessary
            if ( !iIsCopiedCompletely )
                {
                results.ShowPartlyCopiedNoteL();
                }
            else
                {
                results.ShowNoteL();
                }
            CleanupStack::PopAndDestroy(title);
            }
        else
            {
            TPsu2SimCopyResults results;
            results.ShowErrorNoteL();
            }
        }
    else
        {
        TPsu2SimCopyResults results(iCopied, iLinkArray->Count());
        results.ShowNoteL();
        // Show partly copied note if necessary
        if ( !iIsCopiedCompletely )
            {
            results.ShowPartlyCopiedNoteL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::CompleteL
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::CompleteL()
    {
    if ( iUpdateControl  && iNameListUiControl )
        {
        iNameListUiControl->SetBlank( EFalse );
        // Reset find due to case that if contact was not really copied
        // (=the target contact had already all the same fields) then
        // UI control doesn't receive any events and find text is not
        // reset by the UI control.
        iNameListUiControl->ResetFindL();
        if ( iCurrentSourceContact )
            {
            // Maintain NamesList focus on the source contact
            iNameListUiControl->SetFocusedContactL( *iCurrentSourceContact );
            }
        // If the copy has been cancelled we don't want to update control.
        // Markings are not removed after cancel.
        iNameListUiControl->UpdateAfterCommandExecution();
        }
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished(*this);
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::NextSimContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPsu2CopyToSimCmd::NextSimContact()
    {
    if (iSimContacts.Count() > 0)
        {
        const TInt firstPos = 0;
        MVPbkStoreContact* contact = iSimContacts[firstPos];
        iSimContacts.Remove(firstPos);
        return contact;
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::HandleError
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::HandleError(TInt aError)
    {
    switch (aError)
        {
        // Errors that can be ignored
        case KErrGsm0707InvalidCharsInTextString:   // FALLTHROUGH
        case KErrGsm0707TextStringTooLong:          // FALLTHROUGH
        case KErrGsm0707DialStringTooLong:          // FALLTHROUGH
        case KErrGsmCCUnassignedNumber:
            {
            MoveToNextContact();
            break;
            }
        default:
            {
            // If the copy was failed for memory full
            if (aError == KErrGsm0707MemoryFull)
                {
                iIsLastCopyFailedForSimIsFull = ETrue;
                }
            CompleteWithResults();
            CCoeEnv::Static()->HandleError(aError);
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::MoveToNextContact
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::MoveToNextContact()
    {
    iDecorator->ProcessAdvance(KOne);
    ++iCurrentIndex;
    iNextTask = ERetrieveSourceContact;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::CopyNextSimContact
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::CopyNextSimContact()
    {
    CPsu2CopyToSimCmd::TSimSpaceStatus simSpaceStatus = ESimSpaceUnknown;

    // Check always before copying that there is space for a new contact
    TRAPD( res, simSpaceStatus = GetSimSpaceStatusL() )
    if ( res != KErrNone )
        {
        HandleError( res );
        }
    else
        {
        iNextTask = ECopyContact;
        if ( simSpaceStatus == ESimSpaceIsFull )
            {
            CompleteWithResults();
            }
        else
            {
            IssueRequest();
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::CompleteWithResults
// --------------------------------------------------------------------------
//
void CPsu2CopyToSimCmd::CompleteWithResults()
    {
    iNextTask = EShowResultsAndComplete;
    if ( iDecorator )
        {
        // This will cause a callback to ProcessDismissed
        iDecorator->ProcessStopped();
        }
    else
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ShowNoteIfSimIsFullL
// --------------------------------------------------------------------------
//
CPsu2CopyToSimCmd::TSimSpaceStatus CPsu2CopyToSimCmd::GetSimSpaceStatusL()
    {
    const MVPbkContactStoreInfo& info = iTargetStore.StoreInfo();
    
    if ( info.MaxNumberOfContactsL() <= 0 )
        {
        return ESimSpaceUnknown;
        }
    else if ( info.MaxNumberOfContactsL() == info.NumberOfContactsL() )
        {
        return ESimSpaceIsFull;
        }
    
    return ESimSpaceIsNotFull;
    }

// --------------------------------------------------------------------------
// CPsu2CopyToSimCmd::ShowSimIsFullNoteL
// --------------------------------------------------------------------------
//    
void CPsu2CopyToSimCmd::ShowSimIsFullNoteL()
    {
        const CPbk2StoreProperty* storeProperty =
            iAppServices->StoreProperties().FindProperty(
                    iTargetStore.StoreProperties().Uri() );

        // R_QTN_PHOB_NOTE_STORE_FULL is from Pbk2Commands and it's
        // safe to use because commands resources are loaded in core pbk2
        HBufC* fullText = StringLoader::LoadLC(
            R_QTN_PHOB_NOTE_STORE_FULL,
            storeProperty->StoreName() );

        CAknInformationNote* noteDlg =
            new(ELeave) CAknInformationNote( ETrue );
        noteDlg->ExecuteLD( *fullText );
        CleanupStack::PopAndDestroy( fullText );
    }

//  End of File
