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
* Description:  Phonebook 2 delete contacts command.
*
*/


// INCLUDE FILES
#include "CPbk2DeleteContactsCmd.h"

// Phonebook2
#include <MPbk2CommandObserver.h>
#include "MPbk2ProcessDecorator.h"
#include "Pbk2ProcessDecoratorFactory.h"
#include <Pbk2Commands.rsg>
#include <Pbk2UIControls.rsg>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2GeneralConfirmationQuery.h>
#include <MPbk2ContactLinkIterator.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactLinkArray.h>

// System includes
#include <AknQueryDialog.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KOneContact = 1;

// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ResetWhenDestroyed = 1,
    ERunL_InvalidState,
    EPanicLogic_DoDeleteContactsL,
    EPanicPreCond_Null_Pointer
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2DeleteContactsCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

}

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::CPbk2DeleteContactsCmd
// --------------------------------------------------------------------------
//
inline CPbk2DeleteContactsCmd::CPbk2DeleteContactsCmd(
        MPbk2ContactUiControl& aUiControl ) :
    CActive( EPriorityIdle ),
    iUiControl( &aUiControl )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteContactsCmd::CPbk2DeleteContactsCmd(0x%x)"), this);

    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::~CPbk2DeleteContactsCmd
// --------------------------------------------------------------------------
//
CPbk2DeleteContactsCmd::~CPbk2DeleteContactsCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteContactsCmd::~CPbk2DeleteContactsCmd(0x%x)"), this);

    Cancel();
    if (iUiControl)
        {
        iUiControl->RegisterCommand(NULL);
        }
    delete iDecorator;
    delete iContactIterator;
    delete iContactsToDelete;

    delete iRetrieveOperation;
    delete iDeleteOperation;
    delete iListContactsOperation;
    delete iStoreContact;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2DeleteContactsCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();

    // register command to ui control
    iUiControl->RegisterCommand(this);

    // Returns NULL if there are no selected contacts.
    // In that case we will delete the focused contact.
    iContactIterator = iUiControl->SelectedContactsIteratorL();

    iContactCount = GetContactCountL();

    if( iContactCount > 1 )
    	{
    	//we are deleting multiple contacts
        iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL
            ( R_QTN_PHOB_NOTE_CLEARING_PB_WITH_CANCEL, EFalse );

        iDecorator->SetObserver(*this);
    	}
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2DeleteContactsCmd* CPbk2DeleteContactsCmd::NewL(
        MPbk2ContactUiControl& aUiControl )
    {
    CPbk2DeleteContactsCmd* self = new(ELeave) CPbk2DeleteContactsCmd(
            aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteContactsCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL(this);

    if ( iContactCount > KOneContact )
        {
        // Move straight to confirm phase
        iState = EConfirming;
        IssueRequest();
        }
    else if ( iContactCount == KOneContact )
        {
        // There was only one contact, we have to retrieve it to get
        // its name
        iState = ERetrieving;
        IssueRequest();
        }
    else 
        {
        // If iContactCount == 0 we should end this command properly.
        // This happens when contact to be deleted is from read-only store.
        ProcessDismissed( KErrNone );
        }        

    CleanupStack::Pop( this );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::AddObserver(
        MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::RunL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteContactsCmd::RunL() start"), this);

   switch ( iState )
        {
        case ERetrieving:
            {
            // Retrieve phase is executed only when there is one contact
            // to delete. Check is the contact marked one or focused one.            
            if ( iContactIterator )
                {
                iContactIterator->SetToFirst();
                if ( iContactIterator->HasNext() )
                    {
                    MVPbkContactLink* link = iContactIterator->NextL();
                    CleanupDeletePushL( link );
                    if ( link )
                        {
                        RetrieveContactL( *link );
                        }                    
                    CleanupStack::PopAndDestroy(); // link
                    }                   
                }
            else
                {
                if (iUiControl)
                    {
                    const MVPbkBaseContact* focusedContact =
                            iUiControl->FocusedContactL();
                    if ( focusedContact )
                        {
                        MVPbkContactLink* link = focusedContact->CreateLinkLC();
                        RetrieveContactL( *link );
                        CleanupStack::PopAndDestroy(); // link
                        }
                    }
                else
                    {
                    // nothing to delete
                    IssueStopRequest( EStopping );
                    }
                }            
            break;
            }
        case EConfirming:
            {
            ConfirmDeletionL();
            break;
            }
        case EStarting:
            {
            
            // Returns NULL if there are no selected contacts.
            // In that case we will delete the focused contact.
            if ( iUiControl )
                {
                //Re-Fetch the Contact Iterator
                MPbk2ContactLinkIterator* contactIterator = iUiControl->SelectedContactsIteratorL();
                if ( contactIterator )
                    {
                    delete iContactIterator;
                    iContactIterator = contactIterator;
                    }
                }
                
            if( iDecorator )
            	{
                iDecorator->ProcessStartedL( iContactCount );
            	}
            
            if ( iContactIterator )
                {
                iContactIterator->SetToLast();
                }
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
            // Decorator calls ProcessDismissed
            if( iDecorator )
            	{
                iDecorator->ProcessStopped();            	
            	}
            else
            	{
                ProcessDismissed( KErrNone );
            	}
            break;
            }
        case ECanceling:
            {
            ProcessDismissed( KErrCancel );
            break;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(ERunL_InvalidState));
            break;
            }
        }

    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2DeleteContactsCmd::RunL end"));
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2DeleteContactsCmd::RunError( 
        TInt aError )
    {
    return FilterErrors( aError );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We now have a store contact and we can issue a confirmation
        iStoreContact = aContact;
        iState = EConfirming;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError )
    {
    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We cannot get the contact, so we have to
        // fail. We cannot continue, since this operation
        // was executed only in case of one contact.
        ProcessDismissed( aError );
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::StepComplete
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::StepComplete(
        MVPbkContactOperationBase& aOperation,
        TInt aStepSize )
    {
    if ( &aOperation == iDeleteOperation )
        {
        // Indicate process advancement to the decorator
        if( iDecorator )
        	{
            iDecorator->ProcessAdvance( aStepSize );
        	}
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::StepFailed
// --------------------------------------------------------------------------
//
TBool CPbk2DeleteContactsCmd::StepFailed(
        MVPbkContactOperationBase& aOperation,
        TInt /*aStepSize*/,
        TInt aError )
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
// CPbk2DeleteContactsCmd::OperationComplete
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::OperationComplete(
        MVPbkContactOperationBase& aOperation )
    {
    if ( &aOperation == iDeleteOperation )
        {
        delete iDeleteOperation;
        iDeleteOperation = NULL;

        if ( iContactIterator && iContactIterator->HasPrevious() )
            {
            TRAPD( error, DoDeleteContactsL() );
            if ( error != KErrNone )
                {
                FilterErrors( error );
                IssueStopRequest( EStopping );
                }
            }
        else
            {
            IssueStopRequest( EStopping );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::ProcessDismissed( TInt aCancelCode )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2DeleteContactsCmd::ProcessDismissed()"));

    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_Null_Pointer));

    Cancel();
    delete iDeleteOperation;
    iDeleteOperation = NULL;

    if ( aCancelCode != KErrCancel && iUiControl)
        {
        // Don't remove list markings if user canceled
        iUiControl->SetBlank(EFalse);
        iUiControl->UpdateAfterCommandExecution();
        }

    // Notify command owner that the command has finished
    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::RetrieveContactL(
        const MVPbkContactLink& aContactLink )
    {
    // Retrieve the actual store contact from the given link
    iRetrieveOperation = iAppServices->
        ContactManager().RetrieveContactL( aContactLink, *this );
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::DoDeleteContactsL
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::DoDeleteContactsL()
    {
    delete iContactsToDelete;
    iContactsToDelete = NULL;
    iContactsToDelete = CVPbkContactLinkArray::NewL();
    const TInt KDeleteBlockSize = 200;

    if ( iContactIterator )
        {
        for ( TInt i = 0; i < KDeleteBlockSize &&
                iContactIterator->HasPrevious(); ++i )
            {
            MVPbkContactLink* link = iContactIterator->PreviousL();
            CleanupDeletePushL( link );
            iContactsToDelete->AppendL( link ); // takes ownership
            CleanupStack::Pop(); // link
            }
        }
    else
        {
        __ASSERT_DEBUG( iStoreContact,
            Panic( EPanicLogic_DoDeleteContactsL ) );

        MVPbkContactLink* link = iStoreContact->CreateLinkLC();
        iContactsToDelete->AppendL( link ); // takes ownership
        CleanupStack::Pop(); // link
        }

    iDeleteOperation = iAppServices->
        ContactManager().DeleteContactsL( *iContactsToDelete, *this );

    if (iUiControl)
        {
        // Blank UI control to avoid flicker
        iUiControl->SetBlank( ETrue );
        }

    iState = EDeleting;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::FilterErrors
// --------------------------------------------------------------------------
//
inline TInt CPbk2DeleteContactsCmd::FilterErrors( TInt aErrorCode )
	{
	PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2DeleteContactsCmd::FilterErrors(%d) start" ), aErrorCode );

	TInt result = aErrorCode;
	switch ( aErrorCode )
		{
		case KErrNotFound:  // FALLTHROUGH
		case KErrInUse:
			{
			// If you only have one contact or it's the last contact that failed, 
			// remove the progress bar. 
			if ( iContactCount == KOneContact || 
				( ( iContactIterator ) && !( iContactIterator->HasNext() ) ) ) 
				{ 
				Cancel(); 
				if ( iDecorator )
					{
					iDecorator->ProcessStopped();
				 	}
				ProcessDismissed( aErrorCode );
				} 
			else 
				{ 
				// Ignore these errors 
				// KErrNotFound means that somebody got the contact first  
				// KErrInUse means that the contact is open
				result = KErrNone;   
				}
			CCoeEnv::Static()->HandleError( aErrorCode ); 
			
			break; 
			}

		default:  // Something more serious happened -> give up
			{
			Cancel();
			ProcessDismissed( aErrorCode );
			
			break;
			}
		}
	
	PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
		( "CPbk2DeleteContactsCmd::FilterErrors(%d) end" ), result );
	
	return result;
	}

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::IssueStopRequest
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::IssueStopRequest( TProcessState aState )
    {
    iState = aState;
    if ( !IsActive() )
        {
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::ConfirmDeletionL
// --------------------------------------------------------------------------
//
void CPbk2DeleteContactsCmd::ConfirmDeletionL()
    {
    CPbk2GeneralConfirmationQuery* query =
            CPbk2GeneralConfirmationQuery::NewL();
    TInt queryResult = KErrNotFound;
    if (iContactCount > 1)
        {
        queryResult = query->ExecuteLD(
            iContactCount, R_QTN_PHOB_QUERY_DELETE_N_ENTRIES );
        }
    else
        {
        queryResult = query->ExecuteLD(
                    *iStoreContact, R_QTN_QUERY_COMMON_CONF_DELETE, MPbk2ContactNameFormatter::EPreserveAllOriginalSpaces );
            
        }

    if (queryResult)
        {
        // Continue with starting the deletion
        iState = EStarting;
        IssueRequest();
        }
    else
        {
        IssueStopRequest(ECanceling);
        }
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::GetContactCountL
// --------------------------------------------------------------------------
//
TInt CPbk2DeleteContactsCmd::GetContactCountL()
    {
    TInt result = 0;

    if ( iContactIterator )
        {
        iContactIterator->SetToFirst();
        while( iContactIterator->HasNext() )
            {
            MVPbkContactLink* link = iContactIterator->NextL();
            if ( link && !IsFromReadOnlyStore( *link ) )
                {
                ++result;
                }
            delete link;
            link = NULL;
            }
        }
    else
        {
        if (iUiControl)
            {            
            // There was no iterator so there are no selected contacts,
            // this means we are deleting only the focused contact
            const MVPbkBaseContact* focusedContact = iUiControl->FocusedContactL();
            if ( focusedContact )
                {
                MVPbkContactLink* link = focusedContact->CreateLinkLC();
                if ( link && !IsFromReadOnlyStore( *link ) )
                    {
                    ++result;
                    }
                CleanupStack::PopAndDestroy(); // link
                }
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2DeleteContactsCmd::IsFromReadOnlyStore
// --------------------------------------------------------------------------
//
TBool CPbk2DeleteContactsCmd::IsFromReadOnlyStore
        ( const MVPbkContactLink& aContactLink ) const
    {
    TBool ret = EFalse;

    const MVPbkContactStore& store = aContactLink.ContactStore();
    if ( store.StoreProperties().ReadOnly() )
        {
        ret = ETrue;
        }

    return ret;
    }

//  End of File
