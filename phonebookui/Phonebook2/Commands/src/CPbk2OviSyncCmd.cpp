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
* Description:  Phonebook 2 OviSync command object.
*
*/

// INCLUDE FILES
#include "CPbk2OviSyncCmd.h"

// System includes
#include <AiwServiceHandler.h>
#include <AiwGenericParam.hrh>

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2OviSyncCmd::CPbk2OviSyncCmd
// --------------------------------------------------------------------------
//
CPbk2OviSyncCmd::CPbk2OviSyncCmd( 
        TInt aCommandId, 
        CAiwServiceHandler& aServiceHandler ) :
    iCommandId( aCommandId ),
    iServiceHandler( aServiceHandler )
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2OviSyncCmd::CPbk2OviSyncCmd(0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2OviSyncCmd::~CPbk2OviSyncCmd
// --------------------------------------------------------------------------
//
CPbk2OviSyncCmd::~CPbk2OviSyncCmd()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2OviSyncCmd::~CPbk2OviSyncCmd(0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2OviSyncCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2OviSyncCmd* CPbk2OviSyncCmd::NewL
        ( TInt aCommandId, CAiwServiceHandler& aServiceHandler )
    {
    CPbk2OviSyncCmd* self = new( ELeave ) CPbk2OviSyncCmd( 
            aCommandId, aServiceHandler );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2OviSyncCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2OviSyncCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2OviSyncCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL( this );
    
    // Relay the command to AIW for handling
    iServiceHandler.ExecuteServiceCmdL(
        iCommandId,
        iServiceHandler.InParamListL(),
        iServiceHandler.OutParamListL(),
        0,      // No options used.
        NULL);  // No need for callback
    
    // Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this
    }

// --------------------------------------------------------------------------
// CPbk2OviSyncCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2OviSyncCmd::ResetUiControl(
        MPbk2ContactUiControl& /*aUiControl*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2OviSyncCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2OviSyncCmd::AddObserver( MPbk2CommandObserver& /*aObserver*/ )
    {
    // Do nothing
    }

// End of File
