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
* Description:  Phonebook 2 attachment file.
*
*/


#ifndef CPBK2ATTACHMENTFILE_H
#define CPBK2ATTACHMENTFILE_H

// INCLUDES
#include <f32file.h>     // File modes
#include <bamdesca.h>    // MDesCArray

// CLASS DECLARATION

/**
 * Class which creates a temporary file at construction and deletes the
 * file when destroyed.
 */
NONSHARABLE_CLASS(CPbk2AttachmentFile) : public CBase
    {
    public: // Types

        /**
         * Drive enumerations.
         */
        enum TTempDrive
            {
            /// RAM disk, contents lost at power-down
            ERamDisk = 1,
            /// Persistent file system
            EPersistent
            };

    public: // Construction and destruction

        /**
         * Creates a new attachment file.
         *
         * @param aBaseName Basename used to build an unique filename. Used
         *                  as-is if the filename is not in use.
         * @param aRfs      Open file server session handle.
         * @param aFileMode Mode in which to create the file.
         *                  @see RFile::Create.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2AttachmentFile* NewL(
                const TDesC& aBaseName,
                RFs& aRfs,
                TUint aFileMode = EFileStream|EFileShareExclusive|EFileWrite );

        /**
         * Creates a new attachment file.
         *
         * @param aBaseName Basename used to build an unique filename. Used
         *                  as-is if the filename is not in use.
         * @param aRfs      Open file server session handle.
         * @param aFileMode Mode in which to create the file.
         *                  @see RFile::Create.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2AttachmentFile* NewLC(
                const TDesC& aBaseName,
                RFs& aRfs,
                TUint aFileMode = EFileStream|EFileShareExclusive|EFileWrite );

        /**
         * Destructor. Closes and deletes the file from the file system.
         * If Release() has been called the file is not deleted.
         */
        ~CPbk2AttachmentFile();

    public: // Interface

        /**
         * Returns the full file name.
         *
         * @return  File name.
         */
        IMPORT_C const TDesC& FileName() const;

        /**
         * Returns the opened attachment file.
         *
         * @return  Opened attachment file.
         */
        IMPORT_C RFile& File();

        /**
         * Switches drive to use from RAM disk to persistent disk or vice
         * versa and recreates the attachment file on the other drive. Current
         * file is closed and deleted.
         */
        IMPORT_C void SwitchDriveL();

        /**
         * Closes the file and releases ownership of it. After calling this
         * function the file is no longer deleted in destructor.
         */
        IMPORT_C void Release();

    private: // Implementation
        CPbk2AttachmentFile( RFs& aRfs, TUint aFileMode );
        void ConstructL( const TDesC& aBaseName );
        void CreateFileL(
                const TDesC& aDrive,
                const TDesC& aDir,
                const TDesC& aBaseName );
        void DeleteFile();

    private: // Data
        /// Ref: Handle to file server
        RFs iRfs;
        /// Own: File mode
        TUint iFileMode;
        /// Own: Cleaned up base name
        HBufC* iBaseName;
        /// Own: Name of the file
        HBufC* iFileName;
        /// Own: The opened file
        RFile iFile;
        /// Own: ETrue if this object owns the file and should delete it in
        ///      the destructor
        TBool iOwnsFile;
    };

class MPbk2AttachmentFileArray
    {
    public:
        virtual ~MPbk2AttachmentFileArray() {}
    
    public:
        virtual RFile& FileHandleAt( TInt aIndex ) =0;
    };

/**
 * Array of CPbk2AttachmentFile objects. This array owns the object it contains,
 * in other words it will delete the contained objects in its destructor.
 * Implements MDesCArray interface through which the file names can be
 * retrieved.
 * Also RFile handle objects can be retrieved by index.
 */
NONSHARABLE_CLASS(CPbk2AttachmentFileArray) :
        public CArrayPtrFlat<CPbk2AttachmentFile>, 
        public MDesCArray,
        public MPbk2AttachmentFileArray
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aGranularity  Array reallocation granularity.
         */
        IMPORT_C CPbk2AttachmentFileArray(
                TInt aGranularity );

        /**
         * Destructor.
         * Deletes all CPbk2AttachmentFile objects in the array.
         */
        ~CPbk2AttachmentFileArray();

    public: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC MdcaPoint(
                TInt aIndex ) const;
    
    public: // From MPbk2AttachmentFileArray
        RFile& FileHandleAt( TInt aIndex );
    };

#endif // CPBK2ATTACHMENTFILE_H

// End of File
