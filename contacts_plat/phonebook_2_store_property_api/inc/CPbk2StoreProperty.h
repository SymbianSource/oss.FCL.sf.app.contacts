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
* Description:  Phonebook 2 contact store property.
*
*/


#ifndef CPBK2STOREPROPERTY_H
#define CPBK2STOREPROPERTY_H

// INCLUDE FILES
#include <e32base.h>
#include <TVPbkContactStoreUriPtr.h>
#include <Pbk2ContentIdentifiers.hrh>
#include <Pbk2ContactView.hrh>

// FORWARD DECLARATIONS
class TResourceReader;
class CPbk2StoreViewDefinition;
class MPbk2FieldPropertyArray;
class MVPbkFieldTypeList;
class CPbk2Content;
class CPbk2LocalizedText;

// CLASS DECLARATION

/**
 * Phonebook 2 contact store property.
 * Responsible for presenting contact store related information.
 */
class CPbk2StoreProperty : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreProperty* NewL();

        /**
         * Creates a new instance of this class.
         *
         * @param aReader   Resource reader pointed to a
         *                  PHONEBOOK2_STORE_PROPERTY resource.
         *                  @see PHONEBOOK2_STORE_PROPERTY
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreProperty* NewLC(
                TResourceReader& aReader );

        /**
         * Creates a new instance of this class.
         *
         * @param aProperty     An existing store property.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreProperty* NewLC(
                const CPbk2StoreProperty& aProperty );

        /**
         * Destructor
         */
        ~CPbk2StoreProperty();

    public: // Interface

        /**
         * Returns the store URI of the property.
         *
         * @return  Store URI of the property.
         */
        IMPORT_C TVPbkContactStoreUriPtr StoreUri() const;

        /**
         * Returns the name of the store.
         *
         * @return  Name of the store or KNullDesC if not set.
         */
        IMPORT_C const TDesC& StoreName() const;

        /**
         * Returns a set of info types that define what information
         * is requested from the store's MVPbkContactStoreInfo interface.
         *
         * @return  Memory info types.
         *          @see Pbk2StoreProperty.hrh
         */
        IMPORT_C TUint32 MemoryInfoTypes() const;

        /**
         * Returns an array of view definitions for the store.
         *
         * @return  Array of view definitions.
         */
        IMPORT_C TArray<const CPbk2StoreViewDefinition*> Views() const;

        /**
         * Returns store property flags.
         *
         * @return  Store property flags.
         *          @see Pbk2StoreProperty.hrh
         */
        IMPORT_C TUint32 Flags() const;

        /**
         * Returns the resource id of the store's field property array.
         *
         * @see PHONEBOOK2_FIELD_PROPERTY_ARRAY
         * @return  Resource id of the store's field properties.
         */
        IMPORT_C TInt FieldPropertiesResId() const;

        /**
         * Sets the store URI for the property.
         *
         * @param aStoreUri     The URI that is used to load the store from
         *                      the Virtual Phonebook.
         */
        IMPORT_C void SetStoreUriL(
                const TDesC& aStoreUri );

        /**
         * Sets the name for the store. The name is used in the UI.
         *
         * @param aStoreName    Name for the store.
         */
        IMPORT_C void SetStoreNameL(
                const TDesC& aStoreName );

        /**
         * Appends a new view definition.
         *
         * @param aViewDefinition   View definition to append.
         *                          Ownership changes. Client must
         *                          release the ownership after the call.
         */
        IMPORT_C void AppendViewDefinitionL(
                CPbk2StoreViewDefinition* aViewDefinition );

        /**
         * Sets the info types that define which data is requested
         * from the MVPbkContactStoreInfo interface.
         *
         * @param aInfoTypes    Memory info types.
         *                      @see  Pbk2StoreProperty.hrh
         */
        IMPORT_C void SetMemoryInfoTypes(
                TUint32 aInfoTypes );

        /**
         * Sets the store property flags. Overwrites existing flags.
         *
         * @param aFlags    Store property flags to set.
         *                  @see Pbk2StoreProperty.hrh
         */
        IMPORT_C void SetFlags(
                TUint32 aFlags );

        /**
         * Sets the store specific field property array resource id.
         *
         * @param aFieldPropertiesResId     Resource id of a
         *                                  PHONEBOOK2_FIELD_PROPERTY_ARRAY
         *                                  resource.
         */
        IMPORT_C void SetFieldPropertiesResId(
                TInt aFieldPropertiesResId );

        /**
         * Finds the view definitions from the property.
         *
         * @param aViewType     View type.
         *                      @see TPbk2ContactViewType
         * @return  Phonebook 2 store view definition or NULL if not found.
         *          Ownership is not given.
         */
        IMPORT_C const CPbk2StoreViewDefinition* FindView(
                TPbk2ContactViewType aViewType ) const;

        /**
         * Returns the store specific field properties. The store property
         * contains a link to the field property array. If the field property
         * resource has been defined then those properties are returned. The
         * related field types must be given to create properties because
         * each property links to one single field type.
         *
         * @param aFieldTypeList    In most cases this is the Virtual
         *                          Phonebook master field type list
         *                          because it contains all the field types.
         * @return  A field property array or NULL if there was no
         *          field property array definition for the store.
         */
        IMPORT_C MPbk2FieldPropertyArray* CreateStoreSpecificFieldPropertiesL(
                const MVPbkFieldTypeList& aFieldTypeList ) const;

        /**
         * Returns a content object that is mapped to the requested context.
         * Ownership of the returned object is passed. The client is able to
         * acquire content to a certain context using this method. The client
         * can pass information of the context to the content generation
         * implementation. The client is then responsible for finding the
         * contents type dynamically.
         *
         * @param aContext      Identifier of the context of the query.
         * @param aContextInfo  Information of the context the query
         *                      was made from. Can be NULL.
         * @return  The context object.
         */
        IMPORT_C CPbk2Content* RetrieveContentLC(
                TPbk2ContentId aContext ) const;

    private: // Implementation
        CPbk2StoreProperty();
        void ConstructL(
                TResourceReader& aReader );
        void ConstructL(
                const CPbk2StoreProperty& aProperty );
        void ConstructFieldPropertiesL(
                TInt aFieldPropertyArrayResId );
        void CreateLocalizedTextsL(
                const CPbk2StoreProperty& aProperty );

    private: // Data
        /// Own: Store URI
        HBufC* iStoreUri;
        /// Own: Store name
        HBufC* iStoreName;
        /// Own: Flags
        TUint32 iFlags;
        /// Own: Store views
        RPointerArray<const CPbk2StoreViewDefinition> iStoreViews;
        /// Own: Memory info types
        TUint32 iMemoryInfoTypes;
        /// Own: Store field properties resource id
        TInt iFieldPropertiesResId;
        /// Own: Array of context sensitive localized strings
        RPointerArray<const CPbk2LocalizedText> iLocalizedTexts;
    };

#endif // CPBK2STOREPROPERTY_H

// End of File
