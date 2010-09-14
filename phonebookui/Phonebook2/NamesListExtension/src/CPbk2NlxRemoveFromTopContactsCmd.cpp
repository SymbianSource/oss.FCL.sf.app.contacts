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
* Description:  Phonebook 2 Remove From Top Contacts launcher command
*
*/


// INCLUDE FILES
#include "CPbk2NlxRemoveFromTopContactsCmd.h"
#include <CPbk2NamesListControl.h>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ApplicationServices.h>
#include <CVPbkContactManager.h>
#include <MVPbkBaseContact.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkTopContactManager.h>
#include <pbk2uicontrols.rsg> 

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>



// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::CPbk2NlxRemoveFromTopContactsCmd
// --------------------------------------------------------------------------
//
CPbk2NlxRemoveFromTopContactsCmd::CPbk2NlxRemoveFromTopContactsCmd(
    MPbk2ContactUiControl& aUiControl ) :
        CActive( EPriorityStandard ),
        iUiControl( &aUiControl )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::~CPbk2NlxRemoveFromTopContactsCmd
// --------------------------------------------------------------------------
//
CPbk2NlxRemoveFromTopContactsCmd::~CPbk2NlxRemoveFromTopContactsCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NlxRemoveFromTopContactsCmd(%x)::~CPbk2NlxRemoveFromTopContactsCmd()"), 
        this);

    Cancel();
    
    if (iUiControl)
        {
        iUiControl->RegisterCommand( NULL );
        }
        
    //Cancel the operation if not yet done
    delete iVPbkContactOperationBase;                
    delete iVPbkTopContactManager;
    delete iDelayedWaitNote;
    iDelayedWaitNote = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2NlxRemoveFromTopContactsCmd* CPbk2NlxRemoveFromTopContactsCmd::NewL(
    MPbk2ContactUiControl& aUiControl )
    {
    CPbk2NlxRemoveFromTopContactsCmd* self = new ( ELeave ) 
        CPbk2NlxRemoveFromTopContactsCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::ConstructL()
    {    
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }    
    
    iVPbkTopContactManager = CVPbkTopContactManager::NewL(
    		Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager() );
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::ExecuteLD()
    {
    if(IsActive())
    	{
    	Cancel();
    	}
    
    if( iUiControl )
        {
        iUiControl->SetBlank( ETrue );
        }     
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::ResetUiControl( 
    MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::AddObserver( 
    MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }        
    
// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::RunL()
    {
    ShowDelayedWaitNoteL();
    
    RemoveTopContactL();
    }    

// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2NlxRemoveFromTopContactsCmd::RunError(TInt aError)
	{
    if (iDelayedWaitNote)
        {
        iDelayedWaitNote->Stop();       
        }  
    return aError;
	}
    
// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::DoCancel()
    {
    //Currently no op    
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::ShowDelayedWaitNoteL
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::ShowDelayedWaitNoteL()
    {
    iDelayedWaitNote = CPbk2DelayedWaitNote::NewL(*this, R_QTN_GEN_NOTE_SAVING_WAIT );
    iDelayedWaitNote->Start();
    }

// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::ProcessDismissed(TInt /*aCancelCode*/)
    {
    if( iUiControl )
        {
        iUiControl->SetBlank( EFalse);
        }     
    iCommandObserver->CommandFinished( *this );   
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::RemoveTopContactL
// --------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::RemoveTopContactL()
    {   
    if (iUiControl)
        {
        const MVPbkBaseContact* contact = iUiControl->FocusedContactL();  

        if(contact) 
            {
            MVPbkContactLink* link = contact->CreateLinkLC(); 

            if (link)
                {
                iVPbkContactOperationBase = 
                    iVPbkTopContactManager->RemoveFromTopL( *link, *this, *this );                
                CleanupStack::PopAndDestroy();  //link
                }
            }
        }
    
    return;
    }
    

////////////////////////////// CALLBACKS /////////////////////////////////////    

// ---------------------------------------------------------------------------    
// CPbk2NlxRemoveFromTopContactsCmd::TopOperationCompleted
// ---------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::VPbkOperationCompleted(MVPbkContactOperationBase*)
    {
    if (iDelayedWaitNote)
        {
        iDelayedWaitNote->Stop();       
        }     
    }

// ---------------------------------------------------------------------------
// CPbk2NlxRemoveFromTopContactsCmd::TopErrorOccured
// ---------------------------------------------------------------------------
//
void CPbk2NlxRemoveFromTopContactsCmd::VPbkOperationFailed( MVPbkContactOperationBase*, TInt aError )
    {
    CCoeEnv::Static()->HandleError( aError );
    if (iDelayedWaitNote)
        {
        iDelayedWaitNote->Stop();       
        }         
    }
	    
//  End of File
