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
*       Provides methods for the Image data abstraction hierarchy.
*
*/


// INCLUDE FILES
#include "CPbk2ImageData.h"

// From Phonebook2
#include "CPbk2AttachmentFile.h"

// From System
#include <f32file.h>

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CPbk2ImageFileData::CPbk2ImageFileData
// --------------------------------------------------------------------------
//
inline CPbk2ImageFileData::CPbk2ImageFileData(const TDesC& aFileName)
    {
    iFileName.Copy(aFileName.Left(iFileName.MaxLength()));
    }

// --------------------------------------------------------------------------
// CPbk2ImageFileData::~CPbk2ImageFileData
// --------------------------------------------------------------------------
//
CPbk2ImageFileData::~CPbk2ImageFileData()
    {
    delete iBuffer;
    }
    
// --------------------------------------------------------------------------
// CPbk2ImageFileData::NewL
// --------------------------------------------------------------------------
//
CPbk2ImageFileData* CPbk2ImageFileData::NewL(const TDesC& aFileName)
    {   
    return new(ELeave) CPbk2ImageFileData(aFileName);
    }

// --------------------------------------------------------------------------
// CPbk2ImageFileData::GetBufferL
// --------------------------------------------------------------------------
//
const TDesC8& CPbk2ImageFileData::GetBufferL() const
    {
    if (!iBuffer)
        {
        RFs fsSession;
        User::LeaveIfError(fsSession.Connect());
        CleanupClosePushL(fsSession);
        RFile file;
        User::LeaveIfError( file.Open(
            fsSession, iFileName, EFileRead|EFileShareReadersOnly ) );
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

// --------------------------------------------------------------------------
// CPbk2ImageFileData::GetFileNameL
// --------------------------------------------------------------------------
//
const TDesC&  CPbk2ImageFileData::GetFileNameL() const
    {
    return iFileName;
    }

// --------------------------------------------------------------------------
// CPbk2ImageBufferData::CPbk2ImageBufferData
// --------------------------------------------------------------------------
//
inline CPbk2ImageBufferData::CPbk2ImageBufferData
        (const TDesC8& aBuffer, const TDesC& aBaseName) :
    iBuffer(aBuffer)
    {
    iBaseName.Copy(aBaseName.Left(iBaseName.MaxLength()));
    }

// --------------------------------------------------------------------------
// CPbk2ImageBufferData::~CPbk2ImageBufferData
// --------------------------------------------------------------------------
//
CPbk2ImageBufferData::~CPbk2ImageBufferData()
    {
    delete iFile;
    iFsSession.Close();
    }
    
// --------------------------------------------------------------------------
// CPbk2ImageBufferData::NewL
// --------------------------------------------------------------------------
//
CPbk2ImageBufferData* CPbk2ImageBufferData::NewL
        (const TDesC8& aBuffer, const TDesC& aBaseName/*=KNullDesC*/)
    {
    CPbk2ImageBufferData* self = 
        new(ELeave) CPbk2ImageBufferData(aBuffer,aBaseName);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ImageBufferData::GetBufferL
// --------------------------------------------------------------------------
//
const TDesC8& CPbk2ImageBufferData::GetBufferL() const
    {
    return iBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2ImageBufferData::GetFileNameL
// --------------------------------------------------------------------------
//
const TDesC&  CPbk2ImageBufferData::GetFileNameL() const
    {
    if (!iFile)
        {
        if (iFsSession.Handle() == KNullHandle)
            {
            User::LeaveIfError(iFsSession.Connect());
            }
        CPbk2AttachmentFile* imageFile = 
            CPbk2AttachmentFile::NewLC(iBaseName,iFsSession);
        RFile& file = imageFile->File();
        User::LeaveIfError(file.Write(iBuffer));
        User::LeaveIfError(file.Flush());
        file.Close();
        CleanupStack::Pop(imageFile);
        iFile = imageFile;
        }
    return iFile->FileName();
    }

//  End of File
