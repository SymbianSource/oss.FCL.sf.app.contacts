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
*      Phonebook field import type.
*
*/


#ifndef __CPBKFIELDIMPORTTYPE_H__
#define __CPBKFIELDIMPORTTYPE_H__

// INCLUDES
#include "CPbkVcardParameters.h"
#include "TPbkContactItemFieldType.h"

// FORWARD DECLARATIONS
class TPbkVcardFieldType;
class TPbkMatchPriorityLevel;


// CLASS DECLARATION

/**
 * Phonebook field import type.
 */
NONSHARABLE_CLASS(CPbkFieldImportType) :
        public CBase
    {
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aResReader resource reader associated with resource file
         */
        static CPbkFieldImportType* NewL(TResourceReader& aResReader);

        /**
         * Destructor.
         */
        ~CPbkFieldImportType();

    public:  // New functions
        /**
         * Returns ETrue if this import type matches aFieldType at any import type
         * priority level.
         *
         * @precond CalculateSignatures() has been called for this object.
         */
        TBool Match(const TPbkContactItemFieldType& aFieldType) const;

        /**
         * Returns ETrue if this import type matches aFieldType at import type
         * priority level defined by aMatchPriority.
         *
         * @precond CalculateSignatures() has been called for this object.
         */
        TBool Match
            (const TPbkContactItemFieldType& aFieldType,
            const TPbkMatchPriorityLevel& aMatchPriority) const;

        /**
         * Returns ETrue if this import type matches aVcardType at import type
         * priority level defined by aMatchPriority.
         *
         * @precond CalculateSignatures() has been called for this object.
         */
        TBool Match
            (const TPbkVcardFieldType& aVcardType,
            const TPbkMatchPriorityLevel& aMatchPriority) const;

        /**
         * Returns the number of import priority levels.
         */
        TInt ImportPropertyCount() const;

        /**
         * Adds all type UIDs in this import type to aTypeUidMap.
         */
        void UpdateTypeUidMapL(CPbkUidMap& aTypeUidMap) const;

        /**
         * Calculates the type signatures using aUidMap. Needs to done before
         * Match() -functions are called.
         *
         * @param aFieldInfo    field info object that contains this import 
         *                      type.
         * @param aUidMap   an initialised UID map to use when calculating the
         *                  signatures.
         */
        void CalculateSignatures
            (const CPbkFieldInfo& aFieldInfo, const CPbkUidMap& aUidMap);

    private:  // Implementation
        CPbkFieldImportType();
        void ConstructL(TResourceReader& aResReader);
        TBool ContainsExcludeTypes(const TPbkFieldTypeSignature& aSignature) const;

    private:  // Data
        /// Own: import type based on the database type
        TPbkContactItemFieldType iContactItemExportType;
        struct TImportProperty;
        /// Own: vCard and DB import properties array
        RArray<TImportProperty> iImportProperties;  // STRUCT   ImportProperties 
        /// Own: vCard exclude parameters
        CPbkVcardParameters iExcludeParameters;  // STRUCT   ExcludeParameters
        /// Own: Signature of exclude parameters
        TPbkFieldTypeSignature iExcludeSignature;
    };


#endif // __CPBKFIELDIMPORTTYPE_H__

// End of File
