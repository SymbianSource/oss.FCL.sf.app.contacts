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
* Description:  Phonebook 2 SyncML AIW interest item.
*
*/


#include "CPbk2AiwInterestItemSyncMl.h"

// Phonebook 2
#include "CPbk2SyncMlCmd.h"
#include <Pbk2Commands.hrh>
#include <MPbk2Command.h>
#include <MPbk2ContactUiControl.h>
#include <Pbk2Commands.rsg>

// System includes
#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>
#include <eikmenup.h>

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSyncMl::CPbk2AiwInterestItemSyncMl
// --------------------------------------------------------------------------
//
inline CPbk2AiwInterestItemSyncMl::CPbk2AiwInterestItemSyncMl
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler ) :
            CPbk2AiwInterestItemBase( aInterestId, aServiceHandler )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSyncMl::~CPbk2AiwInterestItemSyncMl
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemSyncMl::~CPbk2AiwInterestItemSyncMl()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSyncMl::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwInterestItemSyncMl* CPbk2AiwInterestItemSyncMl::NewL
        ( TInt aInterestId, CAiwServiceHandler& aServiceHandler )
    {
    CPbk2AiwInterestItemSyncMl* self =
        new ( ELeave ) CPbk2AiwInterestItemSyncMl
            ( aInterestId, aServiceHandler );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSyncMl::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemSyncMl::DynInitMenuPaneL
        ( const TInt aResourceId, CEikMenuPane& aMenuPane,
          const MPbk2ContactUiControl& aControl )
    {
    TBool ret = EFalse;

    // Normal menu
    if ( aResourceId == R_PHONEBOOK2_SYNCML_MENU )
        {
        ret = ETrue;
        // If the list has marked items, hide SyncML option
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
            aMenuPane.SetItemDimmed( KAiwCmdSynchronize, ETrue );
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSyncMl::HandleCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwInterestItemSyncMl::HandleCommandL
        ( const TInt aMenuCommandId, MPbk2ContactUiControl& /*aControl*/,
          TInt aServiceCommandId  )
    {
    TBool ret = EFalse;

    // Only handle this command if the service command is SyncML based
    if ( aServiceCommandId == KAiwCmdSynchronize ||
         aServiceCommandId == KAiwCmdSynchronizeSelected )
        {
        // Create the syncMl command object
        MPbk2Command* cmd = CPbk2SyncMlCmd::NewL(
            aMenuCommandId, iServiceHandler );
        // Execute the command
        cmd->ExecuteLD();
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSyncMl::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemSyncMl::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2AiwInterestItemSyncMl::RunL
// --------------------------------------------------------------------------
//
void CPbk2AiwInterestItemSyncMl::RunL()
    {
    // Do nothing
    }

// End of File
