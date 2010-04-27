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
* Description:  Phonebook 2 CCA launcher command
*
*/


// INCLUDE FILES
#include "CPbk2NlxOpenCcaCmd.h"
#include <CPbk2NamesListControl.h>
#include <MPbk2CommandObserver.h>
#include <MVPbkBaseContact.h>

//Cca
#include <mccaparameter.h>
#include <mccaconnection.h>
#include <ccafactory.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::CPbk2NlxOpenCcaCmd
// --------------------------------------------------------------------------
//
CPbk2NlxOpenCcaCmd::CPbk2NlxOpenCcaCmd(
    MPbk2ContactUiControl& aUiControl,
    MCCAConnection*& aCCAConnection ) :
        CActive( EPriorityStandard ),
        iUiControl( &aUiControl ),
        iConnectionRef(aCCAConnection)
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::~CPbk2NlxOpenCcaCmd
// --------------------------------------------------------------------------
//
CPbk2NlxOpenCcaCmd::~CPbk2NlxOpenCcaCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NlxOpenCcaCmd(%x)::~CPbk2NlxOpenCcaCmd()"), this);

    if (IsActive())
        {
        Cancel();
        }
    
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2NlxOpenCcaCmd* CPbk2NlxOpenCcaCmd::NewL(
    MPbk2ContactUiControl& aUiControl,
    MCCAConnection*& aCCAConnection )
    {
    CPbk2NlxOpenCcaCmd* self = new ( ELeave ) CPbk2NlxOpenCcaCmd
        ( aUiControl, aCCAConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2NlxOpenCcaCmd::ConstructL()
    {    
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }    
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2NlxOpenCcaCmd::ExecuteLD()
    {
    iState = ELaunching;
    IssueRequest();
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2NlxOpenCcaCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2NlxOpenCcaCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }        
    
// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::LaunchCcaL
// --------------------------------------------------------------------------
//
void CPbk2NlxOpenCcaCmd::LaunchCcaL()
    {   
    if (iUiControl)
        {
        if(!iConnectionRef)
            {
            iConnectionRef = TCCAFactory::NewConnectionL();
            }
        
        MCCAParameter* parameter = TCCAFactory::NewParameterL();
        CleanupClosePushL( *parameter );
        parameter->SetConnectionFlag(MCCAParameter::ENormal);
        parameter->SetContactDataFlag(MCCAParameter::EContactLink);
        
        const MVPbkBaseContact* contact = iUiControl->FocusedContactL();  
        
        if(contact) 
            {
            MVPbkContactLink* link = contact->CreateLinkLC(); 

            if (link)
                {
                HBufC8* link8 = link->PackLC();                        
                HBufC16* link16 = HBufC16::NewLC( link8->Length() );
                link16->Des().Copy( *link8);
                parameter->SetContactDataL(link16->Des());
                CleanupStack::PopAndDestroy(3); 
                }

            // Sync call
            iConnectionRef->LaunchAppL( *parameter );
            CleanupStack::Pop();// parameter is taken care by iConnectionRef
            }
        }
    iState = ERunning;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2NlxOpenCcaCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2NlxOpenCcaCmd::RunL()
    {
    if ( iState == ELaunching )
        {
        LaunchCcaL();   //Sync
        }
    else if ( iState == ERunning )
        {
        if ( iUiControl )
            {
            iUiControl->UpdateAfterCommandExecution();  
            }            
        // Asynchronous delete of this command object
        iCommandObserver->CommandFinished( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2NlxOpenCcaCmd::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NlxOpenCcaCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2NlxOpenCcaCmd::RunError(TInt aError)
    {
    // Forward all errors to the framework. They will be
    // displayed on screen to the user. Typically there 
    // should not be any errors.
    
    return aError;
    }

//  End of File
