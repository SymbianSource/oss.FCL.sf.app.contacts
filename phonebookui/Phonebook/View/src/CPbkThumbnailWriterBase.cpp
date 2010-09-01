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
*       Provides methods for CPbkThumbnailManager implementation classes
*
*/


// INCLUDE FILES

#include "CPbkThumbnailWriterBase.h"
#include "CPbkImageReader.h"
#include "MPbkThumbnailOperationObservers.h"
#include "PbkImageData.h"

#include <imageconversion.h>
#include <bitmaptransforms.h>

/// Unnamed namespace for local defintions
namespace {

// LOCAL CONSTANTS

/// JPEG file mime type
_LIT8(KJpegMimeType, "image/jpeg");

// States for CPbkThumbnailWriterBase
enum TWriterState
    {
    EStateInitialize = 0,
    EStateCheckFormatAndSize,
    EStateScaleBitmap,
    EStateCreateImage,
    EStateConvertToJpeg,
    EStateStoreImage,
    EStateComplete,
    EStateEnd
    };

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ConstructL,
    EPanicPreCond_CheckFormatAndSizeL,
    EPanicPreCond_ScaleBitmapL,
    EPanicPreCond_CreateImageL,
    EPanicPreCond_ConvertToJpegL,
    EPanicPreCond_StoreImageL,
    EPanicPreCond_DoCancel
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

inline TBool operator<=(const TSize& aLhs, const TSize& aRhs)
    {
    return (aLhs.iWidth<=aRhs.iWidth && aLhs.iHeight<=aRhs.iHeight);
    }

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkThumbnailWriterBase");
    User::Panic(KPanicText,aReason);
    }
#endif
}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkThumbnailWriterBase::CPbkThumbnailWriterBase
        (MPbkThumbnailSetObserver& aObserver) :
    CActive(CActive::EPriorityStandard),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

void CPbkThumbnailWriterBase::ConstructL()
    {
    __ASSERT_DEBUG(!iImageReader, Panic(EPanicPreCond_ConstructL));
    iImageReader = CPbkImageReader::NewL(*this);
    }

CPbkThumbnailWriterBase::~CPbkThumbnailWriterBase()
    {
    Cancel();
    delete iImageEncoder;
    delete iBitmapScaler;
    delete iBitmap;
    delete iImageData;
    delete iImageReader;
    }

void CPbkThumbnailWriterBase::StartWriteL(const CFbsBitmap& aBitmap)
    {
    CancelWrite();
    iBitmap = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Duplicate(aBitmap.Handle()));
    ScaleBitmapL();
    }

void CPbkThumbnailWriterBase::StartWriteL(const TDesC8& aBuffer)
    {
    CancelWrite();

    iImageData = CPbkImageBufferData::NewL(aBuffer);

    iImageReader->RecognizeFormatFromBufferL(aBuffer);

    TPbkImageLoadParameters readerParams;
    SetupReaderParams(readerParams);
    iImageReader->ReadFromBufferL(aBuffer,&readerParams);
    }

void CPbkThumbnailWriterBase::StartWriteL(const TDesC& aFileName)
    {
    CancelWrite();

    iImageData = CPbkImageFileData::NewL(aFileName);

    iImageReader->RecognizeFormatFromFileL(aFileName);

    TPbkImageLoadParameters readerParams;
    SetupReaderParams(readerParams);
    iImageReader->ReadFromFileL(aFileName, &readerParams);
    }

void CPbkThumbnailWriterBase::CancelWrite()
    {
    Cancel();
    }

void CPbkThumbnailWriterBase::ImageReadComplete
        (CPbkImageReader& /*aReader*/, CFbsBitmap* aBitmap)
    {
    delete iBitmap;
    iBitmap = aBitmap;
    NextState();
    }

void CPbkThumbnailWriterBase::ImageReadFailed
        (CPbkImageReader& /*aReader*/, TInt aError)
    {
    CancelWrite();
    iObserver.PbkThumbnailSetFailed(*this,aError);
    }

void CPbkThumbnailWriterBase::ImageOpenComplete(CPbkImageReader& /*aReader*/)
    {
    NextState();
    }

void CPbkThumbnailWriterBase::NextState()
    {
    ++iState;
    TRAPD(err, ExecuteStateL());
    if (err)
        {
        CancelWrite();
        iObserver.PbkThumbnailSetFailed(*this,err);
        }
    }

