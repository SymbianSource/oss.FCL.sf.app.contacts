/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __SELECTOR_CONSTANTS_H
#define __SELECTOR_CONSTANTS_H

#include <vpbkeng.rsg>
#include "stifunitmacros.h"
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkFieldType.h>


const TInt KUniEditorFields[] = {
    R_VPBK_FIELD_TYPE_MOBILEPHONEGEN, 
    R_VPBK_FIELD_TYPE_MOBILEPHONEHOME,
    R_VPBK_FIELD_TYPE_MOBILEPHONEWORK,
    R_VPBK_FIELD_TYPE_ASSTPHONE,
    R_VPBK_FIELD_TYPE_CARPHONE,
    R_VPBK_FIELD_TYPE_EMAILGEN,
    R_VPBK_FIELD_TYPE_EMAILHOME,
    R_VPBK_FIELD_TYPE_EMAILWORK
    };
const TInt KUniEditorFieldsSize = sizeof(KUniEditorFields) / sizeof(TInt);


const TInt KEmailEditorSelectors[] = {
    R_VPBK_FIELD_TYPE_EMAILGEN, 
    R_VPBK_FIELD_TYPE_EMAILHOME,
    R_VPBK_FIELD_TYPE_EMAILWORK
    };
const TInt KEmailEditorSelectorsSize = sizeof(KEmailEditorSelectors) / sizeof(TInt);


const TInt KInstantMessagingSelectors[] = {
    R_VPBK_FIELD_TYPE_IMPP 
    };
const TInt KInstantMessagingSelectorsSize = sizeof(KInstantMessagingSelectors) / sizeof(TInt);


const TInt KEVoiceCallSelectorFields[] = {
    R_VPBK_FIELD_TYPE_MOBILEPHONEGEN, 
    R_VPBK_FIELD_TYPE_MOBILEPHONEHOME,
    R_VPBK_FIELD_TYPE_MOBILEPHONEWORK,
    R_VPBK_FIELD_TYPE_LANDPHONEGEN,
    R_VPBK_FIELD_TYPE_LANDPHONEHOME,
    R_VPBK_FIELD_TYPE_LANDPHONEWORK,
    R_VPBK_FIELD_TYPE_PAGERNUMBER,
    R_VPBK_FIELD_TYPE_ASSTPHONE,
    R_VPBK_FIELD_TYPE_CARPHONE
    };
const TInt KEVoiceCallSelectorFieldsSize = sizeof(KEVoiceCallSelectorFields) / sizeof(TInt);

// voip
const TInt KEVOIPCallSelectorFields[] = {
    R_VPBK_FIELD_TYPE_VOIPGEN,
    R_VPBK_FIELD_TYPE_VOIPHOME, 
    R_VPBK_FIELD_TYPE_VOIPWORK,
    R_VPBK_FIELD_TYPE_POC,
    R_VPBK_FIELD_TYPE_SWIS,
    R_VPBK_FIELD_TYPE_SIP,
    R_VPBK_FIELD_TYPE_IMPP
    };
const TInt KEVOIPCallSelectorFieldsSize = sizeof(KEVOIPCallSelectorFields) / sizeof(TInt);

const TInt KEVOIPCallOutSelectorFields[] = {
        R_VPBK_FIELD_TYPE_MOBILEPHONEGEN, 
        R_VPBK_FIELD_TYPE_MOBILEPHONEHOME,
        R_VPBK_FIELD_TYPE_MOBILEPHONEWORK,
        R_VPBK_FIELD_TYPE_LANDPHONEGEN,
        R_VPBK_FIELD_TYPE_LANDPHONEHOME,
        R_VPBK_FIELD_TYPE_LANDPHONEWORK,
        R_VPBK_FIELD_TYPE_ASSTPHONE,
        R_VPBK_FIELD_TYPE_CARPHONE
    };
const TInt KEVOIPCallOutSelectorFieldsSize = sizeof(KEVOIPCallOutSelectorFields) / sizeof(TInt);

const TInt KEVOIPXspSelectorFields[] = {
        R_VPBK_FIELD_TYPE_IMPP
    };
const TInt KEVOIPXspSelectorFieldsSize = sizeof(KEVOIPXspSelectorFields) / sizeof(TInt);


const TInt KVideoCallSelectors[] = {
        R_VPBK_FIELD_TYPE_MOBILEPHONEGEN, 
        R_VPBK_FIELD_TYPE_MOBILEPHONEHOME,
        R_VPBK_FIELD_TYPE_MOBILEPHONEWORK,
        R_VPBK_FIELD_TYPE_VIDEONUMBERGEN,
        R_VPBK_FIELD_TYPE_VIDEONUMBERHOME,
        R_VPBK_FIELD_TYPE_VIDEONUMBERWORK
    };
const TInt KVideoCallSelectorsSize = sizeof(KVideoCallSelectors) / sizeof(TInt);

const TInt KURLSelectors[] = {
        R_VPBK_FIELD_TYPE_URLGEN, 
        R_VPBK_FIELD_TYPE_URLHOME,
        R_VPBK_FIELD_TYPE_URLWORK
    };
const TInt KURLSelectorsSize = sizeof(KURLSelectors) / sizeof(TInt);

class CAssertUtils : public CBase
    {
public:
    CAssertUtils(CStifLogger& aLog) : iLog(&aLog) 
        {}
    void AssertTypesL(
            CVPbkFieldTypeSelector& aSelector, 
            const TInt aTypes[],
            const TInt aSize,
            const MVPbkFieldTypeList& aFieldTypes,
            TTestResult& aResult)
        {
        iResult = &aResult;
        TInt count = aFieldTypes.FieldTypeCount();
        while (--count >= 0)
            {
            const MVPbkFieldType& type = aFieldTypes.FieldTypeAt(count);
            TBool included = aSelector.IsFieldTypeIncluded(type);
            TInt index = Find(type.FieldTypeResId(), aTypes, aSize);
            if (index >= 0)
                {
                // assert that field is included
                STIF_LOG("resid: %d, included %d", type.FieldTypeResId(), included);
                STIF_ASSERT(included);
                }
            else
                {
                // assert that field is not included
                STIF_LOG("resid: %d, included %d", type.FieldTypeResId(), included);
                STIF_ASSERT(!included);
                }
            }
        }
    
    TInt Find(const TInt type, const TInt aTypes[], const TInt aSize)
        {
        for (TInt i = 0; i < aSize; ++i)
            {
            if (type == aTypes[i])
                {
                return i;
                }
            }
        return KErrNotFound;
        }
    
private:
    CStifLogger* iLog;
    TTestResult* iResult;
    };


#endif // __SELECTOR_CONSTANTS_H

// End of file
