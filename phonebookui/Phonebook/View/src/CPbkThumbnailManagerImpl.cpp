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
*       Provides methods for CPbkThumbnailManager implementation classes.
*
*/


// INCLUDE FILES
#include "CPbkThumbnailManagerImpl.h"
#include "CPbkEmbeddedThumbnail.h"

#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>
#include <CPbkFieldsInfo.h>

// ================= MEMBER FUNCTIONS =======================

MPbkThumbnailReader* PbkThumbnailOperationFactory::CreateReaderLC
        (const CPbkContactItem& aContact,
        MPbkThumbnailGetObserver& aObserver,
        TInt aFieldId /*= EPbkFieldIdThumbnailImage*/)
    {
    const TPbkContactItemField* field = aContact.FindField(aFieldId);
    MPbkThumbnailReader* reader = NULL;
    if (field)
        {
        switch (field->StorageType())
            {
            case KStorageTypeStore:
                {
                reader = CPbkEmbeddedThumbnailReader::NewLC(*field,aObserver);
                break;
                }
              default:
                {
                // Unsupported storage type -> return NULL
                break;
                }
            }
        }
    return reader;
    }

MPbkThumbnailReader* PbkThumbnailOperationFactory::CreateImageReaderLC
        (const CPbkContactItem& aContact,
        MPbkThumbnailGetImageObserver& aObserver,
        TInt aFieldId /*= EPbkFieldIdThumbnailImage*/)
    {
    const TPbkContactItemField* field = aContact.FindField(aFieldId);
    MPbkThumbnailReader* reader = NULL;
    if (field)
        {
        switch (field->StorageType())
            {
            case KStorageTypeStore:
                {
                reader = CPbkEmbeddedThumbnailImageReader::NewLC(*field,aObserver);
                break;
                }
            default:
                {
                // Unsupported storage type -> return NULL
                break;
                }
            }
        }
    return reader;
    }

MPbkThumbnailWriter* PbkThumbnailOperationFactory::CreateWriterLC
        (CPbkContactItem& aContact, 
        const CPbkFieldsInfo& aFieldsInfo,
        MPbkThumbnailSetObserver& aObserver,
        TInt aFieldId /*= EPbkFieldIdThumbnailImage*/)
    {
    MPbkThumbnailWriter* writer = NULL;

    TPbkContactItemField* field = aContact.FindField(aFieldId);
    if (field && !SupportedStorageType(field->StorageType()))
        {
        // Existing field's storage type is not supported -> remove field
        aContact.RemoveField(aContact.FindFieldIndex(*field));
        field = NULL;
        }

    if (!field)
        {
        // No thumbnail field -> create one
        CPbkFieldInfo* fieldInfo = aFieldsInfo.Find(aFieldId);
        if (!SupportedStorageType(fieldInfo->FieldStorageType()))
            {
            // Global field type's storage type is not supported -> abort
            User::Leave(KErrNotSupported);
            }     
        field = &aContact.AddFieldL(*fieldInfo);
        }

    switch (field->StorageType())
        {
        case KStorageTypeStore:
            {
            writer = CPbkEmbeddedThumbnailWriter::NewLC(*field, aObserver);
            break;
            }
        default:
            {
            // Field's storage type is not supported
            User::Leave(KErrNotSupported);
            break;
            }
        }
    return writer;
    }

TBool PbkThumbnailOperationFactory::SupportedStorageType
        (TStorageType aStorageType)
    {
    return KStorageTypeStore==aStorageType ? ETrue: EFalse;
    }

//  End of File
