/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Presentation text data of the contact field.
*
*/


#ifndef CPBK2PRESENTATIONCONTACTFIELDTEXTDATA_H
#define CPBK2PRESENTATIONCONTACTFIELDTEXTDATA_H


// INCLUDES
#include <e32base.h>
#include <MVPbkContactFieldTextData.h>

class MVPbkStoreContactField;
class MPbk2FieldProperty;

// CLASS DECLARATIONS

/**
 * Presentation text data of the contact field
 */
NONSHARABLE_CLASS( CPbk2PresentationContactFieldTextData ) : 
        public MVPbkContactFieldTextData
    {
    public:  // Constructor & Destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aTextData         The field text data
         * @param aFieldProperty    Phonebook 2 field property.
         * @param aMaxDataLength    Data's max length
         * @return  A new instance of this class.
         */
        static CPbk2PresentationContactFieldTextData* NewL(
            MVPbkContactFieldTextData& aTextData,
            const MPbk2FieldProperty& aFieldProperty,
            const TInt aMaxDataLength );
        
        /**
         * Destructor.
         */
        ~CPbk2PresentationContactFieldTextData();

    public:  // Interface
        /**
         * Casts MVPbkContactFieldData to this interface.
         *
         * @precond aFieldData.DataType() == EVPbkFieldStorageTypeText
         *          VPbkError::Panic(VPbkError::EFieldDataTypeMismatch)
         *          is raised if the precondition does not hold.
         * @param aFieldData    Data of the field.
         * @return Casted inteface for the text field data.
         */
        static CPbk2PresentationContactFieldTextData& Cast(
                MVPbkContactFieldData& aFieldData);

        /**
         * Casts const MVPbkContactFieldData to this interface.
         *
         * @precond aFieldData.DataType() == EVPbkFieldStorageTypeText
         *          VPbkError::Panic(VPbkError::EFieldDataTypeMismatch)
         *          is raised if the precondition does not hold.
         * @param aFieldData    Data of the field.
         * @return Casted inteface for the text field data.
         */
        static const CPbk2PresentationContactFieldTextData& Cast(
                const MVPbkContactFieldData& aFieldData);

        
        TPtrC Text() const;
        void SetTextL( const TDesC& aText );
        TInt MaxLength() const;
        TVPbkFieldStorageType DataType() const;
        TBool IsEmpty() const;
        void CopyL(const MVPbkContactFieldData& aFieldData);
        
    private: 
        CPbk2PresentationContactFieldTextData( 
            MVPbkContactFieldTextData& aTextData,
            const MPbk2FieldProperty& aFieldProperty,
            const TInt aMaxDataLength );        
        
    private: // Data
        /// Ref: The contact field text data
        MVPbkContactFieldTextData& iTextData;
        /// Ref: Phonebook 2 field property
        const MPbk2FieldProperty& iFieldProperty;
        /// Own: Field data max length
        const TInt iMaxDataLength;

    };

#endif  // CPBK2PRESENTATIONCONTACTFIELDTEXTDATA_H

// End of File
