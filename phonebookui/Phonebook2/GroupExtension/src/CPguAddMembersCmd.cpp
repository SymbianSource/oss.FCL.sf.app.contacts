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
* Description:  Phonebook 2 add to group command object.
*
*/

// INCLUDE FILES
#include "CPguAddMembersCmd.h"

// Phonebook 2
#include <Pbk2GroupUIRes.rsg>
#include <Pbk2Commands.rsg>
#include <Pbk2UIControls.rsg>
#include <CPbk2FetchDlg.h>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2ContactRelocator.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <CPbk2ViewState.h>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>

// Virtual Phonebook
#include <CVPbkFilteredContactView.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>

// System includes
#include <coemain.h>
#include <eikenv.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KOneContact = 1;
const TInt KFirstElement = 0;
// This is used in progress bar tweaking
const TInt KCertainContactAmount = 3;

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_NewL = 1,
    EPanicPreCond_ExecuteLD,
    EPanicPreCond_RelocateContactsL,
    EPanicContactNotGroup,
    EInvalidView
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPguAddMembersCmd");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPguAddMembersCmd::CPguAddMembersCmd
// --------------------------------------------------------------------------
//
CPguAddMembersCmd::CPguAddMembersCmd
        ( MVPbkContactLink& aContactGroup,
          MPbk2ContactUiControl& aUiControl ) :
            CActive( CActive::EPriorityStandard ),
            iUiControl( &aUiControl ),
            iGroupLink( aContactGroup ),
            iAddedContactsCount( 0 )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPguAddMembersCmd::CPguAddMembersCmd(0x%x)"), this);

    CActiveScheduler::Add( this );

    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::~CPguAddMembersCmd
// --------------------------------------------------------------------------
//
CPguAddMembersCmd::~CPguAddMembersCmd()
    {
    Cancel();
    if ( iFilteredView )
        {
        iFilteredView->RemoveObserver( *this );
        delete iFilteredView;
        }
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }

    delete iRetrieveContactOperation;
    delete iRetrieveGroupOperation;
    delete iContactGroup;
    delete iEntriesToAdd;
    delete iEntriesToRelocate;
    delete iContactRelocator;
    delete iDecorator;
    delete iContactsInGroup;
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguAddMembersCmd::ConstructL()
    {
    iState = ERetrievingContact;
    }
