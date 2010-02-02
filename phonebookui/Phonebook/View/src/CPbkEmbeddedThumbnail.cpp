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
*       Methods for Phonebook contact item field embedded thumbnail reader.
*
*/


// INCLUDE FILES

#include "CPbkEmbeddedThumbnail.h"
#include "CPbkImageReader.h"
#include "MPbkThumbnailOperationObservers.h"
#include "PbkImageData.h"
#include <CPbkFFSCheck.h>

#include <e32std.h>
#include <eikappui.h>
#include <TPbkContactItemField.h>
#include <imageconversion.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicReadPreCond_Constructor = 1,
    EPanicReadPreCond_StartReadL,

    EPanicWritePreCond_Constructor = 100,
    EPanicWritePreCond_CreateImageWriterL,
    EPanicWritePreCond_StoreImageL_CMdaImageDataWriteUtility
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CPbkEmbeddedThumbnail");
    User::Panic(KPanicText, aPanicCode);
    }
#endif

// When thumbnail image is saved to contactdatabase then during saving
// some extra disk space is consumed besides raw thumbnail data.
// Following const is approximation for this needed extra space.
const TInt KHdOverhead = 1024;

// Gives approximation for saved thumbnail size on the disk
inline TInt ThumbnailSizeOnDisk(TInt aBytes)
    {
    return aBytes+KHdOverhead;
    }

}  // namespace


// ================= MEMBER FUNCTIONS =======================

/////////////////////////////////////////////////////////////////
// CPbkEmbeddedThumbnailReader 

inline CPbkEmbeddedThumbnailReader::CPbkEmbeddedThumbnailReader
        (const TPbkContactItemField& aField, 
        MPbkThumbnailGetObserver& aObserver) :
    CPbkThumbnailReaderBase(aObserver),
    iField(aField)
    {
    __ASSERT_DEBUG(iField.StorageType()==KStorageTypeStore, 
        Panic(EPanicReadPreCond_Constructor));
    }

inline void CPbkEmbeddedThumbnailReader::ConstructL()
    {
    CPbkThumbnailReaderBase::ConstructL();    
    }

