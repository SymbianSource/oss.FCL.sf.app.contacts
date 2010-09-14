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
* Description:  Implementation for CFmPbkEngineExtension.
*
*/



// INCLUDES
#include "Cfmpbkengineextension.h"
#include <pbkengine_fmres.rsg> // Common for all langugages
#include <barsc.h>
#include <bautils.h>
#include <featmgr.h>

#include <bldvariant.hrh>
#include "PbkDataCaging.hrh"

#include <CPbkConstants.h>
#include <PbkEngUtils.h>
#include <PhonebookVariant.hrh>
#include "PbkFieldInfoWrite.h"
#include "CPbkFieldsInfo.h"
#include "CPbkFieldInfoGroup.h"
#include <PbkConfig.hrh>

/// Unnamed namespace for local definitions
namespace {

_LIT(KFmResFileName, "PbkEngine_FMRes.RSC"); // default resource file name
_LIT(KFmResChineseFileName, "PbkEngine_FMChineseRes.RSC"); // Chinese variant
_LIT(KFmResJapaneseFileName, "PbkEngine_FMJapaneseRes.RSC"); // Japanese

enum TFieldType
    {
    ENickNameFieldType = 0,
    EVideoTelephonyFieldType,
    EJapaneseReadingFieldType,
    EVOIPFieldType,
    EPOCFieldType,
    EShareviewFieldType,
    ESIPFieldType,
    EOldVOIPFieldType,
    EOldPOCFieldType,
    EOldShareviewFieldType,
    ECODFieldType,
    EPrefixFieldType,
    ESuffixFieldType,
    EMegaOperatorFieldType,
    ELastFieldType // Must be last, used to indicate number of field types
    };

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_ConstructL = 1,
    EPanicPreCond_NewL
    };

void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CFmPbkEngineExtension");
    User::Panic(KPanicText, aPanicCode);
    };

#endif // _DEBUG

/**
 * Solves correct language specific FM engine's resource filename. 
 *
 * @return A reference to resource file
 */
inline const TDesC& FmResFileName()
    {
    if (FeatureManager::FeatureSupported(KFeatureIdChinese))
        {
        return KFmResChineseFileName;
        }
    else if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        return KFmResJapaneseFileName;
        }
    return KFmResFileName;
    }


} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CFmPbkEngineExtension::CFmPbkEngineExtension()
    {
    }

inline void CFmPbkEngineExtension::ConstructL(
        const CPbkEngineExtensionInterface::TEngExtParams& aParam)
    {
    __ASSERT_DEBUG(aParam.iFs, Panic(EPanicPreCond_ConstructL));

    iFs = *aParam.iFs;
    RResourceFile pbkResFile;
    
    PbkEngUtils::FindAndOpenDefaultResourceFileLC(iFs,pbkResFile);
    pbkResFile.ConfirmSignatureL();

    iConstants = CPbkConstants::NewL(pbkResFile);

    CleanupStack::PopAndDestroy();  // pbkResFile

    PbkEngUtils::FindAndOpenResourceFileLC
        (iFs, FmResFileName(), KDC_RESOURCE_FILES_DIR, KPbkRomFileDrive, iResourceFile);
    iResourceFile.ConfirmSignatureL(0);
    CleanupStack::Pop();
    }

CFmPbkEngineExtension* CFmPbkEngineExtension::NewL(TAny* aParam)
    {
    __ASSERT_DEBUG(aParam, Panic(EPanicPreCond_NewL));
    CFmPbkEngineExtension* self = new (ELeave) CFmPbkEngineExtension;
    CleanupStack::PushL(self);
    self->ConstructL(*static_cast<CPbkEngineExtensionInterface::TEngExtParams*>(aParam));
    CleanupStack::Pop(self);
    return self;
    }

CFmPbkEngineExtension::~CFmPbkEngineExtension()
    {
    delete iConstants;
    iResourceFile.Close();
    }

