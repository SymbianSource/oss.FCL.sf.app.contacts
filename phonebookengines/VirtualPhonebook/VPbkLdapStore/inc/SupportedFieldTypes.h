/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSupportedFieldTypes - LDAP supported fields.
*
*/


#ifndef __SUPPORTEDFIELDTYPES_H__
#define __SUPPORTEDFIELDTYPES_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mvpbkfieldtype.h>

// FORWARD DECLARATIONS
class TLDAPFieldType;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CFieldTypeMappings;

// CLASS DECLARATION

/**
*  CSupportedFieldTypes - ldap supported fields.
*  
*/
class CSupportedFieldTypes : public CBase, public MVPbkFieldTypeList
    {
    public:     // CSupportedFieldTypes public constructors and destructor
        /**
        @function   NewL
        @discussion Create CSupportedFieldTypes object
        @param      aFields Field types array
        @param      aFieldTypeMappings Field type mappings
        @return     Pointer to instance of CSupportedFieldTypes
        */
        static CSupportedFieldTypes* NewL(RArray<TLDAPFieldType>& aFields,
                            const CFieldTypeMappings& aFieldTypeMappings);

        /**
        @function   NewLC
        @discussion Create CSupportedFieldTypes object
        @param      aFields Field types array
        @param      aFieldTypeMappings Field type mappings
        @return     Pointer to instance of CSupportedFieldTypes
        */
        static CSupportedFieldTypes* NewLC(RArray<TLDAPFieldType>& aFields,
                            const CFieldTypeMappings& aFieldTypeMappings);

        /**
        @function  ~CSupportedFieldTypes
        @discussion CSupportedFieldTypes destructor
        */
        ~CSupportedFieldTypes();

    public:     // Methods from MVPbkFieldTypeList

        /**
        @function   FieldTypeCount
        @discussion Returns the number of field types in this list.
        @return     The number of field types
        */
        TInt FieldTypeCount() const;

        /**
        @function   FieldTypeAt
        @discussion Returns the field type at index.
        @param      aIndex Index to get field type.
        @return     Field type at index
        */
        const MVPbkFieldType& FieldTypeAt(TInt aIndex) const;

        /**
        @function   FieldTypeAt
        @discussion Returns true if this list contains fied type.
        @param      aFieldType  Field type
        @return     True, list contains type.
        */
        TBool ContainsSame(const MVPbkFieldType& aFieldType) const;

        /**
        @function   FieldTypeAt
        @discussion Returns the maximum matching priority held by this field type list.
        @return     The maximum matching priority.
        */
        TInt MaxMatchPriority() const;

        /**
        @function   FindMatch
        @discussion Returns the field type at index.
        @param      aMatchProperty Property to match
        @param      aMatchPriority Priority
        @return     Matching field type.
        */
        const MVPbkFieldType* FindMatch(
            const TVPbkFieldVersitProperty& aMatchProperty,
            TInt aMatchPriority) const;

        /**
        @function   FindMatch
        @discussion Returns the field type at index.
        @param      aNonVersitType Non versit type
        @return     Matching field type.
        */
        const MVPbkFieldType* FindMatch(
            TVPbkNonVersitFieldType aNonVersitType) const;

    private:    // CSupportedFieldTypes private constructors

        /**
        @function   CSupportedFieldTypes
        @discussion CSupportedFieldTypes default contructor
        */
        CSupportedFieldTypes();

        /**
        @function   ConstructL
        @param      aFields Field types array
        @param      aFieldTypeMappings Field type mappings
        @discussion Perform the second stage construction of CSupportedFieldTypes
        */
        void ConstructL(RArray<TLDAPFieldType>& aFields,
            const CFieldTypeMappings& aFieldTypeMappings);

    private:    // CSupportedFieldTypes private member variables

        /// Supported types array. Doesn't own types
        RPointerArray<const MVPbkFieldType> iSupportedTypes;

    };

} // End of namespace LDAPStore
#endif // __SUPPORTEDFIELDTYPES_H__
