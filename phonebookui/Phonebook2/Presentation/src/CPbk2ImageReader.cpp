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
*           Provides Phonebook2 image reader class methods.
*
*/


// INCLUDE FILES
#include "CPbk2ImageReader.h"

// From Phonebook2
#include "MPbk2ImageReaderObserver.h"
#include "TPbk2ImageManagerParams.h"

// From Virtual Phonebook

// From System
#include <imageconversion.h>
#include <bitmaptransforms.h>
#include <JP2KUids.hrh>

/// Unnamed namespace for local defintions
namespace {

// ============== LOCAL CONSTANTS AND MACROS ===============

enum TReaderState
    {
    EStateIntialize = 0,
    EStateOpenImage,
    EStateConvertImageToBitmap,
    EStateScaleBitmap,
    EStateComplete,
    EStateCancelled
    };

// Definition for max mime type length
const TInt KMaxMimeTypeLength(256);

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ConvertImageToBitmapL = 1,
    EPanicPreCond_ScaleBitmapL,
    EPanicPreCond_Complete,
    EPanicPostCond_Complete,
    EPanicPostCond_OptimalLoadingSize
    };
#endif // _DEBUG


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CPbk2ImageReader");
    User::Panic(KPanicText, aPanicCode);
    }
#endif  // _DEBUG

// --------------------------------------------------------------------------

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

/**
 * Ceils division result based on modulus.
 *
 * @param aVal Divident. 
 * @param aDiv Divider.
 * @return If modulus is zero, returns the result of aVal / aDiv. Otherwise
 *         returns returns the result of aVal / aDiv increased by one. 
 *
 * NOTE: Copied from CPalbBitmap, remove when possible
 */
TInt Ceil(const TInt aVal, const TInt aDiv)
    {
    return (((aVal%aDiv)>0) ? (TInt)((aVal/aDiv)+1):(TInt)(aVal/aDiv));
    }


/**
 * Calculates the the size based on divider. Uses Ceil function for ceiling
 * the calculated size.
 *
 * @param aSize Orginal size.
 * @param aDiv Divider.
 * @return Calculated size. The result's width and height might be adjusted
 *         according to the results of calling Ceil function.
 *
 * @see Ceil
 *
 * NOTE: Copied from CPalbBitmap, remove when possible
 */
 TSize SizeDividedByValueAndCeil(const TSize& aSize, const TInt aDiv)
    {
    return TSize(
        Ceil( aSize.iWidth, aDiv), 
        Ceil( aSize.iHeight, aDiv) );
    }

/**
 * Calculates the optimal loading size. If optimal loading size cannot be
 * calculated, function panics with 
 * CPbk2ImageReader:EPanicPostCond_OptimalLoadingSize panic code.
 *
 * @param aOriginalSize Orginal size.
 * @param aNeededSize Needed size.
 * @return Calculated optimal size.
 *
 * @see SizeDividedByValueAndCeil
 * @see Ceil
 *
 * NOTE: Copied from CPalbBitmap, remove when possible
 */
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

// --------------------------------------------------------------------------
// CPbk2ImageReader::CPbk2ImageReader
// --------------------------------------------------------------------------
//
inline CPbk2ImageReader::CPbk2ImageReader
        (MPbk2ImageReaderObserver& aObserver) :
    CActive(CActive::EPriorityStandard),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::~CPbk2ImageReader
// --------------------------------------------------------------------------
//
CPbk2ImageReader::~CPbk2ImageReader()
    {
    Cancel();
    delete iBitmapScaler;
    delete iImageDecoder;
    delete iMimeString;
    delete iBitmap;
    iFsSession.Close();
    }
    
