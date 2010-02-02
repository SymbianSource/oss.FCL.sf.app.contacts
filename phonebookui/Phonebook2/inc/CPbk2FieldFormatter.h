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
* Description:  Phonebook 2 field formatter.
*
*/


#ifndef CPBK2FIELDFORMATTER_H
#define CPBK2FIELDFORMATTER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPbk2PhoneNumberFormatter;
class MVPbkBaseContactField;
class MVPbkFieldType;
class MPbk2FieldPropertyArray;
class MVPbkContactFieldDateTimeData;
class MVPbkFieldTypeList;

// CLASS DECLARATION

/**
 * Phonebook 2 field formatter.
 *
 * Responsible for formatting list box field for contact
 * field presentation needs.
 */
NONSHARABLE_CLASS(CPbk2FieldFormatter) : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aFieldProperties  Field property array
         * @param aFieldTypeList    Virtual Phonebook field type list.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2FieldFormatter* NewL(
                const MPbk2FieldPropertyArray& aFieldProperties,
                const MVPbkFieldTypeList& aFieldTypeList );

        /**
         * Destructor.
         */
        ~CPbk2FieldFormatter();

    public: // Interface

        /**
         * Returns formatted contents of given field.
         *
         * @param aField        The field to format.
         * @param aFieldType    The field type of the field.
         * @return  Formatted content of the field.
         */
        IMPORT_C TPtrC FormatFieldContentL(
                const MVPbkBaseContactField& aField,
                const MVPbkFieldType& aFieldType );

        /**
         * Sets the field's time format.
         *
         * @param aTimeFormat   Time format to set.
         */
        IMPORT_C void SetTimeFormatL(
                const TDesC& aTimeFormat );

    private: // Implementation
        CPbk2FieldFormatter(
                const MPbk2FieldPropertyArray& aFieldProperties,
                const MVPbkFieldTypeList& aFieldTypeList );
        void ConstructL();
        TPtr ReallocBufferL(
                TInt aNewSize );
        TBool MatchesFieldTypeL(
                const MVPbkFieldType& aFieldType,
                TInt aResourceId ) const;
        void FormatPhoneNumberContentL(
                TPtrC& aRawContent,
                TPtr& aFormattedContent );
        void FormatDtmfContentL(
                TPtrC& aRawContent,
                TPtr& aFormattedContent );
        void FormatSyncContentL(
                TPtrC& aRawContent,
                TPtr& aFormattedContent );
        void FormatFileNameContentL(
                TPtrC& aRawContent,
                TPtr& aFormattedContent );
        void FormatTextContentL(
                TPtrC& aRawContent,
                TPtr& aFormattedContent,
                const MVPbkFieldType& aFieldType );
        void FormatAddressContentL(
                TPtrC& aRawContent,
                TPtr& aFormattedContent );
        void FormatDateTimeContentL(
                const MVPbkContactFieldDateTimeData& aRawContent,
                TPtr& aFormattedContent );

    private: // Data
        /// Own: Buffer for handling line strings
        HBufC* iBuffer;
        /// Own: Time format
        HBufC* iTimeFormat;
        /// Own: Phone number formatter
        MPbk2PhoneNumberFormatter* iNumberFormatter;
        /// Ref: Phonebook 2 field property array
        const MPbk2FieldPropertyArray& iFieldProperties;
        /// Ref: Virtual Phonebook field type list
        const MVPbkFieldTypeList& iFieldTypeList;
    };

#endif // CPBK2FIELDFORMATTER_H

// End of File
