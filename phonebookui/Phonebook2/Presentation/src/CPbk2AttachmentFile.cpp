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
*        Attachment file handler
*
*/


// INCLUDES
#include    "CPbk2AttachmentFile.h"

// System includes
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
const TInt KFirstDuplicateNameIndex = 1;

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
    _LIT(KPanicText, "CPbk2AttachmentFile");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AttachmentFile* CPbk2AttachmentFile::NewL(
        const TDesC& aBaseName, 
        RFs& aRfs, 
        TUint aFileMode /*= EFileStream|EFileShareExclusive|EFileWrite*/)
    {
    CPbk2AttachmentFile* self = 
        CPbk2AttachmentFile::NewLC( aBaseName, aRfs, aFileMode );
    CleanupStack::Pop(self);  // self
    return self;
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AttachmentFile* CPbk2AttachmentFile::NewLC(
        const TDesC& aBaseName, 
        RFs& aRfs, 
        TUint aFileMode /*= EFileStream|EFileShareExclusive|EFileWrite*/)
    {
    CPbk2AttachmentFile* self = 
        new(ELeave) CPbk2AttachmentFile( aRfs, aFileMode );
    CleanupStack::PushL( self );
    self->ConstructL( aBaseName );
    return self;
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::~CPbk2AttachmentFile
// --------------------------------------------------------------------------
//
CPbk2AttachmentFile::~CPbk2AttachmentFile()
    {
    DeleteFile();
    delete iFileName;
    delete iBaseName;
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::FileName
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPbk2AttachmentFile::FileName() const
    {
    if ( iFileName )
        {
        return *iFileName;
        }
    else
        {
        return KNullDesC;
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::File
// --------------------------------------------------------------------------
//
EXPORT_C RFile& CPbk2AttachmentFile::File()
    {
    return iFile;
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::Release
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AttachmentFile::Release()
    {
    iFile.Close();
    iOwnsFile = EFalse;
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::SwitchDriveL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AttachmentFile::SwitchDriveL()
    {
    TPtrC drive;
    if ( iFileName )
        {
        // File already created successfully: switch the drive
        TParsePtrC fileNameParser( *iFileName );
        drive.Set( fileNameParser.Drive()==KRamDrive ? KPersistentDrive : KRamDrive );
        }
    else
        {
        // Default is RAM drive in ConstructL
        drive.Set( KPersistentDrive );
        }

    // Create new file
    CreateFileL( drive, KTempFilePath, *iBaseName );
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::CPbk2AttachmentFile
// --------------------------------------------------------------------------
//
CPbk2AttachmentFile::CPbk2AttachmentFile(
        RFs& aRfs, 
        TUint aFileMode ) :
    iRfs( aRfs ),
    iFileMode( aFileMode )
    {
    // CBase::operator new will reset members
    __ASSERT_DEBUG(!iBaseName && !iFileName && iFile.SubSessionHandle()==KNullHandle, 
        Panic(EPanicPostCond_Constructor) );
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AttachmentFile::ConstructL( const TDesC& aBaseName )
    {
    __ASSERT_DEBUG(!iBaseName && !iFileName && 
                    iFile.SubSessionHandle()==KNullHandle, 
                        Panic(EPanicPreCond_ConstructL));

    // Call this to ensure that we create session handle that can be passed 
    // via IPC. We are sharing RFile handles with SendUi.
    iRfs.ShareProtected();
    
    // Clean up aBaseName
    const TInt maxLength = 
        KMaxFileName - 
        KMaxDriveNameLength - 
        KTempFilePath().Length() - 
        KMaxNumberLength;
    iBaseName = HBufC::NewL( maxLength );
    TPtr baseName = iBaseName->Des();
    for ( TInt i=0; i < aBaseName.Length() && i < maxLength; ++i )
        {
        TChar ch = aBaseName[i];
        if ( KInvalidFileNameChars().Locate( ch ) == KErrNotFound )
            {
            baseName.Append( ch );
            }
        }
    baseName.TrimAll();
    if ( baseName.Length() == 0 )
        {
        baseName = KDefaultTempFileName;
        }

    // Try to create the file
    TRAPD( err, CreateFileL( KRamDrive, KTempFilePath, baseName ) );
    if ( err != KErrNone )
        {
        // Try on different drive
        SwitchDriveL();
        }

    __ASSERT_DEBUG( iBaseName && iFileName, Panic(EPanicPostCond_ConstructL) );
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::CreateFileL
// --------------------------------------------------------------------------
//
void CPbk2AttachmentFile::CreateFileL(
        const TDesC& aDrive, 
        const TDesC& aDir, 
        const TDesC& aBaseName )
    {
    __ASSERT_DEBUG(iBaseName && iRfs.Handle()!=KNullHandle, 
        Panic(EPanicPreCond_CreateFileL));

    // Create and init a local buffer for the file name
    HBufC* fileNameBuf = HBufC::NewLC( KMaxFileName );
    TPtr fileName = fileNameBuf->Des();
    fileName = aDrive;
    fileName.Append( aDir );
    // Create directory
    TInt error = iRfs.MkDirAll( fileName );
    if ( error != KErrAlreadyExists )
        {
        User::LeaveIfError( error );
        }
    fileName.Append( aBaseName );

    TInt number = KFirstDuplicateNameIndex;
    FOREVER
        {
        RFile file;
        const TInt error = file.Create( iRfs, fileName, iFileMode );
        if ( error == KErrNone )
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
        else if ( error != KErrAlreadyExists )
            {
            User::Leave( error );
            }

        // File name was reserved: append number to basename to make it unique
        fileName = aDrive;
        fileName.Append( aDir );
        TParsePtrC baseNameParser( aBaseName );
        fileName.Append( baseNameParser.Name() );
        fileName.AppendNum( number );
        fileName.Append( baseNameParser.Ext() );
        if ( ++number > KMaxFileNumber )
            {
            User::Leave( KErrTooBig );
            }
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFile::DeleteFile
// --------------------------------------------------------------------------
//
void CPbk2AttachmentFile::DeleteFile()
    {
    iFile.Close();
    if ( iFileName && iOwnsFile )
        {
        // Delete the file. If the deletion fails for some reason trust that the
        // system will delete the file when it cleans up temp file directories.
        iRfs.Delete( *iFileName );
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFileArray::CPbk2AttachmentFileArray
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AttachmentFileArray::CPbk2AttachmentFileArray( TInt aGranularity )
    : CArrayPtrFlat<CPbk2AttachmentFile>( aGranularity )
    {
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFileArray::~CPbk2AttachmentFileArray
// --------------------------------------------------------------------------
//
CPbk2AttachmentFileArray::~CPbk2AttachmentFileArray()
    {
    // Delete all objects in the array
    ResetAndDestroy();
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFileArray::MdcaCount
// --------------------------------------------------------------------------
//
TInt CPbk2AttachmentFileArray::MdcaCount() const
    {
    return Count();
    }
    
// --------------------------------------------------------------------------
// CPbk2AttachmentFileArray::MdcaPoint
// --------------------------------------------------------------------------
//
TPtrC CPbk2AttachmentFileArray::MdcaPoint( TInt aIndex ) const
    {
    return At( aIndex )->FileName();
    }

// --------------------------------------------------------------------------
// CPbk2AttachmentFileArray::FileHandleAt
// --------------------------------------------------------------------------
//    
RFile& CPbk2AttachmentFileArray::FileHandleAt( TInt aIndex )
    {
    return At( aIndex )->File();
    }

// End of File
