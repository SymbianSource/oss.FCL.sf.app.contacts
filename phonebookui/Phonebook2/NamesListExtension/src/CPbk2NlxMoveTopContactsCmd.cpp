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
* Description:  Organize(reorder) top contacts command
*
*/


// INCLUDE FILES
#include "CPbk2NlxMoveTopContactsCmd.h"
#include "CPbk2NlxMoveTopContactsDlg.h"
#include "CPbk2NlxReorderingModel.h"

#include <Pbk2UIControls.rsg>
#include <Pbk2ProcessDecoratorFactory.h>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>
#include <MPbk2ContactViewSupplier2.h>
#include <CVPbkContactManager.h>
#include <MVPbkBaseContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactViewBase.h>

namespace
    {
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_IncorrestState = 1,
    EPanicPreCond_NoteActive
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2NlxMoveTopContactsCmd");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::CPbk2NlxMoveTopContactsCmd
// --------------------------------------------------------------------------
//
CPbk2NlxMoveTopContactsCmd::CPbk2NlxMoveTopContactsCmd(
    MPbk2ContactUiControl& aUiControl ) :
        CActive( EPriorityStandard ),
        iUiControl( &aUiControl )
    {
    CActiveScheduler::Add( this );
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::~CPbk2NlxMoveTopContactsCmd
// --------------------------------------------------------------------------
//
CPbk2NlxMoveTopContactsCmd::~CPbk2NlxMoveTopContactsCmd()
    {
    Cancel();
    if (iUiControl)
        {
        iUiControl->RegisterCommand( NULL );
        }
    
    delete iFocusedContact;
    delete iTopContactOperation;
    delete iReorderingModel;
    delete iVPbkTopContactManager;
    delete iDecorator;
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2NlxMoveTopContactsCmd* CPbk2NlxMoveTopContactsCmd::NewL(
    MPbk2ContactUiControl& aUiControl )
    {
    CPbk2NlxMoveTopContactsCmd* self = 
        new ( ELeave ) CPbk2NlxMoveTopContactsCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsCmd::ConstructL()
    {    
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }   
    
    iVPbkTopContactManager = CVPbkTopContactManager::NewL(
    		Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager() );
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsCmd::ExecuteLD()
    {
    if ( iUiControl )
    	{
    	iUiControl->SetBlank(ETrue);
    	}
    IssueRequest(EShowMoveDlg);
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsCmd::ResetUiControl( 
    MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsCmd::AddObserver( 
    MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }        
    
// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsCmd::RunL()
    {
    switch(iState)
    	{
    	case EShowMoveDlg:
    		{
    		ShowDlgL();
    		break;
    		}
    	case ESavingTopContacts:
    		{
    		ShowWaitNoteL();
    		
    		iFocusedContact = iReorderingModel->FocusedContactLinkL();
    		MVPbkContactLinkArray* topContacts =
    			iReorderingModel->ReorderedLinksLC();
    		//save top contacts
    		iTopContactOperation = iVPbkTopContactManager->SetTopOrderL(
    		        *topContacts,
    		        *this,
    		        *this );
    		CleanupStack::PopAndDestroy(); //topContacts
    		break;
    		}
    	case ECompleteCommand:
    		{
    	    if ( iUiControl )
    	    	{
    	    	// keep focused item
    	    	if ( iFocusedContact )
    	    		{
    	    		iUiControl->SetFocusedContactL( *iFocusedContact );
    	    		}
   	            iUiControl->SetBlank(EFalse);
   	            iUiControl->UpdateAfterCommandExecution();
    	    	}
    	    if ( iCommandObserver )
    	        {
    	        iCommandObserver->CommandFinished( *this );
    	        }
    	    break;
    		}
    	}
    }    
    
// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2NlxMoveTopContactsCmd::RunError( TInt aError )
    {
    if ( aError != KErrNone )
        {
        FinishCommand( aError );
        }

    return KErrNone;
    }   

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsCmd::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::IssueRequest
// Advances to the next state 
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsCmd::IssueRequest(TState aNextState)
	{
	iState = aNextState;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();	
	}

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsCmd::FinishCommand
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsCmd::FinishCommand(TInt aResult)
    {
    if ( aResult != KErrNone && aResult != KErrCancel )
        {
        CCoeEnv::Static()->HandleError( aResult );
        }
    if ( iDecorator )
        {
        // wait for callback from the wait note and finish then
        iDecorator->ProcessStopped();
        }
    else
        {
        IssueRequest( ECompleteCommand );
        }
    }

// ---------------------------------------------------------------------------    
// CPbk2NlxMoveTopContactsCmd::ShowDlgL
// Displays Reordering dialog and creates listbox model for it
// ---------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsCmd::ShowDlgL()
	{
	MPbk2ContactViewSupplier2* viewSupplierExtension = 
	    reinterpret_cast<MPbk2ContactViewSupplier2*>(
	        Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().
                MPbk2ContactViewSupplierExtension(
                    KMPbk2ContactViewSupplierExtension2Uid ));
    __ASSERT_DEBUG( viewSupplierExtension, Panic(EPanicPreCond_IncorrestState));
	__ASSERT_DEBUG( viewSupplierExtension->TopContactsViewL(),
	        Panic(EPanicPreCond_IncorrestState));
	__ASSERT_DEBUG(!iReorderingModel, Panic(EPanicPreCond_IncorrestState));
	
    iReorderingModel = CPbk2NlxReorderingModel::NewL(
            Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
            *viewSupplierExtension->TopContactsViewL(),
    		Phonebook2::Pbk2AppUi()->ApplicationServices().NameFormatter(),
    		Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties() );
    
	if ( iUiControl )
		{
		//keep focus from the namelist view
		MVPbkContactLink* focusedContact = 
		    iUiControl->FocusedContactL()->CreateLinkLC();
		iReorderingModel->SetFocusedContactL( *focusedContact );
		CleanupStack::PopAndDestroy();//focusedContact
		}    
    
    CPbk2NlxMoveTopContactsDlg* dlg = CPbk2NlxMoveTopContactsDlg::NewL(
    		*iReorderingModel );

    if ( dlg->ExecuteLD() )
    	{
    	IssueRequest(ESavingTopContacts);
    	}
    else
    	{
    	FinishCommand( KErrCancel );
    	}
	}

void CPbk2NlxMoveTopContactsCmd::VPbkOperationCompleted(
        MVPbkContactOperationBase*)
    {
    FinishCommand( KErrNone );
    }

void CPbk2NlxMoveTopContactsCmd::VPbkOperationFailed(
        MVPbkContactOperationBase*,
        TInt aError )
    {
    FinishCommand( aError );
    }

void CPbk2NlxMoveTopContactsCmd::ShowWaitNoteL()
    {
    __ASSERT_DEBUG(!iDecorator, Panic(EPanicPreCond_NoteActive));
    
    iDecorator = Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL
        ( R_QTN_GEN_NOTE_SAVING_WAIT, ETrue );
    iDecorator->SetObserver( *this );
    iDecorator->ProcessStartedL( 0 ); // wait note doesn't care about amount
    }

void CPbk2NlxMoveTopContactsCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    // if it's cancel, just finish cmd execution,
    // the cmd will be deleted and canceled that way
    IssueRequest( ECompleteCommand );
    }
//  End of File
