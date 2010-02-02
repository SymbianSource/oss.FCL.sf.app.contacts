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
* Description:
*
*/


// INCLUDE FILES
#include "cpbk2openmycardcmd.h"
#include <CPbk2NamesListControl.h>
#include <MPbk2CommandObserver.h>
#include <MVPbkBaseContact.h>
#include <ccappmycardpluginuids.hrh>

//Cca
#include <mccaparameter.h>
#include <mccaconnection.h>
#include <ccafactory.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::CPbk2OpenMyCardCmd
// --------------------------------------------------------------------------
//
CPbk2OpenMyCardCmd::CPbk2OpenMyCardCmd(
    MPbk2ContactUiControl* aUiControl,
    MCCAConnection*& aCCAConnection ) :
        CActive( EPriorityStandard ),
        iUiControl( aUiControl ),
        iConnectionRef(aCCAConnection),
        iViewUid( TUid::Uid( KCCAMyCardPluginImplmentationUid ) ) // default mycard view
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::~CPbk2OpenMyCardCmd
// --------------------------------------------------------------------------
//
CPbk2OpenMyCardCmd::~CPbk2OpenMyCardCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2OpenMyCardCmd(%x)::~CPbk2OpenMyCardCmd()"), this);
    Cancel();

    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2OpenMyCardCmd* CPbk2OpenMyCardCmd::NewL(
    MCCAConnection*& aCCAConnection,
    MPbk2ContactUiControl* aUiControl )
    {
    CPbk2OpenMyCardCmd* self = new ( ELeave ) CPbk2OpenMyCardCmd
        ( aUiControl, aCCAConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::ConstructL()
    {
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::SetViewUid
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::SetViewUid( TUid aViewUid )
    {
    iViewUid = aViewUid;
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::ExecuteLD()
    {
    iState = ELaunching;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::LaunchCcaL
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::LaunchCcaL()
    {
    if(!iConnectionRef)
        {
        iConnectionRef = TCCAFactory::NewConnectionL();
        }

    MCCAParameter* parameter = TCCAFactory::NewParameterL();
    CleanupClosePushL( *parameter );
    parameter->SetConnectionFlag( MCCAParameter::ENormal );
    parameter->SetLaunchedViewUid( iViewUid );

    // Sync call
    iConnectionRef->LaunchAppL( *parameter );
    CleanupStack::Pop();// parameter is taken care by iConnectionRef

    iState = ERunning;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::RunL()
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
        if( iCommandObserver )
            {
            iCommandObserver->CommandFinished( *this );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2OpenMyCardCmd::RunError(TInt aError)
    {
    // Forward all errors to the framework. They will be
    // displayed on screen to the user. Typically there
    // should not be any errors.

    return aError;
    }

//  End of File
