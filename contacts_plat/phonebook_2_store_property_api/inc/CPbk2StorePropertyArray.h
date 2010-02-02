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
* Description:  Phonebook 2 contact store property array.
*
*/


#ifndef CPBK2STOREPROPERTYARRAY_H
#define CPBK2STOREPROPERTYARRAY_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class TResourceReader;
class CPbk2StoreProperty;
class TVPbkContactStoreUriPtr;

// CLASS DECLARATION

/**
 * Phonebook 2 contact store property array.
 * Collection of Phonebook 2 store property objects.
 */
class CPbk2StorePropertyArray : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aReader   Resource reader pointed to a
         *                  PHONEBOOK2_STORE_PROPERTY_ARRAY resource.
         *                  @see PHONEBOOK2_STORE_PROPERTY_ARRAY
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StorePropertyArray* NewL(
                TResourceReader& aReader );

        /**
         * Creates a new instance of this class.
         * Constructs the core Phonebook 2 store property array.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StorePropertyArray* NewL();

        /**
         * Destructor
         */
        ~CPbk2StorePropertyArray();

    public: // Interface

        /**
         * Returns the number of properties in the array.
         *
         * @return  Number of properties in the array.
         */
        IMPORT_C TInt Count() const;

        /**
         * Returns the property at a given position.
         *
         * @param aIndex    The position to investigate.
         * @return  Property at a given position.
         */
        IMPORT_C const CPbk2StoreProperty& At(
                TInt aIndex ) const;

        /**
         * Finds a property from the array.
         *
         * @param aUriPtr   The URI of the store to investigate.
         * @return  Store property for the URI or NULL.
         */
        IMPORT_C const CPbk2StoreProperty* FindProperty(
                const TVPbkContactStoreUriPtr& aUriPtr ) const;

        /**
         * Appends new store properties from the resources.
         * If this array already contains a property for the URI
         * the old one is removed.
         *
         * @param aReader   Resource reader pointed to
         *                  a PHONEBOOK2_STORE_PROPERTY_ARRAY resource.
         */
        IMPORT_C void AppendFromResourceL(
                TResourceReader& aReader );

        /**
         * Appends new store properties from an existing array
         * If this array already contains a property for the URI
         * the old one is removed.
         *
         * @param aPropertyArray    A property array that is copied
         *                          into this property array.
         */
        IMPORT_C void AppendFromArrayL(
                CPbk2StorePropertyArray& aPropertyArray );

        /**
         * Appends a new property to the array.
         *
         * @param aProperty     The new property to append.
         *                      Ownership changes. Client must
         *                      release the ownership after the call.
         */
        IMPORT_C void AppendL(
                CPbk2StoreProperty* aProperty );

        /**
         * Deletes a property from the array. Safe to call even if there
         * is no property for the URI in the array.
         *
         * @param aUriPtr       The URI of the store that identifies
         *                      the property to be removed.
         */
        IMPORT_C void DeleteProperty(
                const TVPbkContactStoreUriPtr& aUriPtr );

    private: // Implementation
        CPbk2StorePropertyArray();
        void ConstructL();
        void ConstructFromResourceL(
                TResourceReader& aReader );
        void RemoveDuplicates(
                CPbk2StoreProperty& aProperty );
        TInt FindPropertyIndex(
                const TVPbkContactStoreUriPtr& aUriPtr );

    private: // Data
        /// Own: Store properties in an array
        RPointerArray<CPbk2StoreProperty> iStoreProperties;
    };

#endif // CPBK2STOREPROPERTYARRAY_H

// End of File
