/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook vCard data class.
*
*/


#include "CVPbkVCardData.h"
#include "CVPbkVCardIdNameMapping.h"
#include "CVPbkVCardFieldTypeProperty.h"
#include "CVPbkVCardFieldTypeList.h"
#include "CVPbkVCardOperationFactory.h"

#include <CVPbkContactManager.h>
#include <VPbkDataCaging.hrh>
#include <VPbkVCardEng.rsg>
#include <RLocalizedResourceFile.h>
#include <CVPbkContactCopyPolicyManager.h>
#include <VPbkStoreUriLiterals.h>
#include <MVPbkContactStoreList.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactDuplicatePolicy.h>
#include <VPbkUtil.h>

#include <barsread.h>
#include <vcard.h>

namespace 
    {
    /// VCard engine default resource file
    _LIT(KVCardEngDefResFileName, "VPbkVCardEng.rsc");
    }

CVPbkVCardData::CVPbkVCardData(CVPbkContactManager& aContactManager) :
        iContactManager(aContactManager),
        iMaskedFieldsArray(NULL)
    {
    }

inline void CVPbkVCardData::ConstructL()
    {
    // Open a file server session
    RFs fs;
	User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    // Create text store for extension names
    iTextStore = new(ELeave) VPbkEngUtils::CTextStore;
    
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC( fs, 
                    KVPbkRomFileDrive, 
                    KDC_RESOURCE_FILES_DIR, 
                    KVCardEngDefResFileName );
    HBufC8* fieldTypesBuf = resFile.AllocReadLC( R_VPBK_TYPE_MAPPINGS );
    TResourceReader resReader;
    resReader.SetBuffer( fieldTypesBuf );

    TInt propertyCount( resReader.ReadInt16() );
    while (propertyCount-- > 0)
        {
        CVPbkVCardFieldTypeProperty* prop = 
            CVPbkVCardFieldTypeProperty::NewLC( resReader, *iTextStore );
        User::LeaveIfError( iProperties.Append( prop ) );
        CleanupStack::Pop( prop );
        }

    resReader.SetBuffer( 
        resFile.AllocReadLC( R_VPBK_VCARD_PARAM_ID_NAME_MAPPINGS ) );
    TInt mappingsCount( resReader.ReadInt16() );
    while ( mappingsCount-- > 0 )
        {
        CVPbkVCardIdNameMapping* mapping = 
            CVPbkVCardIdNameMapping::NewLC( resReader );
        User::LeaveIfError( iParamIdNameMappings.Append( mapping ) );
        CleanupStack::Pop( mapping );
        }

    resReader.SetBuffer( 
        resFile.AllocReadLC( R_VPBK_VCARD_VNAME_ID_NAME_MAPPINGS ) );
    TInt vNameMappingsCount( resReader.ReadInt16() );
    while ( vNameMappingsCount-- > 0 )
        {
        CVPbkVCardIdNameMapping* mapping = 
            CVPbkVCardIdNameMapping::NewLC( resReader );
        User::LeaveIfError( iVNameIdNameMappings.Append( mapping ) );
        CleanupStack::Pop( mapping );
        }

    HBufC8* dataMaskedFields         = resFile.AllocReadLC(R_VPBK_VCARD_MASKED_FIELDS);
    resReader.SetBuffer(dataMaskedFields);
    iMaskedFieldsArray = resReader.ReadDesC8ArrayL();
    CleanupStack::PopAndDestroy(dataMaskedFields);
    
    CleanupStack::PopAndDestroy(4, &resFile);   
    // R_VPBK_VCARD_VNAME_ID_NAME_MAPPINGS
    // R_VPBK_VCARD_PARAMETER_ID_NAME_MAPPINGS, fieldTypesBuf, resFile

    CleanupStack::PopAndDestroy(); // fs

    iSupportedFieldTypes = 
        CVPbkVCardFieldTypeList::NewLC( 
            iProperties.Array(), iContactManager.FieldTypes() );
    CleanupStack::Pop(); // iSupportedFieldTypes
    
    CVPbkContactDuplicatePolicy::TParam param( iContactManager );
    
    iCopyPolicyManager = CVPbkContactCopyPolicyManager::NewL();
    
    // Don leave if implementation is not found -> duplicate checking
    // is not used if there is no policy
    TRAPD( res, iDuplicatePolicy = CVPbkContactDuplicatePolicy::NewL( param ));
    if ( res != KErrNone && res != KErrNotFound )
        {
        User::LeaveIfError( res );
        }
    }

CVPbkVCardData* CVPbkVCardData::NewL(CVPbkContactManager& aContactManager)
    {
    CVPbkVCardData* self = new(ELeave) CVPbkVCardData(aContactManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
CVPbkVCardData::~CVPbkVCardData()
    {
    delete iCopyPolicyManager;
    iParamIdNameMappings.ResetAndDestroy();
    iVNameIdNameMappings.ResetAndDestroy();
    iProperties.ResetAndDestroy();
    delete iSupportedFieldTypes;
    delete iDuplicatePolicy;
    delete iTextStore;
    delete iMaskedFieldsArray;    
    }
    
const MVPbkFieldTypeList& CVPbkVCardData::SupportedFieldTypes() const
    {
    return *iSupportedFieldTypes;
    }
    
TArray<CVPbkVCardFieldTypeProperty*> CVPbkVCardData::Properties() const
    {
    return iProperties.Array();
    }
    
TArray<CVPbkVCardIdNameMapping*> CVPbkVCardData::VNameIdNameMappings() const
    {
    return iVNameIdNameMappings.Array();
    }
    
TArray<CVPbkVCardIdNameMapping*> CVPbkVCardData::ParamIdNameMappings() const
    {
    return iParamIdNameMappings.Array();
    }
    
MVPbkContactAttributeManager& CVPbkVCardData::AttributeManagerL() const
	{
	return iContactManager.ContactAttributeManagerL();
	}    

MVPbkContactDuplicatePolicy* CVPbkVCardData::DuplicatePolicy()
    {
    return iDuplicatePolicy;
    }

MVPbkContactCopyPolicy& CVPbkVCardData::CopyPolicyL(
        const TVPbkContactStoreUriPtr& aUri) const
    {
    MVPbkContactCopyPolicy* policy = 
        iCopyPolicyManager->GetPolicyL(iContactManager,aUri);
    if (!policy)
        {
        User::Leave(KErrNotFound);
        }
    return *policy;
    }
    
MVPbkContactStore& CVPbkVCardData::GetCntModelStoreL()
    {
    TVPbkContactStoreUriPtr uriPtr(KVPbkDefaultCntDbURI);
    MVPbkContactStore* store = iContactManager.ContactStoresL().Find(uriPtr);
    if (!store)
        {
        iContactManager.LoadContactStoreL(uriPtr);
        store = iContactManager.ContactStoresL().Find(uriPtr);
        }
    return *store;
    }

const CDesC8ArrayFlat* CVPbkVCardData::GetMaskedFields()
{
    return (const CDesC8ArrayFlat*) iMaskedFieldsArray;
}
CVPbkContactManager& CVPbkVCardData::GetContactManager()
{
return iContactManager;
}    
