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
*		Phonebook field type from CContactItemField
*
*/


// INCLUDE FILES
#include "TPbkContactItemFieldType.h"
#include <cntitem.h>
#include "MPbkVcardProperty.h"
#include "CPbkFieldInfo.h"


// ==================== MEMBER FUNCTIONS ====================

inline void TPbkContactItemFieldType::AddStorageType
        (TStorageType aStorageType, const CPbkUidMap& aUidMap)
    {
    iTypeSignature.AddType(TUid::Uid(aStorageType), aUidMap);
    }

void TPbkContactItemFieldType::AddContentType
        (const CContentType& aContentType, const CPbkUidMap& aUidMap)
    {
    // Field vCard mapping
    iTypeSignature.AddType(aContentType.Mapping(), aUidMap);
    // All the other types in the content type
    const TInt fieldTypeCount = aContentType.FieldTypeCount();
    for (TInt i=0; i < fieldTypeCount; ++i)
        {
        iTypeSignature.AddType(aContentType.FieldType(i), aUidMap);
        }
    }

TPbkContactItemFieldType::TPbkContactItemFieldType
        (const CPbkFieldInfo& aFieldInfo, 
        const CPbkUidMap& aUidMap)
    {
    // Field Storage type
    AddStorageType(aFieldInfo.FieldStorageType(), aUidMap);
    // Field content type
    AddContentType(aFieldInfo.ContentType(), aUidMap);
    }

TPbkContactItemFieldType::TPbkContactItemFieldType
        (const CPbkFieldInfo& aFieldInfo, 
        const MPbkVcardProperty& aImportProperty,
        const CPbkUidMap& aUidMap)
    {
    // Field Storage type
    AddStorageType(aFieldInfo.FieldStorageType(), aUidMap);
    // Field vCard mapping
    iTypeSignature.AddType(aFieldInfo.ContentType().Mapping(), aUidMap);
    // Field main Contact Model type
    iTypeSignature.AddType(aFieldInfo.ContentType().FieldType(0), aUidMap);
    // Additional vCard import parameters
    iTypeSignature.AddParameters(aImportProperty.PropertyParameters(), aUidMap);
    }

TPbkContactItemFieldType::TPbkContactItemFieldType
        (const CContactItemField& aField, const CPbkUidMap& aUidMap)
    {
    // Field Storage type
    AddStorageType(aField.StorageType(), aUidMap);
    // Field content type
    AddContentType(aField.ContentType(), aUidMap);
    }


// End of File
