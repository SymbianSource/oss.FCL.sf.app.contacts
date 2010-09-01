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
*     Phonebook card converter class definition.
*
*/


#ifndef __CPbkvCardConverter_H__
#define __CPbkvCardConverter_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include <MPbkBackgroundProcess.h>
#include <cntdef.h>     // TContactItemId, CContactIdArray

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class TPbkContactItemField;
class CBCardEngine;
class CPbkAttachmentFileArray;
class MDesC16Array;

// CLASS DECLARATION

/**
 * Represents the different 'sending selected detail' menu selections.
 */
enum TPbkSendvCardChoiceItemEnumerations
    {
    ESendCurrentItem = 0,
    ESendAllData,
    ESendAllDataWithoutPicture,
    ECancel
    };


/**
 * A process which converts contacts to vCard files.
 */
class CPbkvCardConverter : 
        public CBase, 
        public MPbkBackgroundProcess
    {
    public: // constructor
        /**
         * Creates a new instance of this class.
		 * @param aFs file server handle
		 * @param aEngine phonebook contact engine
		 * @param aBCardEngine business card engine
         */
        static CPbkvCardConverter* NewL
            (RFs& aFs, CPbkContactEngine& aEngine, CBCardEngine& aBCardEngine);

        /**
         * Destructor.
         */
        ~CPbkvCardConverter();

    public: // interface
        /**
         * Initializes this process to convert aContactId to a vCard file.
         * @param aContactId id of contact item to convert
         * @param aField focused field of the contact (can be null)
         * @param aDataToSend what data of contact to send mapped into
         *      TPbkSendvCardChoiceItemEnumerations
         */
        void ConvertContactL(TContactItemId aContactId,
            TPbkContactItemField* aField, TInt aDataToSend);

        /**
         * Initializes this process to convert aContacts to vCard files.
		 * @param aContacts contact items to convert
         * @param aDataToSend what data of contact to send mapped into
         *      TPbkSendvCardChoiceItemEnumerations
         */
        void ConvertContactsL(const CContactIdArray& aContacts,
            TInt aDataToSend);

        /**
         * Returns the created vCard file names when the process is finished.
         */
        MDesC16Array& FileNames() const;

        /**
         * Returns Phonebook business card engine.
         */
        CBCardEngine& BCardEngine();

        /**
         * Resets the process and destroys any results generated.
         */
        void Reset();

    private: // from MPbkBackgroundProcess
        void StepL();
        TInt TotalNumberOfSteps();
        TBool IsProcessDone() const;
        void ProcessFinished();
        TInt HandleStepError(TInt aError);
        void ProcessCanceled();

    private:  // Implementation
        CPbkvCardConverter
            (RFs& aFs,
            CPbkContactEngine& aEngine, 
            CBCardEngine& aBCardEngine);
        void ConstructL();
        void CreatevCardFileL(TContactItemId aContactId);
        CPbkContactItem* PrepareContactLC(TContactItemId aContactId);
        void FillTemporaryContactL(CPbkContactItem& aDestItem,
            CPbkContactItem& aSourceItem,
            const TPbkContactItemField& aSourceField) const;
        void AddFieldToContactL(CPbkContactItem& aDestItem, 
            const TPbkContactItemField& aSourceField) const;

    private:  // Data
        /// Ref: open file server session
        RFs& iFs;
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: Business card engine
        CBCardEngine& iBCardEngine;
        /// Own: array of vCard files
        CPbkAttachmentFileArray* iVcardFiles;
        /// Own: contacts to convert
        CContactIdArray* iContacts;
        /// Ref: the field to send, can be null
        TPbkContactItemField* iField;
        /// Own: what data to send
        /// (mapped into TPbkSendvCardChoiceItemEnumerations)
        TInt iDataToSend;
    };

#endif // __CPbkvCardConverter_H__
            
// End of File
