/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*           Provides phonebook SyncML command object methods.
*
*/


// INCLUDE FILES
#include "CPbkSyncMlCmd.h"

#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>
#include <CPbkFieldInfo.h>
#include <Phonebook.hrh>
#include <PbkUid.h>
#include <pbkdebug.h>

#include <coemain.h>
#include <AiwServiceHandler.h>      // AIW
#include <AiwCommon.hrh>            // AIW
#include <AiwGenericParam.hrh>

// ================= MEMBER FUNCTIONS =======================

// Default constructor
CPbkSyncMlCmd::CPbkSyncMlCmd(        
        TInt aCommandId,
        CAiwServiceHandler& aServiceHandler) :			
            iCommandId(aCommandId),
            iServiceHandler(aServiceHandler)
	{
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSyncMlCmd::CPbkSyncMlCmd(0x%x)"), this);    
    }

// Static constructor
CPbkSyncMlCmd* CPbkSyncMlCmd::NewL(        
        TInt aCommandId,
        CAiwServiceHandler& aServiceHandler)
    {
    CPbkSyncMlCmd* self = new(ELeave) CPbkSyncMlCmd(        
        aCommandId,
        aServiceHandler);
    return self;
    }

// Destructor
CPbkSyncMlCmd::~CPbkSyncMlCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSyncMlCmd::~CPbkSyncMlCmd(0x%x)"), this);
    }

void CPbkSyncMlCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSyncMlCmd::ExecuteLD(0x%x)"), this);

   	CleanupStack::PushL(this);
   
    // Set parameters for the AIW SyncMl
    CAiwGenericParamList& paramList = iServiceHandler.InParamListL();
    
    TAiwVariant variant;
    // Variant value not relevant when all contacts synchronized
    variant.Set(EGenericParamUnspecified);
    
    // Param value for Contacts syncrhonization
	TAiwGenericParam param(EGenericParamContactItem, variant);
	paramList.AppendL( param );
    
    // Relay the command to AIW for handling
        iServiceHandler.ExecuteMenuCmdL(
            iCommandId,
            paramList,
            iServiceHandler.OutParamListL(),
            0,      // No options used.
            NULL);  // No need for callback    

	// Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this

    }

void CPbkSyncMlCmd::ProcessFinished(MPbkBackgroundProcess& /*aProcess*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSyncMlCmd::ProcessFinished(0x%x)"), this);
    }

//  End of File
