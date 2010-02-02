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
* Description:  Phonebook 2 SyncML command object.
*
*/


// INCLUDE FILES
#include "CPbk2SyncMlCmd.h"

// System includes
#include <AiwServiceHandler.h>
#include <AiwGenericParam.hrh>

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2SyncMlCmd::CPbk2SyncMlCmd
// --------------------------------------------------------------------------
//
CPbk2SyncMlCmd::CPbk2SyncMlCmd( 
        TInt aCommandId, 
        CAiwServiceHandler& aServiceHandler ) :
    iCommandId( aCommandId ),
    iServiceHandler( aServiceHandler )
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SyncMlCmd::CPbk2SyncMlCmd(0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2SyncMlCmd::~CPbk2SyncMlCmd
// --------------------------------------------------------------------------
//
CPbk2SyncMlCmd::~CPbk2SyncMlCmd()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SyncMlCmd::~CPbk2SyncMlCmd(0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2SyncMlCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2SyncMlCmd* CPbk2SyncMlCmd::NewL
        ( TInt aCommandId, CAiwServiceHandler& aServiceHandler )
    {
    CPbk2SyncMlCmd* self = new( ELeave ) CPbk2SyncMlCmd( 
            aCommandId, aServiceHandler );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SyncMlCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2SyncMlCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2SyncMlCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL( this );

    // Set parameters for the AIW SyncML
    CAiwGenericParamList& paramList = iServiceHandler.InParamListL();

    TAiwVariant variant;
    variant.Set( EGenericParamUnspecified );
    TAiwGenericParam param( EGenericParamContactItem, variant );
    paramList.AppendL( param );

    // Relay the command to AIW for handling
    iServiceHandler.ExecuteMenuCmdL(
        iCommandId, paramList,
        iServiceHandler.OutParamListL(),
        0,      // No options used.
        NULL);  // No need for callback

    // Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this
    }

// --------------------------------------------------------------------------
// CPbk2SyncMlCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2SyncMlCmd::ResetUiControl(
        MPbk2ContactUiControl& /*aUiControl*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2SyncMlCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2SyncMlCmd::AddObserver( MPbk2CommandObserver& /*aObserver*/ )
    {
    // Do nothing
    }

// End of File