TBool CFmPbkEngineExtension::GetAdditionalFieldTypesResourceL
        (TFieldTypeResource& aResource)
    {
    TBool result = EFalse;

    switch (iCurrentResourceIndex)
        {
        case ENickNameFieldType:
            {
            if (iConstants->
                    LocallyVariatedFeatureEnabled(EPbkLVAddNickNameField))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_NICK_NAME_FIELD_PROPERTIES,
                              R_CNTUI_ADD_NICK_NAME_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case EVideoTelephonyFieldType:
            {
            if (iConstants->
                    LocallyVariatedFeatureEnabled(EPbkLVAddVideoTelephonyFields))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_VIDEO_TELEPHONY_FIELD_PROPERTIES,
                              R_CNTUI_ADD_VIDEO_TELEPHONY_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case EJapaneseReadingFieldType:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_JAPANESE_READING_FIELD_PROPERTIES,
                              R_CNTUI_ADD_JAPANESE_READING_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case EVOIPFieldType:
            {
            if (iConstants->LocallyVariatedFeatureEnabled(EPbkLVVOIP))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_VOIP_FIELD_PROPERTIES,
                              R_CNTUI_ADD_VOIP_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case EPOCFieldType:
            {
            if (iConstants->LocallyVariatedFeatureEnabled(EPbkLVPOC))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_POC_FIELD_PROPERTIES,
                              R_CNTUI_ADD_POC_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case EShareviewFieldType:
            {
            if (iConstants->LocallyVariatedFeatureEnabled(EPbkLVShareView))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_SHARE_VIEW_FIELD_PROPERTIES,
                              R_CNTUI_ADD_SHARE_VIEW_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case ESIPFieldType:
            {
            if (iConstants->LocallyVariatedFeatureEnabled(EPbkLVAddSIPFields))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_SIP_FIELD_PROPERTIES,
                              R_CNTUI_ADD_SIP_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case EOldVOIPFieldType:
            {
            if (iConstants->LocallyVariatedFeatureEnabled(EPbkLVVOIP))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_OLD_VOIP_FIELD_PROPERTIES,
                              R_CNTUI_ADD_OLD_VOIP_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case EOldPOCFieldType:
            {
            if (iConstants->LocallyVariatedFeatureEnabled(EPbkLVPOC))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_OLD_POC_FIELD_PROPERTIES,
                              R_CNTUI_ADD_OLD_POC_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case EOldShareviewFieldType:
            {
            if (iConstants->LocallyVariatedFeatureEnabled(EPbkLVShareView))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_OLD_SHARE_VIEW_FIELD_PROPERTIES,
                              R_CNTUI_ADD_OLD_SHARE_VIEW_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }
        case ECODFieldType:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdCallImagetext))
                {
                GetFieldTypeL(aResource,
                    R_PHONEBOOK_ADD_COD_FIELD_PROPERTIES,
                    R_CNTUI_ADD_COD_FIELD_DEFNS );
                result = ETrue;
                }
            break;
            }
        case EPrefixFieldType:
            {
            if (iConstants->
                    LocallyVariatedFeatureEnabled(EPbkLVAddMDOFields))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_PREFIX_FIELD_PROPERTIES,
                              R_CNTUI_ADD_PREFIX_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }

        case ESuffixFieldType:
            {
            if (iConstants->
                    LocallyVariatedFeatureEnabled(EPbkLVAddMDOFields))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOK_ADD_SUFFIX_FIELD_PROPERTIES,
                              R_CNTUI_ADD_SUFFIX_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }

        case EMegaOperatorFieldType:
            {
            if (iConstants->
                    LocallyVariatedFeatureEnabled(EVPbkLVMegaOperatorFields))
                {
                GetFieldTypeL(aResource,
                              R_PHONEBOOOK_ADD_MEGAOP_FIELD_PROPERTIES,
                              R_CNTUI_ADD_MEGAOP_FIELD_DEFNS);
                result = ETrue;
                }
            break;
            }

        default:
            {
            // no other possible field types
            break;
            }
        }

    ++iCurrentResourceIndex;
    return result;
    }

void CFmPbkEngineExtension::GetFieldTypeL
        (TFieldTypeResource& aResource,
        TInt aPbkResId,
        TInt aCntResId)
    {
    HBufC8* pbkResBuffer = iResourceFile.AllocReadLC(aPbkResId);
    HBufC8* cntResBuffer = iResourceFile.AllocReadLC(aCntResId);
    aResource.iPbkRes = pbkResBuffer;
    aResource.iCntModelRes = cntResBuffer;
    CleanupStack::Pop(cntResBuffer);
    CleanupStack::Pop(pbkResBuffer);
    }

TBool CFmPbkEngineExtension::ModifyFieldTypesL(CPbkFieldsInfo& /*aFieldsInfo*/)
    {
    return EFalse;
    }

TInt CFmPbkEngineExtension::AdditionalFieldTypesResourceCount() const
    {
    return ELastFieldType;
    }

// End of file
