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
*     Implements SyncML AIW interest item functionality.
*
*/


// INCLUDE FILES
#include "CPbkAiwInterestItemSyncMl.h"

#include <CPbkContactEngine.h>
#include "CPbkPocAddressSelect.h"
#include <Phonebook.hrh>
#include <CPbkAppGlobalsBase.h>
#include <MPbkCommand.h>
#include <MPbkCommandFactory.h>
#include <MenuFilteringFlags.h>
#include <MPbkAiwCommandObserver.h>

#include <phonebook.rsg>

#include <cntdef.h>
#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>
#include <eikmenup.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkAiwInterestItemSyncMl::CPbkAiwInterestItemSyncMl
        (CAiwServiceHandler& aServiceHandler,
        CPbkContactEngine& aEngine) :
            CPbkAiwInterestItemBase(aServiceHandler),
            iEngine(aEngine)
    {
    }


CPbkAiwInterestItemSyncMl* CPbkAiwInterestItemSyncMl::NewL(
        CAiwServiceHandler& aServiceHandler,
        CPbkContactEngine& aEngine)
    {
    CPbkAiwInterestItemSyncMl* self =
        new (ELeave) CPbkAiwInterestItemSyncMl(aServiceHandler, aEngine);
    return self;
    }

CPbkAiwInterestItemSyncMl::~CPbkAiwInterestItemSyncMl()
    {
    }


TBool CPbkAiwInterestItemSyncMl::InitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane& aMenuPane,
        TUint aFlags)
    {
    TBool ret(EFalse);

    // Normal menu
    if (aResourceId == R_PHONEBOOK_SYNCML_MENU)
        {
        ret = ETrue;

	    // If the list is empty or it has marked items, hide syncMl option
        if (!(aFlags & KPbkControlEmpty)
            && !(aFlags & KPbkItemsMarked))
            {
            // Let provider add its menu items to the menu
            iServiceHandler.InitializeMenuPaneL(
                aMenuPane,
                aResourceId,
                KAiwCmdSynchronize,
                iServiceHandler.InParamListL());
            }
        else
            {
            aMenuPane.SetItemDimmed(KAiwCmdSynchronize, ETrue);
            }
        }
        
    return ret;
    }


TBool CPbkAiwInterestItemSyncMl::HandleCommandL(
        TInt aMenuCommandId,
        const CContactIdArray& /*aContacts*/,
        const TPbkContactItemField* /*aFocusedField*/ /* =NULL */,
        TInt aServiceCommandId /* =KNullHandle */,
        MPbkAiwCommandObserver* aObserver /*= NULL*/)
    {
    TBool ret(EFalse);
    
    // Only handle this command if service command is syncMl
    // aiw service command uid    
    if (aServiceCommandId == KAiwCmdSynchronize ||
        aServiceCommandId == KAiwCmdSynchronizeSelected)    
        {                        
		// Create the syncMl command object
		MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
			CommandFactory().CreateSyncMlCmdL(
            aMenuCommandId,
			iServiceHandler);
		// Execute the command
		cmd->ExecuteLD();
        // Inform observer
        if (aObserver)
            {
            aObserver->AiwCommandHandledL(aMenuCommandId,
               aServiceCommandId, KErrNone);
            }                    
        ret = ETrue;
        }

    return ret;
    }

void CPbkAiwInterestItemSyncMl::AttachL(
        TInt aMenuResourceId,
        TInt aInterestResourceId,
        TBool aAttachBaseServiceInterest)
    {
    // Always delegate to base class!
    CPbkAiwInterestItemBase::AttachL(aMenuResourceId,
        aInterestResourceId,
        aAttachBaseServiceInterest);
    }

//  End of File