void CPbkThumbnailWriterBase::ExecuteStateL()
    {
    switch (iState)
        {
        case EStateCheckFormatAndSize:
            {
            CheckFormatAndSizeL();
            break;
            }
        case EStateScaleBitmap:
            {
            ScaleBitmapL();
            break;
            }
        case EStateCreateImage:
            {
            CreateImageL();
            break;
            }
        case EStateConvertToJpeg:
            {
            ConvertToJpegL();
            break;
            }
        case EStateStoreImage:
            {
            StoreImageL();
            break;
            }
        case EStateComplete:
            {
            Complete();
            break;
            }
        default:
            {
            // Media server objects might sometimes complete although they have
            // been canceled. Catch those cases here.
            CancelWrite();
            break;
            }
        }
    }

void CPbkThumbnailWriterBase::CheckFormatAndSizeL()
    {
    __ASSERT_DEBUG(iImageReader, Panic(EPanicPreCond_CheckFormatAndSizeL));
    iState = EStateCheckFormatAndSize;
    TFrameInfo frameInfo;
    iImageReader->FrameInfo(0,frameInfo);
    if (frameInfo.iOverallSizeInPixels <= ImageSize() &&
        iImageReader->MimeString() == KJpegMimeType)
        {
        // Image resolution is <= personal image size and it is in JPEG
        // format -> store the image directly skipping scaling and
        // conversion.
        iImageReader->Cancel();
        StoreImageL();
        }
    }

void CPbkThumbnailWriterBase::ScaleBitmapL()
    {
    __ASSERT_DEBUG(iBitmap, Panic(EPanicPreCond_ScaleBitmapL));
    iState = EStateScaleBitmap;
    if (iBitmap->SizeInPixels() <= ImageSize())
        {
        // Bitmap size is <= personal image size -> skip scaling
        CreateImageL();
        }
    else
        {
        if (!iBitmapScaler)
            {
            iBitmapScaler = CBitmapScaler::NewL();
            }
        iBitmapScaler->Scale(&iStatus, *iBitmap, ImageSize());
        SetActive();
        }
    }

void CPbkThumbnailWriterBase::CreateImageL()
    {
    __ASSERT_DEBUG(
        iBitmap && iBitmap->SizeInPixels()<=ImageSize() && !iImageEncoder,
        Panic(EPanicPreCond_CreateImageL));
    iState = EStateCreateImage;
    iImageEncoder = CreateImageWriterL();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbkThumbnailWriterBase::ConvertToJpegL()
    {
    __ASSERT_DEBUG(
        iBitmap && iBitmap->SizeInPixels()<=ImageSize() && iImageEncoder,
        Panic(EPanicPreCond_ConvertToJpegL));
    
    iImageEncoder->Convert(&iStatus, *iBitmap);
    SetActive();
    }

void CPbkThumbnailWriterBase::StoreImageL()
    {
    iState = EStateStoreImage;
    if (iImageEncoder)
        {
        // Store converted image
        //iImageWriter->Close();
        StoreImageL(*iImageEncoder);
        }
    else
        {
        // Store input image directly
        __ASSERT_DEBUG(iImageData, Panic(EPanicPreCond_StoreImageL));
        StoreImageL(*iImageData);
        }
    // this causes moving to next state asynchronously
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbkThumbnailWriterBase::Complete()
    {
    // End state machine
    iState = EStateEnd;
    // Notify observer
    iObserver.PbkThumbnailSetComplete(*this);
    }

void CPbkThumbnailWriterBase::SetupReaderParams(TPbkImageLoadParameters& aParams)
    {
    aParams.iSize = ImageSize();
    aParams.iFlags = 
        TPbkImageLoadParameters::EScaleImage | 
        TPbkImageLoadParameters::EUseFastScaling;
    }

void CPbkThumbnailWriterBase::RunL()
    {
    TInt status = iStatus.Int();
    if (status == KErrNone)
        {
        NextState();
        }
    else
        {
        iObserver.PbkThumbnailSetFailed(*this, status);
        }
    }

TInt CPbkThumbnailWriterBase::RunError(TInt aError)
    {
    iObserver.PbkThumbnailSetFailed(*this, aError);
    return KErrNone;
    }

void CPbkThumbnailWriterBase::DoCancel()
    {
    __ASSERT_DEBUG(iImageReader, Panic(EPanicPreCond_DoCancel));
    // Cancel own operations
    iState = EStateInitialize;
    if (iImageEncoder)
        {
        iImageEncoder->Cancel();
        delete iImageEncoder;
        iImageEncoder = NULL;
        }
    if (iBitmapScaler) 
        {
        iBitmapScaler->Cancel();
        }

    iImageReader->Cancel();
    delete iImageData;
    iImageData = NULL;
    delete iBitmap;
    iBitmap = NULL;
    }


//  End of File
