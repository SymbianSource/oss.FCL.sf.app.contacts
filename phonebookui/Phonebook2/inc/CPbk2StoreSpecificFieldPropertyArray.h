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
* Description:  Phonebook 2 store specific field property array.
*
*/


#ifndef CPBK2STORESPECIFICFIELDPROPERTYARRAY_H
#define CPBK2STORESPECIFICFIELDPROPERTYARRAY_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2FieldPropertyArray.h>
#include <MPbk2FieldPropertyArray2.h>

// FORWARD DECLARATIONS
class CPbk2StorePropertyArray;
class MVPbkContactStore;
class TVPbkContactStoreUriPtr;
class MVPbkFieldTypeList;

// CLASS DECLARATION

/**
 * Phonebook 2 store specific field property array.
 * Overloads properties with store specific ones.
 */
class CPbk2StoreSpecificFieldPropertyArray : public CBase,
                                             public MPbk2FieldPropertyArray,
                                             public MPbk2FieldPropertyArray2
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aFieldProperties  The base properties that are
         *                          overwritten by store specific properties.
         * @param aStoreProperties  Store properties for getting store
         *                          property and field property information.
         * @param aFieldTypes       The field types that are used to
         *                          build the properties.
         * @param aStore            The store whose properties are build.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreSpecificFieldPropertyArray* NewL(
                MPbk2FieldPropertyArray& aFieldProperties,
                CPbk2StorePropertyArray& aStoreProperties,
                const MVPbkFieldTypeList& aFieldTypes,
                MVPbkContactStore& aStore );

        /**
         * Destructor.
         */
        virtual ~CPbk2StoreSpecificFieldPropertyArray();

    public: // From MPbk2FieldPropertyArray
        const MPbk2FieldProperty* FindProperty(
                const MVPbkFieldType& aFieldType ) const;
        TInt Count() const;
        const MPbk2FieldProperty& At(
                TInt aIndex ) const;
        TAny* FieldPropertyArrayExtension( TUid aExtensionUid );
    
    public: // From MPbk2FieldPropertyArray2            
       const MPbk2FieldProperty* FindProperty(
                    const MVPbkFieldType& aFieldType,
                    const TDesC& aName ) const;

    private: // Implementation
        CPbk2StoreSpecificFieldPropertyArray(
                MPbk2FieldPropertyArray& aFieldProperties );
        void ConstructL(
                CPbk2StorePropertyArray& aStoreProperties,
                const MVPbkFieldTypeList& aFieldTypes,
                const TVPbkContactStoreUriPtr& aStoreUri );

    private: // Data
        /// Ref: Base field properties
        MPbk2FieldPropertyArray& iFieldProperties;
        /// Own: Store specific field properties
        MPbk2FieldPropertyArray* iSpecificFieldProperties;
    };

#endif // CPBK2STORESPECIFICFIELDPROPERTYARRAY_H

// End of File
