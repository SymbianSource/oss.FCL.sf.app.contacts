/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*           Remote contact lookup command event handling class.
*
*/

// INCLUDE FILES
#include "CPbkRclCmd.h"

// Phonebook 2
#include <Pbk2Commands.rsg>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>

//#include <CVPbkContactStoreUriArray.h>

// RCL
#include <pbk2remotecontactlookupfactory.h>
#include <cpbk2remotecontactlookupaccounts.h>
#include <cpbkxremotecontactlookupserviceuicontext.h>

// System includes
#include <StringLoader.h>
#include <avkon.hrh>

/// Unnamed namespace for local definitions
namespace {

enum TPbk2RclCmdState
    {
    ELaunchDialog,
    EComplete
    };
   
} /// namespace

// --------------------------------------------------------------------------
// CPbkRclCmd::CPbkRclCmd
// --------------------------------------------------------------------------
//
CPbkRclCmd::CPbkRclCmd
        ( MPbk2ContactUiControl& aUiControl ) :
            CActive( EPriorityStandard ),
            iUiControl(aUiControl)
    {
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::~CPbkRclCmd
// --------------------------------------------------------------------------
//
CPbkRclCmd::~CPbkRclCmd()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::NewL
// --------------------------------------------------------------------------
//
CPbkRclCmd* CPbkRclCmd::NewL(
        MPbk2ContactUiControl& aUiControl)
    {
    CPbkRclCmd* self = 
        new( ELeave ) CPbkRclCmd(aUiControl);    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbkRclCmd::ConstructL()
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbkRclCmd::ExecuteLD()
    {
    iState = ELaunchDialog;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbkRclCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbkRclCmd::ResetUiControl(
        MPbk2ContactUiControl& /*aUiControl*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::RunL
// --------------------------------------------------------------------------
//
void CPbkRclCmd::RunL()
    {
    switch (iState)
        {
        case ELaunchDialog:
            {
            LaunchDialogL();
            break;
            }
        case EComplete: // FALLTHROUGH
        default:
            {
            iCommandObserver->CommandFinished(*this);
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::DoCancel
// --------------------------------------------------------------------------
//        
void CPbkRclCmd::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbkRclCmd::RunError(TInt aError)
    {
    HandleError(aError);
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CPbkRclCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbkRclCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbkRclCmd::LaunchDialogL
// --------------------------------------------------------------------------
//
void CPbkRclCmd::LaunchDialogL()
    {
    TPbkxRemoteContactLookupProtocolAccountId account = CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();
   
    CPbkxRemoteContactLookupServiceUiContext::TContextParams params = { account,      
            CPbkxRemoteContactLookupServiceUiContext::EModeNormal   }; 

    CPbkxRemoteContactLookupServiceUiContext* ctx =
        Pbk2RemoteContactLookupFactory::NewContextL( params );
    CleanupStack::PushL( ctx );

    CPbkxRemoteContactLookupServiceUiContext::TResult aResult;
    
    // If user hasn't entered any characters to the findbox - FindTextL
    // returns an empty string and the search query is left blank.
    ctx->ExecuteL( iUiControl.FindTextL(), aResult );   

    CleanupStack::PopAndDestroy( ctx );
    
    iState = EComplete;
    IssueRequest();
    }
    
// --------------------------------------------------------------------------
// CPbkRclCmd::HandleError
// --------------------------------------------------------------------------
//
void CPbkRclCmd::HandleError(TInt aError)
    {
    if (aError != KErrNone)
        {
        iCommandObserver->CommandFinished(*this);
        CCoeEnv::Static()->HandleError(aError);
        }
    }
    
//  End of File  
