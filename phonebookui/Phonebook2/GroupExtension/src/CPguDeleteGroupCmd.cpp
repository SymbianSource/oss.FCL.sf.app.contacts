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
* Description:  Phonebook 2 Group UI Extension delete group command.
*
*/


// INCLUDE FILES
#include "CPguDeleteGroupCmd.h"

// Phonebook 2
#include <MPbk2ProcessDecorator.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <MPbk2CommandObserver.h>
#include <pbk2groupuires.rsg>
#include <pbk2commands.rsg>
#include <pbk2uicontrols.rsg>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2GeneralConfirmationQuery.h>
#include <CPbk2SortOrderManager.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactGroup.h>

// System includes
#include <AknQueryDialog.h>
#include <StringLoader.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstContact = 0;
const TInt KOneContact = 1;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ResetWhenDestroyed = 1,
    ERunL_InvalidState,
    VPbkSingleContactOperationComplete_Logic
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPguDeleteGroupCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::CPguDeleteGroupCmd
// --------------------------------------------------------------------------
//
inline CPguDeleteGroupCmd::CPguDeleteGroupCmd(
        MPbk2ContactUiControl& aUiControl ) :
    CActive( EPriorityIdle ),
    iUiControl( &aUiControl )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguDeleteGroupCmd::CPguDeleteGroupCmd(0x%x)"), this);

    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::~CPguDeleteGroupCmd
