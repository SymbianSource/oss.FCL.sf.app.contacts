/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Selector factory for a set of predefined selectors.
*
*/


// Virtual Phonebook
#include "VPbkFieldTypeSelectorFactory.h"
#include "CVPbkFieldTypeList.h"

#include <VPbkDataCaging.hrh>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkFieldType.h>
#include <CVPbkLocalVariationManager.h>
#include <VPbkFieldTypeSelectors.rsg>
#include <RLocalizedResourceFile.h>
#include <featmgr.h>
#include <VPbkEng.rsg>
#include <spsettings.h>
#include <spproperty.h>
#include <spentry.h>

// System includes
#include <f32file.h>
#include <barsread.h>

namespace {

struct TMap
    {
    TInt iType;
    TInt iResId;
    };

const TMap KFieldMapTable[] = {
    { VPbkFieldTypeSelectorFactory::EPhoneNumberSelector, R_VPBK_PHONE_NUMBER_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EMobileNumberSelector, R_VPBK_MOBILE_NUMBER_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EFaxNumberSelector, R_VPBK_FAX_NUMBER_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EVideoNumberSelector, R_VPBK_VIDEO_NUMBER_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EPagerNumberSelector, R_VPBK_PAGER_NUMBER_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EEmailAddressSelector, R_VPBK_EMAIL_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EEmailOverSmsSelector, R_VPBK_EMAIL_OVER_SMS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EMmsAddressSelector, R_VPBK_MMS_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EVoipAddressSelector, R_VPBK_VOIP_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EPttAddressSelector, R_VPBK_PTT_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ESipAddressSelector, R_VPBK_SIP_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ESipMsisdnAddressSelector, R_VPBK_SIP_MSISDN_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EThumbnailImageSelector, R_VPBK_THUMBNAIL_IMAGE_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ECodImageSelector, R_VPBK_COD_IMAGE_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EImageSelector, R_VPBK_IMAGE_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ECodTextSelector, R_VPBK_COD_TEXT_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ERingingToneSelector, R_VPBK_RINGING_TONE_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EUrlAddressSelector, R_VPBK_URL_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ELastNameSelector, R_VPBK_LAST_NAME_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EFirstNameSelector, R_VPBK_FIRST_NAME_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ELastNamePronunciationSelector, R_VPBK_LAST_NAME_PRONUNCIATION_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EFirstNamePronunciationSelector, R_VPBK_FIRST_NAME_PRONUNCIATION_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ESynchronizationSelector, R_VPBK_SYNCHRONIZATION_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EDtmfNumberSelector, R_VPBK_DTMF_NUMBER_SELECTOR },
    { VPbkFieldTypeSelectorFactory::ETopcontactSelector, R_VPBK_TOPCONTACT_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EImppSelector, R_VPBK_IMPP_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EFindOnMapSelector, R_VPBK_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EAssignFromMapSelector, R_VPBK_ADDRESS_SELECTOR },
    { VPbkFieldTypeSelectorFactory::EPagerfaxSelector, R_VPBK_PAGERFAX_SELECTOR }
    // add more selectors here
};
const TInt KFieldMapTableLength = sizeof( KFieldMapTable ) / sizeof( TMap );

/**
 * Maps selector iType and ressource id.
 *
 * @param aType Id of the predefined selector.
 * @return A matching ressource id or KErrNotFound.
 */
TInt MapFieldTypeSelectorToRessourceId(
        VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector aType )
    {
    for( TInt i = 0; i < KFieldMapTableLength; ++i )
        {
        if( aType == KFieldMapTable[i].iType )
            {
            return KFieldMapTable[i].iResId;
            }
        }

    return KErrNotFound;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void FillL(RArray<TInt>& aArray, const TInt* aTable, const TInt aSize)
    {
    for (TInt i = 0; i < aSize; ++i)
        {
        if (aArray.Find(aTable[i]) == KErrNotFound)
            {
            // allows to add only once
            aArray.AppendL(aTable[i]);
            }
        }
    }

/**
 * Voip tables extracted from code to clear implementation logic.
 */
namespace VoipTables {
    const TInt KVoipEnabledFieldSelectors[] = {
                    R_VPBK_VOIP_ADDRESS_SELECTOR,
                    R_VPBK_PTT_ADDRESS_SELECTOR,
                    R_VPBK_SIP_ADDRESS_SELECTOR
                    };
    const TInt KVoipEnabledFieldSelectorsSize =
        sizeof(KVoipEnabledFieldSelectors) / sizeof(TInt);

    const TInt KVoipCallOutFieldSelectors[] = {
                    R_VPBK_VOIP_CALL_OUT_SELECTOR
                    };
    const TInt KVoipCallOutFieldSelectorsSize =
        sizeof(KVoipCallOutFieldSelectors) / sizeof(TInt);

    const TInt KVoipXspProviderSupportsVoipSelectors[] = {
            R_VPBK_IMPP_SELECTOR
            };
    const TInt KVoipXspProviderSupportsVoipSelectorsSize =
        sizeof(KVoipXspProviderSupportsVoipSelectors) / sizeof(TInt);
}

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
const CSPProperty* GetProperty(const CSPEntry& aEntry, TServicePropertyName mask)
    {
    const CSPProperty* result = NULL;
    if (aEntry.GetProperty(result, mask) == KErrNone)
        {
        return result;
        }
    return result;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
TInt GetIntValue(const CSPProperty* aProperty)
    {
    TInt value = 0;
    if (aProperty && aProperty->GetValue(value) == KErrNone)
        {
        return value;
        }
    return 0;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void VoipSelectorsL(RArray<TInt>& aArray)
    {
    // Special Voip checker, which selectors can be included in which case
    RIdArray idArray;
    CleanupClosePushL(idArray);

    CSPSettings* settings = CSPSettings::NewLC();
    User::LeaveIfError( settings->FindServiceIdsL(idArray) );
    for (TInt i = 0; i < idArray.Count(); ++i)
        {
        CSPEntry* entry = CSPEntry::NewLC();
        TServiceId id = idArray[i];
        User::LeaveIfError( settings->FindEntryL(id, *entry) );
        // service is an xsp service
        const CSPProperty* tmpProp = GetProperty(*entry, EPropertyServiceAttributeMask);
        TInt value = GetIntValue(tmpProp);
        if (value & ESupportsInternetCall)
            {
            // supports voip
            FillL(aArray, VoipTables::KVoipEnabledFieldSelectors,
                    VoipTables::KVoipEnabledFieldSelectorsSize);
            // include xsp fields
            FillL(aArray, VoipTables::KVoipXspProviderSupportsVoipSelectors,
                    VoipTables::KVoipXspProviderSupportsVoipSelectorsSize);
            }
        CleanupStack::PopAndDestroy(); // entry
        }

    if (settings->IsFeatureSupported(ESupportCallOutFeature))
        {
        // supports call out, add call out field types
        FillL(aArray, VoipTables::KVoipCallOutFieldSelectors,
                VoipTables::KVoipCallOutFieldSelectorsSize);
        }

    CleanupStack::PopAndDestroy(2); // settings, idArray
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
RArray<TInt> FilteredSelectorsL(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aType)
    {
    RArray<TInt> result;
    CleanupClosePushL(result);
    switch (aType)
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
            {
            result.AppendL(R_VPBK_VOICE_CALL_ACTION_SELECTOR);
            break;
            }
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
            {
            CVPbkLocalVariationManager* localVariation =
                CVPbkLocalVariationManager::NewL();
            CleanupStack::PushL(localVariation);
            
            if ( localVariation->LocallyVariatedFeatureEnabled(
                        EVPbkLVShowEmailInSendMsg ) )
            {
                result.AppendL(R_VPBK_UNIEDITOR_EMAIL_ACTION_SELECTOR);
            }
            else
            {
                result.AppendL(R_VPBK_UNIEDITOR_ACTION_SELECTOR);
            }
            CleanupStack::PopAndDestroy(localVariation);
            break;
            }
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
            {
            result.AppendL(R_VPBK_EMAIL_ADDRESS_SELECTOR);
            break;
            }
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
            {
            result.AppendL(R_VPBK_IMPP_SELECTOR);
            break;
            }
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            {
            VoipSelectorsL(result);
            break;
            }
        case VPbkFieldTypeSelectorFactory::EURLSelector:
            {
            result.AppendL(R_VPBK_URL_ADDRESS_SELECTOR);
            break;
            }
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
            {
            result.AppendL(R_VPBK_VIDEO_NUMBER_SELECTOR);
            result.AppendL(R_VPBK_MOBILE_NUMBER_SELECTOR);
            break;
            }
        case VPbkFieldTypeSelectorFactory::EFindOnMapSelector:
        case VPbkFieldTypeSelectorFactory::EAssignFromMapSelector:
            {
            result.AppendL(R_VPBK_ADDRESS_SELECTOR);
            break;
            }
        }

    CleanupStack::Pop(); // result
    return result;
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
TResourceReader CreateSelectorResourceReaderLC(TInt aResourceId, RFs& aFs)
    {
    //Read resource file
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC( aFs, KVPbkRomFileDrive,
                    KDC_RESOURCE_FILES_DIR,
                    KVPbkFieldTypeSelectorsResFileName );
    TResourceReader resReader;
    HBufC8* selectorBuf = resFile.AllocReadLC( aResourceId );
    resReader.SetBuffer( selectorBuf );
    CleanupStack::Pop(); // selectorBuf
    CleanupStack::PopAndDestroy(); // resFile
    CleanupStack::PushL(selectorBuf);
    return resReader;
    }

/**
 * Appends a filter for the given field iType selector.
 *
 * @param aRessourceId Selector ressource Id.
 * @param aFieldTypeList Field iType list used to search
 *                       for the field.
 * @return A new instance of CVPbkFieldTypeSelector
 */
CVPbkFieldTypeSelector* CreateSelectorL(
                TInt aRessourceId, RFs& aFs, const MVPbkFieldTypeList& aFieldTypeList )
    {
    TResourceReader resourceReader = CreateSelectorResourceReaderLC(aRessourceId, aFs);

    //create and return CVPbkFieldTypeSelector
    CVPbkFieldTypeSelector* returnPtr =  CVPbkFieldTypeSelector::NewL(
            resourceReader, aFieldTypeList );

    //Cleanup
    CleanupStack::PopAndDestroy(); // resourReader

    return returnPtr;
    }

} // namespace

// --------------------------------------------------------------------------
// VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeSelector*
                VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL(
                TVPbkFieldTypeSelector aType,
                const MVPbkFieldTypeList& aFieldTypeList)
    {
    //Connect fileserver
    RFs fSrv;
    User::LeaveIfError( fSrv.Connect() );
    CleanupClosePushL( fSrv );

    //Map iType to resource
    TInt resourceId = MapFieldTypeSelectorToRessourceId( aType );

    // leave if no appropriate resource id has not been found
    User::LeaveIfError(resourceId);
    CVPbkFieldTypeSelector* result = CreateSelectorL( resourceId, fSrv, aFieldTypeList );
    CleanupStack::PopAndDestroy(); // fSrv
    return result;
    }

// --------------------------------------------------------------------------
// VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeSelector*
                VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                TVPbkContactActionTypeSelector aType,
                const MVPbkFieldTypeList& aFieldTypeList )
    {
    //Connect fileserver
    RFs fSrv;
    User::LeaveIfError( fSrv.Connect() );
    CleanupClosePushL( fSrv );

    RArray<TInt> fieldTypeSelectors = FilteredSelectorsL(aType);
    CleanupClosePushL(fieldTypeSelectors);

    CVPbkFieldTypeSelector* result = CVPbkFieldTypeSelector::NewL(aFieldTypeList);
    CleanupStack::PushL(result);
    result->SetContactActionType(aType);
    for (TInt i = 0; i < fieldTypeSelectors.Count(); ++i)
        {
        TInt resourceId = fieldTypeSelectors[i];
        if (resourceId != KErrNotFound)
            {
            TResourceReader reader = CreateSelectorResourceReaderLC(resourceId, fSrv);
            result->AppendL(reader);
            CleanupStack::PopAndDestroy(); // reader
            }
        }

    CleanupStack::Pop(); // result
    CleanupStack::PopAndDestroy(2); // fieldTypeSelectors, fSrv
    return result;
    }

// --------------------------------------------------------------------------
// VPbkFieldTypeSelectorFactory::CreateActionTypeSelectorIdL
// --------------------------------------------------------------------------
//
EXPORT_C VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
       VPbkFieldTypeSelectorFactory::CreateActionTypeSelectorIdL( const TInt aID )
    {
    if(
        aID == VPbkFieldTypeSelectorFactory::EEmptySelector ||
        aID == VPbkFieldTypeSelectorFactory::EVoiceCallSelector ||
        aID == VPbkFieldTypeSelectorFactory::EUniEditorSelector ||
        aID == VPbkFieldTypeSelectorFactory::EEmailEditorSelector ||
        aID == VPbkFieldTypeSelectorFactory::EInstantMessagingSelector ||
        aID == VPbkFieldTypeSelectorFactory::EVOIPCallSelector ||
        aID == VPbkFieldTypeSelectorFactory::EURLSelector ||
        aID == VPbkFieldTypeSelectorFactory::EVideoCallSelector ||
        aID == VPbkFieldTypeSelectorFactory::EFindOnMapSelector ||
        aID == VPbkFieldTypeSelectorFactory::EAssignFromMapSelector ||
        aID == VPbkFieldTypeSelectorFactory::EPocSelector
       )
        {
        return (VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector)aID;
        }
    else
        {
        User::Leave( KErrNotFound );

        // sloc - Stupid Line Of Code : but compiler expects
        return VPbkFieldTypeSelectorFactory::EPocSelector;
        }
    }

// End of file
