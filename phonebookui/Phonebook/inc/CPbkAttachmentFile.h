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
*     Attachment file handler
*
*/


#ifndef __CPbkAttachmentFile_H__
#define __CPbkAttachmentFile_H__

// INCLUDES
#include    <f32file.h>     // File modes
#include    <bamdesca.h>    // MDesCArray

//  CLASS DECLARATION 

/**
 * @internal Only Phonebook internal use supported!
 *
 * Class which creates a temporary file at construction and deletes the
 * file when destroyed.
 */
class CPbkAttachmentFile : public CBase
	{
    public: // Interface
        /// Drive enumeration
        enum TTempDrive
            {
            /// RAM disk. Contents lost at power-down
            ERamDisk = 1,
            /// Persistent file system
            EPersistent
            };

        /**
         * Creates new attachment file.
         *
         * @param aBaseName basename used to build a unique filename. Used
         *                  as-is if the filename is not in use.
         * @param aRfs      open file server session handle.
         * @param aFileMode mode in which to create the file, see RFile::Create.
         */
		IMPORT_C static CPbkAttachmentFile* NewL
            (const TDesC& aBaseName, 
            RFs& aRfs,
            TUint aFileMode = EFileStream|EFileShareExclusive|EFileWrite);

        /**
         * Like NewL but leaves the created object on the cleanup stack.
         * @see CPbkAttachmentFile::NewL
         */
		IMPORT_C static CPbkAttachmentFile* NewLC
            (const TDesC& aBaseName, 
            RFs& aRfs,
            TUint aFileMode = EFileStream|EFileShareExclusive|EFileWrite);

        /**
         * Destructor. Closes and deletes the file from the file system.
         * If Release() has been called the file is not deleted.
         */
        ~CPbkAttachmentFile();

        /**
         * Returns the full file name.
         */
        IMPORT_C const TDesC& FileName() const;

        /**
         * Returns the opened attachment file.
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

	private:  // Implementation
        CPbkAttachmentFile();
        void ConstructL(const TDesC& aBaseName);
        void CreateFileL(const TDesC& aDrive, const TDesC& aDir, const TDesC& aBaseName);
        void DeleteFile();
    
    private:  // Data
        /// Ref: Handle to file server
        RFs iRfs;
        /// Own: file mode
        TUint iFileMode;
        /// Own: cleaned up base name
        HBufC* iBaseName;
        /// Own: name of the file
        HBufC* iFileName;
        /// Own: the opened file
        RFile iFile;
        /// Own: ETrue if this object owns the file and should delete it in
        /// destructor
        TBool iOwnsFile;
	};


/**
 * Array of CPbkAttachmentFile objects. This array owns the object it contains,
 * in other words it will delete the contained objects in its destructor.
 * Implements MDesCArray interface through which the file names can be 
 * retrieved.
 */
class CPbkAttachmentFileArray : 
        public CArrayPtrFlat<CPbkAttachmentFile>, public MDesCArray
    {
    public:  // Interface
        /**
         * Constructor.
         * @param aGranularity array reallocation granularity.
         */
        IMPORT_C CPbkAttachmentFileArray(TInt aGranularity);

        /**
         * Destructor. Deletes all CPbkAttachmentFile objects in the array.
         */
        IMPORT_C ~CPbkAttachmentFileArray();

    public:  // from MDesCArray
        IMPORT_C TInt MdcaCount() const;
        IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;
    };

#endif // __CPbkAttachmentFile_H__

// End of File
