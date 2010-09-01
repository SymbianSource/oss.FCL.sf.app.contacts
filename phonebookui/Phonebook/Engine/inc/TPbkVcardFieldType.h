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
*      Declaration of TPbkVcardFieldType class
*
*/


#ifndef __TPbkVcardFieldType_H__
#define __TPbkVcardFieldType_H__

// INCLUDES
#include <e32def.h>
#include "TPbkFieldTypeSignature.h"

// FORWARD DECLARATIONS
class MPbkVcardProperty;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook field type signature.
 */
NONSHARABLE_CLASS(TPbkVcardFieldType)
    {
    public: // Constructors
        /**
         * Default constructor.
         */
        TPbkVcardFieldType();

        /**
         * Constructor. Initializes this type to match aVcardProperty.
		 * @param aVcardProperty vCard property
		 * @param aUidMap UID map
         */
        TPbkVcardFieldType
            (const MPbkVcardProperty& aVcardProperty, const CPbkUidMap& aUidMap);

        // Default copy constructor, assignment and destructor are ok for
        // this class.

    public:  // New functions
        /**
         * Returns ETrue if this vCard field type matches aFieldType.
		 * @param aFieldType vCard field type
		 * @return ETrue if field types match
         */
        TBool Match(const TPbkVcardFieldType& aFieldType) const;

        /**
         * Returns signature.
		 * @return the signature
		 */
        const TPbkFieldTypeSignature& Signature() const;

    private:  // Data
		/// Own: field type signature
        TPbkFieldTypeSignature iTypeSignature;
    };


// INLINE FUNCTIONS

inline TPbkVcardFieldType::TPbkVcardFieldType()
    {
    }

inline TBool TPbkVcardFieldType::Match
        (const TPbkVcardFieldType& aFieldType) const
    {
    return iTypeSignature.IsProperSubsetOf(aFieldType.iTypeSignature);
    }

inline const TPbkFieldTypeSignature& TPbkVcardFieldType::Signature() const
    {
    return iTypeSignature;
    }


#endif // __TPbkVcardFieldType_H__

// End of File
