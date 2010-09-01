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
*     List box model class for field listboxes displaying Phonebook fields.
*
*/


#ifndef __CPbkFieldListBoxModel_H__
#define __CPbkFieldListBoxModel_H__

//  INCLUDES
#include <e32base.h>        // CBase
#include <bamdesca.h>       // MDesCArray
#include "PbkIconId.hrh"    // TPbkIconId


// FORWARD DECLARATIONS
class CPbkFieldArray;
class CPbkIconArray;
class CDesC16Array;
class TPbkContactItemField;
class MPbkClipListBoxText;
class MPbkFieldAnalyzer;
class CPbkFieldFormatter;
class CPbkInputAbsorber;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * List box model row class for listboxes displaying Phonebook fields.
 *
 * @see CPbkContactInfoControl
 */
NONSHARABLE_CLASS(CPbkFieldListBoxRow) :
        public CBase
    {
    public:
        /**
         * Creates a new instance of this class.
         */
        static CPbkFieldListBoxRow* NewL();

        /**
         * Destructor.
         */
        ~CPbkFieldListBoxRow();

    public: // interface
        /**
         * Returns the number of columns in this row.
         */
        TInt ColumnCount() const;

        /**
         * Returns the descriptor at the aColumnIndex position.
         */
        TPtrC At(TInt aColumnIndex) const;

        /**
         * Appends a column aColumnText to the row.
         */
        void AppendColumnL(const TDesC& aColumnText);

        /**
         * Returns the total descriptor length of all the columns.
         */
        TInt TotalLength() const;

        /**
         * Returns the maximum column length of this row.
         */
        TInt MaxColumnLength() const;

    private: // implementation
        CPbkFieldListBoxRow();

    private:
        /// Own: column array
        RPointerArray<HBufC> iColumns;
    };

/**
 * @internal Only Phonebook internal use supported!
 *
 * List box model class for listboxes displaying Phonebook fields.
 *
 * @see CPbkContactInfoControl
 */
NONSHARABLE_CLASS(CPbkFieldListBoxModel) :
        public CBase,
        public MDesCArray
    {
    public:  // Types
        enum TColumnIndex
            {
            EIconColumn = 0,
            ELabelColumn,
            EContentColumn
            };

    public:  // Constructors and destructor
        /**
         * Construction parameters for CPbkFieldListBoxModel.
         */
        class TParams
            {
            public:
                /**
                 * Constructor.
                 *
                 * @param aFields array of fields to display
                 * @param aTimeFormat time formatting string to use
                 * @param aIconArray icon array to use
                 */
                TParams
                    (const CPbkFieldArray& aFields,
                    const TDesC& aTimeFormat,
                    const CPbkIconArray& aIconArray);
                /// Ref: array of fields to display
                const CPbkFieldArray& iFields;
                /// Ref: time formatting string
                const TDesC& iTimeFormat;
                /// Ref: icon array
                const CPbkIconArray& iIconArray;
                /// Own: default icon id
                TPbkIconId iDefaultIconId;
                /// Ref: interface for analyzing field
                MPbkFieldAnalyzer* iFieldAnalyzer;
            };

        /**
         * Creates a new instance of this class.
         *
         * @param aParams @see TParams
         */
        static CPbkFieldListBoxModel* NewL(TParams& aParams);

        /**
         * Destructor.
         */
        ~CPbkFieldListBoxModel();

        /**
         * Returns the internal field array set in NewL.
         */
        const CPbkFieldArray& Array() const;

        /**
         * Appends a row aLine to the end of the model.
         */
        void AppendRowL(CPbkFieldListBoxRow* aLine);

        /**
         * Set the text clipper aTextClipper.
         */
        void SetClipper(MPbkClipListBoxText& aTextClipper);

    public: // from MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;

    private:  // Implementation
        CPbkFieldListBoxModel();
        void FormatFieldL
            (const TPbkContactItemField& aField, TParams& aParams);
        void ConstructL(TParams& aParams);
        void ExpandBuffersL(const CPbkFieldListBoxRow& aRow);
        TPtr ExpandColumnBufferL(TInt aRequiredLength);

    private:    // Data
        /// Own: text row array
        CArrayPtrFlat<CPbkFieldListBoxRow> iRows;
        /// Own: buffer storing one formatted listbox row
        HBufC* iLineBuf;
        /// Own: buffer for storing one formatted listbox column
        HBufC* iColumnBuf;
        /// Own: field content formatter
        CPbkFieldFormatter* iFieldFormatter;
        /// Ref: array of fields
        const CPbkFieldArray* iFields;
        /// Ref: object for clipping text
        MPbkClipListBoxText* iTextClipper;
        /// Own: Input absorber to protect model creation from
        /// NssVas active wait
        CPbkInputAbsorber* iAbsorber;
    };

#endif // __CPbkFieldListBoxModel_H__

// End of File
