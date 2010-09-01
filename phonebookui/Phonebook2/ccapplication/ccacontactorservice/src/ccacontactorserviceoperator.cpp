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

    // Resolve special cases
    TRAP(err, ResolveSpecialCasesL(*result));

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
    iServiceId((TUint32)KErrNotFound),
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
// CCCAContactorServiceOperator::ResolveSpecialCasesL
// --------------------------------------------------------------------------
//
void CCCAContactorServiceOperator::ResolveSpecialCasesL(const TDesC& aFieldData)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveSpecialCasesL"));

    /*
     * Currently only VOIP ServiceId is needed to find out.
     */
    switch(iParameter->iCommTypeSelector)
        {
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            iServiceId = ResolveServiceIdL(aFieldData);
            break;
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveSpecialCasesL: Done."));
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::ResolveServiceIdL
// --------------------------------------------------------------------------
//
TUint32 CCCAContactorServiceOperator::ResolveServiceIdL(const TDesC& aFieldData)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveServiceIdL"));

    __ASSERT_ALWAYS (NULL != &aFieldData, User::Leave (KErrArgument));
    __ASSERT_ALWAYS (0 < aFieldData.Size(), User::Leave (KErrArgument));

    TUint32 ret = (TUint32)KErrNotFound;

    //LOGIC:
    /*
     * 1. Find out is there xSP prefix
     * 2. If there is, search service id
     * 3. If not, find out how many services support msisdn
     * 4. If only one, find use that service id
     * 5. If more than one, do not use service id.
     */

    TPtrC result;

    // 1. Find out is there xSP prefix
    if (ExtractServiceL(aFieldData, result))
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveServiceIdL: ExtractServiceL found."));

        // 2. If there is, search service id
        ret = SearchServiceIdL(result);
        }
    else
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveServiceIdL: ExtractServiceL not found."));

        // 3. If not, find out how many services support msisdn
        ret = ResolveMSISDNAddressingSupportedL();
        if ((TUint32)KErrNotFound != ret)
            {
            // 4. If only one, find use that service id
            CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveServiceIdL: ResolveMSISDNAddressingSupportedL only one service."));

            }
        else
            {
            // 5. If more than one or none found, do not use service id.
            CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveServiceIdL: ResolveMSISDNAddressingSupportedL more than one service."));

            }
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveServiceIdL: Done."));

    return ret;
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::ExtractServiceL
// --------------------------------------------------------------------------
//
TBool CCCAContactorServiceOperator::ExtractServiceL(
    const TDesC& aFieldData, TPtrC& aXSPId)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ExtractServiceL"));

    TBool found = EFalse;

    TInt pos = aFieldData.Find(KColon);
    if (pos >= 0)
        {
        // ok input
        aXSPId.Set(aFieldData.Left(pos));
        found = ETrue;
        }

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ExtractServiceL: Done."));

    return found;
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::SearchServiceIdL
// --------------------------------------------------------------------------
//
TUint32 CCCAContactorServiceOperator::SearchServiceIdL(const TDesC& aFieldData)
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::SearchServiceIdL"));

    TUint32 ret = (TUint32)KErrNotFound;
    TInt err = KErrNone;
    CDesCArrayFlat* nameArray = NULL;

    RIdArray ids;
    CleanupClosePushL(ids);

    nameArray = new (ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL(nameArray);

    CSPSettings* settings = CSPSettings::NewLC();

    err = settings->FindServiceIdsL(ids);
    User::LeaveIfError(err);
    err = settings->FindServiceNamesL(ids, *nameArray);
    User::LeaveIfError(err);

    TInt count = nameArray->MdcaCount();
    for (TInt i(0); i < count; i++)
        {
        // search the mathching service name
        TPtrC p = nameArray->MdcaPoint(i);
        if (!p.CompareF(aFieldData))
            {
            // Service found
            ret = ids[i];

            CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::SearchServiceIdL: Service found."));
            break;
            }
        }
    CleanupStack::PopAndDestroy(3); // ids, nameArray, settings

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::SearchServiceIdL: Done."));

    return ret;
    }

// --------------------------------------------------------------------------
// CCCAContactorServiceOperator::ResolveMSISDNAddressingSupportedL
// --------------------------------------------------------------------------
//
TUint32 CCCAContactorServiceOperator::ResolveMSISDNAddressingSupportedL()
    {
    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveMSISDNAddressingSupportedL"));

    /*
     * LOGIC:
     * -Find out services
     * -If only 1 service, return the serviceid
     * -If services are more than 1, then do not use serviceid.
     */

    TInt err = KErrNone;
    TInt foundNo = 0;
    TUint32 ret = (TUint32)KErrNotFound;

    RIdArray ids;
    CleanupClosePushL(ids);

    CSPSettings* settings = CSPSettings::NewLC();

    err = settings->FindServiceIdsL(ids);
    User::LeaveIfError(err);

    TInt count = ids.Count();
    for (TInt i(0); i < count; i++)
        {
        CSPProperty* property = CSPProperty::NewLC();

        // Find out property
        err = settings->FindPropertyL(ids[i],
            EPropertyServiceAttributeMask, *property);

        // If service have property
        if (KErrNone == err)
            {
            // read the value of mask property
            TInt mask = 0;
            err = property->GetValue(mask);
            if (KErrNone == err)
                {
                if ((mask & ESupportsMSISDNAddressing) &&
                    (mask & ESupportsInternetCall))
                    {
                    // Found one.
                    ret = ids[i];
                    foundNo++;

                    }// if mask
                }// if err
            }// if err

        CleanupStack::PopAndDestroy(property);
        }// for

    // If more than 1 service, do not use serviceid

    if (1 < foundNo)
    {
        ret = (TUint32)KErrNotFound;
    }

    CleanupStack::PopAndDestroy(2); // ids, settings

    CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ResolveMSISDNAddressingSupportedL: Done."));
    return ret;
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
    
    // If serviceid found, use it.
    if ((TUint32)KErrNotFound != iServiceId)
        {
        CCA_DP(KCCAContactorServiceLoggerFile, CCA_L("CCCAContactorServiceOperator::ExecuteL: ExecuteServiceL with serviceid"));
        CCAContactor::ExecuteServiceL(
            iParameter->iCommTypeSelector,
            aFieldData,
            iParameter->iFullName,
            iParameter->iContactLinkArray,
            iServiceId
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
