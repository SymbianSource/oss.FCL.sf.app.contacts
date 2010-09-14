/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Methods for Phonebook2 contact item field embedded image reader.
*
*/


// INCLUDE FILES

#include "Pbk2PresentationUtils.h"
#include "CPbk2EmbeddedImage.h"
#include "CPbk2ImageReader.h"
#include "MPbk2ImageOperationObservers.h"
#include "MVPbkBaseContactField.h"
#include "MVPbkStoreContactField.h"
#include "MVPbkContactFieldBinaryData.h"
#include "CVPbkContactManager.h"
#include "TVPbkFieldTypeMapping.h"
#include "MVPbkFieldType.h"
#include "CPbk2ImageData.h"

#include <e32std.h>
#include <eikappui.h>
#include <imageconversion.h>
#include <aknappui.h>   // CEikAppUi
#include <AknUtils.h>   // TAknLayoutRect
#include <barsread.h>   // TResourceReader
#include <RPbk2LocalizedResourceFile.h>
#include "Pbk2DataCaging.hrh"

#include <pbk2presentation.rsg>

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
#endif // _DEBUG


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CPbkEmbeddedImage");
    User::Panic(KPanicText, aPanicCode);
    }
#endif // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================
// --------------------------------------------------------------------------
// CPbk2EmbeddedImageReader::CPbk2EmbeddedImageReader
// --------------------------------------------------------------------------
//
inline CPbk2EmbeddedImageReader::CPbk2EmbeddedImageReader
        (const MVPbkBaseContactField& aField, 
        MPbk2ImageGetObserver& aObserver) :
    CPbk2ImageReaderBase(aObserver),
    iField(aField)
    {
    __ASSERT_DEBUG(iField.FieldData().DataType()==EVPbkFieldStorageTypeBinary, 
        Panic(EPanicReadPreCond_Constructor));
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageReader::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2EmbeddedImageReader::ConstructL()
    {
    CPbk2ImageReaderBase::ConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageReader::NewLC
// --------------------------------------------------------------------------
//
CPbk2EmbeddedImageReader* CPbk2EmbeddedImageReader::NewLC
        (const MVPbkBaseContactField& aField,
        MPbk2ImageGetObserver& aObserver)
    {
    CPbk2EmbeddedImageReader* self = 
        new(ELeave) CPbk2EmbeddedImageReader(aField,aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageReader::~CPbk2EmbeddedImageReader
// --------------------------------------------------------------------------
//
CPbk2EmbeddedImageReader::~CPbk2EmbeddedImageReader()
    {
    delete iImageBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageReader::StartReadL
// --------------------------------------------------------------------------
//
void CPbk2EmbeddedImageReader::StartReadL(
        const TPbk2ImageManagerParams* aParameters)
    {
    if (iImageBuffer)
        {
        delete iImageBuffer;
        iImageBuffer = NULL;
        }
    	
    iImageBuffer = 
    	MVPbkContactFieldBinaryData::Cast(iField.FieldData()).BinaryData().AllocL();
    	
    __ASSERT_DEBUG(iImageBuffer && iImageBuffer->Length()>0, 
        Panic(EPanicReadPreCond_StartReadL));
    CancelRead();
    
    if (iImageBuffer)
        {
        ImageReader().ReadFromBufferL(*iImageBuffer, aParameters);
        }
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageReader::ImageBuffer
// --------------------------------------------------------------------------
//    
const TDesC8& CPbk2EmbeddedImageReader::ImageBuffer() const    
	{
	if (iImageBuffer)
		{
		return *iImageBuffer;		
		}
	return KNullDesC8;
	}

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::CPbk2EmbeddedImageWriter
// --------------------------------------------------------------------------
//
inline CPbk2EmbeddedImageWriter::CPbk2EmbeddedImageWriter
        (CVPbkContactManager& aContactManager,
        MVPbkStoreContactField& aField, 
        MPbk2ImageSetObserver& aObserver) :
    CPbk2ImageWriterBase(aObserver), 
    iContactManager(aContactManager), 
    iField(aField)   
    {
    __ASSERT_DEBUG(iField.FieldData().DataType()==EVPbkFieldStorageTypeBinary, 
        Panic(EPanicWritePreCond_Constructor));
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2EmbeddedImageWriter::ConstructL()
    {
    CPbk2ImageWriterBase::ConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::NewLC
// --------------------------------------------------------------------------
//
CPbk2EmbeddedImageWriter* CPbk2EmbeddedImageWriter::NewLC
        (CVPbkContactManager& aContactManager,
        MVPbkStoreContactField& aField, 
        MPbk2ImageSetObserver& aObserver)
    {
    CPbk2EmbeddedImageWriter* self = 
        new(ELeave) CPbk2EmbeddedImageWriter(
            aContactManager, aField, aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::~CPbk2EmbeddedImageWriter
// --------------------------------------------------------------------------
//
CPbk2EmbeddedImageWriter::~CPbk2EmbeddedImageWriter()
    {
    delete iBitmapBlob;
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::CreateImageWriterL
// --------------------------------------------------------------------------
//
CImageEncoder* CPbk2EmbeddedImageWriter::CreateImageWriterL()
    {
    __ASSERT_DEBUG(!iBitmapBlob, 
        Panic(EPanicWritePreCond_CreateImageWriterL));

    return CImageEncoder::DataNewL
        (iBitmapBlob, CImageEncoder::EOptionNone, KImageTypeJPGUid);
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::StoreImageL
// --------------------------------------------------------------------------
//
void CPbk2EmbeddedImageWriter::StoreImageL(const MPbk2ImageData& aImageData)
    {
    MVPbkContactFieldBinaryData::Cast(iField.FieldData())
    	.SetBinaryDataL(aImageData.GetBufferL());
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::StoreImageL
// --------------------------------------------------------------------------
//    
void CPbk2EmbeddedImageWriter::StoreImageL
        (CImageEncoder& /*aImageWriter*/)
    {
    __ASSERT_DEBUG(iBitmapBlob, 
        Panic(EPanicWritePreCond_StoreImageL_CMdaImageDataWriteUtility));
    MVPbkContactFieldBinaryData::Cast(iField.FieldData())
    	.SetBinaryDataL(*iBitmapBlob);    
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2EmbeddedImageWriter::DoCancel()
    {
    delete iBitmapBlob;
    iBitmapBlob = NULL;

    // Let base class do its own canceling
    CPbk2ImageWriterBase::DoCancel();
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::ImageSize
// --------------------------------------------------------------------------
//
TSize CPbk2EmbeddedImageWriter::ImageSize() const
    {
    TBool isCodImage (EFalse);
    TRAP_IGNORE(isCodImage = IsCodImageL());
    	
    if (isCodImage)
    	{
	    return (static_cast<CEikAppUi*>(CCoeEnv::Static()->AppUi()))->
            ApplicationRect().Size();   
    	}
    else 
    	{
	    return KPbk2PersonalImageSize;    	
    	}
    }

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::IsCodImageL
// --------------------------------------------------------------------------
//    
TBool CPbk2EmbeddedImageWriter::IsCodImageL() const
	{
    // If iField's type is 
    // 		EVPbkNonVersitCodImage return ETrue otherwise EFalse.
    
    const MVPbkFieldTypeList& masterFieldTypeList = 
    	iContactManager.FieldTypes();
	RPbk2LocalizedResourceFile resFile(*CCoeEnv::Static());
    resFile.OpenLC(KPbk2RomFileDrive, 
                   KDC_RESOURCE_FILES_DIR,
                   Pbk2PresentationUtils::PresentationResourceFile());
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(reader, R_CODIMAGE_FIELD_TYPE);
    const TInt count = reader.ReadInt16();
    TBool found(EFalse);
    for (TInt i = 0; i < count && !found; ++i)
        {
        TVPbkFieldTypeMapping mapping(reader);
        const MVPbkFieldType* fieldType = 
            mapping.FindMatch(masterFieldTypeList);
        if (!fieldType)
            {
            User::Leave(KErrNotFound);
            }
        if (IsSameFieldType(*fieldType))
        	{
        	found = ETrue;
        	}
        }
    CleanupStack::PopAndDestroy(2); // resFile, CreateResourceReaderLC
    
	return found;	
	}

// --------------------------------------------------------------------------
// CPbk2EmbeddedImageWriter::IsSameFieldType
// --------------------------------------------------------------------------
//	
TBool CPbk2EmbeddedImageWriter::IsSameFieldType(
        const MVPbkFieldType& aFieldType ) const
	{
    TInt maxMatchPriority(iContactManager.FieldTypes().MaxMatchPriority());
    for ( TInt matchPriority = 0; 
    	  matchPriority <= maxMatchPriority; 
    	  ++matchPriority )
        {            
        const MVPbkFieldType* fieldType = 
            iField.MatchFieldType(matchPriority);
        if (fieldType && fieldType->IsSame(aFieldType))
            {
            return ETrue;
            }
        }	
    return EFalse;
    }

//  End of File  
