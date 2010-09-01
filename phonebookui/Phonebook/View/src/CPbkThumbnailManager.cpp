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
*       Provides Phonebook thumbnail manager methods.
*
*/


// INCLUDE FILES
#include "CPbkThumbnailManager.h"  // This class
#include "MPbkThumbnailOperationObservers.h"
#include "CPbkThumbnailManagerImpl.h"
#include "TPbkImageLoadParameters.h"

#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>

// ================= MEMBER FUNCTIONS =======================

inline CPbkThumbnailManager::CPbkThumbnailManager
        (CPbkContactEngine& aEngine) :
    iEngine(aEngine)
    {
    }

inline void CPbkThumbnailManager::ConstructL()
    {
    }

EXPORT_C CPbkThumbnailManager* CPbkThumbnailManager::NewL(CPbkContactEngine& aEngine)
    {
    CPbkThumbnailManager* self = new(ELeave) CPbkThumbnailManager(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    return self;
    }

CPbkThumbnailManager::~CPbkThumbnailManager()
    {
    }

EXPORT_C MPbkThumbnailOperation* CPbkThumbnailManager::GetThumbnailAsyncL
        (MPbkThumbnailGetObserver& aObserver, 
        const CPbkContactItem& aContactItem)
    {
    MPbkThumbnailReader* reader = PbkThumbnailOperationFactory::CreateReaderLC
        (aContactItem, aObserver);
    if (reader)
        {
        reader->StartReadL();
        CleanupStack::Pop();  // reader
        }
    return reader;
    }

EXPORT_C MPbkThumbnailOperation* CPbkThumbnailManager::GetThumbnailAsyncL
        (MPbkThumbnailGetObserver& aObserver, 
        const CPbkContactItem& aContactItem,
        TPbkImageLoadParameters& aParams)
    {
    MPbkThumbnailReader* reader = PbkThumbnailOperationFactory::CreateReaderLC
        (aContactItem, aObserver);
    if (reader)
        {
        TSize KNullSize(0,0);
        if (aParams.iSize == KNullSize)
            {
            // Parameters are at defaults, don't pass them
            reader->StartReadL();
            }
        else
            {
            // Frame number parameter has no meaning here.
            aParams.iFrameNumber = 0; 
            reader->StartReadL(&aParams);
            }
        CleanupStack::Pop();  // reader
        }
    return reader;
    }


EXPORT_C MPbkThumbnailOperation* CPbkThumbnailManager::GetThumbnailImageAsyncL
        (MPbkThumbnailGetImageObserver& aObserver, 
        const CPbkContactItem& aContactItem)
    {
    MPbkThumbnailReader* reader = PbkThumbnailOperationFactory::
        CreateImageReaderLC(aContactItem, aObserver);
    if (reader)
        {
        reader->StartReadL();
        CleanupStack::Pop();  // reader
        }
    return reader;
    }

EXPORT_C MPbkThumbnailOperation* CPbkThumbnailManager::SetThumbnailAsyncL
        (MPbkThumbnailSetObserver& aObserver, 
        CPbkContactItem& aContactItem, 
        const CFbsBitmap& aBitmap)
    {
    MPbkThumbnailWriter* writer = PbkThumbnailOperationFactory::CreateWriterLC
        (aContactItem, iEngine.FieldsInfo(), aObserver);
    writer->StartWriteL(aBitmap);
    CleanupStack::Pop();  // writer
    return writer;
    }

EXPORT_C MPbkThumbnailOperation* CPbkThumbnailManager::SetThumbnailAsyncL(
        MPbkThumbnailSetObserver& aObserver, 
        CPbkContactItem& aContactItem, 
        const TDesC8& aImageBuffer)
    {
    MPbkThumbnailWriter* writer = PbkThumbnailOperationFactory::CreateWriterLC
        (aContactItem, iEngine.FieldsInfo(), aObserver);
    writer->StartWriteL(aImageBuffer);
    CleanupStack::Pop();  // writer
    return writer;
    }

EXPORT_C MPbkThumbnailOperation* CPbkThumbnailManager::SetThumbnailAsyncL(
        MPbkThumbnailSetObserver& aObserver, 
        CPbkContactItem& aContactItem, 
        const TDesC& aFileName)
    {
    MPbkThumbnailWriter* writer = PbkThumbnailOperationFactory::CreateWriterLC
        (aContactItem, iEngine.FieldsInfo(), aObserver);
    writer->StartWriteL(aFileName);
    CleanupStack::Pop();  // writer
    return writer;
    }

EXPORT_C TBool CPbkThumbnailManager::HasThumbnail
        (const CPbkContactItem& aContactItem) const
    {
    const TPbkContactItemField* field = aContactItem.FindField(EPbkFieldIdThumbnailImage);
    return (field && !field->IsEmptyOrAllSpaces());
    }

EXPORT_C void CPbkThumbnailManager::RemoveThumbnail
        (CPbkContactItem& aContactItem)
    {
    TPbkContactItemField* field = aContactItem.FindField(EPbkFieldIdThumbnailImage);
    if (field)
        {
        aContactItem.RemoveField(aContactItem.FindFieldIndex(*field));
        }
    }

//  End of File
