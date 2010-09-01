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
* Description:  Creates Phonebook 2 command objects.
*
*/


// INCLUDE FILES
#include "CPbk2CommandFactory.h"

// Phonebook 2
#include <CPbk2UIExtensionManager.h>

// include command handlers
#include "CPbk2DeleteContactsCmd.h"
#include "CPbk2GoToURLCmd.h"
#include "CPbk2CreateNewContactCmd.h"
#include "CPbk2EditContactCmd.h"
#include "CPbk2SendMessageCmd.h"
#include "CPbk2SetThumbnailCmd.h"
#include "CPbk2AssignDefaultsCmd.h"
#include "CPbk2RemoveThumbnailCmd.h"
#include "CPbk2AssignSpeedDialCmd.h"
#include "CPbk2RemoveSpeedDialCmd.h"
#include "CPbk2SendContactCmd.h"
#include "CPbk2SetImageCmd.h"
#include "CPbk2ViewImageCmd.h"
#include "CPbk2RemoveImageCmd.h"
#include "CPbk2CopyContactsStarterCmd.h"
#include "CPbk2CopyNumberToClipboardCmd.h"
#include "CPbk2InfoDialogCmd.h"
#include "CPbk2CopyDetailToClipboardCmd.h" 
#include "CPbk2CopyAddressToClipboardCmd.h" 
#include "CPbk2MergeContactsCmd.h"
#include "CPbkRclCmd.h"  //RCL_ADD
// Send UI Uids
#include <SenduiMtmUids.h>

