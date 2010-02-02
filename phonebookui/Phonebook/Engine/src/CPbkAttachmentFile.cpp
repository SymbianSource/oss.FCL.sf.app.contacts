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
*        Attachment file handler
*
*/


// INCLUDES
#include    "CPbkAttachmentFile.h"
#include    <e32std.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS
_LIT(KRamDrive, "d:");
_LIT(KPersistentDrive, "c:");
_LIT(KTempFilePath, "\\system\\temp\\");
_LIT(KInvalidFileNameChars, "?*<>/\"|\\:");
_LIT(KDefaultTempFileName, "TEMP.tmp");
const TInt KMaxDriveNameLength = 2;
const TInt KMaxNumberLength = 5;
const TInt KMaxFileNumber = 99999;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicPreCond_CreateFileL,
    EPanicPostCond_CreateFileL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkAttachmentFile");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkAttachmentFile* CPbkAttachmentFile::NewL
        (const TDesC& aBaseName, 
        RFs& aRfs, 
        TUint aFileMode /*= EFileStream|EFileShareExclusive|EFileWrite*/)
    {
    CPbkAttachmentFile* self = CPbkAttachmentFile::NewLC(aBaseName, aRfs, aFileMode);
    CleanupStack::Pop();  // self
    return self;
    }

EXPORT_C CPbkAttachmentFile* CPbkAttachmentFile::NewLC
        (const TDesC& aBaseName, 
        RFs& aRfs, 
        TUint aFileMode /*= EFileStream|EFileShareExclusive|EFileWrite*/)
    {
    CPbkAttachmentFile* self = new(ELeave) CPbkAttachmentFile;
    CleanupStack::PushL(self);
    self->iRfs = aRfs;
    self->iFileMode = aFileMode;
    self->ConstructL(aBaseName);
    return self;
    }

CPbkAttachmentFile::~CPbkAttachmentFile()
    {
    DeleteFile();
    delete iFileName;
    delete iBaseName;
    }

EXPORT_C const TDesC& CPbkAttachmentFile::FileName() const
    {
    if (iFileName)
        {
        return *iFileName;
        }
    else
        {
        return KNullDesC;
        }
    }

EXPORT_C RFile& CPbkAttachmentFile::File()
    {
    return iFile;
    }

EXPORT_C void CPbkAttachmentFile::Release()
    {
    iFile.Close();
    iOwnsFile = EFalse;
    }

EXPORT_C void CPbkAttachmentFile::SwitchDriveL()
    {
    TPtrC drive;
    if (iFileName)
        {
        // File already created successfully: switch the drive
        TParsePtrC fileNameParser(*iFileName);
        drive.Set(fileNameParser.Drive()==KRamDrive ? KPersistentDrive : KRamDrive);
        }
    else
        {
        // Default is RAM drive in ConstructL
        drive.Set(KPersistentDrive);
        }

    // Create new file
    CreateFileL(drive, KTempFilePath, *iBaseName);
    }

CPbkAttachmentFile::CPbkAttachmentFile()
    {
    // CBase::operator new will reset members
    __ASSERT_DEBUG(!iBaseName && !iFileName && iFile.SubSessionHandle()==KNullHandle, 
        Panic(EPanicPostCond_Constructor));
    }

void CPbkAttachmentFile::ConstructL(const TDesC& aBaseName)
    {
    __ASSERT_DEBUG(!iBaseName && !iFileName && iFile.SubSessionHandle()==KNullHandle, 
        Panic(EPanicPreCond_ConstructL));

    // Clean up aBaseName
    const TInt maxLength = KMaxFileName - KMaxDriveNameLength - KTempFilePath().Length() - KMaxNumberLength;
    iBaseName = HBufC::NewL(maxLength);
    TPtr baseName = iBaseName->Des();
    for (TInt i=0; i < aBaseName.Length() && i < maxLength; ++i)
        {
        TChar ch = aBaseName[i];
        if (KInvalidFileNameChars().Locate(ch) == KErrNotFound)
            {
            baseName.Append(ch);
            }
        }
    baseName.TrimAll();
    if (baseName.Length() == 0)
        {
        baseName = KDefaultTempFileName;
        }

    // Try to create the file
    TRAPD(err, CreateFileL(KRamDrive, KTempFilePath, baseName));
    if (err != KErrNone)
        {
        // Try on different drive
        SwitchDriveL();
        }

    __ASSERT_DEBUG(iBaseName && iFileName, Panic(EPanicPostCond_ConstructL));
    }

void CPbkAttachmentFile::CreateFileL
        (const TDesC& aDrive, const TDesC& aDir, const TDesC& aBaseName)
    {
    __ASSERT_DEBUG(iBaseName && iRfs.Handle()!=KNullHandle, 
        Panic(EPanicPreCond_CreateFileL));

    // Create and init a local buffer for the file name
    HBufC* fileNameBuf = HBufC::NewLC(KMaxFileName);
    TPtr fileName = fileNameBuf->Des();
    fileName = aDrive;
    fileName.Append(aDir);
    // Create directory
    TInt error = iRfs.MkDirAll(fileName);
    if (error != KErrNone && error != KErrAlreadyExists)
        {
        User::Leave(error);
        }
    fileName.Append(aBaseName);

    TInt number = 0;
    FOREVER
        {
        RFile file;
        const TInt error = file.Create(iRfs, fileName, iFileMode);
        
        TParsePtrC parsePtr(fileName);
                
        if (error == KErrNone && parsePtr.Name().Length() > 0)
            {
            // File created succesfully: switch state
            DeleteFile();  // Delete previous file
            iFile = file;
            delete iFileName;
            iFileName = fileNameBuf;
            iOwnsFile = ETrue;
            CleanupStack::Pop();  // fileNameBuf
            __ASSERT_DEBUG(iFileName && iFile.SubSessionHandle()!=KNullHandle, 
                Panic(EPanicPostCond_CreateFileL));
            return;
            }
        else if (error != KErrAlreadyExists && error != KErrNone)
            {
            User::Leave(error);
            }

        // File name was reserved: append number to basename to make it unique
        fileName = aDrive;
        fileName.Append(aDir);
        TParsePtrC baseNameParser(aBaseName);
        fileName.Append(baseNameParser.Name());
        fileName.AppendNum(number);
        fileName.Append(baseNameParser.Ext());
        if (++number > KMaxFileNumber)
            {
            User::Leave(KErrTooBig);
            }
        }
    }

void CPbkAttachmentFile::DeleteFile()
    {
    iFile.Close();
    if (iFileName && iOwnsFile)
        {
        // Delete the file. If the deletion fails for some reason trust that the
        // system will delete the file when it cleans up temp file directories.
        iRfs.Delete(*iFileName);
        }
    }

EXPORT_C CPbkAttachmentFileArray::CPbkAttachmentFileArray(TInt aGranularity)
    : CArrayPtrFlat<CPbkAttachmentFile>(aGranularity)
    {
    }

EXPORT_C CPbkAttachmentFileArray::~CPbkAttachmentFileArray()
    {
    // Delete all objects in the array
    ResetAndDestroy();
    }

EXPORT_C TInt CPbkAttachmentFileArray::MdcaCount() const
    {
    return Count();
    }

EXPORT_C TPtrC CPbkAttachmentFileArray::MdcaPoint(TInt aIndex) const
    {
    return At(aIndex)->FileName();
    }

// End of File
