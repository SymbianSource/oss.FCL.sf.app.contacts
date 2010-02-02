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
*      Phonebook field type signature.
*
*/


#ifndef __TPbkContactItemFieldType_H__
#define __TPbkContactItemFieldType_H__

// INCLUDES
#include <cntdef.h> // TStorageType
#include "TPbkFieldTypeSignature.h"

// FORWARD DECLARATIONS
class CPbkFieldInfo;
class CContactItemField;
class CContentType;
class MPbkVcardProperty;


// CLASS DECLARATION

/**
 * Phonebook field type from CContactItemField.
 */
NONSHARABLE_CLASS(TPbkContactItemFieldType)
    {
    public: // Constructors
        /**
         * Default constructor.
         */
        TPbkContactItemFieldType();

        /**
         * Constructor. Used when the reference types are created in 
         * CPbkFieldInfo instances.
		 * @param aFieldInfo reference to field info
		 * @param aVcardProperty reference to vCard property
		 * @param aUidMap reference to UID map
         */
        TPbkContactItemFieldType
            (const CPbkFieldInfo& aFieldInfo, 
            const MPbkVcardProperty& aVcardProperty,
            const CPbkUidMap& aUidMap);

        /**
         * Constructor. Used when the reference types are created in 
         * CPbkFieldInfo instances with no import properties.
		 * @param aFieldInfo reference to field info
		 * @param aUidMap reference to UID map
         */
        TPbkContactItemFieldType
            (const CPbkFieldInfo& aFieldInfo, 
            const CPbkUidMap& aUidMap);

        /**
         * Constructor. Used when matching CContactItemField objects to 
         * CPbkFieldInfo instances.
		 * @param aField reference to contact item field
		 * @param aUidMap reference to UID map
         */
        TPbkContactItemFieldType
            (const CContactItemField& aField,
			const CPbkUidMap& aUidMap);

        // Default copy constructor, assignment and destructor are ok for
        // this class.

    public:  // New functions
        /**
         * Returns ETrue if this field type matches aFieldType.
		 * @param aFieldType contact item field type
		 * @return ETrue if types match
         */
        TBool Match(const TPbkContactItemFieldType& aFieldType) const;

        /**
         * Returns the signature of this field type.
		 * @return the signature
         */
        const TPbkFieldTypeSignature& Signature() const;

    private:  // Implementation
        void AddStorageType(TStorageType aStorageType, const CPbkUidMap& aUidMap);
        void AddContentType(const CContentType& aContentType, const CPbkUidMap& aUidMap);

    private:  // Data
		/// Own: field type signature
        TPbkFieldTypeSignature iTypeSignature;
    };


// INLINE FUNCTIONS

inline TPbkContactItemFieldType::TPbkContactItemFieldType()
    {
    }

inline TBool TPbkContactItemFieldType::Match
        (const TPbkContactItemFieldType& aFieldType) const
    {
    return iTypeSignature.IsProperSubsetOf(aFieldType.iTypeSignature);
    }

inline const TPbkFieldTypeSignature& TPbkContactItemFieldType::Signature() const
    {
    return iTypeSignature;
    }

#endif // __TPbkContactItemFieldType_H__

// End of File
