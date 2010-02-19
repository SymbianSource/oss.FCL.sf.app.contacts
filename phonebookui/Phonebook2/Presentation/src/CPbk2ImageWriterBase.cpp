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
* Description: 
*       Provides methods for CPbk2ImageManager implementation classes
*
*/


// INCLUDE FILES

#include "CPbk2ImageWriterBase.h"

// From Phonebook2
#include "CPbk2ImageReader.h"
#include "MPbk2ImageOperationObservers.h"
#include "CPbk2ImageData.h"
#include "Pbk2PresentationUtils.h"

// From System
#include <imageconversion.h>
#include <bitmaptransforms.h>

/// Unnamed namespace for local defintions
namespace {

// LOCAL CONSTANTS

/// JPEG file mime type
_LIT8(KJpegMimeType, "image/jpeg");

// States for CPbk2ImageWriterBase
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
#endif // _DEBUG


// ==================== LOCAL FUNCTIONS ====================

/**
 * Comparison operator for TSize objects. Compares width and height members.
 *
 * @param aLhs Reference to left hand side TSize
 * @param aRhs Reference to right hand side TSize
 * @return ETrue if lhs's width and height are less or equal to rhs's
 *         width and height. Otherwise returns EFalse.
 */
 inline TBool operator<=(const TSize& aLhs, const TSize& aRhs)
    {
    return (aLhs.iWidth<=aRhs.iWidth && aLhs.iHeight<=aRhs.iHeight);
    }

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2ImageWriterBase");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG
}  // namespace


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::CPbk2ImageWriterBase
// --------------------------------------------------------------------------
//
CPbk2ImageWriterBase::CPbk2ImageWriterBase
        (MPbk2ImageSetObserver& aObserver) :
    CActive(CActive::EPriorityStandard),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::~CPbk2ImageWriterBase
// --------------------------------------------------------------------------
//
CPbk2ImageWriterBase::~CPbk2ImageWriterBase()
    {
    Cancel();
    delete iImageEncoder;
    delete iBitmapScaler;
    delete iBitmap;
    delete iImageData;
    delete iImageReader;
    }
    
// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::ConstructL()
    {
    __ASSERT_DEBUG(!iImageReader, Panic(EPanicPreCond_ConstructL));
    iImageReader = CPbk2ImageReader::NewL(*this);
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::StartWriteL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::StartWriteL(const CFbsBitmap& aBitmap)
    {
    CancelWrite();
    iBitmap = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Duplicate(aBitmap.Handle()));
    ScaleBitmapL();
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::StartWriteL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::StartWriteL(const TDesC8& aBuffer)
    {
    CancelWrite();

    if (iImageData)
    	{
        delete iImageData;
        iImageData = NULL;
    	}
    iImageData = CPbk2ImageBufferData::NewL(aBuffer);

    iImageReader->RecognizeFormatFromBufferL(aBuffer);

    TPbk2ImageManagerParams readerParams;
    SetupReaderParams(readerParams);
    iImageReader->ReadFromBufferL(aBuffer,&readerParams);
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::StartWriteL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::StartWriteL(const TDesC& aFileName)
    {
    CancelWrite();

    if (iImageData)
    	{
        delete iImageData;
        iImageData = NULL;
    	}
    iImageData = CPbk2ImageFileData::NewL(aFileName);

    iImageReader->RecognizeFormatFromFileL(aFileName);

    TPbk2ImageManagerParams readerParams;
    SetupReaderParams(readerParams);
    iImageReader->ReadFromFileL(aFileName, &readerParams);
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::CancelWrite
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::CancelWrite()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::ImageReadComplete
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::ImageReadComplete
        (CPbk2ImageReader& /*aReader*/, CFbsBitmap* aBitmap)
    {
    delete iBitmap;
    iBitmap = aBitmap;
    NextState();
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::ImageReadFailed
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::ImageReadFailed
        (CPbk2ImageReader& /*aReader*/, TInt aError)
    {
    CancelWrite();
    iObserver.Pbk2ImageSetFailed(*this,aError);
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::ImageOpenComplete
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::ImageOpenComplete(CPbk2ImageReader& /*aReader*/)
    {
    NextState();
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::NextState
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::NextState()
    {
    ++iState;
    TRAPD(err, ExecuteStateL());
    if (err)
        {
        CancelWrite();
        iObserver.Pbk2ImageSetFailed(*this,err);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::ExecuteStateL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::ExecuteStateL()
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

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::CheckFormatAndSizeL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::CheckFormatAndSizeL()
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

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::ScaleBitmapL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::ScaleBitmapL()
    {
    __ASSERT_DEBUG(iBitmap, Panic(EPanicPreCond_ScaleBitmapL));
    iState = EStateScaleBitmap;
    
    Pbk2PresentationImageUtils::CropImageL( 
            *iBitmap, 
            Pbk2PresentationImageUtils::ELandscapeOptimizedCropping, 
            ImageSize() );
    
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

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::CreateImageL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::CreateImageL()
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

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::ConvertToJpegL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::ConvertToJpegL()
    {
    __ASSERT_DEBUG(
        iBitmap && iBitmap->SizeInPixels()<=ImageSize() && iImageEncoder,
        Panic(EPanicPreCond_ConvertToJpegL));
    
    iImageEncoder->Convert(&iStatus, *iBitmap);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::StoreImageL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::StoreImageL()
    {
    iState = EStateStoreImage;
    if (iImageEncoder)
        {
        // Store converted image        
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

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::Complete
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::Complete()
    {
    // End state machine
    iState = EStateEnd;
    // Notify observer
    iObserver.Pbk2ImageSetComplete(*this);
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::SetupReaderParams
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::SetupReaderParams(TPbk2ImageManagerParams& aParams)
    {
    aParams.iSize = ImageSize();
    aParams.iFlags = 
        TPbk2ImageManagerParams::EScaleImage | 
        TPbk2ImageManagerParams::EUseFastScaling;
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::RunL
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::RunL()
    {
    TInt status = iStatus.Int();
    if (status == KErrNone)
        {
        NextState();
        }
    else
        {
        iObserver.Pbk2ImageSetFailed(*this, status);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ImageWriterBase::RunError(TInt aError)
    {
    iObserver.Pbk2ImageSetFailed(*this, aError);
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ImageWriterBase::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ImageWriterBase::DoCancel()
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
