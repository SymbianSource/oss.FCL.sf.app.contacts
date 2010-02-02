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
*
*/


// INCLUDE FILES
#include <CPbkAppGlobalsBase.h>
#include <sendui.h>
#include <SenduiMtmUids.h>	// Send UI MTM uid's
#include <AiwGenericParam.h>

#include "CPbkCommandFactory.h"
#include "CPbkSendContactCmd.h"
#include "CPbkCallCmd.h"
#include "CPbkSendMultipleMessageCmd.h"
#include "CPbkGoToURLCmd.h"
#include "CPbkSettingsCmd.h"
#include "CPbkPocCmd.h"
#include "CPbkSyncMlCmd.h"

// ================= MEMBER FUNCTIONS =======================
CPbkCommandFactory::CPbkCommandFactory()
    {
    }

CPbkCommandFactory* CPbkCommandFactory::NewL()
    {
    return new (ELeave) CPbkCommandFactory;
    }

CPbkCommandFactory::~CPbkCommandFactory()
    {
    }

MPbkCommand* CPbkCommandFactory::CreateSendContactCmdL
        (TPbkSendingParams aParams,
        CPbkContactEngine& aEngine, CBCardEngine& aBCardEng,
        TContactItemId aContactId,
        TPbkContactItemField* aField /*=NULL*/)
    {    
	CPbkSendContactCmd* cmd =
		CPbkSendContactCmd::NewL(
		    aParams,
		    aEngine, aBCardEng,
            aContactId, aField);
    return cmd;
    }

MPbkCommand* CPbkCommandFactory::CreateSendContactsCmdL
        (TPbkSendingParams aParams, 
        CPbkContactEngine& aEngine, CBCardEngine& aBCardEng,
		const CContactIdArray& aContacts)
    {
	CPbkSendContactCmd* cmd = CPbkSendContactCmd::NewL(
        aParams, aEngine, aBCardEng, aContacts);
    return cmd;
    }

MPbkCommand* CPbkCommandFactory::CreateCallCmdL
        (const CPbkContactItem& aContactItem,
    	const TPbkContactItemField* aSelectedField,
    	const TPbkContactItemField* aFocusedField,
        const TInt aCommandId,
        CAiwServiceHandler& aServiceHandler,
        const CPbkCallTypeSelector& aCallTypeSelector)
    {
	CPbkCallCmd* cmd = CPbkCallCmd::NewL(
        aContactItem,
        aSelectedField,
        aFocusedField,
        aCommandId,
        aServiceHandler,
        aCallTypeSelector);
    return cmd;
    }

MPbkCommand* CPbkCommandFactory::CreateSendMultipleMessageCmdL(
            CPbkContactEngine& aEngine,
            TPbkSendingParams aParams,
			const CContactIdArray& aContacts,
			const TPbkContactItemField* aFocusedField,
			TBool aUseDefaultDirectly)
	{
	CPbkSendMultipleMessageCmd* cmd = CPbkSendMultipleMessageCmd::NewL(
        aEngine, aParams, aContacts, aFocusedField, aUseDefaultDirectly);
	return cmd;
	}

MPbkCommand* CPbkCommandFactory::CreateGoToURLCmdL
			(const CPbkContactItem& aContact,
			const TPbkContactItemField* aFocusedField)
	{
	CPbkGoToURLCmd* cmd = CPbkGoToURLCmd::NewL(aContact, aFocusedField);
	return cmd;
	}

MPbkCommand* CPbkCommandFactory::CreateSettingsCmdL(CPbkContactEngine& aEngine,
        MObjectProvider& aParent)
    {
    CPbkSettingsCmd* cmd = CPbkSettingsCmd::NewL(aEngine, aParent);
    return cmd;
    }


TBool CPbkCommandFactory::IsEmailEnabledL()
    {
    return CPbkAppGlobalsBase::InstanceL()->SendUiL()->ValidateServiceL
        (KSenduiMtmSmtpUid, TSendingCapabilities(0,0,0));
    }

MPbkCommand* CPbkCommandFactory::CreatePocCmdL(
        TInt aCommandId,
        const CContactIdArray& aContacts,
        TUint aControlFlags,
        CAiwServiceHandler& aServiceHandler,
        const CPbkCallTypeSelector& aCallTypeSelector)
    {    
    return CPbkPocCmd::NewL(aCommandId, aContacts,
        aControlFlags, aServiceHandler, aCallTypeSelector);
    }
    
MPbkCommand* CPbkCommandFactory::CreateSyncMlCmdL(                    
        TInt aCommandId,
        CAiwServiceHandler& aServiceHandler)
    {    
    return CPbkSyncMlCmd::NewL( aCommandId, aServiceHandler );
    }

// End of File