/// Unnamed namespace for local definitions
namespace {

typedef MPbk2Command* (CPbk2CommandFactory::*CommandCreationFunction)
    (MPbk2ContactUiControl&) const;

struct TPbk2CommandIdMapping
    {
    TPbk2CommandId iCommandId;
    CommandCreationFunction iCreationFunctionL;
    };

static const TPbk2CommandIdMapping CommandIdMapping[] =
    {
    {EPbk2CmdDeleteMe,
        &CPbk2CommandFactory::CreateDeleteContactsCommandL},
    {EPbk2CmdGoToURL,
        &CPbk2CommandFactory::CreateGoToURLCommandL},
    {EPbk2CmdCreateNew,
        &CPbk2CommandFactory::CreateNewContactCommandL},
    {EPbk2CmdRcl,  //RCL_ADD
        &CPbk2CommandFactory::CreateRclCommandL},
    {EPbk2CmdEditMe,
        &CPbk2CommandFactory::CreateEditContactCommandL},
    {EPbk2CmdWrite,
        &CPbk2CommandFactory::CreateSendMessageCommandL},
    {EPbk2CmdFetchThumbnail,
        &CPbk2CommandFactory::CreateSetThumbnailCommandL},
    {EPbk2CmdDefaultSettings,
        &CPbk2CommandFactory::CreateAssignDefaultsCommandL},
    {EPbk2CmdRemoveThumbnail,
        &CPbk2CommandFactory::CreateRemoveThumbnailCommandL},
    {EPbk2CmdAssignSpeedDial,
        &CPbk2CommandFactory::CreateAssignSpeedDialCommandL},
    {EPbk2CmdRemoveSpeedDial,
        &CPbk2CommandFactory::CreateRemoveSpeedDialCommandL},
    {EPbk2CmdSend,
        &CPbk2CommandFactory::CreateSendContactCommandL},
    {EPbk2CmdAddImage,
        &CPbk2CommandFactory::CreateAddImageCommandL},
    {EPbk2CmdViewImage,
        &CPbk2CommandFactory::CreateViewImageCommandL},
    {EPbk2CmdChangeImage,
        &CPbk2CommandFactory::CreateChangeImageCommandL},
    {EPbk2CmdRemoveImage,
        &CPbk2CommandFactory::CreateRemoveImageCommandL},
    {EPbk2CmdCopy,
        &CPbk2CommandFactory::CreateCopyContactsStarterCommandL},
    {EPbk2CmdCopyNumber,
        &CPbk2CommandFactory::CreateCopyNumberToClipboardCommandL},
    {EPbk2CmdPhonebook2Info,
        &CPbk2CommandFactory::CreateLaunchInfoDialogCommandL},
    {EPbk2CmdWriteNoQuery,
        &CPbk2CommandFactory::CreateSendMessageNoQueryCommandL},
    {EPbk2CmdCopyDetail,
        &CPbk2CommandFactory::CreateCopyDetailToClipboardCommandL},
    {EPbk2CmdCopyAddress,
        &CPbk2CommandFactory::CreateCopyAddressToClipboardCommandL},
    {EPbk2CmdMergeContacts,
        &CPbk2CommandFactory::CreateMergeContactsCommandL},
    {EPbk2CmdLast,
        NULL}
    };

} /// namespace

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CPbk2CommandFactory
// --------------------------------------------------------------------------
//
CPbk2CommandFactory::CPbk2CommandFactory()
    {
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::~CPbk2CommandFactory
// --------------------------------------------------------------------------
//
CPbk2CommandFactory::~CPbk2CommandFactory()
    {
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::NewL
// --------------------------------------------------------------------------
//
CPbk2CommandFactory* CPbk2CommandFactory::NewL()
    {
    CPbk2CommandFactory* self = new ( ELeave ) CPbk2CommandFactory;
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateCommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateCommandForIdL
        ( TPbk2CommandId aCommandId,
          MPbk2ContactUiControl& aUiControl ) const
    {
    MPbk2Command* result = NULL;
    for (TInt i = 0; CommandIdMapping[i].iCommandId != EPbk2CmdLast; ++i)
        {
        if (CommandIdMapping[i].iCommandId == aCommandId)
            {
            result = (this->*CommandIdMapping[i].iCreationFunctionL)
                (aUiControl);
            break;
            }
        }
                
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateDeleteContactsCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateDeleteContactsCommandL
        (MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2DeleteContactsCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateGoToURLCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateGoToURLCommandL
        (MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2GoToURLCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateNewContactCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateNewContactCommandL(
        MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2CreateNewContactCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateEditContactCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateEditContactCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2EditContactCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateSendMessageCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateSendMessageCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2SendMessageCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateSetThumbnailCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateSetThumbnailCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2SetThumbnailCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateAssignDefaultsCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateAssignDefaultsCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2AssignDefaultsCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateRemoveThumbnailCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateRemoveThumbnailCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2RemoveThumbnailCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateAddImageCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateAddImageCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2SetImageCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateViewImageCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateViewImageCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2ViewImageCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateChangeImageCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateChangeImageCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2SetImageCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateRemoveImageCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateRemoveImageCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2RemoveImageCmd::NewL(aUiControl);
    }


// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateAssignSpeedDialCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateAssignSpeedDialCommandL(
        MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2AssignSpeedDialCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateRemoveSpeedDialCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateRemoveSpeedDialCommandL(
        MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2RemoveSpeedDialCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateSendContactCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateSendContactCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2SendContactCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateCopyContactsStarterCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateCopyContactsStarterCommandL(
            MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2CopyContactsStarterCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateCopyNumberToClipboardCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateCopyNumberToClipboardCommandL(
            MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2CopyNumberToClipboardCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateCopyDetailToClipboardCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateCopyDetailToClipboardCommandL(
            MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2CopyDetailToClipboardCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateCopyAddressToClipboardCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateCopyAddressToClipboardCommandL(
            MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2CopyAddressToClipboardCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateLaunchInfoDialogCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateLaunchInfoDialogCommandL(
            MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2InfoDialogCmd::NewL(aUiControl);
    }

// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateSendMessageNoQueryCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateSendMessageNoQueryCommandL(
    MPbk2ContactUiControl& aUiControl) const
    {
    CPbk2SendMessageCmd* cmd = CPbk2SendMessageCmd::NewL(aUiControl);

    // Unified message editor used as default 
    // when message type not queried from the user
    cmd->SetMtmUid( KSenduiMtmUniMessageUid );
    return cmd;
    }
	
//RCL_ADD
// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateLaunchInfoDialogCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateRclCommandL(
            MPbk2ContactUiControl& aUiControl) const
    {
    return CPbkRclCmd::NewL(aUiControl);
    }
	
// --------------------------------------------------------------------------
// CPbk2CommandFactory::CreateMergeContactsCommandL
// --------------------------------------------------------------------------
//
MPbk2Command* CPbk2CommandFactory::CreateMergeContactsCommandL(
            MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2MergeContactsCmd::NewL(aUiControl);
    }

// End of File

