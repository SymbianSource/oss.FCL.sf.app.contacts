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
* Description:  Phonebook 2 OviSync AIW interest item.
*
*/

#include "CPbk2AiwInterestItemOviSync.h"

// Phonebook 2
#include "CPbk2OviSyncCmd.h"
#include <Pbk2Commands.hrh>
#include <MPbk2Command.h>
#include <MPbk2ContactUiControl.h>
#include <Pbk2Commands.rsg>
#include "Pbk2InternalCommands.hrh"

// System includes
#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>
#include <eikmenup.h>

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemOviSync::CPbk2AiwInterestItemOviSync
// --------------------------------------------------------------------------
//
inline CPbk2AiwInterestItemOviSync::CPbk2AiwInterestItemOviSync
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler ) :
            CPbk2AiwInterestItemBase( aInterestId, aServiceHandler )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemOviSync::~CPbk2AiwInterestItemOviSync
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemOviSync::~CPbk2AiwInterestItemOviSync()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemOviSync::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemOviSync* CPbk2AiwInterestItemOviSync::NewL
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler )
    {
    CPbk2AiwInterestItemOviSync* self =
        new ( ELeave ) CPbk2AiwInterestItemOviSync
            ( aInterestId, aServiceHandler );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemOviSync::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemOviSync::DynInitMenuPaneL
        ( const TInt aResourceId, CEikMenuPane& aMenuPane,
          const MPbk2ContactUiControl& aControl )
    {
    TBool ret = EFalse;
    
    if ( aResourceId == R_PHONEBOOK2_OVISYNC_MENU )
        {
        ret = ETrue;
        // If the list has marked items, hide ovisync option
        if ( !aControl.ContactsMarked() )
            {
            // Let provider add its menu items to the menu
            iServiceHandler.InitializeMenuPaneL(
                aMenuPane,
                aResourceId,
                EPbk2CmdLast,
                iServiceHandler.InParamListL() );
            }
        else
            {
            aMenuPane.SetItemDimmed( KOtasAiwCmdSynchronize, ETrue );
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemOviSync::HandleCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemOviSync::HandleCommandL
        ( const TInt aMenuCommandId, MPbk2ContactUiControl& /*aControl*/,
          TInt aServiceCommandId  )
    {
    TBool ret = EFalse;

    // Only handle this command if the service command is Ovisync based
    if ( aServiceCommandId == KOtasAiwCmdSynchronize || 
            aServiceCommandId == KOtasAiwCmdSettings )
        {
        // Create the Ovisync command object
        MPbk2Command* cmd = CPbk2OviSyncCmd::NewL(
            aMenuCommandId, iServiceHandler );
        // Execute the command
        cmd->ExecuteLD();
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemOviSync::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemOviSync::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemOviSync::RunL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemOviSync::RunL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemOviSync::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2AiwInterestItemOviSync::RunError(TInt aError)
    {
    CCoeEnv::Static()->HandleError(aError);
    return KErrNone;
    }

// End of File
