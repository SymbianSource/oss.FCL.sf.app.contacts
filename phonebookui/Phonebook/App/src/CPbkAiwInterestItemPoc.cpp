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
*     Implements Poc AIW interest item functionality.
*
*/


// INCLUDE FILES
#include "CPbkAiwInterestItemPoc.h"

#include <CPbkContactEngine.h>
#include "CPbkPocAddressSelect.h"
#include "CPbkCallTypeSelector.h"
#include "PbkUid.h"
#include <Phonebook.hrh>
#include <CPbkAppGlobalsBase.h>
#include <MPbkCommand.h>
#include <MPbkCommandFactory.h>
#include <MenuFilteringFlags.h>
#include <MPbkAiwCommandObserver.h>
#include <AiwPoCParameters.h>

#include <phonebook.rsg>
#include <AiwServiceHandler.h>
#include <AiwPoCParameters.hrh>
#include <eikmenup.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkAiwInterestItemPoc::CPbkAiwInterestItemPoc
        (CAiwServiceHandler& aServiceHandler,
        CPbkContactEngine& aEngine) :
            CPbkAiwInterestItemBase(aServiceHandler),
            iEngine(aEngine)
    {
    }


CPbkAiwInterestItemPoc* CPbkAiwInterestItemPoc::NewL(
        CAiwServiceHandler& aServiceHandler,
        CPbkContactEngine& aEngine)
    {
    CPbkAiwInterestItemPoc* self =
        new (ELeave) CPbkAiwInterestItemPoc(aServiceHandler, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkAiwInterestItemPoc::~CPbkAiwInterestItemPoc()
    {
    delete iSelector;
    }

void CPbkAiwInterestItemPoc::ConstructL()
    {
    iSelector = CPbkCallTypeSelector::NewL(iEngine);
    }

TBool CPbkAiwInterestItemPoc::InitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane& aMenuPane,
        TUint aFlags)
    {
    TBool ret = EFalse;
    
    iFlags = aFlags;
    // Normal menu
    if ( aResourceId == R_PHONEBOOK_POC_MENU )
        {
        ret = ETrue;

	    // If the list is empty or it has marked items, hide poc option
        if ( !(iFlags & KPbkControlEmpty) )
            {
            CAiwGenericParamList& params = iServiceHandler.InParamListL();
            TAiwPocParameterData pocParameter;
            pocParameter.iConsumerAppUid = TUid::Uid(KPbkUID3);
            pocParameter.iConsumerWindowGroup =
                CCoeEnv::Static()->RootWin().Identifier();
            pocParameter.iCommandId = 0;// not used with menu construction!
            
            // Select POC call type
            pocParameter.iPreferredMenu = 
                iSelector->SelectPocCallType(aFlags);
            
            // PoC parameter data always included
            TAiwGenericParam param = TAiwGenericParam(
                EGenericParamPoCData,
                TAiwVariant( TAiwPocParameterDataPckg(pocParameter)));
            params.AppendL(param);            

            // Let provider add its menu items to the menu
            iServiceHandler.InitializeMenuPaneL(
                aMenuPane,
                aResourceId,
                KAiwCmdPoC,
                params);
            }
        else
            {
            aMenuPane.SetItemDimmed(KAiwCmdPoC, ETrue);
            }
        }

    // Context menu
    else if (aResourceId == R_PHONEBOOK_POC_CONTEXT_MENU)
        {
        ret = ETrue;
        // Hide the item by default
        aMenuPane.SetItemDimmed(KAiwCmdPoC, ETrue);    
        
        TBool showTalkMenu = EFalse;
        // Show "Talk" in contact info context menu if field is a PoC field
        if ((aFlags & KPbkInfoView) && (aFlags & KPbkCurrentFieldPoc))
            {
            showTalkMenu = ETrue;
            }
        // Show "Talk" in contact views if there are marked items
        else if (aFlags & KPbkItemsMarked)
            {
            showTalkMenu = ETrue;
            }
        
        if (showTalkMenu)
            {
            aMenuPane.SetItemDimmed(KAiwCmdPoC, EFalse);
            
            CAiwGenericParamList& params = iServiceHandler.InParamListL();
            TAiwPocParameterData pocParameter;
            pocParameter.iConsumerAppUid = TUid::Uid(KPbkUID3);
            pocParameter.iConsumerWindowGroup =
                CCoeEnv::Static()->RootWin().Identifier();
            pocParameter.iCommandId = 0;// not used with menu construction!
            
            // Select POC call type
            pocParameter.iPreferredMenu = 
                iSelector->SelectPocCallType(aFlags);
            
            // PoC parameter data always included
            TAiwGenericParam param = TAiwGenericParam(
                EGenericParamPoCData,
                TAiwVariant( TAiwPocParameterDataPckg(pocParameter)));
            params.AppendL(param);            

            // Let provider add its menu items to the menu
            iServiceHandler.InitializeMenuPaneL(
                aMenuPane,
                aResourceId,
                KAiwCmdPoC,
                params);    
            }
        }

    return ret;
    }


TBool CPbkAiwInterestItemPoc::HandleCommandL(
        TInt aMenuCommandId,
        const CContactIdArray& aContacts,
        const TPbkContactItemField* /*aFocusedField*/,
        TInt aServiceCommandId ,
        MPbkAiwCommandObserver* aObserver)
    {
    TBool ret = EFalse;

    // Only handle this command if service command is poc
    if (aServiceCommandId == KAiwCmdPoC)
        {
		// Create the poc command object
		MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
			CommandFactory().CreatePocCmdL
			    (aMenuCommandId, aContacts, iFlags,
			    iServiceHandler, *iSelector);

		// Execute the command
		TRAPD(res, cmd->ExecuteLD());

        // Inform observer
        if (aObserver)
            {
            aObserver->AiwCommandHandledL(aMenuCommandId,
               aServiceCommandId, res);
            }
        ret = ETrue;
        }

    return ret;
    }

void CPbkAiwInterestItemPoc::AttachL(
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
