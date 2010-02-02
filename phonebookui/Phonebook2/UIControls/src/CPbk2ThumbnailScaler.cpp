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
*
*
*/


#include "CPbk2ThumbnailScaler.h"

#include <bitmaptransforms.h>
#include <applayout.cdl.h>
#include <AknUtils.h>

// Sizes for different original images from LAF
#define KVgaSize        TSize(640, 480)
#define KVgaTurnedSize  TSize(480, 640)
#define KCifSize        TSize(352, 288)
#define KCifTurnedSize  TSize(288, 352)
#define KCommSize       TSize(80,  96)
const TReal KTallThumbnailRatio = 0.625;
const TReal KWideThumbnailRatio = 1.467;

CPbk2ThumbnailScaler* CPbk2ThumbnailScaler::NewL
        (MPbk2ThumbnailScalerObserver& aObserver)
    {
    return new(ELeave) CPbk2ThumbnailScaler(aObserver);
    }

CPbk2ThumbnailScaler::CPbk2ThumbnailScaler
        (MPbk2ThumbnailScalerObserver& aObserver) :
    CActive(CActive::EPriorityStandard),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

CPbk2ThumbnailScaler::~CPbk2ThumbnailScaler()
    {
    Cancel();
    delete iScaler;
    delete iBitmap;
    }

void CPbk2ThumbnailScaler::DoCropL(CFbsBitmap* aBitmap)
    {
    TSize size = aBitmap->SizeInPixels();
    TReal ratio = TReal(size.iWidth) / TReal(size.iHeight);

    TSize targetSize;

    if (ratio < KTallThumbnailRatio)
        {
        targetSize.SetSize
            (size.iWidth, TInt(size.iWidth / KTallThumbnailRatio));
        }
    else if (ratio > KWideThumbnailRatio)
        {
        targetSize.SetSize
            (TInt(size.iHeight * KWideThumbnailRatio), size.iHeight);
        }
    else
        {
        // no cropping needed
        return;
        }

    TRect targetRect(TPoint((size.iWidth - targetSize.iWidth) / 2,
                            (size.iHeight - targetSize.iHeight) / 2),
                     targetSize);

    CFbsBitmap* target = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(target);
    User::LeaveIfError(target->Create(targetSize, aBitmap->DisplayMode()));
    
    HBufC8* scanLine = HBufC8::NewLC(aBitmap->ScanLineLength
        (targetSize.iWidth, aBitmap->DisplayMode()));
    TPtr8 scanLinePtr = scanLine->Des();
    
    TPoint startPoint(targetRect.iTl.iX, targetRect.iTl.iY);
    TInt targetY = 0;
    for (; startPoint.iY < targetRect.iBr.iY; ++startPoint.iY)
        {
        aBitmap->GetScanLine(scanLinePtr, startPoint, targetSize.iWidth, aBitmap->DisplayMode());
        target->SetScanLine(scanLinePtr, targetY++);
        }

    aBitmap->Reset();
    User::LeaveIfError(aBitmap->Duplicate(target->Handle()));
    CleanupStack::PopAndDestroy(2, target); // scanLine, target
    }

void CPbk2ThumbnailScaler::StartScaleL(CFbsBitmap* aBitmap, const TSize& aSize)
    {
    // Crops the image if needed
    DoCropL(aBitmap);

    if (!iScaler)
        {
        iScaler = CBitmapScaler::NewL();
        }

    iBitmap = aBitmap;

    iScaler->Scale(&iStatus, *iBitmap, aSize);
    SetActive();
    }

void CPbk2ThumbnailScaler::CreateThumbnail(CFbsBitmap* aBitmap)
    {
    Cancel();
    
    TSize size = aBitmap->SizeInPixels();
    TReal ratio = TReal(size.iWidth) / TReal(size.iHeight);

    TSize targetSize;
    TRect zeroRect(0, 0, 0, 0);
    if (size == KVgaSize)
        {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(zeroRect, AppLayout::VGA());
        targetSize = TSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());
        }
    else if (size == KVgaTurnedSize)
        {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(zeroRect, AppLayout::VGA_turned_90());
        targetSize = TSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());
        }
    else if (size == KCifSize)
        {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(zeroRect, AppLayout::CIF());
        targetSize = TSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());
        }
    else if (size == KCifTurnedSize)
        {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(zeroRect, AppLayout::CIF_turned_90());
        targetSize = TSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());
        }
    else if (size == KCommSize)
        {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(zeroRect, AppLayout::Communicator_personal_image());
        targetSize = TSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());
        }
    else if (ratio < KTallThumbnailRatio)
        {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(zeroRect, AppLayout::Image_aspect_ratio___0_625());
        targetSize = TSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());
        }
    else if (ratio > KWideThumbnailRatio)
        {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(zeroRect, AppLayout::Image_aspect_ratio___1_467());
        targetSize = TSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());
        }
    else // KTallThumbnailRatio < ratio < KWideThumbnailRatio
        {
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(zeroRect, AppLayout::_0_625___image_aspect_ratio___1_467());
        targetSize = TSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());
        }

    TRAPD(err, StartScaleL(aBitmap, targetSize));
    if (err != KErrNone)
        {
        iObserver.ThumbnailScalingComplete(err, aBitmap);
        }
    }

void CPbk2ThumbnailScaler::RunL()
    {
    TInt status = iStatus.Int();
    iObserver.ThumbnailScalingComplete(status, iBitmap);
    iBitmap = NULL;
    }

void CPbk2ThumbnailScaler::DoCancel()
    {
    iScaler->Cancel();
    iObserver.ThumbnailScalingComplete(KErrCancel, iBitmap);
    iBitmap = NULL;
    }

TInt CPbk2ThumbnailScaler::RunError(TInt aError)
    {
    iObserver.ThumbnailScalingComplete(aError, iBitmap);
    iBitmap = NULL;
    return KErrNone;
    }

// End of File
