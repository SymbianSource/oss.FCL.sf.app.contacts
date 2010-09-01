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
*       Provides methods for the Image data abstraction hierarchy.
*
*/


// INCLUDE FILES
#include "PbkImageData.h"
#include <f32file.h>
#include <CPbkAttachmentFile.h>


// ================= MEMBER FUNCTIONS =======================

// CPbkImageFileData
inline CPbkImageFileData::CPbkImageFileData(const TDesC& aFileName)
    {
    iFileName.Copy(aFileName.Left(iFileName.MaxLength()));
    }

CPbkImageFileData* CPbkImageFileData::NewL(const TDesC& aFileName)
    {
    CPbkImageFileData* self = new(ELeave) CPbkImageFileData(aFileName);
    return self;
    }

CPbkImageFileData::~CPbkImageFileData()
    {
    delete iBuffer;
    }

const TDesC8& CPbkImageFileData::GetBufferL() const
    {
    if (!iBuffer)
        {
        RFs fsSession;
        User::LeaveIfError(fsSession.Connect());
        CleanupClosePushL(fsSession);
        RFile file;
        User::LeaveIfError(file.Open(fsSession, iFileName, EFileRead|EFileShareReadersOnly));
        CleanupClosePushL(file);
        TInt fileSize;
        User::LeaveIfError(file.Size(fileSize));
        HBufC8* buf = HBufC8::NewLC(fileSize);
        TPtr8 bufPtr(buf->Des());
        User::LeaveIfError(file.Read(bufPtr));
        CleanupStack::Pop(buf);
        iBuffer = buf;
        CleanupStack::PopAndDestroy(2);  // file, fsSession
        }
    return *iBuffer;
    }

const TDesC&  CPbkImageFileData::GetFileNameL() const
    {
    return iFileName;
    }

// CPbkImageBufferData
inline CPbkImageBufferData::CPbkImageBufferData
        (const TDesC8& aBuffer, const TDesC& aBaseName) :
    iBuffer(aBuffer)
    {
    iBaseName.Copy(aBaseName.Left(iBaseName.MaxLength()));
    }

CPbkImageBufferData* CPbkImageBufferData::NewL
        (const TDesC8& aBuffer, const TDesC& aBaseName/*=KNullDesC*/)
    {
    CPbkImageBufferData* self = 
        new(ELeave) CPbkImageBufferData(aBuffer,aBaseName);
    return self;
    }

CPbkImageBufferData::~CPbkImageBufferData()
    {
    delete iFile;
    iFsSession.Close();
    }

const TDesC8& CPbkImageBufferData::GetBufferL() const
    {
    return iBuffer;
    }

const TDesC&  CPbkImageBufferData::GetFileNameL() const
    {
    if (!iFile)
        {
        if (iFsSession.Handle() == KNullHandle)
            {
            User::LeaveIfError(iFsSession.Connect());
            }
        CPbkAttachmentFile* imageFile = CPbkAttachmentFile::NewLC(iBaseName,iFsSession);
        RFile& file = imageFile->File();
        User::LeaveIfError(file.Write(iBuffer));
        User::LeaveIfError(file.Flush());
        file.Close();
        CleanupStack::Pop(imageFile);
        iFile = imageFile;
        }
    return iFile->FileName();
    }

// CPbkImageDataWithInfo
EXPORT_C CPbkImageDataWithInfo::TParams::TParams() :
    iImageData(NULL),
    iSizeInPixels(0,0),
    iMimeType(NULL)
    {
    }

inline CPbkImageDataWithInfo::CPbkImageDataWithInfo()
    {
    }
            
inline void CPbkImageDataWithInfo::ConstructL(const TParams& aParams)
    {
    iParams.iSizeInPixels = aParams.iSizeInPixels;
    if (aParams.iMimeType)
        {
        iParams.iMimeType = aParams.iMimeType->AllocL();
        }
    // Take ownership of iImageData after all leaving operations so that this
    // class operates consistently (==does not take ownership of aParams.iImageData) 
    // if NewL leaves.
    iParams.iImageData = aParams.iImageData;
    }

EXPORT_C CPbkImageDataWithInfo* CPbkImageDataWithInfo::NewL(const TParams& aParams)
    {
    CPbkImageDataWithInfo* self = new(ELeave) CPbkImageDataWithInfo;
    CleanupStack::PushL(self);
    self->ConstructL(aParams);
    CleanupStack::Pop(self);
    return self;
    }

CPbkImageDataWithInfo::~CPbkImageDataWithInfo()
    {
    // ConstructL puts a HBufC8* to iParams.iMimeType
    delete ((HBufC8*)(iParams.iMimeType));
    delete iParams.iImageData;
    }

EXPORT_C const MPbkImageData& CPbkImageDataWithInfo::ImageData() const
    {
    return *iParams.iImageData;
    }

EXPORT_C TSize CPbkImageDataWithInfo::ImageSizeInPixels() const
    {
    return iParams.iSizeInPixels;
    }

EXPORT_C const TDesC8& CPbkImageDataWithInfo::MimeType() const
    {
    if (iParams.iMimeType)
        {
        return *iParams.iMimeType;
        }
    else
        {
        return KNullDesC8;
        }
    }

//  End of File
