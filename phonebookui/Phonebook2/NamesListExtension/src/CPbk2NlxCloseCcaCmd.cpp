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
* Description:  Phonebook 2 CCA close command
*
*/


// INCLUDE FILES
#include "CPbk2NlxCloseCcaCmd.h"
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
// CPbk2NlxCloseCcaCmd::CPbk2NlxCloseCcaCmd
// --------------------------------------------------------------------------
//
CPbk2NlxCloseCcaCmd::CPbk2NlxCloseCcaCmd(
    MPbk2ContactUiControl& aUiControl,
    MCCAConnection*& aCCAConnection ) :
        iUiControl( &aUiControl ),
        iConnectionRef(aCCAConnection)
    {
    }

// --------------------------------------------------------------------------
// CPbk2NlxCloseCcaCmd::~CPbk2NlxCloseCcaCmd
// --------------------------------------------------------------------------
//
CPbk2NlxCloseCcaCmd::~CPbk2NlxCloseCcaCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NlxCloseCcaCmd(%x)::~CPbk2NlxCloseCcaCmd()"), this);

    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NlxCloseCcaCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2NlxCloseCcaCmd* CPbk2NlxCloseCcaCmd::NewL(
    MPbk2ContactUiControl& aUiControl,
    MCCAConnection*& aCCAConnection )
    {
	CPbk2NlxCloseCcaCmd* self = new ( ELeave ) CPbk2NlxCloseCcaCmd
        ( aUiControl, aCCAConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NlxCloseCcaCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2NlxCloseCcaCmd::ConstructL()
    {    
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }    
    }

// --------------------------------------------------------------------------
// CPbk2NlxCloseCcaCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2NlxCloseCcaCmd::ExecuteLD()
    {
    if ( iConnectionRef )
        {
        iConnectionRef->Close();
        iConnectionRef = NULL;
        }

    if ( iUiControl )
        {
        iUiControl->UpdateAfterCommandExecution();  
        }
    // Asynchronous delete of this command object
    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPbk2NlxCloseCcaCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2NlxCloseCcaCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2NlxCloseCcaCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2NlxCloseCcaCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

//  End of File
