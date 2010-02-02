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
*           Provides Phonebook image reader class methods.
*
*/


// INCLUDE FILES
#include "CPbkImageReader.h"
#include "MPbkImageReaderObserver.h"
#include "TPbkImageLoadParameters.h"

#include <imageconversion.h>
#include <bitmaptransforms.h>

/// Unnamed namespace for local defintions
namespace {

// LOCAL CONSTANTS AND MACROS

enum TReaderState
    {
    EStateIntialize = 0,
    EStateOpenImage,
    EStateConvertImageToBitmap,
    EStateScaleBitmap,
    EStateComplete,
    EStateCancelled
    };

const TInt KMaxMimeTypeLength = 256;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ConvertImageToBitmapL = 1,
    EPanicPreCond_ScaleBitmapL,
    EPanicPreCond_Complete,
    EPanicPostCond_Complete,
    EPanicPostCond_OptimalLoadingSize
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CPbkImageReader");
    User::Panic(KPanicText, aPanicCode);
    }
#endif

inline TBool operator<=(const TSize& aLhs, const TSize& aRhs)
    {
    return (aLhs.iWidth<=aRhs.iWidth && aLhs.iHeight<=aRhs.iHeight);
    }

// Copied from CPalbBitmap, remove when possible
TInt Ceil(const TInt aVal, const TInt aDiv)
    {
    return (((aVal%aDiv)>0) ? (TInt)((aVal/aDiv)+1):(TInt)(aVal/aDiv));
    }

// Copied from CPalbBitmap, remove when possible
TSize SizeDividedByValueAndCeil(const TSize& aSize, const TInt aDiv)
    {
    return TSize(
        Ceil( aSize.iWidth, aDiv), 
        Ceil( aSize.iHeight, aDiv) );
    }

// Copied from CPalbBitmap, remove when possible
TSize OptimalLoadingSize(const TSize& aOriginalSize, const TSize& aNeededSize)
    {
    TSize resSize = SizeDividedByValueAndCeil( aOriginalSize, 8 );
    if( !(aNeededSize <= resSize) )
        {
        resSize = SizeDividedByValueAndCeil( aOriginalSize, 4 );
        if( !(aNeededSize <= resSize) )
            {
            resSize = SizeDividedByValueAndCeil( aOriginalSize, 2 );
            if( !(aNeededSize <= resSize) )
                {
                resSize = aOriginalSize;
                }
            }
        }

    // if the resulting size is not the original size,
    // it has to be between needed size and original size
    __ASSERT_DEBUG(resSize == aOriginalSize
                   || (aNeededSize <= resSize && resSize <= aOriginalSize),
                   Panic(EPanicPostCond_OptimalLoadingSize));

    return resSize;
    }

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkImageReader::CPbkImageReader
        (MPbkImageReaderObserver& aObserver) :
    CActive(CActive::EPriorityStandard),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

