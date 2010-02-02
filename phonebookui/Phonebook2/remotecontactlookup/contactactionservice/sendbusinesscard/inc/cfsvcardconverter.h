/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  vCard converter.
*
*/


#ifndef CFscvCardConverter_H
#define CFscvCardConverter_H

// INCLUDES
#include "mfsvcardconverterobserver.h"
#include <s32file.h>    // RFileWriteStream
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkStoreContact;
class MVPbkBaseContactField;
class CVPbkVCardEng;
class CPbk2AttachmentFile;
class CPbk2AttachmentFileArray;
class MDesC16Array;
class MPbk2ContactNameFormatter;
class MVPbkContactOperationBase;

// DATA DECLARATION

/**
 * Represents the different 'sending selected detail' menu selections.
 */
enum TFSSendvCardChoiceItemEnumerations
    {
    ESendCurrentItem = 0,
    ESendAllData,
    ESendAllDataWithoutPicture,
    ECancel
    };

// CLASS DECLARATION
/**
 * RCL vCard converter.
 * Responsible for converting contacts to vCards..
 */
NONSHARABLE_CLASS(CFscvCardConverter) :
        public CActive,
        public MVPbkSingleContactOperationObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aFs               File server handle.
         * @param aEngine           Virtual Phonebook contact manager.
         * @param aVCardEngine      Business card engine.
         * @param aNameFormatter    Contact name formatter.
         * @return A new instance of this class
         */
        static CFscvCardConverter* NewL(
                RFs& aFs,
                CVPbkContactManager& aEngine,
                CVPbkVCardEng& aVCardEngine,
                MPbk2ContactNameFormatter& aNameFormatter );

        /**
         * Destructor.
         */
        ~CFscvCardConverter();

    public: // Interface

        /**
         * Initializes this process to convert given contact to a vCard file.
         *
         * @param aContacts     Contact to convert.
         * @param aField        Focused field of the contact (can be null).
         * @param aDataToSend   The data to send mapped into
         *                      TFSSendvCardChoiceItemEnumerations.
         * @param aObserver     Observer.
         */
        void ConvertContactL(
                const TArray<MVPbkStoreContact*> aContacts,
                const MVPbkBaseContactField* aField,
                TInt aDataToSend,
                MFsvCardConverterObserver& aObserver );

        /**
         * Initializes this process to convert given contacts to vCard files.
         *
         * @param aContacts     Contacts to convert.
         * @param aDataToSend   The data to send mapped into
         *                      TFSSendvCardChoiceItemEnumerations.
         * @param aObserver     Observer.
         */
        void ConvertContactsL(
                const TArray<MVPbkStoreContact*> aContacts,
                TInt aDataToSend,
                MFsvCardConverterObserver& aObserver );
        
        /**
         * Returns the created vCard file names when the process is finished.
         *
         * @return  vCard file names.
         */
        MDesC16Array& FileNames() const;

        /**
         * Resets the process and destroys any results generated.
         */
        void Reset();

        /**
         * Returns attachment file array.
         *
         * @return  Attachment file array.
         */
        CPbk2AttachmentFileArray& AttachmentFileArray();
        
    private:    //From CActive    
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);
    
    private: // From MVPbkSingleContactOperationObserver
            void VPbkSingleContactOperationComplete(
                    MVPbkContactOperationBase& aOperation,
                    MVPbkStoreContact* aContact );
            void VPbkSingleContactOperationFailed(
                    MVPbkContactOperationBase& aOperation,
                    TInt aError );
            
    private: // Implementation
        CFscvCardConverter(
                RFs& aFs,
                CVPbkContactManager& aEngine,
                CVPbkVCardEng& aBCardEngine,
                MPbk2ContactNameFormatter& aNameFormatter );
        void ConstructL();
        void CreatevCardFileL(
                MVPbkStoreContact* aContact );
        MVPbkStoreContact* PrepareContactL(
                MVPbkStoreContact* aContact );
        void FillTemporaryContactL(
                MVPbkStoreContact& aDestContact,
                const MVPbkStoreContact& aSourceContact,
                const MVPbkBaseContactField& aSourceField ) const;
        void AddFieldToContactL(
                MVPbkStoreContact& aDestItem,
                const MVPbkBaseContactField& aSourceField ) const;
        void Start();
        void HandleNextContactL(
                MVPbkStoreContact* aContact );
        void IssueRequest();
        void FinalizeVCardExport();

    private: // Data
        /// Ref: Open file server session
        RFs& iFs;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iEngine;
        /// Ref: Business card engine
        CVPbkVCardEng& iVCardEngine;
        /// Own: Array of vCard files
        CPbk2AttachmentFileArray* iVcardFiles;
        /// Own: Contacts to convert
        RPointerArray<MVPbkStoreContact> iContacts;
        /// Ref: Contact name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: The field to send, can be null
        const MVPbkBaseContactField* iField;
        /// Own: The data to send
        /// (mapped into TFSSendvCardChoiceItemEnumerations)
        TInt iDataToSend;
        /// Ref: Observer
        MFsvCardConverterObserver* iObserver;        
        /// Own: Temp contact with fields to be sended
        MVPbkStoreContact* iVCardContact;
        /// Own: vCard file
        CPbk2AttachmentFile* iVCardFile;
        /// Own: File stream for vCard file writing
        RFileWriteStream iFileWriteStream;
        /// Own: Index of currently converted contact
        TInt iOpIndex;
        /// Own: Flag indicating is this field level operation
        TBool iFieldLevelOperation;
        /// Own: Flag indicating that the file writer has tried next drive
        TBool iNextDriveTried;    
        /// Own: Contact operation handle
        MVPbkContactOperationBase* iContactOperation;
    };

#endif // CFscvCardConverter_H

// End of File
