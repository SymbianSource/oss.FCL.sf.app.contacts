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
*
*
*/


#include "CPbkThumbnailScaler.h"

#include <bitmaptransforms.h>
#include <applayout.cdl.h>
#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>

CPbkThumbnailScaler* CPbkThumbnailScaler::NewL
        (MPbkThumbnailScalerObserver& aObserver)
    {
    return new(ELeave) CPbkThumbnailScaler(aObserver);
    }

CPbkThumbnailScaler::CPbkThumbnailScaler
        (MPbkThumbnailScalerObserver& aObserver) :
    CActive(CActive::EPriorityStandard),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

CPbkThumbnailScaler::~CPbkThumbnailScaler()
    {
    Cancel();
    delete iScaler;
    delete iBitmap;
    }


void CPbkThumbnailScaler::StartScaleL(CFbsBitmap* aBitmap, const TSize& aSize)
    {
    if (!iScaler)
        {
        iScaler = CBitmapScaler::NewL();
        }

    iBitmap = aBitmap;

    iScaler->Scale(&iStatus, *iBitmap, aSize);
    SetActive();
    }

void CPbkThumbnailScaler::CreateThumbnail(CFbsBitmap* aBitmap)
    {
    Cancel();
    
    TAknWindowComponentLayout compomentThumbnail = 
        AknLayoutScalable_Apps::popup_phob_thumbnail_window_g1();

    TAknWindowLineLayout lineThumbnail = compomentThumbnail.LayoutLine();
    TAknLayoutRect aknRectThumbnail;
    aknRectThumbnail.LayoutRect( TRect(), compomentThumbnail );
    TRect rectThumbnail = aknRectThumbnail.Rect();
    
    TSize targetSize = rectThumbnail.Size();

    TRAPD(err, StartScaleL(aBitmap, targetSize));
    if (err != KErrNone)
        {
        iObserver.ThumbnailScalingComplete(err, aBitmap);
        }
    }

void CPbkThumbnailScaler::RunL()
    {
    TInt status = iStatus.Int();
    iObserver.ThumbnailScalingComplete(status, iBitmap);
    iBitmap = NULL;
    }

void CPbkThumbnailScaler::DoCancel()
    {
    iScaler->Cancel();
    iObserver.ThumbnailScalingComplete(KErrCancel, iBitmap);
    iBitmap = NULL;
    }

TInt CPbkThumbnailScaler::RunError(TInt aError)
    {
    iObserver.ThumbnailScalingComplete(aError, iBitmap);
    iBitmap = NULL;
    return KErrNone;
    }

// End of File
