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
*       Provides Phonebook Image manager methods.
*
*/


// INCLUDE FILES
#include "CPbkImageManager.h"  // This class
#include "MPbkThumbnailOperationObservers.h"
#include "MPbkImageOperationObservers.h"
#include "CPbkThumbnailManagerImpl.h"
#include "TPbkImageLoadParameters.h"
#include "CPbkObserverAdapter.h"
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>

/// Unnamed namespace for local definitions
namespace {
    
    // LOCAL CONSTANTS AND MACROS
    enum TPbkImageManagerPanic
        {
        // Panic code if CPbkImageManager initialization parameters 
        // are not valid
        EPbkImageManagerInvalidArgument = 1
        };

    static void Panic(TPbkImageManagerPanic aReason)
        {
        _LIT(KPanicText, "CPbkImageManager");
        User::Panic(KPanicText, aReason);
        }
} // namespace

// ================= MEMBER FUNCTIONS =======================

CPbkImageManager::CPbkImageManager(CPbkContactEngine& aEngine) :
    iEngine(aEngine)
    {
    }

inline void CPbkImageManager::ConstructL()
    {
    iPbkObserverAdapter = CPbkObserverAdapter::NewL();
    }

EXPORT_C CPbkImageManager* CPbkImageManager::NewL(CPbkContactEngine& aEngine)
    {
    CPbkImageManager* self = new(ELeave) CPbkImageManager(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    return self;
    }

CPbkImageManager::~CPbkImageManager()
    {
    delete iPbkObserverAdapter;
    }

EXPORT_C MPbkImageOperation* CPbkImageManager::GetImageAsyncL(
        TPbkFieldId aFieldId,
        MPbkImageGetObserver& aObserver, 
        const CPbkContactItem& aContactItem)
    {
    __ASSERT_ALWAYS(aFieldId == EPbkFieldIdCodImageID ||
        aFieldId == EPbkFieldIdThumbnailImage, 
        Panic(EPbkImageManagerInvalidArgument));

    iPbkObserverAdapter->SetPbkImageGetObserver(aObserver);    
    MPbkThumbnailReader* reader = PbkThumbnailOperationFactory::CreateReaderLC
        (aContactItem, iPbkObserverAdapter->PbkThumbnailGetObserver(), aFieldId);
    if (reader)
        {
        reader->StartReadL();
        CleanupStack::Pop();  // reader
        }
    return reader;

    }

EXPORT_C MPbkImageOperation* CPbkImageManager::GetImageAsyncL(
        TPbkFieldId aFieldId,
        MPbkImageGetObserver& aObserver, 
        const CPbkContactItem& aContactItem,
        TPbkImageLoadParameters& aParams)
    {
    __ASSERT_ALWAYS(aFieldId == EPbkFieldIdCodImageID ||
        aFieldId == EPbkFieldIdThumbnailImage,
        Panic(EPbkImageManagerInvalidArgument));

    iPbkObserverAdapter->SetPbkImageGetObserver(aObserver);
    MPbkThumbnailReader* reader = PbkThumbnailOperationFactory::CreateReaderLC(
        aContactItem, iPbkObserverAdapter->PbkThumbnailGetObserver(),aFieldId);
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

EXPORT_C MPbkImageOperation* CPbkImageManager::GetImageAsyncL(
        TPbkFieldId aFieldId,
        MPbkImageGetImageObserver& aObserver, 
        const CPbkContactItem& aContactItem)
    {
    __ASSERT_ALWAYS(aFieldId == EPbkFieldIdCodImageID ||
        aFieldId == EPbkFieldIdThumbnailImage,
        Panic(EPbkImageManagerInvalidArgument));

    iPbkObserverAdapter->SetPbkImageGetImageObserver(aObserver);
    MPbkThumbnailReader* reader = PbkThumbnailOperationFactory::
        CreateImageReaderLC(aContactItem, 
            iPbkObserverAdapter->PbkThumbnailGetImageObserver(), aFieldId);
    if (reader)
        {
        reader->StartReadL();
        CleanupStack::Pop();  // reader
        }

    return reader;
    }

EXPORT_C MPbkImageOperation* CPbkImageManager::SetImageAsyncL(
        TPbkFieldId aFieldId,
        MPbkImageSetObserver& aObserver, 
        CPbkContactItem& aContactItem, 
        const CFbsBitmap& aBitmap)
    {
    __ASSERT_ALWAYS(aFieldId == EPbkFieldIdCodImageID ||
        aFieldId == EPbkFieldIdThumbnailImage, 
        Panic(EPbkImageManagerInvalidArgument));

    iPbkObserverAdapter->SetPbkImageSetObserver(aObserver);
    MPbkThumbnailWriter* writer = PbkThumbnailOperationFactory::CreateWriterLC(
        aContactItem, iEngine.FieldsInfo(), 
        iPbkObserverAdapter->PbkThumbnailSetObserver(), aFieldId);
    writer->StartWriteL(aBitmap);
    CleanupStack::Pop();  // writer
    return writer;
    }

EXPORT_C MPbkImageOperation* CPbkImageManager::SetImageAsyncL(
        TPbkFieldId aFieldId,
        MPbkImageSetObserver& aObserver, 
        CPbkContactItem& aContactItem, 
        const TDesC8& aImageBuffer)
    {
    __ASSERT_ALWAYS(aFieldId == EPbkFieldIdCodImageID ||
        aFieldId == EPbkFieldIdThumbnailImage, 
        Panic(EPbkImageManagerInvalidArgument));

    iPbkObserverAdapter->SetPbkImageSetObserver(aObserver);
    MPbkThumbnailWriter* writer = PbkThumbnailOperationFactory::CreateWriterLC(
        aContactItem, iEngine.FieldsInfo(), 
        iPbkObserverAdapter->PbkThumbnailSetObserver(), aFieldId);
    writer->StartWriteL(aImageBuffer);
    CleanupStack::Pop();  // writer
    return writer;
    }

EXPORT_C MPbkImageOperation* CPbkImageManager::SetImageAsyncL(
        TPbkFieldId aFieldId,
        MPbkImageSetObserver& aObserver, 
        CPbkContactItem& aContactItem, 
        const TDesC& aFileName)
    {
    __ASSERT_ALWAYS(aFieldId == EPbkFieldIdCodImageID ||
        aFieldId == EPbkFieldIdThumbnailImage, 
        Panic(EPbkImageManagerInvalidArgument));

    iPbkObserverAdapter->SetPbkImageSetObserver(aObserver);
    MPbkThumbnailWriter* writer = PbkThumbnailOperationFactory::CreateWriterLC(
        aContactItem, iEngine.FieldsInfo(), 
        iPbkObserverAdapter->PbkThumbnailSetObserver(), aFieldId);
    writer->StartWriteL(aFileName);
    CleanupStack::Pop();  // writer
    return writer;

    }

EXPORT_C TBool CPbkImageManager::HasImage(
        TPbkFieldId aFieldId,
        const CPbkContactItem& aContactItem) const
    {
    __ASSERT_ALWAYS(aFieldId == EPbkFieldIdCodImageID ||
        aFieldId == EPbkFieldIdThumbnailImage, 
        Panic(EPbkImageManagerInvalidArgument));

    const TPbkContactItemField* field = aContactItem.FindField(aFieldId);
    return (field && !field->IsEmptyOrAllSpaces());
    }

EXPORT_C void CPbkImageManager::RemoveImage(
        TPbkFieldId aFieldId,
        CPbkContactItem& aContactItem)
    {
    __ASSERT_ALWAYS(aFieldId == EPbkFieldIdCodImageID ||
        aFieldId == EPbkFieldIdThumbnailImage, 
        Panic(EPbkImageManagerInvalidArgument));

    TPbkContactItemField* field = aContactItem.FindField(aFieldId);
    if (field)
        {
        aContactItem.RemoveField(aContactItem.FindFieldIndex(*field));
        }
    }

//  End of File
