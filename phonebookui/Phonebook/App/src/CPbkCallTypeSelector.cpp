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
*           Call type selector.
*
*/


// INCLUDE FILES
#include "CPbkCallTypeSelector.h"
#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>
#include <Phonebook.hrh>
#include <CPbkFieldInfo.h>
#include <MenuFilteringFlags.h>
#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>
#include <AiwPoCParameters.hrh>
#include <CPbkContactEngine.h>
#include <CPbkConstants.h>
#include <PbkGlobalSettingFactory.h>
#include <MPbkGlobalSetting.h>
#include <FeatMgr.h>


/// Unnamed namespace for local definitions
namespace {

#define KPbkNoDefaults                  0x00000000
#define KPbkVoiceCallDefault            0x00000001
#define KPbkVideoCallDefault            0x00000002
#define KPbkVoipCallDefault             0x00000004
#define KPbkPocCallDefault              0x00000008

const TInt KInternetCallPreferred = 1;

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkCallTypeSelector::CPbkCallTypeSelector
        (CPbkContactEngine& aEngine) :
        iEngine(aEngine)
    {
    }
        
CPbkCallTypeSelector::~CPbkCallTypeSelector()
    {
	if (iPersistentSetting)
        {
        iPersistentSetting->Close();
        delete iPersistentSetting;
        }    
    }

CPbkCallTypeSelector* CPbkCallTypeSelector::NewL
        (CPbkContactEngine& aEngine)
    {
    CPbkCallTypeSelector* self =
        new(ELeave) CPbkCallTypeSelector(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }
    
void CPbkCallTypeSelector::ConstructL()
    {
    iPersistentSetting = 
        PbkGlobalSettingFactory::CreatePersistentSettingL();
    iPersistentSetting->
        ConnectL(MPbkGlobalSetting::ERichCallSettingsCategory);
    }


TInt CPbkCallTypeSelector::SelectCallType
        (const TInt aCommandId, const CPbkContactItem& aContact,
        const TPbkContactItemField* aFocusedField,
        const TPbkContactItemField& aSelectedField) const
    {
    // Default to voice call
    TInt ret = EAiwVoice;
    
    // If the command was launched via Call UI menu we do not
    // need to apply our own logic for call type selection.
    // But if the command was not launched via menu but via the send key
    // (which is the case when iCommandId is EPbkCmdCall), we
    // need to deduct what kind of a call to launch.
    if (aCommandId == EPbkCmdCall)
        {
        // If there was no focused field, the call was launched from names list
        if (!aFocusedField)
            {
            ret = SelectCallType(aContact, aSelectedField);
            }
        // There was a focused field, so call was launched from info view
        else
            {
            ret = SelectCallType(aSelectedField);
            }
        }
        
    return ret;        
    }
    
TInt CPbkCallTypeSelector::OkToLaunchTelephonyCall
        (const TPbkContactItemField* aSelectedField) const
    {
    // Default to telephony ok
    TBool ret = ETrue;
    
    // Get the field's id.
    if (aSelectedField)
        {
        TPbkFieldId fieldId = aSelectedField->FieldInfo().FieldId();

        if (fieldId == EPbkFieldIdPushToTalk)
            {
            // In case of PTT field, the POC call is to be launched
            ret = EFalse;
            }
        }
        
    return ret;
    }
    
TInt CPbkCallTypeSelector::SelectPocCallType
        (TUint aControlFlags) const
    {
    TInt ret = 0;

    // If there is a new type then remember to check the function
    // SelecPocCallTypeForPocKeyPress below too.
    if (aControlFlags & KPbkGroupsView)
        {
        ret = EAiwPoCCmdTalkGroup; 
        }
    else
        {
        if (!(aControlFlags & KPbkInfoView))
            {
            ret = EAiwPoCCmdTalkMany;            
            }
        
        if (!(aControlFlags & KPbkItemsMarked))
            {
            ret |= EAiwPoCCmdTalk1to1 |
                EAiwPoCCmdSendCallBackRequest;
            }        
        }

    return ret;
    }

TInt CPbkCallTypeSelector::SelecPocCallTypeForPocKeyPress(
        TUint aControlFlags) const
    {
    // There is no specified logic for selecting a type.
    // Define some priority for types...
    TInt all = SelectPocCallType(aControlFlags);
    if (all & EAiwPoCCmdTalkGroup)
        {
        return EAiwPoCCmdTalkGroup;
        }
    else if (all & EAiwPoCCmdTalk1to1)
        {
        return EAiwPoCCmdTalk1to1;
        }            
    else if (all & EAiwPoCCmdTalkMany)
        {
        return EAiwPoCCmdTalkMany;
        }
    else if (all & EAiwPoCCmdSendCallBackRequest)
        {
        return EAiwPoCCmdSendCallBackRequest;
        }
    else
        {
        return all;
        }
    }
    
const TPbkContactItemField* CPbkCallTypeSelector::SelectDefaultToUse(
    const CPbkContactItem& aContact) const
    {
    TPbkContactItemField* field = NULL;
    TInt defaultFlags = GetDefaults(aContact);

    if ((defaultFlags & KPbkVoiceCallDefault) &&
        (defaultFlags & KPbkVoipCallDefault))
        {
        // Use preffered telephony setting
        // to deduct which default to use
        if (SelectBetweenCsAndVoip() == EAiwVoIPCall)
            {
            field = aContact.DefaultVoipField();
            }
        else
            {
            field = aContact.DefaultPhoneNumberField();
            }
        }
    else if (defaultFlags & KPbkVoiceCallDefault)
        {
        // Voice overrides everything else
        field = aContact.DefaultPhoneNumberField();
        }
    else if (defaultFlags & KPbkVoipCallDefault)
        {
        // VoIP overrides everything but voice
        field = aContact.DefaultVoipField();
        }
    else if (defaultFlags & KPbkVideoCallDefault)
        {
        // Video overrides POC
        field = aContact.DefaultVideoNumberField();
        }
    else if (defaultFlags & KPbkPocCallDefault)
        {
        // Only POC default exists, lets use it
        field = aContact.DefaultPocField();
        }

    return field;
    }


inline TInt CPbkCallTypeSelector::SelectCallType
        (const CPbkContactItem& aContact,
        const TPbkContactItemField& aSelectedField) const
    {
    // Default to voice call
    TInt ret = EAiwVoice;

    // Get the defaults    
    TInt defaultFlags = GetDefaults(aContact);

    if ((defaultFlags & KPbkVoiceCallDefault) &&
        (defaultFlags & KPbkVoipCallDefault))
        {
        // Both the two dominating defaults are available.
        // Address select was not shown, the call type
        // depends on the preferred telephony setting.
        ret = SelectBetweenCsAndVoip();
        }
    else if ((defaultFlags & KPbkVideoCallDefault) &&
        (defaultFlags & KPbkVoipCallDefault))
        {
        // Video and VoIP defaults are available.
        // VoIP default overrides video default.
        ret = EAiwVoIPCall;
        }
    else if ((defaultFlags & KPbkVoiceCallDefault) &&
        (defaultFlags & KPbkVideoCallDefault))
        {
        // Voice and video defaults are available.
        // The user was not shown a selection list.
        // Video type is ignored, the voice dominates here.
        // We must however select between CS and VoIP.
        ret = SelectBetweenCsAndVoip();
        }
    else if (defaultFlags & KPbkVoiceCallDefault)
        {
        // Voice default available without VoIP default,
        // we must however select between CS and VoIP
        ret = SelectBetweenCsAndVoip();
        }
    else if (defaultFlags & KPbkVoipCallDefault)
        {
        // VoIP default available without Voice default,
        // the call type is VoIP
        ret = EAiwVoIPCall;
        }        
    else if (defaultFlags & KPbkVideoCallDefault)
        {
        // Video default available, without Voice or VoIP
        // defaults, in this case we launch video call
        ret = EAiwForcedVideo;
        }
    else if (!defaultFlags)
        {
        // No defaults, so select based on selected field type only.
        // The preferred telephony is checked and taken into account
        // in this case.
        ret = SelectCallTypeBasedOnFieldType(aSelectedField, ETrue);
        }

    return ret;
    }

inline TInt CPbkCallTypeSelector::SelectCallType
        (const TPbkContactItemField& aSelectedField) const
    {
    // When there is clearly a selected field,
    // the call type of course depends on the type of the field.
    // We must however select between CS and VOIP.
    TInt ret = SelectCallTypeBasedOnFieldType(aSelectedField, ETrue);
    return ret;
    }

/**
 * This function selects the call type by the type
 * of passed aSelectedField alone.
 * @param aSelectBetweenCsAndVoip If ETrue, automated
 * selection between CS and VoIP is done if the
 * field in question is a normal phone number field.
 */    
inline TInt CPbkCallTypeSelector::SelectCallTypeBasedOnFieldType
        (const TPbkContactItemField& aSelectedField,
        TBool aSelectBetweenCsAndVoip) const
    {
    // Default to voice call
    TInt ret = EAiwVoice;
    
    // Get the field's id.
    TPbkFieldId fieldId = aSelectedField.FieldInfo().FieldId();

    // Deduct the call type based on the field id
    switch (fieldId)
        {
        case EPbkFieldIdVOIP:           // FALLTHROUGH
        case EPbkFieldIdShareView:      // FALLTHROUGH
        case EPbkFieldIdSIPID:
        // EPbkFieldIdPushToTalk will launch POC call and
        // it is not handled here
            {
            ret = EAiwVoIPCall;
            break;
            }
        case EPbkFieldIdPhoneNumberVideo:
            {
            ret = EAiwForcedVideo;
            break;
            }
        default:
            {
            if (aSelectBetweenCsAndVoip)
                {
                // We are dealing with a normal phone number field,
                // the call type must be checked nevertheless
                ret = SelectBetweenCsAndVoip();
                }
            break;
            }
        
        }

    return ret;        
    }

/**
 * This function selects whether a CS call or a VoIP call
 * should be created. It depends on preferred telephony setting.
 */
inline TInt CPbkCallTypeSelector::SelectBetweenCsAndVoip() const
    {
    // Default to voice call
    TInt ret = EAiwVoice;
    
	TInt preferred = KErrNotFound;
    iPersistentSetting->Get
        (MPbkGlobalSetting::EPreferredTelephony, preferred);

    // VoIP type is preferred only if the setting is,
    // KInternetCallPreferred. In other cases, like when the setting
    // is not found, voice call is preferred.
    switch (preferred)
        {
        case KInternetCallPreferred:
            {
            ret = EAiwVoIPCall;
            break;
            }
        default:
            {
            break;
            }
        }

    return ret;
    }
    
inline TInt CPbkCallTypeSelector::GetDefaults
        (const CPbkContactItem& aContact) const
    {
    TInt defaultFlags = KPbkNoDefaults;
    
    if (aContact.DefaultPhoneNumberField())
        {
        defaultFlags |= KPbkVoiceCallDefault;
        }
    if (FeatureManager::FeatureSupported(KFeatureIdCsVideoTelephony))
        {
        if (aContact.DefaultVideoNumberField())
            {
            defaultFlags |= KPbkVideoCallDefault;
            }
        }
    if (FeatureManager::FeatureSupported(KFeatureIdCommonVoip))
        {
        if (aContact.DefaultVoipField())
            {
            defaultFlags |= KPbkVoipCallDefault;
            }
        }
    if(iEngine.Constants()->LocallyVariatedFeatureEnabled(EPbkLVPOC))
        {
        if (aContact.DefaultPocField())
            {
            defaultFlags |= KPbkPocCallDefault;
            }
        }

    return defaultFlags;
    }

//  End of File  
