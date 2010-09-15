/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for handling service logic.
 *
*/

#include <e32std.h>
#include <s32mem.h>
#include <spsettingsvoiputils.h>

#include "ccacontactorserviceheaders.h"

// ================= MEMBER FUNCTIONS =======================
//

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::NewLC
// --------------------------------------------------------------------------
//
CCCAContactorServiceOperator* CCCAContactorServiceOperator::NewLC()
    {
    CCCAContactorServiceOperator* self = new (ELeave) CCCAContactorServiceOperator();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::NewL
// --------------------------------------------------------------------------
//
CCCAContactorServiceOperator* CCCAContactorServiceOperator::NewL()
    {
    CCCAContactorServiceOperator* self = CCCAContactorServiceOperator::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::~CCCAContactorServiceOperator
// --------------------------------------------------------------------------
//
CCCAContactorServiceOperator::~CCCAContactorServiceOperator()
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::~CCCAContactorServiceOperator"));

    delete iPopupHandler;
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::Execute
// --------------------------------------------------------------------------
//
void CCCAContactorServiceOperator::Execute(
    const CCAContactorService::TCSParameter& aParameter)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::Execute"));

    iParameter = &aParameter;

    /**
     * LOGIC:
     * 1. Launch AIW popup
     * 2. Special cases: Find out serviceid for voip
     * 3. Launch communication service
     *
     * All actions are trapped here.
     */
    TDesC* result = NULL;
    TInt err( KErrNone );
    TBool hasSelectedField( EFalse );
    
    // Check use field control flag
    if (CCAContactorService::TCSParameter::EUseFieldParam &
        iParameter->iControlFlag)
        {
        hasSelectedField = ETrue;
        }
    
    if ( !hasSelectedField )
        {
        TBool useDefaults = EFalse;

        // Check default control flag
        if (CCAContactorService::TCSParameter::EEnableDefaults &
            iParameter->iControlFlag)
            {
            useDefaults = ETrue;
            }

        // Launch popup
        TRAP(err, result = &iPopupHandler->LaunchPopupL(
            iParameter->iContactLinkArray,
            CommunicationEnumMapper(iParameter->iCommTypeSelector),
            useDefaults));
        
        if (result != NULL)
        	{
        	isSelected = ETrue;
        	}
        
        if (KErrNone != err)
            {
            CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ExecuteL: LaunchPopupL: Leaves %d."), err);
            return;
            }
        }
    else
        {
        __ASSERT_ALWAYS (NULL != iParameter->iSelectedField, User::Leave (KErrArgument));
        __ASSERT_ALWAYS (0 < iParameter->iSelectedField->Size(), User::Leave (KErrArgument));
        result = iParameter->iSelectedField;
        }

    if (KErrNone != err)
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ExecuteL: ResolveSpecialCasesL: Leaves %d."), err);
        return;
        }

    // Launch communication service
    TRAP(err, LaunchCommunicationMethodL(*result));

    if (KErrNone != err)
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ExecuteL: LaunchCommunicationMethodL: Leaves %d."), err);
        return;
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::Execute: Done."));
    }


TBool CCCAContactorServiceOperator::IsSelected()
	{
	return isSelected;
	}
// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::CCCAContactorServiceOperator
// --------------------------------------------------------------------------
//
CCCAContactorServiceOperator::CCCAContactorServiceOperator():
    isSelected(EFalse)
    {
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::ConstructL
// --------------------------------------------------------------------------
//
void CCCAContactorServiceOperator::ConstructL()
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ConstructL"));

    iPopupHandler = CCCAContactorPopupHandler::NewL();

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ConstructL: Done."));
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::LaunchCommunicationMethodL
// --------------------------------------------------------------------------
//
void CCCAContactorServiceOperator::LaunchCommunicationMethodL(
    const TDesC& aFieldData)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::LaunchCommunicationMethodL"));

    __ASSERT_ALWAYS (NULL != &iParameter->iFullName, User::Leave (KErrArgument));
    
    // If there is a default service, use the service 
    TUint serviceId = 0;
    CSPSettingsVoIPUtils* spSettings = CSPSettingsVoIPUtils::NewLC();
    
    if ( ( iParameter->iCommTypeSelector == VPbkFieldTypeSelectorFactory::EVOIPCallSelector ) &&
    		( !spSettings->GetPreferredService( serviceId ) ) )
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ExecuteL: ExecuteServiceL with serviceid"));
        CCAContactor::ExecuteServiceL(
            iParameter->iCommTypeSelector,
            aFieldData,
            iParameter->iFullName,
            iParameter->iContactLinkArray,
            serviceId
            );
        }
    else
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ExecuteL: ExecuteServiceL without serviceid"));
        CCAContactor::ExecuteServiceL(
            iParameter->iCommTypeSelector,
            aFieldData,
            iParameter->iFullName,
            iParameter->iContactLinkArray);
    	}
    
    CleanupStack::PopAndDestroy( spSettings );

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::LaunchCommunicationMethodL: Done."));
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::CommunicationEnumMapper
// --------------------------------------------------------------------------
//
TAiwCommAddressSelectType CCCAContactorServiceOperator::CommunicationEnumMapper(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommTypeSelector)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::CommunicationEnumMapper"));

    TAiwCommAddressSelectType type;

    switch(aCommTypeSelector)
        {
        case VPbkFieldTypeSelectorFactory::EEmptySelector:
            type = EAiwCommEmpty;
            break;

        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
            type = EAiwCommVoiceCall;
            break;

        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
            type = EAiwCommUniEditor;
            break;

        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
            type = EAiwCommEmailEditor;
            break;

        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
            type = EAiwCommInstantMessaging;
            break;

        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            type = EAiwCommVOIPCall;
            break;

        case VPbkFieldTypeSelectorFactory::EURLSelector:
            type = EAiwCommURL;
            break;

        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
            type = EAiwCommVideoCall;
            break;

        default:
            CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::CommunicationEnumMapper: default -> Empty mapped."));
            type = EAiwCommEmpty;
            break;
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::CommunicationEnumMapper: Done."));

    return type;
    }

// End of file
