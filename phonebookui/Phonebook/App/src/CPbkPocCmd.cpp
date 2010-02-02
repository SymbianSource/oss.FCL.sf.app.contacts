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
*           Provides phonebook poc command object methods.
*
*/


// INCLUDE FILES
#include "CPbkPocCmd.h"
#include "PbkUid.h"
#include <pbkdebug.h>
#include <MenuFilteringFlags.h>
#include <Phonebook.hrh>
#include <coemain.h>
#include "CPbkCallTypeSelector.h"
#include <AiwServiceHandler.h>      // AIW

// ================= MEMBER FUNCTIONS =======================

// Default constructor
CPbkPocCmd::CPbkPocCmd(
        TInt aCommandId,
        const CContactIdArray& aContacts,
        TUint aControlFlags,
        CAiwServiceHandler& aServiceHandler,
        const CPbkCallTypeSelector& aSelector) :
            iCommandId(aCommandId),
            iServiceHandler(aServiceHandler),
            iContacts(aContacts),
            iSelector(aSelector),            
            iFlags(aControlFlags)                        
	{
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkPocCmd::CPbkPocCmd(0x%x)"), this);
    }

// Static constructor
CPbkPocCmd* CPbkPocCmd::NewL(
        TInt aCommandId,
        const CContactIdArray& aContacts,
        TUint aControlFlags,
        CAiwServiceHandler& aServiceHandler,
        const CPbkCallTypeSelector& aSelector)
    {
    CPbkPocCmd* self = new(ELeave) CPbkPocCmd
        (aCommandId, aContacts, aControlFlags,
        aServiceHandler, aSelector);
    return self;
    }

// Destructor
CPbkPocCmd::~CPbkPocCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkPocCmd::~CPbkPocCmd(0x%x)"), this);
    }

void CPbkPocCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkPocCmd::ExecuteLD(0x%x)"), this);

   	CleanupStack::PushL(this);
   	
    CAiwGenericParamList& params = iServiceHandler.InParamListL();
    TAiwPocParameterData pocParameter;
    pocParameter.iConsumerAppUid = TUid::Uid(KPbkUID3);
    pocParameter.iConsumerWindowGroup =
        CCoeEnv::Static()->RootWin().Identifier();
        
    pocParameter.iCommandId = 0;

    // Select POC call type
    pocParameter.iPreferredMenu = 
        iSelector.SelectPocCallType(iFlags);
    
    // PoC launched using PoC-key
    if (iCommandId == EPbkCmdPoc)
        {
        // Must select one type when using PoC key
        pocParameter.iCommandId = 
            iSelector.SelecPocCallTypeForPocKeyPress(iFlags);
        }
    
    TAiwGenericParam param = TAiwGenericParam(
        EGenericParamPoCData,
        TAiwVariant( TAiwPocParameterDataPckg(pocParameter)));
    params.AppendL(param);            

    const TInt count(iContacts.Count());
    for (TInt i(0); i < count; ++i)
        {
        TAiwGenericParam addressParam = TAiwGenericParam(
        EGenericParamContactItem,
        TAiwVariant(iContacts[i]));
        params.AppendL(addressParam);
        }
    
    // PoC launched using PoC-key
    if (iCommandId == EPbkCmdPoc)
        {
        iServiceHandler.ExecuteServiceCmdL(
            KAiwCmdPoC,
            params,
            iServiceHandler.OutParamListL(), 
            0,
            NULL );
        }
    else // from menu
        {
        // Relay the command to AIW for handling
        iServiceHandler.ExecuteMenuCmdL(
            iCommandId,
            params,
            iServiceHandler.OutParamListL(), 
            0,      // No options used.
            NULL);  // No need for callback       
        }    

	// Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this
    }
        
void CPbkPocCmd::ProcessFinished(MPbkBackgroundProcess& /*aProcess*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkPocCmd::ProcessFinished(0x%x)"), this);
    }

//  End of File  