CPbkEmbeddedThumbnailReader* CPbkEmbeddedThumbnailReader::NewLC
        (const TPbkContactItemField& aField,
        MPbkThumbnailGetObserver& aObserver)
    {
    CPbkEmbeddedThumbnailReader* self = 
        new(ELeave) CPbkEmbeddedThumbnailReader(aField,aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CPbkEmbeddedThumbnailReader::~CPbkEmbeddedThumbnailReader()
    {
    }

void CPbkEmbeddedThumbnailReader::StartReadL(const TPbkImageLoadParameters* aParameters)
    {
    HBufC8* imageBuf = iField.ContactItemField().StoreStorage()->Thing();
    __ASSERT_DEBUG(imageBuf && imageBuf->Length()>0, 
        Panic(EPanicReadPreCond_StartReadL));
    CancelRead();
	if (imageBuf)
		{
		ImageReader().ReadFromBufferL(*imageBuf, aParameters);
		}
    }

/////////////////////////////////////////////////////////////////
// CPbkEmbeddedThumbnailImageReader
inline CPbkEmbeddedThumbnailImageReader::CPbkEmbeddedThumbnailImageReader
        (const TPbkContactItemField& aField, 
        MPbkThumbnailGetImageObserver& aObserver) :
    CPbkThumbnailImageReaderBase(aObserver),
    iField(aField)
    {
    __ASSERT_DEBUG(iField.StorageType()==KStorageTypeStore && !iField.IsEmpty(),
        Panic(EPanicReadPreCond_Constructor));
    }

CPbkEmbeddedThumbnailImageReader* CPbkEmbeddedThumbnailImageReader::NewLC
        (const TPbkContactItemField& aField, 
        MPbkThumbnailGetImageObserver& aObserver)
    {
    CPbkEmbeddedThumbnailImageReader* self = 
        new(ELeave) CPbkEmbeddedThumbnailImageReader(aField,aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CPbkEmbeddedThumbnailImageReader::ConstructL()
    {
    CPbkThumbnailImageReaderBase::ConstructL();
    }

CPbkEmbeddedThumbnailImageReader::~CPbkEmbeddedThumbnailImageReader()
    {
    }

void CPbkEmbeddedThumbnailImageReader::DoStartReadL(const TPbkImageLoadParameters* aParameters)
    {
    const TDesC8& imageBuffer = ImageBuffer();
    ImageReader().RecognizeFormatFromBufferL(imageBuffer);
    ImageReader().ReadFromBufferL(imageBuffer, aParameters);
    }

MPbkImageData* CPbkEmbeddedThumbnailImageReader::CreateImageDataLC()
    {
    CPbkImageBufferData* imageData = CPbkImageBufferData::NewL(ImageBuffer());
    CleanupStack::PushL(imageData);
    return imageData;
    }

const TDesC8& CPbkEmbeddedThumbnailImageReader::ImageBuffer() const
    {
    return *(iField.ContactItemField().StoreStorage()->Thing());
    }


/////////////////////////////////////////////////////////////////
// CPbkEmbeddedThumbnailWriter

inline CPbkEmbeddedThumbnailWriter::CPbkEmbeddedThumbnailWriter
        (TPbkContactItemField& aField, 
        MPbkThumbnailSetObserver& aObserver) :
    CPbkThumbnailWriterBase(aObserver), iField(aField)   
    {
    __ASSERT_DEBUG(iField.StorageType()==KStorageTypeStore, 
        Panic(EPanicWritePreCond_Constructor));
    }

inline void CPbkEmbeddedThumbnailWriter::ConstructL()
    {
    CPbkThumbnailWriterBase::ConstructL();
    iFFSCheck=CPbkFFSCheck::NewL();
    }

CPbkEmbeddedThumbnailWriter* CPbkEmbeddedThumbnailWriter::NewLC
        (TPbkContactItemField& aField, 
        MPbkThumbnailSetObserver& aObserver)
    {
    CPbkEmbeddedThumbnailWriter* self = 
        new(ELeave) CPbkEmbeddedThumbnailWriter(aField, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CPbkEmbeddedThumbnailWriter::~CPbkEmbeddedThumbnailWriter()
    {
    delete iBitmapBlob;
    delete iFFSCheck;
    }

CImageEncoder* CPbkEmbeddedThumbnailWriter::CreateImageWriterL()
    {
    __ASSERT_DEBUG(!iBitmapBlob, Panic(EPanicWritePreCond_CreateImageWriterL));

    return CImageEncoder::DataNewL
        (iBitmapBlob, CImageEncoder::EOptionNone, KImageTypeJPGUid);
    }

void CPbkEmbeddedThumbnailWriter::StoreImageL(const MPbkImageData& aImageData)
    {
    const TDesC8& imageBuffer = aImageData.GetBufferL();
    if(iFFSCheck->FFSClCheckL(ThumbnailSizeOnDisk(imageBuffer.Length())))
        {
        iField.ContactItemField().StoreStorage()->SetThingL(imageBuffer);
        }    
    }

void CPbkEmbeddedThumbnailWriter::StoreImageL
        (CImageEncoder& /*aImageWriter*/)
    {
    __ASSERT_DEBUG(iBitmapBlob, 
        Panic(EPanicWritePreCond_StoreImageL_CMdaImageDataWriteUtility));
    if(iFFSCheck->FFSClCheckL(ThumbnailSizeOnDisk(iBitmapBlob->Length())))
        {
        iField.ContactItemField().StoreStorage()->SetThingL(*iBitmapBlob);        
        }
    }

void CPbkEmbeddedThumbnailWriter::DoCancel()
    {
    delete iBitmapBlob;
    iBitmapBlob = NULL;

    // Let base class do its own canceling
    CPbkThumbnailWriterBase::DoCancel();
    }

TSize CPbkEmbeddedThumbnailWriter::ImageSize() const
    {
    if (EPbkFieldIdCodImageID==iField.PbkFieldId())
        {
        return (static_cast<CEikAppUi*>(CCoeEnv::Static()->AppUi()))->
            ApplicationRect().Size();
        }
    else 
        {
        //Default: EPbkFieldIdThumbnailImage
        return KPbkPersonalImageSize;
        }
    }

//  End of File  
