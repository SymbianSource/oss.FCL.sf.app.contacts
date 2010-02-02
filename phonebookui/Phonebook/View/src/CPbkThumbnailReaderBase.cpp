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
#include "CPbkThumbnailReaderBase.h"
#include "CPbkImageReader.h"
#include "MPbkThumbnailOperationObservers.h"
#include "PbkImageData.h"

#include <fbs.h>
#include <imageconversion.h>

namespace {

enum TPanicCode
    {
    EPanicPreCond_CancelRead = 1,
    EPanicPreCond_StartReadL,
    EPanic_ImageReadComplete_Called,
    EPanic_ImageReadFailed_Called
    };

#ifdef _DEBUG
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CPbkThumbnailReaderBase");
    User::Panic(KPanicText, aPanicCode);
    }
#endif

} // namespace

// ================= MEMBER FUNCTIONS =======================

CPbkThumbnailReaderBase::CPbkThumbnailReaderBase
        (MPbkThumbnailGetObserver& aObserver) :
    iObserver(aObserver)
    {
    }

void CPbkThumbnailReaderBase::ConstructL()
    {
    iImageReader = CPbkImageReader::NewL(*this);
    }

CPbkThumbnailReaderBase::~CPbkThumbnailReaderBase()
    {
    delete iImageReader;
    }

void CPbkThumbnailReaderBase::CancelRead()
    {
    __ASSERT_DEBUG(iImageReader, Panic(EPanicPreCond_CancelRead));
    iImageReader->Cancel();
    }

void CPbkThumbnailReaderBase::ImageReadComplete
        (CPbkImageReader& /*aReader*/, CFbsBitmap* aBitmap)
    {
    iObserver.PbkThumbnailGetComplete(*this,aBitmap);
    }

void CPbkThumbnailReaderBase::ImageReadFailed
        (CPbkImageReader& /*aReader*/, TInt aError)
    {
    iObserver.PbkThumbnailGetFailed(*this,aError);
    }


// CPbkThumbnailImageReaderBase
CPbkThumbnailImageReaderBase::CPbkThumbnailImageReaderBase
        (MPbkThumbnailGetImageObserver& aObserver) :
    iObserver(aObserver)
    {
    }

void CPbkThumbnailImageReaderBase::ConstructL()
    {
    iImageReader = CPbkImageReader::NewL(*this);
    }

CPbkThumbnailImageReaderBase::~CPbkThumbnailImageReaderBase()
    {
    delete iImageData;
    delete iImageReader;
    }

void CPbkThumbnailImageReaderBase::StartReadL(const TPbkImageLoadParameters* aParameters)
    {
    __ASSERT_DEBUG(iImageReader, Panic(EPanicPreCond_StartReadL));
    CancelRead();
    DoStartReadL(aParameters);
    }

void CPbkThumbnailImageReaderBase::CancelRead()
    {
    __ASSERT_DEBUG(iImageReader, Panic(EPanicPreCond_CancelRead));
    iImageReader->Cancel();
    delete iImageData;
    iImageData = NULL;
    }

void CPbkThumbnailImageReaderBase::ImageReadComplete
        (CPbkImageReader& /*aReader*/, CFbsBitmap* aBitmap)
    {
    // this does not get called because reading is canceled 
    // when opening completes
    __ASSERT_DEBUG(EFalse, Panic(EPanic_ImageReadComplete_Called));
    delete aBitmap;
    }

void CPbkThumbnailImageReaderBase::ImageReadFailed
        (CPbkImageReader& /*aReader*/, TInt aError)
    {
    // this does not get called because reading is canceled 
    // when opening completes
    __ASSERT_DEBUG(EFalse, Panic(EPanic_ImageReadFailed_Called));
    iObserver.PbkThumbnailGetImageFailed(*this,aError);
    }

void CPbkThumbnailImageReaderBase::ImageOpenComplete
        (CPbkImageReader& /*aReader*/)
    {
    TRAPD(err, HandleImageOpenCompleteL());
    // Cancel further reading of image
    iImageReader->Cancel();
    if (err)
        {
        iObserver.PbkThumbnailGetImageFailed(*this,err);
        }
    else
        {
        // Release ownership of iImageData
        CPbkImageDataWithInfo* imageData = iImageData;
        iImageData = NULL;
        iObserver.PbkThumbnailGetImageComplete(*this,imageData);
        }
    }

void CPbkThumbnailImageReaderBase::HandleImageOpenCompleteL()
    {
    MPbkImageData* imageData = CreateImageDataLC();

    CPbkImageDataWithInfo::TParams params;
    params.iImageData = imageData;
    params.iMimeType = &iImageReader->MimeString();
    if (iImageReader->FrameCount() > 0)
        {
        TFrameInfo frameInfo;
        iImageReader->FrameInfo(0,frameInfo);
        params.iSizeInPixels = frameInfo.iOverallSizeInPixels;
        }

    iImageData = CPbkImageDataWithInfo::NewL(params);
    CleanupStack::Pop();  // imageData
    }

//  End of File
