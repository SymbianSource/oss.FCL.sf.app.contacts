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
* Description:  A class for combining the data of two contacts.
*
*/


#ifndef CPBK2CONTACTMERGE_H
#define CPBK2CONTACTMERGE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkBaseContact;
class MVPbkStoreContact;
class MVPbkBaseContactField;
class MVPbkContactFieldData;
class MVPbkContactFieldTextData;
class MVPbkContactFieldBinaryData;
class MVPbkContactFieldDateTimeData;
class CVPbkFieldTypeSelector;
class MPbk2ContactNameFormatter;
class MPbk2FieldPropertyArray;

// CLASS DECLARATION

/**
 * A class for combining the data of two contacts.
 */
class CPbk2ContactMerge : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aNameFormatter    Name formatter.
         * @param aFieldProperties  Phonebook 2 field properties.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ContactMerge* NewL(
                CVPbkContactManager& aContactManager,
                MPbk2ContactNameFormatter& aNameFormatter,
                const MPbk2FieldPropertyArray& aFieldProperties );

        /**
         * Destructor.
         */
        virtual ~CPbk2ContactMerge();

    public: // Interface

        /**
         * Merges the data of two contacts.
         * Title fields of the target contact are not changed.
         *
         * @param aContactFrom  The source contact whose data need to
         *                      be merged.
         * @param aContactTo    The target contact into which the data
         *                      is copied to.
         * @return  ETrue if all the data fields from the source
         *          contact were copied to the target contact.
         *          EFalse if none or part of the fields were copied.
         */
        IMPORT_C TBool MergeDataL(
                const MVPbkBaseContact& aContactFrom,
                MVPbkStoreContact& aContactTo );

    private: // Implementation
        CPbk2ContactMerge(
                CVPbkContactManager& aContactManager,
                MPbk2ContactNameFormatter& aNameFormatter,
                const MPbk2FieldPropertyArray& aFieldProperties );
        void ConstructL();
        void CopyFieldDataL(
                const MVPbkBaseContactField& aField,
                MVPbkStoreContact& aContactTo );
        TBool FindMatch(
                const MVPbkBaseContactField& aField,
                MVPbkStoreContact& aContactTo );
        TBool CompareData(
                const MVPbkContactFieldData& aData1,
                const MVPbkContactFieldData& aData2 );
        TBool CompareTextData(
                const MVPbkContactFieldTextData& aData1,
                const MVPbkContactFieldTextData& aData2 );
        TBool CompareBinaryData(
                const MVPbkContactFieldBinaryData& aData1,
                const MVPbkContactFieldBinaryData& aData2 );
        TBool CompareDateTimeData(
                const MVPbkContactFieldDateTimeData& aData1,
                const MVPbkContactFieldDateTimeData& aData2 );

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: Phonebook 2 field properties
        const MPbk2FieldPropertyArray& iFieldProperties;
        /// Own: Field type selector for email fields
        CVPbkFieldTypeSelector* iEMailSelector;
        /// Own: Field type selector for number fields
        CVPbkFieldTypeSelector* iNumberSelector;
    };

#endif // CPBK2CONTACTMERGE_H

// End of File
