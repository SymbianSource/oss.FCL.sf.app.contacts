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
* Description:  A vpbk field type list that contains sim supported types
*
*/



#ifndef VPBKSIMSTORE_CSUPPORTEDFIELDTYPES_H
#define VPBKSIMSTORE_CSUPPORTEDFIELDTYPES_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkFieldType.h>

// FORWARD DECLARATIONS
struct TVPbkGsmStoreProperty;
struct TVPbkUSimStoreProperty;  

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CFieldTypeMappings;

// CLASS DECLARATION

/**
*  A vpbk field type list that contains sim supported types
*
*/
NONSHARABLE_CLASS( CSupportedFieldTypes ): 
        public CBase,
        public MVPbkFieldTypeList
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aSimStoreProperty sim store properties
        * @return a new instance of this class
        */
        static CSupportedFieldTypes* NewL(
            const CFieldTypeMappings& aFieldTypeMappings,
            TVPbkGsmStoreProperty& aSimStoreProperty );
        /**
        * Two-phased constructor.
        * @param aSimStoreProperty sim store properties
        * @return a new instance of this class
        */
        static CSupportedFieldTypes* NewL(
            const CFieldTypeMappings& aFieldTypeMappings,
            TVPbkGsmStoreProperty& aSimStoreProperty,
            TVPbkUSimStoreProperty& aUSimStoreProperty ); 
        
        /**
        * Destructor.
        */
        virtual ~CSupportedFieldTypes();

    public: // Functions from base classes

        /**
        * From MVPbkFieldTypeList
        */
        TInt FieldTypeCount() const;

        /**
        * From MVPbkFieldTypeList
        */
        const MVPbkFieldType& FieldTypeAt( TInt aIndex ) const;

        /**
        * From MVPbkFieldTypeList
        */
        TBool ContainsSame( const MVPbkFieldType& aFieldType ) const;

        /**
        * From MVPbkFieldTypeList
        */
        TInt MaxMatchPriority() const;

        /**
        * From MVPbkFieldTypeList
        */
        const MVPbkFieldType* FindMatch
            ( const TVPbkFieldVersitProperty& aMatchProperty,
              TInt aMatchPriority ) const;

        /**
        * From MVPbkFieldTypeList
        */
        const MVPbkFieldType* FindMatch
            ( TVPbkNonVersitFieldType aNonVersitType ) const;

        /**
         * from MVPbkFieldTypeList
         */
        const MVPbkFieldType* Find(
                TInt aFieldTypeResId) const;
        
    private:
        /**
        * C++ default constructor.
        */
        CSupportedFieldTypes();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CFieldTypeMappings& aFieldTypeMappings,
            TVPbkGsmStoreProperty& aSimStoreProperty,
            TVPbkUSimStoreProperty* aUSimStoreProperty = NULL ); 

    private:    // Data
        ///Ref: An array for the supported types. Doesn't own types
        RPointerArray<const MVPbkFieldType> iSupportedTypes;

    };
} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CSUPPORTEDFIELDTYPES_H
            
// End of File
