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
* Description:  Phonebook 2 call type selector.
*
*/


// INCLUDE FILES
#include "CPbk2CallTypeSelector.h"

// Phonebook 2
#include <MPbk2ContactUiControl.h>
#include <Pbk2Commands.hrh>
#include <Pbk2UIControls.rsg>
#include <TPbk2StoreContactAnalyzer.h>

// Virtual Phonebook
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <CVPbkDefaultAttribute.h>
#include <CVPbkContactManager.h>
#include <RVPbkContactFieldDefaultPriorities.h>

// System includes
#include <coemain.h>
#include <centralrepository.h>
#include <AiwServiceHandler.h>
#include <AiwPoCParameters.hrh>
#include <featmgr.h>

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::CPbk2CallTypeSelector
// --------------------------------------------------------------------------
//
CPbk2CallTypeSelector::CPbk2CallTypeSelector
        ( CVPbkContactManager& aContactManager ) :
        iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::~CPbk2CallTypeSelector
// --------------------------------------------------------------------------
//
CPbk2CallTypeSelector::~CPbk2CallTypeSelector()
    {
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::NewL
// --------------------------------------------------------------------------
//
CPbk2CallTypeSelector* CPbk2CallTypeSelector::NewL
        ( CVPbkContactManager& aContactManager )
    {
    CPbk2CallTypeSelector* self =
        new ( ELeave ) CPbk2CallTypeSelector( aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2CallTypeSelector::ConstructL()
    {
    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SelectCallTypeL
// --------------------------------------------------------------------------
//
CAiwDialData::TCallType CPbk2CallTypeSelector::SelectCallTypeL
        ( const TInt aCommandId, MVPbkStoreContact& aContact,
          const MVPbkBaseContactField* aFocusedField,
          const MVPbkBaseContactField& aSelectedField ) const
    {
    // Default to voice call
    CAiwDialData::TCallType ret = CAiwDialData::EAIWVoice;

    // If the command was launched via Call UI menu we do not
    // need to apply our own logic for call type selection.
    // But if the command was not launched via menu but via the send key
    // (which is the case when aCommandId is EPbk2CmdCall), we
    // need to deduct what kind of a call to launch.
    if ( aCommandId == EPbk2CmdCall )
        {
        if ( !aFocusedField )
            {
            // If there was no focused field, the call was
            // launched from names list
            ret = SelectCallTypeL( aContact, aSelectedField );
            }
        else
            {
            // There was a focused field, so the
            // call was launched from info view
            ret = SelectCallTypeL( aSelectedField );
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::OkToLaunchTelephonyCallL
// --------------------------------------------------------------------------
//
TBool CPbk2CallTypeSelector::OkToLaunchTelephonyCallL
        ( const MVPbkBaseContactField* aSelectedField ) const
    {
    // Default to yes
    TBool ret = ETrue;

    if ( aSelectedField )
        {
        TPbk2StoreContactAnalyzer analyzer( iContactManager, NULL );

        if ( analyzer.IsFieldTypeIncludedL( *aSelectedField,
                R_PHONEBOOK2_PTT_SELECTOR ) )
            {
            // In case of PTT field, a POC call is to be launched
            ret = EFalse;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SelectPocCallType
// --------------------------------------------------------------------------
//
TInt CPbk2CallTypeSelector::SelectPocCallType
        ( const MPbk2ContactUiControl& aControl ) const
    {
    TInt ret = 0;

    // Check are we in a contact info field (which has focused field)
    if ( !aControl.FocusedField() )
        {
        ret |= EAiwPoCCmdTalkMany;
        }

    if ( !aControl.ContactsMarked() )
        {
        ret |= EAiwPoCCmdTalk1to1 | EAiwPoCCmdSendCallBackRequest;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SelectPocCallTypeForPocKeyPress
// --------------------------------------------------------------------------
//
TInt CPbk2CallTypeSelector::SelectPocCallTypeForPocKeyPress
        ( const MPbk2ContactUiControl& aControl ) const
    {
    TInt all = SelectPocCallType( aControl );
    TInt ret = all;

    // There is no specified logic for selecting a type.
    // So just define some priority for the call types.
    if ( all & EAiwPoCCmdTalkGroup )
        {
        ret = EAiwPoCCmdTalkGroup;
        }
    else if (all & EAiwPoCCmdTalk1to1)
        {
        ret = EAiwPoCCmdTalk1to1;
        }
    else if (all & EAiwPoCCmdTalkMany)
        {
        ret = EAiwPoCCmdTalkMany;
        }
    else if (all & EAiwPoCCmdSendCallBackRequest)
        {
        ret = EAiwPoCCmdSendCallBackRequest;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SetDefaultPrioritiesLC
// --------------------------------------------------------------------------
//
void CPbk2CallTypeSelector::SetDefaultPrioritiesLC
        ( RVPbkContactFieldDefaultPriorities& aDefaults,
        MVPbkStoreContact& aContact ) const
    {
    CleanupClosePushL( aDefaults );

    if ( HasContactAttributeL( EVPbkDefaultTypePhoneNumber, aContact ) )
        {
        // Voice overrides everything else
        User::LeaveIfError( aDefaults.Append( EVPbkDefaultTypePhoneNumber ) );
        }
    else if ( HasContactAttributeL( EVPbkDefaultTypeVoIP, aContact ) )
        {
        // VoIP overrides everything but voice
        User::LeaveIfError( aDefaults.Append( EVPbkDefaultTypeVoIP ) );
        }
    else if ( HasContactAttributeL( EVPbkDefaultTypeVideoNumber, aContact ) )
        {
        // Video overrides POC
        User::LeaveIfError( aDefaults.Append( EVPbkDefaultTypeVideoNumber ) );
        }
    else if ( HasContactAttributeL( EVPbkDefaultTypePOC, aContact ) )
        {
        // Only POC default exists, lets use it
        User::LeaveIfError( aDefaults.Append( EVPbkDefaultTypePOC ) );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SelectCallTypeL
// Selects call type based on selected field and
// contacts default information.
// --------------------------------------------------------------------------
//
inline CAiwDialData::TCallType CPbk2CallTypeSelector::SelectCallTypeL
        ( MVPbkStoreContact& aContact,
          const MVPbkBaseContactField& aSelectedField ) const
    {
    // Default to voice call
    CAiwDialData::TCallType ret = CAiwDialData::EAIWVoice;

    if ( HasContactAttributeL( EVPbkDefaultTypePhoneNumber, aContact ) )
        {
        ret = CAiwDialData::EAIWVoice;
        }
    else if ( ( HasContactAttributeL( EVPbkDefaultTypeVoIP, aContact ) ) &&
                ( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) ) )
        {
        ret = CAiwDialData::EAIWVoiP;
        }
    else if ( HasContactAttributeL( EVPbkDefaultTypeVideoNumber, aContact ) )
        {
        ret = CAiwDialData::EAIWForcedVideo;
        }
    else
        {
        // No defaults, so select based on selected field type only
        ret = SelectCallTypeBasedOnFieldTypeL( aSelectedField );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SelectCallTypeL
// Select call type based on field type, the preferred telephony
// information setting is taken in consideration also.
// --------------------------------------------------------------------------
//
inline CAiwDialData::TCallType CPbk2CallTypeSelector::SelectCallTypeL
        ( const MVPbkBaseContactField& aSelectedField ) const
    {
    // When there is clearly a selected field, the call type
    // of course depends on the type of the field
    return SelectCallTypeBasedOnFieldTypeL( aSelectedField );
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SelectCallTypeBasedOnFieldTypeL
// This function selects the call type by the type of passed
// aSelectedField alone.
// --------------------------------------------------------------------------
//
inline CAiwDialData::TCallType
    CPbk2CallTypeSelector::SelectCallTypeBasedOnFieldTypeL
        ( const MVPbkBaseContactField& aSelectedField ) const
    {
    // Default to voice call
    CAiwDialData::TCallType ret = CAiwDialData::EAIWVoice;

    TPbk2StoreContactAnalyzer analyzer( iContactManager, NULL );

    if ( analyzer.IsFieldTypeIncludedL( aSelectedField,
            R_PHONEBOOK2_SIP_SELECTOR ) )
        {
        ret = CAiwDialData::EAIWVoiP;
        }
    else if ( analyzer.IsFieldTypeIncludedL( aSelectedField,
            R_PHONEBOOK2_VIDEO_SELECTOR ) )
        {
        ret = CAiwDialData::EAIWForcedVideo;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::HasContactAttributeL
// Checks if aContact has default attribute of given type.
// --------------------------------------------------------------------------
//
TBool CPbk2CallTypeSelector::HasContactAttributeL
        ( TVPbkDefaultType aType, MVPbkStoreContact& aContact ) const
    {
    TBool retval = EFalse;

    CVPbkDefaultAttribute* attr = CVPbkDefaultAttribute::NewL( aType );
    CleanupStack::PushL( attr );
    MVPbkContactAttributeManager& attrManager =
        iContactManager.ContactAttributeManagerL();
    retval = attrManager.HasContactAttributeL( *attr, aContact );
    CleanupStack::PopAndDestroy( attr );

    return retval;
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SetDefaultPrioritiesLC
// --------------------------------------------------------------------------
//
void CPbk2CallTypeSelector::SetDefaultPrioritiesLC
        ( RVPbkContactFieldDefaultPriorities& aDefaults,
        MVPbkStoreContact& aContact,
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelector ) const
    {
    CleanupClosePushL( aDefaults );

    switch( aSelector )
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector :
            if ( HasContactAttributeL( EVPbkDefaultTypePhoneNumber, aContact ) )
                {
                User::LeaveIfError( aDefaults.Append( EVPbkDefaultTypePhoneNumber ) );
                }
            break;
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector :
            if ( HasContactAttributeL( EVPbkDefaultTypeVoIP, aContact ) )
                {
                User::LeaveIfError( aDefaults.Append( EVPbkDefaultTypeVoIP ) );
                }
            break;
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector :
            if ( HasContactAttributeL( EVPbkDefaultTypeVideoNumber, aContact ) )
                {
                User::LeaveIfError( aDefaults.Append( EVPbkDefaultTypeVideoNumber ) );
                }
            break;
        case VPbkFieldTypeSelectorFactory::EPocSelector :
            if ( HasContactAttributeL( EVPbkDefaultTypePOC, aContact ) )
                {
                User::LeaveIfError( aDefaults.Append( EVPbkDefaultTypePOC ) );
                }
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CallTypeSelector::SelectVoipCallTypeL
// --------------------------------------------------------------------------
//
CAiwDialData::TCallType CPbk2CallTypeSelector::SelectVoipCallTypeL(
        const MVPbkBaseContactField* aSelectedField ) const
    {
    // Default to CAiwDialData::EAIWVoiP
    CAiwDialData::TCallType ret = CAiwDialData::EAIWVoiP;

    TPbk2StoreContactAnalyzer analyzer( iContactManager, NULL );
    
    // Call type should be set CAiwDialData::EAIWVoice 
    // if mobile field (or other CS field) is selected 
    if ( analyzer.IsFieldTypeIncludedL( *aSelectedField,
    		R_PHONEBOOK2_VOIP_CALL_OUT_SELECTOR ) )
        {
        ret = CAiwDialData::EAIWVoice ;
        }
    
    return ret;
    }
        
// End of File
