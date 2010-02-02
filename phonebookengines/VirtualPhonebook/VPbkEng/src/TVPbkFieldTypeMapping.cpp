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
* Description:  Field Type Mapping
*
*/


#include "TVPbkFieldTypeMapping.h"
#include <barsread.h>
#include "MVPbkFieldType.h"

EXPORT_C TVPbkFieldTypeMapping::TVPbkFieldTypeMapping(TResourceReader& aReader)
    {
    iVersitProperty.ReadFromResource(aReader);
    iNonVersitType = static_cast<TVPbkNonVersitFieldType>(aReader.ReadUint8());
	}

EXPORT_C TVPbkFieldTypeMapping::TVPbkFieldTypeMapping()
    {
    // create empty object
    }

EXPORT_C const MVPbkFieldType* TVPbkFieldTypeMapping::FindMatch(
                        const MVPbkFieldTypeList& aFieldTypeList) const
    {
    const MVPbkFieldType* result = NULL;

    if (iVersitProperty.Name() != EVPbkVersitNameNone)
        {
        const TInt maxMatchPriority = aFieldTypeList.MaxMatchPriority();
        for (TInt matchPriority = 0; matchPriority <= maxMatchPriority; ++matchPriority)
            {
            result = aFieldTypeList.FindMatch(iVersitProperty, matchPriority);
            if (result)
                {
                break;
                }
            }
        }

    if (!result)
        {
        if (iNonVersitType != EVPbkNonVersitTypeNone)
            {
            result = aFieldTypeList.FindMatch(iNonVersitType);
            }
        }

    return result;
	}

EXPORT_C void TVPbkFieldTypeMapping::SetVersitProperty(
               TVPbkFieldVersitProperty aVersitProperty)
    {
    iVersitProperty.SetName(aVersitProperty.Name());
    iVersitProperty.SetSubField(aVersitProperty.SubField());
    iVersitProperty.SetParameters(aVersitProperty.Parameters());
    iVersitProperty.SetExtensionName(aVersitProperty.ExtensionName());
    }

EXPORT_C void TVPbkFieldTypeMapping::SetNonVersitType(TVPbkNonVersitFieldType aNonVersitType)
    {
    iNonVersitType = aNonVersitType;
    }

// End of File
