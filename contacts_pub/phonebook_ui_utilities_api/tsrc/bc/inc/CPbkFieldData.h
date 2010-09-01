/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Classes for storing Phonebook contact data in a contact database 
*
*/








#ifndef __CPbkFieldData_H__
#define __CPbkFieldData_H__

//  INCLUDES
#include "MPbkFieldData.h"  // Abstract interfaces


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook field data class. 
 * Implements MPbkFieldData.
 */
class CPbkFieldData : 
        public CBase, public MPbkFieldData
    {
    public:  // Constructors and destructor
        /**
         * Creates a new text field data.
		 * @param aFieldId field id to create
		 * @param aText text to insert to field
		 * @return the created field data object
         */
        IMPORT_C static CPbkFieldData* NewL(TPbkFieldId aFieldId,
			const TDesC& aText);

        /**
         * Creates a new time field data.
		 * @param aFieldId field id to create
		 * @param aTime time to insert to field
		 * @return the created field data object
         */
        IMPORT_C static CPbkFieldData* NewL(TPbkFieldId aFieldId,
			const TTime& aTime);

        /**
         * Creates a new field data object from aStream.
		 * @return the created field data object
         */
        IMPORT_C static CPbkFieldData* NewL(RReadStream& aStream);

        /**
         * Destructor.
         */
        ~CPbkFieldData();

    public:  // Interface
        /**
         * Returns the type of this field data.
         * @return KStrorageTypeText or KStoragetTypeDateTime.
         */
        inline TStorageType Type() const;

        /**
         * Returns the field id of this field data.
         */
        inline TPbkFieldId FieldId() const;

        /**
         * Returns the text value of this field data.
         * @exception EPanicInvalidType if Type() != KStrorageTypeText.
         */
        IMPORT_C const TDesC& Text() const;

        /**
         * Returns the Time value of this field data.
         * @exception EPanicInvalidType if Type() != KStoragetTypeDateTime.
         */
        IMPORT_C const TTime& Time() const;

    public:  // from MPbkFieldData
        IMPORT_C TStorageType PbkFieldType() const;
        IMPORT_C TPbkFieldId PbkFieldId() const;
        IMPORT_C TPtrC PbkFieldText() const;
        IMPORT_C TTime PbkFieldTime() const;

    public:  // Externalize/internalize
        /**
         * Externalizes this object to aStream.
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

        /**
         * Internalizes this object from aStream.
         */
        IMPORT_C void InternalizeL(RReadStream& aStream);

    private:  // Implementation
        CPbkFieldData();
		void ConstructL(TPbkFieldId aFieldId, const TDesC& aText);
		void ConstructL(TPbkFieldId aFieldId, const TTime& aTime);
		void ConstructL(RReadStream& aStream);

    private:  // Data
        /// Type enum
        enum TType
            {
            /// Text data
            ETypeText = KStorageTypeText,
            /// Time data
            ETypeTime = KStorageTypeDateTime
            };
        /// Own: type
        TType iType;
        /// Own: field id
        TPbkFieldId iFieldId;
        /// Own: text data
        HBufC* iText;
        /// Own: time data
        TTime iTime;
    };


/**
 * Array of CPbkFieldData objects.
 */
class CPbkFieldDataArray : 
        public CArrayPtrFlat<CPbkFieldData>,
        public MPbkFieldDataArray
    {
    public:  // Constructors and destructor
        /**
         * Creates an instance of this class.
         * @return Newly created instance of this class.
         */
        IMPORT_C static CPbkFieldDataArray* NewL();

        /**
         * Creates an instance of this class from aStream.
         * @return Newly created instance of this class.
         */
        IMPORT_C static CPbkFieldDataArray* NewL(RReadStream& aStream);

        // Compiler-generated destructor is ok for this class.

    public:  // New functions
        /**
         * Pushes a cleanup item which will call ResetAndDestroy() for this
         * array when PopAndDestroy()'ed.
         */
        IMPORT_C void ResetAndDestroyPushL();

        /**
         * Externalizes this object to aStream.
         */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

        /**
         * Internalizes this object from aStream.
         */
        IMPORT_C void InternalizeL(RReadStream& aStream);

    public:  // from MPbkFieldDataArray
        TInt PbkFieldCount() const;
        MPbkFieldData& PbkFieldAt(TInt aIndex);
        const MPbkFieldData& PbkFieldAt(TInt aIndex) const;

    private:  // Implementation
        CPbkFieldDataArray();
        static void CleanupResetAndDestroy(TAny* aSelf);
    };


// INLINE FUNCTIONS
inline TStorageType CPbkFieldData::Type() const
    {
    return iType;
    }

inline TPbkFieldId CPbkFieldData::FieldId() const
    {
    return iFieldId;
    }

#endif  // __CPbkFieldData_H__

// End of File