// --------------------------------------------------------------------------
// CPbk2ImageReader::NewL
// --------------------------------------------------------------------------
//
CPbk2ImageReader* CPbk2ImageReader::NewL
        (MPbk2ImageReaderObserver& aObserver)
    {
    CPbk2ImageReader* self = new(ELeave) CPbk2ImageReader(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::ConstructL()
    {
    User::LeaveIfError(iFsSession.Connect());
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::ReadFromFileL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::ReadFromFileL
        (const TDesC& aFileName, const TPbk2ImageManagerParams* aParams)
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

// --------------------------------------------------------------------------
// CPbk2ImageReader::ReadFromBufferL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::ReadFromBufferL
        (const TDesC8& aBuffer, const TPbk2ImageManagerParams* aParams/*=NULL*/)
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

// --------------------------------------------------------------------------
// CPbk2ImageReader::MimeString
// --------------------------------------------------------------------------
//
const TDesC8& CPbk2ImageReader::MimeString() const
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

// --------------------------------------------------------------------------
// CPbk2ImageReader::RecognizeFormatFromFileL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::RecognizeFormatFromFileL(const TDesC& aFileName)
    {
    delete iMimeString;
    iMimeString = NULL;
    iMimeString = HBufC8::NewL(KMaxMimeTypeLength);
    TPtr8 mimePtr = iMimeString->Des();
    CImageDecoder::GetMimeTypeFileL(iFsSession, aFileName, mimePtr);
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::RecognizeFormatFromBufferL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::RecognizeFormatFromBufferL(const TDesC8& aBuffer)
    {
    delete iMimeString;
    iMimeString = NULL;
    iMimeString = HBufC8::NewL(KMaxMimeTypeLength);
    TPtr8 mimePtr = iMimeString->Des();
    CImageDecoder::GetMimeTypeDataL(aBuffer, mimePtr);
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::FrameInfo
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::FrameInfo(TInt aFrame, TFrameInfo& aInfo) const
    {
    aInfo = iImageDecoder->FrameInfo(aFrame);
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::FrameCount
// --------------------------------------------------------------------------
//
TInt CPbk2ImageReader::FrameCount() const
    {
    return iImageDecoder->FrameCount();
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::NextStateL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::NextStateL()
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
            CropImageToSquareL();
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
            // iImageReader might sometimes complete although it has been canceled.
            // Catch those cases here.
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::ConvertImageToBitmapL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::ConvertImageToBitmapL()
    {
    __ASSERT_DEBUG(iImageDecoder && !iBitmap, 
        Panic(EPanicPreCond_ConvertImageToBitmapL));

    // Get image size
    const TFrameInfo& frameInfo = 
        iImageDecoder->FrameInfo(iParams.iFrameNumber);
    TSize bitmapSize = frameInfo.iOverallSizeInPixels;
    if (iParams.iFlags & TPbk2ImageManagerParams::EScaleImage)
        {
        // Get optimal loading size >= desired size
        bitmapSize = OptimalLoadingSize(bitmapSize,iParams.iSize);
        }

    // Create bitmap
    delete iBitmap;
	iBitmap = NULL;
    iBitmap = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Create(bitmapSize, iParams.iDisplayMode));

    // Convert image to bitmap
    iImageDecoder->Convert(&iStatus, *iBitmap, iParams.iFrameNumber);
    SetActive();
    }


// --------------------------------------------------------------------------
// CPbk2ImageReader::CropImageToSquareL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::CropImageToSquareL()
	{
	TInt useCropping = 0x0008;
	// if cropping is wanted
	if( iParams.iFlags & useCropping )	//TODO change value ( contacts_plat/image_managemet_api/TPbk2ImageManagerParams )
		{
		TSize size = iBitmap->SizeInPixels();
		// crop the image only if the width is bigger than height 
		if( size.iHeight >= size.iWidth )
			{
			// no cropping
			return;
			}
		// take the shorter side
		TInt sideSize = size.iHeight;	
		
		// set target size
		TSize targetSize( sideSize,sideSize );
	
		// crop from both sides
		TRect targetRect( TPoint( ( size.iWidth - targetSize.iWidth ) / 2,
								  ( size.iHeight - targetSize.iHeight ) / 2 ),
									targetSize );
		
		// create new bitmap
		CFbsBitmap* target = new( ELeave ) CFbsBitmap;
		CleanupStack::PushL( target );
		User::LeaveIfError( target->Create( targetSize, iBitmap->DisplayMode() ) );
		
		// get scanline
		HBufC8* scanLine = HBufC8::NewLC( iBitmap->ScanLineLength
			( targetSize.iWidth, iBitmap->DisplayMode() ) );
		TPtr8 scanLinePtr = scanLine->Des();
		
		TPoint startPoint( targetRect.iTl.iX, targetRect.iTl.iY );
		TInt targetY = 0;
		for (; startPoint.iY < targetRect.iBr.iY; ++startPoint.iY )
			{
			iBitmap->GetScanLine( scanLinePtr, startPoint, targetSize.iWidth, iBitmap->DisplayMode() );
			target->SetScanLine( scanLinePtr, targetY++ );
			}
	
		iBitmap->Reset();
		User::LeaveIfError( iBitmap->Duplicate( target->Handle() ) );
		CleanupStack::PopAndDestroy(2, target); // scanLine, target
		}
	}


// --------------------------------------------------------------------------
// CPbk2ImageReader::ScaleBitmapL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::ScaleBitmapL()
    {
    __ASSERT_DEBUG(iBitmap, Panic(EPanicPreCond_ScaleBitmapL));

    if ((iParams.iFlags & TPbk2ImageManagerParams::EScaleImage) && 
        !(iParams.iFlags & TPbk2ImageManagerParams::EUseFastScaling))
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

// --------------------------------------------------------------------------
// CPbk2ImageReader::Complete
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::Complete()
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

// --------------------------------------------------------------------------
// CPbk2ImageReader::InitReadL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::InitReadL(const TPbk2ImageManagerParams* aParams)
    {
    Cancel();
    if (aParams)
        {
        iParams = *aParams;
        }
    iState = EStateOpenImage;
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::CloseImage
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::CloseImage()
    {
    delete iImageDecoder;
    iImageDecoder = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::RunL
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::RunL()
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
            // Jpeg2000 decoder might need more heap than Phonebook can 
            // provide, the situation is handled so, that "Feature not 
            // supported" -note is shown if memory runs out when decoding 
            // Jpeg2000 image instead of "Out of memory" -note.
            //
            // Small jp2 images (e.g. 50*50 pixels) can be decoded, but
            // heap runs out when decoding e.g. 1 mega pixel image. 
            //
            // Increasing phonebook heap size doesn't help on this issue,
            // since jp2 decoder heap usage is increased linearly when a bigger
            // image is tried to be decoded.
            if ( status == KErrNoMemory )            
                {
                TUid imageType;
                TUid imageSubType;
                iImageDecoder->ImageType(iParams.iFrameNumber, 
                                         imageType, 
                                         imageSubType);

                if ( imageType.iUid == KImageTypeJ2KUid )
                    {
                    status = KErrNotSupported;
                    }
                }            
            iObserver.ImageReadFailed(*this, status);
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::Cancel
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::Cancel()
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

// --------------------------------------------------------------------------
// CPbk2ImageReader::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ImageReader::RunError(TInt aError)
    {
    iObserver.ImageReadFailed(*this, aError);
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ImageReader::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ImageReader::DoCancel()
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
