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
*     Implements Call AIW interest item functionality.
*
*/


// INCLUDE FILES
#include "CPbkAiwInterestItemCall.h"

#include <CPbkContactEngine.h>
#include <CPbkSendKeyAddressSelect.h>
#include <Phonebook.hrh>
#include <CPbkAppGlobalsBase.h>
#include <MPbkCommand.h>
#include <MPbkCommandFactory.h>
#include <MenuFilteringFlags.h>
#include <MPbkAiwCommandObserver.h>
#include "CPbkCallTypeSelector.h"
#include <Phonebook.rsg>

#include <cntdef.h>
#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>
#include <eikmenup.h>
#include <FeatMgr.h>


/// Unnamed namespace for local definitions
namespace {

    /**
     * Sets parameter object's focused field and
     * default usage values.
     * @param aParams the parameter object to modify
     * @param aFocusedField the field to use as focused field
     */
    void SetParams(
        CPbkAddressSelect::TBaseParams& aParams,
        const TPbkContactItemField* aFocusedField)
        {
        if (aFocusedField)
            {
            aParams.SetFocusedField(aFocusedField);
            }
        else
            {
            aParams.SetUseDefaultDirectly(ETrue);
            }
        }
        
} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkAiwInterestItemCall::CPbkAiwInterestItemCall
        (CAiwServiceHandler& aServiceHandler,
        CPbkContactEngine& aEngine) : 
            CPbkAiwInterestItemBase(aServiceHandler),
            iEngine(aEngine)
    {
    }