// --------------------------------------------------------------------------
//
CPguDeleteGroupCmd::~CPguDeleteGroupCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguDeleteGroupCmd::~CPguDeleteGroupCmd(0x%x)"), this);

    Cancel();
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    delete iDecorator;
    delete iContactLinkArray;
    delete iRetrieveOperation;
    delete iDeleteOperation;
    delete iContactGroup;
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::NewL
// --------------------------------------------------------------------------
//
CPguDeleteGroupCmd* CPguDeleteGroupCmd::NewL(
        MPbk2ContactUiControl& aUiControl )
    {
    CPguDeleteGroupCmd* self = new (ELeave) CPguDeleteGroupCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguDeleteGroupCmd::ConstructL()
    {
    iContactLinkArray = iUiControl->SelectedContactsOrFocusedContactL();

    TBool visibilityDelay = EFalse;    
    if ( iContactLinkArray && iContactLinkArray->Count() == KOneContact )
        {
        // When deleting only one contact, the progress dialog
        // is not (immediately) wanted
        visibilityDelay = ETrue;
        }
    iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL(
        R_QTN_PHOB_NOTE_CLEARING_PB, visibilityDelay );

    iDecorator->SetObserver( *this );
    iUiControl->RegisterCommand( this );
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguDeleteGroupCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL( this );

    if ( iContactLinkArray )
        {
        TInt count = iContactLinkArray->Count();
        if ( count > KOneContact )
            {
            // Move straight to confirm phase
            iState = EConfirming;
            IssueRequest();
            }
        else if ( count == KOneContact )
            {
            // If the array contains only one contact, we have to retrieve it
            // to get its name for the confirmation query
            iState = ERetrieving;
            IssueRequest();
            }
        }

    CleanupStack::Pop( this );
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::RunL
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::RunL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguDeleteGroupCmd::RunL() start"), this);

   switch ( iState )
        {
        case ERetrieving:
            {
            const MVPbkContactLink& link =
                iContactLinkArray->At( KFirstContact );
            RetrieveContactL( link );
            break;
            }
        case EConfirming:
            {
            ConfirmDeletionL();
            break;
            }
        case EStarting:
            {
            DoDeleteContactsL();
            break;
            }
        case EDeleting:
            {
            // Do nothing
            break;
            }
        case EStopping:
            {
            // decorator calls ProcessDismissed
            iDecorator->ProcessStopped();
            break;
            }
        case ECanceling:
            {
            ProcessDismissed( KErrCancel );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic(ERunL_InvalidState) );
            break;
            }
        }

    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPguDeleteGroupCmd::RunL end"));
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPguDeleteGroupCmd::RunError( TInt aError )
    {
    return FilterErrors( aError );
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        iContactGroup = aContact->Group();
        __ASSERT_DEBUG( iContactGroup,
            Panic(VPbkSingleContactOperationComplete_Logic) );

        // We now have a group and we can issue a confirmation
        iState = EConfirming;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& aOperation, TInt /*aError*/ )
    {
    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We cannot get the contact, so we have to
        // fail. We cannot continue, since this operation
        // was executed only in case of one contact.
        iDecorator->ProcessStopped();
        }
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::StepComplete
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::StepComplete
        ( MVPbkContactOperationBase& aOperation, TInt aStepSize )
    {
    if ( &aOperation == iDeleteOperation )
        {
        // Indicate process advancement to the decorator
        iDecorator->ProcessAdvance( aStepSize );
        }
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::StepFailed
// --------------------------------------------------------------------------
//
TBool CPguDeleteGroupCmd::StepFailed(
        MVPbkContactOperationBase& aOperation,
        TInt /*aStepSize*/, TInt aError )
    {
    TBool result = ETrue;
    if ( &aOperation == iDeleteOperation )
        {
        // Check whether the error is a serious one
        TInt err = FilterErrors( aError );
        if ( err != KErrNone )
            {
            Cancel();
            result = EFalse;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::OperationComplete
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::OperationComplete(
        MVPbkContactOperationBase& aOperation )
    {
    if ( &aOperation == iDeleteOperation )
        {
        delete iDeleteOperation;
        iDeleteOperation = NULL;
        IssueStopRequest( EStopping );
        }
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::ProcessDismissed( TInt aCancelCode )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguDeleteGroupCmd::ProcessDismissed()"));

    if ( aCancelCode != KErrCancel && iUiControl)
        {
        // Don't remove list markings if user canceled
        iUiControl->SetBlank( EFalse );
        iUiControl->UpdateAfterCommandExecution();
        }

    // Notify command owner that the command has finished
    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::RetrieveContactL
        (const MVPbkContactLink& aContactLink )
    {
    // Retrieve the actual store contact from the given link
    iRetrieveOperation = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().RetrieveContactL( aContactLink, *this );
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::FilterErrors
// --------------------------------------------------------------------------
//
inline TInt CPguDeleteGroupCmd::FilterErrors( TInt aErrorCode )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguDeleteGroupCmd::FilterErrors(%d) start"), aErrorCode);

    TInt result = aErrorCode;
    switch ( aErrorCode )
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
            // Decorator calls processdismissed
            iDecorator->ProcessStopped();
            break;
            }
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguDeleteGroupCmd::FilterErrors(%d) end"), result);

    return result;
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::IssueRequest()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();        
        }
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::IssueStopRequest
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::IssueStopRequest( TProcessState aState )
    {
    iState = aState;
    if ( !IsActive() )
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::ConfirmDeletionL
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::ConfirmDeletionL()
    {
    TInt queryResult = 0;
    MVPbkContactLinkArray* items = iContactGroup->ItemsContainedLC();

    // Show the query "Delete X” "( X is the group name ).
    CPbk2GeneralConfirmationQuery* query =
        CPbk2GeneralConfirmationQuery::NewL();
    queryResult = query->ExecuteLD(
        *iContactGroup, R_QTN_QUERY_COMMON_CONF_DELETE, MPbk2ContactNameFormatter::EPreserveAllOriginalSpaces );

    if ( queryResult )
        {
        // Continue with starting the deletion
        iState = EStarting;
        IssueRequest();
        }
    else
        {
        IssueStopRequest( ECanceling );
        }
    CleanupStack::PopAndDestroy(); // items
    }

// --------------------------------------------------------------------------
// CPguDeleteGroupCmd::DoDeleteContactsL
// --------------------------------------------------------------------------
//
void CPguDeleteGroupCmd::DoDeleteContactsL()
    {
    iDeleteOperation = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().DeleteContactsL( *iContactLinkArray, *this );

    iDecorator->ProcessStartedL( iContactLinkArray->Count() );

    if (iUiControl)
        {
        // Blank UI control to avoid flicker
        iUiControl->SetBlank( ETrue );
        }

    iState = EDeleting;
    IssueRequest();
    }

//  End of File
