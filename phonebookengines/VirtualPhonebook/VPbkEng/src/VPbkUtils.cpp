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
* Description:  Utils for virtual phonebook
*
*/


#include "VPbkUtils.h"

#include <barsread.h>

#include <VPbkError.h>
#include <MVPbkFieldType.h>
#include <MVPbkBaseContact.h>
#include <MVPbkBaseContactField.h>
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkStoreContact.h>
#include <vpbkeng.rsg>
#include <RLocalizedResourceFile.h>
#include <VPbkDataCaging.hrh>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <VPbkSyncConstants.h>


namespace VPbkUtils {

EXPORT_C const MVPbkFieldType* MatchFieldType
    (const MVPbkFieldTypeList& aFieldTypeList,
    const MVPbkBaseContactField& aField)
    {
    const MVPbkFieldType* result = NULL;

    for (TInt matchPriority = 0; 
        matchPriority <= aFieldTypeList.MaxMatchPriority() && !result;
        ++matchPriority)
        {
        result = aField.MatchFieldType(matchPriority);
        }

    return result;
    }

EXPORT_C TBool IsFieldTypeIncludedL(
        const MVPbkFieldTypeList& aFieldTypeList,
        const MVPbkBaseContactField& aField,
        TResourceReader& aResourceReader)
    {
    TBool ret = EFalse;
    // Get aField's field type
    const MVPbkFieldType* fieldType = MatchFieldType(aFieldTypeList, 
            aField);
    
    if ( fieldType )
        {
        CVPbkFieldTypeSelector* selector = CVPbkFieldTypeSelector::NewL(
            aResourceReader, aFieldTypeList );
        // Check if the fields type is the one needed
        ret = selector->IsFieldTypeIncluded( *fieldType );
        delete selector;
        }
    return ret;
    }

EXPORT_C TBool IsFieldTypeIncludedInContactL(
        const MVPbkFieldTypeList& aFieldTypeList,
        const MVPbkBaseContact& aContact,
        TResourceReader& aResourceReader)
    {
    // create the field type selector
    CVPbkFieldTypeSelector* selector = CVPbkFieldTypeSelector::NewL(
            aResourceReader, aFieldTypeList);
    CleanupStack::PushL(selector);
    
    TBool ret = EFalse;
    // loop though all the fields in the contact
    const MVPbkBaseContactFieldCollection& fieldCol = aContact.Fields();
    for (TInt i = 0; i < fieldCol.FieldCount(); ++i)
        {
        const MVPbkBaseContactField& field = fieldCol.FieldAt(i);
        // get the field type
        const MVPbkFieldType* fieldType = MatchFieldType(aFieldTypeList, 
                field);
        ret = selector->IsFieldTypeIncluded(*fieldType);
        if (ret)
            {
            // break and return if a field matching the criteria is found
            break;
            }
        }
    CleanupStack::PopAndDestroy(selector);
    return ret;
    }

void DoValidateFieldContentL(
        MVPbkStoreContactField& aSyncField)
    {
    // get the field content
    MVPbkContactFieldData& data = aSyncField.FieldData();
    if (data.DataType() == EVPbkFieldStorageTypeText)
        {
        // check that a valid text is in the field
        MVPbkContactFieldTextData& textData = MVPbkContactFieldTextData::Cast(data);
        if (!(!textData.Text().CompareF(KVPbkContactSyncPrivate) || 
            !textData.Text().CompareF(KVPbkContactSyncPublic) ||
            !textData.Text().CompareF(KVPbkContactSyncNoSync)))
            {
            // set default sync setting to field
            textData.SetTextL(KVPbkContactSyncPrivate);
            }
        }
    }

EXPORT_C TBool VerifySyncronizationFieldL(
        RFs& aFs,
        const MVPbkFieldTypeList& aSupportedFieldTypeList,
        MVPbkStoreContact& aContact)
    {
    TBool ret = EFalse;
    // check whether the contact store supports the sync field
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC(aFs, KVPbkRomFileDrive, 
        KDC_RESOURCE_FILES_DIR, KVPbkDefResFileName);
    HBufC8* resourceBuf = resFile.AllocReadLC(R_VPBK_SYNCRONIZATION_FIELD_PROPERTY);

    TResourceReader resReader;
    resReader.SetBuffer(resourceBuf);
    TVPbkFieldVersitProperty syncProperty(resReader);
    // find field type from aSupportedFieldTypeList
    const MVPbkFieldType* syncFieldType = 
        aSupportedFieldTypeList.FindMatch(syncProperty, 0);
    CleanupStack::PopAndDestroy(2); // resFile, resourceBuf
    if (syncFieldType)
        {
        // syncronization field is supported by store
        // check whether the contact has the syncronization field
        CVPbkContactFieldTypeIterator* iter = CVPbkContactFieldTypeIterator::NewLC(
                *syncFieldType, aContact.Fields());
        if(iter->HasNext())
            {
            // syncronization field is in contact
            MVPbkStoreContactField* syncField = iter->Next();
            // check that the contact contains one of the required texts, 
            // else replace it with default
            DoValidateFieldContentL(*syncField);
            }
        else
            {
            // No sync field in contact
            // create field to contact and set content to default
            MVPbkStoreContactField* syncField = aContact.CreateFieldLC(*syncFieldType);
            DoValidateFieldContentL(*syncField);
            // add created field to contact
            aContact.AddFieldL(syncField);
            CleanupStack::Pop(syncField);
            }
        CleanupStack::PopAndDestroy(iter);
        }
    return ret;
    }

} // VPbkUtils

// end of file