CPbkAiwInterestItemCall* CPbkAiwInterestItemCall::NewL(
        CAiwServiceHandler& aServiceHandler,
        CPbkContactEngine& aEngine)
    {
    CPbkAiwInterestItemCall* self =
        new (ELeave) CPbkAiwInterestItemCall(aServiceHandler, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
void CPbkAiwInterestItemCall::ConstructL()
    {
    iSelector = CPbkCallTypeSelector::NewL(iEngine);
    }

CPbkAiwInterestItemCall::~CPbkAiwInterestItemCall()
    {
    delete iSelector;
    }

TBool CPbkAiwInterestItemCall::InitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane& aMenuPane,
        TUint aFlags)
    {
    TBool ret = EFalse;

    // Normal menu
    if (aResourceId == R_PHONEBOOK_CALL_MENU)
        {
        ret = ETrue;
           
	    // If the list is empty or it has marked items, hide call option
        if (!(aFlags & KPbkControlEmpty)
            && !(aFlags & KPbkItemsMarked))
            {
            // Let provider add its menu items to the menu
            iServiceHandler.InitializeMenuPaneL(
                aMenuPane,
                aResourceId,
                EPbkCmdLast,
                iServiceHandler.InParamListL());
            }
        else
            {
            aMenuPane.SetItemDimmed(KAiwCmdCall, ETrue);
            }
        }

    // Context menu
    else if (aResourceId == R_PHONEBOOK_CALL_CONTEXT_MENU)
        {
        ret = ETrue;
        // Hide the item by default
        aMenuPane.SetItemDimmed(KAiwCmdCall, ETrue);

        // Get empty parameter list
        CAiwGenericParamList& paramList =
            iServiceHandler.InParamListL();

        if (!(aFlags & KPbkControlEmpty))
            {
            if (aFlags & KPbkCurrentFieldPhoneNumber)
                {
                aMenuPane.SetItemDimmed(KAiwCmdCall, EFalse);
                }
            else if (aFlags & KPbkCurrentFieldVoipAddress)
                {
                // If we are on a Internet telephony field,
                // the Call UI must show only the VoIP option
                TAiwVariant variant;
                TAiwGenericParam param(EGenericParamSIPAddress, variant);
                paramList.AppendL(param);
                }
                                
            // Let provider add its menu items to the menu
            iServiceHandler.InitializeMenuPaneL(
                aMenuPane,
                aResourceId,
                EPbkCmdLast,
                paramList);
            }
        }

    return ret;
    }


TBool CPbkAiwInterestItemCall::HandleCommandL(
        TInt aMenuCommandId,
        const CContactIdArray& aContacts,
        const TPbkContactItemField* aFocusedField /* =NULL */,
        TInt aServiceCommandId /* =KNullHandle */,
        MPbkAiwCommandObserver* aObserver /*= NULL*/)
    {
    TBool ret = EFalse;
    
    // Only handle this command if service command is call,
    // note that this section is accessed also when the send key
    // is pressed and the initiated command object can then be POC
    // call also
    if (aServiceCommandId == KAiwCmdCall)
        {
        MPbkCommand* cmd = NULL;
        
        // Read the first contact of the array
        const CPbkContactItem* item =
            iEngine.ReadMinimalContactLC(aContacts[0]);
        
        // Because send key is used also to launch POC calls
        // we have to verify what was the selection and is
        // it ok to launch a telephony call
        if (iSelector->OkToLaunchTelephonyCall(aFocusedField))
            {
            cmd = CreateCallCmdObjectL
                (aMenuCommandId, *item, aContacts, aFocusedField);
            }
        else
            {
            cmd = CreatePocCmdObjectL
                (aMenuCommandId, aContacts, aFocusedField);
            }
            
        if (cmd)
            {
		    // Execute the command
		    cmd->ExecuteLD();

            // Inform observer
            if (aObserver)
                {
                aObserver->AiwCommandHandledL(aMenuCommandId,
                   aServiceCommandId, KErrNone);
                }
            }
        CleanupStack::PopAndDestroy(); // item
        ret = ETrue;
        }

    return ret;
    }

void CPbkAiwInterestItemCall::AttachL(
        TInt aMenuResourceId,
        TInt aInterestResourceId,
        TBool aAttachBaseServiceInterest)
    {
    // Always delegate to base class!
    CPbkAiwInterestItemBase::AttachL(aMenuResourceId,
        aInterestResourceId,
        aAttachBaseServiceInterest);
    }

MPbkCommand* CPbkAiwInterestItemCall::CreateCallCmdObjectL
        (TInt aMenuCommandId, const CPbkContactItem& aContact,
        const CContactIdArray& aContacts,
        const TPbkContactItemField* aFocusedField)
    {
    // We need a bunch of pre-initialized variables    
    MPbkCommand* cmd = NULL;
    TBool selectAccepted = ETrue;
    TBool okToLaunchTelephonyCall = ETrue;
	const TPbkContactItemField* selectedField = NULL;
    
    if (aMenuCommandId == EPbkCmdCall)
        {
        // Call launched with send key, before creating the address select
        // object, we have to spend some time to select which default to use
        const TPbkContactItemField* defaultField =
            iSelector->SelectDefaultToUse(aContact);
        CPbkSendKeyAddressSelect* phoneNumberSelect =
            new (ELeave) CPbkSendKeyAddressSelect(iEngine);
        CPbkSendKeyAddressSelect::TParams params(aContact, defaultField);
        SetParams(params, aFocusedField);
        selectAccepted = phoneNumberSelect->ExecuteLD(params);

        if (selectAccepted)
            {
            selectedField = params.SelectedField();

            // Because send key is used also to launch POC calls
            // we have to verify what was the selection and is
            // it ok to launch a telephony call
            okToLaunchTelephonyCall = iSelector->OkToLaunchTelephonyCall
                (selectedField);
            }
        }
        
    if (selectAccepted && okToLaunchTelephonyCall)
        {
        // Select was either accepted, or we are calling from
        // CallUI menus. In the latter case, the address select
        // will be shown later. Next, create the call command object.
	    cmd = CPbkAppGlobalsBase::InstanceL()->
		    CommandFactory().CreateCallCmdL(
		    aContact,
		    selectedField,
		    aFocusedField,
            aMenuCommandId,
		    iServiceHandler,
		    *iSelector);
        }
    else if (selectAccepted && !okToLaunchTelephonyCall)
        {
        // Select was accepted, but the selection indicates
        // we have to create a POC call
        cmd = CreatePocCmdObjectL
            (aMenuCommandId, aContacts, aFocusedField);
        }
    
    return cmd;
    }

    
MPbkCommand* CPbkAiwInterestItemCall::CreatePocCmdObjectL
        (TInt aMenuCommandId, const CContactIdArray& aContacts,
        const TPbkContactItemField* aFocusedField)
    {
    MPbkCommand* cmd = NULL;
    
    // We have to deduct the control flags for the POC cmd
    TUint controlFlags = 0;
    if (aFocusedField)
        {
        controlFlags |= KPbkInfoView;
        }
    
    // Create the POC call command object
    cmd = CPbkAppGlobalsBase::InstanceL()->
	    CommandFactory().CreatePocCmdL(aMenuCommandId,
        aContacts,
        controlFlags,
	    iServiceHandler,
	    *iSelector);
	    
    return cmd;
    }
    
//  End of File