CPbkImageReader* CPbkImageReader::NewL
        (MPbkImageReaderObserver& aObserver)
    {
    CPbkImageReader* self = new(ELeave) CPbkImageReader(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CPbkImageReader::ConstructL()
    {
    User::LeaveIfError(iFsSession.Connect());
    }

CPbkImageReader::~CPbkImageReader()
    {
    Cancel();
    delete iBitmapScaler;
    delete iImageDecoder;
    delete iMimeString;
    delete iBitmap;
    iFsSession.Close();
    }

void CPbkImageReader::ReadFromFileL
        (const TDesC& aFileName, const TPbkImageLoadParameters* aParams)
    {
    InitReadL(aParams);
    delete iImageDecoder;
    iImageDecoder = NULL;
    iImageDecoder = CImageDecoder::FileNewL(iFsSession, aFileName);

    // Make the open phase asynchronous as well by signaling own iStatus
    iState = EStateOpenImage;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbkImageReader::ReadFromBufferL
        (const TDesC8& aBuffer, const TPbkImageLoadParameters* aParams/*=NULL*/)
    {
    InitReadL(aParams);
    delete iImageDecoder;
    iImageDecoder = NULL;
    iImageDecoder = CImageDecoder::DataNewL(iFsSession, aBuffer);

    // Make the open phase asynchronous as well by signaling own iStatus
    iState = EStateOpenImage;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

const TDesC8& CPbkImageReader::MimeString() const
    {
    if (iMimeString)
        {
        return *iMimeString;
        }
    else
        {
        return KNullDesC8;
        }
    }

void CPbkImageReader::RecognizeFormatFromFileL(const TDesC& aFileName)
    {
    delete iMimeString;
    iMimeString = NULL;
    iMimeString = HBufC8::NewL(KMaxMimeTypeLength);
    TPtr8 mimePtr = iMimeString->Des();
    CImageDecoder::GetMimeTypeFileL(iFsSession, aFileName, mimePtr);
    }

void CPbkImageReader::RecognizeFormatFromBufferL(const TDesC8& aBuffer)
    {
    delete iMimeString;
    iMimeString = NULL;
    iMimeString = HBufC8::NewL(KMaxMimeTypeLength);
    TPtr8 mimePtr = iMimeString->Des();
    CImageDecoder::GetMimeTypeDataL(aBuffer, mimePtr);
    }

void CPbkImageReader::FrameInfo(TInt aFrame, TFrameInfo& aInfo) const
    {
    aInfo = iImageDecoder->FrameInfo(aFrame);
    }

TInt CPbkImageReader::FrameCount() const
    {
    return iImageDecoder->FrameCount();
    }

void CPbkImageReader::NextStateL()
    {
    ++iState;

    switch (iState)
        {
        case EStateConvertImageToBitmap:
            {
            ConvertImageToBitmapL();
            break;
            }
        case EStateScaleBitmap:
            {
            ScaleBitmapL();
            break;
            }
        case EStateComplete:
            {
            Complete();
            break;
            }
        default:
            {
            // iImageReader might sometimes complete although it has been canceled!
            // Catch those cases here.
            break;
            }
        }
    }

void CPbkImageReader::ConvertImageToBitmapL()
    {
    __ASSERT_DEBUG(iImageDecoder && !iBitmap, 
        Panic(EPanicPreCond_ConvertImageToBitmapL));

    // Get image size
    const TFrameInfo& frameInfo = 
        iImageDecoder->FrameInfo(iParams.iFrameNumber);
    TSize bitmapSize = frameInfo.iOverallSizeInPixels;
    if (iParams.iFlags & TPbkImageLoadParameters::EScaleImage)
        {
        // Get optimal loading size >= desired size
        bitmapSize = OptimalLoadingSize(bitmapSize,iParams.iSize);
        }

    // Create bitmap
    delete iBitmap;
	iBitmap = NULL;
    iBitmap = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Create(bitmapSize,iParams.iDisplayMode));

    // Convert image to bitmap
    iImageDecoder->Convert(&iStatus, *iBitmap, iParams.iFrameNumber);
    SetActive();
    }

void CPbkImageReader::ScaleBitmapL()
    {
    __ASSERT_DEBUG(iBitmap, Panic(EPanicPreCond_ScaleBitmapL));

    if ((iParams.iFlags & TPbkImageLoadParameters::EScaleImage) && 
        !(iParams.iFlags & TPbkImageLoadParameters::EUseFastScaling))
        {
        const TSize bitmapSize = iBitmap->SizeInPixels();
        if (bitmapSize.iWidth > iParams.iSize.iWidth || 
            bitmapSize.iHeight > iParams.iSize.iHeight)
            {
            if (!iBitmapScaler)
                {
                iBitmapScaler = CBitmapScaler::NewL();
                }
            iBitmapScaler->Scale(&iStatus, *iBitmap, iParams.iSize);
            SetActive();
            return;
            }
        }

    // No scaling requested or needed, go directly to next state
    NextStateL();
    }

void CPbkImageReader::Complete()
    {
    __ASSERT_DEBUG(iImageDecoder && iBitmap, Panic(EPanicPreCond_Complete));

    // End state machine
    ++iState;

    // Close the image source
    CloseImage();

    // Release ownership of iBitmap
    CFbsBitmap* bitmap = iBitmap;
    iBitmap = NULL;

    __ASSERT_DEBUG(!iImageDecoder && !iBitmap, 
        Panic(EPanicPostCond_Complete));

    // Notify observer about completion
    iObserver.ImageReadComplete(*this,bitmap);
    }

void CPbkImageReader::InitReadL(const TPbkImageLoadParameters* aParams)
    {
    Cancel();
    if (aParams)
        {
        iParams = *aParams;
        }
    iState = EStateOpenImage;
    }

void CPbkImageReader::CloseImage()
    {
    delete iImageDecoder;
    iImageDecoder = NULL;
    }

void CPbkImageReader::RunL()
    {
    TInt status = iStatus.Int();
    switch (status)
        {
        case KErrNone:
            {
            if (iState == EStateOpenImage)
                {
                iObserver.ImageOpenComplete(*this);
                }
            NextStateL();
            break;
            }
        case KErrCancel:
            {
            // In case of cancel the observer is not signaled
            break;
            }
        default:
            {
            iObserver.ImageReadFailed(*this, status);
            break;
            }
        }
    }

void CPbkImageReader::Cancel()
    {
    if (!IsActive())
        {
        DoCancel();
        }
    else
        {
        CActive::Cancel();
        }
    }

TInt CPbkImageReader::RunError(TInt aError)
    {
    iObserver.ImageReadFailed(*this, aError);
    return KErrNone;
    }

void CPbkImageReader::DoCancel()
    {
    if (iImageDecoder)
        {
        iImageDecoder->Cancel();
        }
    if (iBitmapScaler)
        {
        iBitmapScaler->Cancel();
        }
    CloseImage();
    delete iBitmap;
    iBitmap = NULL;
    iState = EStateCancelled;
    }

//  End of File  
