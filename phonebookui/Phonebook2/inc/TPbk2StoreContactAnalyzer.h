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
* Description:  Phonebook 2 store contact analyzer.
*
*/


#ifndef TPBK2STORECONTACTANALYZER_H
#define TPBK2STORECONTACTANALYZER_H

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkStoreContact;
class MVPbkContactFieldSelector;
class MVPbkFieldTypeList;
class MVPbkBaseContactFieldCollection;
class MVPbkBaseContactField;
class CVPbkFieldTypeSelector;
class MVPbkFieldTypeSelector;
class MVPbkFieldType;

// CLASS DECLARATION

/**
 * Phonebook 2 store contact analyzer.
 */
class TPbk2StoreContactAnalyzer
    {
    public: // Constructors and destructor

        /**
         * Constructor.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aContact          Store contact.
         */
        IMPORT_C TPbk2StoreContactAnalyzer(
                const CVPbkContactManager& aContactManager,
                const MVPbkStoreContact* aContact );

    public: // Interface

        /**
         * Analyses if the contact has a field with type specified
         * in aResId definition.
         *
         * @param aResId        A VPBK_FIELD_TYPE_SELECTOR resource id.
         * @param aStartIndex   Starts looking up the specified field from
         *                      this field index.
         * @param aContact      The contact to analyze. If NULL the contact
         *                      got from the constructor is used.
         * @return  Field index if contact has field specified in aResId,
         *          otherwise KErrNotFound.
         */
        IMPORT_C TInt HasFieldL(
                TInt aResId,
                TInt aStartIndex = 0,
                const MVPbkStoreContact* aContact = NULL ) const;

        /**
         * Analyses if the contact supports field with type specified
         * in aResId definition.

         * @param aResId        A VPBK_FIELD_TYPE_SELECTOR resource id.
         * @param aContact      The contact to analyze. If NULL the
         *                      contact got from constructor is used.
         * @return  ETrue if the field type is supported by the contact,
         *          otherwise EFalse.
         */
        IMPORT_C TBool IsFieldTypeSupportedL(
                TInt aResId,
                const MVPbkStoreContact* aContact = NULL ) const;

        /**
         * Checks if the aFieldType is same as specified in aResId definition.
         * Returns ETrue is same, otherwise EFalse.
         *
         * @param aFieldType   The field type to check.
         * @param aResId       Selector's resource id.
         * @return  ETrue if the field type is included.
         */
        IMPORT_C TBool IsFieldTypeIncludedL(
                const MVPbkFieldType& aFieldType,
                TInt aResId ) const;

        /**
         * Checks if the aField's field type is same as specified in
         * aResId definition. Returns ETrue is same, otherwise EFalse.
         *
         * @param aField       The field whose type to check.
         * @param aResId       Selector's resource id.
         * @return  ETrue if the field type is included.
         */
        IMPORT_C TBool IsFieldTypeIncludedL(
                const MVPbkBaseContactField& aField,
                TInt aResId ) const;

    private: // Implementation
        TPbk2StoreContactAnalyzer();
        TInt IsFieldIncluded(
                const MVPbkBaseContactFieldCollection& aFields,
                TInt aStartIndex,
                const MVPbkContactFieldSelector& aFieldTypeSelector ) const;
        TBool IsFieldTypeIncluded(
                const MVPbkFieldTypeList& aFieldTypes,
                const MVPbkFieldTypeSelector& aFieldTypeSelector ) const;
        CVPbkFieldTypeSelector* CreateFieldTypeSelectorLC(
                TInt aResId ) const;

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        const CVPbkContactManager& iContactManager;
        /// Ref: The contact to analyze
        const MVPbkStoreContact* iContact;
    };

#endif // TPBK2STORECONTACTANALYZER_H

// End of File