// --------------------------------------------------------------------------
// CPguAddMembersCmd::NewLC
// --------------------------------------------------------------------------
//
CPguAddMembersCmd* CPguAddMembersCmd::NewLC
        ( MVPbkContactLink& aContactGroup,
          MPbk2ContactUiControl& aUiControl )
    {
    CPguAddMembersCmd* self = new ( ELeave ) CPguAddMembersCmd
        ( aContactGroup, aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ExecuteLD()
    {
    __ASSERT_DEBUG( iCommandObserver, Panic( EPanicPreCond_ExecuteLD ) );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPguAddMembersCmd::ExecuteLD(0x%x)"), this );

    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ResetUiControl
        ( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::RunL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::RunL()
    {
    switch( iState )
        {
        case ERetrievingContact:
            {
            iRetrieveGroupOperation = Phonebook2::Pbk2AppUi()->
                ApplicationServices().ContactManager().
                    RetrieveContactL( iGroupLink, *this );
            break;
            }
        case EHandleContactLockedEvent:
            {
            HandleContactLockedEventL();
            break;
            }
        case EShowingProgressNote:
            {
            ShowProgressNoteL();
            break;
            }
        case EAddingContactsToGroup:
            {
            AddContactsToGroupL();
            break;
            }
        case ECommitingTransaction:
            {
            CommitTransactionL();
            break;
            }
        case EStopping:
            {
            ProcessDismissed( KErrNone );
            break;
            }

        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPguAddMembersCmd::RunError( TInt aError )
    {
    if ( iState == EAddingContactsToGroup && aError == KErrInUse &&
         iEntriesToAdd && iEntriesToAdd->Count() > KOneContact )
        {    
        // Incase there was more than just one contact being added, ignore
        // the KErrInUse error and continue with the remaining contacts
        iState = EAddingContactsToGroup;
        IssueRequest();   
        }
    else if ( aError != KErrNone )
        {
        FinishCommand( aError );
        }

    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& aOperation,
          MVPbkStoreContact* aContact )
    {
    TInt error = KErrNone;

    if ( &aOperation == iRetrieveGroupOperation )
        {
        delete iRetrieveGroupOperation;
        iRetrieveGroupOperation = NULL;

        iContactGroup = aContact->Group();

        __ASSERT_DEBUG( iContactGroup, Panic( EPanicContactNotGroup ) );

        TRAP( error, iContactGroup->LockL( *this ) );
        }
    else if ( &aOperation == iRetrieveContactOperation )
        {
        TRAP( error, RelocateContactL( aContact ) );
        }

    if ( error != KErrNone )
        {
        FinishCommand( error );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    FinishCommand( aError );
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactOperationCompleted( TContactOpResult aResult )
    {
    if ( aResult.iOpCode == MVPbkContactObserver::EContactLock )
        {
        iState = EHandleContactLockedEvent;
        IssueRequest();
        }
    else if ( aResult.iOpCode == MVPbkContactObserver::EContactCommit )
        {
        FinishCommand( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode, TBool aErrorNotified )
    {
    if ( !aErrorNotified )
        {
        FinishCommand( aErrorCode );
        }
    else
        {
        FinishCommand( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactViewReady
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactViewReady
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactViewError
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactViewError
        ( MVPbkContactViewBase& PBK2_DEBUG_ONLY( aView ),
        TInt aError, TBool /*aErrorNotified*/ )
    {
    __ASSERT_DEBUG( iFilteredView == &aView, Panic( EInvalidView ) );

    FinishCommand( aError );
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::IsContactIncluded
//
// This function should be as efficient as possible because it's called
// as many times as there are contacts in all contacts view.
// --------------------------------------------------------------------------
//
TBool CPguAddMembersCmd::IsContactIncluded
        ( const MVPbkBaseContact& aContact )
    {
    TBool result = ETrue;
    TRAPD ( err,
        {
        // Contacts that already belongs to the group are NOT included
        // to the fetch dialog.
        if ( !iContactsInGroup )
            {
            iContactsInGroup = iContactGroup->ItemsContainedLC();
            CleanupStack::Pop(); // iContactsInGroup
            }

        const TInt count( iContactsInGroup->Count() );
        for ( TInt i(0); i < count; ++i)
            {
            if ( iContactsInGroup->At( i ).RefersTo( aContact ) )
                {
                result = EFalse;
                break;
                }
            }
        });

    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::AcceptFetchSelectionL
// --------------------------------------------------------------------------
//
MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection
    CPguAddMembersCmd::AcceptFetchSelectionL
        ( TInt /*aNumMarkedEntries*/,
          MVPbkContactLink& /*aLastSelection*/ )
    {
    return MPbk2FetchDlgObserver::EFetchYes;
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::FetchCompletedL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::FetchCompletedL
        ( MVPbkContactLinkArray* aMarkedEntries )
    {
    if ( aMarkedEntries )
        {
        PrepareFetchResultsL( aMarkedEntries );
        }
    else
        {
        FinishCommand( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::FetchCanceled
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::FetchCanceled()
    {
    FinishCommand( KErrNone );
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::FetchAborted
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::FetchAborted()
    {
    FinishCommand( KErrNone );
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::FetchOkToExit
// --------------------------------------------------------------------------
//
TBool CPguAddMembersCmd::FetchOkToExit()
    {
    return ETrue;
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactRelocatedL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactRelocatedL
        ( MVPbkStoreContact* aRelocatedContact )
    {
    MVPbkContactLink* contactLink = aRelocatedContact->CreateLinkLC();
    if ( contactLink )
        {
        if ( iEntriesToAdd )
            {
            CleanupStack::Pop(); // contactLink
            iEntriesToAdd->AppendL( contactLink );
            }
        else
            {
            CleanupStack::PopAndDestroy(); // contactLink
            }
        }

    delete aRelocatedContact;

    // Advance the relocation progress note
    if ( iDecorator )
        {
        iDecorator->ProcessAdvance( 1 );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactRelocationFailed
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactRelocationFailed
        ( TInt aReason, MVPbkStoreContact* aContact )
    {
    // Can't do much than just delete the contact we were given ownership,
    // it can not be added to the group
    delete aContact;

    if ( aReason == KErrCancel
        && iEntriesToAdd
        && iEntriesToAdd->Count() > 0 )
        {
        // Although user discards relocation we have to add other entries
        // to the group if them exists. First show the progress note.
        iState = EShowingProgressNote;
        IssueRequest();
        }
    else
        {
        FinishCommand( aReason );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ContactsRelocationFailed
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ContactsRelocationFailed
        ( TInt aReason, CVPbkContactLinkArray* aContacts )
    {
    // Can't do much than just delete the contacts we were given ownership,
    // they can not be added to the group
    delete aContacts;

    if ( aReason == KErrCancel
        && iEntriesToAdd
        && iEntriesToAdd->Count() > 0 )
        {
        // Although user discards relocation we have to add other entries
        // to the group if them exists. First show the progress note.
        iState = EShowingProgressNote;
        IssueRequest();
        }
    else
        {
        FinishCommand( aReason );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::RelocationProcessComplete
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::RelocationProcessComplete()
    {
    iState = EShowingProgressNote;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    if ( iUiControl )
        {
        iUiControl->UpdateAfterCommandExecution();
        }
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::HandleContactLockedEventL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::HandleContactLockedEventL()
    {
    delete iFilteredView;
    iFilteredView = NULL;
    iFilteredView = CVPbkFilteredContactView::NewL
        ( *Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().
          AllContactsViewL(), *this, *this,
          Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            FieldTypes() );

    DoLaunchFetchDialogL();
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::DoLaunchFetchDialogL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::DoLaunchFetchDialogL()
    {
    CPbk2FetchDlg::TParams params;
    params.iResId = R_PBK2_MULTIPLE_ENTRY_FETCH_NO_GROUPS_DLG;
    // Fetch dlg uses this view instead of AllNameslistView
    params.iNamesListView = iFilteredView;
    params.iFlags = CPbk2FetchDlg::EFetchMultiple;
    params.iNaviPaneId = R_PBK2_GROUP_MEMBER_FETCH_NAVILABEL;

    CPbk2FetchDlg* dlg = CPbk2FetchDlg::NewL( params, *this );
    // Fetch dialog is not waiting dialog, so we just launch it
    dlg->ExecuteLD();
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::PrepareFetchResultsL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::PrepareFetchResultsL
        ( MVPbkContactLinkArray* aMarkedEntries )
    {
    if ( aMarkedEntries )
        {
        const TInt count = aMarkedEntries->Count();

        delete iEntriesToAdd;
        iEntriesToAdd = NULL;
        iEntriesToAdd = CVPbkContactLinkArray::NewL();

        delete iEntriesToRelocate;
        iEntriesToRelocate = NULL;
        iEntriesToRelocate = CVPbkContactLinkArray::NewL();

        for ( TInt i = 0; i < count; ++ i )
            {
            MVPbkContactLink* contactLink =
                aMarkedEntries->At( i ).CloneLC();
            CleanupStack::Pop(); // contactLink

            TVPbkContactStoreUriPtr uri =
                    contactLink->ContactStore().StoreProperties().Uri();
            TVPbkContactStoreUriPtr phoneMemoryUri(
                    VPbkContactStoreUris::DefaultCntDbUri() );

            if ( uri.Compare( phoneMemoryUri,
                    TVPbkContactStoreUriPtr::EContactStoreUriStoreType )
                        == 0 )
                {
                // Phone memory contact can be added to group directly
                iEntriesToAdd->AppendL( contactLink );
                }
            else
                {
                // We must first relocate the contact
                iEntriesToRelocate->AppendL( contactLink );
                }
            }

        if ( iEntriesToRelocate->Count() > KOneContact )
            {
            RelocateContactsL();
            }
        else if ( iEntriesToRelocate->Count() == KOneContact )
            {
            // We'll have to retrieve the one single contact and
            // use single contact relocator
            delete iRetrieveContactOperation;
            iRetrieveContactOperation = NULL;
            iRetrieveContactOperation =
                Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ContactManager().RetrieveContactL(
                        iEntriesToRelocate->At( KFirstElement ), *this );
            }
        else
            {
            // Contacts will be added to group. First show
            // the progress note.
            iState = EShowingProgressNote;
            IssueRequest();
            }
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::RelocateContactL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::RelocateContactL( MVPbkStoreContact* aStoreContact )
    {
    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    // Asynchronously relocate the contact
    // One contact doesn't need waiting note
    iContactRelocator->RelocateContactL( aStoreContact, *this,
        Pbk2ContactRelocator::EPbk2DisplayBasicQuery,
        CPbk2ContactRelocator::EPbk2RelocatorExistingContact );
    }
// --------------------------------------------------------------------------
// CPguAddMembersCmd::RelocateContactsL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::RelocateContactsL()
    {
    __ASSERT_DEBUG( iEntriesToRelocate->Count() > KOneContact,
        Panic( EPanicPreCond_RelocateContactsL ) );

    TInt count = iEntriesToRelocate->Count();
    delete iContactRelocator;
    iContactRelocator = NULL;
    iContactRelocator = CPbk2ContactRelocator::NewL();

    // Asynchronously relocate contacts
    TBool result = iContactRelocator->RelocateContactsL( iEntriesToRelocate,
        *this, Pbk2ContactRelocator::EPbk2DisplayBasicQuery );
    iEntriesToRelocate = NULL;    // Ownership was taken
    if ( result )
        {
        // User has accepted relocation, show "Retrieving" progress note
        ShowRelocationNoteL( count );
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::AddContactsToGroupL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::AddContactsToGroupL()
    { 
    if (iEntriesToAdd && 
        iAddedContactsCount < iEntriesToAdd->Count() )
        { 
        const MVPbkContactLink& contactToAdd = iEntriesToAdd->At( iAddedContactsCount );
        iAddedContactsCount++; 
        
        // If leaves, RunError called
        iContactGroup->AddContactL( contactToAdd );
      
        if ( iDecorator )
            {
            iDecorator->ProcessAdvance( 1 );
            }     
        iState = EAddingContactsToGroup;
        }
    else
        {
        iState = ECommitingTransaction;
        }
    
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::CommitTransactionL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::CommitTransactionL()
    {
    iContactGroup->CommitL( *this );
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::FinishCommand
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::FinishCommand( TInt aResult )
    {
    if ( iUiControl )
        {
        iUiControl->SetBlank( EFalse );
        TRAPD(err, SetFocusToLastAddedL() );
        if ( KErrNone != err )
            {
            CCoeEnv::Static()->HandleError( aResult );
            }
        }

    if ( aResult != KErrNone && aResult != KErrCancel )
        {
        CCoeEnv::Static()->HandleError( aResult );
        }

    if ( iDecorator )
        {
        // iDecorator calls ProcessStopped to end this cmd
        iDecorator->ProcessStopped();
        }
    else
        {
        if ( iUiControl )
            {
            iUiControl->UpdateAfterCommandExecution();
            }
        if ( iCommandObserver )
            {
            iCommandObserver->CommandFinished( *this );
            }
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::ShowRelocationNoteL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ShowRelocationNoteL( TInt aCount )
    {
    if ( iUiControl )
        {
        iUiControl->SetBlank( ETrue );
        }

    if ( !iDecorator )
        {
        // Show relocation note only if there are enough contacts
        // You can't use here the TBool-delay switch,
        // it just doesn't progress
        if ( aCount > KCertainContactAmount )
            {
            iDecorator = Pbk2ProcessDecoratorFactory::
                CreateProgressDialogDecoratorL
                ( R_QTN_GEN_NOTE_RETRIEVING_PROGRESS, EFalse );
            iDecorator->SetObserver( *this );
            iDecorator->ProcessStartedL( aCount + 1 );
            }
        else
            {
            // There are few contacts, so omit the whole note
            iDecorator = Pbk2ProcessDecoratorFactory::
                CreateProgressDialogDecoratorL
                ( R_QTN_GEN_NOTE_RETRIEVING_PROGRESS, ETrue );
            iDecorator->SetObserver( *this );
            iDecorator->ProcessStartedL( 0 );
            }
        }
    }


// --------------------------------------------------------------------------
// CPguAddMembersCmd::ShowProgressNoteL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::ShowProgressNoteL()
    {
    if ( iUiControl )
        {
        iUiControl->SetBlank( ETrue );
        }

    if ( !iDecorator && iEntriesToAdd )
        {
        // Show relocation note only if there are enough contacts
        // You can't use here the TBool-delay switch, it just doesn't
        // progress
        if ( iEntriesToAdd->Count() > KCertainContactAmount )
            {
            iDecorator = Pbk2ProcessDecoratorFactory::
                CreateProgressDialogDecoratorL
                ( R_QTN_GEN_NOTE_RETRIEVING_PROGRESS, EFalse );
            iDecorator->SetObserver( *this );
            iDecorator->ProcessStartedL( iEntriesToAdd->Count() + 1 );
            }
        else
            {
            // There are few contacts, so omit the whole note
            iDecorator = Pbk2ProcessDecoratorFactory::
                CreateProgressDialogDecoratorL
                ( R_QTN_GEN_NOTE_RETRIEVING_PROGRESS, ETrue );
            iDecorator->SetObserver( *this );
            iDecorator->ProcessStartedL( 0 );
            }
        }
    iState = EAddingContactsToGroup;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::IssueStopRequest
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::IssueStopRequest()
    {
    iState = EStopping;
    if ( !IsActive() )
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPguAddMembersCmd::SetFocusToLastAddedL
// --------------------------------------------------------------------------
//
void CPguAddMembersCmd::SetFocusToLastAddedL()
    {
    if ( iUiControl && iEntriesToAdd && iEntriesToAdd->Count() )
        {
        // Sets the focus to last added contactlink
        TInt focusedIndex( iEntriesToAdd->Count() - 1 );
        CPbk2ViewState* viewState = iUiControl->ControlStateL();
        CleanupStack::PushL( viewState );

        viewState->SetFocusedContact(
            iEntriesToAdd->At( focusedIndex ).CloneLC() );
        CleanupStack::Pop(); // iEntriesToAdd->At(focusedIndex).CloneLC

        iUiControl->RestoreControlStateL( viewState );
        CleanupStack::PopAndDestroy();  // viewState
        }
    }

// End of File
