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
* Description:  Creates Phonebook 2 command objects.
*
*/


// INCLUDE FILES
#include "CPbk2CommandFactory.h"

// Phonebook 2
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionFactory.h>
#include <cpbk2commandactivator.h>

// include command handlers
#include "CPbk2SetToneCmd.h"
#include "cpbk2activatecntinfoviewcmd.h"

/// Unnamed namespace for local definitions
namespace {

typedef MPbk2Command* (CPbk2CommandFactory::*CommandCreationFunction)
    (MPbk2ContactUiControl&) const;

/// mapping table entries
struct TPbk2CommandIdMapping
    {
    /// Phonebook2 command id
    TPbk2CommandId iCommandId; 
    /// command handler factory function
    CommandCreationFunction iCreationFunctionL; 
    };

/// mapping table contains Phonebook2 command id's and function pointers to 
/// command handlers
static const TPbk2CommandIdMapping CommandIdMapping[] =
    {
    {EPbk2CmdPersonalRingingTone,
        &CPbk2CommandFactory::CreateSetToneCommandL},
    {EPbk2CmdOpenMeViews,
        &CPbk2CommandFactory::CreateActivateCntInfoViewCommandL},
    {EPbk2CmdLast,
        NULL}
    };

} /// namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbk2CommandFactory::CPbk2CommandFactory()
    {
    }

inline void CPbk2CommandFactory::ConstructL()
    {
    iExtensionManager = CPbk2UIExtensionManager::InstanceL();
    }

CPbk2CommandFactory* CPbk2CommandFactory::NewL()
    {
    CPbk2CommandFactory* self = new (ELeave) CPbk2CommandFactory;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbk2CommandFactory::~CPbk2CommandFactory()
    {
    Release(iExtensionManager);
    }

MPbk2Command* CPbk2CommandFactory::CreateCommandForIdL(
        TPbk2CommandId aCommandId,
        MPbk2ContactUiControl& aUiControl) const
    {
    MPbk2Command* cmd = iExtensionManager->FactoryL()->
            CreatePbk2CommandForIdL(aCommandId, aUiControl);
    if (!cmd)
        {
        // search mapping table for command handler
        for (TInt i = 0; CommandIdMapping[i].iCommandId != EPbk2CmdLast; ++i)
            {
            if (CommandIdMapping[i].iCommandId == aCommandId)
                {
                // command id handler found, create command object
                // through mapping table function pointer
                cmd = (this->*CommandIdMapping[i].iCreationFunctionL)(aUiControl);
                break;
                }
            }
        }

    if ( cmd )
        {
        // activator takes control of created command
        MPbk2Command* activator = 
            CPbk2CommandActivator::NewL( cmd, aUiControl );
        cmd = activator;
        }

    return cmd;
    }

MPbk2Command* CPbk2CommandFactory::CreateSetToneCommandL(
        MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2SetToneCmd::NewL(aUiControl);
    }

MPbk2Command* CPbk2CommandFactory::CreateActivateCntInfoViewCommandL(
            MPbk2ContactUiControl& aUiControl) const
    {
    return CPbk2ActivateCntInfoViewCmd::NewL(aUiControl);
    }

// End of File

