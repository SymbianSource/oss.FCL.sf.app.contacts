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
*       Image manipulation utilities needed in BCardEngine.    
*
*/


// INCLUDE FILES
#include "CBCardImageUtil.h"
#include <imageconversion.h>

// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
_LIT8(KJpegMimeType, "image/jpeg");

NONSHARABLE_CLASS(CBitmapToDescConverter) : public CActive
    {
    typedef void (*ConversionCallBack)(TAny* aParam, TInt aError);
    public:
        CBitmapToDescConverter(ConversionCallBack aCallBack, TAny* aParam);
        ~CBitmapToDescConverter();

        void ConvertL(CFbsBitmap& aBitmap, HBufC8*& aTargetBlob);
    private: // From CActive
	    void DoCancel();
	    void RunL();
        TInt RunError(TInt aError);

    private:
        ConversionCallBack iCallBack;
        TAny* iParam;
        CImageEncoder* iEncoder;
    };

CBitmapToDescConverter::CBitmapToDescConverter
        (ConversionCallBack aCallBack, TAny* aParam) :
    CActive(CActive::EPriorityStandard),
    iCallBack(aCallBack),
    iParam(aParam)
    {
    CActiveScheduler::Add(this);
    }

CBitmapToDescConverter::~CBitmapToDescConverter()
    {
    Cancel();
    delete iEncoder;
    }

void CBitmapToDescConverter::ConvertL(CFbsBitmap& aBitmap, HBufC8*& aTargetBlob)
    {
    Cancel();
    delete iEncoder;
    iEncoder = NULL;
    iEncoder = CImageEncoder::DataNewL(aTargetBlob, KJpegMimeType);
    iEncoder->Convert(&iStatus, aBitmap);
    SetActive();
    }

void CBitmapToDescConverter::DoCancel()
    {
    iEncoder->Cancel();
    }

void CBitmapToDescConverter::RunL()
    {
    TInt status = iStatus.Int();
    iCallBack(iParam, status);
    }

TInt CBitmapToDescConverter::RunError(TInt aError)
    {
    iCallBack(iParam, aError);
    return KErrNone;
    }

} // namespace

// ==================== MEMBER FUNCTIONS ====================

CBCardImageUtil* CBCardImageUtil::NewL()
    {
    CBCardImageUtil* self = new(ELeave) CBCardImageUtil;
    return self;
    }

CBCardImageUtil::~CBCardImageUtil()
    {
    }

HBufC8* CBCardImageUtil::BitmapToDescL(CFbsBitmap& aBitmap)
    {
    HBufC8* bitmapBlob = NULL;
    CleanupStack::PushL(bitmapBlob);

    CBitmapToDescConverter* converter = new(ELeave) CBitmapToDescConverter
        (CBCardImageUtil::ConversionDone, this);
    CleanupStack::PushL(converter);

    converter->ConvertL(aBitmap, bitmapBlob);
    if(iWait.IsStarted())
        {
        iWait.AsyncStop();
        }
    iWait.Start();
    User::LeaveIfError(iError);

    CleanupStack::PopAndDestroy(converter);
    CleanupStack::Pop(); // bitmapBlob

    return bitmapBlob;
    }

void CBCardImageUtil::ConversionDone(TAny* aThis, TInt aError)
    {
    static_cast<CBCardImageUtil*>(aThis)->ConversionDone(aError);
    }

void CBCardImageUtil::ConversionDone(TInt aError)
    {
    iError = aError;
    iWait.AsyncStop();
    iRunning = EFalse;
    }

//  End of File 
