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
*         Handling thumbnail save/retrieve operations and making these
*         operations synchronous
*
*/


// INCLUDE FILES
#include    "CBCardThumbnailHandler.h"
#include    <fbs.h>
#include    <CPbkThumbnailManager.h>
#include    <PbkImageData.h>
#include    <TPbkImageLoadParameters.h>
#include    "CBCardImageUtil.h"


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
// Default size for thumbnail images
#define KDefaultThumbnailSize TSize(150,150)
/// JPEG file mime type
_LIT8(KJpegMimeType, "image/jpeg");

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ProcessComplete = 1,
    EPanicPreCond_ProcessError,
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CBCardThumbnailHandler");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

} // namespace

// ==================== MEMBER FUNCTIONS ====================


CBCardThumbnailHandler* CBCardThumbnailHandler::NewL(CPbkContactEngine& aEngine)
    {
	CBCardThumbnailHandler* self = new (ELeave) CBCardThumbnailHandler();
	CleanupStack::PushL(self);
	self->ConstructL(aEngine);
	CleanupStack::Pop(self);
	return self;
    }

CBCardThumbnailHandler::CBCardThumbnailHandler()
    {
    }

CBCardThumbnailHandler::~CBCardThumbnailHandler()
    {
    delete iImageBlob;
    delete iBitmap;
    delete iImageData;
    delete iThumbOperation;
    delete iManager;
    }

void CBCardThumbnailHandler::ConstructL(CPbkContactEngine& aEngine)
    {
    iManager = CPbkThumbnailManager::NewL(aEngine); 
    }

void CBCardThumbnailHandler::SaveThumbnailL(
        CPbkContactItem& aContactItem,const TDesC8& aSource)
    {
    iError = KErrNone;
    delete iThumbOperation; 
    iThumbOperation = NULL;
   
    iThumbOperation = iManager->SetThumbnailAsyncL(*this, aContactItem, aSource);    
    if(iWait.IsStarted())
        {
        iWait.AsyncStop();
        }
    iWait.Start();
	User::LeaveIfError(iError);
    }

TBool CBCardThumbnailHandler::HasThumbnail(
        const CPbkContactItem& aContactItem) const
    {
    return iManager->HasThumbnail(aContactItem);
    }

const TDesC8& CBCardThumbnailHandler::GetThumbnailL(CPbkContactItem& aContactItem)
    {
    if (IsAlreadyExportableL(aContactItem))
        {
        const TDesC8& imageBuf = iImageData->ImageData().GetBufferL();        
        return imageBuf; 
        }
    else
        {
        delete iImageData;
        iImageData = NULL;

        delete iImageBlob;
        iImageBlob = NULL;
        iImageBlob = GetThumbnailBitmapL(aContactItem);
        
        return *iImageBlob;
        }
    }

TBool CBCardThumbnailHandler::IsAlreadyExportableL(CPbkContactItem& aContactItem)
    {
    iError = KErrNone;
    delete iThumbOperation; 
    iThumbOperation = NULL;
        
    iThumbOperation = iManager->GetThumbnailImageAsyncL(*this, aContactItem);
    if(iWait.IsStarted())
        {
        iWait.AsyncStop();
        }
    iWait.Start();
    User::LeaveIfError(iError);
    
    // Image must be in jpeg format
    if (iImageData->MimeType() != KJpegMimeType)
        {
        return EFalse;
        }

    // Image must be of size 80x96
    return (iImageData->ImageSizeInPixels() == KDefaultThumbnailSize);
    }


HBufC8* CBCardThumbnailHandler::GetThumbnailBitmapL(CPbkContactItem& aContactItem)
    {
    iError = KErrNone;
    delete iThumbOperation; 
    iThumbOperation = NULL;


    // Define parameters for thumbnail
    TPbkImageLoadParameters params;
    params.iSize = KDefaultThumbnailSize;
    params.iFlags = TPbkImageLoadParameters::EScaleImage | 
                    TPbkImageLoadParameters::EKeepAspectRatio;

    iThumbOperation = iManager->GetThumbnailAsyncL(*this, aContactItem ,params);
    if(iWait.IsStarted())
        {
        iWait.AsyncStop();
        }
    iWait.Start();
    User::LeaveIfError(iError);
    
    CBCardImageUtil* util = CBCardImageUtil::NewL();
    CleanupStack::PushL(util);
    
    if (!iBitmap)
        {
        User::Leave(KErrNotFound);
        }

    
    // convert bitmap to jpeg descriptor
    HBufC8* imageBlob = util->BitmapToDescL(*iBitmap);

    delete iBitmap;
    iBitmap = NULL;

    CleanupStack::PopAndDestroy(util);
    
    return imageBlob;
    }

void CBCardThumbnailHandler::PbkThumbnailGetImageComplete(
        MPbkThumbnailOperation& aOperation, CPbkImageDataWithInfo* aImageData)
    {    
    ProcessComplete(aOperation);
        
    delete iImageData; 
    iImageData = aImageData;    
    }

void CBCardThumbnailHandler::PbkThumbnailGetImageFailed(
        MPbkThumbnailOperation& aOperation, TInt aError)
    {
    ProcessError(aOperation, aError);
    }

void CBCardThumbnailHandler::PbkThumbnailGetComplete(
        MPbkThumbnailOperation& aOperation, CFbsBitmap* aBitmap)
    {
    ProcessComplete(aOperation);
        
    delete iBitmap;        
    iBitmap = aBitmap;
    }

void CBCardThumbnailHandler::PbkThumbnailGetFailed(
        MPbkThumbnailOperation& aOperation, TInt aError)
    {
    ProcessError(aOperation, aError);
    }

void CBCardThumbnailHandler::PbkThumbnailSetComplete(
        MPbkThumbnailOperation& aOperation)    
    {
    ProcessComplete(aOperation);
    }

void CBCardThumbnailHandler::PbkThumbnailSetFailed(
        MPbkThumbnailOperation& aOperation, TInt aError)
    {
    ProcessError(aOperation, aError);
    }

void CBCardThumbnailHandler::ProcessError(
    MPbkThumbnailOperation& 
    #ifdef _DEBUG 
    aOperation
    #endif
    , TInt aError)
    {
    __ASSERT_DEBUG(iThumbOperation && iThumbOperation == &aOperation, 
        Panic(EPanicPreCond_ProcessError));

    iWait.AsyncStop();
    iError = aError;            
    }

void CBCardThumbnailHandler::ProcessComplete(
    MPbkThumbnailOperation& 
    #ifdef _DEBUG 
    aOperation
    #endif
    )
    {
    __ASSERT_DEBUG(iThumbOperation && iThumbOperation == &aOperation, 
        Panic(EPanicPreCond_ProcessComplete));

    iWait.AsyncStop();
    iError = KErrNone;            
    }

//  End of File 
