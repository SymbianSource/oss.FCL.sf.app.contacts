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
* Description:  Field property array for Phonebook 2 field types.
*
*/


#ifndef CPBK2FIELDPROPERTYARRAY_H
#define CPBK2FIELDPROPERTYARRAY_H

// INCLUDE FILES
#include <e32base.h>
#include <MPbk2FieldPropertyArray.h>
#include <MPbk2FieldPropertyArray2.h>

// FORWARD DECLARATIONS
class TResourceReader;
class MVPbkFieldTypeList;
class CPbk2FieldProperty;
class RFs;
class CPbk2FieldOrderingManager;
class CPbk2ServiceManager;

// CLASS DECLARATION

/**
 * Field property array for Phonebook 2 field types.
 */
class CPbk2FieldPropertyArray : public CBase,
                                public MPbk2FieldPropertyArray,
                                public MPbk2FieldPropertyArray2
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aSupportedFieldTypeList   A list of field types.
         *                                  If this is the master fieldtype
         *                                  list then all the properties
         *                                  are loaded, otherwise only those
         *                                  properties that have the field
         *                                  type in the list are loaded.
         * @param aRFs                      File system session for reading
         *                                  a resource file.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2FieldPropertyArray* NewL(
                const MVPbkFieldTypeList& aSupportedFieldTypeList,
                RFs& aRFs );

        /**
         * Creates a new instance of this class.
         *
         * @param aSupportedFieldTypeList   A list of field types.
         *                                  If this is the master fieldtype
         *                                  list then all the properties
         *                                  are loaded, otherwise only those
         *                                  properties that have the field
         *                                  type in the list are loaded.
         * @param aRFs                      File system session for reading
         *                                  a resource file.
         * @param aServMan                  Service manager.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2FieldPropertyArray* NewL(
                const MVPbkFieldTypeList& aSupportedFieldTypeList,
                RFs& aRFs, CPbk2ServiceManager* aServMan );

        /**
         * Creates a new instance of this class.
         *
         * @param aSupportedFieldTypeList   A list of field types.
         *                                  If this is the master fieldtype
         *                                  list then all the properties
         *                                  are loaded, otherwise only those
         *                                  properties that have the field
         *                                  type in the list are loaded.
         * @param aReader                   Resource reader pointed to
         *                                  PHONEBOOK2_FIELD_PROPERTY_ARRAY
         *                                  resource.
         * @param aRFs                      File system session for reading
         *                                  a resource file.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2FieldPropertyArray* NewL(
                const MVPbkFieldTypeList& aSupportedFieldTypeList,
                TResourceReader& aReader,
                RFs& aRFs );

        /**
         * Destructor.
         */
        ~CPbk2FieldPropertyArray();

    public: // From MPbk2FieldPropertyArray
        const MPbk2FieldProperty* FindProperty(
                const MVPbkFieldType& aFieldType ) const;
        TInt Count() const;
        const MPbk2FieldProperty& At(
                TInt aIndex ) const;
        TAny* FieldPropertyArrayExtension( TUid aExtensionUid );
                
    public: // From MPbk2FieldPropertyArray2
        const MPbk2FieldProperty* FindProperty(
                const MVPbkFieldType& aFieldType, const TDesC& aName ) const;


    private: // Implementation
        CPbk2FieldPropertyArray();
        void ConstructL(
                const MVPbkFieldTypeList& aSupportedFieldTypeList,
                RFs& aRFs, CPbk2ServiceManager* aServMan );
        void ReadFieldPropertiesL(
                TResourceReader& aReader,
                const MVPbkFieldTypeList& aSupportedFieldTypeList,
                CPbk2FieldOrderingManager& aOrderingManager);
        CPbk2FieldOrderingManager* CreateFieldOrderingsLC( RFs& aRFs );
        void MultiplyImppFieldPropertyL(CPbk2ServiceManager* aServMan, RFs& aRFs );
        
    private: // Data
        /// Own: Field properties
        RPointerArray<CPbk2FieldProperty> iFieldProperties;
    };

#endif // CPBK2FIELDPROPERTYARRAY_H

// End of File
