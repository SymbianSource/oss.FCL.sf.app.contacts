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
* Description:  Phonebook 2 Group UI Extension remove from group command.
*
*/


// INCLUDE FILES
#include "CPguRemoveFromGroupCmd.h"

// Phonebook2
#include <pbk2groupuires.rsg>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2GeneralConfirmationQuery.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <coemain.h>
#include <eikprogi.h> // CEikProgessInfo
#include <akninputblock.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KOne( 1 );
const TInt KFirstContact( 0 );

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_ExecuteLD = 1,
    EPanicContactNotGroup, 
    EInvalidState,
    EPanicNullPointer
    };

void Panic( TPanicCode aReason )
    {
    _LIT( KPanicText, "CPguRemoveFromGroupCmd" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AknProgressDialog::CPbk2AknProgressDialog
// --------------------------------------------------------------------------
// 
CPbk2AknProgressDialog::CPbk2AknProgressDialog(
		CEikDialog** aSelfPtr,
		TBool aVisibilityDelayOff ):
	CAknProgressDialog( aSelfPtr, aVisibilityDelayOff ),
	iEndCallKeyPressed( EFalse )
	{
	
	}
		
// --------------------------------------------------------------------------
// CPbk2AknProgressDialog::~CPbk2AknProgressDialog
// --------------------------------------------------------------------------
//
CPbk2AknProgressDialog::~CPbk2AknProgressDialog()
    {

    }

// --------------------------------------------------------------------------
// CPbk2AknProgressDialog::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2AknProgressDialog::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
	{	
	if( aType == EEventKey  && aKeyEvent.iCode == EKeyEscape )
		{
		if( !iEndCallKeyPressed )
		    {
		    iEndCallKeyPressed = ETrue;
		    }
		}
	return CAknProgressDialog::OfferKeyEventL( aKeyEvent, aType );
	}

// --------------------------------------------------------------------------
// CPbk2AknProgressDialog::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2AknProgressDialog::OkToExitL(TInt aButtonId)
	{
	if( iEndCallKeyPressed )
		{
		iEndCallKeyPressed = EFalse;
	    if ( iCallback )
            {
            iCallback->DialogDismissedL(aButtonId);
            }
        return ETrue;
		}	
	return CAknProgressDialog::OkToExitL( aButtonId );
	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::CPguRemoveFromGroupCmd
// --------------------------------------------------------------------------
//    
CPguRemoveFromGroupCmd::CPguRemoveFromGroupCmd(
	    MVPbkContactLink& aContactGroup, 
	    MPbk2ContactUiControl& aUiControl ) :
	CActive( EPriorityIdle ),
    iUiControl( &aUiControl ),
    iGroupLink( aContactGroup )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguRemoveFromGroupCmd::CPguRemoveFromGroupCmd(0x%x)"),this);
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::~CPguRemoveFromGroupCmd
// --------------------------------------------------------------------------
//
CPguRemoveFromGroupCmd::~CPguRemoveFromGroupCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguRemoveFromGroupCmd::~CPguRemoveFromGroupCmd(0x%x)"),
        this);
	Cancel();
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
	
	delete iSelectedContacts;
	delete iProgressDialog;
	delete iStoreContact;
    delete iRetrieveOperation;
    delete iContactGroup;
    delete iInputBlock;
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::NewLC
// --------------------------------------------------------------------------
//
CPguRemoveFromGroupCmd* CPguRemoveFromGroupCmd::NewLC(
        MVPbkContactLink& aContactGroup, 
        MPbk2ContactUiControl& aUiControl )
    {
    CPguRemoveFromGroupCmd* self = new(ELeave) CPguRemoveFromGroupCmd(
                                        aContactGroup, aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguRemoveFromGroupCmd::ConstructL()
    {
    // UI input not allowed during async. operation
    ReactivateUIBlockL();   
    
    // Save focus 
    iFocusedContactIndex = iUiControl->FocusedContactIndex();

	iSelectedContacts = iUiControl->SelectedContactsOrFocusedContactL();
	__ASSERT_DEBUG(iSelectedContacts, Panic(EPanicNullPointer));
	iTotalContactsToRemove = iSelectedContacts->Count();    
    iUiControl->RegisterCommand( this );
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::ExecuteLD()
    {
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_ExecuteLD));
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguRemoveFromGroupCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL( this );
	 
    if ( iSelectedContacts )
        {
        if ( iSelectedContacts->Count() > KOne )
            {
            // Move straight to confirm phase
            iState = EConfirming;
            IssueRequest();
            }
        else if ( iSelectedContacts->Count() == KOne )
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
// CPguRemoveFromGroupCmd::AddObserver
// --------------------------------------------------------------------------
//        
void CPguRemoveFromGroupCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::ResetUiControl
        ( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::DoCancel()
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    if (iUiControl)
        {
        iUiControl->SetBlank( EFalse );
        }    
    TRAP_IGNORE( DeleteProgressNoteL() );
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::RunL
// --------------------------------------------------------------------------
//    
void CPguRemoveFromGroupCmd::RunL()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPguRemoveFromGroupCmd::RunL()"), this);

	switch ( iState )
		{
		case ERetrieving:
			{
			// UI input not allowed during async. operation
			ReactivateUIBlockL();	
			
            const MVPbkContactLink& link =
                iSelectedContacts->At( KFirstContact );
            RetrieveContactL( link );
			break; 
			}
		case EConfirming:
			{
			ConfirmRemovingL();
			break;
			}
		case ERetrievingGroupContact:
			{
	         // UI input not allowed during async. operation
	        ReactivateUIBlockL();
	        
			InitProgressDlgL();
			RetrieveContactL( iGroupLink );
			break;
			}
		case ERemoving:
			{
			HandleRemovingL();
			break;
			}
		case ECompleting:
	        {
	        CompleteL();
            DeleteProgressNoteL();
            break;	        
	        }
        case ECompletingWithError:
            {
            CompleteWithError();
            DeleteProgressNoteL();
            break;
            }
		default:
		    {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidState ) );
		    break;
		    }
		}		

    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPguRemoveFromGroupCmd::RunL end")); 
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPguRemoveFromGroupCmd::RunError( TInt aError )
    {
    iCommandObserver->CommandFinished( *this );
    return aError;
    }    

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//    
void CPguRemoveFromGroupCmd::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    if ( iState == ERetrievingGroupContact )
    	{
	    iContactGroup = aContact->Group();    	

	    __ASSERT_DEBUG(iContactGroup, Panic(EPanicContactNotGroup));
	        
	    TRAPD( error, iContactGroup->LockL( *this ) );
        
        if ( error != KErrNone )
            {            
            CCoeEnv::Static()->HandleError( error );
            
            iState = ECompletingWithError;
            IssueRequest();
            }
	    }
	else
		{
		// We now have a store contact and we can issue a confirmation
        iStoreContact = aContact;
        iState = EConfirming;
        IssueRequest();
		}
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//    
void CPguRemoveFromGroupCmd::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/,  TInt aError )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    iState = ECompletingWithError;
    IssueRequest();

    CCoeEnv::Static()->HandleError( aError );
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    if ( aResult.iOpCode == MVPbkContactObserver::EContactLock )
        {        		
        iState = ERemoving;
        IssueRequest();
        }
	else if ( aResult.iOpCode == MVPbkContactObserver::EContactCommit )
        {
        if ( iCommandObserver )
            {
            iCommandObserver->CommandFinished( *this );
            }
        }	
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//    
void CPguRemoveFromGroupCmd::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode, TBool aErrorNotified )
    {
    iState = ECompletingWithError;
    IssueRequest();

    if ( !aErrorNotified )
        {
        CCoeEnv::Static()->HandleError( aErrorCode );
        }
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ContactViewReady
// --------------------------------------------------------------------------
//	
void CPguRemoveFromGroupCmd::ContactViewReady
        ( MVPbkContactViewBase& /*aView*/ )
	{
	// Do nothing
	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ContactViewUnavailable
// --------------------------------------------------------------------------
//	
void CPguRemoveFromGroupCmd::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
	{
	// Do nothing
	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ContactAddedToView
// --------------------------------------------------------------------------
//	
void CPguRemoveFromGroupCmd::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
	{
	// Do nothing
	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ContactRemovedFromView
// --------------------------------------------------------------------------
//	
void CPguRemoveFromGroupCmd::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
	{
	// Do nothing
	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ContactViewError
// --------------------------------------------------------------------------
//	
void CPguRemoveFromGroupCmd::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt aError,
          TBool /*aErrorNotified*/ )
	{
    iState = ECompletingWithError;
    IssueRequest();

    CCoeEnv::Static()->HandleError( aError );
	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::DialogDismissedL
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::DialogDismissedL( TInt /*aButtonId*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbkRemoveFromGroupCmd::DialogDismissedL"));
        
    iProgressDialog = NULL;        
    iDialogDismissed = ETrue;    
    // Notify command owner that the command has finished
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );
        }
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::MoreContactsToRemove
// --------------------------------------------------------------------------
//
inline TBool CPguRemoveFromGroupCmd::MoreContactsToRemove() const
    {
    return ( iSelectedContacts && 
        ( iTotalContactsToRemove > iRemovedCount ) );
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::IsProcessDone
// --------------------------------------------------------------------------
//	
TBool CPguRemoveFromGroupCmd::IsProcessDone() const
    {
    return !MoreContactsToRemove();
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::StepL
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::StepL()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbkRemoveFromGroup::StepL()"), this);

    if ( MoreContactsToRemove() && 
         iSelectedContacts->Count() > iRemovedCount )
        {
        const MVPbkContactLink& contactLink = 
        	iSelectedContacts->At( iRemovedCount );
        TRAPD( err, iContactGroup->RemoveContactL( contactLink ) );
        if ( err == KErrInUse )
            {
            // If there is only one contact that failed, 
            // system error info note is shown and the process is terminated.
            if ( iSelectedContacts->Count() == KOne )
                {
                CCoeEnv::Static()->HandleError( err ); 
                iState = ECompletingWithError;
                }
            }
        else
            {
            User::LeaveIfError( err ); 
            }
        ++iRemovedCount;
        }
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::IssueRequest
// --------------------------------------------------------------------------
//    	
void CPguRemoveFromGroupCmd::IssueRequest()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();        
        }
    } 

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::CompleteL
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::CompleteL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPguRemoveFromGroupCmd::ProcessFinished(0x%x)"), this);

    if ( iUiControl )
        {
        iUiControl->SetBlank( EFalse );
        // Try to set focused contact
        iUiControl->SetFocusedContactIndexL( iFocusedContactIndex );
        // Clear listbox selections
        iUiControl->UpdateAfterCommandExecution();
        }
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::CompleteWithError
// --------------------------------------------------------------------------
//   
void CPguRemoveFromGroupCmd::CompleteWithError()
    {
    if ( iUiControl )
        {
        iUiControl->SetBlank( EFalse );
        }
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::DeleteProgressNoteL
// --------------------------------------------------------------------------
//    	
void CPguRemoveFromGroupCmd::DeleteProgressNoteL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPguRemoveFromGroupCmd::DeleteProgressNote start"));
        
    if ( iProgressDialog && !iDialogDismissed )
        {
        // Delete the dialog
        iProgressDialog->ProcessFinishedL();
        iProgressDialog = NULL;
        }
    else if ( iCommandObserver )
        {        
        iCommandObserver->CommandFinished( *this );
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPguRemoveFromGroupCmd::DeleteProgressNote end"));
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::RetrieveContactL
// --------------------------------------------------------------------------
//    
void CPguRemoveFromGroupCmd::RetrieveContactL
        ( const MVPbkContactLink& aContactLink)
    {
    // Retrieve the actual store contact from the given link
    iRetrieveOperation = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().RetrieveContactL( aContactLink, *this );
    }

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ConfirmRemovingL
// --------------------------------------------------------------------------
//	
void CPguRemoveFromGroupCmd::ConfirmRemovingL()
	{
	// When popping up the query, the input block will lose its focus, and it can't 
    // block the event. So delete the input block before the query pops up.
    if (iInputBlock)
        {
        delete iInputBlock;
        iInputBlock = NULL;
        }
    
    //original we will ask user if they really want to remove contacts from group,
    //but currently we decide that we will just do it without asking user
    TInt count = iSelectedContacts->Count();
    if (count >= 1)
        {
        // If the query result is confirm, restart the input block again to
        // keep blocking other event during the command execution.
        if (!iInputBlock)
            {
            iInputBlock = CAknInputBlock::NewLC();
            CleanupStack::Pop(iInputBlock);
            }

        // Retrieve the group contact
        iState = ERetrievingGroupContact;
        IssueRequest();
        }
    else
        {
        // Complete command, leave UI control as it was
        iState = ECompletingWithError;
        IssueRequest();
        }
	
	

	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::InitProgressDlgL
// --------------------------------------------------------------------------
//    
void CPguRemoveFromGroupCmd::InitProgressDlgL()
	{
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPguRemoveFromGroupCmd::InitProgressDlgL"));
    
    if ( iUiControl )
        {
        iUiControl->SetBlank( ETrue );
        }
    
    CAknProgressDialog* progressDialog = new(ELeave) CPbk2AknProgressDialog(
            reinterpret_cast<CEikDialog**>(NULL), EFalse );
    progressDialog->PrepareLC( R_QTN_PHOB2_WNOTE_REM_FROM_GROUP );
    iProgressDlgInfo = progressDialog->GetProgressInfoL();
    iProgressDlgInfo->SetFinalValue( iTotalContactsToRemove );
    progressDialog->SetCallback( this );
    iDialogDismissed = EFalse;
    progressDialog->RunLD();
    iProgressDialog = progressDialog;	
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPguRemoveFromGroupCmd::InitProgressDlgL end"));
	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::HandleRemovingL
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::HandleRemovingL()
	{
    if ( !IsProcessDone() )
        {
        // Process one step
        StepL();
        
        // Incrementing progress of the process
        iProgressDlgInfo->SetAndDraw( iRemovedCount );

        // Issue request to remove next item
        IssueRequest();
        }
    else
        {
        // Process is completed, all entries have been removed
        iState = ECompleting;
        IssueRequest();
        }	
	}

// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::ReactivateUIBlockL
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::ReactivateUIBlockL()
    {
    // Use CAknInputBlock to block user input
    
    if( iInputBlock )
        {
        delete iInputBlock;
        iInputBlock = NULL;
        }
    
    iInputBlock = CAknInputBlock::NewLC();
    CleanupStack::Pop( iInputBlock );   
    }


// --------------------------------------------------------------------------
// CPguRemoveFromGroupCmd::Abort
// --------------------------------------------------------------------------
//
void CPguRemoveFromGroupCmd::Abort()
    {
    if ( iProgressDialog && !iDialogDismissed )
        {
        // Delete the dialog
        TRAP_IGNORE( iProgressDialog->ProcessFinishedL() );
        iProgressDialog = NULL;
        }
    }

//  End of File  
